//
// Created by ozzadar on 2024-10-13.
//
#pragma once

#include "renderers/renderer.h"

namespace OZZ {
    class Overlay: public IRenderer {
    public:
        Overlay() = default;
        ~Overlay() override = default;

        void Initialize() override;
        void Update() override;
        void Render() override;
        void Shutdown() override;
    };
} // OZZ