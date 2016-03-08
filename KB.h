#pragma once
#include "castor1.1\castor.h"
#include <map>
#include <ctime>

const int g_mapWidth = 3;
const int g_mapHeight = 3;

castor::relation gender( castor::lref < std::string > p, castor::lref<std::string> g )
{
	using namespace castor;
	return eq( p, "Frank" ) && eq( g, "male" )
		|| eq( p, "Mary" ) && eq( g, "female" )
		|| eq( p, "Sam" ) && eq( g, "male" )
		|| eq( p, "Tom" ) && eq( g, "male" )
		|| eq( p, "Beth" ) && eq( g, "female" )
		|| eq( p, "Ana" ) && eq( g, "female" )
		;
}
//////////////////////////////////////////////////////////////////////////////////////////
//[GLOBAL RULES]
castor::relation exceededMaxTurns( int curNumTurns )
{
	using namespace castor;
	return Boolean( curNumTurns > 100 )
		;
}

//////////////////////////////////////////////////////////////////////////////////////////
//stats: HP, Attack, and Movement
struct CharInfo
{
    
	std::string _name;
	std::string _teamName;
	float _hp;
	int _attack;
	int _movement;
	std::vector<int> _pos;
	
	CharInfo() : _name(""), 
                 _teamName(""), 
                 _hp(0.0f),
                 _attack(0),
                 _movement(0) 
                 {}
};
using CharInfoMap = std::map<std::string/*name*/, CharInfo>;

struct MapTile
{
	CharInfo _occupant;
	int _movementValue;
	int _defenseBonus;
};

enum class TerrainType
{
	normal,
	water,
	tree,
	boulder
};

enum class StrategyType
{
	standard, //move + attack if enemy in range
	run, //just keep running without attacking
	aggressive //if attackable enemy exists, attacks right away
};

//////////////////////////////////////////////////////////////////////////////////////////
//[TEAM REPRESENTATIONS]
//board representation: AxB (0,0 at bottom left)

//stats: HP, Attack, and Movement
castor::relation onTeam(
	castor::lref<std::string> charName, castor::lref<std::string> teamName,
	castor::lref<float> hp, castor::lref<int> attack, castor::lref<int> movement,
	castor::lref<int> posX = { }, castor::lref<int> posY = { }
	)
{
	using namespace castor;
	return eq( charName, "Lance" ) && eq( teamName, "A" ) && eq( hp, 12 ) && eq( attack, 7 ) && eq( movement, 1 ) && eq( posX, 0 ) && eq (posY, 2 )
		|| eq( charName, "Arthur" ) && eq( teamName, "A" ) && eq( hp, 10 ) && eq( attack, 8 ) && eq( movement, 1 ) && eq( posX, 1 ) && eq( posY, 2)
		|| eq( charName, "Homer" ) && eq( teamName, "A" ) && eq( hp, 3 ) && eq( attack, 1 ) && eq( movement, 1 ) && eq( posX, 2 ) && eq( posY, 1 )
		|| eq( charName, "Diana" ) && eq( teamName, "B" ) && eq( hp, 16 ) && eq( attack, 10 ) && eq( movement, 1 ) && eq( posX, 2 ) && eq(posY, 2)
		|| eq( charName, "Sorn" ) && eq( teamName, "B" ) && eq( hp, 7 ) && eq( attack, 4 ) && eq( movement, 1 ) && eq( posX, 2 ) && eq(posY, 0)
		|| eq( charName, "Vesta" ) && eq( teamName, "B" ) && eq( hp, 10 ) && eq( attack, 2 ) && eq( movement, 1 ) && eq( posX, 1 ) && eq(posY, 1)
		;
}
castor::relation teamNames( castor::lref<std::string> myTeam, castor::lref<std::string> enemyTeam )
{
	using namespace castor;
	//this specifically allows to specify adversaries
	//(or priorities to crtain adversaries)
	return eq( myTeam, "A" ) && eq( enemyTeam, "B" )
		|| eq( myTeam, "B" ) && eq( enemyTeam, "A" )
		;
}
bool teamAllDead( CharInfoMap& charInfos, std::string teamName )
{
	using namespace std;
	using namespace castor;
	lref<string> charName;
	//go through all chars on same team
	relation characterInfo = onTeam( charName, teamName, { }, { }, { } );
	while ( characterInfo() )
	{
		if ( charInfos.find( charName.get() ) != charInfos.end() && charInfos[charName.get()]._hp > 0 )
			return false;
	}
	return true;
}
castor::relation someTeamAllDead( CharInfoMap& charInfos, castor::lref<std::string> teamName )
{
	using namespace castor;
	//if any are true, then the teamName gets unified to the proper team letter
	return Boolean( teamAllDead( charInfos, "A" ) ) && eq( teamName, "A" )
		|| Boolean( teamAllDead( charInfos, "B" ) ) && eq( teamName, "B" )
		;
}
//finding a path

