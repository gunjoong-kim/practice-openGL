#ifndef __CONTEXT_HPP__
#define __CONTEXT_HPP__

#include "common.hpp"
#include "shader.hpp"
#include "program.hpp"
#include "buffer.hpp"
#include "vertex_layout.hpp"

CLASS_PTR(Context)
class Context {
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
};

#endif // __CONTEXT_HPP__