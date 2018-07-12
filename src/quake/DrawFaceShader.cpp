#include "quake/DrawFaceShader.h"

namespace eone
{
namespace quake
{

DrawFaceShader::DrawFaceShader(ur::RenderContext* rc, const char* vs, const char* fs,
	                           const std::vector<std::string>& textures, const CU_VEC<ur::VertexAttrib>& va_list)
	: ur::Shader(rc, vs, fs, textures, va_list)
{
}

void DrawFaceShader::DrawBefore(const ur::TexturePtr& tex)
{
	SetInt("u_apply_texture", tex ? 1 : 0);
}

}
}
