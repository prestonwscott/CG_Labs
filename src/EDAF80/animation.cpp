#include "bonafide.hpp"
#define ob (*objects)
#define sT (*subT)

void
bonafide::animation_c::input(char key)
{
    switch(key) {
        case 'W':
            printf("w\n");
            to(ob.submarine.ship.object, sT.x - 0.5f, sT.y, sT.z);
            break;
        case 'A':
            to(ob.submarine.ship.object, sT.x, sT.y, sT.z - 0.5f);
            break;
        case 'S':
            to(ob.submarine.ship.object, sT.x + 0.5f, sT.y, sT.z);
            break;
        case 'D':
            to(ob.submarine.ship.object, sT.x, sT.y, sT.z + 0.5f);
            break;
        case 'U':
            to(ob.submarine.ship.object, sT.x, sT.y + 0.5f, sT.z);
            break;
        case 'N':
            to(ob.submarine.ship.object, sT.x, sT.y + 0.5f, sT.z);
            break;
        case 'R':
            break;
        case 'L':
            break;
        case 'E':
            break;
        case 'P':
            break;
    }
}
