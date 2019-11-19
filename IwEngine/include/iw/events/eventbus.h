#pragma once

#include "iwevents.h"
#include "event.h"
#include "callback.h"
#include "iw/util/queue/blocking_queue.h"
#include "iw/util/memory/linear_allocator.h"
#include "iw/log/logger.h"
#include <mutex>

namespace iw {
inline namespace events {
	class eventbus {
	private:
		std::vector<iw::callback<event&>> m_callbacks;
		iw::linear_allocator m_alloc;
		iw::blocking_queue<event*> m_events;
		std::mutex m_mutex;

	public:
		IWEVENTS_API eventbus();

		IWEVENTS_API void subscribe(
			const iw::callback<event&>& callback);

		IWEVENTS_API void unsubscribe(
			const iw::callback<event&>& callback);

		IWEVENTS_API void publish();

		template<
			typename _e>
		void push(
			const _e& event)
		{
			std::unique_lock<std::mutex> lock(m_mutex);

			_e* e = m_alloc.alloc<_e>();
			if (e == nullptr) {
				m_alloc.resize(m_alloc.capacity() * 2);
				e = m_alloc.alloc<_e>();
			}

			*e = event;

			m_events.push((iw::event*)e);
		}
	};
}
}