//////////////////////////////////////////////////////////////////////////////////////////
//[CHARACTER REPRESENTATIONS]
castor::relation characterHeldWeapon( castor::lref<std::string> charName, castor::lref<std::string> weapon )
{
	using namespace castor;
	return eq( charName, "Lance" ) && eq( weapon, "Hammer of Wrath" )
		|| eq( charName, "Arthur" ) && eq( weapon, "Blade of Doom" )
		|| eq( charName, "Homer" ) && eq( weapon, "Paper Towel" )
		|| eq( charName, "Diana" ) && eq( weapon, "Spear of Life" )
		|| eq( charName, "Sorn" ) && eq( weapon, "Ageon Stone" )
		|| eq( charName, "Vesta" ) && eq( weapon, "Amulet of Desire" )
		;
}

//ADV: BEING ABLE TO BACKTRACK WHICH CHARACTERS HAVE A CERTAIN STRATEGY TYPE,
//ALLOWS POSSIBLE SCNEARIOS OF EX. WANTING TO JUST TEST HOW WELL A CERTAIN STRATEGY GOES AGAINST ANOTHER
castor::relation characterStrategyType( castor::lref<std::string> charName, castor::lref<StrategyType> strategyType )
{
	using namespace castor;
	return eq( charName, "Lance" ) && eq( strategyType, StrategyType::run )
		|| eq( charName, "Arthur" ) && eq( strategyType, StrategyType::standard )
		|| eq( charName, "Homer" ) && eq( strategyType, StrategyType::standard )
		|| eq( charName, "Diana" ) && eq( strategyType, StrategyType::aggressive )
		|| eq( charName, "Sorn" ) && eq( strategyType, StrategyType::standard )
		|| eq( charName, "Vesta" ) && eq( strategyType, StrategyType::run )
		;
}

castor::relation characterAtkDialogue( castor::lref<std::string> charName, castor::lref<std::string> dialogue )
{
	using namespace castor;
	return eq( charName, "Lance" ) && eq( dialogue, "Lance: For the home country!\n" ) 
		|| eq( charName, "Arthur" ) && eq( dialogue, "Arthur: This is my day of courage!\n" )
		|| eq( charName, "Homer" ) && eq( dialogue, "Homer: My knife is pointy, ain't it?\n" )
		|| eq( charName, "Diana" ) && eq( dialogue, "Diana: For the dark angels!\n" )
		|| eq( charName, "Sorn" ) && eq( dialogue, "Sorn: Do you doubt my power now?\n" )
		|| eq( charName, "Vesta" ) && eq( dialogue, "Vesta: Fear my wrath for it is directed at you!\n" )
		;
}

castor::relation characterAtkdDialogue( castor::lref<std::string> charName, castor::lref<std::string> dialogue )
{
	using namespace castor;
	return eq( charName, "Lance" ) && eq( dialogue, "Lance: I know no defeat!\n" )
		|| eq( charName, "Arthur" ) && eq( dialogue, "Arthur: You think this is the last of me!?\n" )
		|| eq( charName, "Homer" ) && eq( dialogue, "Homer: Ouch! No pity? \n" )
		|| eq( charName, "Diana" ) && eq( dialogue, "Diana: Don't dare do that again.\n" )
		|| eq( charName, "Sorn" ) && eq( dialogue, "Sorn: I did not see that coming.\n" )
		|| eq( charName, "Vesta" ) && eq( dialogue, "Vesta: I may have underestimated your strength.\n" )

		;
}

castor::relation characterDeathDialogue( castor::lref<std::string> charName, castor::lref<std::string> dialogue )
{
	using namespace castor;
	return eq( charName, "Lance" ) && eq( dialogue, "Lance: See you in the afterlife.\n" )
		|| eq( charName, "Arthur" ) && eq( dialogue, "Arthur: My sword will live on!\n" )
		|| eq( charName, "Homer" ) && eq( dialogue, "Homer: Dang.\n" )
		|| eq( charName, "Diana" ) && eq( dialogue, "Diana: My comrades will avenge me.\n" )
		|| eq( charName, "Sorn" ) && eq( dialogue, "Sorn: You were a worthy foe.\n" )
		|| eq( charName, "Vesta" ) && eq( dialogue, "Vesta: My powers were little match.\n" )
		;
}

