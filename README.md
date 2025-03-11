# 2D Game Engine

A custom 2D game engine built in C++ with SDL, developed as part of JHU 605.668: Computer Gaming Engines.

## Features

### Transformation System
- Complete transformation pipeline with local, world, and screen coordinate spaces
- Matrix stack implementation for hierarchical transformations
- Support for translation, rotation, and scaling operations

### Camera System
- Flexible camera implementation with controllable viewport
- World-to-screen space transformation
- Camera movement and zoom functionality
- Integration with the scene graph through CameraNode

### Scene Graph
- Hierarchical node structure
- Parent-child relationships with transformation inheritance
- Specialized node types (GeometryNode, CameraNode, etc.)

### Input Handling
- Three-layer input architecture for maximum flexibility:
  1. Raw device event capture through SDL
  2. Device-agnostic game action translation
  3. Centralized IO handler for the input pipeline
- Decoupled input system allowing for easy remapping and device independence

### Rendering
- Texture management through TextureNode
- Geometry rendering with proper world-to-screen transformation
- Affine texture rendering using SDL

## Architecture

The engine employs a scene graph architecture where each node can have a transformation and children. The matrix stack accumulates transformations as it traverses the scene hierarchy, properly handling parent-child relationships. The camera system converts world coordinates to screen coordinates for rendering.

The input system decouples raw device events from game logic, allowing for flexible control schemes and future input device support.

## Roadmap

- Animation System (Completed 3/6/25): Implementation of sprite-based animation and animation state management
- **Physics System** (In Development): Basic collision detection and response
- Particle System: Support for particle effects
- Audio System: Sound effect and music playback
- UI Framework: In-game user interface components

## License

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
