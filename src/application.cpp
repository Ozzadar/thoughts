//
// Created by ozzadar on 2024-10-09.
//

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <iostream>

#include <input/input_subsystem.h>
#include "application.h"
#include "input/glfw_keys.h"

using namespace std::chrono_literals;
namespace OZZ {
    Application::Application() {
        Initialize();
    }

    Application::~Application() {
        Shutdown();
    }

    void Application::Run() {
        bRunning = true;

        while (bRunning) {
             if (pWindowManager->Update()) {
                 bRunning = false;
             }
         }
    }

    void Application::Initialize() {
        // Initialize Window Manager
        pWindowManager = std::make_unique<WindowManager>();
        OverlayRenderer = std::make_shared<Overlay>();
        pToolRenderer = std::make_shared<ToolRenderer>();

        // We want the window to span the entire screen, on all screens
        int totalWidth = 0;
        int maxHeight = 0;
        int monitorCount = 0;
        int minPosX = 0;
        int minPosY = 0;
        GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);

        for (int i = 0; i < monitorCount; i++) {
            const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);
            totalWidth += mode->width;
            maxHeight = std::max(maxHeight, mode->height);

            int posX, posY;
            glfwGetMonitorPos(monitors[i], &posX, &posY);

            minPosX = std::min(minPosX, posX);
            minPosY = std::min(minPosY, posY);
        }

        // Create first window
        WindowParams Params = {
                .key = 0,
                .Title = "Overlay",
                .Width = totalWidth,
                .Height = maxHeight,
                .bAlwaysOnTop = true,
                .bInputPassthrough = true,
                .bTransparentFramebuffer = true,
                .bDecorated = false,
                .Mode = EWindowMode::Windowed,
                .OpenGLVersion = {4, 6},
                .bEnableImGUI = false
        };

        auto* pWindow = pWindowManager->CreateWindow(std::move(Params));
        pWindow->SetWindowPosition({minPosX, minPosY});


        pWindow->SetRenderer(OverlayRenderer);

        Params = {
                .key = 1,
                .Title = "Other window",
                .Width = 800,
                .Height = 600,
                .bAlwaysOnTop = false,
                .bInputPassthrough = false,
                .bTransparentFramebuffer = false,
                .bDecorated = true,
                .Mode = EWindowMode::Windowed,
                .OpenGLVersion = {4, 6},
                .bEnableImGUI = true
        };

        pWindow = pWindowManager->CreateWindow(std::move(Params));
        // Register global hotkeys
        pWindow->GetInput()->RegisterInputMapping(
                {
                        .Action = "Quit",
                        .Chord = InputChord{.Keys = std::vector<EKey>{EKey::LControl, EKey::LAlt, EKey::X}},
                        .Callbacks = {
                                .OnPressed = [this]() {
                                    bRunning = false;
                                },
                                .OnReleased = []() {
                                    std::cout << "Escape Released" << std::endl;
                                }
                        }
                });

        pWindow->GetInput()->RegisterInputMapping(
                {
                        .Action = "Konami",
                        .Chord = {
                                .Keys = std::vector<EKey>{EKey::Up, EKey::Up, EKey::Down, EKey::Down,
                                                          EKey::Left, EKey::Right, EKey::Left, EKey::Right,
                                                          EKey::B, EKey::A},
                                .bIsSequence = true,
                                .TimeBetweenKeys = 1000ms

                        },
                        .Callbacks = {
                                .OnPressed = []() {
                                    std::cout << "Konami Code Entered" << std::endl;
                                }
                        }
                });

        pWindow->SetRenderer(pToolRenderer);
    }

    void Application::Shutdown() {
        pWindowManager.reset();
    }

    void Application::Render() {

    }

} // OZZ