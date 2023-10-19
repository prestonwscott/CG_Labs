#include "bonafide.hpp"


bonafide::bonafide(std::vector<Node*> nodes, FPSCameraf* cam, float* time, glm::vec3* light_pos){
    this->nodes = nodes;
    this->cam = cam;
    this->time = time;
    this->light_pos = light_pos;
    
    sub.ship.object = np(SUBMARINE_BODY);
    sub.ship.model = n(SUBMARINE_BODY); //useless but no default Node
    to(getSub, spawn.x, spawn.y, spawn.z);
    sub.ship.health = 100;
    sub.propeller = np(SUBMARINE_PROPELLER);
    getProp.get_transform().RotateY(glm::three_over_two_pi<float>());
    printf("%p\n", cam);
    printf("%p\n", this->cam);
    
    animation.time = time;
    physics.time = time;
    physics.objects = &objects;
    animation.submarine = &getSub;
    
    int idx = 0;
    for(int i=0;i<SEAFLOOR_GRID;i++){
        for(int z=0;z<SEAFLOOR_GRID;z++){
            seafloor.emplace_back(n(SAND));
            to(seafloor.at(idx++), 50.0f * i - 100.0f, -SEAFLOOR_DEPTH, 50.0f * z - 100.0f);
        }
    }
    camera.mWorld.SetTranslate(glm::vec3(5.0f,-148.0f,8.0f));
    
    glm::vec3 buffC = camT;
    float buffT = clock;
    
    for (int i = 0; i < NUM_LOOT; i++) {
        Node coin = n(COIN);
        to(coin, rand_coord, rand_coord - 250.0f, rand_coord - 250.0f);
        objects.loot.push_back(coin);
    }
    for (int i = 0; i < NUM_ENEMIES; i++) {
        struct properties tuna_p;
        struct properties shark_p;
        Node tuna = n(TUNA);
        Node shark = n(SHARK);
        to(tuna, rand_coord, rand_coord - 250.0f, rand_coord);
        to(shark, rand_coord, rand_coord - 250.0f, rand_coord);
        tuna_p.radius = rand() % RADIUS;
        tuna_p.speed = (rand() % 4) * (glm::pi<float>()/20);
        shark_p.radius = rand() % RADIUS;
        shark_p.speed = (rand() % 5) * (glm::pi<float>() / 30);
        tuna_p.model = tuna;
        shark_p.model = shark;
        objects.enemies.push_back(tuna_p);
        objects.enemies.push_back(shark_p);
    }
    
    glm::vec3 TreasurePosition = glm::vec3(rand_coord, rand_coord, rand_coord);
}

void
bonafide::mainMenu() {
    float waterHeight = -1.0f * sin(clock) - 150.0f;
    to(getSub, subT.x + 7.0f, waterHeight, subT.z - 5.0f);
    t(PANE, camT.x - 5.0f, camT.y + 2.0f, camT.z - 5.0f);
    r(PANE);
    t(OCEAN, camT.x - 500.0f, waterHeight - 0.6f, camT.z - 500.0f);
    r(OCEAN);
    to(getProp, (subT + p_offset).x, (subT + p_offset).y, (subT + p_offset).z);
    ro(getSub);
    ro(getProp);
    r(SKYBOX);
}

void
bonafide::pauseMenu() {
    
}

