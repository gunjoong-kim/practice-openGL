#include "Program.hpp"

ProgramUPtr Program::Create(const std::vector<ShaderPtr>& shaders)
{
	auto program = ProgramUPtr(new Program());
	if (!program->Link(shaders))
		return nullptr;
	return std::move(program);
}

bool Program::Link(const std::vector<ShaderPtr>& shaders)
{
	this->m_program = glCreateProgram();

	for (auto& shader : shaders)
		glAttachShader(this->m_program, shader->Get());
	glLinkProgram(this->m_program);

	int success = 0;
	glGetProgramiv(this->m_program, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[1024];
		glGetProgramInfoLog(this->m_program, 1024, nullptr, infoLog);
		SPDLOG_ERROR("failed to link program: {}", infoLog);
		return false;
	}
	return true;
}

Program::~Program()
{
	if (this->m_program) {
		glDeleteProgram(this->m_program);
	}
}

void Program::Use() const
{
	glUseProgram(this->m_program);
}

// program.cpp
void Program::SetUniform(const std::string& name, int value) const {
	auto loc = glGetUniformLocation(m_program, name.c_str());
	glUniform1i(loc, value);
}

void Program::SetUniform(const std::string& name,
	const glm::mat4& value) const {
	auto loc = glGetUniformLocation(m_program, name.c_str());
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}
