#pragma once

#include "IwEntity.h"
#include "Component.h"

namespace IW {
namespace ECS {
	struct PrefabComponent {
	private:
		iw::ref<Component> Component;
	};

}
	using namespace ECS;
}
