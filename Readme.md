# C++ Client–Server Multiplayer Game Engine

![Language](https://img.shields.io/badge/language-C%2B%2B17-blue.svg)
![Library](https://img.shields.io/badge/library-SFML-green.svg)
![Platform](https://img.shields.io/badge/platform-Windows-lightgrey.svg)

A robust prototype of a real-time multiplayer game engine built from scratch using C++ and SFML. This project demonstrates core concepts of network programming, server-authoritative architecture, state synchronization, and custom game physics.

Unlike typical game projects that rely on Unity or Unreal, this project implements the low-level systems manually to understand the "how" and "why" behind multiplayer game architecture.

---

## 🌟 Key Features

### 📡 Networking & Architecture
- **Server-Authoritative Model:** The server owns the game state; clients are merely "dumb" terminals that send inputs and render the state.
- **Protocol:** Custom packet handling over TCP/UDP sockets (using SFML Network).
- **Snapshot Synchronization:** The server broadcasts world snapshots to clients at a fixed tick rate.
- **Interest Management:** Bandwidth optimization by only sending entity data relevant to specific clients (based on proximity/chunks).
- **Event System:** Discrete event handling for actions like `EquipItem`, `MoveItem`, `NewClient`, `DeleteClient`.

### 🎮 Client-Side Simulation
- **Client-Side Prediction:** Immediate local movement response to hide latency.
- **Reconciliation:** Correcting local state if it diverges from the authoritative server snapshot.
- **Entity Interpolation:** Smooth rendering of remote entities between snapshot updates.
- **UI & Rendering:**
  - Inventory UI with Drag & Drop support.
  - Debug rendering layer (hitboxes, chunks, vectors).

### ⚔️ Gameplay & Systems
- **Combat System:**
  - Melee attacks (Sword Slashes) with hitbox detection.
  - Projectile system for ranged attacks.
  - Health and Damage handling on the server.
- **Inventory & Equipment:**
  - Slot-based inventory managed strictly on the server to prevent cheating.
  - Equipment slots that modify player stats/appearance.
  - JSON-based persistence for player data (Inventory/Stats).
- **Physics & World:**
  - **Chunk System:** Infinite/Tiled world management (loading/unloading chunks).
  - **AABB Collision:** Custom axis-aligned bounding box collision detection and resolution.
  - **Physics System:** Gravity, velocity, and friction handling.

---

## 📂 Project Structure

The project is divided into Client, Server, and Shared codebases.

```bash
.
├── Assets/                 # Fonts and JSON Data (Player persistence)
├── Build/                  # Compiled executables
│   ├── GameClient.exe
│   └── GameServer.exe
├── Sources/
│   ├── Client/             # Client-side Logic
│   │   ├── Core/           # Main loop, Renderer
│   │   ├── Debug/          # Debug visualization tools
│   │   ├── Entities/       # Remote representations (interpolated)
│   │   ├── Inventory/      # UI logic for items
│   │   ├── Network/        # Socket handling & Snapshot parsing
│   │   ├── Snapshots/      # Data structures for received world state
│   │   ├── Systems/        # Prediction, Input, Interpolation
│   │   └── UI/             # User Interface elements
│   │
│   ├── Server/             # Server-side Logic
│   │   ├── Core/           # Game Loop, World State
│   │   ├── Chunk/          # Map management & Spatial hashing
│   │   ├── Entities/       # Authoritative Entity logic (AI, Player)
│   │   ├── Network/        # Packet broadcasting & Event handling
│   │   └── Systems/        # ECS-style systems
│   │       ├── Combat/     # Damage calculation
│   │       ├── Input/      # Processing client requests
│   │       ├── Interest/   # Culling network traffic
│   │       ├── Inventory/  # Item logic & Persistence
│   │       └── Physics/    # Collision & Movement
│   │
│   └── Shared/             # Common Utils, Constants, Math, AABB
└── Makefile                # Build configuration
```

---

## 🛠️ Build & Installation

### Prerequisites
- **Compiler:** g++ (MinGW-w64 recommended) supporting C++17.
- **Library:** SFML 2.5+.
- **Make:** GNU Make.

### Compilation
The project uses a Makefile to handle separate builds for Client and Server.

```bash
# Build both Client and Server
make

# Clean build artifacts
make clean
```

### Running the Game
1. **Start the Server** (Must run first):
   ```bash
   ./Build/GameServer.exe
   ```
2. **Start one or multiple Clients**:
   ```bash
   ./Build/GameClient.exe
   ```

---

## 🧠 What I Learned & Engineering Challenges

Developing this engine provided deep insights into systems programming:

1.  **Latency Compensation Techniques:**
    *   *Challenge:* Without prediction, movement feels sluggish.
    *   *Solution:* Implemented Client-side Prediction and Reconciliation to store input history and replay it when server correction arrives.

2.  **Bandwidth Optimization (Interest Management):**
    *   *Challenge:* Sending the entire world state to every player kills performance.
    *   *Solution:* Implemented a `ChunkSystem` and `InterestSystem` to only sync entities within the player's view range.

3.  **Memory Management & Pointers:**
    *   Extensive use of smart pointers and raw pointers for Entity management to avoid memory leaks while maintaining performance in the game loop.

4.  **Concurrency & Sockets:**
    *   Handling non-blocking sockets and ensuring the server processes inputs from multiple clients fairly within a fixed timestep tick.

5.  **Data Persistence:**
    *   Designing a JSON serialization system to save/load player states (Inventory, Position) securely on the server side.

---

## 🚀 Roadmap & Future Work

- [ ] **Database Integration:** Migrate from JSON files to SQLite/MySQL for robust data handling.
- [ ] **Advanced AI:** Implement Behavior Trees or State Machines for smarter Enemies.
- [ ] **Security:** Add handshake authentication and basic packet encryption.
- [ ] **Asset Pipeline:** Replace debug primitives with Sprite animations and Tilemaps.
- [ ] **Chat System:** Implement a TCP-based chat overlaid on the game.

---

## 📝 License

This project is for educational purposes. Feel free to use the code to learn about multiplayer game architecture.