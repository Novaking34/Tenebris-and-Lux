---
project_title: "Tenebris and Lux"
subtitle: "An Atmospheric Tactical Strategy RPG & Custom Engine Architecture Written in C"
version: "0.0.1"
organization: "CreativeWorkzStudio LLC"
repository: "https://github.com/Novaking34/tenebris-lux"
license: "MIT"
---

# Tenebris and Lux

> **"The Shield protects and confines; the Sword severs and defends. The metal knows neither virtue nor malice—only the hand that grips the hilt."**

*Tenebris and Lux* ("Darkness and Light") is a custom-engineered **Tactical Strategy RPG (TRPG)** and C11 game engine architecture. Conceived between 2014–2018 in RPG Maker as the foundational project of **CreativeWorkzStudio**, the story chronicles the defense of the chivalric **Kingdom of Sacyra** (pronounced *"saw-kee-ruh"*, `/sɔːˈkiːrə/`) against the re-awakened **Dark Lord Malakor**.

The journey is anchored by two protagonists: **Elijah Ro-enheart**, son of the former Head of the Royal Guard, and **Katrina Jaren**, a noble court aspirant drawn to the Adventurers' Guild. Together they awaken and wield two primordial, morally neutral relics—**The Shield of Lux** and **The Sword of Tenebris**—whose power shapes the destiny of the realm through player choices, tactical positioning, and moral sway.

---

## Manifest Metadata & Covenant Identity

