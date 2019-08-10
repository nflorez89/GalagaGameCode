#ifndef _GRAVITATIONALBODY_H               // Prevent multiple definitions if this 
#define _GRAVITATIONALBODY_H               // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

#include <ctime>
#include <random>
#include "entity.h"
#include "input.h"
#include "game.h"

namespace gravitationalBodyNS
{
	const int   WIDTH = 128;                 // image width (each frame)
    const int   HEIGHT = 128;                // image height
	const int   X = GAME_WIDTH/2 - WIDTH/2; // location on screen
    const int   Y = GAME_HEIGHT/6 - HEIGHT;
	const int   PLANET_START_FRAME = 1;      // ship1 starts at frame 0
    const int   PLANET_END_FRAME = 1;        // ship1 animation frames 0,1,2,3
	const int	TEXTURE_COLS = 2;
	const float VERTICAL_SPEED = 30;
}

class GravitationalBody : public Entity
{
private:
	static float lastXLocation;
	float	yVelocity;

public:
    // Constructor
    GravitationalBody();
    // Destructor
    virtual ~GravitationalBody() {}

	// Update Planet.
    // typically called once per frame
    // frameTime is used to regulate the speed of movement and animation
    virtual void update(float frameTime);

	// Initialize Planet
    // Pre: *gamePtr = pointer to Game object
    //      width = width of Image in pixels  (0 = use full texture width)
    //      height = height of Image in pixels (0 = use full texture height)
    //      ncols = number of columns in texture (1 to n) (0 same as 1)
    //      *textureM = pointer to TextureManager object
    virtual bool initialize(Game *gamePtr, int width, int height, int ncols,
                            TextureManager *textureM);
};

#endif