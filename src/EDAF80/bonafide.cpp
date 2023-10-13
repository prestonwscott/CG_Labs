//
//  bonafide.cpp
//  CG_Labs
//
//  Created by Preston Scott on 9/10/23.
//

#include "bonafide.hpp"


bonafide::bonafide(std::vector<Node> nod, FPSCameraf* cam, long long* time){
    nodes = nod;
    camera = cam;
    clock = *time;
}

void
bonafide::gameframe()
{
    
    animation();
    
    
}