| Attribute | Specification |
| :--- | :--- |
| **Engine Version** | `0.0.1` (Early Prototype Milestone) |
| **Genre** | Tactical Strategy RPG (TRPG) — 2D Grid Battlefield + Cinematic Side-View Duels |
| **Organization** | CreativeWorkzStudio LLC |
| **Repository** | [https://github.com/Novaking34/tenebris-lux](https://github.com/Novaking34/tenebris-lux) |
| **Core Standard** | ISO C11 (`-std=c11`, zero external dependencies) |
| **Build System** | CMake 3.24+ (and direct C11 compiler support) |
| **Structural Paradigm** | OmniCode Universal 5-Block Code Lifecycle Protocol |
| **Layered Strata** | Layer 0 (Util), Layer 1 (Sim), Layer 2 (GUI), Layer 3 (Entry), Layer 4 (Tests) |
| **Author / Lead** | ProfessorSeanEX / CreativeWorkzStudio LLC |
| **License** | MIT License |

---

## 1. Project Vision & Canonical Setting

In *Tenebris and Lux*, Light and Darkness are not moral absolutes of good and evil; they are primal cosmic forces personified.

* **The Kingdom of Sacyra (`/sɔːˈkiːrə/`)**: A historic kingdom governed by the Crown and the legendary **Royal Guard**, balanced against competing regional **Noble Houses** on the Royal Court, and the independent, meritocratic **Adventurers' Guild**.
* **Dual Protagonists**:
  * **Elijah Ro-enheart**: Lineage of royal army service; son of the former Head of the Royal Guard. Frontline heavy vanguard bearer of the Shield of Lux.
  * **Katrina Jaren**: Daughter of noble House Jaren; groomed for a seat on the Royal Court while longing for the freedom of the Adventurers' Guild. Agile outrider wielder of the Sword of Tenebris.
* **The Existential Threat**: **Dark Lord Malakor** breaking through the ancient Abyssal Vaults along Sacyra's frontier.

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

For in-depth architectural specifications, see [ARCHITECTURE.adoc](file:///home/professorseanex/Tenebris%20and%20Lux/docs/architecture/ARCHITECTURE.adoc) and [dual_frontend_architecture.adoc](file:///home/professorseanex/Tenebris%20and%20Lux/docs/architecture/dual_frontend_architecture.adoc).

---

## 3. Roadmap: From Prototype to Custom TRPG Engine

* [x] **Phase 1: Dual-Frontend Prototype (Current - v0.0.1)**
  * Pure C11 interactive simulation with zero external downloads.
  * Native double-buffered X11 GUI and terminal CLI running side-by-side.
  * Sacyra setting, Twin Relics, Stances, Moral Sway continuum `[-100, +100]`.
  * Layer 0 Core Utilities (`manifest`, `math_utils`, `string_utils`, `geom_utils`, `time_utils`).
  * Central Manifest Registry and Pre-Flight Subsystem Integrity Check (`--audit`) with loose-bolts tripwire and graceful degradation.
  * Automated diagnostic audits and CTest integration (6/6 tests passing in 0.02s).
* [ ] **Phase 2: Tactical Strategy RPG Grid Engine**
  * Discrete 2D grid coordinates, movement budgets, terrain defense bonuses, Zone-of-Control (ZOC).
  * Dual-protagonist party state modeling (Elijah Ro-enheart & Katrina Jaren).
* [ ] **Phase 3: Cinematic Side-View Combat Resolution**
  * Grid-to-duel scene transition, 2D side-scroll action clashes, timed warding/strikes, stagger meters.
* [ ] **Phase 4: Full Sacyra Campaign**
  * Tactical campaign across the provinces of Sacyra against Dark Lord Malakor.

---

## 4. Quickstart: Build & Play

For detailed prerequisites, operating system setup, and troubleshooting, refer to [INSTALL.adoc](file:///home/professorseanex/Tenebris%20and%20Lux/docs/INSTALL.adoc).

### Quick Build with CMake

```bash
# 1. Configure the build directory
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

# 2. Compile the game and test suites
cmake --build build --config Debug

# 3. Launch the game (auto-detects native GUI, or fallback to CLI)
./build/tenebris-lux

# Launch explicitly in terminal CLI mode
./build/tenebris-lux --cli

# Launch explicitly in native desktop GUI mode
./build/tenebris-lux --gui
```

### In-Game Commands & Hotkeys

* `look` / **(L)**: Inspect current location, faction tensions, and surroundings.
* `status` / **(I)**: Inspect the Shield of Lux, Sword of Tenebris, Stance, and Moral Sway.
* `ward` / **(W)**: Raise the Shield of Lux to establish a radiant barrier or reveal hidden truths.
* `strike` / **(S)**: Unleash the Sword of Tenebris to sever enemy defenses or cloak in shadow.
* `balance` / **(B)**: Seek harmony between the twin relics to restore equilibrium.
* `stance <ward|strike|equilibrium>` / **(T)**: Shift tactical and metaphysical stance.
* `move <n|s|e|w>` / **(N, S, R, W)**: Travel across the fractured Kingdom provinces.
* `help`: View all available actions.
* `quit` / **(Q)**: Exit the game.

### Automated Diagnostics & Testing

```bash
# Run pre-flight inspection and internal self-audit (7 registered modules + 5 invariant checks)
./build/tenebris-lux --audit

# Run standalone Layer 0 core utilities and preflight test suite
./build/test_utils

# Run full CTest verification suite (6/6 passing in 0.02s)
ctest --test-dir build -C Debug --output-on-failure
```

---

## 5. Repository Documentation Index

* **[docs/README.adoc](file:///home/professorseanex/Tenebris%20and%20Lux/docs/README.adoc)**: Central documentation portal and index.
* **[docs/INSTALL.adoc](file:///home/professorseanex/Tenebris%20and%20Lux/docs/INSTALL.adoc)**: Comprehensive compilation, installation, and troubleshooting guide.
* **[docs/architecture/](file:///home/professorseanex/Tenebris%20and%20Lux/docs/architecture/README.adoc)**: System design, 5-block lifecycle, TRPG architecture, and dual-frontend engine.
* **[docs/lore/](file:///home/professorseanex/Tenebris%20and%20Lux/docs/lore/README.adoc)**: Canonical chronicles of Sacyra, the Shield and Sword, and [characters.adoc](file:///home/professorseanex/Tenebris%20and%20Lux/docs/lore/characters.adoc) (Elijah Ro-enheart & Katrina Jaren).
* **[docs/game_design/](file:///home/professorseanex/Tenebris%20and%20Lux/docs/game_design/README.adoc)**: [tactical_trpg_system.adoc](file:///home/professorseanex/Tenebris%20and%20Lux/docs/game_design/tactical_trpg_system.adoc) (Grid + Cinematic Side-View), relics, topography, and testing philosophy.
* **[docs/dev_logs/](file:///home/professorseanex/Tenebris%20and%20Lux/docs/dev_logs/README.adoc)**: Living engineering chronology ([DevLog 001](file:///home/professorseanex/Tenebris%20and%20Lux/docs/dev_logs/devlog_001_genesis_and_pivot.adoc) and [DevLog 002](file:///home/professorseanex/Tenebris%20and%20Lux/docs/dev_logs/devlog_002_the_sacyra_chronicles_and_trpg_vision.adoc)).
* **[docs/code/](file:///home/professorseanex/Tenebris%20and%20Lux/docs/code/README.adoc)**: Subsystem implementation tours for utilities, simulation, GUI, and test suites.
* **[AGENTS.md](file:///home/professorseanex/Tenebris%20and%20Lux/AGENTS.md)**: Contributor and AI agent coding guidelines, style rules, and commit discipline.
