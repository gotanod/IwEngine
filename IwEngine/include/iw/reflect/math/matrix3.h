#pragma once
#include "iw/reflection/Reflect.h"
#include "C:\dev\IwEngine\IwEngine\include\iw/math/matrix3.h"

namespace iw {
namespace detail {
	template<>
	const Class* GetClass(ClassTag<iw::math::matrix3>) {
		static Class c = Class("iw::math::matrix3", 36, 1);
		c.fields[0] = {"elements", GetType(TypeTag<float [9]>()), 0};
		return &c;
	}
	template<size_t _s>
	const Class* GetClass(ClassTag<iw::math::matrix3[_s]>) {
		static Class c = Class("iw::math::matrix3""[]", 36, 1, _s);
		c.fields[0] = {"elements", GetType(TypeTag<float [9]>()), 0};
		return &c;
	}
}
}
