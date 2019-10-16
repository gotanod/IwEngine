#pragma once

#include "Core.h"
#include "Stack.h"
#include "System.h"
#include "InitOptions.h"
#include "Events/AllEvents.h"
#include "iw/entity/Space.h"

namespace IwEngine {
	class IWENGINE_API Layer {
	private:
		const char* m_name;
		Stack<ISystem*> m_systems;
	protected:
		IwEntity::Space&         Space;
		IwGraphics::RenderQueue& RenderQueue;

	public:
		Layer(
			IwEntity::Space& space,
			IwGraphics::RenderQueue& renderQueue,
			const char* name);

		virtual ~Layer();

		virtual int Initialize(
			InitOptions& options);


		virtual void Destroy();
		virtual void ImGui();
		virtual void PreUpdate();
		virtual void Update();
		virtual void PostUpdate();
		virtual void FixedUpdate();

		virtual bool On(WindowResizedEvent& event);
		virtual bool On(MouseWheelEvent&    event);
		virtual bool On(MouseMovedEvent&    event);
		virtual bool On(MouseButtonEvent&   event);
		virtual bool On(KeyEvent&           event);
		virtual bool On(KeyTypedEvent&      event);

		void UpdateSystems();

		inline const char* Name() {
			return m_name;
		}

		template<
			typename S,
			typename... Args>
		S* PushSystem(
			Args&&... args)
		{
			S* layer = new S(Space, RenderQueue, std::forward<Args>(args)...);
			m_systems.PushBack(layer);
			return layer;
		}

		template<
			typename S>
		void PopSystem(
			S* system)
		{
			m_systems.Pop(system);
		}
	};
}
