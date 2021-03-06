# Imperium Editor

Map editor for the upcoming iPad game Imperium. This editor allows you to create scenarios for the game and in fact all the included scenarios are done with this editor.

This README is definitely a work in progress and is right now quite useless.

## Compiling
The editor requires Qt 4.x. Will probably work with Qt 5 too, but the libraries have changed a bit and the .pro file needs to be updated. Should compile ok on any OS that has Qt.

	% qmake ImperiumEditor.pro
	% make

Or similar. The build ends up in `imperium-editor-build` next to the source tree.

## Map type
Maps have a size, title and a description. The size is in meters. The title is used on the scenario selection screen in the game and should thus not be too long. Look at existing names for ideas. The description is shown when a scenario is tapped in the scenario selection screen. It can contain a few lines of text, but not too much. Look at existing scenarios to see what can safely be used. The description should give a short explanation to the player what the scenario is about and what the player's goals are.

Each scenario has a time when it starts. This is given as a normal 24 hour time. The duration is given in seconds (perhaps to be changed to minutes or so later). 1h == 3600s. You don't do much in one hour of game time, but nobody wants to play a scenario that takes 5h real time to complete either.

### Id and depends
These values give the scenario an unique id. The id is used all over the game. The *depends* value says which scenario must be completed before this one can be played. It's used to make only certain scenarios playable until the player has won enough scenarios.

### AI hint
This is a hint to the AI code and basically states what kind of battle it is. The AI will adapt its strategy based on the hint.

### Tutorial
Leave unchecked. Only used for the first few tutorial maps in the game.


## Terrain
An empty map is always filled with grass, even if you add no other terrain elements. Terrain is made up from closed polygons. When you add terrain just click on the map where you want the terrain to start and then click to add more points to the terrain. All terrain polygons are always closed, but you need to add at least three points to see the terrain polygon.

Don't go wild and add insane amounts of points to terrains, it all needs to be rendered in the end by the game engine. The terrain polygons can be a bit rough and when a scenario loaded by the game a simple form of smoothing is automatically done to sharp corners.

## Units
Add units to both players (doh!). Try to keep the troops balanced and suitable to the type of scenario you are designing. There are no real limitations to troops, but hundreds of units are not really manageable anymore.

## Organizations
Organizations are companies and batteries formed into brigades or other larger formations. Add headquarter units and assign them as headquarters for the wanted units.

## Objectives
Objectives are the targets that both players will try to capture. A normal map has some objectives, but they are not mandatory. Don't add too many objectives, 5 is probably a sane maximum.

## Map limitations
* The maps should not be too big, as the real map engine probably croaks with too big maps. The size you can give for the map in the editor is in meters. Sizes a big as 3 km x 3 km should be fine, depending on the amount of extra terrain.

* Infantry units are limited to a maximum of 100 men.

* Cavalry and artillery units are limited to a maximum of 60 men.

* Artillery units can also have a maximum of 10 men per gun. So a unit with 4 guns can have a minimum of 4 guns.

## Testing scenarios
Currently that is not even possible. :) The scenarios have to be included in a build of the game which is not even released yet. In the future I will add some way to test maps without recompiling the game, perhaps by entering an URL to a scenario. That would allow scenarios to be put on some web server somewhere for testing. Not there yet.
