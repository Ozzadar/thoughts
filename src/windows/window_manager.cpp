//
// Created by ozzadar on 2024-10-12.
//

#include "windows/window_manager.h"
#include "spdlog/spdlog.h"
#ifndef OZZ_WIN_NATIVE
    #include "windows/win_glfw.h"
#else
#include "windows/win_window.h"
#endif

OZZ::WindowManager::WindowManager() {
    Initialize();
}

OZZ::WindowManager::~WindowManager() {
    Shutdown();
}

OZZ::IWindow *OZZ::WindowManager::GetWindow(uint32_t key) {
    auto FoundWindow = std::find_if(Windows.begin(), Windows.end(), [key](
            std::pair<unsigned int, std::unique_ptr<OZZ::IWindow>>& Window) {
        return Window.first == key;
    });

    return FoundWindow != Windows.end() ? FoundWindow->second.get() : nullptr;
}

OZZ::IWindow* OZZ::WindowManager::NewWindow(OZZ::WindowParams &&Params) {
    // check if key is already in use
    auto FoundWindow = std::find_if(Windows.begin(), Windows.end(), [Params](
            std::pair<unsigned int, std::unique_ptr<OZZ::IWindow>>& Window) {
        return Window.first == Params.Key;
    });

    if (FoundWindow != Windows.end()) {
        return nullptr;
    }

#ifdef OZZ_WIN_NATIVE
    // if on windows, use WinWindow
    #ifdef _WIN32
        auto& NewWindow = Windows.emplace_back(Params.Key, std::make_unique<WinWindow>(std::move(Params)));
        NewWindow.second->OnClosed = [this, Params]() {
            std::lock_guard Lock(WindowsMutex);
            WindowsToDestroy.push_back(Params.Key);
        };
    #endif
#else
    auto& NewWindow = Windows.emplace_back(Params.Key, std::make_unique<WinGLFW>(std::move(Params)));
#endif
    return NewWindow.second.get();
}

void OZZ::WindowManager::DestroyWindow(uint32_t key) {
    auto FoundWindow = std::find_if(Windows.begin(), Windows.end(), [key](
            std::pair<unsigned int, std::unique_ptr<OZZ::IWindow>>& Window) {
        return Window.first == key;
    });

    if (FoundWindow != Windows.end()) {
        Windows.erase(FoundWindow);
    }
}

void OZZ::WindowManager::Initialize() {
#ifndef OZZ_WIN_NATIVE
    if (!glfwInit()) {
        spdlog::error("Failed to initialize GLFW");
        return;
    }
#else
    // Let's create an invisible window to catch raw input
    // we can then forward what we get through to the individual windows under our control
//    NewWindow(WindowParams {
//        .Key = MAIN_WINDOW_KEY,
//        .Title = "Raw Input Window",
//        .Size = { 400, 400 },
//        .Mode = EWindowMode::Windowed,
//        .OpenGLVersion = { 4, 6 },
//        .Styles = { EWindowStyle::ToolWindow },
//    });
#endif

}

void OZZ::WindowManager::Shutdown() {
    // Destroy all windows
    Windows.clear();

    // Terminate glfw
    glfwTerminate();
}

bool OZZ::WindowManager::Update() {
    // destroy windows that need to be destroyed
    {
        std::lock_guard Lock(WindowsMutex);
        for (auto& Key : WindowsToDestroy) {
            DestroyWindow(Key);
        }
        WindowsToDestroy.clear();
    }

    // Call update method on each window
    for (auto& Window : Windows) {
        if (Window.second->Update()) {
            return true;
        }
    }
    return false;
}


