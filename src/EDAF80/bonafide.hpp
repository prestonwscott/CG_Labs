#pragma once
#include "core/helpers.hpp"
#include "core/node.hpp"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "physics.hpp"
#include "animation.hpp"
#include "assignment5.hpp"

#include <stdio.h>

class Bonafide
{
public:
    Bonafide(std::vector<Node> nod, FPSCameraf* cam);
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
    ~Bonafide();
    void gameframe();
private:
    std::vector<Node> nodes;
    FPSCameraf* camera;
};
