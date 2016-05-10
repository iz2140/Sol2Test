# Sol2Test

This is a demonstration project for the Sol library. See the "UsingSol" branch to compare it with master. This is a side-scroller based on a volunteer project for a client that was based in Lua using CoronaSDK. The basic idea of the game is that the player would control a protagonist on the screen that can shoot bullets. Enemies will randomly generate and move in different speeds towards the protagonist. When a bullet hits an enemy, that counts as collision detection and both enemy and bullet disappear. 

To build the Demo Project, you must have Xcode installed with SFML, Lua, and set the linker flags, build settings, architecture, etc as appropriate.

1. Using SFML with XCode: http://www.sfml-dev.org/tutorials/2.3/start-osx.php
2. Install Lua locally: http://www.lua.org/manual/5.2/readme.html
3. Place the lua directory containing the  install folder that was produced from this step 2 into the root directory of the Xcode project. 
4. Install Sol2 and place sol.hpp and Sol source files into the same directory that main.cpp will run in. 

