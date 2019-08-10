#include <math.h>
#include "missile.h"

//=============================================================================
// default constructor
//=============================================================================
Missile::Missile() : Entity()
{
	active = false;                                 // torpedo starts inactive
    spriteData.width        = missileNS::WIDTH;     // size of 1 image
    spriteData.height       = missileNS::HEIGHT;
    spriteData.rect.bottom  = missileNS::HEIGHT;    // rectangle to select parts of an image
    spriteData.rect.right   = missileNS::WIDTH;
    cols            = missileNS::TEXTURE_COLS;
    frameDelay       = missileNS::ANIMATION_DELAY;
    startFrame      = missileNS::START_FRAME;       // first frame of ship animation
    endFrame        = missileNS::END_FRAME;         // last frame of ship animation
    currentFrame    = startFrame;
    radius          = missileNS::COLLISION_RADIUS;  // for circular collision
    visible         = false;
    collisionType = entityNS::CIRCLE;
	rotation = 0.0f;
	setRadians(0.0f);
	setHealth(100.0f);
	isDestroyable = true;
	explosionOn = false;
	//myfile.open ("log.txt");
}

Missile::~Missile()
{
	//myfile.close();
}

//=============================================================================
// Initialize the missile.
// Post: returns true if successful, false if failed
//=============================================================================
bool Missile::initialize(Game *gamePtr, int width, int height, int ncols,
                            TextureManager *textureM)
{
	explosion.initialize(gamePtr->getGraphics(), width, height, ncols, textureM);
    explosion.setFrames(missileNS::EXPLOSION_START_FRAME, missileNS::EXPLOSION_END_FRAME);
    explosion.setCurrentFrame(missileNS::EXPLOSION_START_FRAME);
    explosion.setFrameDelay(missileNS::EXPLOSION_ANIMATION_DELAY);
    explosion.setLoop(false);               // do not loop animation

	return(Entity::initialize(gamePtr, width, height, ncols, textureM));
}

//=============================================================================
// update
// typically called once per frame
// frameTime is used to regulate the speed of movement and animation
//=============================================================================
void Missile::update(float frameTime)
{
	//time (&rawtime);
	//timeinfo = localtime (&rawtime);
	//strftime (buffer,80,"%I:%M:%S%p.",timeinfo);

	//myfile << buffer;
	//myfile << " - Calling Missile::update\n";

    if (visible == false)
	{
		return;
	}

	if(health <= 0.0f && explosionOn == false)
	{
		//myfile << "Explode the missile\n";
		explode();
    }

	if(explosionOn)
	{
		//myfile << "update the explosion\n";
		explosion.update(frameTime);
        if(explosion.getAnimationComplete())    // if explosion animation complete
        {
			//myfile << "explosion animation complete\n";
			explosionOn = false;                // turn off explosion
            visible = false;
			active = false;
            explosion.setAnimationComplete(false);
            explosion.setCurrentFrame(missileNS::EXPLOSION_START_FRAME);
        }
	}

	Entity::update(frameTime);

	if(explosionOn)
	{
		return;
	}

	// get the adjacent and opposite sides of the right triangle
	float diffx = destinationEntity->getCenterX() - getCenterX();
	float diffy = destinationEntity->getCenterY() - getCenterY();
	float diffAngle1 = 0.0;
	float diffAngle2 = 0.0;
	short direction = 0;

	// set the velocity based on the direction the missile is facing
	velocity.x = (float)cos(spriteData.angle) * missileNS::SPEED * frameTime;
	velocity.y = (float)sin(spriteData.angle) * missileNS::SPEED * frameTime;

	//get angle of the hypotenuse
	float hypAngleRads = atan2(-diffy, diffx);

	if (hypAngleRads < 0)
	{
		hypAngleRads = abs(hypAngleRads);
	}
	else
	{
		hypAngleRads = 2*(float)PI - hypAngleRads;
	}

	// get the angle difference between where the missile is facing and the angle of the ship to the missile
	diffAngle1 = spriteData.angle - hypAngleRads;

	if (diffAngle1 < 0)
	{
		diffAngle2 = 2*(float)PI + diffAngle1;
	}
	else
	{
		diffAngle2 = 2*(float)PI - diffAngle1;
	}

	// check which direction the missile should be turned
	if (abs(diffAngle1) < abs(diffAngle2))
	{

		if (diffAngle1 < 0)
		{
			// rotate clockwise
			direction = 1;
		}
		else
		{
			// rotate counterclockwise
			direction = -1;
		}
	}
	else
	{

		// for special case
		float angleFromTwoPI = 2.0f*(float)PI - spriteData.angle;

		if (diffAngle2 < 0 || (spriteData.angle >= PI && spriteData.angle <= 2*PI && hypAngleRads >= 0 && hypAngleRads <= PI && angleFromTwoPI + hypAngleRads <= PI))
		{
			// rotate clockwise
			direction = 1;
		}
		else
		{
			// rotate counterclockwise
			direction = -1;
		}
	}

	float maxRotation = frameTime * missileNS::ROTATION_RATE;

	rotation += direction * maxRotation;

	if(rotation > 2*PI)
	{
		rotation -= 2*(float)PI;
	}
	else if(rotation < 0)
	{
		rotation += 2*(float)PI;
	}

	setRadians(rotation);

	spriteData.x += frameTime * velocity.x;     // move ship along X 
    spriteData.y += frameTime * velocity.y;     // move ship along Y
}

//=============================================================================
// draw the ship
//=============================================================================
void Missile::draw(COLOR_ARGB color)
{
	//myfile << "Drawing!!!!!\n";
    if(explosionOn)
	{
		//myfile << "Drawing Explosion.\n";
        explosion.draw(spriteData, color); // draw explosion using current spriteData
	}
    else
    {
		//myfile << "Drawing missile.\n";
        Image::draw(color);              // draw ship
	}
}

//=============================================================================
// explode
//=============================================================================
void Missile::explode()
{
	//audio->playCue(EXPLODE);
    explosionOn = true;
    velocity.x = 0.0f;
    velocity.y = 0.0f;
}

//=============================================================================
// fire
// Fires a missile from source ship towards the destination ship's current location
//=============================================================================
void Missile::setTarget(float xLoc, float yLoc, Entity *destEntity)
{	
	//myfile << "Setting the target.\n";
	destinationEntity = destEntity;

	setX(xLoc);
	setY(yLoc);
	
	visible = true;
	active = true;
	//audio->playCue(TORPEDO_FIRE);
}

