// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// spacewar.cpp Scrolling Bitmap Demo
// This class is the core of the game

#include "spaceWar.h"

//=============================================================================
// Constructor
//=============================================================================
Spacewar::Spacewar()
{
    menuOn = true;
    countDownOn = false;
    roundOver = false;
	myfile.open("spacewar.log");
}

//=============================================================================
// Destructor
//=============================================================================
Spacewar::~Spacewar()
{
    releaseAll();           // call onLostDevice() for every graphics item
	myfile.close();
}

//=============================================================================
// Initializes the game
// Throws GameError on error
//=============================================================================
void Spacewar::initialize(HWND hwnd)
{
    Game::initialize(hwnd); // throws GameError

    // initialize DirectX fonts
    fontBig.initialize(graphics, spacewarNS::FONT_BIG_SIZE, false, false, spacewarNS::FONT);
    fontBig.setFontColor(spacewarNS::FONT_COLOR);

    // menu texture
    if (!menuTexture.initialize(graphics,MENU_IMAGE))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing menu texture"));

    // space texture
    if (!spaceTexture.initialize(graphics,SPACE_IMAGE))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing space texture"));

	// planet texture
    if (!planetTexture.initialize(graphics, PLANET_IMAGE))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing planet texture"));

    // game textures
    if (!textures1.initialize(graphics,TEXTURES1_IMAGE))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing game textures1"));

	if (!textures2.initialize(graphics,TEXTURES2_IMAGE))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing game textures2"));

	// boss1 textures
	if(!boss1Texture.initialize(graphics, BOSS_IMAGE))
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing boss1 texture"));

    // menu image
    if (!menu.initialize(graphics,0,0,0,&menuTexture))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing menu"));

    // space image
    if (!space.initialize(graphics,0,0,0,&spaceTexture))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing space"));
    space.setScale((float)SPACE_SCALE);

	// planet image
	if (!planetSurface.initialize(graphics,0,0,0,&planetTexture))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing planet surface"));
    //planetSurface.setScale((float)SPACE_SCALE);

	// todo: please remove this after testing
	if(!powerUp.initialize(this,powerUpNS::WIDTH,powerUpNS::HEIGHT,powerUpNS::TEXTURE_COLS,&textures2))
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing asteroid"));

	// asteroid image. todo: remove this. For testing purposes only
	/*if(!asteroid.initialize(this,asteroidNS::WIDTH,asteroidNS::HEIGHT,asteroidNS::TEXTURE_COLS,&textures1))
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing asteroid"));
	asteroid.setFrames(asteroidNS::ASTEROID1_START_FRAME, asteroidNS::ASTEROID1_END_FRAME);
	asteroid.setCurrentFrame(asteroidNS::ASTEROID1_START_FRAME);*/


    // ship1
    if (!ship1.initialize(this,shipNS::WIDTH,shipNS::HEIGHT,shipNS::TEXTURE_COLS,&textures1))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing ship1"));

    ship1.setFrames(shipNS::SHIP1_START_FRAME, shipNS::SHIP1_END_FRAME);
    ship1.setCurrentFrame(shipNS::SHIP1_START_FRAME);
    ship1.setColorFilter(SETCOLOR_ARGB(255,230,230,255));   // light blue, used for shield and torpedo
    ship1.setMass(shipNS::MASS);
    // Start ship in center
    ship1.setX(GAME_WIDTH/2 - shipNS::WIDTH);
    ship1.setY(GAME_HEIGHT - shipNS::HEIGHT);
	ship1.setDegrees(270);

	fireTimer = 0;

	rushTimer = spacewarNS::RUSH_TIMER_MAX;

	////////////////////////////////////////
	// todo: for testing purposes
	/*if (!missile.initialize(this, missileNS::WIDTH, missileNS::HEIGHT, missileNS::TEXTURE_COLS, &textures1))
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing missile"));

	missile.setFrames(missileNS::START_FRAME, missileNS::END_FRAME);
	missile.setCurrentFrame(missileNS::START_FRAME);
    missile.setColorFilter(SETCOLOR_ARGB(255,230,230,255));   // light blue, used for shield and torpedo
    // Start missile at top
    missile.setX(GAME_WIDTH/2);
    missile.setY(GAME_HEIGHT/2);

	missile.setTarget(GAME_WIDTH/2, GAME_HEIGHT/2, &ship1);
	*/
	////////////////////////////////////////////

	// have this set to false, then to true when appropriate
	asteroidsOn = false;

	asteroidSpawnTimer = 0.0f;
	asteroidDuration = 0.0f;

	// keeps track of which obstacle we are on
	obstacleInd = 0;

	nextWaveReady = true;

	enemySpawnSide = spacewarNS::RIGHT;
	enemyMissileSpawnSide = spacewarNS::LEFT;

	shipSpawnXLoc = 0.0f;

	blackHoleTimer = spacewarNS::MAX_BLACK_HOLE_TIME;

	// todo: this is here for testing. Change it so it spawns the planet when necessary
	//initiatePlanet();

	initiateBlackHole();

	// todo: temporary. For proof of concept. Modify later
	// todo: comment back in. Commented out to test asteroid functionality
	//initiateEnemyShips(1/*16*/);

	// todo: temporary. For proof of concept. Modify later
	//initiateEnemyMissileShips(8);

	// todo: just for testing. Remove once randomization is in place
	//worldSequence.push_back(spacewarNS::ASTEROIDS);
	//worldSequence.push_back(spacewarNS::SHIPS);
	//worldSequence.push_back(spacewarNS::SHIPS);
	//worldSequence.push_back(spacewarNS::ASTEROIDS);
	//worldSequence.push_back(spacewarNS::PLANET);
	//worldSequence.push_back(spacewarNS::SHIPS);

	generator = std::default_random_engine(static_cast<unsigned int>(time(0)));
	distributionObstacle = std::uniform_int_distribution<int>(1, 100);
	distributionEnemySpawnLoc = std::uniform_int_distribution<int>(1,4);

	generateWorldSequence(10);

	currObstacle = worldSequence[obstacleInd];

	planetSurfaceMode = false;

}

