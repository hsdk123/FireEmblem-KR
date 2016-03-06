#pragma once
#include "castor1.1\castor.h"
#include <map>
#include <ctime>

extern const int g_mapWidth = 3;
extern const int g_mapHeight = 3;

castor::relation gender( castor::lref < std::string > p, castor::lref<std::string> g )
{
	using namespace castor;
	return eq( p, "Frank" ) && eq( g, "male" )
		|| eq( p, "Mary" ) && eq( g, "female" )
		|| eq( p, "Sam" ) && eq( g, "male" )
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
	int _hp;
	int _attack;
	int _movement;
	std::vector<int> _pos;
	
	CharInfo() : _name(""), 
                 _teamName(""), 
                 _hp(0),
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
	castor::lref<int> hp, castor::lref<int> attack, castor::lref<int> movement,
	castor::lref<int> posX = { }, castor::lref<int> posY = { } )
{
	using namespace castor;
	return eq( charName, "Lance" ) && eq( teamName, "A" ) && eq( hp, 12 ) && eq( attack, 7 ) && eq( movement, 1 ) && eq( posX, 0 ) && eq (posY, 2 )
		|| eq( charName, "Arthur" ) && eq( teamName, "A" ) && eq( hp, 10 ) && eq( attack, 8 ) && eq( movement, 1 ) && eq( posX, 1 ) && eq( posY, 2)
		|| eq( charName, "Homer" ) && eq( teamName, "A" ) && eq( hp, 3 ) && eq( attack, 1 ) && eq( movement, 1 ) && eq( posX, 2 ) && eq( posY, 1 )
		|| eq( charName, "Diana" ) && eq( teamName, "B" ) && eq( hp, 16 ) && eq( attack, 15 ) && eq( movement, 1 ) && eq( posX, 3 ) && eq(posY, 0)
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
castor::relation characterHeldWeaponType( castor::lref<std::string> charName, castor::lref<std::string> weaponType )
{
	using namespace castor;
	return eq( charName, "Lance" ) && eq( weaponType, "rock" )
		|| eq( charName, "Arthur" ) && eq( weaponType, "paper" )
		|| eq( charName, "Homer" ) && eq( weaponType, "scissors" )
		|| eq( charName, "Diana" ) && eq( weaponType, "scissors" )
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
		;
}

castor::relation characterAtkDialogue( castor::lref<std::string> charName, castor::lref<std::string> dialogue )
{
	using namespace castor;
	return eq( charName, "Lance" ) && eq( dialogue, "Lance: For the home country!\n" ) 
		|| eq( charName, "Arthur" ) && eq( dialogue, "Arthur: This is my day of courage!\n" )
		|| eq( charName, "Homer" ) && eq( dialogue, "Homer: My knife is pointy, ain't it?\n" )
		|| eq( charName, "Diana" ) && eq( dialogue, "Diana: For the dark angels!\n" )
		;
}

castor::relation characterAtkdDialogue( castor::lref<std::string> charName, castor::lref<std::string> dialogue )
{
	using namespace castor;
	return eq( charName, "Lance" ) && eq( dialogue, "Lance: I know no defeat!\n" )
		|| eq( charName, "Arthur" ) && eq( dialogue, "Arthur: You think this is the last of me!?\n" )
		|| eq( charName, "Homer" ) && eq( dialogue, "Homer: Ouch! No pity? \n" )
		|| eq( charName, "Diana" ) && eq( dialogue, "Diana: Don't dare do that again\n" )
		;
}

//////////////////////////////////////////////////////////////////////////////////////////
//[BOARD REPRESENTATIONS]

//2D array of mapTile structs
MapTile g_gameMap[g_mapWidth][g_mapHeight];


castor::relation coordsWithinMap( castor::lref<int> posX, castor::lref<int> posY )
{
	using namespace castor;
	return predicate( 0 <= posX ) && predicate( posX <= g_mapWidth )
		&& predicate( 0 <= posY ) && predicate( posY <= g_mapHeight )
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
//[PLAYER MOVEMENT]
//Note: MIXING MULTIPLE PREDICATES TOGETHER TO FORM MORE COMPLEX [CAN WRITE IN LOG]
//Note: NEED TO MAKE SURE THAT OUTPUTS AREN'T UNKNOWNS (EX. WHEN CHECKPOSX, NOT DEFINED)
bool cellIsOccupied(
	CharInfoMap& charInfos, int checkPosX, int checkPosY
	)
{
	//note: can't make this a relation since castor can't do backtracking on [] operators
	for ( auto& unit : charInfos )
	{
		if ( unit.second._pos[0] == checkPosX && unit.second._pos[1] == checkPosY )
			return true;
	}
	return false;
}

castor::relation playerCanMoveTo( 
	CharInfoMap& charInfos,
	castor::lref<int> curPosX, castor::lref<int> curPosY,
	castor::lref<int> adjPosX, castor::lref<int> adjPosY 
	)
{
	using namespace castor;
	return adjacentCoords( curPosX, curPosY, adjPosX, adjPosY )
		&& coordsWithinMap( adjPosX, adjPosY )
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
		|| eq( weapon1, "Paper Towel" ) && eq( type, "paper" )
		|| eq( weapon1, "Blade of Doom" ) && eq( type, "scissors" )
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

castor::relation weaponEffectOnWeapon(
	castor::lref<std::string> weapon1, castor::lref<std::string> weapon2, castor::lref<int> effectMultiplier
	)
{
	return weaponIsStrongerThanWeapon( weapon1, weapon2 ) && eq( effectMultiplier, 1.5 )
		|| eq( effectMultiplier, 0.5 )
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

//unit vs terrain type
//


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
