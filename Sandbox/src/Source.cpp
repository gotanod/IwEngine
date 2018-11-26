#include <iostream>

#include "IwEngine/IwEngine.h"
#include "IwEcs/ecs_manager.h"

#include <vector>
#include <iostream>


class intfloat_system {
public:
	void update() {
		std::vector<int> ints = std::vector<int>();

		ints.push_back(1);
		ints.push_back(2);
		ints.push_back(3);

		std::vector<int>::iterator itr = ints.begin();
		std::vector<int>::iterator end = ints.end();

		for (; itr != end; itr++) {
			std::cout << *itr << std::endl;
		}
	}
};

class Game : public IwEngine::Application {
public:
	Game() {}
	~Game() {}

	void Run() override {
		iwecs::component_registry cr = iwecs::component_registry();
		for (int i = 0; i < 30; i++) {
			float f = i + 1.0f;
			cr.create_entity2<int, float>(i, f);
		}

		cr.destroy_entity(5);

		cr.create_entity<int, float>(3, 4.0f);

		intfloat_system ifs = intfloat_system();
		ifs.update();
	}
};

IwEngine::Application* CreateApplication() {
	return new Game();
}


//iwm	iwmath
//iwecs	iwecs
//iwevt	iwevents
//iwi	iwinput
//iwe	iwengine
//iwp	iwphysics
//iwa	iwaudio