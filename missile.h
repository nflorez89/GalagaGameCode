#include <iostream>
#include <fstream>
#include <time.h>
#include "entity.h"
#include "constants.h"
using namespace std;

#ifndef _MISSILE_H              // Prevent multiple definitions if this 
#define _MISSILE_H              // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

namespace missileNS
{
	const int	WIDTH = 32;
	const int	HEIGHT = 32;
	const float SPEED = 10000;            // pixels per second
	const int   TEXTURE_COLS = 8;       // texture has 8 columns
    const int   START_FRAME = 6;       // starts at frame 8
    const int   END_FRAME = 6;
	const int   EXPLOSION_START_FRAME = 32; // explosion start frame
    const int   EXPLOSION_END_FRAME = 39;   // explosion end frame
    const float EXPLOSION_ANIMATION_DELAY = 0.2f;   // time between frames
	const float ANIMATION_DELAY = 0.1f; // time between frames
	const int   COLLISION_RADIUS = 16;   // for circular collision
	const float ROTATION_RATE = (float)PI/2.0f; // radians per second
}

class Missile : public Entity
{
private:
	Entity* destinationEntity;
	float   rotation;               // current rotation rate (radians/second)
	bool	explosionOn;
	Image   explosion;
	ofstream myfile;

	time_t rawtime;
	struct tm * timeinfo;
	char buffer [80];
public:
	// constructor
	Missile();

	// destructor
	~Missile();

	// initializer
	virtual bool initialize(Game *gamePtr, int width, int height, int ncols,
                            TextureManager *textureM);

	// inherited member functions
    virtual void update(float frameTime);
	
	virtual void draw(COLOR_ARGB color = graphicsNS::WHITE);

	// new member functions
	void explode();
	void setTarget(float xLoc, float yLoc, Entity* destEntity);

};

#endif