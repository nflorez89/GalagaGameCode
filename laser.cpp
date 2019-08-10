// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// torpedo.cpp v1.0

#include <math.h>
#include "laser.h"

//=============================================================================
// default constructor
//=============================================================================
Laser::Laser() : Entity()
{
    active = false;                                 // torpedo starts inactive
    spriteData.width        = laserNS::WIDTH;     // size of 1 image
    spriteData.height       = laserNS::HEIGHT;
    spriteData.rect.bottom  = laserNS::HEIGHT;    // rectangle to select parts of an image
    spriteData.rect.right   = laserNS::WIDTH;
    cols            = laserNS::TEXTURE_COLS;
    frameDelay       = laserNS::ANIMATION_DELAY;
    startFrame      = laserNS::START_FRAME;       // first frame of ship animation
    endFrame        = laserNS::END_FRAME;         // last frame of ship animation
    currentFrame    = startFrame;
    radius          = laserNS::COLLISION_RADIUS;  // for circular collision
    visible         = false;
    mass = laserNS::MASS;
    collisionType = entityNS::CIRCLE;
	isDestroyable = false;
}

//=============================================================================
// update
// typically called once per frame
// frameTime is used to regulate the speed of movement and animation
//=============================================================================
void Laser::update(float frameTime)
{
    if (visible == false)
        return;

    Image::update(frameTime);

    spriteData.x += frameTime * velocity.x;     // move along X 
    spriteData.y += frameTime * velocity.y;     // move along Y

    // Wrap around screen edge
    /*if (spriteData.x > GAME_WIDTH)              // if off right screen edge
        spriteData.x = -laserNS::WIDTH;       // position off left screen edge
    else if (spriteData.x < -laserNS::WIDTH)  // else if off left screen edge
        spriteData.x = GAME_WIDTH;              // position off right screen edge
    if (spriteData.y > GAME_HEIGHT)             // if off bottom screen edge
        spriteData.y = -laserNS::HEIGHT;      // position off top screen edge
    else if (spriteData.y < -laserNS::HEIGHT) // else if off top screen edge
        spriteData.y = GAME_HEIGHT;             // position off bottom screen edge
		*/
}

//=============================================================================
// fire
// Fires a laser from ship
//=============================================================================
void Laser::fire(Entity *ship)
{
    velocity.y = (float)sin(ship->getRadians()) * laserNS::SPEED;
    spriteData.x = ship->getCenterX() - spriteData.width/2;
    spriteData.y = ship->getCenterY() - spriteData.height/2;
    visible = true;                         // make torpedo visible
    active = true;                          // enable collisions
    //audio->playCue(TORPEDO_FIRE);
}

//=============================================================================
// fire
// Fires a laser from source ship towards the destination ship's current location
//=============================================================================
void Laser::fire(Entity *sourceShip, Entity *destinationShip)
{
	// get the x and y of the right triangle
	float adjacent = sourceShip->getCenterX() - destinationShip->getCenterX();
	float opposite = sourceShip->getCenterY() - destinationShip->getCenterY();

	// calculate the hypotenuse
	float hypotenuse = sqrt((adjacent * adjacent) + (opposite * opposite));

	// calculate the velocity
	velocity.x = (float)((-adjacent)/hypotenuse) * 20/*laserNS::SPEED*/;
	velocity.y = (float)((-opposite)/hypotenuse) * 20/*laserNS::SPEED*/;

	spriteData.x = sourceShip->getCenterX() - (spriteData.width/2);
	spriteData.y = sourceShip->getCenterY() - (spriteData.height/2);
	visible = true;
	active = true;
	//audio->playCue(TORPEDO_FIRE);
}