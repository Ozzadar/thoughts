//
// Created by ozzadar on 2024-10-13.
//

#include "renderers/overlay/overlay.h"

#include <spdlog/spdlog.h>

#include "graphics.h"

namespace OZZ {
    void Overlay::Initialize() {
        spdlog::info("Overlay::Initialize()");
    }

    void Overlay::Update() {
        spdlog::info("Overlay::Update()");
    }

    void Overlay::Render() {
        spdlog::info("Overlay::Render()");
        // clear the screen
        glClearColor(0.1f, 0.1f, 0.7f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Overlay::Shutdown() {
        spdlog::info("Overlay::Shutdown()");
    }
} // OZZ