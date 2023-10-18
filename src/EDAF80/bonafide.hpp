#pragma once
#include "core/helpers.hpp"
#include "core/node.hpp"
#include "core/FPSCamera.h"
#include "core/Bonobo.h"

#define clock (*time)
#define camera (*cam)
#define n(lib) nodes.at(lib)
#define r(lib); n(lib).render(camera.GetWorldToClipMatrix());
#define ro(lib); lib.render(camera.GetWorldToClipMatrix());
#define t(lib,a1,a2,a3); n(lib).get_transform().SetTranslate(glm::vec3(a1,a2,a3));
#define to(lib,a1,a2,a3); lib.get_transform().SetTranslate(glm::vec3(a1,a2,a3));
#define rtx(arg1,arg2) arg1.get_transform().RotateX(arg2)
#define rtz(arg1,arg2) arg1.get_transform().RotateZ(arg2)
#define rtxg(arg) arg.get_transform().GetRotation().x
#define rtzg(arg) arg.get_transform().GetRotation().z
#define sub objects.submarine
#define spawn_node(lib) (lib - 8)

#define NUM_ENEMIES 0
#define NUM_LOOT 0
#define TREASURE_PROB 0.02
#define TIME_BUF 2
#define SEAFLOOR_GRID 3 //num x num
#define SEAFLOOR_THRESH 250
#define SEAFLOOR_DEPTH 280

class bonafide
{
public:
    bonafide(std::vector<Node> nodes, FPSCameraf* cam, float* time);
    
    struct input_t {
        bool val;
        char key;
    };
    
    struct input {
        struct input_t W;
        struct input_t A;
        struct input_t S;
        struct input_t D;
        struct input_t UP;
        struct input_t DOWN;
        struct input_t LEFT;
        struct input_t RIGHT;
        struct input_t SPACE;
        struct input_t ENTER;
    } keydown;

    void mainMenu();
    void pauseMenu();
    void gameframe();
    void mute();
private:
    void test();
    void gen_floor();
    void load(int who, int diff);
    glm::vec3 spawn_loc(int who);
    
    glm::vec3 subT;
    glm::vec3 camT;
    const glm::vec3 c_offset = glm::vec3(0.0f, 1.0f, -10.0f);
    const glm::vec3 p_offset = glm::vec3(0.0f, 0.0f, -1.3f);
    const glm::vec3 title_offset = glm::vec3(1.0f, 0.0f, 1.3f);
    const glm::vec3 spawn = glm::vec3(0.0f, -250.0f, 0.0f);
    std::vector<Node> nodes;
    std::vector<Node> seafloor;
    FPSCameraf* cam;
    float* time;
    
    int difficulty = 0;
    
    enum audio_scenarios {
        SHARK_APPROACH = 0,
        TUNA_APPROACH = 1,
        EERIE = 2
    } playback;
    
    struct properties {
        Node object;
        int health;
    };

    struct objects_t {
        struct submarine_t {
            struct properties ship;
            Node propeller;
        } submarine;
        std::vector<struct properties> enemies;
        std::vector<Node> loot;
    } objects;
    
    enum libids {
        SAND = 0,
        OCEAN = 1,
        SKYBOX = 2,
        WATERBOX = 3,
        SUBMARINE_BODY = 4,
        SUBMARINE_PROPELLER = 5,
        WHALE_SKELETON = 6,
        DINO_SKELETON = 7,
        TUNA = 8,
        SHARK = 9,
        HAMMER_SHARK = 10,
        COIN = 11,
        TREASURE = 12,
        SEAWEED = 13,
        ROCK = 14,
        AMMONITE = 15,
        PANE = 16
    };
    
    class animation_c {
    public:
        void input(char key);
        float* time;
        struct objects_t* objects;
        glm::vec3* subT;
    } animation;
    class physics_c {
    public:
        void test();
        float* time;
        struct objects_t* objects;
    } physics;
};
