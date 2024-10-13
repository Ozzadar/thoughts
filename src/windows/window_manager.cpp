//
// Created by ozzadar on 2024-10-12.
//

#include "windows/window_manager.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

OZZ::WindowManager::WindowManager() {
    Initialize();
}

OZZ::WindowManager::~WindowManager() {
    Shutdown();
}

OZZ::Window *OZZ::WindowManager::GetWindow(uint32_t key) {
    auto FoundWindow = std::find_if(Windows.begin(), Windows.end(), [key](
            std::pair<unsigned int, std::unique_ptr<OZZ::Window>>& Window) {
        return Window.first == key;
    });

    return FoundWindow != Windows.end() ? FoundWindow->second.get() : nullptr;
}

OZZ::Window* OZZ::WindowManager::CreateWindow(OZZ::WindowParams &&Params) {
    // check if key is already in use
    auto FoundWindow = std::find_if(Windows.begin(), Windows.end(), [Params](
            std::pair<unsigned int, std::unique_ptr<OZZ::Window>>& Window) {
        return Window.first == Params.key;
    });

    if (FoundWindow != Windows.end()) {
        return nullptr;
    }

    auto& NewWindow = Windows.emplace_back(Params.key, std::make_unique<Window>(std::move(Params)));
    return NewWindow.second.get();
}

void OZZ::WindowManager::DestroyWindow(uint32_t key) {
    auto FoundWindow = std::find_if(Windows.begin(), Windows.end(), [key](
            std::pair<unsigned int, std::unique_ptr<OZZ::Window>>& Window) {
        return Window.first == key;
    });

    if (FoundWindow != Windows.end()) {
        Windows.erase(FoundWindow);
    }
}

void OZZ::WindowManager::Initialize() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

}

void OZZ::WindowManager::Shutdown() {
    // Destroy all windows
    Windows.clear();

    // Terminate glfw
    glfwTerminate();
}

bool OZZ::WindowManager::Update() {
    // Call update method on each window
    for (auto& Window : Windows) {
        if (Window.second->Update()) {
            return true;
        }
    }
    return false;
}


