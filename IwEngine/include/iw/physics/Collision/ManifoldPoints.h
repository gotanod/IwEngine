#pragma once

#include "iw/physics/IwPhysics.h"

namespace iw {
namespace Physics {
	struct ManifoldPoints {
		glm::vec3 A;
		glm::vec3 B;
		glm::vec3 Normal;
		scalar PenetrationDepth;
		bool HasCollision;

		ManifoldPoints()
			: A(0.0f)
			, B(0.0f)
			, Normal(0.0f)
			, PenetrationDepth(0.0f)
			, HasCollision(false)
		{}

		ManifoldPoints(
			glm::vec2 a,
			glm::vec2 b
		)
			: A(a, 0.f)
			, B(b, 0.f)
			, HasCollision(true)
		{
			glm::vec2 ba = a - b;
			PenetrationDepth = glm::length(ba);
			if (PenetrationDepth > 0.00001f)
			{
				Normal = glm::vec3(ba / PenetrationDepth, 0.f);
			}
			else {
				Normal = glm::vec3(0, 1, 0);
				PenetrationDepth = 1;
			}
		}

		ManifoldPoints(
			glm::vec3 a,
			glm::vec3 b
		)
			: A(a)
			, B(b)
			, HasCollision(true)
		{
			glm::vec3 ba = a - b;
			PenetrationDepth = glm::length(ba);
			if (PenetrationDepth > 0.00001f)
			{
				Normal = ba / PenetrationDepth;
			}
			else {
				Normal = glm::vec3(0, 1, 0);
				PenetrationDepth = 1;
			}
		}

		ManifoldPoints(
			glm::vec3 a,
			glm::vec3 b,
			glm::vec3 n,
			float     d
		)
			: A(a)
			, B(b)
			, Normal(n)
			, PenetrationDepth(d)
			, HasCollision(true)
		{}

		ManifoldPoints(
			glm::vec2 a,
			glm::vec2 b,
			glm::vec2 n,
			float     d
		)
			: A(a, 0)
			, B(b, 0)
			, Normal(n, 0.0f)
			, PenetrationDepth(d)
			, HasCollision(true)
		{}

		void SwapPoints()
		{
			glm::vec3 t = A;
			A = B;
			B = t;
			Normal = -Normal;
		}
	};
}

	using namespace Physics;
}
