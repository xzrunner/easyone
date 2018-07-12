#pragma once

#include <unirender/Shader.h>

namespace eone
{
namespace quake
{

class DrawFaceShader : public ur::Shader
{
public:
	DrawFaceShader(ur::RenderContext* rc, const char* vs, const char* fs,
		const std::vector<std::string>& textures, const CU_VEC<ur::VertexAttrib>& va_list);

	virtual void DrawBefore(const ur::TexturePtr& tex);

}; // DrawFaceShader

}
}
