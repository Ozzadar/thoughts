//
// Created by ozzadar on 2024-10-09.
//

#include "input_subsystem_impl_windows.h"
#include "spdlog/spdlog.h"
#include <wil/win32_helpers.h>


void OZZ::InputSubsystemWindowsImpl::Initialize() {
    // Let's ask for all keyboard input
    RAWINPUTDEVICE Rid[1];

    Rid[0].usUsagePage = 0x01; // Generic Desktop Controls
    Rid[0].usUsage = 0x06; // Keyboard
    Rid[0].dwFlags = RIDEV_NOLEGACY; // No legacy messages please
    Rid[0].hwndTarget = nullptr; // I don't want to tie it to a specific window

//    if (!RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]))) {
//        spdlog::error("Failed to register raw input devices: {}", GetLastError());
//        return;
//    }

//    spdlog::info("Successfully registered raw input devices");
}

void OZZ::InputSubsystemWindowsImpl::HandleInput() {

}

void OZZ::InputSubsystemWindowsImpl::Shutdown() {

}
