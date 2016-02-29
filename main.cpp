
#include "castor1.1\castor.h"
#include <map>
#include <ctime>
#include "KB.h"

//GLOBAL VARS
int g_GameState = 0; //0:continue game, 1:A won, 2:B won, -1: exceeded max turns
int g_turns = 0;
//
bool endGame()
{
	return g_GameState != 0;
}

using CharInfoMap = std::map<std::string/*name*/, charInfo>;
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
			auto& curChar = charInfos[charName.get()];
			if ( curChar._hp > 0 )
			{
				//attack an enemy team player
				//strategy
				//[MOVE 1. look for an enemy that is adjacent => attack]
				relation enemyInfo = onTeam( enemyName, oppTeamName, { }, { }, { } );
				while ( enemyInfo() )
				{
					//note: 'while' is used so that we keep looping until we find an enemy we can attack and hp > 0
					auto& curEnemy = charInfos[enemyName.get()];
					relation canAttack = playerCanAttack( curChar._pos[0], curChar._pos[1], curEnemy._pos[0], curEnemy._pos[1] );
					if ( canAttack() && curEnemy._hp > 0 )
					{
						curEnemy._hp -= curChar._attack;
						cout << string_format( "%s(team:%s[%i,%i]) atkd %s(team:%s, rem-hp:%i)[%i,%i]",
											   charName.get().c_str(), curTeamName.get().c_str(),
											   curChar._pos[0], curChar._pos[1],
											   enemyName.get().c_str(), oppTeamName.get().c_str(), curEnemy._hp,
											   curEnemy._pos[0], curEnemy._pos[1] ) << endl;
						//check if enemy dead
						if ( curEnemy._hp <= 0 )
						{
							//update player's position to enemy's
							curChar._pos = { curEnemy._pos[0], curEnemy._pos[1] };
							cout << string_format( "\n%s(team:%s) has died!\n",
												   enemyName.get().c_str(), oppTeamName.get().c_str() ) << endl;
						}

						evalGameState( charInfos );
						if ( g_GameState != 0 )
							return;

						//each player can only attack 1 enemy
						break;
					}
				}
				enemyName.reset();
				//[MOVE 2. just move to an adjacent position on the map]
				{
					//move to a random adjacent point
					{
						lref<int> adjPosX, adjPosY;
						//get total possible moveable points and choose random
						relation canMoveTo = playerCanMoveTo( curChar._pos[0], curChar._pos[1], adjPosX, adjPosY );
						auto i = 0, random_var = 0;
						{
							while ( canMoveTo() )
								++i;
							adjPosX.reset();
							adjPosY.reset();
							std::srand( std::time( 0 ) ); // use current time as seed for random generator
							random_var = std::rand() % i;
						}
						//actually make the movement
						i = 0;
						relation canMoveTo2 = playerCanMoveTo( curChar._pos[0], curChar._pos[1], adjPosX, adjPosY );
						while ( canMoveTo2() )
						{
							if ( i == random_var )
							{
								cout << string_format( "%s(team:%s)[%i,%i] has moved to:[%i,%i]",
													   charName.get().c_str(), curTeamName.get().c_str(),
													   curChar._pos[0], curChar._pos[1], adjPosX.get(), adjPosY.get()
													   ) << endl;
								curChar._pos = { adjPosX.get(), adjPosY.get() };
								break;
							}
							++i;
						}
					}
				}
			}
		}
	}
}

int main()
{
	using namespace std;
	using namespace castor;
	relation samIsMale = gender( "Sam", "male" ); //this does not perform anything
	if ( samIsMale() ) //this performs the query
		cout << "Sam is male \n" << endl;
	else
		cout << "Sam is not male \n" << endl;

	using CharInfoVec = map<string/*name*/, charInfo>;
	CharInfoVec playerInfos;

	//[POPULATE CHARACTERS]
	{
		lref<string> charName, teamName;
		lref<int> hp, attack, movement, posX, posY;
		relation characterInfo = onTeam( charName, teamName, hp, attack, movement, posX, posY );
		while ( characterInfo() )
		{
			charInfo charInfo;
			charInfo._name = charName.get();
			charInfo._teamName = teamName.get();
			charInfo._hp = hp.get();
			charInfo._attack = attack.get();
			charInfo._movement = movement.get();
			charInfo._pos = { posX.get(), posY.get() };
			playerInfos.emplace( charName.get(), charInfo );
		}
	}

	//[GAME LOOP]
	while ( g_GameState == 0 )
	{
		evalGameState( playerInfos );
		if ( endGame() ) break;

		//each player in team attacks a player in the other team
		fightPhase( playerInfos );
		if ( endGame() ) break;

		++g_turns;
	}

	if ( g_GameState == -1 )
		cout << string_format( "\nGame exceeded max turns:%i ", g_turns ) << endl;
	else
	{
		//announce winner + finish
		cout << string_format( "\nTeam:%s has won! (tot_rounds:%i)\n", 
							   ( g_GameState == 1 ? "A" : "B" ), g_turns ) << endl;
	}

	std::string input;
	cout << "press enter to finish";
	std::getline( std::cin, input );
	return 0;
}
