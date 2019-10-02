*****************************************************************************************************************

Hey there!
This project is a Unreal Engine 4.22 template to get you started developping a City Builder styled game. 
It contains basic features such as implementing buildings (also called Blocks here), placing them down
on a grid that is customisable using a custom player pawn which behaves like it would in any sim/strat game.

I used this project to get me started with C++ programming in Unreal Engine, so some things might not be as good
as they could be. Make sure to check out the documentation at samuelmetters.com/TinyTown to learn more on how to 
use this template. 

*****************************************************************************************************************

Here is a breakdown of every major class the templates contains:

GridManager - Actor class used to build custom grids the rest of the templates relies on.
PlayerGridCamera - A pawn class with typical sim like movements and basic grid interaction logic.
Blockmanager - Actor class that keeps track of every block that is placed on the grid.
Block - Actor class, the most abstract class of object that can be placed on the grid.
Pathfinder - Component, allows grid based pathfinding operations to be executed.
GameState - GameState class used to manage time.

Every class should be documented (both in C++ and Blueprints), if you still have a question make sure to check out 
the documentation at samuelmetters.com/TinyTown.