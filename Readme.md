```md
# C++ Client–Server Multiplayer Game Prototype

This repository contains a C++ client–server multiplayer game prototype developed as a personal learning project.
The project focuses on implementing core real-time multiplayer game systems, with an emphasis on networking,
state synchronization, and modular architecture rather than graphics or content.

---

## Project Scope

The objective of this project is to study and implement:
- Client–server architecture for real-time games
- Server-authoritative world simulation
- Client-side prediction and state reconciliation
- Modular system design for game logic
- Basic persistence for player data

The project is not intended to be a complete game.

---

## Implemented Systems

### Networking

- Client–server communication using TCP and UDP
- Server assigns unique client identifiers on connection
- Client sends input data to the server
- Server broadcasts world snapshots to connected clients
- Snapshot-based world state synchronization

### Client-side Simulation

- Client-side movement prediction
- Input buffering and reconciliation
- Snapshot interpolation infrastructure
- Separation of simulation and rendering logic
- Camera system following the local player using SFML views

### Rendering

- SFML-based rendering
- Separation between world rendering and UI rendering
- World-space rendering for entities
- Screen-space rendering for user interface
- Debug-oriented visual representation of entities

### Entity Management

- Client-side entity manager maintaining:
  - Remote players
  - Projectiles
  - Melee attack entities (sword slashes)
- Centralized snapshot application for world state updates
- Per-frame entity updates on the client

### Server-side Game Logic

- Authoritative game world simulation
- Player input processing system
- Combat system for damage and attack handling
- Interest management system for relevant entity updates
- Event-based server architecture
- Entity lifetime management

### Inventory and Equipment

- Slot-based inventory system
- Equipment slots for player characters
- Inventory and equipment logic implemented on the server
- Inventory and equipment state synchronized to clients
- Drag-and-drop inventory user interface on the client

### Data Persistence

- Player inventory and equipment stored in JSON format
- One JSON file per player
- Load and save operations handled on the server

---

## Directory Structure

```

.
├── Assets/
│   ├── Roboto_Mono.ttf
│   └── Data/
│       └── Players/
│           ├── player_0.json
│           ├── player_1.json
│           └── player_2.json
│
├── Build/
│   ├── GameClient.exe
│   ├── GameServer.exe
│   ├── Client/
│   └── Server/
│
├── Sources/
│   ├── Client/
│   │   ├── Networking, prediction, rendering, UI
│   │   ├── Inventory and equipment (client-side)
│   │   └── Snapshot and remote entity representations
│   │
│   ├── Server/
│   │   ├── Game world and server systems
│   │   ├── Combat, input, and interest management
│   │   ├── Inventory and equipment logic
│   │   └── Network server implementation
│   │
│   └── Shared/
│       ├── Shared constants
│       ├── Input state definitions
│       └── Utility functions
│
└── Makefile

````

---

## Build and Execution

### Requirements

- C++17 compatible compiler
- SFML library
- Windows environment (MSYS2 / MinGW recommended)

### Build

```bash
make
````

### Run

```bash
Build/GameServer.exe
Build/GameClient.exe
```

---

## Current Limitations

* No map loading or tile-based world representation
* No collision system
* No non-player character (NPC) logic
* No database-backed persistence
* Minimal visual representation for debugging purposes
* No authentication or security mechanisms

---

## Future Work

* Tile-based map system
* Collision detection and resolution
* NPC and enemy AI systems
* Skill and ability system
* Improved snapshot interpolation
* Database-backed persistence
* Login and authentication system

---

## Notes

This project prioritizes architectural clarity and system correctness over content and visual quality.
The codebase is intended to remain readable and modular to support further experimentation and extension.