#pragma once
#include "core/helpers.hpp"
#include "core/node.hpp"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "physics.hpp"
#include "animation.hpp"

#include <stdio.h>

namespace Bonafide{
    class Bonafide
    {
    public:
        Bonafide(Node *objects, FPSCameraf camera);
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
        
        
    private:
        FPSCameraf camera;
        Node *objects;
    };


}