//=============================================================================
// Start a new round of play
//=============================================================================
void Spacewar::roundStart()
{
    space.setX(0);
    space.setY(0);

    // Start ship in center
    ship1.setX(GAME_WIDTH/2 - shipNS::WIDTH);
    ship1.setY(GAME_HEIGHT - shipNS::HEIGHT);

    countDownTimer = spacewarNS::COUNT_DOWN;
    countDownOn = true;
    roundOver = false;
}

//=============================================================================
// Update all game items
//=============================================================================
void Spacewar::update()
{
    if (menuOn)
    {
        if (input->anyKeyPressed())
        {
            menuOn = false;
            input->clearAll();
            roundStart();
        }
    }
    else if(countDownOn)
    {
        countDownTimer -= frameTime;
        if(countDownTimer <= 0)
		{
            countDownOn = false;
		}
    }
    else // here check if planet flag is true or not. If not, do the below, else if it is then get next obstacle for planet
    {
		if(obstacleInd < worldSequence.size())
		{
			if(planetSurfaceMode)
			{
				if(planetObstacleInd < planetSequence.size())
				{
					if(currPlanetObstacle == spacewarNS::PLANET_SHIPS)
					{
						if(nextWaveReady)
						{
							initiateEnemyShips(16);
							nextWaveReady = false;
						}
						else if(enemyShips.empty())
						{
							// this obstacle is finished. Tell spacewar to move onto the next one
							planetObstacleInd++;
							if(planetObstacleInd < planetSequence.size())
							{
								currPlanetObstacle = planetSequence[planetObstacleInd];
							}
							nextWaveReady = true;
						}
					}
					// note: more planet obstacles put here later
				}
				else
				{
					// clean up any loose objects
					// note: may be more to clean up later
					deleteAllLasers();

					// planet traversing is done. Go back to space
					planetSurfaceMode = false;

					nextWaveReady = false;
				}
			}
			else
			{
				// check which new obstacle to create if any yet
				if (currObstacle == spacewarNS::SHIPS)
				{
					if(nextWaveReady)
					{
						initiateEnemyShips(16);
						initiateEnemyMissileShips(8);
						nextWaveReady = false;
					}
					else if(enemyShips.empty())
					{
						// if LEVELS_BEFORE_POWER_UP obstacles have been generated and the player has no power ups and there is no powere up pellet on the screen, 
						// then there is a chance to initiate a new power up
						if ((obstacleInd + 1) % spacewarNS::LEVELS_BEFORE_POWER_UP == 0 && !ship1.getShieldOn() && !blackHole.getActive() && !powerUp.getActive())
						{
							if(!powerUp.initialize(this,powerUpNS::WIDTH,powerUpNS::HEIGHT,powerUpNS::TEXTURE_COLS,&textures2))
								throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing asteroid"));
						}

						// this obstacle is finished. Tell spacewar to move onto the next one
						obstacleInd++;
						if(obstacleInd < worldSequence.size())
						{
							currObstacle = worldSequence[obstacleInd];
						}
						nextWaveReady = true;
					}
				}
				else if (currObstacle == spacewarNS::ASTEROIDS)
				{
					if(nextWaveReady)
					{
						asteroidsOn = true;
						nextWaveReady = false;
						asteroidDuration = spacewarNS::ASTEROID_TOTAL_DURATION;
					}
					else if(asteroidDuration <= 0.0f)
					{
						// if LEVELS_BEFORE_POWER_UP obstacles have been generated and the player has no power ups and there is no powere up pellet on the screen, 
						// then there is a chance to initiate a new power up
						if ((obstacleInd + 1) % spacewarNS::LEVELS_BEFORE_POWER_UP == 0 && !ship1.getShieldOn() && !blackHole.getActive() && !powerUp.getActive())
						{
							if(!powerUp.initialize(this,powerUpNS::WIDTH,powerUpNS::HEIGHT,powerUpNS::TEXTURE_COLS,&textures2))
								throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing asteroid"));
						}

						// this obstacle is finished. Tell spacewar to move onto the next one
						obstacleInd++;
						if(obstacleInd < worldSequence.size())
						{
							currObstacle = worldSequence[obstacleInd];
						}
						asteroidsOn = false;
						nextWaveReady = true;
					}
				}
				else if(currObstacle == spacewarNS::PLANET)
				{
					if(nextWaveReady)
					{
						initiatePlanet();
						nextWaveReady = false;
					}
					else if(!planet.getActive())
					{
						// if LEVELS_BEFORE_POWER_UP obstacles have been generated and the player has no power ups and there is no powere up pellet on the screen, 
						// then there is a chance to initiate a new power up
						if ((obstacleInd + 1) % spacewarNS::LEVELS_BEFORE_POWER_UP == 0 && !ship1.getShieldOn() && !blackHole.getActive() && !powerUp.getActive())
						{
							if(!powerUp.initialize(this,powerUpNS::WIDTH,powerUpNS::HEIGHT,powerUpNS::TEXTURE_COLS,&textures2))
								throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing asteroid"));
						}

						// this obstacle is finished. Tell spacewar to move onto the next one
						obstacleInd++;
						if(obstacleInd < worldSequence.size())
						{
							currObstacle = worldSequence[obstacleInd];
						}
						nextWaveReady = true;
					}
				}
			}
		}

		if(rushTimer <= 0.0f)
		{
			// rush timer should be reset
			rushTimer = spacewarNS::RUSH_TIMER_MAX;

			// set one of the ships' rush mode to true if any are eligible for this
			findAndSetRushReadyShip();
		}
		else
		{
			// time remaining until one of the enemy ships should rush the player
			rushTimer -= frameTime;
		}

		// set all the rushing enemy ship's direction with respect to the player's ship
		setAllEnemyShipRushDirection();

        if (ship1.getActive())
        {

			if(fireTimer >= 0)
			{
				fireTimer -= frameTime;                     // time remaining until fire enabled
			}

            if (input->isKeyDown(W_KEY) || input->getGamepadDPadUp(0))   // if thrust forward
            {
				ship1.setForwardThrustOn(true);
            }
			else
			{
				ship1.setForwardThrustOn(false);
			}

			if(input->isKeyDown(A_KEY) || input->getGamepadDPadLeft(0)) // if move left
			{
				ship1.setLeftThrustOn(true);
			}
			else
			{
				ship1.setLeftThrustOn(false);
			}

			if(input->isKeyDown(D_KEY) || input->getGamepadDPadRight(0)) // if move right
			{
				ship1.setRightThrustOn(true);
			}
			else
			{
				ship1.setRightThrustOn(false);
			}

			if(input->isKeyDown(S_KEY) || input->getGamepadDPadDown(0))
			{
				ship1.setBackwardThrustOn(true);
			}
            else
            {
				ship1.setBackwardThrustOn(false);
            }

			if(input->isKeyDown(SHIP1_FIRE_KEY2))
			{
				if(fireTimer <= 0.0f)                       // if ready to fire
				{
					Laser* newPlayerLaser = new Laser();

					initializeLaser(newPlayerLaser);

					newPlayerLaser->fire(&ship1);
					playerLasers.push_back(newPlayerLaser);
					fireTimer = spacewarNS::FIRE_DELAY;      // delay firing
				}
			}
        }

		//initialize asteroid if enough time has passed
		if(asteroidsOn)
		{
			asteroidSpawnTimer -= frameTime;
			if(asteroidSpawnTimer <= 0.0f)
			{
				initiateAnAsteroid();
				asteroidSpawnTimer = spacewarNS::ASTEROID_SPAWN_DELAY;
			}

			asteroidDuration -= frameTime;
		}

		// check if any of the enemy ships are ready to fire
		for(std::list<EnemyShip*>::iterator it=enemyShips.begin(); it != enemyShips.end(); /*++it*/)
		{
			// check which enemy ships have been destroyed or todo: left the screen
			if(!(*it)->getActive() && !(*it)->getVisible())
			{
				SAFE_DELETE(*it);
				it = enemyShips.erase(it);
			}
			// check which enemy ships are ready to fire
			else
			{
				if( (*it)->getAmmoReady() )
				{
					// todo: if statement to initialize based on weapon type
					if((*it)->getWeaponType() == 0)
					{
						// initiate a laser
						Laser* newEnemyLaser = new Laser();
						initializeLaser(newEnemyLaser, false);

						// fire the enemy laser
						newEnemyLaser->fire(*it, &ship1);

						// store this laser in the list so we can update it, along with the other lasers, later
						enemyProjectiles.push_back(newEnemyLaser);

						(*it)->setFireTimer(shipNS::FIRE_DELAY);
					}
					else if((*it)->getWeaponType() == 1)
					{
						// initiate a missile
						Missile* newEnemyMissile = new Missile();
						initializeMissile(newEnemyMissile);

						// fire the enemy missile
						newEnemyMissile->setTarget((*it)->getCenterX(), (*it)->getCenterY(), &ship1);

						// store this missile in the list so we can update it, along with the other missiles, later
						enemyProjectiles.push_back(newEnemyMissile);

						(*it)->setFireTimer(enemyMissileShipNS::FIRE_DELAY);
					}

					// reset the fire delay for this enemy ship
					(*it)->setAmmoReady(false);
				}
				++it;
			}
		}

		ship1.setVelocity(VECTOR2(0,0));

		ship1.gravityConstantForce(&planet, frameTime);

		// Update the entities
		ship1.update(frameTime);

		for(std::list<EnemyShip*>::iterator it=enemyShips.begin(); it != enemyShips.end(); ++it)
		{
			(*it)->gravityConstantForce(&blackHole, frameTime);
			(*it)->update(frameTime);
		}

		for(std::list<Laser*>::iterator it=playerLasers.begin(); it != playerLasers.end(); ++it)
		{
			(*it)->update(frameTime);
		}

		for(std::list<Entity*>::iterator it=enemyProjectiles.begin(); it != enemyProjectiles.end(); ++it)
		{
			(*it)->update(frameTime);
		}

		for(std::list<Asteroid*>::iterator it=asteroids.begin(); it != asteroids.end(); /*++it*/)
		{
			if((*it)->getY() > GAME_HEIGHT)
			{
				SAFE_DELETE(*it);
				it = asteroids.erase(it);
			}
			else
			{
				(*it)->update(frameTime);
				++it;
			}
		}

		planet.update(frameTime);

		if (blackHole.getActive())
		{
			blackHoleTimer -= frameTime;
			if(blackHoleTimer <= 0.0f)
			{
				blackHole.setActive(false);
				blackHole.setVisible(false);
				EnemyShip::setBlackHoleActive(false);
			}
		}
		blackHole.update(frameTime);
		powerUp.update(frameTime);

		if(planetSurfaceMode)
		{
			wrapImage(planetSurface, PLANET_SURFACE_WIDTH, PLANET_SURFACE_HEIGHT);
		}
		else
		{
			wrapImage(space, SPACE_WIDTH, SPACE_HEIGHT);
		}

		deleteDestroyedLasers();
		deleteOutOfBoundsShips();

        if(roundOver)
        {
            roundTimer -= frameTime;
            if(roundTimer <= 0)
                roundStart();
        }
    }
}

