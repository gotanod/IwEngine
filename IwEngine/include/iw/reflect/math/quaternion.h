#pragma once

#include "iw/reflection/Reflect.h"
#include "C:\dev\IwEngine\IwEngine\include\iw/math/quaternion.h"

namespace iw {
namespace Reflect {
namespace detail {
	template<>
	const Class* GetClass(ClassTag<iw::math::quaternion>) {
		static Class c = Class("iw::math::quaternion", sizeof(iw::math::quaternion), 4);
		c.fields[0] = {"x", GetType(TypeTag<float>()), offsetof(iw::math::quaternion, x)};
		c.fields[1] = {"y", GetType(TypeTag<float>()), offsetof(iw::math::quaternion, y)};
		c.fields[2] = {"z", GetType(TypeTag<float>()), offsetof(iw::math::quaternion, z)};
		c.fields[3] = {"w", GetType(TypeTag<float>()), offsetof(iw::math::quaternion, w)};
		return &c;
	}
	template<size_t _s>
	const Class* GetClass(ClassTag<iw::math::quaternion[_s]>) {
		static Class c = Class("iw::math::quaternion""[]", sizeof(iw::math::quaternion), 4, _s);
		c.fields[0] = {"x", GetType(TypeTag<float>()), offsetof(iw::math::quaternion, x)};
		c.fields[1] = {"y", GetType(TypeTag<float>()), offsetof(iw::math::quaternion, y)};
		c.fields[2] = {"z", GetType(TypeTag<float>()), offsetof(iw::math::quaternion, z)};
		c.fields[3] = {"w", GetType(TypeTag<float>()), offsetof(iw::math::quaternion, w)};
		return &c;
	}
}
}
}