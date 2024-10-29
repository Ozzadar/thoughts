//
// Created by ozzadar on 2024-10-09.
//

#include <memory>

#include "application.h"
#include "spdlog/spdlog.h"

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

        while (bRunning && pWindowManager->NumWindows()) {
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

//        // We want the window to span the entire screen, on all screens
        int totalWidth = 400;
        int maxHeight = 400;

#ifndef OZZ_WIN_NATIVE
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
#endif

        // Create first window
        WindowParams Params = {
                .Key = 1,
                .Title = "Overlay",
                .Size =  { 400, 400 },
                .Mode = EWindowMode::Windowed,
                .OpenGLVersion = {4, 6},
                .Styles = {
//                        EWindowStyle::AlwaysOnTop,
//                        EWindowStyle::InputPassthrough,
//                        EWindowStyle::TransparentFramebuffer,
EWindowStyle::Decorated
                },
                .bEnableImGUI = false
        };

        auto* pWindow = pWindowManager->NewWindow(std::move(Params));
//        pWindow->SetWindowPosition({minPosX, minPosY});


        pWindow->SetRenderer(OverlayRenderer);
//
        Params = WindowParams {
                .Key = 2,
                .Title = "Other window",
                .Size = { 800, 600 },
                .Mode = EWindowMode::Windowed,
                .OpenGLVersion = {4, 6},
                .Styles = {
                        EWindowStyle::Decorated,
                        EWindowStyle::Resizable,
                },
                .bEnableImGUI = true
        };

        pWindow = pWindowManager->NewWindow(std::move(Params));
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
                                    spdlog::info("Quit hotkey released");
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
                                    spdlog::info("Konami Code Entered");
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