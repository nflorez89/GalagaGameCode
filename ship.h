// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// ship.h v1.0

#ifndef _SHIP_H                 // Prevent multiple definitions if this 
#define _SHIP_H                 // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

#include "entity.h"
#include "constants.h"

namespace shipNS
{
    const int   WIDTH = 32;                 // image width (each frame)
    const int   HEIGHT = 32;                // image height
    const int   X = GAME_WIDTH/2 - WIDTH/2; // location on screen
    const int   Y = GAME_HEIGHT/6 - HEIGHT;
    const float ROTATION_RATE = (float)PI; // radians per second
    const float SPEED = 40000;
    const float MASS = 300.0f;              // mass
    enum DIRECTION {NONE, LEFT, RIGHT};     // rotation direction
    const int   TEXTURE_COLS = 8;           // texture has 8 columns
    const int   SHIP1_START_FRAME = 0;      // ship1 starts at frame 0
    const int   SHIP1_END_FRAME = 3;        // ship1 animation frames 0,1,2,3
    const int   ENEMY_SHIP_START_FRAME = 8;      // ship2 starts at frame 8
    const int   ENEMY_SHIP_END_FRAME = 11;       // ship2 animation frames 8,9,10,11
    const float SHIP_ANIMATION_DELAY = 0.2f;    // time between frames
    const int   EXPLOSION_START_FRAME = 32; // explosion start frame
    const int   EXPLOSION_END_FRAME = 39;   // explosion end frame
    const float EXPLOSION_ANIMATION_DELAY = 0.2f;   // time between frames
    const int   ENGINE_START_FRAME = 16;    // engine start frame
    const int   ENGINE_END_FRAME = 19;      // engine end frame
    const float ENGINE_ANIMATION_DELAY = 0.1f;  // time between frames
    const int   SHIELD_START_FRAME = 24;    // shield start frame
    const int   SHIELD_END_FRAME = 27;      // shield end frame
    const float SHIELD_ANIMATION_DELAY = 0.1f; // time between frames
    const float TORPEDO_DAMAGE = 46;        // amount of damage caused by torpedo
    const float SHIP_DAMAGE = 10;           // damage caused by collision with another ship
	const float FIRE_DELAY = 10.0f;      // 1/2 a second between torpedo firing
	const float MAX_INVINCIBILITY_TIME = 10.0f;
}

// inherits from Entity class
class Ship : public Entity
{
private:
    float   oldX, oldY, oldAngle;
    shipNS::DIRECTION direction;    // direction of rotation
    float   explosionTimer;
    bool    engineOn;               // true to move ship forward
	bool	forwardThrustOn;
	bool	leftThrustOn;
	bool	rightThrustOn;
	bool	backwardThrustOn;
    bool    shieldOn;
    Image   engine;
    Image   shield;

protected:
	bool	explosionOn;
	Image   explosion;
	float   rotation;               // current rotation rate (radians/second)
	boolean hasBlackHole;
	float invincibilityTimer;

public:
    // constructor
    Ship();

    // inherited member functions
    virtual void draw();
    virtual bool initialize(Game *gamePtr, int width, int height, int ncols,
                            TextureManager *textureM);

    // update ship position and angle
    virtual void update(float frameTime);

    // damage ship with WEAPON
    void damage(WEAPON);

    // new member functions
    
    // move ship out of collision
    void toOldPosition()            
    {
        spriteData.x = oldX; 
        spriteData.y = oldY, 
        spriteData.angle = oldAngle;
        rotation = 0.0f;
    }

    // Returns rotation
    float getRotation() {return rotation;}

    // Returns engineOn condition
    bool getEngineOn()  {return engineOn;}

    // Returns shieldOn condition
    bool getShieldOn()  {return shieldOn;}

	// get explosionOn
	bool getExplosionOn() {return explosionOn;}

	// get if explosion animation is complete or not
	bool getExplosionAnimComplete() {return explosion.getAnimationComplete();}

    // Sets engine on
    void setEngineOn(bool eng)  {engineOn = eng;}

	// Sets forward thrust on
	void setForwardThrustOn(bool thrust)  {forwardThrustOn = thrust;}

	// Sets left thrust on
	void setLeftThrustOn(bool thrust)  {leftThrustOn = thrust;}

	// Sets right thrust on
	void setRightThrustOn(bool thrust)  {rightThrustOn = thrust;}

	// Sets backward thrust on
	void setBackwardThrustOn(bool thrust)  {backwardThrustOn = thrust;}

    // Set shield on
    void setShieldOn(bool sh)   {shieldOn = sh;}

    // Sets Mass
    void setMass(float m)       {mass = m;}

    // Set rotation rate
    void setRotation(float r)   {rotation = r;}

	// Set explosionOn
	void setExplosionOn(bool exOn) {explosionOn = exOn;}

	// Set the explosion animation complete status
	void setExplosionAnimComplete(bool animComp) {explosion.setAnimationComplete(animComp);}

	// Set the explosion current frame
	void setExplosionCurrFrame(int c) {explosion.setCurrentFrame(c);}

	// Set the invincibility timer
	void setInvincibilityTimer(float invincTime) {invincibilityTimer = invincTime;}

	// update the explosion image
	void updateExplosion(float frameTime) {explosion.update(frameTime);}

    // direction of rotation force
    void rotate(shipNS::DIRECTION dir) {direction = dir;}

    // ship explodes
    void explode();

    // ship is repaired
    void repair();
};
#endif

