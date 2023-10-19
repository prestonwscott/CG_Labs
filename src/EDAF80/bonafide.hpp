#pragma once
#include "core/helpers.hpp"
#include "core/node.hpp"
#include "core/FPSCamera.h"
#include "core/Bonobo.h"
#include <glm/gtc/type_ptr.hpp>

#define clock (*time)
#define camera (*cam)
#define n(lib) (*(nodes.at(lib)))
#define p(lib) panes.at(lib)
#define np(lib) nodes.at(lib)
#define r(lib); n(lib).render(camera.GetWorldToClipMatrix());
#define rp(lib); p(lib).render(camera.GetWorldToClipMatrix());
#define ro(lib); lib.render(camera.GetWorldToClipMatrix());
#define t(lib,a1,a2,a3); n(lib).get_transform().SetTranslate(glm::vec3(a1,a2,a3));
#define pt(lib,a1,a2,a3); p(lib).get_transform().SetTranslate(glm::vec3(a1,a2,a3));
#define to(lib,a1,a2,a3); lib.get_transform().SetTranslate(glm::vec3(a1,a2,a3));
#define tl(lib,a); lib.get_transform().SetTranslate(a);
#define rtx(arg1,arg2) arg1.get_transform().RotateX(arg2)
#define rty(arg1,arg2) arg1.get_transform().RotateY(arg2)
#define rtz(arg1,arg2) arg1.get_transform().RotateZ(arg2)
#define rtxg(arg) arg.get_transform().GetRotation().x
#define rtzg(arg) arg.get_transform().GetRotation().z
#define sub objects.submarine
#define spawn_node(lib) (lib - 8)
#define rand_coord rand() % RADIUS
#define rand_coordy (rand() % 60) - 220
#define light (*light_pos)
#define getSub (*(sub.ship.object))
#define getProp (*(sub.propeller))
#define getEnem objects.enemies
#define getLoot objects.loot
#define camT camera.mWorld.GetTranslation()
#define subT getSub.get_transform().GetTranslation()
#define getT(arg) (arg).get_transform().GetTranslation()

#define NUM_ENEMIES 50
#define NUM_LOOT 50
#define TREASURE_PROB 0.02
#define TIME_BUF 2
#define SEAFLOOR_GRID 32 //num x num
#define RADIUS 150
#define COLL_THRESH 8.0f
#define SEAFLOOR_THRESH 200
#define SEAFLOOR_DEPTH 280
#define SUB_SPEED 1
#define SUB_ROT glm::pi<float>() / 8

#define coll(arg1,arg2,i) glm::distance(getT(arg1),getT(arg2.at(i))) <= COLL_THRESH

class bonafide
{
public:
    bonafide(std::vector<Node*> nodes, std::vector<Node> panes, FPSCameraf* cam, float* time, glm::vec3* light_pos);
    
    struct input_t {
        bool val;
        char key;
    };
    
    struct input {
        struct input_t W;
        struct input_t A;
        struct input_t S;
        struct input_t D;
        struct input_t E;
        struct input_t Q;
        struct input_t UP;
        struct input_t DOWN;
        struct input_t LEFT;
        struct input_t RIGHT;
        struct input_t SPACE;
        struct input_t ENTER;
    } keydown;
    
    enum MovingStyle {
        CONTROL_POINTS,
        CIRCULAR,
        FOREWARD,
        BACKWARD,
        UPWARD,
        DOWNWARD
    };
    
    enum CircularMovement {
        CLOCKWISE,
        COUNTER_CLOCKWISE
    };
    glm::vec3* light_pos;

    int mainMenu();
    int gameframe();
    int accomp = -1;
private:
    void test();
    void gen_floor();
    void load(int who, int diff);
    glm::vec3 spawn_loc(int who);
    void moveObjectCircular(Node& Object, float Omega, float Radius, enum CircularMovement direction, float elapsed_time_s);
    void moveObjectLinear(Node& Object, float movingSpeed, glm::vec3 MovingDirection, float elapsed_time_s);
    
    glm::vec3* subTp;
    glm::vec3* camTp;
    const glm::vec3 c_offset = glm::vec3(0.0f, 1.0f, -15.0f);
    const glm::vec3 s_offset = glm::vec3(0.0f, 2.0f, -5.0f);
    const glm::vec3 p_offset = glm::vec3(0.0f, 0.0f, -1.3f);
    const glm::vec3 f_offset = glm::vec3(-250.0f, 0.0f, 0.0f);
    const glm::vec3 title_offset = glm::vec3(1.0f, 0.0f, 1.3f);
    const glm::vec3 spawn = glm::vec3(0.0f, -250.0f, 0.0f);
    std::vector<Node*> nodes;
    std::vector<Node> seafloor;
    std::vector<Node> panes;
    FPSCameraf* cam;
    float* time;
    
    int difficulty = 0;
    int points = 0;
    int collisions = 0;
    glm::vec3 bones;
    bool dino;
    
    enum audio_scenarios {
        SHARK_APPROACH = 0,
        TUNA_APPROACH = 1,
        EERIE = 2
    } playback;
    
    struct properties {
        Node* object = nullptr;
        Node model;
        int health;
        float radius;
        float speed;
    };

    struct objects_t {
        struct submarine_t {
            struct properties ship;
            Node* propeller;
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
        SEAWEED = 11,
        ROCK = 12,
        AMMONITE = 13,
        COIN = 14,
        TREASURE = 15,
    };
    
    enum libpane {
        TITLE = 0,
        START = 1,
        QUIT = 2,
        CURSOR = 3,
        SONAR = 4,
        SONAR_PING = 5,
        CICON = 6,
        PICON = 7,
        HEALTH = 8,
        BAR = 9,
        WIN = 10,
        LOSE = 11,
        TEXT = 12,
    };
    
    class animation_c {
    public:
        void input(char key);
        float* time;
        Node* submarine;
        glm::vec3 transform;
        glm::vec3* light_pos;
        float buff = 999.0f;
    } animation;
    class physics_c {
    public:
        void test();
        float* time;
        struct objects_t* objects;
    } physics;
};
