#pragma once
#include "KB.h"

extern int g_GameState;
extern int g_turns;
extern MapTile g_gameMap[g_mapWidth][g_mapHeight];

void UpdateCharPos( CharInfo& charInfo, const int x, const int y )
{
	g_gameMap[charInfo._pos[0]][charInfo._pos[1]]._occupant = CharInfo();
	g_gameMap[x][y]._occupant = charInfo;
	charInfo._pos = { x,y };
	
}

namespace Phases
{
	void evalGameState( CharInfoMap& charInfos )
	{
		using namespace std;
		using namespace castor;

		//check: exceeded max amount of turns
		if ( exceededMaxTurns( g_turns )( ) )
		{
			g_GameState = -1;
			return;
		}
		//check: if one team all dead
		{
			lref<string> deadTeamName;
			relation sTAD = someTeamAllDead( charInfos, deadTeamName );
			if ( sTAD() )
			{
				g_GameState = deadTeamName.get() == "A" ? 2 : 1;
				return;
			}
		}
	}

	void move_moveOnMap( CharInfoMap& charInfos, CharInfo& curChar )
	{
		using namespace std;
		using namespace castor;
		lref<int> adjPosX, adjPosY;
		//get total possible moveable points and choose random
		relation canMoveTo = playerCanMoveTo( charInfos, curChar._pos[0], curChar._pos[1], adjPosX, adjPosY );
		auto i = 0, random_var = 0;
		{
			while ( canMoveTo() )
			{
				if ( !cellIsOccupied( charInfos, adjPosX.get(), adjPosY.get() ) )
					++i;
			}
			if ( i == 0 )
			{
				cout << curChar._name << " cannot move anywhere\n";
				return;
			}
			adjPosX.reset();
			adjPosY.reset();
			std::srand( std::time( 0 ) ); // use current time as seed for random generator
			random_var = std::rand() % i;
		}
		//actually make the movement
		i = 0;
		lref<TerrainType> terrainType;
		relation canMoveTo2 = playerCanMoveTo( charInfos, curChar._pos[0], curChar._pos[1], adjPosX, adjPosY, terrainType );
		while ( canMoveTo2() )
		{
			if ( !cellIsOccupied( charInfos, adjPosX.get(), adjPosY.get() ) )
			{
				if ( i == random_var )
				{
					cout << string_format( "%s(team:%s)[%i,%i] has moved to:[%i,%i]",
										   curChar._name.c_str(), curChar._teamName.c_str(),
										   curChar._pos[0], curChar._pos[1], adjPosX.get(), adjPosY.get()
										   ) << endl;

					//update character position
					UpdateCharPos( curChar, adjPosX.get(), adjPosY.get() );
					break;
				}
				++i;
			}
		}
	}

