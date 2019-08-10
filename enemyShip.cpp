#include "enemyShip.h"
#include "laser.h"

bool EnemyShip::blackHoleActive = false;
std::array<EnemyShipCell*, enemyShipNS::GRID_WIDTH*enemyShipNS::GRID_HEIGHT> EnemyShip::s_enemyShipGrid = EnemyShip::initEnemyShipGridArray();

//=============================================================================
// default constructor
//=============================================================================
EnemyShip::EnemyShip() : Ship(), weaponType(enemyShipNS::LASER)
{
	fireTimer = 0;
	ammoReady = false;
	formingFormationMode = true;
	myCell = NULL;
	velocity.x = 0.0f;
	velocity.y = 0.0f;
	rushMode = false;
	rushDirection = enemyShipNS::NONE;
	rotation = 0/*(float)PI/2.0f*/;
	loopRotation = 0.0f;
	loopPhase = enemyShipNS::PRIOR_TO_LOOP;
	loopX = 0.0f;
	loopY = 0.0f;
	loopCenterX = 0.0f;
	loopCenterY = 0.0f;
}

//=============================================================================
// destructor
//=============================================================================
EnemyShip::~EnemyShip()
{
	if(myCell)
	{
		myCell->isOccupied = false;
	}
}

//=============================================================================
// Initialize the grid array.
//=============================================================================
std::array<EnemyShipCell*, enemyShipNS::GRID_WIDTH*enemyShipNS::GRID_HEIGHT> EnemyShip::initEnemyShipGridArray()
{
	// get size of the array
	short size = EnemyShip::s_enemyShipGrid.size();

	// get the cell width and height
	short cellWidth = GAME_WIDTH/enemyShipNS::GRID_WIDTH;
	short cellHeight = cellWidth/2;

	// keep track of y coord
	float cellTopCoord = 0.0f;

	int indexMod = 0;

	EnemyShipCell* currCell;

	// create a new cell struct and initiate each
	for(int i=0; i < size; i++)
	{
		EnemyShip::s_enemyShipGrid[i] = new EnemyShipCell;
		currCell = EnemyShip::s_enemyShipGrid[i];
		SecureZeroMemory(currCell, sizeof(currCell));

		indexMod = (i+1) % enemyShipNS::GRID_WIDTH;

		// initialize the values
		currCell->cellRect.top = (long)cellTopCoord;
		currCell->cellRect.bottom = (long)(cellTopCoord + cellHeight);
		currCell->isOccupied = false;

		if (indexMod == 0)
		{
			currCell->cellRect.right = GAME_WIDTH;
			cellTopCoord += cellHeight;
		}
		else
		{
			currCell->cellRect.right = (indexMod * cellWidth);
		}

		currCell->cellRect.left = currCell->cellRect.right - cellWidth;
	}

	return EnemyShip::s_enemyShipGrid;
}

