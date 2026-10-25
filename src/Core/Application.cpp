#include "glad/glad.h"

#include "Application.h"

#include <functional>
#include <imgui.h>
#include <string>
#include <GLFW/glfw3.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Util.h"
#include "UI/Views/LoginView.hpp"
#include "UI/Views/MainView.h"

using namespace Core;

void WindowResizeCallback(GLFWwindow* window, const int width, const int height)
{
    if (width == 0 || height == 0) return;

    glViewport(0, 0, width, height);

    auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));


    if (app)
    {
        const ImVec2 oldSize = app->GetLastWindowSize();
        const ImVec2 newSize = { static_cast<float>(width), static_cast<float>(height) };
        app->GetViewManager()->OnViewportResize(oldSize, newSize);
        app->SetLastWindowSize(newSize);
    }
}

Application::Application(AppErrorCode* error) : window_(nullptr)
{
    if (!glfwInit())
    {
        // spd log. ERROR could not initialize GLFW
        if (error) *error = AppErrorCode::GLFW_INIT_FAILED;
        return;
    }

    if (error) *error = AppErrorCode::SUCCESS;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // spd log print gl version?

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

AppErrorCode Application::Init(const int width, const int height, const std::string_view title)
{
    {
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        GLFWwindow* window = glfwCreateWindow(width, height, std::string(title).c_str(), nullptr, nullptr);
        if (!window)
        {
            // spd log
            return AppErrorCode::GLFW_INIT_FAILED;
        }

        window_ = window;
        SetLastWindowSize({static_cast<float>(width), static_cast<float>(height)});
    }

    glfwSetWindowUserPointer(window_, this);

    glfwSetFramebufferSizeCallback(window_, WindowResizeCallback);

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1); // VSync, enabled for now

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        // spd log
        return AppErrorCode::GLAD_INIT_FAILED;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

#if defined(_WIN32)
    // Windows: Load Segoe UI (the modern Windows UI font) or Arial
    const ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\seguiui.ttf", 16.0f);
#elif defined(__APPLE__)
    // macOS: Load San Francisco or Helvetica
    const ImFont* font = io.Fonts->AddFontFromFileTTF("/System/Library/Fonts/Helvetica.ttc", 16.0f);
#else
    const ImFont* font = io.Fonts->AddFontFromFileTTF("resources/fonts/DejaVuSans/DejaVuSans.ttf", 16.0f);
#endif

    if (!font) {
        io.Fonts->AddFontDefault();
    }

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    std::function<void(Model::ViewType)> navCallback = [this](Model::ViewType target)
    {
        viewManager_->SwitchView(target);
    };

    themeController_ = std::make_unique<Service::ThemeController>();

    viewManager_ = std::make_unique<UI::ViewManager>();
    viewManager_->RegisterView<UI::View::LoginView>(Model::ViewType::LOGIN, navCallback, *themeController_);
    viewManager_->RegisterView<UI::View::MainView>(Model::ViewType::MAIN, navCallback, *themeController_);

    return AppErrorCode::SUCCESS;
}

Application::~Application()
{
    if (ImGui::GetCurrentContext() != nullptr)
    {
        const ImGuiIO& io = ImGui::GetIO();
        ImGui::DestroyPlatformWindows();

        if (io.BackendRendererUserData != nullptr)
            ImGui_ImplOpenGL3_Shutdown();

        if (io.BackendPlatformUserData != nullptr)
            ImGui_ImplGlfw_Shutdown();

        ImGui::DestroyContext();
    }

    if (window_)
    {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }

    glfwTerminate();
}

void Application::Run()
{
    viewManager_->SwitchView(Model::ViewType::MAIN);

    while (!glfwWindowShouldClose(window_))
    {
        Update();
        Render();
    }
}

void Application::Render()
{
    PreRender();

    themeController_->PushGlobalTheme();
    viewManager_->Render();
    themeController_->PopGlobalTheme();

    PostRender();
}

void Application::Update()
{
    glfwPollEvents();
    viewManager_->Update();
}

void Application::PreRender() const
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    if (viewport->WorkSize.x <= 0 || viewport->WorkSize.y <= 0)
        return;
    viewManager_->InitializeViews();
    //ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

}

void Application::PostRender() const
{
    ImGui::Render();

    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);
    if (width == 0 || height == 0) return;

    GLCall(glViewport(0, 0, width, height));

    GLCall(glClearColor(0.10f, 0.11f, 0.13f, 1.00f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT));

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    const ImGuiIO& io = ImGui::GetIO(); (void)io;
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(window_);
}
