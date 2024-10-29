//
// Created by ozzadar on 2024-10-12.
//

#pragma once
#include <cstdint>
#include <string>
#include <memory>

#include <platform.h>
#include "input/input_subsystem.h"
#include "renderers/renderer.h"
#include "window_params.h"
#include "window.h"
#include <glm/glm.hpp>
#include <utility>

class GLFWwindow;
namespace OZZ {
    class WinGLFW : public IWindow {
    public:
        explicit WinGLFW(WindowParams &&InParams);
        ~WinGLFW() override;

        /**
         * @brief Updates the window
         * @return true if the application should close
         */
        bool Update() override;

        void MakeContextCurrent() const override;
        void SetWindowPosition(const glm::ivec2& Position) const override;
        void SetUserPointer(void* Pointer) override;
        void SetRenderer(std::shared_ptr<IRenderer> InRenderer) override { pRenderer = std::move(InRenderer); }

        [[nodiscard]] void* GetUserPointer() const override { return UserPointer; }
        [[nodiscard]] InputSubsystem* GetInput() const override { return Input.get(); }
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
