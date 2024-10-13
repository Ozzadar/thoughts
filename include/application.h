//
// Created by ozzadar on 2024-10-09.
//

#pragma once

#include <array>
#include <memory>

#include "windows/window_manager.h"
#include "renderers/overlay/overlay.h"
#include "renderers/tool/tool_renderer.h"

namespace OZZ {

    class Application {
    public:
        Application();
        ~Application();

        void Run();

    private:
        void Initialize();
        void Render();
        void Shutdown();
    private:
       bool bRunning { false };

       std::unique_ptr<WindowManager> pWindowManager;
       std::shared_ptr<Overlay> OverlayRenderer;
       std::shared_ptr<ToolRenderer> pToolRenderer;
    };

} // OZZ