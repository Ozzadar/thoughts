//
// Created by ozzadar on 2024-10-14.
//
#pragma once
#include <string>
#include <glm/glm.hpp>

namespace OZZ {

    enum class EWindowMode {
        Windowed,
        Fullscreen,
        Borderless,
        Hidden
    };

    enum class EWindowStyle {
        Decorated,
        Resizable,
        AlwaysOnTop,
        InputPassthrough,
        TransparentFramebuffer,
        ToolWindow
    };

    struct WindowParams {
        uint32_t Key;
        std::string Title;
        glm::i32vec2 Size;
        glm::i32vec2 Position;
        EWindowMode Mode;
        std::pair<int, int> OpenGLVersion;
        std::vector<EWindowStyle> Styles;

        bool bEnableImGUI;
    };
}