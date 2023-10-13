#pragma once
#include "core/helpers.hpp"
#include "core/node.hpp"
#include "core/FPSCamera.h"
#include "core/Bonobo.h"

#include "physics.hpp"
#include "animation.hpp"

class bonafide
{
public:
    bonafide(std::vector<Node> nod, FPSCameraf* cam, long long* time);
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
    struct properties {
        Node object;
        glm::vec3 coord;
        int health;
    } model;
    void gameframe();
    long long clock;
private:
    std::vector<Node> nodes;
    FPSCameraf* camera;
    enum audio_scenarios {
        SHARK = 0,
        TUNA = 0,
        EERIE = 0
    } playback;
};
