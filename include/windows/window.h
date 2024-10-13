//
// Created by ozzadar on 2024-10-12.
//

#pragma once
#include <cstdint>
#include <string>
#include <memory>

#include <graphics.h>
#include "input/input_subsystem.h"
#include "renderers/renderer.h"
#include <glm/glm.hpp>
#include <utility>

class GLFWwindow;
namespace OZZ {
    enum class EWindowMode {
        Windowed,
        Fullscreen,
        Borderless
    };

    struct WindowParams {
        uint32_t key;
        std::string Title;
        int Width;
        int Height;
        bool bAlwaysOnTop;
        bool bInputPassthrough;
        bool bTransparentFramebuffer;
        bool bDecorated;
        EWindowMode Mode;
        std::pair<int, int> OpenGLVersion;
        bool bEnableImGUI;
    };

    class Window {
    public:
        explicit Window(WindowParams &&InParams);
        ~Window();

        /**
         * @brief Updates the window
         * @return true if the application should close
         */
        bool Update();

        void MakeContextCurrent() const;
        void SetWindowPosition(const glm::vec2& Position) const;
        void SetUserPointer(void* Pointer);
        void SetRenderer(std::shared_ptr<IRenderer> InRenderer) { pRenderer = std::move(InRenderer); }

        [[nodiscard]] void* GetUserPointer() const { return UserPointer; }
        [[nodiscard]] GLFWwindow* GetWindow() const { return pWindow; }
        [[nodiscard]] InputSubsystem* GetInput() const { return Input.get(); }
    private:
        void Initialize();
        void Shutdown();

        void FrameStart();
        void FrameEnd();

    private:
        WindowParams Params;
        std::unique_ptr<InputSubsystem> Input;
        std::shared_ptr<IRenderer> pRenderer;

        GLFWwindow* pWindow { nullptr };

        void* UserPointer { nullptr };
        ImGuiIO *io { nullptr };
    };
} // OZZ
