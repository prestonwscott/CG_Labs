#include "bonafide.hpp"


bonafide::bonafide(std::vector<Node> nodes, FPSCameraf* cam, float* time){
    this->nodes = nodes;
    this->cam = cam;
    this->time = time;
    
    sub.ship.object = n(SUBMARINE_BODY);
    to(sub.ship.object, spawn.x, spawn.y, spawn.z);
    sub.ship.health = 100;
    sub.propeller = n(SUBMARINE_PROPELLER);
    sub.propeller.get_transform().RotateY(glm::three_over_two_pi<float>());
    
    animation.time = time;
    animation.objects = &objects;
    animation.subT = &subT;
    physics.time = time;
    physics.objects = &objects;
    
    int idx = 0;
    for(int i=0;i<SEAFLOOR_GRID;i++){
        for(int z=0;z<SEAFLOOR_GRID;z++){
            seafloor.emplace_back(n(SAND));
            to(seafloor.at(idx++), 50.0f * i - 100.0f, -SEAFLOOR_DEPTH, 50.0f * z - 100.0f);
        }
    }
    camera.mWorld.SetTranslate(glm::vec3(5.0f,-148.0f,8.0f));
    camT = camera.mWorld.GetTranslation();
}

void
bonafide::mainMenu() {
    float waterHeight = -1.0f * sin(clock) - 150.0f;
    to(sub.ship.object, subT.x + 7.0f, waterHeight, subT.z - 5.0f);
    t(PANE, camT.x - 5.0f, camT.y + 2.0f, camT.z - 5.0f);
    r(PANE);
    t(OCEAN, camT.x - 500.0f, waterHeight - 0.6f, camT.z - 500.0f);
    r(OCEAN);
    to(sub.propeller, (subT + p_offset).x, (subT + p_offset).y, (subT + p_offset).z);
    ro(sub.ship.object);
    ro(sub.propeller);
    r(SKYBOX);
}

void
bonafide::pauseMenu() {
    
}

void
bonafide::gameframe()
{
    
    
    float waterHeight = -3.0f * sin(clock) - 150.0f;
    subT = glm::vec3(sub.ship.object.get_transform().GetTranslation());
    camT = glm::vec3(camera.mWorld.GetTranslation());
    
    to(sub.propeller, subT.x - 50.0f, subT.y, subT.z - 50.0f);
    //rtz(sub.propeller, (((int)clock) / 10) % 6);
    
    t(OCEAN, camT.x - 500.0f, waterHeight, camT.z - 500.0f);
    r(OCEAN);
    //rtx(sub.propeller, rtxg(sub.ship.object));
    //rtz(sub.propeller, rtzg(sub.ship.object));
    to(sub.propeller, (subT + p_offset).x, (subT + p_offset).y, (subT + p_offset).z);
    ro(sub.ship.object);
    ro(sub.propeller);
    for(Node seg : seafloor){
        ro(seg);
    }
    //camera.mWorld.LookAt(subT - c_offset);
    //camera.mWorld.SetTranslate(subT + c_offset);
    
    /*
    struct input_t* temp = &keydown.W;
    for(int i=0;i<10;i++){
        if((*temp).val)
            printf("%c\n",(*temp).key);
            animation.input((*temp++).key);
    }*/
    
    
    if(keydown.W.val){
        animation.input(keydown.W.key);
    }
    if(keydown.A.val){
        animation.input(keydown.A.key);
    }
    if(keydown.S.val){
        animation.input(keydown.S.key);
    }
    if(keydown.D.val){
        animation.input(keydown.D.key);
    }
    if(keydown.UP.val){
        animation.input(keydown.UP.key);
    }
    if(keydown.DOWN.val){
        animation.input(keydown.DOWN.key);
    }
    if(keydown.RIGHT.val){
        animation.input(keydown.RIGHT.key);
    }
    if(keydown.LEFT.val){
        animation.input(keydown.LEFT.key);
    }
    if(keydown.ENTER.val){
        animation.input(keydown.ENTER.key);
    }
    if(keydown.SPACE.val){
        animation.input(keydown.SPACE.key);
    }
     
    
    if(camT.y >= waterHeight) {
        to(sub.ship.object, subT.x, waterHeight, subT.z);
        t(SKYBOX, camT.x, 0.0f, camT.z);
        r(SKYBOX);
    }
    else {
        t(WATERBOX, camT.x, camT.y, camT.z);
        r(WATERBOX);
    }
    
    if(!(int)clock % TIME_BUF){
        gen_floor();
        if(objects.enemies.size() < NUM_ENEMIES * difficulty + 1)
            load(spawn_node(TUNA), NUM_ENEMIES - (int)objects.enemies.size());
        if(objects.loot.size() < NUM_LOOT * difficulty + 1)
            load(spawn_node(COIN), NUM_LOOT - (int)objects.loot.size());
    }
        
}

