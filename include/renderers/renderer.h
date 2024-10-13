//
// Created by ozzadar on 2024-10-13.
//

#pragma once

class IRenderer {
public:
    IRenderer() = default;
    virtual ~IRenderer() = default;

    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Render() = 0;
    virtual void Shutdown() = 0;
};
