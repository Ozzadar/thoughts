//
// Created by ozzadar on 2024-10-13.
//

#include "renderers/tool/tool_renderer.h"
#include "platform.h"
#include <cmath>
#include <chrono>

namespace OZZ {

    void ToolRenderer::Initialize() {
    }

    void ToolRenderer::Update() {

    }

    void ToolRenderer::Render() {
        // oscilate between 0 and 1 using cos
        static auto lastTime = std::chrono::high_resolution_clock::now();
        static double osc = 0.0f;
        auto currentTime = std::chrono::high_resolution_clock::now();
        double deltaTime = std::chrono::duration<double, std::chrono::seconds::period>(currentTime - lastTime).count();
        lastTime = currentTime;

        osc += deltaTime;
        float colorR = std::abs(std::cos(osc));
        float colorB = std::abs(std::sin(osc));
        float colorG = std::abs(std::cos(osc));

        glClearColor(colorR, colorG, colorB, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        bool bOpen = true;
//        ImGui::ShowDemoWindow(&bOpen);
    }

    void ToolRenderer::Shutdown() {

    }
} // OZZ