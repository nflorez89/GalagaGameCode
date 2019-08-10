#include "enemyMissileShip.h"

//=============================================================================
// default constructor
//=============================================================================
EnemyMissileShip::EnemyMissileShip() : EnemyShip()
{
	weaponType = enemyShipNS::MISSILE;
}

/////////////////////////////////////////////
// returns a pointer to EnemyShipCell struct once it finds a vacant cell in the s_enemyShipGrid array
/////////////////////////////////////////////
EnemyShipCell* EnemyMissileShip::findVacantCell()
{
	int size = s_enemyShipGrid.size();

	// start at index GRID_WIDTH since the top row is reserved for the enemy missile ships
	for(int i = 0; i < enemyShipNS::GRID_WIDTH; i++)
	{
		if(s_enemyShipGrid[i]->isOccupied == false)
		{
			return s_enemyShipGrid[i];
		}
	}

	return NULL;
}