//=============================================================================
// update
// typically called once per frame
// frameTime is used to regulate the speed of movement and animation
//=============================================================================
void EnemyShip::update(float frameTime)
{
	float newLoopX = 0.0f;
	float newLoopY = 0.0f;

    if(explosionOn)
    {
        explosion.update(frameTime);
        if(explosion.getAnimationComplete())    // if explosion animation complete
        {
            explosionOn = false;                // turn off explosion
            visible = false;
            explosion.setAnimationComplete(false);
            explosion.setCurrentFrame(shipNS::EXPLOSION_START_FRAME);
        }
    }

	if(fireTimer >= 0)
	{
		fireTimer -= frameTime;                     // time remaining until fire enabled
	}

	if(fireTimer <= 0.0f)                       // if ready to fire
	{
		ammoReady = true;
		//fireTimer = shipNS::FIRE_DELAY;      // delay firing
	}

	if(!EnemyShip::blackHoleActive)
	{
		if(formingFormationMode)
		{
			if(myCell == NULL)
			{
				if(loopPhase == enemyShipNS::PRIOR_TO_LOOP)
				{
					if(getCenterX() > (float)GAME_WIDTH/3.0f && getCenterX() < (float)GAME_WIDTH * (2.0f/3.0f))
					{
						if(velocity.x < 0)
						{
							// set loop clockwise
							loopPhase = enemyShipNS::CLOCKWISE;
						}
						else
						{
							loopPhase = enemyShipNS::COUNTERCLOCKWISE;
						}

						// get the x and y of the right triangle
						loopX = 0.0f;
						loopY = enemyShipNS::LOOP_RADIUS;

						// set the center of the loop circle for this ship (directly above where the ship is now)
						loopCenterX = getCenterX();
						loopCenterY = getCenterY() - loopY;
					}
					else
					{
						// this is when the ship is still moving towards the middle of the screen
						if(spawnMovementDir == enemyShipNS::SPAWN_MOVING_LEFT)
						{
							velocity.x = -enemyShipNS::SPEED * frameTime;
						}
						else
						{
							velocity.x = enemyShipNS::SPEED * frameTime;
						}
					}
				}

				if(loopPhase == enemyShipNS::CLOCKWISE || loopPhase == enemyShipNS::COUNTERCLOCKWISE)
				{
					// increment/decrement the angle depending on whether the loop is clockwise or counter clockwise
					if(loopPhase == enemyShipNS::CLOCKWISE)
					{
						rotation = frameTime * enemyShipNS::LOOP_ROTATION_RATE;
					}
					else if(loopPhase == enemyShipNS::COUNTERCLOCKWISE)
					{
						rotation = -frameTime * enemyShipNS::LOOP_ROTATION_RATE;
					}

					// increment/decrement the loop rotation
					loopRotation += rotation;

					// calculate the x and y of the right triangle using the rotation matrix
					newLoopX = (loopX * cos(rotation)) - (loopY * sin(rotation));
					newLoopY = (loopX * sin(rotation)) + (loopY * cos(rotation));

					spriteData.x = loopCenterX + newLoopX - ((float)spriteData.width/2*getScale());
					spriteData.y = loopCenterY + newLoopY - ((float)spriteData.height/2*getScale());

					loopX = newLoopX;
					loopY = newLoopY;

					// set the velocity to 0 since we are moving the ship directly with the sprite's x and y values for looping around a point
					velocity.x = 0.0f;
					velocity.y = 0.0f;
				}
			
				if(loopPhase != enemyShipNS::LOOP_COMPLETE)
				{
					if(loopRotation >= 2.5f*(float)PI || loopRotation <= -2.5f*(float)PI)
					{
						loopPhase = enemyShipNS::LOOP_COMPLETE;
						rotation = (float)PI/2.0f;
					}
				}
				else
				{
					RECT myRect;
					// check each cell in the grid until we find an unnoccupied one and set my cell to this new one
					myCell = findVacantCell();

					// get the cell's center
					myRect = myCell->cellRect;

					/////////////////////
					//velocity.x = distanceX*frameTime;
					//velocity.y = distanceY*frameTime;

					// call 'dibs' on this cell, i.e. set this cell to occupied
					myCell->isOccupied = true;
				}
			}
		
			// check if the ship has reached it's cell yet
			//if(myCell && getCenterX() < myCell->cellRect.right && getCenterX() > myCell->cellRect.left && getCenterY() < myCell->cellRect.bottom && getCenterY() > myCell->cellRect.top)
			if(myCell != NULL)
			{
				
				RECT myRect = myCell->cellRect;

				calcCellPathVelocity(myRect, frameTime);

				if(isHigherThanRect(myRect))
				{
					formingFormationMode = false;
					
					snapShipToCell(myRect);
					rotation = (float)PI/2.0f;
					loopPhase = enemyShipNS::PARKED;
				}
			}
		}
		else
		{
			// check if the ship is being moved by the black hole that no longer exists. If so, have it go back to it's original position
			if(!rushMode)
			{
				RECT rect = myCell->cellRect;
				// check if the ship is where it should be
				if(isHigherThanRect(rect))
				{
					snapShipToCell(rect);

					// change ship status back to PARKED
					loopPhase = enemyShipNS::PARKED;
				}
				else
				{
					// recalculate the velocity so the ship goes towards it's cell location again
					calcCellPathVelocity(myCell->cellRect, frameTime);
				}

			}
			else if(rushMode)
			{
				// the ship will fly off the screen, so abandon this cell to free it up for another ship
				if(myCell)
				{
					myCell->isOccupied = false;
					myCell = NULL;
				}

				// rushing logic
				// check which direction the ship should rotate in
				switch (rushDirection)
				{
				case enemyShipNS::LEFT:
					rotation += frameTime * enemyShipNS::ROTATION_RATE;
					if(rotation > enemyShipNS::MAX_RUSH_ROTATION_RAD)
					{
						rotation = enemyShipNS::MAX_RUSH_ROTATION_RAD;
					}
					setRadians(rotation);
					break;
				case enemyShipNS::RIGHT:
					rotation -= frameTime * enemyShipNS::ROTATION_RATE;
					if(rotation < enemyShipNS::MIN_RUSH_ROTATION_RAD)
					{
						rotation = enemyShipNS::MIN_RUSH_ROTATION_RAD;
					}
					setRadians(rotation);
					break;
				}

				// set the velocity based on the direction the missile is facing
				velocity.x = (float)cos(spriteData.angle) * enemyShipNS::RUSH_SPEED_X * frameTime;
				velocity.y = (float)sin(spriteData.angle) * enemyShipNS::SPEED * frameTime;
			}
		}
	}
	else if(loopPhase == enemyShipNS::CLOCKWISE || loopPhase == enemyShipNS::COUNTERCLOCKWISE)
	{
		// just set the loop phase as if the loop completed, otherwise the ship will snap back to it's next position in the original loop which we don't want
		loopPhase = enemyShipNS::LOOP_COMPLETE;
	}
	/*else
	{
		loopPhase = enemyShipNS::BLACK_HOLE_INFLUENCED;
	}*/

	/*
	if(formingFormationMode)
	{
		// rotate the enemy ship
		if((spriteData.angle == 0 || spriteData.angle == (float)PI) && !rotating)
		{
			if(spriteData.x < (GAME_WIDTH*(2.0f/3.0f)) && spriteData.x > (GAME_WIDTH/3.0f))
			{
				// rotate flag
				rotating = true;
			}
		}
		else if(spriteData.angle == (float)PI/2.0f && !rotating)
		{
			if(spriteData.y > (GAME_HEIGHT/3.0f) && spriteData.y < (GAME_HEIGHT*(2.0f/3.0f)))
			{
				// rotate flag
				rotating = true;
			}
		}

		if(rotating)
		{
			// todo: do rotation calculation
			//rotation += frameTime * 12.0f;  // rotate clockwise
		}

		spriteData.angle = rotation;   // apply rotation

		// calculate the velocity based on which direction the enemy ship is facing
		//velocity.x = (float)cos(spriteData.angle) * 30000 * frameTime;
		//velocity.y = (float)sin(spriteData.angle) * 30000 * frameTime;
	}
	*/

	spriteData.x += frameTime * velocity.x;     // move ship along X 
    spriteData.y += frameTime * velocity.y;     // move ship along Y

    Entity::update(frameTime);
}

