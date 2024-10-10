//
// Created by ozzadar on 2024-10-09.
//

#pragma once

#include "GLFW/glfw3.h"
#include "imgui.h"
#include <array>
#include <memory>

namespace OZZ {

    class Application {
    public:
        Application();
        ~Application();

        void Run();

    private:
        void Initialize();
        void HandleInput();
        void Render();
        void StartFrame();
        void EndFrame();
        void Shutdown();
    private:
       GLFWwindow* Window { nullptr };
       bool bRunning { false };
       ImGuiIO* io;

       std::shared_ptr<class InputSubsystem> InputSubsystem;
    };

} // OZZ