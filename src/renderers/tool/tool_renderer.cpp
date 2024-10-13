//
// Created by ozzadar on 2024-10-13.
//

#include "renderers/tool/tool_renderer.h"
#include "graphics.h"

namespace OZZ {

    void ToolRenderer::Initialize() {
    }

    void ToolRenderer::Update() {

    }

    void ToolRenderer::Render() {
        // Cornflower blue
        glClearColor(0.392f, 0.584f, 0.929f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        bool bOpen = true;
        ImGui::ShowDemoWindow(&bOpen);
    }

    void ToolRenderer::Shutdown() {

    }
} // OZZ