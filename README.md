# RZUF3

RZUF3 is a simple and efficient game framework based on SDL2. It allows for building scenes and objects with assigned scripts. Communication between scripts is done directly or through events. Each scene has its own event manager broadcasting global events (such as update, draw, mouse events), as well as object event managers for peer-to-peer communication between scripts within the same object. The framework also offers language and config files support, as well as an assets manager, preventing multiple loading of the same asset by different scripts/objects.

## Sample projects list
Sample RZUF3 projects. They can serve as a good base for your own game/app.  
- [izawartka/siudemka](https://github.com/izawartka/siudemka)
- [izawartka/mathgraph](https://github.com/izawartka/mathgraph)
- [izawartka/pressuresand](https://github.com/izawartka/pressuresand)

### Dependencies
RZUF3 requires the following libraries: `SDL2`, `SDL2_image`, `SDL2_ttf` (min. >=2.0.18, recommended >=2.20.0), `spdlog`. 

### Building
Detailed building instructions for Windows and Linux can be found in sample projects README files.

### Author:
masuo / izawartka
