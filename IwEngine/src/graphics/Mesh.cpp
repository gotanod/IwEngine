#include "iw/graphics/Mesh.h"
#include "iw/graphics/IndexBuffer.h"
#include "iw/math/matrix4.h"
#include "gl/glew.h"

namespace IwGraphics {
	Mesh::Mesh(
		VertexArray* va,
		IndexBuffer* ib)
		: m_vertexArray(va)
		, m_indexBuffer(ib) 
	{}

	Mesh::Mesh(
		const Mesh& copy) noexcept
		: m_vertexArray(copy.m_vertexArray)
		, m_indexBuffer(copy.m_indexBuffer)
	{}

	Mesh::Mesh(
		Mesh&& copy) noexcept
		: m_vertexArray(copy.m_vertexArray)
		, m_indexBuffer(copy.m_indexBuffer)
	{}

	Mesh& Mesh::operator=(
		const Mesh& copy) noexcept
	{
		m_vertexArray = copy.m_vertexArray;
		m_indexBuffer = copy.m_indexBuffer;
		return *this;
	}

	Mesh& Mesh::operator=(
		Mesh&& copy) noexcept
	{
		m_vertexArray = copy.m_vertexArray;
		m_indexBuffer = copy.m_indexBuffer;
		copy.m_vertexArray = nullptr;
		copy.m_indexBuffer = nullptr;
		return *this;
	}

	void Mesh::Draw(
		const iwmath::vector3& position,
		const iwmath::quaternion& rotation) const
	{
		iwmath::matrix4 world = iwm::matrix4::identity;

		iwmath::matrix4 view = iwmath::matrix4::create_perspective_field_of_view(
			1.2f, 1.777f, .01f, 1000.0f);

		iwmath::matrix4 proj
			= iwmath::matrix4::create_from_quaternion(rotation)
			* iwmath::matrix4::create_translation(position);

		glUniformMatrix4fv(0, 1, GL_FALSE, world.elements);
		glUniformMatrix4fv(4, 1, GL_FALSE, view.elements);
		glUniformMatrix4fv(8, 1, GL_FALSE, proj.elements);

		m_vertexArray->Bind();
		m_indexBuffer->Bind();

		m_indexBuffer->Draw();
	}
}
