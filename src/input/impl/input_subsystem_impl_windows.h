//
// Created by ozzadar on 2024-10-09.
//

#pragma once
#include <input/input_subsystem.h>
#include <windows.h>

namespace OZZ {
    class InputSubsystemWindowsImpl : public IInputSubsystem {
    public:
        virtual ~InputSubsystemWindowsImpl() = default;

    private:
        void Initialize() override;

        void HandleInput() override;

        void Shutdown() override;
    };
} // OZZ