void
bonafide::gameframe()
{
    printf("%f,%f,%f bon\n", camera.mWorld.GetTranslation().x, camera.mWorld.GetTranslation().y, camera.mWorld.GetTranslation().x);
    bool interpolate = false;
    bool show_control_points = true;
    int iRotationCnt = 0;
    int iCollisionCnt = 0;
    int iRewardCounter = 0;
    float const fSharkCollRadius = 1.0f;
    float const fTunaCollRadius = 1.0f;
    float const fCoinCollRadius = 1.0f;
    float const fSubMarineCollRadius = 1.0f;
    float const fTreasureCollRadius = 1.0f;
    bool circularMovementSet = false;
    
    float waterHeight = -3.0f * sin(clock) - 150.0f;
    
    to(getProp, subT.x - 50.0f, subT.y, subT.z - 50.0f);
    rtz(getProp, 0.1f);
    
    t(OCEAN, camT.x - 500.0f, waterHeight, camT.z - 500.0f);
    r(OCEAN);
    //rtx(getProp, rtxg(getSub));
    //rtz(getProp, rtzg(getSub));
    to(getProp, (subT + p_offset).x, (subT + p_offset).y, (subT + p_offset).z);
    ro(getSub);
    ro(getProp);
    for(Node seg : seafloor){
        ro(seg);
    }
    camera.mWorld.LookAt(subT - c_offset);
    camera.mWorld.SetTranslate(subT + c_offset);
    //glm::vec3 tea = glm::vec3(1.0f,0.0f,0.0f);
    //camera.mWorld.SetRotate(1.0f, glm::rotate(glm::quat(tea), subT - c_offset));
    
    //camera.mWorld.SetRotateY(glm::dot(glm::vec3(glm::column(getSub.get_transform().GetRotation(),2)), glm::vec3(1.0f,1.0f,1.0f)));
    light = subT + s_offset;
    
    //animation.transform = subT;
    
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
        to(getSub, subT.x, waterHeight, subT.z);
        t(SKYBOX, camT.x, 0.0f, camT.z);
        r(SKYBOX);
    }
    else {
        t(WATERBOX, camT.x, camT.y, camT.z);
        r(WATERBOX);
    }
    
    if(!(int)clock % TIME_BUF){
        gen_floor();
        /*if(objects.enemies.size() < NUM_ENEMIES * difficulty + 1)
            load(spawn_node(TUNA), NUM_ENEMIES - (int)objects.enemies.size());
        if(objects.loot.size() < NUM_LOOT * difficulty + 1)
            load(spawn_node(COIN), NUM_LOOT - (int)objects.loot.size());*/
    }
    
    for (int i = 0; i < getLoot.size(); i++)
    {
        if (coll(getSub,getLoot,i)) {
            points++;
            getLoot.erase(objects.loot.begin() + i);
            Node coin = n(COIN);
            getLoot.emplace_back(coin);
            to(getLoot.at(i), rand_coord, rand_coord - 250.0f, rand_coord);
            std::cout << "Point count " << points << "\n";
        } else {
            ro(getLoot.at(i));
            rty(getLoot.at(i), glm::pi<float>() / 20.0f);
        }
    }
    
    for (int i = 0; i < getEnem.size(); i++)
    {
        ro(getEnem.at(i).model);
        if (glm::distance(getT(getSub),getT(getEnem.at(i).model)) <= COLL_THRESH)
        {
            if(points)
                points--;
            else {
                sub.ship.health = 0;
                std::cout << "YOU LOSE! Point count " << points << "\n";
            }
            tl(getSub, subT + 3.0f * (getT(getEnem.at(i).model) - subT));
        }
    
    }
}

void
bonafide::moveObjectCircular(Node& Object, float Omega, float Radius, enum CircularMovement direction, float elapsed_time_s)
{
    float pi = 3.14f;
    glm::vec3 newLoc;

    glm::vec3 ObjectLoc = Object.get_transform().GetTranslation();
    //float vecLen = sqrt( pow(ObjectLoc.x,2) + pow(ObjectLoc.y, 2) + pow(ObjectLoc.z, 2) );
    float vecLen = abs(Radius);
    if (vecLen < 10)
    {
        vecLen = 10; //default length
    }
    newLoc = glm::vec3(vecLen * cos(Omega * elapsed_time_s), ObjectLoc.y, vecLen * sin(Omega * elapsed_time_s));
    Object.get_transform().LookAt(newLoc);
    Object.get_transform().RotateY(pi);
    Object.get_transform().SetTranslate(newLoc);
}

void
bonafide::moveObjectLinear(Node& Object, float movingSpeed, glm::vec3 MovingDirection, float elapsed_time_s)
{
    float pi = 3.14f;
    glm::vec3 newLoc;

    glm::vec3 ObjectLoc = Object.get_transform().GetTranslation();
    newLoc = ObjectLoc + movingSpeed * elapsed_time_s * normalize(MovingDirection);
    Object.get_transform().LookAt(newLoc);
    Object.get_transform().RotateY(pi);
    Object.get_transform().SetTranslate(newLoc);
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
        Node nbuf;
        int res = (int)rand() % (HAMMER_SHARK * difficulty) + TUNA;
        glm::vec3 coord;
        switch(res){
            case(TUNA):
                nbuf = n(TUNA);
                buf.object = &nbuf; //bad code, create new tuna object
                coord = spawn_loc(spawn_node(TUNA));
                buf.health = 25;
                break;
            case(SHARK):
                nbuf = n(SHARK);
                buf.object = &nbuf;
                coord = spawn_loc(spawn_node(SHARK));
                buf.health = 75;
                break;
            default:
                nbuf = n(HAMMER_SHARK);
                buf.object = &nbuf;
                coord = spawn_loc(spawn_node(HAMMER_SHARK));
                buf.health = 100;
        }
        to((*(buf.object)), coord.x, coord.y, coord.z);
        objects.enemies.emplace_back(buf);
    }
    for(int i=0;i<diff && !who;i++){
        struct properties buf;
        bool lucky = (int)rand() % (int)(1/TREASURE_PROB) == (int)clock % (int)(1/TREASURE_PROB);
        buf.object = lucky ? np(TREASURE) : np(COIN);
        glm::vec3 coord = lucky ? spawn_loc(spawn_node(TREASURE)) : spawn_loc(spawn_node(COIN));
        to((*(buf.object)), coord.x, coord.y, coord.z);
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
