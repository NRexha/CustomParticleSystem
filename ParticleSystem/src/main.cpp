#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <algorithm>

#include "Core/ParticleSystem.h"
#include "UI/ParticleSystemUI.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#ifdef _WIN32
extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
#endif

int main()
{
#pragma region GLFWInit
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW.\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Particle System", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Failed to create GLFW window.\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
#pragma endregion

#pragma region GLADInit
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    std::cout << "OpenGL GPU: " << glGetString(GL_RENDERER) << '\n';
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << '\n';

    glEnable(GL_PROGRAM_POINT_SIZE);
#pragma endregion

#pragma region ImGuiInit
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImFont* font = io.Fonts->AddFontFromFileTTF("src/Fonts/JetBrainsMono-Regular.ttf", 16.0f);

    if (font == nullptr)
    {
        std::cerr << "Failed to load font: Fonts/JetBrainsMono-Regular.ttf\n";
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460 core");
#pragma endregion

#pragma region MainLoop
    {
        ParticleSystem particles;
        ParticleSystemUI ui;

        float lastTime = static_cast<float>(glfwGetTime());

        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();

            float time = static_cast<float>(glfwGetTime());
            float dt = std::min(time - lastTime, 1.0f / 30.0f);
            lastTime = time;

            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ui.Draw(particles, dt);

            if (width > 0 && height > 0)
            {
                particles.Update(dt, time);
                particles.Render(width, height);
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }
    }
#pragma endregion

#pragma region Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
#pragma endregion

    return 0;
}
