#include "context.hpp"

ContextUPtr Context::Create() {
  auto context = ContextUPtr(new Context());
  if (!context->Init())
    return nullptr;
  return std::move(context);
}

bool Context::Init() {
	// shared pointer로 인자 받기
	ShaderPtr vertShader = Shader::CreateFromFile("./shader/simple.vs", GL_VERTEX_SHADER);
	ShaderPtr fragShader = Shader::CreateFromFile("./shader/simple.fs", GL_FRAGMENT_SHADER);
	if (!vertShader || !fragShader)
		return false;
	SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
	SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

	//{ } 이런 식으로 벡터 생성자 호출 가능
	m_program = Program::Create({fragShader, vertShader});
  	if (!m_program)
		return false;
	SPDLOG_INFO("program id: {}", m_program->Get());

	glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

	uint32_t vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	return true;
}

void Context::Render() {
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(m_program->Get());
	glDrawArrays(GL_POINTS, 0, 1);
}