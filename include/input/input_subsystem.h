//
// Created by ozzadar on 2024-10-09.
//
#pragma once

#include <input/input_keys.h>
#include <string>
#include <functional>
#include <memory>
#include <array>
#include <chrono>

namespace OZZ {

    using namespace std::chrono_literals;

    struct KeyboardEvent {
        EKey Key;
        EKeyState State;
    };

    struct InputChord {
        std::vector<EKey> Keys;
        bool bIsSequence { false };
        std::chrono::duration<long long, std::milli> TimeBetweenKeys { 1000 };

        // These next values are not meant to be used directly
        std::vector<EKeyState> States;
        EKeyState CurrentState { EKeyState::KeyReleased };
        int CurrentKeyIndex { 0 };
        std::chrono::time_point<std::chrono::high_resolution_clock> LastKeyTime;


        /*
         * Receive an event from the input system
         * @param Key The key that was pressed
         * @param State The state of the key
         * @return True if there was a change in chord state
         */
        bool ReceiveEvent(EKey Key, EKeyState State);
        void EnsureInitialized();

        bool bInitialized { false };
    };

    struct ActionCallbacks {
        std::function<void()> OnPressed;
        std::function<void()> OnReleased;
        std::function<void()> OnHeld;
    };

    struct InputMapping {
        std::string Action;
        InputChord Chord;

        ActionCallbacks Callbacks;
    };

    class IInputSubsystem {
        friend class InputSubsystem;
        virtual void Initialize() = 0;
        virtual void HandleInput() = 0;
        virtual void Shutdown() = 0;
    };

    class InputSubsystem {
    public:
        static std::shared_ptr<InputSubsystem> Get();

        void RegisterInputMapping(InputMapping&& Mapping);
        void UnregisterInputMapping(const std::string& Action);

        void NotifyKeyboardEvent(const KeyboardEvent& Event);

        [[nodiscard]] EKeyState GetKeyState(EKey Key) const {
            return KeyStates[static_cast<size_t>(Key)];
        };

        ~InputSubsystem() = default;
    private:
        InputSubsystem();

        void Initialize();
        void HandleInput();
        void Shutdown();

    private:
        static std::shared_ptr<InputSubsystem> Instance;
        std::array<EKeyState, static_cast<size_t>(EKey::KeyCount)> KeyStates;
        std::unique_ptr<IInputSubsystem> Impl;
        std::vector<InputMapping> Mappings;
    };
} // OZZ
