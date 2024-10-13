//
// Created by ozzadar on 2024-10-12.
//

#pragma once
#include <vector>
#include <memory>
#include <string>
#include <tuple>

#include "windows/window.h"

namespace OZZ {
    class WindowManager {
    public:
        WindowManager();
        ~WindowManager();

        Window* GetWindow(uint32_t key);
        Window* CreateWindow(WindowParams&& Params);
        void DestroyWindow(uint32_t key);

        /**
         * These functions get hooked into the main application loop
         */

        /**
         * @brief Updates all windows
         * @return true if the application should close
         */
         bool Update();

    private:
        void Initialize();
        void Shutdown();
    private:
        std::vector<std::pair<uint32_t, std::unique_ptr<Window>>> Windows;
    };
} // OZZ