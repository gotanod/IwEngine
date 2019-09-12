#pragma once

#include "iw/entity/ComponentManager.h"
#include <unordered_map>
#include <typeindex>
#include <assert.h>
#include <memory>

namespace IwEntity {
	iwu::ref<Component> ComponentManager::RegisterComponent(
		ComponentType type,
		size_t size)
	{
		iwu::ref<Component>& component = m_components[type];
		if (!component) {
			component = std::make_shared<Component>();
			component->Type = type.hash_code();
			component->Name = type.name();
			component->Size = size;
		}

		return component;
	}

	iwu::ref<ComponentData> ComponentManager::ReserveComponents(
		iwu::ref<const Entity2> entity)
	{
		ChunkList& list = FindOrCreateChunkList(entity->Archetype);
		return list.ReserveComponents(entity);
	}

	bool ComponentManager::DestroyComponents(
		iwu::ref<const Entity2> entity)
	{
		ChunkList* list = FindChunkList(entity->Archetype);
		if (list) {
			return list->FreeComponents(entity);
		}

		return false;
	}

	ChunkList* ComponentManager::FindChunkList(
		iwu::ref<const Archetype2> archetype)
	{
		auto itr = m_componentData.find(archetype->Hash);
		if (itr == m_componentData.end()) {
			return nullptr;
		}

		return &itr->second;
	}

	ChunkList& ComponentManager::FindOrCreateChunkList(
		iwu::ref<const Archetype2> archetype)
	{
		auto itr = m_componentData.find(archetype->Hash);
		if (itr == m_componentData.end()) {
			itr = m_componentData.emplace(archetype->Hash,
				ChunkList { 
					archetype, m_chunkSize 
				}).first;
		}

		return itr->second;
	}
}
