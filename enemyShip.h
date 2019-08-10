// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// ship.h v1.0

#ifndef _ENEMY_SHIP_H                 // Prevent multiple definitions if this 
#define _ENEMY_SHIP_H                 // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

#include <array>
#include "ship.h"
#include "constants.h"
#include "laser.h"

namespace enemyShipNS
{
	const short GRID_WIDTH = 8; // 8 enemy ships per row
	const short GRID_HEIGHT = 3; // 3 rows
	const float SPEED = 30000.0f;
	const float RUSH_SPEED_X = 30000.0f;
	const float MIN_RUSH_ROTATION_RAD = 0.0f/*(float)PI/4.0f*/;
	const float MAX_RUSH_ROTATION_RAD = (float)PI/*((float)PI*3.0f)/4.0f*/;
	const float ROTATION_RATE = (float)PI/2.0f; // radians per second
	const float LOOP_ROTATION_RATE = 3.0f/*(float)PI*2.0f*/;
	const float LOOP_RADIUS = 60.0f; // radius of the circle the ship will loop around
    enum WEAPON_TYPE {LASER, MISSILE};
	enum RUSH_DIR {NONE, LEFT, RIGHT};
	enum CLOCKWISE_LOOP_PHASE {PRIOR_TO_LOOP, CLOCKWISE, COUNTERCLOCKWISE, LOOP_COMPLETE, PARKED, BLACK_HOLE_INFLUENCED};
	enum SPAWN_MOVEMENT_DIR {SPAWN_MOVING_LEFT, SPAWN_MOVING_RIGHT};
}

struct EnemyShipCell
{
	RECT cellRect;
	bool isOccupied;
};

// inherits from Entity class
class EnemyShip : public Ship
{
private:
	bool rushMode;
	enemyShipNS::RUSH_DIR rushDirection;
	bool isHigherThanRect(RECT rect);
	void snapShipToCell(RECT rect);
	void calcCellPathVelocity(RECT myRect, float frameTime);

protected:
	static bool blackHoleActive;
	static std::array<EnemyShipCell*, enemyShipNS::GRID_WIDTH*enemyShipNS::GRID_HEIGHT> s_enemyShipGrid;
	EnemyShipCell* myCell; // so that when the ship is no longer in the game it can tell the cell that it is no longer occupied before this ship is destroyed
	Game* gamePtr;
	float   fireTimer;                  // time remaining until fire enabled
	bool	ammoReady;
	enemyShipNS::WEAPON_TYPE weaponType;
	bool formingFormationMode;
	bool rotating;
	float loopX;
	float loopY;
	float loopCenterX;
	float loopCenterY;
	float rotationRads;
	enemyShipNS::CLOCKWISE_LOOP_PHASE loopPhase;
	enemyShipNS::SPAWN_MOVEMENT_DIR spawnMovementDir;
	float loopRotation;

	virtual EnemyShipCell* findVacantCell();

public:
    // constructor
    EnemyShip();

	// destructor
	~EnemyShip();

    // update ship position and angle
    virtual void update(float frameTime);

	void setFireTimer(float timer) {fireTimer = timer;}
	bool getAmmoReady() {return ammoReady;}
	bool getRushMode() {return rushMode;}
	enemyShipNS::RUSH_DIR getRushDir() {return rushDirection;}
	void setAmmoReady(bool ammoRdy) {ammoReady = ammoRdy;}
	enemyShipNS::WEAPON_TYPE getWeaponType() {return weaponType;}
	bool getFormingFormationMode() {return formingFormationMode;}
	void setFormingFormationMode(bool ffm) {formingFormationMode = ffm;}
	void setCellToUnoccupied();
	void setRushMode(bool rushMd) {rushMode = rushMd;}
	void setRushDir(enemyShipNS::RUSH_DIR rushDir) { rushDirection = rushDir;}
	static void setBlackHoleActive(bool activeStatus) {blackHoleActive = activeStatus;}
	static std::array<EnemyShipCell*, enemyShipNS::GRID_WIDTH*enemyShipNS::GRID_HEIGHT> initEnemyShipGridArray();
	static void deleteEnemyShipGrid();
	void setSpawnMovementDirection(enemyShipNS::SPAWN_MOVEMENT_DIR spwnMovementDir) {spawnMovementDir = spwnMovementDir;}

};
#endif

