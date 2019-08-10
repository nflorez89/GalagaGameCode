#ifndef _BLACKHOLE_H               // Prevent multiple definitions if this 
#define _BLACKHOLE_H               // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

#include <ctime>
#include <random>
#include "entity.h"
#include "input.h"
#include "game.h"

namespace blackHoleNS
{
	const int   WIDTH = 32;                 // image width (each frame)
    const int   HEIGHT = 32;                // image height
	const int   X = GAME_WIDTH/2 - WIDTH/2; // location on screen
    const int   Y = GAME_HEIGHT/6 - HEIGHT;
	const int	BLACK_HOLE_START_FRAME = 24;
	const int	BLACK_HOLE_END_FRAME = 27;
	const int	TEXTURE_COLS = 8;
}

class BlackHole : public Entity
{
private:
	float	yVelocity;
	float	stayTime;
	float	totalStayTime;

public:
    // Constructor
    BlackHole();
    // Destructor
    virtual ~BlackHole() {}

	// Update black hole.
    // typically called once per frame
    // frameTime is used to regulate the speed of movement and animation
    virtual void update(float frameTime);

	// Initialize black hole
    // Pre: *gamePtr = pointer to Game object
    //      width = width of Image in pixels  (0 = use full texture width)
    //      height = height of Image in pixels (0 = use full texture height)
    //      ncols = number of columns in texture (1 to n) (0 same as 1)
    //      *textureM = pointer to TextureManager object
    virtual bool initialize(Game *gamePtr, int width, int height, int ncols,
                            TextureManager *textureM, float duration);
};

#endif