//=============================================================================
// Artificial Intelligence
//=============================================================================
void Spacewar::ai()
{}

//=============================================================================
// Handle collisions
//=============================================================================
void Spacewar::collisions()
{
	VECTOR2 collisionVector;
	powerUpNS::POWER_UP_TYPE powerType;

	// detect if the enemy's ship has been hit with a player's laser
	for(std::list<Laser*>::iterator itPlayerLaser=playerLasers.begin(); itPlayerLaser != playerLasers.end(); ++itPlayerLaser)
	{
		for(std::list<EnemyShip*>::iterator itEnemyShip=enemyShips.begin(); itEnemyShip != enemyShips.end(); ++itEnemyShip)
		{
			// if collision between a player laser and an enemy ship
			if((*itPlayerLaser)->collidesWith(**itEnemyShip, collisionVector))
			{
				(*itEnemyShip)->explode();
				(*itPlayerLaser)->setActive(false);

				// ??? maybe need this for optimization???: since this laser has been destroyed, break out of this loop to check with the next laser
				//break;
			}
		}

		for(std::list<Entity*>::iterator itEnemyProjectiles=enemyProjectiles.begin(); itEnemyProjectiles != enemyProjectiles.end(); ++itEnemyProjectiles)
		{
			if((*itPlayerLaser)->collidesWith(**itEnemyProjectiles, collisionVector) && (*itEnemyProjectiles)->getDestroyable())
			{
				(*itEnemyProjectiles)->setHealth((*itEnemyProjectiles)->getHealth() - 50.0f);
				(*itPlayerLaser)->setActive(false);

			}
		}
	}

	// detect if the player's ship has been hit with an enemy laser
	for(std::list<Entity*>::iterator itEnemyLaser=enemyProjectiles.begin(); itEnemyLaser != enemyProjectiles.end(); ++itEnemyLaser)
	{

		// if collision between ship1 and an enemy laser
		if((*itEnemyLaser)->collidesWith(ship1, collisionVector))
		{
			ship1.explode();
			(*itEnemyLaser)->setActive(false);

			// ??? maybe need this for optimization???: since this laser has been destroyed, break out of this loop to check with the next laser
			//break;
		}
	}

	// check if an enemy ship has collided with the player ship or if any enemy ships have collided with a black hole
	for(std::list<EnemyShip*>::iterator itEnemyShip=enemyShips.begin(); itEnemyShip != enemyShips.end(); ++itEnemyShip)
	{
		// if collision between player ship and enemy ship
		if((*itEnemyShip)->collidesWith(ship1, collisionVector))
		{
			ship1.explode();
			(*itEnemyShip)->explode();
		}

		// check if collision between enemy ship and black hole
		if((*itEnemyShip)->collidesWith(blackHole, collisionVector))
		{
			(*itEnemyShip)->explode();
		}
	}

	// check if an asteroid collided with the player ship
	for(std::list<Asteroid*>::iterator itAsteroids=asteroids.begin(); itAsteroids != asteroids.end(); ++itAsteroids)
	{
		// if collision between player ship and asteroids
		if((*itAsteroids)->collidesWith(ship1, collisionVector))
		{
			ship1.explode();
		}
	}

	// check for collisions between asteroids
	for(std::list<Asteroid*>::iterator itAsteroids=asteroids.begin(); itAsteroids != asteroids.end(); ++itAsteroids)
	{
		std::list<Asteroid*>::iterator itOtherAsteroids = itAsteroids;
		++itOtherAsteroids;

		for(; itOtherAsteroids != asteroids.end(); ++itOtherAsteroids)
		{
			if((*itAsteroids)->collidesWith(**itOtherAsteroids, collisionVector))
			{
				if((*itAsteroids)->bounce(collisionVector, **itOtherAsteroids))
				{
					(*itOtherAsteroids)->bounce(collisionVector*-1, **itAsteroids);
				}
			}
		}
	}

	// check if the player's ship is colliding with the planet
	if(ship1.collidesWith(planet, collisionVector))
	{
		// signal to transition to flying in planet's atmosphere
		planetSurfaceMode = true;

		planetObstacleInd = 0;

		// set the next wave to be ready, i.e. the signal that the first wave on the planet is ready to come out
		nextWaveReady = true;

		//move planet off screen and deactivate it
		planet.setY((float)GAME_HEIGHT);
		planet.setActive(false);

		// clean up any remaining objects before transitioning if there are any
		deleteAllLasers();
		deleteAllAsteroids();

		// generate planet's sequence of obstacles
		generatePlanetSequence(2); // todo: change value. 5 is just for testing for now.

		// position ship at middle bottom of screen to avoid enemies spawning on top of the ship
		ship1.setX((GAME_WIDTH/2.0f) - (ship1.getWidth()/2.0f));
		ship1.setY(GAME_HEIGHT*0.90f);

		currPlanetObstacle = planetSequence[planetObstacleInd];
	}

	// check if the player ship has collided with a power up pellet
	if (ship1.collidesWith(powerUp, collisionVector))
	{
		powerType = powerUp.getType();
		if(powerType == powerUpNS::INVINCIBILITY)
		{
			// invincibility shield
			ship1.setInvincibilityTimer(shipNS::MAX_INVINCIBILITY_TIME);
			ship1.setShieldOn(true);
		}
		else
		{
			// black hole
			blackHole.setActive(true);
			blackHole.setVisible(true);
			EnemyShip::setBlackHoleActive(true);
			blackHoleTimer = spacewarNS::MAX_BLACK_HOLE_TIME;
		}
		// deactivate power up pellet
		powerUp.setActive(false);
		powerUp.setVisible(false);
	}
}

