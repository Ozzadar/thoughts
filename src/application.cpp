//
// Created by ozzadar on 2024-10-09.
//

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
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
            glfwPollEvents();

            HandleInput();
            if (glfwWindowShouldClose(Window)) {
                bRunning = false;
                continue;
            }

            // TODO: Most of the stuff in this loop can be extracted to a "Game" class
            //  -- this would allow for re-using the framework for other projects
            StartFrame();
            Render();
            EndFrame();
        }
    }

    void Application::Initialize() {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return;
        }

        // window hints
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

        // Create the window
        Window = glfwCreateWindow(800, 600, "Hello, World!", nullptr, nullptr);
        if (!Window) {
            std::cerr << "Failed to create window" << std::endl;
            glfwTerminate();
            return;
        }

        // Make the window's context current
        glfwMakeContextCurrent(Window);

        // start glad
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            glfwTerminate();
            return;
        }

        glfwSetWindowUserPointer(Window, this);
        glfwSetKeyCallback(Window, [](GLFWwindow *Window, int Key, int Scancode, int Action, int Mods) {
            auto App = static_cast<Application *>(glfwGetWindowUserPointer(Window));
            if (App->InputSubsystem) {
                if (Action == GLFW_REPEAT) return; // Repeat is not needed.
                GLFWKeyState glfwKeyState(Action);
                App->InputSubsystem->NotifyKeyboardEvent({GLFWKey(Key), glfwKeyState});
            }
        });

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        io = &ImGui::GetIO();
        (void) io;
        io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
        io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
        io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(Window, true);
        ImGui_ImplOpenGL3_Init();

        // get version info
        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
        // renderer string
        std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;

        // enable depth test
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        // Register global hotkeys
        InputSubsystem = InputSubsystem::Get();
        InputSubsystem->RegisterInputMapping(
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

        InputSubsystem->RegisterInputMapping(
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
    }

    void Application::Shutdown() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        io = nullptr;
        Window = nullptr;

        // cleanup
        glfwDestroyWindow(Window);
        glfwTerminate();
    }

    void Application::Render() {
        ImGui::ShowDemoWindow();

        ImGui::Render();

        // clear the screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    }

    void Application::StartFrame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void Application::EndFrame() {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        // swap buffers
        glfwSwapBuffers(Window);
    }

    void Application::HandleInput() {
        if (InputSubsystem->GetKeyState(EKey::Escape) == EKeyState::KeyPressed) {
            bRunning = false;
        }
    }
} // OZZ