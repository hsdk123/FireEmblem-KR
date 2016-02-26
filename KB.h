#pragma once

castor::relation gender( castor::lref < std::string > p, castor::lref<std::string> g )
{
	using namespace castor;
	return eq( p, "Frank" ) && eq( g, "male" )
		|| eq( p, "Mary" ) && eq( g, "female" )
		|| eq( p, "Sam" ) && eq( g, "male" )
		;
}

//stats: HP, Attack, and Movement
struct charInfo
{
	std::string _name;
	std::string _teamName;
	int _hp;
	int _attack;
	int _movement;
};
using CharInfoMap = std::map<std::string/*name*/, charInfo>;

//stats: HP, Attack, and Movement
castor::relation onTeam(
	castor::lref<std::string> charName, castor::lref<std::string> teamName,
	castor::lref<int> hp, castor::lref<int> attack, castor::lref<int> movement )
{
	using namespace castor;
	return eq( charName, "Lance" ) && eq( teamName, "A" ) && eq( hp, 2 ) && eq( attack, 1 ) && eq( movement, 1 )
		|| eq( charName, "Arthur" ) && eq( teamName, "A" ) && eq( hp, 1 ) && eq( attack, 1 ) && eq( movement, 1 )
		|| eq( charName, "Diana" ) && eq( teamName, "B" ) && eq( hp, 10 ) && eq( attack, 5 ) && eq( movement, 1 )
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
		if ( charInfos[charName.get()]._hp > 0 )
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

using CharInfoList = std::list<charInfo>;
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