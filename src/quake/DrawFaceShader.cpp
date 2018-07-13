#include "quake/DrawFaceShader.h"

#include <painting3/EffectsManager.h>

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
	float col[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
	SetVec4("u_face_color", col);

	SetFloat("u_brightness", 1.4f);

	SetInt("u_apply_texture", tex ? 1 : 0);
}

}
}