//=============================================================================
// Render game items
//=============================================================================
void Spacewar::render()
{
    graphics->spriteBegin();                // begin drawing sprites

	if(planetSurfaceMode)
	{
		fillRestOfBackdrop(planetSurface, PLANET_SURFACE_WIDTH, PLANET_SURFACE_HEIGHT);
	}
	else
	{
		fillRestOfBackdrop(space, SPACE_WIDTH, SPACE_HEIGHT);
	}

	for(std::list<Laser*>::iterator it=playerLasers.begin(); it != playerLasers.end(); ++it)
	{
		(*it)->draw(graphicsNS::FILTER);      // draw the player lasers using colorFilter
	}

	for(std::list<Entity*>::iterator it=enemyProjectiles.begin(); it != enemyProjectiles.end(); ++it)
	{
		/*time_t rawtime;
		struct tm * timeinfo;
		char buffer [80];

		time (&rawtime);
		timeinfo = localtime (&rawtime);

		strftime (buffer,80,"%I:%M:%S%p.",timeinfo);

		myfile << buffer;
		myfile << " - drawing enemy projectile\n";
		*/
		(*it)->draw();      // draw the enemy projectiles using colorFilter
	}

	for(std::list<EnemyShip*>::iterator it=enemyShips.begin(); it != enemyShips.end(); ++it)
	{
		(*it)->draw();      // draw the enemy ships
	}

	for(std::list<Asteroid*>::iterator it=asteroids.begin(); it != asteroids.end(); ++it)
	{
		(*it)->draw();	// draw the asteroids
	}

	planet.draw();

	blackHole.draw();

	powerUp.draw();

	ship1.draw();       // draw the spaceship

    if(menuOn)
        menu.draw();
    if(countDownOn)
    {
        _snprintf_s(buffer, spacewarNS::BUF_SIZE, "%d", (int)(ceil(countDownTimer)));
        fontBig.print(buffer,spacewarNS::COUNT_DOWN_X,spacewarNS::COUNT_DOWN_Y);
    }

    graphics->spriteEnd();                  // end drawing sprites
}

