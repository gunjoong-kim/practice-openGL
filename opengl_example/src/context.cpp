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
	float vertices[] = {
  -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
   0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
   0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
  -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,

  -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
   0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
   0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
  -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,

  -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
  -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
  -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

   0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
   0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
   0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
   0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

  -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
   0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
   0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
  -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

  -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
   0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
   0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
  -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
};

uint32_t indices[] = {
   0,  2,  1,  2,  0,  3,
   4,  5,  6,  6,  7,  4,
   8,  9, 10, 10, 11,  8,
  12, 14, 13, 14, 12, 15,
  16, 17, 18, 18, 19, 16,
  20, 22, 21, 22, 20, 23,
};

	// VBO 만들기 전에 VAO 만들기, VAO binding 먼저
	this->m_vertexLayout = VertexLayout::Create();

	// VBO 생성
	// GL_ARRAY_BUFFER -> 정점의 위치를 비롯해 여러 정보들이 있는 버퍼
	// GL_ELEMENT_ARRAY_BUFFER -> 정점의 index 정보들이 들어있는 버퍼
	// vertices indices 복사
	this->m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 120);
	this->m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t) * 36);

	// 0번 사용하기 -> vao attribute 0 -> location 0 in v shader
	// 0번 정점 1번 색깔
	// index / 개수 / 타입 / normalize 여부 / stride / offset
	this->m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	this->m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, sizeof(float) * 3);

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
	m_program->SetUniform("tex", 0);
	m_program->SetUniform("tex2", 1);

	// x축으로 -55도 회전
	auto model = glm::rotate(glm::mat4(1.0f),
  	glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	// 카메라는 원점으로부터 z축 방향으로 -3만큼 떨어짐
	auto view = glm::translate(glm::mat4(1.0f),
	glm::vec3(0.0f, 0.0f, -3.0f));
	// 종횡비 4:3, 세로화각 45도의 원근 투영
	auto projection = glm::perspective(glm::radians(45.0f),
	(float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 10.0f);
	auto transform = projection * view * model;
	auto transformLoc = glGetUniformLocation(m_program->Get(), "transform");
	m_program->SetUniform("transform", transform);

	return true;
}

void Context::Render()
{
	std::vector<glm::vec3> cubePositions = {
        glm::vec3( 0.0f, 0.0f, 0.0f),
        glm::vec3( 2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f, 2.0f, -2.5f),
        glm::vec3( 1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f),
    };

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

    auto projection = glm::perspective(glm::radians(45.0f),
        (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 20.0f);
    auto view = glm::translate(glm::mat4(1.0f),
        glm::vec3(0.0f, 0.0f, -3.0f));

    for (size_t i = 0; i < cubePositions.size(); i++){
        auto& pos = cubePositions[i];
        auto model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::rotate(model,
            glm::radians((float)glfwGetTime() * 120.0f + 20.0f * (float)i),
            glm::vec3(1.0f, 0.5f, 0.0f));
        auto transform = projection * view * model;
        m_program->SetUniform("transform", transform);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
}

