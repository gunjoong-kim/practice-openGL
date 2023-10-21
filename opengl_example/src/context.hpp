#ifndef __CONTEXT_HPP__
#define __CONTEXT_HPP__

#include "common.hpp"
#include "shader.hpp"
#include "program.hpp"

CLASS_PTR(Context)
class Context {
public:
	static ContextUPtr Create();
	void Render();
private:
	Context() {}
	bool Init();
	ProgramUPtr m_program;
};

#endif // __CONTEXT_HPP__