//=============================================================================
// The graphics device was lost.
// Release all reserved video memory so graphics device may be reset.
//=============================================================================
void Spacewar::releaseAll()
{
    menuTexture.onLostDevice();
    spaceTexture.onLostDevice();
    textures1.onLostDevice();
    fontScore.onLostDevice();
    fontBig.onLostDevice();

	deleteAllLasers();
	deleteAllEnemyShips();
	deleteAllAsteroids();
	EnemyShip::deleteEnemyShipGrid();

    Game::releaseAll();
    return;
}

//=============================================================================
// The grahics device has been reset.
// Recreate all surfaces.
//=============================================================================
void Spacewar::resetAll()
{
    fontBig.onResetDevice();
    fontScore.onResetDevice();
    textures1.onResetDevice();
    spaceTexture.onResetDevice();
    menuTexture.onResetDevice();

    Game::resetAll();
    return;
}

//=============================================================================
// remove enemy lasers that leave the screen.
//=============================================================================
void Spacewar::deleteDestroyedLasers()
{
	float xPos, yPos;

	for(std::list<Laser*>::iterator it=playerLasers.begin(); it != playerLasers.end(); ++it)
	{
		xPos = (*it)->getX();
		yPos = (*it)->getY();
		if(xPos > GAME_WIDTH || yPos > GAME_HEIGHT || xPos < -laserNS::WIDTH || yPos < -laserNS::HEIGHT || !(*it)->getActive())
		{
			SAFE_DELETE(*it);
		}
	}

	// remove all the deleted lasers from the list
	playerLasers.remove(NULL);

	for(std::list<Entity*>::iterator it=enemyProjectiles.begin(); it != enemyProjectiles.end(); ++it)
	{
		xPos = (*it)->getX();
		yPos = (*it)->getY();
		if(xPos > GAME_WIDTH || yPos > GAME_HEIGHT || xPos < -laserNS::WIDTH || yPos < -laserNS::HEIGHT || !(*it)->getActive())
		{
			SAFE_DELETE(*it);
		}
	}

	// remove all the deleted lasers from the list
	enemyProjectiles.remove(NULL);
}

//=============================================================================
// remove enemy ships that leave the screen.
//=============================================================================
void Spacewar::deleteOutOfBoundsShips()
{
	float xPos, yPos;
	bool isInFormingFormation;

	for(std::list<EnemyShip*>::iterator it=enemyShips.begin(); it != enemyShips.end();)
	{
		xPos = (*it)->getX();
		yPos = (*it)->getY();
		isInFormingFormation = (*it)->getFormingFormationMode();

		// check if the ship is no longer in forming formation mode and if the ship is out of bounds
		if(!isInFormingFormation && (xPos > GAME_WIDTH || yPos > GAME_HEIGHT || xPos < -shipNS::WIDTH || yPos < -shipNS::HEIGHT))
		{
			SAFE_DELETE(*it);
			it = enemyShips.erase(it);
		}
		else
		{
			++it;
		}
	}
}

