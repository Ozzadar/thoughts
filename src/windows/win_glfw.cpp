//
// Created by ozzadar on 2024-10-12.
//

#include "windows/win_glfw.h"
#include "input/glfw_keys.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>

namespace OZZ {
    WinGLFW::WinGLFW(WindowParams &&InParams) : Params(std::move(InParams)) {
        spdlog::info("Creating GLFW window with title: {} | and key: {}", Params.Title, Params.Key);
        Initialize();
    }

    WinGLFW::~WinGLFW() {
        Shutdown();
    }

    bool WinGLFW::Update() {
        if (glfwWindowShouldClose(pWindow)) {
            return true;
        }

        glfwPollEvents();

        FrameStart();

        // Render
        if (pRenderer) {
            pRenderer->Render();
        }

        FrameEnd();

        return false;
    }

    void WinGLFW::Initialize() {
        Input = std::make_unique<InputSubsystem>();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, Params.OpenGLVersion.first);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, Params.OpenGLVersion.second);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

        // window hints
        // apply window styles
        for (const auto Style : Params.Styles) {
            switch (Style) {
                case EWindowStyle::Resizable:
                    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
                    break;
                case EWindowStyle::TransparentFramebuffer:
                    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
                    break;
                case EWindowStyle::AlwaysOnTop:
                    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
                    break;
                case EWindowStyle::Decorated:
                    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
                    break;
                case EWindowStyle::InputPassthrough:
                    glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);
                    break;
                default:
                    break;
            }
        }

        GLFWmonitor *pMonitor = nullptr;
        if (Params.Mode == EWindowMode::Fullscreen || Params.Mode == EWindowMode::Borderless) {
            // TODO: It's annoying to have to move the application to the other monitor every time the application starts
            // TODO: Implement a way to save the monitor the application was last on.
            pMonitor = glfwGetPrimaryMonitor();

            if (Params.Mode == EWindowMode::Borderless) {
                const GLFWvidmode *pMode = glfwGetVideoMode(pMonitor);
                glfwWindowHint(GLFW_RED_BITS, pMode->redBits);
                glfwWindowHint(GLFW_GREEN_BITS, pMode->greenBits);
                glfwWindowHint(GLFW_BLUE_BITS, pMode->blueBits);
                glfwWindowHint(GLFW_REFRESH_RATE, pMode->refreshRate);
            }
        }

        pWindow = glfwCreateWindow(Params.Size.x, Params.Size.y, Params.Title.c_str(), pMonitor, nullptr);
        if (!pWindow) {
            spdlog::error("Failed to create window | Key: {}", Params.Key);
            glfwTerminate();
            return;
        }
        SetUserPointer(this);

        // Set Windoe
        glfwSetKeyCallback(pWindow, [](GLFWwindow *Window, int Key, int Scancode, int Action, int Mods) {
           auto App = static_cast<OZZ::WinGLFW *>(glfwGetWindowUserPointer(Window));
           try {
               spdlog::info("Key: {}, Action: {}", Key, Action);
               if (App->Input) {
                   if (Action == GLFW_REPEAT) return; // Repeat is not needed.
                   GLFWKeyState glfwKeyState(Action);
                   App->Input->NotifyKeyboardEvent({GLFWKey(Key), glfwKeyState});
               }
           } catch (std::exception &e) {
               spdlog::error("Cannot process Keyboard event, user pointer likely invalid: {}", e.what());
           }
       });

        glfwSetMouseButtonCallback(pWindow, [](GLFWwindow *Window, int Button, int Action, int Mods) {
            auto App = static_cast<OZZ::WinGLFW *>(glfwGetWindowUserPointer(Window));

            try {
                if (App->Input) {
                    GLFWKeyState glfwKeyState(Action);
                }
            } catch (std::exception &e) {
                spdlog::error("Cannot process Mouse event, user pointer likely invalid: {}", e.what());
            }
        });

        MakeContextCurrent();

        // start glad
        if (!gladLoadGL((GLADloadfunc) glfwGetProcAddress)) {
            spdlog::error("Failed to initialize GLAD");
            glfwTerminate();
            return;
        }

        if (Params.bEnableImGUI) {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            io = &ImGui::GetIO();
            io->WantCaptureMouse = false;

            (void) io;
            io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
            io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
            io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

            // Setup Dear ImGui style
            ImGui::StyleColorsDark();
            ImGui_ImplGlfw_InitForOpenGL(pWindow, false);
            ImGui_ImplOpenGL3_Init();
        }

        // enable depth test
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }

    void WinGLFW::MakeContextCurrent() const {
        if (pWindow) {
            glfwMakeContextCurrent(pWindow);
        }
    }


    void WinGLFW::Shutdown() {
        spdlog::info("Shutting down window with key {}", Params.Key);
        if (Params.bEnableImGUI) {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

        io = nullptr;
        Input.reset();
        if (pWindow) {
            glfwDestroyWindow(pWindow);
            pWindow = nullptr;
        }
    }

    void WinGLFW::SetWindowPosition(const glm::ivec2 &Position) const {
        if (pWindow) {
            glfwSetWindowPos(pWindow, Position.x, Position.y);
        }
    }

    void WinGLFW::SetUserPointer(void *Pointer) {
        if (pWindow) {
            UserPointer = Pointer;
            glfwSetWindowUserPointer(pWindow, Pointer);
        }
    }

    void WinGLFW::FrameStart() {
        MakeContextCurrent();
        if (Params.bEnableImGUI) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }
    }

    void WinGLFW::FrameEnd() {
        if (Params.bEnableImGUI) {
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            // Update and Render additional Platform Windows
            // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
            //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
            if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                GLFWwindow* backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }
        }


        glfwSwapBuffers(pWindow);
        glfwPollEvents();
    }

} // OZZ