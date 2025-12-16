# C++ Client–Server Multiplayer Game Prototype

This repository contains a C++ client–server multiplayer game prototype developed as a personal learning and experimentation project.
The project focuses on implementing core systems commonly found in real-time online games, with particular attention to
networking, server-authoritative simulation, state synchronization, and modular software architecture.

Visual fidelity and game content are considered secondary to system correctness and architectural clarity.

---

## Project Objectives

The primary objectives of this project are to study, design, and implement:

- Client–server architecture for real-time multiplayer games
- Server-authoritative world simulation
- Client-side prediction and state reconciliation
- Snapshot-based world synchronization
- Modular and extensible game system architecture
- Basic persistence mechanisms for player-related data

This project is not intended to be a complete or production-ready game.

---

## Implemented Systems

### Networking

- Client–server communication using both TCP and UDP
- Server-managed client connection lifecycle
- Unique client identifier assignment on connection
- Client-to-server input transmission
- Server-to-client world snapshot broadcasting
- Snapshot-based synchronization model

### Client-side Simulation

- Client-side movement prediction
- Input buffering and reconciliation against authoritative snapshots
- Snapshot interpolation infrastructure
- Decoupling of simulation logic and rendering logic
- Camera system following the local player using SFML views

### Rendering

- Rendering implemented using SFML
- Clear separation between:
  - World-space rendering
  - Screen-space (UI) rendering
- Dedicated rendering pipeline for debug visualization
- Minimalistic visual representations intended for development and debugging

### Entity Management

- Centralized client-side entity management system
- Management of the following remote entities:
  - Player characters
  - Projectiles
  - Melee attack entities (sword slashes)
- Snapshot application for entity state updates
- Per-frame entity update logic on the client

### Server-side Game Logic

- Fully server-authoritative game world simulation
- Input processing pipeline on the server
- Combat system handling damage and attack resolution
- Interest management system for limiting irrelevant updates
- Event-based server architecture
- Entity lifetime creation, update, and destruction

### Inventory and Equipment System

- Slot-based inventory system
- Dedicated equipment slots for player characters
- Inventory and equipment logic implemented exclusively on the server
- Inventory and equipment state synchronized to clients
- Client-side inventory user interface
- Drag-and-drop interaction for inventory management on the client

### Data Persistence

- Player inventory and equipment persisted using JSON files
- One JSON file per player
- Load and save operations handled on the server
- Persistence designed for simplicity and prototyping purposes

---

## Directory Structure

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
│   │   ├── Networking, prediction, rendering, and UI systems
│   │   ├── Client-side inventory and equipment handling
│   │   └── Snapshot definitions and remote entity representations
│   │
│   ├── Server/
│   │   ├── Game world and server-side systems
│   │   ├── Combat, input processing, and interest management
│   │   ├── Inventory and equipment logic
│   │   └── Network server implementation
│   │
│   └── Shared/
│       ├── Shared constants
│       ├── Input state definitions
│       └── Utility functions
│
└── Makefile

---

## Build and Execution

### Requirements

- C++17 compatible compiler
- SFML library
- Windows environment
  - MSYS2 / MinGW is recommended

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
* No collision detection or resolution system
* No non-player character (NPC) or AI logic
* Persistence based solely on JSON files (no database integration)
* Visuals intended only for debugging and development
* No authentication, encryption, or security mechanisms

---

## Future Work

* Tile-based or chunk-based map system
* Collision detection and resolution
* NPC and enemy AI systems
* Skill and ability framework
* Improved snapshot interpolation and smoothing
* Database-backed persistence layer
* Login, authentication, and session management

---

## Notes

This project prioritizes architectural soundness and correctness of core systems over gameplay content.
The codebase is intentionally structured to remain readable, modular, and extensible, enabling further experimentation
with multiplayer game programming concepts and techniques.