#include "Shader.hpp"

ShaderUPtr Shader::CreateFromFile(
	const std::string& filename,
	GLenum shaderType
) {
	auto shader = ShaderUPtr(new Shader());
	if (!shader->LoadFile(filename, shaderType))
		return nullptr; // shader instance 자동해제
	return std::move(shader); // 성공했으면 move로 소유권 이전
}

bool Shader::LoadFile(
	const std::string& filename,
	GLenum shaderType
) {
	auto result = LoadTextFile(filename);
	if (!result.has_value())
		return false;

	auto& code = result.value();
	const char* codePtr = code.c_str();
	int32_t codeLength = (int32_t)code.length();

	// create and compile shader
	this->m_shader = glCreateShader(shaderType);
	glShaderSource(this->m_shader, 1, (const GLchar* const*)&codePtr, &codeLength);
	glCompileShader(this->m_shader);

	// check compile error
	int success = 0;
	glGetShaderiv(this->m_shader, GL_COMPILE_STATUS, &success); // shader GL_COMPILE_STATUS 정보 얻어오기
	if (!success) {
		char infoLog[1024];
		glGetShaderInfoLog(this->m_shader, 1024, nullptr, infoLog); // error 정보 얻어오기
		SPDLOG_ERROR("failed to compile shader: \"{}\"", filename);
		SPDLOG_ERROR("reason: {}", infoLog);
		return false;
	}
	return true;
}

Shader::~Shader()
{
	if (this->m_shader) {
		glDeleteShader(this->m_shader);
	}
}