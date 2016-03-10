EECS 371 TERM PROJECT

FIRE EMBLEM KR

SHAWN CAEIRO
HYUNG-SOON KIM
HOSUNG KWON

HOW TO RUN THE PROGRAM

1. Run main.exe and type "standard" when prompted.
2. If main.exe does not run, recompile the program using C++11 and GCC 4.8 or greater.
3. For details on "custom" setups, please refer to the project pdf or follow the steps in the prompt. 

FILE DESCRIPTIONS:

main.cpp: Contains the code for initializing the board and setting up the game, as well as the primary game
loop.

KB.h: Contains facts and relations that contain the information base of the game, such as game rules, unit
attributes, map representation, and so on.

Phases.h: Contains functions that dictate the flow of the game, as well as allow a unit to make an action
based on its strategy type.

HOW TO EDIT UNIT ATTRIBUTES:

1. For most attributes, in the file KB.h, find the relation called onTeam(), and edit the information found in there to modify,
add, or delete unit attributes.
2. For strategy type, find the relation characterStrategyType, and for weapon type, find weaponIsType.
3. All other attributes (dialogue, weapons, terrain, etc.) are also freely editable: disjunctions may be used to add new info.
