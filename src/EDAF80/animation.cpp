#include "bonafide.hpp"
#define transform (*submarine).get_transform().GetTranslation()

void
bonafide::animation_c::input(char key)
{
    switch(key) {
        case 'W':
            to((*submarine), transform.x, transform.y, transform.z + 1.0f/SUB_SPEED);
            break;
        case 'A':
            to((*submarine), transform.x + 1.0f/SUB_SPEED, transform.y, transform.z);
            break;
        case 'S':
            to((*submarine), transform.x, transform.y, transform.z - 1.0f/SUB_SPEED);
            break;
        case 'D':
            to((*submarine), transform.x - 1.0f/SUB_SPEED, transform.y, transform.z);
            break;
        case 'U':
            to((*submarine), transform.x, transform.y + 1.0f/SUB_SPEED, transform.z);
            break;
        case 'N':
            to((*submarine), transform.x, transform.y - 1.0f/SUB_SPEED, transform.z);
            break;
        case 'R':
            rty((*submarine), SUB_ROT);
            break;
        case 'L':
            rty((*submarine), -SUB_ROT);
            break;
        case 'E':
            /*if(buff != 999.0f) {
                buff = light.z;
                light.z = 999.0f;
            } else {
                light.z = buff;
            }*/
            break;
        case 'P':
            //fire artillery idk
            break;
    }
}
