// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// ship.h v1.0

#include "enemyShip.h"
#include "missile.h"

#ifndef _ENEMY_MISSILE_SHIP_H                 // Prevent multiple definitions if this 
#define _ENEMY_MISSILE_SHIP_H                 // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

namespace enemyMissileShipNS
{
	const int   ENEMY_SHIP_START_FRAME = 0;      // enemy missile ship starts at frame 0
    const int   ENEMY_SHIP_END_FRAME = 3;       // enemy missile ship animation frames 0, 1, 2, 3
	const int   TEXTURE_COLS = 8;           // texture has 7 columns
	const float FIRE_DELAY = 30.0f;      // 5 seconds between missile firing
}

// inherits from Entity class
class EnemyMissileShip : public EnemyShip
{
protected:
	virtual EnemyShipCell* findVacantCell();
	
public:
	// constructor
	EnemyMissileShip();
};
#endif