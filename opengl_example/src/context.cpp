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
	auto image = Image::Load("./image/wall.jpeg");
	if (!image) 
		return false;
	SPDLOG_INFO("image: {}x{}, {} channels",
	image->GetWidth(), image->GetHeight(), image->GetChannelCount());

	glGenTextures(1, &(this->m_texture));
	glBindTexture(GL_TEXTURE_2D, this->m_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // texture 너무 크면 어떻게 할까
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 반대
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // 가로 0 보다 작거나 1보다 클때
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // 세로 0 보다 작거나 1보다 클때

	// target : 2D texture <- m_texture에 binding 되어 있음
	// 앞 : GPU texture data
	// 뒤 : CPU texture data
	// CPU -> GPU 복사 해야 하므로 여러 description 명세
	// 0 <- level 값 <- min map
	// GL_RGB -> channel
	// 0 <- boarder size
	// image에서의 format
	// 하나의 채널을 표현하는 데이터 타입
	// 실제 데이터 넘겨주기
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
		image->GetWidth(), image->GetHeight(), 0,
		GL_RGB, GL_UNSIGNED_BYTE, image->GetData());
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

