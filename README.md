# HiveMind Engine

## Description

HiveMind engine is a project from [Mario Hernandez](https://github.com/MHF13), [Oriol Valverde](https://github.com/Makinilla-maker), [Marc Ruiz](https://github.com/Ruizo) group of students from CITY university.

## Features:

### Engine v0.1
- In our engine, you can create base forme as a cube, pyramid, sphere or cylinder. But if you want to use your own files, you can drag and drop them to your project.
- To drag and drop a texture, first select the ojects in hierarchie and then drop the file.
- You can transform position, rotation and scale your object with the inspector window.

### Engine v0.5
- We have a "Game" mode, which can be activated with the "Game State" button.
- Activating this mode changes the colour of the interface to give visual feedback.
- If you change elements while "Game State" is activated, they will return to their initial position when deactivated.
- All meshes use AABB and can be discarded using Frustrum culling Game mode when activated.
- We can select an object in the "Scene" window by LeftClick on it.
- The editor has 2 windows one with the scene(editor view) and another captured from a
gameobject with a camera component(main game camera), each one has its own
framebuffer and the user can visualize both at the same time.
- We can look at the project files from the same engine, create new folders and delete them.
- We have the "library" folder where the metadata is stored.
- The camera object has a guizmo for the frustrum.

## Controls
- WASD to move the camera in X's axis and Z's axis.
- RightClick to rotate the main camera.
- LeftClick to select.
- Pressing “f” should focus the camera around the geometry.
- Holding SHIFT duplicates movement speed.
- Mouse wheel should zoom in and out
- DELETE button to remove gameObjects

In Asset List:
- Can create folders with the button "Create folders"
- Once you have selected a folder, you can delete it by pressing DELETE.

## GitHub
https://github.com/Makinilla-maker/Engine3D

## License

This project is licensed under an unmodified MIT license, which is an OSI-certified license that allows static linking with closed source software. Check [LICENSE](LICENSE) for further details.

We have fork the teacher's project at the day of 9/11 for future usage.

For v0.2 we have used a fork of [this](https://github.com/solidajenjo/Engine3D) project