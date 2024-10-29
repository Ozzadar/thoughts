//
// Created by ozzadar on 2024-10-12.
//

#pragma once
#include <vector>
#include <memory>
#include <string>
#include <tuple>
#include <mutex>
#include "window.h"
#include "window_params.h"


namespace OZZ {
    constexpr uint32_t MAIN_WINDOW_KEY = 0;

    class WindowManager {
    public:
        WindowManager();
        ~WindowManager();

        uint16_t NumWindows() { return Windows.size(); };
        IWindow* GetWindow(uint32_t key);
        IWindow* NewWindow(OZZ::WindowParams &&Params);
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
        std::shared_ptr<IWindow> pMainWindow { nullptr };
        std::vector<std::pair<uint32_t, std::unique_ptr<IWindow>>> Windows {};

        std::mutex WindowsMutex;
        std::vector<uint32_t> WindowsToDestroy {};
    };
} // OZZ