//////////////////////////////////////////////////////////////////////////////////////////
//[BOARD REPRESENTATIONS]

//2D array of mapTile structs
MapTile g_gameMap[g_mapWidth][g_mapHeight];


castor::relation coordsWithinMap( castor::lref<int> posX, castor::lref<int> posY )
{
	using namespace castor;
	return predicate( 0 <= posX ) && predicate( posX < g_mapWidth )
		&& predicate( 0 <= posY ) && predicate( posY < g_mapHeight )
		;
}
castor::relation adjacentCoords(
	castor::lref<int> curPosX, castor::lref<int> curPosY,
	castor::lref<int> adjPosX, castor::lref<int> adjPosY
	)
{
	return eq( curPosX.get()-1, adjPosX ) && eq( curPosY, adjPosY ) //left
		|| eq( curPosX.get()+1, adjPosX ) && eq( curPosY, adjPosY ) //right
		|| eq( curPosX, adjPosX ) && eq( curPosY.get()+1, adjPosY ) //up
		|| eq( curPosX, adjPosX ) && eq( curPosY.get()-1, adjPosY ) //down
		;
}
//////////////////////////////////////////////////////////////////////////////////////////
//[WEAPON INTERACTION]

//note: backtracking allows both strong against && weak against checks
castor::relation weaponTypeIsStrongerThanWeaponType(
	castor::lref<std::string> weapon1, castor::lref<std::string> weapon2
	)
{
	return eq( weapon1, "rock" ) && eq( weapon2, "scissors" )
		|| eq( weapon1, "paper" ) && eq( weapon2, "rock" )
		|| eq( weapon1, "scissors" ) && eq( weapon2, "paper" )
		;
}

castor::relation weaponIsType(
	castor::lref<std::string> weapon1, castor::lref<std::string> type
	)
{
	return eq( weapon1, "Hammer of Wrath" ) && eq( type, "rock" )
		^ eq( weapon1, "Paper Towel" ) && eq( type, "paper" )
		^ eq( weapon1, "Blade of Doom" ) && eq( type, "scissors" )
		^ eq( weapon1, "Ageon Stone" ) && eq( type, "rock" )
		^ eq( weapon1, "Spear of Life" ) && eq( type, "scissors" )
		^ eq( weapon1, "Amulet of Desire" ) && eq( type, "paper" )
		^ eq( type, "scissors" )
		;
}

castor::relation characterHeldWeaponType( castor::lref<std::string> charName, castor::lref<std::string> weaponType )
{
	using namespace castor;
	lref<std::string> weapon;
	return characterHeldWeapon( charName, weapon ) && weaponIsType( weapon, weaponType )
		;
}

castor::relation weaponIsStrongerThanWeapon(
	castor::lref<std::string> weapon1, castor::lref<std::string> weapon2
	)
{
	using namespace castor;
	lref<std::string> weapon1type, weapon2type;
	return weaponIsType( weapon1, weapon1type )
		&& weaponIsType( weapon2, weapon2type )
		&& weaponTypeIsStrongerThanWeaponType( weapon1type, weapon2type )
		;
}

const float g_atk_effective = 1.5;
const float g_atk_uneffective = 0.5;

castor::relation weaponEffectOnWeapon(
	castor::lref<std::string> weapon1, castor::lref<std::string> weapon2, castor::lref<float> effectMultiplier
	)
{
	return weaponIsStrongerThanWeapon( weapon1, weapon2 ) && eq( effectMultiplier, g_atk_effective )
		//Note: basically saying if weapons of same time, also 0.5
		|| eq( effectMultiplier, g_atk_uneffective )
		;
}

castor::relation weaponEffectMsg(
	castor::lref<float> multiplier, castor::lref<std::string> msg
	)
{
	return eq( multiplier, g_atk_effective ) && eq( msg, "attack was effective! \n" )
		^ eq( msg, "attack didn't do much... \n" )
		;
}

//////////////////////////////////////////////////////////////////////////////////////////
//[UNIT/MAP INTERACTION]

