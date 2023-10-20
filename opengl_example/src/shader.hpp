#ifndef __SHADER_HPP__
#define __SHADER_HPP__

#include "common.hpp"

CLASS_PTR(Shader);

class Shader {
public:
	static ShaderUPtr CreateFromFile(const std::string& filename,
	GLenum shaderType); // only unique pointer 오직 이 함수로만 인스턴스화 가능

	~Shader();
	uint32_t Get() const { return m_shader; }
private:
	Shader() {}
	bool LoadFile(const std::string& filename, GLenum shaderType);
	uint32_t m_shader { 0 }; //openGL을 이용해 얻을 수 있는 수 즉 마음대로 set하는 것 위험
};

#endif // __SHADER_HPP__