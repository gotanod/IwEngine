#pragma once

#include "IwGraphics.h"
#include "Mesh.h"
#include "Vertex.h"
#include "QueuedDevice.h"
#include "iw/log/logger.h"
#include "iw/util/async/potential.h"
#include "iw/util/memory/linear_allocator.h"
#include "iw/renderer/VertexBufferLayout.h"
#include "iw/renderer/VertexBuffer.h"
#include "iw/renderer/VertexArray.h"
#include "iw/renderer/IndexBuffer.h"
#include "iw/renderer/Device.h"
#include <queue>

namespace IwGraphics {
	class IWGRAPHICS_API RenderQueue {
	private:
		enum Operation {
			DRAW
		};

		struct RenderOperation {
			Operation Operation;
			void*     Args;
			//RenderOp*   Next; //more complex ops probly need this
		};

		std::vector<RenderOperation> m_queue;
		iwu::linear_allocator        m_scratch;
	public:
		QueuedDevice QueuedDevice;

	public:
		RenderQueue(
			IwRenderer::IDevice& device);

		void Push(
			Operation&& op,
			void* data);

		void Execute();
	};
}