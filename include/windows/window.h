//
// Created by ozzadar on 2024-10-14.
//

#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <platform.h>
#include <functional>

#include "windows/window_params.h"
#include "renderers/renderer.h"
#include "input/input_subsystem.h"

namespace OZZ {
    class IWindow {
    public:
        /**
         * All IWindows need a constructor that takes WindowParams,
         * I can't force it through an interface, but I can make it a requirement in the documentation
         * There's definitely a more elegant way to do this, but I don't feel like thinking about it right now
         */
        explicit IWindow(const WindowParams& InParams = {}) {};

        virtual ~IWindow() = default;

        virtual bool Update() = 0;

        virtual void MakeContextCurrent() const = 0;

        virtual void SetWindowPosition(const glm::ivec2 &Position) const = 0;

        virtual void SetUserPointer(void *Pointer) = 0;

        virtual void SetRenderer(std::shared_ptr<IRenderer> InRenderer) = 0;

        [[nodiscard]] virtual void *GetUserPointer() const = 0;

        [[nodiscard]] virtual InputSubsystem *GetInput() const = 0;

        std::function<void()> OnClosed;
    };
}

