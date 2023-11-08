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

	return true;
}

void Context::ProcessInput(GLFWwindow* window) {
	if (!m_cameraControl)
		return;
	const float cameraSpeed = 0.05f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		m_cameraPos += cameraSpeed * m_cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		m_cameraPos -= cameraSpeed * m_cameraFront;

	auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		m_cameraPos += cameraSpeed * cameraRight;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		m_cameraPos -= cameraSpeed * cameraRight;    

	auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		m_cameraPos += cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		m_cameraPos -= cameraSpeed * cameraUp;
}

void Context::Reshape(int width, int height) {
	m_width = width;
	m_height = height;
	glViewport(0, 0, m_width, m_height);
}

void Context::MouseMove(double x, double y) {
	if (!m_cameraControl)
		return;
	auto pos = glm::vec2((float)x, (float)y);
	auto deltaPos = pos - m_prevMousePos;

	const float cameraRotSpeed = 0.8f;
	m_cameraYaw -= deltaPos.x * cameraRotSpeed;
	m_cameraPitch -= deltaPos.y * cameraRotSpeed;

	if (m_cameraYaw < 0.0f)		m_cameraYaw += 360.0f;
	if (m_cameraYaw > 360.0f)	m_cameraYaw -= 360.0f;

	if (m_cameraPitch > 89.0f)	m_cameraPitch = 89.0f;
	if (m_cameraPitch < -89.0f)	m_cameraPitch = -89.0f;

	m_prevMousePos = pos;
}

void Context::MouseButton(int button, int action, double x, double y) {
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
	if (action == GLFW_PRESS) {
		// 마우스 조작 시작 시점에 현재 마우스 커서 위치 저장
		m_prevMousePos = glm::vec2((float)x, (float)y);
		m_cameraControl = true;
	}
	else if (action == GLFW_RELEASE) {
		m_cameraControl = false;
	}
	}
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

	m_cameraFront =
	glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
	glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
	glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

	auto projection = glm::perspective(glm::radians(45.0f),
		(float)m_width / (float)m_height, 0.01f, 40.0f);

	auto view = glm::lookAt(
		m_cameraPos,
		m_cameraPos + m_cameraFront,
		m_cameraUp);

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

