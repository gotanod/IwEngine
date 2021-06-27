#include "iw/Sand/Tile.h"
#include "iw/common/algos/polygon2.h"
#include "iw/physics/AABB.h"

IW_PLUGIN_SAND_BEGIN

Tile::Tile(
	ref<Texture> texture,
	ref<Shader> shader)
{
	m_sprite = texture;

	if (!m_sprite->m_colors) {
		m_sprite->SetFilter(iw::NEAREST);
		m_sprite->CreateColors();
	}

	ref<Material> material = REF<Material>(shader);
	//material->SetTexture("texture", m_sprite);
	material->Set("color", Color::From255(200, 200, 100));
	material->SetWireframe(true);

	MeshDescription tileDesc;
	tileDesc.DescribeBuffer(bName::POSITION, MakeLayout<float>(2));
	
	m_spriteMesh = (new MeshData(tileDesc))->MakeInstance();
	m_spriteMesh.Material = material;
}

void Tile::UpdatePolygon()
{
	unsigned* colors = (unsigned*)m_sprite->Colors();
	glm::vec2 size   =            m_sprite->Dimensions();

	auto polygons = iw::MakePolygonFromField(colors, size.x, size.y, 1u);
	
	if (polygons.size() == 0) return;

	m_polygon = polygons[0];
	m_index   = iw::TriangulatePolygon(m_polygon);

	if (m_polygon.size() == 0) return;

	for (glm::vec2& v : m_polygon) {
		v = (v / size - glm::vec2(0.5f)) * size / 10.0f;
	}

	m_spriteMesh.Data->SetBufferData(bName::POSITION, m_polygon.size(), m_polygon.data());
	m_spriteMesh.Data->SetIndexData(                  m_index  .size(), m_index  .data());
}

IW_PLUGIN_SAND_END