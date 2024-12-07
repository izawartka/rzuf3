# RZUF3

RZUF3 is a simple and efficient game framework based on SDL2. It allows for building scenes and objects with assigned scripts. Communication between scripts is done through events. Each scene has its own event manager broadcasting global events (such as update, draw, mouse events), as well as object event managers for peer-to-peer communication between scripts within the same object. The framework also offers an assets manager, preventing multiple loading of the same asset by different scripts/objects.

## Sample Project:
You can find a repository of a sample project [here](https://github.com/izawartka/siudemka). It can serve as a good base for your own project.

### Dependencies
RZUF3 requires the following libraries: `SDL2`, `SDL2_image`, `SDL2_ttf`, `spdlog`. More info in the [sample project](https://github.com/izawartka/siudemka) repository.

### Author:
masuo / izawartka
