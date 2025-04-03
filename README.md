# Custom 2D Game Engine

A modular 2D game engine written in modern C++ with SDL. Designed with real-time performance and clean architecture in mind, it features a scene graph with hierarchical transforms, input abstraction, sprite animation, 3D audio spatialization, and an early-stage physics/collision system.

---

## 🔧 Architecture Overview

- **Scene Graph**: Tree-based node system supporting composition of transforms, sprites, cameras, and UI elements.
- **Scene Manager**: Stack-based system enabling runtime push/pop/replace of active scenes (pause menus, game levels, etc.).
- **Input System**: Multi-layered handling of raw input, mapped game actions, and contextual behavior per scene or entity.
- **Update Pipeline**: Consistent frame timing and per-node updates across all active scene graph elements.

---

## 🧩 Core Systems

### 💾 Serialization
- Flexible serialization framework with text and binary formats
- Serializable interface for game objects with consistent save/load behavior
- SaveManager for centralized game state persistence
- Automatic endianness handling for cross-platform binary compatibility
- Key-value based storage with strong type safety

### 🖼️ Animation
- Frame-based sprite animation with shared texture grids
- Looping, one-shot, and dynamic switching (e.g. run → idle)
- Modular animation binding for any transform node

### 🔊 Audio
- FMOD-based engine with positional 3D audio
- Per-entity audio components with spatial delay, echo, and channel control
- Background music and interactive SFX support

### 🧱 Collision
- Modular collision system with AABB and circle colliders
- Dynamic component registration and per-frame collision state tracking
- Integrated sound triggers and behavior changes on collision events

### 🎥 Camera
- World-to-screen projection with smooth following
- Click-based world coordinate inspection (debug/testing)
- Designed to encapsulate all visual elements for clean viewport control

---

## 🗺️ Example Scene

The provided `MainScene` demonstrates:

- Dynamic character animation (Golem + Witch)
- Sound playback on input and collisions
- NPC clap behavior with timers
- Collision boundaries and NPC/player interactions
- Scene-rooted camera with entity follow targeting
- Serialization of player position between game sessions

---

## 🛠️ Usage

This project is designed to be built and run directly in **Visual Studio (Windows)**:

1. Clone the repository:
   ```bash
   git clone https://github.com/nicolasnotcage/GameEngine.git
   ```
2. Open the `.sln` file in Visual Studio 2022 (or later).
3. Set the build configuration to Release or Debug, and ensure the target platform is set to `x64`.
4. Ensure DLLs for FMOD and SDL2 are located in the `build` directory.
5. Build and run using Ctrl+F5 or the green Start button.
## 🚀 Roadmap

- ✅ Sprite Animation
- ✅ Audio Engine
- ✅ Collision System
- ✅ Dynamic Scene Stack
- ✅ Serialization
- 🟡 UI Layer & Menus
  
---

## 📂 Structure

```plaintext
graph/          → Node types: TransformNode, SpriteNode, CameraNode, etc.
platform/       → Core engine modules (input, audio, collisions, config)
examples/       → MainScene demo setup with animated sprites and interactions
system/         → File path management, serialization, configuration, and utilities
```

## 🧾 License
This project is released into the public domain under [The Unlicense](https://unlicense.org/).
