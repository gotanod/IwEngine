#include "iw/entity2/ComponentArray.h"

namespace IwEntity2 {
	ComponentArray::ComponentArray(
		size_t pageSize,
		size_t archetypeSize)
		: m_pool(pageSize, archetypeSize)
	{}

	void* ComponentArray::CreateComponents(
		Entity entity)
	{
		auto itr = m_entities.find(entity);
		if (itr == m_entities.end()) {
			void* components = m_pool.alloc();
			m_entities.emplace(entity, components);
			return components;
		}

		return nullptr;
	}

	void ComponentArray::DestroyComponents(
		Entity entity)
	{
		void* ptr = m_entities.at(entity);
		m_pool.free(ptr);
	}

	void* ComponentArray::GetComponents(
		Entity entity)
	{
		auto itr = m_entities.find(entity);
		if (itr != m_entities.end()) {
			return m_entities.at(entity);
		}

		return nullptr;
	}

	size_t ComponentArray::ArchetypeSize() const {
		return m_pool.item_size();
	}
}