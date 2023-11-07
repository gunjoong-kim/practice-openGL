#include "Common.hpp"
#include "Shader.hpp"
#include "Program.hpp"
#include "Context.hpp"

void OnFramebufferSizeChange(GLFWwindow* window, int width, int height)
{
	SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);
	auto context = (Context*)glfwGetWindowUserPointer(window);
	context->Reshape(width, height);
}

void OnCursorPos(GLFWwindow* window, double x, double y) {
	SPDLOG_INFO("mouse location: ({} x {})", x, y);
	auto context = (Context*)glfwGetWindowUserPointer(window);
	context->MouseMove(x, y);
}

void OnMouseButton(GLFWwindow* window, int button, int action, int modifier) {
	auto context = (Context*)glfwGetWindowUserPointer(window);
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	context->MouseButton(button, action, x, y);
}

void OnKeyEvent
(
	GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mods
) {
	SPDLOG_INFO("key: {}, scancode: {}, action: {}, mods: {}{}{}",
		key, scancode,
		action == GLFW_PRESS ? "Pressed" :
		action == GLFW_RELEASE ? "Released" :
		action == GLFW_REPEAT ? "Repeat" : "Unknown",
		mods & GLFW_MOD_CONTROL ? "C" : "-",
		mods & GLFW_MOD_SHIFT ? "S" : "-",
		mods & GLFW_MOD_ALT ? "A" : "-");
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

int main()
{
	SPDLOG_INFO("Start program");
	
	// glfw library init
    SPDLOG_INFO("Initialize glfw");
    if (!glfwInit()) {
        const char* description = nullptr;
        glfwGetError(&description);
        SPDLOG_ERROR("failed to initialize glfw: {}", description);
        return -1;
    }

    // hit about opengl context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create glfw window
    SPDLOG_INFO("Create glfw window");
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME,
      nullptr, nullptr);
    if (!window) {
        SPDLOG_ERROR("failed to create glfw window");
        glfwTerminate();
        return -1;
    }

	glfwMakeContextCurrent(window);

	// load openGL functions by glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
	    SPDLOG_ERROR("failed to initialize glad");
	    glfwTerminate();
	    return -1;
	}

    // Now we can use openGL functions
	auto glVersion = glGetString(GL_VERSION);
	SPDLOG_INFO("OpenGL context version: {}", (const char*)glVersion);

	auto context = Context::Create();
	if (!context) {
		SPDLOG_ERROR("failed to create context");
		glfwTerminate();
		return -1;
	}

	glfwSetWindowUserPointer(window, context.get());//userpointer 기능 어떠한 포인터를 등록해놓을 수 있음

	OnFramebufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);
	glfwSetKeyCallback(window, OnKeyEvent);
	glfwSetCursorPosCallback(window, OnCursorPos);
	glfwSetMouseButtonCallback(window, OnMouseButton);

	// glfw 루프 실행, 윈도우 close 버튼을 누르면 정상 종료
    SPDLOG_INFO("Start main loop");
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
		context->ProcessInput(window);
		context->Render();
		glfwSwapBuffers(window);
    }
	context.reset(); // unique pointer의 멤버
    glfwTerminate();
    return 0;
}