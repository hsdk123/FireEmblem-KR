
#include "castor1.1\castor.h"
#include <map>
#include <ctime>
#include "KB.h"
#include "Phases.h"

//GLOBAL VARS
int g_GameState = 0; //0:continue game, 1:A won, 2:B won, -1: exceeded max turns
int g_turns = 0;
//
bool endGame()
{
	return g_GameState != 0;
}

using CharInfoMap = std::map<std::string/*name*/, CharInfo>;

int main()
{
	using namespace std;
	using namespace castor;

	//[USER INPUT]
	std::string input;
	std::string msg = "\nENTER TYPE OF GAME: ('DEFAULT' OR 'CUSTOM') \n"
		+ string( " (default: auto run a normal game) \n (custom: custom unit choosing, difficulty level, etc.)\n (use lowercase)\n" );
	cout << msg;
	std::getline( std::cin, input );

	string wantedUnitType = "all";
	if ( input != "default" )
	{
		std::string msg = "\nENTER WANTED UNIT TYPE: \n"
			+ string( " ('rock' or 'paper' or 'scissors')\n (use lowercase)\n" );
		cout << msg;
		std::getline( std::cin, wantedUnitType );
		if ( wantedUnitType != "rock" && wantedUnitType != "scissors" && wantedUnitType != "paper" )
		{
			cout << "invalid input. defaulting to type: 'rock' \n\n";
			wantedUnitType = "rock";
		}
	}

	using CharInfoVec = map<string/*name*/, CharInfo>;
	CharInfoVec playerInfos;
	//Initialize board representation
	for(int i = 0; i < g_mapWidth;i++)
	{
		for (int j = 0; j < g_mapHeight; j++)
		{
			g_gameMap[i][j]._movementValue = 1;
			g_gameMap[i][j]._defenseBonus = 0;
			g_gameMap[i][j]._occupant = CharInfo();
		}
	}
	
	//[POPULATE CHARACTERS]
	{
		lref<string> charName, teamName;
		lref<int> hp, attack, movement, posX, posY;
		relation characterInfo = onTeam( charName, teamName, hp, attack, movement, posX, posY );
		while ( characterInfo() )
		{
			//Note: 'A') player team
			if ( teamName.get() == "A" && wantedUnitType != "all" )
			{
				//if character has chosen a custom preferred type, only use units with that type
				relation charHasWeaponType = characterHeldWeaponType( charName, wantedUnitType );
				if ( !charHasWeaponType() )
					continue;
			}

			CharInfo charInfo;
			charInfo._name = charName.get();
			charInfo._teamName = teamName.get();
			charInfo._hp = hp.get();
			charInfo._attack = attack.get();
			charInfo._movement = movement.get();
			charInfo._pos = { posX.get(), posY.get() };
			playerInfos.emplace( charName.get(), charInfo );
			UpdateCharPos( charInfo, posX.get(), posY.get() );
		}
	}

	//[GAME LOOP]
	while ( g_GameState == 0 )
	{
		cout << "\n";
		Phases::evalGameState( playerInfos );
		if ( endGame() ) break;

		//each player in team attacks a player in the other team
		Phases::fightPhase( playerInfos );
		if ( endGame() ) break;

		++g_turns;
	}

	if ( g_GameState == -1 )
		cout << string_format( "\nGame exceeded max turns:%i ", g_turns ) << endl;
	else
	{
		//announce winner + finish
		cout << string_format( "\nTeam:%s has won! (tot_rounds:%i) (player team:A)\n", 
							   ( g_GameState == 1 ? "A" : "B" ), g_turns ) << endl;
	}

	cout << "press enter to finish";
	std::getline( std::cin, input );
	return 0;
}
