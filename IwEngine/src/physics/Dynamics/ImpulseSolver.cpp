#include "iw/physics/Dynamics/ImpulseSolver.h"

namespace IW {
	void ImpulseSolver::Solve(
		std::vector<Rigidbody*>& bodies,
		std::vector<Manifold>& manifolds,
		scalar dt)
	{
		for (Manifold& manifold : manifolds) {
			Rigidbody* aBody = manifold.BodyA;
			Rigidbody* bBody = manifold.BodyB;

			iw::vector3 aVel = aBody->Velocity();
			iw::vector3 bVel = bBody->Velocity();
			iw::vector3 rVel = bVel - aVel;   
			scalar      nVel = rVel.dot(manifold.Normal);

			// Impluse

			if (nVel > 0)
				continue;

			// This goes in rigidbody
			scalar e = 0.5f;

			scalar j = -(1.0f + e) * nVel / (aBody->InvMass() + bBody->InvMass());

			iw::vector3 impluse = j * manifold.Normal;

			if (aBody->IsKinematic()) {
				aVel -= impluse * aBody->InvMass();
			}

			if (bBody->IsKinematic()) {
				bVel += impluse * bBody->InvMass();
			}

			// Friction

			rVel = bVel - aVel;
			nVel = rVel.dot(manifold.Normal);

			iw::vector3 tangent = (rVel - nVel * manifold.Normal).normalized();
			scalar      fVel = rVel.dot(tangent);

			// These go in rigidbody
			scalar aSF = 1.1f;
			scalar bSF = 1.1f;
			scalar aDF = .15f;
			scalar bDF = .15f;

			scalar mu = iw::vector2(aSF, bSF).length();

			scalar f  = -fVel / (aBody->InvMass() + bBody->InvMass());

			iw::vector3 friction;
			if (abs(f) < j * mu) {
				friction = f * tangent;
			}

			else {
				mu = iw::vector2(aDF, bDF).length();
				friction = -j * tangent * mu;
			}

			if (aBody->IsKinematic()) {
				aBody->SetVelocity(aVel - friction * aBody->InvMass());
			}

			if (bBody->IsKinematic()) {
				bBody->SetVelocity(bVel + friction * bBody->InvMass());
			}
		}
	}
}