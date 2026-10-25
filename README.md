---
project_title: "Tenebris and Lux"
subtitle: "An Atmospheric Game & Engine Architecture Written in C"
version: "0.0.1"
organization: "CreativeWorkzStudio LLC"
repository: "https://github.com/Novaking34/tenebris-lux"
license: "MIT"
---

# Tenebris and Lux

> **"The Shield protects and confines; the Sword severs and defends. The metal knows neither virtue nor malice—only the hand that grips the hilt."**

*Tenebris and Lux* ("Darkness and Light") is a C game project and custom engine architecture. The story chronicles the epic struggle within the divided Kingdom of **Sol-Tenebrae**, threatened by the re-awakening of the **Dark Lord Malakor**. At the heart of the journey lie two neutral, primordial armaments—**The Shield of Lux** and **The Sword of Tenebris**—whose power can be wielded for righteous salvation or tyrannical devastation depending on the bearer's choices.

---

## Manifest Metadata & Covenant Identity

| Attribute | Specification |
| :--- | :--- |
| **Engine Version** | `0.0.1` (Early Prototype Milestone) |
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

In *Tenebris and Lux*, Light and Darkness are not moral absolutes of good and evil; they are apex primordial forces personified.

As the chosen Champion awakening at the *Garrison Altar*, you bear the ancient **Shield of Lux** and the **Sword of Tenebris**. You must navigate a Kingdom fractured by civil strife between the dogmatic Order of the Radiant Solar and the secretive Umbral Covenant, while preparing to confront the awakened Dark Lord Malakor. Every tactical choice, ward, and strike sways whether you become a righteous savior or a terrifying tyrant.

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

For in-depth architectural specifications, see [ARCHITECTURE.md](file:///home/professorseanex/Tenebris%20and%20Lux/docs/ARCHITECTURE.md) and [dual_frontend_architecture.adoc](file:///home/professorseanex/Tenebris%20and%20Lux/docs/dual_frontend_architecture.adoc).

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

- `look` / **(L)**: Inspect current location, faction tensions, and surroundings.
- `status` / **(I)**: Inspect the Shield of Lux, Sword of Tenebris, Stance, and Moral Sway.
- `ward` / **(W)**: Raise the Shield of Lux to establish a radiant barrier or reveal hidden truths.
- `strike` / **(S)**: Unleash the Sword of Tenebris to sever enemy defenses or cloak in shadow.
- `balance` / **(B)**: Seek harmony between the twin relics to restore equilibrium.
- `stance <ward|strike|equilibrium>` / **(T)**: Shift tactical and metaphysical stance.
- `move <n|s|e|w>` / **(N, S, R, W)**: Travel across the fractured Kingdom provinces.
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
- [docs/dual_frontend_architecture.adoc](file:///home/professorseanex/Tenebris%20and%20Lux/docs/dual_frontend_architecture.adoc): The Dual CLI + GUI paradigm and architectural rationale (AsciiDoc).
- [docs/world_and_lore.adoc](file:///home/professorseanex/Tenebris%20and%20Lux/docs/world_and_lore.adoc): Foundations of world-building, Lumina shards, and the cosmology of Lux & Tenebris (AsciiDoc).
- [AGENTS.md](file:///home/professorseanex/Tenebris%20and%20Lux/AGENTS.md): Contributor and AI agent coding guidelines, style rules, and commit discipline.
