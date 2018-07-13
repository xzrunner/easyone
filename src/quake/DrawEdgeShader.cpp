#include "quake/DrawEdgeShader.h"

namespace eone
{
namespace quake
{

DrawEdgeShader::DrawEdgeShader(ur::RenderContext* rc, const char* vs, const char* fs,
	                           const std::vector<std::string>& textures, const CU_VEC<ur::VertexAttrib>& va_list)
	: ur::Shader(rc, vs, fs, textures, va_list)
{
}

void DrawEdgeShader::DrawBefore(const ur::TexturePtr& tex)
{
	float col[4] = { 1, 1, 1, 1 };
	SetVec4("u_edge_color", col);
}

}
}