#pragma once

#include "CollisionObject.h"
#include "Manifold.h"
#include <vector>

namespace IW {
namespace Physics {
	class Solver {
	public:
		virtual void Solve(
			std::vector<CollisionObject*>& objects,
			std::vector<Manifold>& manifolds,
			scalar dt = 0) {}
	};
}

	using namespace Physics;
}