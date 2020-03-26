#pragma once

#include "iw/physics/IwPhysics.h"
#include "Collider.h"

namespace iw {
namespace Physics {
namespace impl {
	template<
		typename V>
	struct REFLECT SphereCollider
		: Collider<V>
	{
		REFLECT V Center;
		REFLECT float Radius;

		SphereCollider(
			V center,
			float radius)
			: Collider<V>()
			, Center(center)
			, Radius(radius)
		{}

		ManifoldPoints TestCollision(
			const Transform* transform,
			const impl::Collider<V>* collider,
			const Transform* colliderTransform) const override
		{
			return collider->TestCollision(colliderTransform, this, transform);
		}

		IWPHYSICS_API
		ManifoldPoints TestCollision(
			const Transform* transform,
			const impl::SphereCollider<V>* sphere,
			const Transform* sphereTransform) const override;

		IWPHYSICS_API
		ManifoldPoints TestCollision(
			const Transform* transform,
			const impl::PlaneCollider<V>* plane,
			const Transform* planeTransform) const override;

		const AABB<V>& Bounds() override {
			if (m_outdated) {
				m_bounds = AABB<V>(Center, Radius);
				m_outdated = false;
			}

			return m_bounds;
		}
	};
}

	using SphereCollider2 = impl::SphereCollider<iw::vector2>;
	using SphereCollider  = impl::SphereCollider<iw::vector3>;
	using SphereCollider4 = impl::SphereCollider<iw::vector4>;
}

	using namespace Physics;
}
