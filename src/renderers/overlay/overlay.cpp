//
// Created by ozzadar on 2024-10-13.
//

#include "renderers/overlay/overlay.h"

#include <spdlog/spdlog.h>

#include "platform.h"

namespace OZZ {
    void Overlay::Initialize() {
        spdlog::info("Overlay::Initialize()");
    }

    void Overlay::Update() {
        spdlog::info("Overlay::Update()");
    }

    void Overlay::Render() {
#ifdef NDEBUG
        glClearColor(0.0, 0.0, 0.0, 0.0f);
#else
        // clear the screen -- the color is a little brighter just for debugging
        glClearColor(0.05, 255.f, 0.05, 255.0f);
#endif
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Overlay::Shutdown() {
        spdlog::info("Overlay::Shutdown()");
    }
} // OZZ