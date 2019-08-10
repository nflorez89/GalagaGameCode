#ifndef _ASTEROID_H               // Prevent multiple definitions if this 
#define _ASTEROID_H               // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

class Asteroid;

#include <ctime>
#include <random>
#include "entity.h"
#include "input.h"
#include "game.h"

namespace asteroidNS
{
	const int   WIDTH = 64;                 // image width (each frame)
    const int   HEIGHT = 64;                // image height
	const float	LARGE_ROTATION_FACTOR = 20.0;
	const float	MEDIUM_ROTATION_FACTOR = 50.0;
	const float	SMALL_ROTATION_FACTOR = 100.0;
	const int   X = GAME_WIDTH/2 - WIDTH/2; // location on screen
    const int   Y = GAME_HEIGHT/6 - HEIGHT;
	const int   ASTEROID1_START_FRAME = 15;      // ship1 starts at frame 0
    const int   ASTEROID1_END_FRAME = 15;        // ship1 animation frames 0,1,2,3
	const int	TEXTURE_COLS = 4;
	const float SMALL_SPEED = 30;
	const float MEDIUM_SPEED = 20;
	const float LARGE_SPEED = 10;
}

enum DIRECTION {LEFT = -1, RIGHT = 1};     // rotation direction

class Asteroid : public Entity
{
private:
	static float lastXLocation;
	float   rotation;               // current rotation rate (radians/second)
	float	rotationRate;
	float	yVelocity;
	DIRECTION rotationDirection;

public:
    // Constructor
    Asteroid();
    // Destructor
    virtual ~Asteroid() {}

	// Update Asteroid.
    // typically called once per frame
    // frameTime is used to regulate the speed of movement and animation
    virtual void update(float frameTime);

	// Initialize Asteroid
    // Pre: *gamePtr = pointer to Game object
    //      width = width of Image in pixels  (0 = use full texture width)
    //      height = height of Image in pixels (0 = use full texture height)
    //      ncols = number of columns in texture (1 to n) (0 same as 1)
    //      *textureM = pointer to TextureManager object
    virtual bool initialize(Game *gamePtr, int width, int height, int ncols,
                            TextureManager *textureM);
};

#endif