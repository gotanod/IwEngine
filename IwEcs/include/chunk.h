#pragma once

#include "iwecs.h"
#include "IwUtil/input_iterator.h"
#include <tuple>

template<std::size_t...>
struct add_all
	: std::integral_constant<std::size_t, 0>
{};

template<std::size_t n, std::size_t... ns>
struct add_all<n, ns...>
	: std::integral_constant<std::size_t, n + add_all<ns...>::value>
{};

namespace iwecs {
	template<typename... _components_t>
	class chunk {
	private:
		using streams_t = std::tuple<_components_t*...>;

		streams_t   m_streams;
		std::size_t m_size;
		std::size_t m_capacity;
		std::size_t m_count;

		template<std::size_t... _tuple_index>
		entity_component_data insert_into_streams(std::index_sequence<_tuple_index...>, _components_t&&... data) {
			int expanded[] = {
				(std::get<_tuple_index>(m_streams)[m_count] = data, 0)... 
			};

			void* v[] = {
				(void*)&std::get<_tuple_index>(m_streams)[m_count]...
			};

			return entity_component_data(sizeof...(_tuple_index), v);
		}

		entity_component_data insert_into_streams(_components_t&&... data) {
			return insert_into_streams(
				std::make_index_sequence<sizeof...(_components_t)>{}, 
				std::forward<_components_t>(data)...
			);
		}

		template<std::size_t... _tuple_index>
		void remove_from_streams(std::index_sequence<_tuple_index...>, std::size_t remove_index) {
			int expanded[] = {
				(std::get<_tuple_index>(m_streams)[remove_index] 
					= std::get<_tuple_index>(m_streams)[m_count - 1], 0)...
			};
		}

		void remove_from_streams(std::size_t remove_index) {
			remove_from_streams(
				std::make_index_sequence<sizeof...(_components_t)>{},
				remove_index
			);
		}
	public:
		constexpr std::size_t size_pre_entity = add_all<sizeof(_components_t)...>::value;

		chunk(std::size_t prefered_size) {
			size_t size_pre_entitiy = sizeof_sum<_components_t...>();
			m_size = prefered_size - prefered_size % size_pre_entitiy;
			m_capacity = m_size / size_pre_entitiy;
			m_streams = streams_t(new _components_t[m_capacity]...);
			m_count = 0;
		}

		entity_component_data insert(_components_t&&... components) {
			entity_component_data data;
			if (!is_full()) {
				data = insert_into_streams(std::forward<_components_t>(components)...);
				m_count++;
			}

			return data;
		}

		bool remove(std::size_t index) {
			if (index > 0 && index < m_count) {
				if (index < m_count - 1) {
					remove_from_streams(index);
				}
				
				m_count--;
				return true;
			}

			return false;
		}
		
		bool is_full() { 
			return m_count == m_capacity; 
		}
	};
}