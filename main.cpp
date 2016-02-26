
#include "castor1.1\castor.h"
#include <map>
#include "KB.h"

//GLOBAL VARS
int g_GameState = 0; //0:continue game, 1:A won, 2:B won
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

	//check: if one team all dead
	{
		lref<string> deadTeamName;
		relation sTAD = someTeamAllDead( charInfos, deadTeamName );
		if ( sTAD() )
		{
			g_GameState = deadTeamName.get() == "A" ? 2 : 1;
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
		relation characterInfo = onTeam( charName, curTeamName, { }, { }, { } );
		while ( characterInfo() )
		{
			if ( charInfos[charName.get()]._hp > 0 )
			{
				//attack an enemy team player
				relation enemyInfo = onTeam( enemyName, oppTeamName, { }, { }, { } );
				while ( enemyInfo() )
				{
					//note: 'while' is used so that we keep looping until we find an enemy with hp > 0
					if ( charInfos[enemyName.get()]._hp > 0 )
					{
						charInfos[enemyName.get()]._hp -= charInfos[charName.get()]._attack;
						cout << string_format( "%s(team:%s) atkd %s(team:%s, rem-hp:%i)",
											   charName.get().c_str(), curTeamName.get().c_str(),
											   enemyName.get().c_str(), oppTeamName.get().c_str(), charInfos[enemyName.get()]._hp ) << endl;
						//check if enemy dead
						if ( charInfos[enemyName.get()]._hp <= 0 )
							cout << string_format( "\n%s(team:%s) has died!\n",
												   enemyName.get().c_str(), oppTeamName.get().c_str() ) << endl;

						evalGameState( charInfos );
						if ( g_GameState != 0 )
							return;

						//each player can only attack 1 enemy
						break;
					}
				}
				enemyName.reset();
			}
		}
	}
}

void main()
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
		lref<int> hp, attack, movement;
		relation characterInfo = onTeam( charName, teamName, hp, attack, movement );
		while ( characterInfo() )
		{
			charInfo charInfo;
			charInfo._name = charName.get();
			charInfo._teamName = teamName.get();
			charInfo._hp = hp.get();
			charInfo._attack = attack.get();
			charInfo._movement = movement.get();
			playerInfos.emplace( charName.get(), charInfo );
		}
	}

	//[GAME LOOP]
	while ( g_GameState == 0 )
	{
		//each player in team attacks a player in the other team
		fightPhase( playerInfos );
		if ( endGame() ) break;
	}

	//announce winner + finish
	cout << string_format( "\nTeam:%s has won!\n", ( g_GameState == 1 ? "A" : "B" ) ) << endl;

	std::string input;
	cout << "press enter to finish";
	std::getline( std::cin, input );
	return;
}