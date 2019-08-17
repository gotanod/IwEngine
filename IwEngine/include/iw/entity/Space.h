#pragma once

#include "IwEntity.h"
#include "View.h"
#include "EntityManager.h"
#include "ComponentArray.h"
#include "iw/util/set/sparse_set.h"
#include "iw/util/type/family.h"
#include <vector>

namespace IwEntity {
	class Space {
	private:
		std::vector<IComponentArray*> m_components;
		EntityArray m_entities;

	public:
		Entity CreateEntity() {
			return m_entities.CreateEntity();
		}

		void DestroyEntity(
			Entity entity)
		{
			Archetype old = m_entities.ArchetypeOf(entity);

			m_entities.DestroyEntity(entity);

			for (auto& set : m_components) {
				if (set->HasEntity(entity)) {
					set->DestroyComponent(entity);
				}
			}
		}

		void Sort() {
			for (auto& set : m_components) {
				if (set) {
					set->Sort(m_entities);
				}
			}
		}

		void Clear() {
			m_entities.Clear();
			for (auto& set : m_components) {
				set->Clear();
			}
		}

		template<
			typename _c,
			typename... _args_t>
		_c& CreateComponent(
			Entity entity,
			_args_t&& ... args)
		{
			Archetype oldArchetype = m_entities.ArchetypeOf(entity);
			Archetype& archetype   = m_entities.AssignComponent<_c>(entity);

			UpdateComponentData(entity, archetype, oldArchetype);

			return EnsureComponentData<_c>().CreateComponent(
					entity, archetype, std::forward<_args_t>(args)...);
		}

		template<
			typename _c>
		void DestroyComponent(
			Entity entity)
		{
			if (ComponentDataExists<_c>()) {
				Archetype oldArchetype = m_entities.ArchetypeOf(entity);
				Archetype& archetype
					= m_entities.UnassignComponent<_c>(entity);

				UpdateComponentData(entity, archetype, oldArchetype);

				GetComponentData<_c>().DestroyComponent(entity);
			}
		}

		template<
			typename _c>
		_c* GetComponent(
			Entity entity)
		{
			if (ComponentDataExists<_c>()) {
				return GetComponentData<_c>().GetComponent(entity);
			}
		}

		template<
			typename... _cs>
		View<_cs...> ViewComponents() {
			return { EnsureComponentData<_cs>()... };
		}

		template<
			typename _c>
		ComponentId RegisterComponent() {
			return ComponentFamily::type<_c>;
		}
	private:
		template<
			typename _c>
		ComponentArray<_c>& EnsureComponentData() {
			ComponentId id = ComponentFamily::type<_c>;
			if (id >= m_components.size()) {
				m_components.resize((size_t)id + 1);
			}

			IComponentArray*& cdata = m_components.at(id);
			if (!cdata) {
				cdata = new ComponentArray<_c>();
			}

			return *static_cast<ComponentArray<_c>*>(cdata);
		}

		void UpdateComponentData(
			Entity entity,
			Archetype archetype,
			Archetype oldArchetype)
		{
			size_t index = 0;
			for (Archetype a = 1; a <= oldArchetype; a = a << 1, index++) {
				if ((oldArchetype & a) == a) {
					m_components.at(index)->UpdateChunk(
						entity, archetype, oldArchetype);
				}
			}
		}

		template<
			typename _c>
		ComponentArray<_c>& GetComponentData() {
			return *static_cast<ComponentArray<_c>*>(
				m_components.at(ComponentFamily::type<_c>));
		}

		template<
			typename _c>
		bool ComponentDataExists() {
			return ComponentFamily::type<_c> < m_components.size();
		}
	};
}