void
bonafide::gen_floor()
{
    char dir;
    glm::vec3 pos = seafloor.at(0).get_transform().GetTranslation();
    int iter;
    for(int i=0;i<SEAFLOOR_GRID;i++){
        for(int j=0;j<SEAFLOOR_GRID;j++){
            float dist = glm::distance(seafloor.at(i+j).get_transform().GetTranslation(), subT);
            
            dir = i == 0 && j != 0 && j != SEAFLOOR_GRID-1 ? 'n' : dir;
            dir = j == SEAFLOOR_GRID-1 && i !=0 && i != SEAFLOOR_GRID-1 ? 'e' : dir;
            dir = i == SEAFLOOR_GRID-1 && j != 0 && j != SEAFLOOR_GRID-1 ? 's' : dir;
            dir = j == 0 && i != 0 && i != SEAFLOOR_GRID-1 ? 'w' : dir;
            if(!dir) {
                dir = i == 0 && j != SEAFLOOR_GRID-1 ? 'n' : dir;
                dir = j == SEAFLOOR_GRID-1 && i != 0 ? 'e' : dir;
                dir = i == SEAFLOOR_GRID-1 && j != SEAFLOOR_GRID-1 ? 's' : dir;
                dir = i == 0 && j != SEAFLOOR_GRID-1 ? 'w' : dir;
            }
            iter = (dir != 'n' && dir != 'e' && dir != 'w' && dir != 's') ? SEAFLOOR_GRID * SEAFLOOR_GRID : SEAFLOOR_GRID;
            
            for(int idx=0, inv=iter-1; idx<iter && dist > SEAFLOOR_THRESH; idx++, inv--){
                Node seg;
                switch(dir){
                    case 'n':
                        printf("north is far\n");
                        //seafloor.erase(seafloor.begin() + idx);
                        seg = n(SAND);
                        to(seg, pos.x + (50.0f * SEAFLOOR_GRID-1), -SEAFLOOR_DEPTH, pos.z + (50.0f * idx)); //translate new seg south
                        seafloor.emplace_back(seg);
                        break;
                    case 'e':
                        printf("east is far\n");
                        //seafloor.erase(seafloor.begin() + ((idx + 1) * SEAFLOOR_GRID) - 1);
                        seg = n(SAND);
                        to(seg, pos.x + (50.0f * idx), -SEAFLOOR_DEPTH, pos.z - 50.0f);
                        seafloor.insert(seafloor.begin() + SEAFLOOR_GRID * idx, seg);
                        break;
                    case 's':
                        printf("south is far\n");
                        seafloor.erase(seafloor.begin() + (SEAFLOOR_GRID * SEAFLOOR_GRID) - (idx + 1));
                        seg = n(SAND);
                        to(seg, pos.x - 50.0f, -SEAFLOOR_DEPTH, pos.z + (50.0f * idx));
                        seafloor.insert(seafloor.begin() + idx, seg);
                        break;
                    case 'w':
                        printf("west is far\n");
                        //seafloor.erase(seafloor.begin() + SEAFLOOR_GRID * idx);
                        seg = n(SAND);
                        to(seg, pos.x + (50.0f * idx), -SEAFLOOR_DEPTH, pos.z + (50.0f * SEAFLOOR_GRID-1));
                        seafloor.insert(seafloor.begin() + ((idx + 1) * SEAFLOOR_GRID) - 1, seg);
                }
            }
            dir = '\0';
        }
    }
}

void
bonafide::load(int who, int diff)
{
    for(int i=0;i<diff && who;i++){
        struct properties buf;
        int res = (int)rand() % (HAMMER_SHARK * difficulty) + TUNA;
        glm::vec3 coord;
        switch(res){
            case(TUNA):
                buf.object = n(TUNA);
                coord = spawn_loc(spawn_node(TUNA));
                buf.health = 25;
                break;
            case(SHARK):
                buf.object = n(SHARK);
                coord = spawn_loc(spawn_node(SHARK));
                buf.health = 75;
                break;
            default:
                buf.object = n(HAMMER_SHARK);
                coord = spawn_loc(spawn_node(HAMMER_SHARK));
                buf.health = 100;
        }
        to(buf.object, coord.x, coord.y, coord.z);
        objects.enemies.emplace_back(buf);
    }
    for(int i=0;i<diff && !who;i++){
        struct properties buf;
        bool lucky = (int)rand() % (int)(1/TREASURE_PROB) == (int)clock % (int)(1/TREASURE_PROB);
        buf.object = lucky ? n(TREASURE) : n(COIN);
        glm::vec3 coord = lucky ? spawn_loc(spawn_node(TREASURE)) : spawn_loc(spawn_node(COIN));
        to(buf.object, coord.x, coord.y, coord.z);
        buf.health = 1;
        objects.enemies.emplace_back(buf);
    }
}

glm::vec3
bonafide::spawn_loc(int who)
{
    switch(who){
        case(spawn_node(TUNA)): return glm::vec3(0.0f);
        case(spawn_node(SHARK)): return glm::vec3(0.0f);
        case(spawn_node(HAMMER_SHARK)): return glm::vec3(0.0f);
        case(spawn_node(TREASURE)): return glm::vec3(0.0f);
        case(spawn_node(COIN)): return glm::vec3(0.0f);
        default: return glm::vec3(0.0f);
    }
}

void
bonafide::test()
{
    int node = (int)((clock/5.0f)) % TREASURE;
    camera.mWorld.LookAt(n(node).get_transform().GetTranslation());
    r(node);
}

void
bonafide::mute()
{
    
}
