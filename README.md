# Cybrelink

This is a small Uplink mod to make important quality of life improvements to the game. I don't have any particular goals here, other than to make the game a bit more fun to play without modifying the core gameplay. 

## Improvements

#### Hotkeys / Shortcuts
* Middle clicking on links (like on the main screen) will load your saved bounce path before connecting.
* Middle clicking on an account on the accounts view will fill in its details on the bank transfer page.
* Middle clicking on a password input box will run the password breaker automatically.
* Middle clicking on a decypher interface will run the decypher tool automatically.
* Middle clicking on a LAN system will run the LAN probe tool automatically.
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

I have gotten this building on Windows in Visual Studio. Building for any other platforms or on other IDEs I can't help you with.