/////////////////////////////////////////////
// returns a pointer to EnemyShipCell struct once it finds a vacant cell in the s_enemyShipGrid array
/////////////////////////////////////////////
EnemyShipCell* EnemyShip::findVacantCell()
{
	int size = s_enemyShipGrid.size();

	// start at index GRID_WIDTH since the top row is reserved for the enemy missile ships
	for(int i = enemyShipNS::GRID_WIDTH; i < size; i++)
	{
		if(s_enemyShipGrid[i]->isOccupied == false)
		{
			return s_enemyShipGrid[i];
		}
	}

	return NULL;
}

void EnemyShip::setCellToUnoccupied()
{
	myCell->isOccupied = false;
}

void EnemyShip::deleteEnemyShipGrid()
{
	int size = s_enemyShipGrid.size();

	for (int i = 0; i < size; i++)
	{
		SAFE_DELETE(s_enemyShipGrid[i]);
	}
}

bool EnemyShip::isHigherThanRect(RECT rect)
{
	float myRectHeight = (float)rect.bottom - (float)rect.top;
	if(getCenterY() <= (float)rect.bottom - (myRectHeight/2.0f))
		return true;

	return false;
}

void EnemyShip::snapShipToCell(RECT rect)
{
	float myRectWidth, myRectHeight;

	myRectHeight = (float)rect.bottom - (float)rect.top;
	myRectWidth = (float)rect.right - (float)rect.left;
	velocity.x = 0;
	velocity.y = 0;

	// snap the location of the ship to the center of the cell
	spriteData.x = rect.left + (myRectWidth/2.0f) - (shipNS::WIDTH/2.0f);
	spriteData.y = rect.top + (myRectHeight/2.0f) - (shipNS::HEIGHT/2.0f);
}

void EnemyShip::calcCellPathVelocity(RECT myRect, float frameTime)
{
	float myRectWidth, myRectHeight, myRectCenterX, myRectCenterY, distanceX, distanceY, angle;

	myRectWidth = (float)myRect.right - (float)myRect.left;
	myRectHeight = (float)myRect.bottom - (float)myRect.top;
	myRectCenterX = (float)myRect.left + (myRectWidth/2.0f);
	myRectCenterY = (float)myRect.top + (myRectHeight/2.0f);

	// get the distance from the ship's center to the cell's center
	distanceX = myRectCenterX - getCenterX();
	distanceY = myRectCenterY - getCenterY();

	// set the velocity to move the ship towards the vacant cell
	angle = atan2(distanceY, distanceX);
	velocity.x = (float)cos(angle) * enemyShipNS::SPEED * frameTime;
	velocity.y = (float)sin(angle) * enemyShipNS::SPEED * frameTime;
}