castor::relation existAdjacentEnemies(
	castor::lref<int> myPosX, castor::lref<int> myPosY, castor::lref<std::string> enemyTeam
    	)
{
	using namespace castor;
	return eq(enemyTeam, g_gameMap[myPosX.get()+1][myPosY.get()]._occupant._teamName)
		|| eq(enemyTeam, g_gameMap[myPosX.get()-1][myPosY.get()]._occupant._teamName)
		|| eq(enemyTeam, g_gameMap[myPosX.get()][myPosY.get()+1]._occupant._teamName)
        || eq(enemyTeam, g_gameMap[myPosX.get()][myPosY.get()-1]._occupant._teamName)   
    ;
}

//gives the ability to create maps
castor::relation tileIsTerrainTypeSpecial(
	castor::lref<int> posX, castor::lref<int> posY, castor::lref<TerrainType> type
	)
{
	return eq( posX, 0 ) && eq( posY, 1 ) && eq( type, TerrainType::tree )
		|| eq( posX, 0 ) && eq( posY, 4 ) && eq( type, TerrainType::tree )
		|| eq( posX, 0 ) && eq( posY, 5 ) && eq( type, TerrainType::tree )
		|| eq( posX, 0 ) && eq( posY, 6 ) && eq( type, TerrainType::tree )
		|| eq( posX, 3 ) && eq( posY, 1 ) && eq( type, TerrainType::water )
		|| eq( posX, 3 ) && eq( posY, 2 ) && eq( type, TerrainType::water )
		|| eq( posX, 3 ) && eq( posY, 3 ) && eq( type, TerrainType::water )
		|| eq( posX, 5 ) && eq( posY, 0 ) && eq( type, TerrainType::boulder )
		|| eq( posX, 5 ) && eq( posY, 1 ) && eq( type, TerrainType::boulder )
		|| eq( posX, 2 ) && eq( posY, 6 ) && eq( type, TerrainType::boulder )
		|| eq( posX, 2 ) && eq( posY, 7 ) && eq( type, TerrainType::boulder )
		;
}

castor::relation tileIsTerrainType(
	castor::lref<int> posX, castor::lref<int> posY, castor::lref<TerrainType> type
	)
{
	using namespace castor;
	return tileIsTerrainTypeSpecial( posX, posY, type )
		|| eq( type, TerrainType::normal ) //note: using a cut (not an or)
		;
}

//can make certain tiles hp ups, or hp downs

//unit vs terrain type
//element type

//////////////////////////////////////////////////////////////////////////////////////////
//[PLAYER MOVEMENT]
//Note: MIXING MULTIPLE PREDICATES TOGETHER TO FORM MORE COMPLEX [CAN WRITE IN LOG]
//Note: NEED TO MAKE SURE THAT OUTPUTS AREN'T UNKNOWNS (EX. WHEN CHECKPOSX, NOT DEFINED)
bool cellIsOccupied(
	CharInfoMap& charInfos, int checkPosX, int checkPosY
	)
{
	return ( g_gameMap[checkPosX][checkPosY]._occupant._hp > 0 );
}

castor::relation playerCanMoveTo(
	CharInfoMap& charInfos,
	castor::lref<int> curPosX, castor::lref<int> curPosY,
	castor::lref<int> adjPosX, castor::lref<int> adjPosY,
	castor::lref<TerrainType> terrainType = { }
	)
{
	using namespace castor;
	//lref<TerrainType> terrainType;
	return adjacentCoords( curPosX, curPosY, adjPosX, adjPosY )
		&& coordsWithinMap( adjPosX, adjPosY )
		&& tileIsTerrainType( adjPosX, adjPosY, terrainType )
		&& eq( terrainType, TerrainType::normal )
		;
}
castor::relation playerCanAttack(
	castor::lref<int> myPosX, castor::lref<int> myPosY,
	castor::lref<int> foePosX, castor::lref<int> foePosY
	)
{
	return adjacentCoords( myPosX, myPosY, foePosX, foePosY )
		;
}

//////////////////////////////////////////////////////////////////////////////////////////
using CharInfoList = std::list<CharInfo>;
using CharInfoListLref = castor::lref<CharInfoList>;
//castor::relation teamAlive( CharInfoListLref& charInfos, castor::lref<std::string> teamName )
//{
//	using namespace castor;
//	lref<charInfo> charInfo;
//	CharInfoListLref t;
//	return head( charInfos, charInfo ) && predicate( charInfo.get()._hp > 0 )
//		&& tail( charInfos, t )
//		&& recurse( t, teamName )
//		;
//}



////////////////////////////////////////////////////////
//helper function for printing
template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
	using namespace std;
	size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
	unique_ptr<char[]> buf( new char[size] );
	snprintf( buf.get(), size, format.c_str(), args ... );
	return string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}
