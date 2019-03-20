#pragma once

#include "IwGraphics.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "iw/math/vector3.h"
#include "iw/math/quaternion.h"
#include <memory>

namespace IwGraphics {
	class IWGRAPHICS_API Mesh {
	private:
		std::shared_ptr<IwGraphics::VertexArray> m_vertexArray;
		std::shared_ptr<IwGraphics::IndexBuffer> m_indexBuffer;

	public:
		Mesh() = default;

		Mesh(
			IwGraphics::VertexArray* va,
			IwGraphics::IndexBuffer* ib);

		Mesh(
			const Mesh& copy) noexcept;

		Mesh(
			Mesh&& copy) noexcept;

		Mesh& operator=(
			const Mesh& copy) noexcept;

		Mesh& operator=(
			Mesh&& copy) noexcept;

		void Draw(
			const iwmath::vector3& position,
			const iwmath::quaternion& rotation) const;
	};
}
