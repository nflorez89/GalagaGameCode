// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// Version 1.0

#ifndef _LASER_H              // Prevent multiple definitions if this 
#define _LASER_H              // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

#include "entity.h"
#include "constants.h"

namespace laserNS
{
    const int   WIDTH = 32;             // image width
    const int   HEIGHT = 32 ;           // image height
    const int   COLLISION_RADIUS = 4;   // for circular collision
    const float SPEED = 600;            // pixels per second
    const float MASS = 300.0f;          // mass
    const int   TEXTURE_COLS = 8;       // texture has 8 columns
    const int   START_FRAME = 40;       // starts at frame 40
    const int   END_FRAME = 43;         // animation frames 40,41,42,43
	const int	START_ENEMY_FRAME = 40;	// starts at frame 16 for enemy lasers
	const int	END_ENEMY_FRAME = 43;	// ends at frame 19 for enemy lasers
    const float ANIMATION_DELAY = 0.1f; // time between frames
}

class Laser : public Entity           // inherits from Entity class
{
private:
public:
    // constructor
    Laser();

    // inherited member functions
    void update(float frameTime);
    float getMass()    const {return laserNS::MASS;}

    // new member functions
    void fire(Entity *ship);                // fire torpedo from ship
	void fire(Entity *sourceShip, Entity *destinationShip);
};
#endif