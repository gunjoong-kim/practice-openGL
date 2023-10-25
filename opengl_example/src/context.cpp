#include "Context.hpp"

ContextUPtr Context::Create()
{
  auto context = ContextUPtr(new Context());
  if (!context->Init())
    return nullptr;
  return std::move(context);
}

bool Context::Init()
{
	float vertices[] = {
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // top right, red
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right, green
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom left, blue
		-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, // top left, yellow
	};
	uint32_t indices[] = { // note that we start from 0!
		0, 1, 3, // first triangle
		1, 2, 3, // second triangle
	};

	// VBO 만들 기 전에 VAO 만들기, VAO binding 먼저
	m_vertexLayout = VertexLayout::Create();

	m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 24);
	// vertex buffer 생성함수
	// gl array buffer -> 위치값과 여러 정보들이 들어 있는 버퍼에 binding
	// 실제로 데이터를 복사하는 과정

	m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	// 0번 사용하기 -> vao attribute 0 -> location 0 in v shader
	// 각각 data float 3개 다음 데이터 float 6개 뒤 normalize 하지 말라 offset 0
	m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, sizeof(float) * 3);
	// 3번째부터 색깔 등장 6개씩 데이터 건너띄기
	
	m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t) * 6);

	// shared pointer로 인자 받기
	ShaderPtr vertShader = Shader::CreateFromFile("./shader/per_vertex_color.vs", GL_VERTEX_SHADER);
	ShaderPtr fragShader = Shader::CreateFromFile("./shader/per_vertex_color.fs", GL_FRAGMENT_SHADER);
	if (!vertShader || !fragShader)
		return false;
	SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
	SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

	//{ } 이런 식으로 벡터 생성자 호출 가능
	m_program = Program::Create({fragShader, vertShader});
  	if (!m_program)
		return false;
	SPDLOG_INFO("program id: {}", m_program->Get());

	// auto loc = glGetUniformLocation(m_program->Get(), "color");
	// m_program->Use();
	// glUniform4f(loc, 0.0f, 1.0f, 0.0f, 1.0f);

	glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
	return true;
}

void Context::Render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	m_program->Use();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
	// 지금 VBO, VAO, EBO에 binding 되어 있는 것들로 그림 그리기
	// offset 첫 정점 index
	// count 몇개 점을 그릴거냐
	// type index의 자료형
	// primitives -> 굉장히 다양한 타입의 그림그리는 형식이 존재함.

