//
// Created by ozzadar on 2024-10-14.
//

#include "windows/win_window.h"
#include "spdlog/spdlog.h"

namespace OZZ {
    WNDCLASSEX WinWindow::wc = {};
    bool WinWindow::bGladInitialized = false;

    WinWindow::WinWindow(WindowParams &&InParams) : Params(std::move(InParams)), bWCInitted(false) {
        if (!bGladInitialized) {
            InitializeWGLAndGLAD();
        }

        spdlog::info("Creating native Windows window with title: {}", Params.Title);
        Input = std::make_unique<InputSubsystem>();

        WindowThread = std::thread([this]() {
            Initialize();

            bRunning = true;
            while (bRunning) {
                Pump();
            }
        });
    }

    WinWindow::~WinWindow() {
        if (WindowThread.joinable()) {
            WindowThread.join();
        }
    }

    bool WinWindow::Update() {

        if (!bRunning) return false;

        FrameStart();

        // Render
        if (pRenderer) {
            pRenderer->Render();
        }

        FrameEnd();
        return false;
    }

    void WinWindow::MakeContextCurrent() const {
        if (hDC) {
            wglMakeCurrent(hDC, GLContext);
        }
    }

    void WinWindow::SetWindowPosition(const glm::ivec2 &Position) const {
        if (hWnd) {
            bool bAlwaysOnTop = std::find(Params.Styles.begin(), Params.Styles.end(), EWindowStyle::AlwaysOnTop) != Params.Styles.end();
            SetWindowPos(hWnd, bAlwaysOnTop ? HWND_TOPMOST : nullptr, Position.x, Position.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        }
    }

    void WinWindow::SetUserPointer(void *Pointer) {

    }

    void WinWindow::SetRenderer(std::shared_ptr<IRenderer> InRenderer) { pRenderer = std::move(InRenderer); }

    void *WinWindow::GetUserPointer() const { return UserPointer; }

    void WinWindow::Initialize() {

        std::string ClassName = "OzzWindowClass-" + Params.Key;

        //TODO: We probably dont need to create a new window class per window
// Define the color
        COLORREF cornflowerBlue = RGB(100, 149, 237);

// Create the brush
        HBRUSH hBrush = CreateSolidBrush(cornflowerBlue);
        // Register window class
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.lpfnWndProc = WndProc;
        wc.hInstance = nullptr;
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_CLASSDC;
        wc.lpszClassName = ClassName.c_str();
        wc.hbrBackground = hBrush;

        if (!RegisterClassEx(&wc)) {
            spdlog::error("Failed to register window class");
            return;
        }

        assert("Must define at least one window style" && !Params.Styles.empty());

        // Set window styles
        long Style = 0;
        long ExStyle = 0;
        for (const auto StyleFlag : Params.Styles) {
            switch (StyleFlag) {
                case EWindowStyle::Decorated : {
                    Style |= WS_OVERLAPPEDWINDOW;
                    break;
                }
                case EWindowStyle::Resizable : {
                    Style |= WS_THICKFRAME;
                    break;
                }
                case EWindowStyle::AlwaysOnTop : {
                    ExStyle |= WS_EX_TOPMOST;
                    break;
                }
                case EWindowStyle::InputPassthrough : {
                    EnableWindow(hWnd, false);
                    break;
                }
                case EWindowStyle::TransparentFramebuffer : {
                    ExStyle |= WS_EX_LAYERED;
                    break;
                }
                case EWindowStyle::ToolWindow : {
                    ExStyle = WS_EX_TOOLWINDOW;
                    break;
                }
            }
        }

        // Create window
        hWnd = CreateWindowEx(
                ExStyle, // window styles
                ClassName.c_str(), // window class
                Params.Title.c_str(), // window title
                WS_OVERLAPPEDWINDOW, // window styles -- borderless windowed would go here ish
                CW_USEDEFAULT, CW_USEDEFAULT, // x, y
                Params.Size.x, Params.Size.y, // width, height
                nullptr, // parent window
                nullptr, // menu
                nullptr, // instance
                nullptr // lparam
        );
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

        if (!hWnd) {
            spdlog::error("Failed to create window | Key: {}", Params.Key);
            return;
        }

        // if styles contains transparent framebuffer, set window to transparent
        if (std::find(Params.Styles.begin(), Params.Styles.end(), EWindowStyle::TransparentFramebuffer) != Params.Styles.end()) {
            SetLayeredWindowAttributes(hWnd, 0, 128, LWA_ALPHA);
        } else {
            SetLayeredWindowAttributes(hWnd, cornflowerBlue, 255, LWA_COLORKEY);
        }

        SetWindowPosition(Params.Position);
        bool bHidden = Params.Mode == EWindowMode::Hidden;
        ShowWindow(hWnd, bHidden ? SW_HIDE: SW_SHOW);
        UpdateWindow(hWnd);

        hDC = GetDC(hWnd);

        // Set up opengl on this window
        int PixelFormatAttributes[] = {
                WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
                WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
                WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
                WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
                WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
                WGL_COLOR_BITS_ARB, 32,
                WGL_DEPTH_BITS_ARB, 24,
                WGL_STENCIL_BITS_ARB, 8,
                0
        };

        int PixelFormat {};
        UINT NumFormats {};
        wglChoosePixelFormatARB(hDC, PixelFormatAttributes, nullptr, 1, &PixelFormat, &NumFormats);
        if (!NumFormats) {
            spdlog::error("Failed to choose pixel format");
            return;
        }
        PIXELFORMATDESCRIPTOR PixelFormatDescriptor {};
        DescribePixelFormat(hDC, PixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &PixelFormatDescriptor);
        SetPixelFormat(hDC, PixelFormat, &PixelFormatDescriptor);

        // Create OpenGL context
        int OpenGLContextAttributes[] = {
                WGL_CONTEXT_MAJOR_VERSION_ARB, Params.OpenGLVersion.first,
                WGL_CONTEXT_MINOR_VERSION_ARB, Params.OpenGLVersion.second,
                WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
                0
        };

        GLContext = wglCreateContextAttribsARB(hDC, nullptr, nullptr);
        if (!GLContext) {
            spdlog::error("Failed to create OpenGL context");
            return;
        }
    }

    void WinWindow::Pump() {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
//        InvalidateRect(hWnd, NULL, TRUE);
//        UpdateWindow(hWnd);
    }

    void WinWindow::Shutdown() {
        bRunning = false;
        spdlog::info("Native window with title {} shutting down", Params.Title);

        wglDeleteContext(GLContext);
        DestroyWindow(hWnd);
        OnClosed();
    }

    void WinWindow::FrameStart() {
        MakeContextCurrent();
    }

    void WinWindow::FrameEnd() {
        wglSwapIntervalEXT(true);
        wglSwapLayerBuffers(hDC, WGL_SWAP_MAIN_PLANE);
    }

    LRESULT WinWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        // Set window long pointer
        switch (uMsg) {
            case WM_CLOSE: {
                spdlog::debug("Window close message received");
                auto *pWindow = reinterpret_cast<WinWindow *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
                pWindow->Shutdown();
                return 1;
            }
            case WM_ERASEBKGND:
                return 0;
            case WM_PAINT:
                // ignore
                return 0;
            default: {
                return DefWindowProc(hWnd, uMsg, wParam, lParam);
            }
        }

        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    void WinWindow::InitializeWGLAndGLAD() {
        spdlog::info("Initializing WGL and GLAD");

        WNDCLASSEX windowClass = {};
        windowClass.style = CS_OWNDC;
        windowClass.lpfnWndProc = DefWindowProcA;
        windowClass.lpszClassName ="OGL3DDummyWindow";
        windowClass.cbSize = sizeof(WNDCLASSEX);

        auto classId = RegisterClassEx(&windowClass);

        HWND dummyWindow = CreateWindowEx(
                0,
                MAKEINTATOM(classId),
                "OGL3DDummyWindow",
                0,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                0,
                0,
                windowClass.hInstance,
                0);

        assert(dummyWindow);

        HDC dummyDC = GetDC(dummyWindow);

        PIXELFORMATDESCRIPTOR pfd = {};
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.cColorBits = 32;
        pfd.cAlphaBits = 8;
        pfd.iLayerType = PFD_MAIN_PLANE;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;

        int pixelFormat = ChoosePixelFormat(dummyDC, &pfd);
        SetPixelFormat(dummyDC, pixelFormat, &pfd);

        HGLRC dummyContext = wglCreateContext(dummyDC);
        assert(dummyContext);

        bool res = wglMakeCurrent(dummyDC, dummyContext);
        assert(res);


        if (!gladLoaderLoadWGL(dummyDC))
            throw std::runtime_error("OGraphicsEngine - gladLoadWGL failed");

        if (!gladLoaderLoadGL())
            throw std::runtime_error("OGraphicsEngine - gladLoadGL failed");


        wglMakeCurrent(dummyDC, 0);
        wglDeleteContext(dummyContext);
        ReleaseDC(dummyWindow, dummyDC);
        DestroyWindow(dummyWindow);

        bGladInitialized = true;
    }
} // OZZ