# Cybrelink

This is an Uplink mod I started to make small quality-of-life changes for things that got a bit grating after fifty hours of play. At this point, it's an aimless spare-time project to refactor as much of Uplink's source code as I can.

## Gameplay Changes

#### Hotkeys / Shortcuts
* Middle clicking on links (like on the main screen) will load your saved bounce path before connecting. If you're connecting to a central mainframe computer, it will connect through that company's internal services system if you've discovered it.
* Middle clicking on an account on the accounts view will fill in its details on the bank transfer page.
* Middle clicking on a password input box will run the password breaker automatically.
* Middle clicking on a decypher interface will run the decypher tool automatically.
* Middle clicking on a LAN system will run the LAN probe tool automatically.
* Middle clicking on the "Connect" button on a LAN system will connect directly back to it without hiding the LAN screen.
* Pressing F2 anywhere in-game will connect you directly to the Uplink Internal Services System.
* Pressing F3 anywhere in-game will connect you directly to InterNIC.

#### UI
* Added a mail view interface tab, which lists all emails in a simpler interface, and makes deleting a bunch of emails a lot easier. There is no scrollbar on this interface yet.
* The mouse wheel now scrolls the main links interface.
* The mouse wheel now zooms on the world map.
* If you are connected to a server that you have a mission for, the mission will be highlighted with a border around it.

#### Misc
* Added an auto bounce path button on the map interface. This will make a path through all found links, starting with InterNIC.
* Added an add all button on the top left of long lists of links (like the InterNIC browse page) that saves all of the links.
* Added an auto-bypass button on the connection analyser page.

## Building

The project has been upgraded to use CMake for building and vcpkg for dependencies. To build the project, have both CMake 3.15+ and vcpkg in your path and run the following commands in the project's root directory:
```
cmake -B out -S .
# to make a debug FULLGAME build
cmake --build out --config debug
# to make a debug DEMOGAME build
cmake --build out --config debug -DUPLINK_BUILD_TYPE=demo
# to make a debug TESTGAME build
cmake --build out --config debug -DUPLINK_BUILD_TYPE=test
# to make a release FULLGAME build
cmake --build out --config release
```

This will write uplink-game.exe to the uplink/bin directory. This is where you should put the contents of your game directory in order to test the build.