	bool move_atkIfPossible( CharInfoMap& charInfos, CharInfo& curChar, std::string oppTeamName, std::string suffix="" )
	{
		using namespace std;
		using namespace castor;
		lref<string> enemyName;
		relation enemyInfo = onTeam( enemyName, oppTeamName, { }, { }, { } );
		while ( enemyInfo() )
		{
			if ( charInfos.find( enemyName.get() ) != charInfos.end() )
			{
				//note: 'while' is used so that we keep looping until we find an enemy we can attack and hp > 0
				auto& curEnemy = charInfos[enemyName.get()];
				relation canAttack = playerCanAttack( curChar._pos[0], curChar._pos[1], curEnemy._pos[0], curEnemy._pos[1] );
				if ( canAttack() && curEnemy._hp > 0 )
				{
					//calculate damage
					float attack = static_cast<float>( curChar._attack );
					float attackMultiplier;
					{
						lref<float> multiplier;
						lref<string> atkWeapon, defWeapon;
						relation charWeapon = characterHeldWeapon( curChar._name, atkWeapon );
						relation enemyWeapon = characterHeldWeapon( enemyName, defWeapon );
						if ( charWeapon() && enemyWeapon() )
						{
							relation weaponMultiplier = weaponEffectOnWeapon( atkWeapon, defWeapon, multiplier );
							if ( weaponMultiplier() )
								attackMultiplier = multiplier.get();
						}
					}

					curEnemy._hp -= (attack * attackMultiplier);
					cout << string_format( "%s(team:%s[%i,%i]) atkd %s(team:%s, rem-hp:%.1f)[%i,%i] %s \n",
										   curChar._name.c_str(), curChar._teamName.c_str(),
										   curChar._pos[0], curChar._pos[1],
										   enemyName.get().c_str(), oppTeamName.c_str(), curEnemy._hp,
										   curEnemy._pos[0], curEnemy._pos[1],
										   suffix.c_str());

					lref<string> msg;
					relation atkEffectMsg = weaponEffectMsg( attackMultiplier, msg );
					if ( atkEffectMsg() )
						cout << msg.get();

					//print appropriate dialogue
					{
						lref<string> dialogue;
						relation charAtkDialogue = characterAtkDialogue( curChar._name, dialogue );
						if ( charAtkDialogue() )
							cout << dialogue.get();
						lref<string> dialogue2;
						relation charAtkdDialogue = characterAtkdDialogue( curEnemy._name, dialogue2 );
						if ( charAtkdDialogue() )
							cout << dialogue2.get();
					}


					if ( curEnemy._hp <= 0 )
					{
						//if enemy is dead, remove it from the map
						cout << string_format( "-- %s died! -- \n", curEnemy._name.c_str() );
						g_gameMap[curEnemy._pos[0]][curEnemy._pos[1]]._occupant = CharInfo();
						lref<string> dialogue3;
						relation charDeathDialogue = characterDeathDialogue( curEnemy._name, dialogue3 );
						if ( charDeathDialogue() )
							cout << dialogue3.get();
					}
					return true;
				}
			}
		}
		return false;
	}

	///////////////////////////////////////////////////////////////////////////////
	void fightPhase( CharInfoMap& charInfos )
	{
		using namespace std;
		using namespace castor;
		lref<string> charName, teamName, enemyName;
		lref<int> hp, attack, movement;

		//go through each team
		lref<string> curTeamName, oppTeamName;
		relation teams = teamNames( curTeamName, oppTeamName );
		while ( teams() )
		{
			//go through all players in each team
			charName.reset();
			relation characterInfo = onTeam( charName, curTeamName, { }, { }, { } );
			while ( characterInfo() )
			{
				if ( charInfos.find( charName.get() ) != charInfos.end() )
				{
					auto& curChar = charInfos[charName.get()];
					if ( curChar._hp > 0 )
					{
						lref<StrategyType> strategyType;
						relation charStratType = characterStrategyType( charName, strategyType );
						if ( charStratType() )
						{
							//attack an enemy team player
							//strategy
							//relation canAttackImmediately = existAdjacentEnemies(curChar._pos[0], curChar._pos[1], oppTeamName);
							//if ( !canAttackImmediately())
							{
								//[MOVE 0. Attack any enemy in sight]
								bool attackFinished = false;
								if ( strategyType.get() == StrategyType::aggressive )
									if ( move_atkIfPossible( charInfos, curChar, oppTeamName.get(), "[AGGRESSIVE ATTACK]" ) )
										attackFinished = true;

								//[MOVE 1. Move to an adjacent position on the map if there is not already an enemy in attack range (adjacent).
								if ( !attackFinished )
									move_moveOnMap( charInfos, curChar );

								//[MOVE 2. look for an enemy that is adjacent => attack]
								if ( !attackFinished && strategyType.get() != StrategyType::run )
									move_atkIfPossible( charInfos, curChar, oppTeamName.get() );

								evalGameState( charInfos );
								if ( g_GameState != 0 )
									return;
							}
						}
					}
				}
			}
		}
	}
};
