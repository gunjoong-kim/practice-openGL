#ifndef __CONTEXT_HPP__
#define __CONTEXT_HPP__

#include "Common.hpp"
#include "Shader.hpp"
#include "Program.hpp"
#include "Buffer.hpp"
#include "VertexLayout.hpp"
#include "Texture.hpp"

CLASS_PTR(Context)

class Context
{
public:
	static ContextUPtr Create();
	void Render();

private:
	Context() {}
	bool Init();
	ProgramUPtr m_program;
	VertexLayoutUPtr m_vertexLayout;
	BufferUPtr m_vertexBuffer;
	BufferUPtr m_indexBuffer;
	TextureUPtr m_texture;
	TextureUPtr m_texture2;

};

#endif // __CONTEXT_HPP__