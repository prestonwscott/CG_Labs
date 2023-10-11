#pragma once
#include "core/helpers.hpp"
#include "core/node.hpp"
#include "core/FPSCamera.h"

#include "physics.hpp"
#include "animation.hpp"

class bonafide
{
public:
    bonafide(std::vector<Node> nod, FPSCameraf* cam);
    int gameState;
    struct input {
        bool keydown_W;
        bool keydown_A;
        bool keydown_S;
        bool keydown_D;
        bool keydown_UP;
        bool keydown_DOWN;
        bool keydown_LEFT;
        bool keydown_RIGHT;
        bool keydown_SPACE;
        bool keydown_ENTER;
    } keydown;
    void gameframe();
private:
    std::vector<Node> nodes;
    FPSCameraf* camera;
};
