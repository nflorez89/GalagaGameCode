#ifndef _POWERUP_H               // Prevent multiple definitions if this 
#define _POWERUP_H               // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

#include <ctime>
#include <random>
#include "entity.h"
#include "game.h"

namespace powerUpNS
{
	const int   WIDTH = 32;                 // image width (each frame)
    const int   HEIGHT = 32;                // image height
    const int   Y = GAME_HEIGHT/6 - HEIGHT;
	const int   START_FRAME_INVINCIBILITY = 4;      // invincibility shield starts and ends at frame 4
    const int   END_FRAME_INVINCIBILITY = 4;
	const int   START_FRAME_BLACK_HOLE = 5;      // black hole starts and ends at frame 5
    const int   END_FRAME_BLACK_HOLE = 5;
	const int	TEXTURE_COLS = 8;
	const float SPEED = 50000;
	enum POWER_UP_TYPE {INVINCIBILITY, BLACK_HOLE};
}

class PowerUp : public Entity
{
private:
	float	yVelocity;
	powerUpNS::POWER_UP_TYPE powerUpType;

public:
    // Constructor
    PowerUp();
    // Destructor
    virtual ~PowerUp() {}

	// getter(s)
	virtual powerUpNS::POWER_UP_TYPE getType() {return powerUpType;}

	// Update PowerUp.
    // typically called once per frame
    // frameTime is used to regulate the speed of movement and animation
    virtual void update(float frameTime);

	// Initialize PowerUp
    // Pre: *gamePtr = pointer to Game object
    //      width = width of Image in pixels  (0 = use full texture width)
    //      height = height of Image in pixels (0 = use full texture height)
    //      ncols = number of columns in texture (1 to n) (0 same as 1)
    //      *textureM = pointer to TextureManager object
    virtual bool initialize(Game *gamePtr, int width, int height, int ncols,
                            TextureManager *textureM);
};

#endif