//=============================================================================
// Deletes all lasers from the playerLasers and enemyProjectiles lists.
//=============================================================================
void Spacewar::deleteAllLasers()
{
	for(std::list<Laser*>::iterator it=playerLasers.begin(); it != playerLasers.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	// remove all the deleted lasers from the list
	playerLasers.remove(NULL);

	for(std::list<Entity*>::iterator it=enemyProjectiles.begin(); it != enemyProjectiles.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	// remove all the deleted lasers from the list
	enemyProjectiles.remove(NULL);
}

//=============================================================================
// deactivate power up pellet so it is no longer updated on the screen (until it is reinitialized again)
//=============================================================================
void Spacewar::deleteOutOfBoundPowerUp()
{
	float xPos = powerUp.getX();
	float yPos = powerUp.getY();
	if (xPos > GAME_WIDTH || yPos > GAME_HEIGHT || xPos < -shipNS::WIDTH || yPos < -shipNS::HEIGHT)
	{
		// deactivate power up pellet
		powerUp.setActive(false);
		powerUp.setVisible(false);
	}
}

void Spacewar::deleteAllAsteroids()
{
	for(std::list<Asteroid*>::iterator it=asteroids.begin(); it != asteroids.end(); ++it)
	{
		SAFE_DELETE(*it);
	}
}

void Spacewar::initializeLaser(Laser* aLaser, bool isPlayers)
{
	if (!aLaser->initialize(this, laserNS::WIDTH, laserNS::HEIGHT, laserNS::TEXTURE_COLS, &textures1))
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing laser"));

	if ( isPlayers )
	{
		aLaser->setFrames(laserNS::START_FRAME, laserNS::END_FRAME);
		aLaser->setCurrentFrame(laserNS::START_FRAME);
	}
	else
	{
		aLaser->setFrames(laserNS::START_ENEMY_FRAME, laserNS::END_ENEMY_FRAME);
		aLaser->setCurrentFrame(laserNS::START_ENEMY_FRAME);
	}
	aLaser->setColorFilter(SETCOLOR_ARGB(255,128,128,255));   // light blue
}

//=============================================================================
// initializes enemy missiles
//=============================================================================
void Spacewar::initializeMissile(Missile* aMissile)
{
	if (!aMissile->initialize(this, missileNS::WIDTH, missileNS::HEIGHT, missileNS::TEXTURE_COLS, &textures2))
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing missile"));

	aMissile->setFrames(missileNS::START_FRAME, missileNS::END_FRAME);
	aMissile->setCurrentFrame(missileNS::START_FRAME);
    aMissile->setColorFilter(SETCOLOR_ARGB(255,230,230,255));   // light blue
}

//=============================================================================
// initializes multiple enemy ships
//=============================================================================
void Spacewar::initiateEnemyShips(short numShips)
{
	int probability;
	float xPos = 0;
	float yPos = 0;
	short maxNumEnemyShips = enemyShipNS::GRID_WIDTH*2;

	if(enemySpawnSide == spacewarNS::LEFT)
	{
		shipSpawnXLoc = -shipNS::WIDTH;
	}
	else
	{
		shipSpawnXLoc = GAME_WIDTH;
	}

	if(numShips > maxNumEnemyShips)
	{
		numShips = maxNumEnemyShips;
	}

	for (int i=0; i < numShips; i++)
	{
		// create a new ship
		EnemyShip* enemyShip = new EnemyShip();

		// initialize the new ship
		if (!enemyShip->initialize(this,shipNS::WIDTH,shipNS::HEIGHT,shipNS::TEXTURE_COLS,&textures1))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing an enemy ship"));

		enemyShip->setFrames(shipNS::ENEMY_SHIP_START_FRAME, shipNS::ENEMY_SHIP_END_FRAME);
		enemyShip->setCurrentFrame(shipNS::ENEMY_SHIP_START_FRAME);
		enemyShip->setColorFilter(SETCOLOR_ARGB(255,230,230,255));   // light blue, used for shield and torpedo
		enemyShip->setMass(shipNS::MASS);

		// find their spawn locations
		probability = distributionEnemySpawnLoc(generator);
		
		enemyShip->setRadians((float)PI/2.0f);
		enemyShip->setY(GAME_WIDTH/3.0f);

		if(enemySpawnSide == spacewarNS::LEFT)
		{
			// spawn the ship off screen to the left

			enemyShip->setX(shipSpawnXLoc);
			enemyShip->setSpawnMovementDirection(enemyShipNS::SPAWN_MOVING_RIGHT);
			shipSpawnXLoc -= shipNS::WIDTH;
		}
		else
		{
			// spawn the ship off screen to the right

			enemyShip->setX(shipSpawnXLoc);
			enemyShip->setSpawnMovementDirection(enemyShipNS::SPAWN_MOVING_LEFT);
			shipSpawnXLoc += shipNS::WIDTH;
		}

		// put ship in the enemyShips list
		enemyShips.push_back(enemyShip);
	}

	if(enemySpawnSide == spacewarNS::LEFT)
	{
		enemySpawnSide = spacewarNS::RIGHT;
	}
	else
	{
		enemySpawnSide = spacewarNS::LEFT;
	}
}

//=============================================================================
// initializes multiple enemy ships
//=============================================================================
void Spacewar::initiateEnemyMissileShips(short numShips)
{
	int probability;
	float xPos = 0;
	float yPos = 0;
	short maxNumEnemyShips = enemyShipNS::GRID_WIDTH;

	if(enemyMissileSpawnSide == spacewarNS::LEFT)
	{
		shipSpawnXLoc = -shipNS::WIDTH;
	}
	else
	{
		shipSpawnXLoc = GAME_WIDTH;
	}

	if(numShips > maxNumEnemyShips)
	{
		numShips = maxNumEnemyShips;
	}

	for (int i=0; i < numShips; i++)
	{
		// create a new ship
		EnemyShip* enemyShip = new EnemyMissileShip();

		// initialize the new ship
		if (!enemyShip->initialize(this,shipNS::WIDTH,shipNS::HEIGHT,shipNS::TEXTURE_COLS,&textures2))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing an enemy missile ship"));

		enemyShip->setFrames(shipNS::SHIP1_START_FRAME, shipNS::SHIP1_END_FRAME);
		enemyShip->setCurrentFrame(shipNS::SHIP1_START_FRAME);
		enemyShip->setColorFilter(SETCOLOR_ARGB(255,230,230,255));   // light blue, used for shield and torpedo
		enemyShip->setMass(shipNS::MASS);

		//enemyShip->setX((GAME_WIDTH/2.0f) + 100);
		//enemyShip->setY((GAME_HEIGHT/2.0f) + 100);
		enemyShip->setDegrees(90);
		enemyShip->setY(GAME_WIDTH/3.0f);

		if(enemyMissileSpawnSide == spacewarNS::LEFT)
		{
			// spawn the ship off screen to the left

			enemyShip->setX(shipSpawnXLoc);
			enemyShip->setSpawnMovementDirection(enemyShipNS::SPAWN_MOVING_RIGHT);
			shipSpawnXLoc -= shipNS::WIDTH;
		}
		else
		{
			// spawn the ship off screen to the right

			enemyShip->setX(shipSpawnXLoc);
			enemyShip->setSpawnMovementDirection(enemyShipNS::SPAWN_MOVING_LEFT);
			shipSpawnXLoc += shipNS::WIDTH;
		}

		// put ship in the enemyShips list
		enemyShips.push_back(enemyShip);

		xPos += shipNS::WIDTH;
	}

	if (enemyMissileSpawnSide == spacewarNS::LEFT)
	{
		enemyMissileSpawnSide = spacewarNS::RIGHT;
	}
	else
	{
		enemyMissileSpawnSide = spacewarNS::LEFT;
	}
}

//=============================================================================
// initializes one asteroid
//=============================================================================
void Spacewar::initiateAnAsteroid()
{
	Asteroid* anAsteroid = new Asteroid();
	if(!anAsteroid->initialize(this,asteroidNS::WIDTH,asteroidNS::HEIGHT,asteroidNS::TEXTURE_COLS,&textures1))
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing asteroid"));
	anAsteroid->setFrames(asteroidNS::ASTEROID1_START_FRAME, asteroidNS::ASTEROID1_END_FRAME);
	anAsteroid->setCurrentFrame(asteroidNS::ASTEROID1_START_FRAME);

	anAsteroid->setY(-(anAsteroid->getScale()*anAsteroid->getHeight()));

	// add to the asteroid list
	asteroids.push_back(anAsteroid);
}

//=============================================================================
// initializes one planet
//=============================================================================
void Spacewar::initiatePlanet()
{
	if(!planet.initialize(this,gravitationalBodyNS::WIDTH,gravitationalBodyNS::HEIGHT,gravitationalBodyNS::TEXTURE_COLS,&textures1))
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing planet"));
	planet.setFrames(gravitationalBodyNS::PLANET_START_FRAME, gravitationalBodyNS::PLANET_END_FRAME);
	planet.setCurrentFrame(gravitationalBodyNS::PLANET_START_FRAME);

	planet.setY(-(planet.getScale()*planet.getHeight()));

	planet.setActive(true);
}

//=============================================================================
// initializes one black hole
//=============================================================================
void Spacewar::initiateBlackHole()
{
	if(!blackHole.initialize(this,blackHoleNS::WIDTH,blackHoleNS::HEIGHT,blackHoleNS::TEXTURE_COLS,&textures1, spacewarNS::NATURAL_BLACK_HOLE_TOTAL_STAY_TIME))
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing black hole"));
	blackHole.setFrames(blackHoleNS::BLACK_HOLE_START_FRAME, blackHoleNS::BLACK_HOLE_END_FRAME);
	blackHole.setCurrentFrame(blackHoleNS::BLACK_HOLE_START_FRAME);
}

void Spacewar::deleteAllEnemyShips()
{
	for(std::list<EnemyShip*>::iterator it=enemyShips.begin(); it != enemyShips.end(); ++it)
	{
		SAFE_DELETE(*it);
	}
}

// fills up the worldSequence vector with spacewarNS::OBSTACLE numbers
void Spacewar::generateWorldSequence(short worldLength)
{
	// generate worldLength spacewarNS::OBSTACLE numbers for the worldSequence vector
	for (int i = 0; i < worldLength; i++)
	{
		// randomly choose a value from spacewarNS::OBSTACLE
		spacewarNS::OBSTACLE anObstacle = generateRandomObstacle();
		worldSequence.push_back(anObstacle);
	}
}

void Spacewar::generatePlanetSequence(short planetSeqLength)
{
	// generate worldLength spacewarNS::OBSTACLE numbers for the worldSequence vector
	for (int i = 0; i < planetSeqLength; i++)
	{
		// randomly choose a value from spacewarNS::PLANET_OBSTACLE
		spacewarNS::PLANET_OBSTACLE anObstacle = generateRandomPlanetObstacle();
		planetSequence.push_back(anObstacle);
	}
}

// generates a random number between 0 and 2
spacewarNS::OBSTACLE Spacewar::generateRandomObstacle()
{
	int probability = distributionObstacle(generator);

	if(probability > 10 && probability <= 20)
	{
		return spacewarNS::ASTEROIDS;
	}
	else if(probability >= 1 && probability <= 10)
	{
		return spacewarNS::PLANET;
	}

	return spacewarNS::SHIPS;
}

// stub
// generates a random number for planet obstacle
spacewarNS::PLANET_OBSTACLE Spacewar::generateRandomPlanetObstacle()
{
	int probability = distributionObstacle(generator);

	return spacewarNS::PLANET_SHIPS;
}

// wraps image around edges
void Spacewar::wrapImage(Image& image, const int width, const int height)
{
	// move space in Y direction
	image.setY(image.getY() + frameTime * BACKDROP_SPEED);

    // Wrap space image around at edge
    // if left edge of space > screen left edge
    if (image.getX() > 0)               
        // move space image left by SPACE_WIDTH
        image.setX(image.getX() - width);
    // if space image off screen left
    if (image.getX() < -width)
        // move space image right by SPACE_WIDTH
        image.setX(image.getX() + width);
    // if top edge of space > screen top edge
    if (image.getY() > 0)
        // move space image up by SPACE_HEIGHT
        image.setY(image.getY() - height);
    // if space image off screen top
    if (image.getY() < -height)
        // move space image down by SPACE_IMAGE
        image.setY(space.getY() + height);
}

// wraps image based on current image location
void Spacewar::fillRestOfBackdrop(Image& image, const int width, const int height)
{
	float x = image.getX();
    float y = image.getY();

    // Wrap space image around at edges
    image.draw();                           // draw at current location
    // if space image right edge visible
    if (image.getX() < -width + (int)GAME_WIDTH)
    {
        image.setX(image.getX() + width); // wrap around to left edge
        image.draw();                           // draw again
    }
    // if space image bottom edge visible
    if (image.getY() < -height + (int)GAME_HEIGHT)
    {
        image.setY(image.getY() + height); // wrap around to top edge
        image.draw();                           // draw again
        image.setX(x);                          // restore x position
        // if space image right edge visible
        // wrap around to left edge
        if (x < -width + (int)GAME_WIDTH)
            image.draw();                       // draw again
    }
    image.setX(x);      // restore x position
    image.setY(y);      // restore y position
}

//=============================================================================
// finds the first enemy ship that is not in forming formation mode and not in rush mode 
// and sets the rush mode of that ship to true. None of the ships rush modes are set to 
// true if none of the ships fit the previously stated criteria.
//=============================================================================
void Spacewar::findAndSetRushReadyShip()
{
	// find a ship that is already in formation but is not in rush mode
	for(std::list<EnemyShip*>::iterator it=enemyShips.begin(); it != enemyShips.end(); ++it)
	{
		if(!(*it)->getFormingFormationMode() && !(*it)->getRushMode())
		{
			// this ship is sitting in place in the grid and ready to start rushing the player
			(*it)->setRushMode(true);
			return;
		}
	}
}

//=============================================================================
// returns if the player's ship is to the right or left of a given enemy ship
//=============================================================================
enemyShipNS::RUSH_DIR Spacewar::getRushDirection(EnemyShip* anEnemyShip)
{
	float diffx = anEnemyShip->getCenterX() - ship1.getCenterX();

	if(diffx > 0.0f)
	{
		return enemyShipNS::LEFT;
	}
	else
	{
		return enemyShipNS::RIGHT;
	}
}

//=============================================================================
// set the enemy ship's direction with respect to the player's ship if the enemy
// ship is in rush mode.
//=============================================================================
void Spacewar::setAllEnemyShipRushDirection()
{
	EnemyShip* anEnemyShip;
	float rotation, xLoc;
	// for each enemy ship, if the ship is rush ready, set it's direction with respect to the player's ship
	for(std::list<EnemyShip*>::iterator it=enemyShips.begin(); it != enemyShips.end(); ++it)
	{
		anEnemyShip = *it;
		// check if this enemy ship is in rush mode
		if(anEnemyShip->getRushMode())
		{
			rotation = anEnemyShip->getRadians();
			xLoc = anEnemyShip->getCenterX();
			enemyShipNS::RUSH_DIR rushDir = getRushDirection(anEnemyShip);

			// check if the ship is ready to change direction
			if(anEnemyShip->getRushDir() == enemyShipNS::LEFT && xLoc <= (GAME_WIDTH/5.0f))
			{
				// if the ship is about to move off the left side of the screen, change it's direction
				anEnemyShip->setRushDir(enemyShipNS::RIGHT);
			}
			else if(anEnemyShip->getRushDir() == enemyShipNS::RIGHT && xLoc >= (GAME_WIDTH * (4.0f/5.0f)))
			{
				// if the ship is about to move off the right side of the screen, change it's direction
				anEnemyShip->setRushDir(enemyShipNS::LEFT);
			}
			else if(anEnemyShip->getRushDir() == enemyShipNS::NONE || rotation >= (float)PI || rotation <= 0.0f)
			{
				anEnemyShip->setRushDir(rushDir);
			}	
		}
	}
}
