#include "Context.hpp"
#include "Image.hpp"

ContextUPtr Context::Create()
{
	auto context = ContextUPtr(new Context());
	if (!context->Init())
    	return nullptr;
	return std::move(context);
}

bool Context::Init()
{
	// data -> x y z r g b s t
	float vertices[] = {
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	};
	uint32_t indices[] = { // note that we start from 0!
		0, 1, 3, // first triangle
		1, 2, 3, // second triangle
	};

	// VBO 만들기 전에 VAO 만들기, VAO binding 먼저
	this->m_vertexLayout = VertexLayout::Create();

	// VBO 생성
	// GL_ARRAY_BUFFER -> 정점의 위치를 비롯해 여러 정보들이 있는 버퍼
	// GL_ELEMENT_ARRAY_BUFFER -> 정점의 index 정보들이 들어있는 버퍼
	// vertices indices 복사
	this->m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 32);
	this->m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t) * 6);

	// 0번 사용하기 -> vao attribute 0 -> location 0 in v shader
	// 0번 정점 1번 색깔
	// index / 개수 / 타입 / normalize 여부 / stride / offset
	this->m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
	this->m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 3);
	this->m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 6);

	// shared 생성
	ShaderPtr vertShader = Shader::CreateFromFile("./shader/texture.vs", GL_VERTEX_SHADER);
	ShaderPtr fragShader = Shader::CreateFromFile("./shader/texture.fs", GL_FRAGMENT_SHADER);
	if (!vertShader || !fragShader)
		return false;
	SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
	SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

	// 두개 shader 넘겨서 program 생성
	// { } 이런 식으로 벡터 생성자 호출 가능
	this->m_program = Program::Create({fragShader, vertShader});
  	if (!this->m_program)
		return false;
	SPDLOG_INFO("program id: {}", this->m_program->Get());
	glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

	auto image = Image::Load("./image/container.jpg");
	if (image == nullptr)
		return false;
	SPDLOG_INFO("image: {}x{}, {} channels",
		image->GetWidth(), image->GetHeight(), image->GetChannelCount());
	auto image2 = Image::Load("./image/awesomeface.png");
	if (image2 == nullptr)
		return false;
	SPDLOG_INFO("image2: {}x{}, {} channels",
		image2->GetWidth(), image2->GetHeight(), image2->GetChannelCount());

	this->m_texture = Texture::CreateFromImage(image.get());
	this->m_texture2 = Texture::CreateFromImage(image2.get());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture->Get());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_texture2->Get());

	m_program->Use();
	glUniform1i(glGetUniformLocation(m_program->Get(), "tex"), 0);
	glUniform1i(glGetUniformLocation(m_program->Get(), "tex2"), 1);
	return true;
}

void Context::Render()
{
	// 지금 VBO, VAO, EBO에 binding 되어 있는 것들로 그림 그리기
	// primitives -> 굉장히 다양한 타입의 그림그리는 형식이 존재
	// count 정점의 개수
	// type index의 자료형
	// offset 첫 정점 index
	glClear(GL_COLOR_BUFFER_BIT);
	this->m_program->Use();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

