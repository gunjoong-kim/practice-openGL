#ifndef __PROGRAM_HPP__
#define __PROGRAM_HPP__

#include "common.hpp"
#include "shader.hpp"

CLASS_PTR(Program)

class Program {
public:
	static ProgramUPtr Create(
		const std::vector<ShaderPtr>& shaders);

	~Program();
	uint32_t Get() const { return m_program; }    
private:
	Program() {}
	bool Link(const std::vector<ShaderPtr>& shaders);
	uint32_t m_program { 0 };
};

#endif // __PROGRAM_HPP__