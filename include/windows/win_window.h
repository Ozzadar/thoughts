//
// Created by ozzadar on 2024-10-14.
//

#pragma once

#include <thread>

#include "platform.h"
#include "windows/window.h"
#include "windows/window_params.h"

namespace OZZ {

    class WinWindow : public IWindow {
    public:
        explicit WinWindow(WindowParams &&InParams);
        ~WinWindow() override;

        /**
         * @brief Updates the window
         * @return true if the application should close
         */
        bool Update() override;

        void MakeContextCurrent() const override;
        void SetWindowPosition(const glm::ivec2& Position) const override;
        void SetUserPointer(void* Pointer) override;
        void SetRenderer(std::shared_ptr<IRenderer> InRenderer) override;

        [[nodiscard]] void* GetUserPointer() const override;
        [[nodiscard]] InputSubsystem* GetInput() const override { return Input.get(); }
    private:
        void Initialize();
        void Pump();
        void Shutdown();

        void FrameStart();
        void FrameEnd();

        /**
         * @brief The window procedure for the window
         * @param hWnd The window handle
         * @param uMsg The message
         * @param wParam The first message parameter
         * @param lParam The second message parameter
         * @return The result of the message processing
         */
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        std::thread WindowThread;
        WindowParams Params;
        std::unique_ptr<InputSubsystem> Input;
        std::shared_ptr<IRenderer> pRenderer;

        bool bRunning { false };
        void* UserPointer { nullptr };
        HWND hWnd;
        HDC hDC;
        HGLRC GLContext;
        static WNDCLASSEX wc;
        bool bWCInitted {false };

        static bool bGladInitialized;
        static void InitializeWGLAndGLAD();
    };

} // OZZ
