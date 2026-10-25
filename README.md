---
project_title: "Tenebris and Lux"
subtitle: "An Atmospheric Game & Engine Architecture Written in C"
version: "0.0.1"
organization: "CreativeWorkzStudio LLC"
repository: "https://github.com/Novaking34/tenebris-lux"
license: "MIT"
---

# Tenebris and Lux

> **"In the boundary where shadow converges upon light, only embers of wisdom endure."**

*Tenebris and Lux* ("Darkness and Light") is a C game project and custom engine initiative. The project starts with a robust, text-based interactive prototype to establish the core mechanics, state machine, and lore balance between **Lux** (light, hope, clarity) and **Tenebris** (creeping shadows and entropy), designed to evolve into 2D/2.5D visual layers and a custom 3D game engine.

---

## Manifest Metadata & Covenant Identity

| Attribute | Specification |
| :--- | :--- |
| **Engine Version** | `0.0.1` (Early Text Prototype Milestone) |
| **Organization** | CreativeWorkzStudio LLC |
| **Repository** | [https://github.com/Novaking34/tenebris-lux](https://github.com/Novaking34/tenebris-lux) |
| **Core Standard** | ISO C11 (`-std=c11`, zero external dependencies) |
| **Build System** | CMake 3.24+ (and direct C11 compiler support) |
| **Structural Paradigm** | OmniCode Universal 5-Block Code Lifecycle Protocol |
| **Invariant Discipline** | Diagnostic Audit Procedures & Strict State Boundaries |
| **Author / Lead** | ProfessorSeanEX / CreativeWorkzStudio LLC |
| **License** | MIT License |

---

## 1. Project Vision & Lore

In *Tenebris and Lux*, light is not merely illumination—it is energy, life, and spiritual cohesion.

As a traveler awakening in *The Fading Sanctuary*, you must navigate forgotten stone corridors while preserving your fading **Lux** (Light). Each step through the dark wanes your aura. To survive the creeping **Tenebris**, you must explore uncharted chambers, discover ancient **Lumina Shards**, and kindle them to push back the abyss.

---

## 2. The Universal 5-Block Architectural Standard

Inheriting the structural discipline of the **OmniCode** framework, every source module, header, and configuration file in this repository is partitioned into 5 explicit blocks:

```text
┌────────────────────────────────────────────────────────────────────────┐
│                        BLOCK 1: METADATA BLOCK                         │
│  Identity, Provenance, Invariant Boundary Contracts & System Anchors   │
├────────────────────────────────────────────────────────────────────────┤
│                    BLOCK 2: OPENING / TYPES BLOCK                      │
│  Imports, Preconditions, Enums, State Structures & Global Constants   │
├────────────────────────────────────────────────────────────────────────┤
│                         BLOCK 3: BODY BLOCK                            │
│  Core Logic, State Transitions, Execution Routines & Gameplay Loop     │
├────────────────────────────────────────────────────────────────────────┤
│                   BLOCK 4: AUDIT PROCEDURES BLOCK                      │
│  Diagnostic Audits, State Invariant Verifications & Telemetry Traces   │
├────────────────────────────────────────────────────────────────────────┤
│                BLOCK 5: CLOSING / TEST & CLI BLOCK                     │
│  CLI Entry Point, Automated Invariant Unit Tests & Execution Teardown  │
└────────────────────────────────────────────────────────────────────────┘
```

This discipline ensures:

- **Zero Ambiguity**: File responsibilities and state definitions are clear at a glance.
- **Embedded Self-Auditing**: Diagnostic routines verify runtime state integrity without crashing.
- **Renderer Decoupling**: Simulation logic remains pure and portable.

For in-depth architectural specifications, see [ARCHITECTURE.md](file:///home/professorseanex/Tenebris%20and%20Lux/docs/ARCHITECTURE.md) and [dual_frontend_architecture.md](file:///home/professorseanex/Tenebris%20and%20Lux/docs/dual_frontend_architecture.md).

---

## 3. Roadmap: From Prototype to Custom Engine

- [x] **Phase 1: Dual-Frontend Prototype (Current - v0.0.1)**
  - Pure C11 interactive simulation with zero external downloads.
  - Native double-buffered X11 GUI and terminal CLI running side-by-side.
  - Core mechanics: Lux decay, Lumina shard harvesting, room traversal, command parser.
  - Automated diagnostic audits (`--audit`) and CTest integration.
- [ ] **Phase 2: Core Engine Subsystems**
  - Modular engine packages (`src/core/`, `src/sim/`, `src/render/`).
  - Decoupled state dispatcher, tick system, event bus, and math/geometry primitives.
- [ ] **Phase 3: 2D / 2.5D Presentation Layer**
  - Tilemap rendering, dynamic ambient lighting shaders, sprite animations.
- [ ] **Phase 4: 3D Custom Engine**
  - Custom rendering pipeline (Vulkan/OpenGL), spatial audio, scene graph, and entity systems.

---

## 4. Quickstart: Build & Play

For detailed prerequisites, operating system setup, and troubleshooting, refer to [INSTALL.md](file:///home/professorseanex/Tenebris%20and%20Lux/docs/INSTALL.md).

### Quick Build with CMake

```bash
# 1. Configure the build directory
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

# 2. Compile the game
cmake --build build --config Debug

# 3. Launch the game (auto-detects native GUI, or fallback to CLI)
./build/tenebris-lux

# Launch explicitly in terminal CLI mode
./build/tenebris-lux --cli

# Launch explicitly in native desktop GUI mode
./build/tenebris-lux --gui
```

### In-Game Commands & Hotkeys

- `look` / **(L)**: Inspect current room and ambient surroundings.
- `status` / **(T)**: Check light level (Lux), Lumina shards, and survival turns.
- `explore` / **(E)**: Search the gloom for Lumina shards and ancient relics.
- `kindle` / **(K)**: Consume 1 Lumina shard to restore 30 Lux.
- `move <n|s|e|w>` / **(N, S, R, W)**: Journey into connected chambers.
- `help`: View all available actions.
- `quit` / **(Q)**: Exit the game.

### Automated Diagnostics & Testing

```bash
# Run internal self-audit (verifies state bounds and GUI layout)
./build/tenebris-lux --audit

# Run CTest verification suite
ctest --test-dir build -C Debug --output-on-failure
```

---

## 5. Repository Documentation Index

- [docs/INSTALL.md](file:///home/professorseanex/Tenebris%20and%20Lux/docs/INSTALL.md): Comprehensive compilation, installation, and troubleshooting guide.
- [docs/ARCHITECTURE.md](file:///home/professorseanex/Tenebris%20and%20Lux/docs/ARCHITECTURE.md): Technical deep-dive into the 5-block structure and game mechanics.
- [docs/dual_frontend_architecture.md](file:///home/professorseanex/Tenebris%20and%20Lux/docs/dual_frontend_architecture.md): The Dual CLI + GUI paradigm and architectural rationale.
- [docs/world_and_lore.md](file:///home/professorseanex/Tenebris%20and%20Lux/docs/world_and_lore.md): Foundations of world-building, Lumina shards, and the cosmology of Lux & Tenebris.
- [AGENTS.md](file:///home/professorseanex/Tenebris%20and%20Lux/AGENTS.md): Contributor and AI agent coding guidelines, style rules, and commit discipline.
