# Repository Guidelines & Agent Standards

> **Project**: Tenebris and Lux  
> **Standard**: OmniCode 5-Block Ontological Architecture & Layered Invariant Discipline  
> **Author**: CreativeWorkzStudio LLC / ProfessorSeanEX  
> **Repository**: [https://github.com/Novaking34/tenebris-lux](https://github.com/Novaking34/tenebris-lux)

---

## 0. Philosophy: The Purpose of this Document

This document (`AGENTS.md`) is **not** an inflexible bureaucratic straitjacket or a rigid legal statute meant to override developer judgment or stifle pragmatic engineering.

Rather, it is a **cognitive anchor and compass**. In complex AI-assisted pair-programming workflows involving multi-layered C11 engines, Rust developer tooling, and deep narrative world-building, autonomous agents can easily experience context drift, introduce synthetic hallucinations (e.g. inventing placeholder names), violate architectural constraints (e.g. introducing heap allocations to zero-heap layers), or pollute repository hygiene.

`AGENTS.md` exists to **re-ground you in proven patterns and axiomatic invariants**:

- **Clarity of Architectural Intent**: Understanding *why* code is layered and *why* contracts exist prevents well-meaning changes from breaking subtle guarantees.
- **Cognitive Consistency**: Grounding names, themes, and narrative logic in authentic project canon ensures all new systems harmonize like an integrated guidebook.
- **Engineering Hygiene**: Maintaining root directory cleanliness, explicit pathing, and zero-warning compilation keeps the codebase robust and maintainable.

When in doubt, use this document to re-center your decisions around the project's true North Star.

---

## 1. Architectural Philosophy: The 5-Block Lifecycle Protocol

Every source file, header, and configuration file in this repository strictly adheres to the **Universal 5-Block Lifecycle Protocol** adapted from the OmniCode framework. This structure eliminates ambiguity, guarantees self-documenting code, and enforces compile-time and runtime invariants.

```text
┌────────────────────────────────────────────────────────────────────────┐
│                        BLOCK 1: METADATA BLOCK                         │
│  Identity, Provenance, Invariant Boundary Contracts & System Anchors   │
├────────────────────────────────────────────────────────────────────────┤
│                    BLOCK 2: OPENING / TYPES BLOCK                      │
│  Includes, Preconditions, Enums, State Structures & Global Constants   │
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

### Block Breakdown

1. **BLOCK 1: METADATA BLOCK**
   - Must contain file identity tags (`@project`, `@file`, `@layer`, `@version`, `@author`, `@system`, `@role`, `@invariants`).
   - Outlines architectural intent and axiomatic invariants governing the module.

2. **BLOCK 2: OPENING / TYPES BLOCK**
   - Explicit includes of layered headers.
   - Type definitions, enums, structs, and compile-time constants (`#define`).
   - Preconditions and memory layout definitions.

3. **BLOCK 3: BODY BLOCK**
   - Pure, modular, single-responsibility functions and algorithms.
   - Core gameplay mechanics, mathematical clamping, and state transitions.

4. **BLOCK 4: AUDIT PROCEDURES BLOCK**
   - Dedicated diagnostic routines (e.g. `audit_*` functions).
   - Validates subsystem state invariant integrity (e.g., resonance within `[0, MAX_RESONANCE]`, viewport non-overlap, monotonic clock ticks).
   - Reports detailed pre-flight audit manifests without relying on crude runtime crashes.

5. **BLOCK 5: CLOSING / TEST & CLI BLOCK**
   - For executables (`src/main.c`): Command-line argument parsing (`--audit`, `--help`, `--version`, `--cli`, `--gui`), teardown, and return codes.
   - For modular units: Standalone unit tests and verification suites.
   - For headers (`.h`): Include guard closing `#endif`.

---

## 2. Layered Architecture & Directory Strata

The codebase is organized into strict, hierarchical strata. Higher layers may depend on lower layers, but lower layers NEVER depend on higher layers.

```text
Layer 4: Verification Suite       [ tests/ ]
           │
Layer 3: Application Entry        [ src/main.c ]
           │
Layer 2: Presentation & Viewport  [ src/gui/   <-> include/gui/ ]
           │
Layer 1: Simulation Engine        [ src/sim/   <-> include/sim/ ]
           │
Layer 0: Core Utilities           [ src/util/  <-> include/util/ ]
```

### Layer Definitions

1. **Layer 0: Core Utilities (`src/util/`, `include/util/`)**:
   - Zero-dependency foundational primitives organized into domain subsystems:
     - `math/`: Bounded arithmetic (`math_utils.h`: clamping, saturating, normalization) and 2D spatial geometry (`geom_utils.h`: AABB, hit-tests).
     - `data/`: Zero-heap filesystem I/O (`fs_utils.h`) and JSON token/value deserialization (`json_utils.h`).
     - `log/`: Forensic telemetry, categorized channels, grade tracking, static ring buffer, and quarantined fallback monoliths (`log.h`, `log_fallback.h`).
     - `debug/`: Interactive state debugger, frozen snapshot ring buffer, FNV-1a state hashing, time-travel rewind, dynamic breakpoints, and quarantined fallback monoliths (`debug.h`, `debug_fallback.h`).
     - Leaf Primitives: Safe string copying and trimming (`string_utils.h`), monotonic timing and ISO timestamps (`time_utils.h`), and central pre-flight manifest registry (`manifest.h`).
   - *Invariant*: Zero heap allocations, pure reentrant C11, no GUI or game dependencies.

2. **Layer 1: Simulation Engine (`src/sim/`, `include/sim/`)**:
   - Implements game state (`player_state_t`), twin armaments, stance mechanics, moral sway continuum, provincial topology, and command dispatching.
   - Fully decoupled from windowing or rendering. Runs headless in CLI, in native GUI viewports, or under simulation test harnesses.
   - *Headers*: `include/sim/*.h` | *Sources*: `src/sim/*.c`

3. **Layer 2: Presentation & Viewports (`src/gui/`, `include/gui/`)**:
   - Native X11 double-buffered desktop interface. Translates mouse clicks and keystrokes into universal simulation commands.
   - Renders health/resonance meters, region chronicles, stance visualizers, and interactive controls.
   - *Header*: `include/gui/gui.h` | *Source*: `src/gui/gui.c`

4. **Layer 3: Application Entry & Presentation Router (`src/main.c`)**:
   - Entry point parsing CLI flags (`--help`, `--version`, `--audit`, `--debug`, `--cli`, `--gui`).
   - Implements automatic headless fallback: launches GUI when X11 display is available, gracefully falls back to terminal CLI otherwise.

5. **Layer 4: Verification & Test Suite (`tests/`)**:
   - Automated game development verification suite integrated into CTest (13/13 tests pass):
     - `tests/functional/`:
       - `test_functional`: Granular function-level debugging harness isolating 20 individual cogs across utilities, simulation, and debugger with `[DEBUG-TRACE]` telemetry for zero-friction debugging.
     - `tests/unit/`:
       - `test_math`: Subsystem unit verification of mathematical clamping, saturating, normalization, and 2D spatial geometry.
       - `test_data`: Subsystem unit verification of zero-heap filesystem I/O and recursive JSON parsing.
       - `test_time`: Subsystem unit verification of monotonic timers, ISO timestamps, and 60/120 FPS frame profiles.
       - `test_string`: Subsystem unit verification of safe bounded string copying, whitespace trimming, and path operations.
       - `test_log`: Subsystem unit verification of categorized telemetry channels, ring buffer cycling, and fallback monoliths across the 4 Ontological Testing Sections.
       - `test_debug`: Subsystem unit verification of state snapshot ring buffer, time-travel rewind, state diffing, dynamic breakpoints, and emergent chaotic stress across the 4 Ontological Testing Sections.
     - `tests/integrated/`:
       - `test_utils`: Cross-utility coordination, central pre-flight manifest DAG verification, multi-subsystem boot, and configuration loading.
     - `tests/system/`:
       - `test_game_invariants`: High-level simulation engine state limits, twin relic resonance conservation, stance deterministic transitions, provincial topology, and audit tripwires.
       - `test_gameplay_scenarios`: Whole-game scenarios, 15-step golden replay determinism, softlock immunity under 1,000 chaotic commands, and strict < 50µs latency budget.

6. **Developer Tooling Strata (`tools/`)**:
   - Standalone Rust toolchain supporting extractor utilities and documentation compilation:
     - `tools/src/extractor.rs`: Historical RPG Maker MV data extractor.
     - `tools/src/guide.rs`: Standalone Master Guide compiler assembling 65 chapters across 6 volumes into AsciiDoc, Markdown, and PDF books.

---

## 3. Strict Include Pathing & Root Hygiene

1. **Explicit Pathing (No Forwarders)**:
   - Header includes must explicitly declare their stratum and subsystem:
     - `#include "sim/game.h"`
     - `#include "gui/gui.h"`
     - `#include "util/math/math_utils.h"`
     - `#include "util/math/geom_utils.h"`
     - `#include "util/data/fs_utils.h"`
     - `#include "util/data/json_utils.h"`
     - `#include "util/log/log.h"`
     - `#include "util/debug/debug.h"`
     - `#include "util/string/string_utils.h"`
     - `#include "util/time/time_utils.h"`
   - Never generate lazy forwarder headers in `include/` root.
2. **Root Directory Hygiene**:
   - The repository root must ONLY contain essential project anchors:
     - `CMakeLists.txt`
     - `.gitignore`
     - `README.md`
     - `AGENTS.md`
   - All source code lives in `src/<layer>/`.
   - All public headers live in `include/<layer>/`.
   - All tests live in `tests/<stratum>/`.
   - All documentation lives in `docs/<category>/`.
   - All raw working notes, transcribe logs, and offline drafts live in `docs/offline_work/`.

---

## 4. Lore, Canon & Thematic Invariants

1. **Genesis & Provenance**:
   - Conceived between **2014–2018** in RPG Maker as the very first major game of **CreativeWorkzStudio** prior to its publishing house evolution.
   - Reborn as a custom-engineered **Tactical Strategy RPG (TRPG)** featuring a grid-based battlefield with cinematic traditional side-scroll / action combat resolution.

2. **Twin Relics Canon**:
   - **The Shield of Lux**: Personification of Light, barrier resonance, kinetic deflection, and protection.
   - **The Sword of Tenebris**: Personification of Darkness, edge sharpness, abyssal mist, and severance.

3. **Metaphysical Neutrality**:
   - Neither Light (Lux) nor Darkness (Tenebris) is inherently good or evil. They are primal cosmic apex forces.
   - The wielder's intent and application determine the moral sway (`moral_sway` in `[-100, +100]`):
     - Positive sway: Virtue, protection, mercy.
     - Negative sway: Malice, ruin, ruthless dominion.

4. **The Thematic Triad: "Trauma is Inherited, Destiny is Chosen"**:
   - **The Past (Healing the Frontier Wound)**: **Usuru** (the tragic Libiran wanderer) and **Elijah Ro-enheart** (son of the vanished Guard Commander Gareth).
   - **The Present (Severing Complacency)**: **Prince Casatti** (the outrider prince who walked the wilderness) and **Lady Katrina Jaren** (noble court aspirant renouncing aristocracy for the Adventurers' Guild).
   - **The Future (The Steward of Balance)**: **Prince Mi-ren** (the founding architect who held the four-year vigil) and **The Custom Bearer** (`<chosen_name>`, unburdened by ancestral grudge, awakening in the Dream Chamber).

5. **Macro-Historical Timeline & The 3,000-Year Dynastic Wave**:
   - **Era I**: Founding of Sacyra, the Two Princes (Mi-ren & Casatti), and the Four-Year Coma.
   - **Era II–III**: The International Festival of Peoples, the Four-Year Vigil, and the 5th Festival passing.
   - **Era IV**: Usuru the Wanderer, Libira's "Quiet Complaint" (7-day courier gap, no outpost system), and the 11th Year Festival Rupture.
   - **Era V**: The Seven-Year Great War between Sacyra and the 6-Power Coalition, culminating in the *Accord of Kings* ("Joint Realm, Separate Kingdoms").
   - **Era VI**: The 3,000-Year Dynastic Wave (30–50 Kings, alternating stasis crests and tether troughs), ending in the possession of King Jihad via **The King's Curse** (the open planar wound beneath the High Throne).

6. **Canonical Antagonist Truth**:
   - The overarching existential antagonist is solely **"The Dark Lord"** (the ancient Libiran sovereign Usuru).
   - Deprecated placeholder names (such as "Malakor") are non-canonical and must not be used.

---

## 5. Ludology, Campaign Structure & Multi-Endings

1. **State Machine Completeness**: Every stance (`STANCE_WARDING`, `STANCE_STRIKING`, `STANCE_EQUILIBRIUM`, `STANCE_NEUTRAL`) forms a strongly connected, deterministic transition graph.
2. **Conserved Resources & Bounded Invariants**: Shield power and sword edge strictly respect `[0, MAX_RESONANCE]`. Moral sway remains bounded in `[-100, +100]`.
3. **The King's Curse as Midpoint Campaign Fork**:
   - Entering the Royal Capital without the calibrated *Dream Compass* triggers the tragic *Neutral Path*.
   - Exposing the planar wound using the calibrated *Dream Compass* (evolved from Grandma's Watch) unlocks the *True Ending Paths*.
4. **The 5-Route Ending Matrix**:
   - *Path of Radiance (Lux Overload)*: Total moral sway $\ge +75$; light crystallization.
   - *Path of the Abyssal Sovereign (Tenebrous Eclipse)*: Total moral sway $\le -75$; dominion over shadow.
   - *The Sovereign Accord (Dual Harmonic Balance)*: Equilibrium maintained `[-25, +25]`; renewal of the Accord of Kings.
   - *The Cycle Shattered (Planar Dissolution)*: Transcending both relics; healing the 3,000-year breach.
   - *The Forgotten Stasis (The King's Slumber)*: Neutral default; repetition of the dynastic cycle.

---

## 6. Documentation Standards (AsciiDoc)

- All technical manuals, architecture designs, dev logs, realm chronicles, and lore documents are standardized in **AsciiDoc (`.adoc`)** format.
- Located within `docs/`:
  - `docs/architecture/`: System design, layered strata, dual-frontend architecture.
  - `docs/lore/`: World canon, timeline, character dossiers, realm chronicles.
  - `docs/game_design/`: Combat mechanics, stances, topographical map, test criteria.
  - `docs/dev_logs/`: Chronological engineering logs and milestone postmortems.
  - `docs/code/`: Module walkthroughs and subsystem code documentation.
  - `docs/offline_work/`: Archived genesis notes and raw working papers.
- **AsciiDoc Best Practices**:
  - Keep lines under 100 characters where practical.
  - Always escape tildes (`\~` or write "approximately") to prevent unintentional unclosed subscript formatting warnings.
  - Use relative cross-references (`xref:`, `link:`) rather than absolute `file:///` URLs within documentation files.
  - Utilize attributed quotes (`[quote, Author, Context]`), multi-column tables, sidebars (`****`), and semantic admonitions (`[NOTE]`, `[IMPORTANT]`, `[WARNING]`).

---

## 7. Build, Test, and Quality Commands

CMake 3.24+ is configured for C11 with zero-warning tolerance; Rust 2021 toolchain manages compilation:

- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`: Configure build directory.
- `cmake --build build --config Debug`: Compile executable `./build/tenebris-lux` and test suites.
- `ctest --test-dir build -C Debug --output-on-failure`: Run full automated test suite (13/13 tests pass).
- `./build/tenebris-lux --audit`: Run internal diagnostic self-audit suite (15-module manifest + 7 invariant tests).
- `./build/tenebris-lux --version`: Display version information.
- `./build/tenebris-lux --help`: Display command-line options.
- `./build/tenebris-lux --debug`: Launch game with interactive CLI state debugger enabled.
- `./build/tenebris-lux --cli`: Force text-based CLI terminal mode.
- `./build/tenebris-lux --gui`: Launch native X11 double-buffered desktop GUI.
- `cargo run --manifest-path tools/Cargo.toml --bin master_guide`: Compile the 67-chapter unified Master Guide (`MASTER_GUIDE.adoc`, `MASTER_GUIDE.md`, and `MASTER_GUIDE.pdf`).

---

## 8. Structured Git Commit & Release Tagging Guidelines

Do **not** use terse or vague one-line commit messages. Every commit must serve as an enduring record of engineering rationale, architectural modifications, and verification telemetry:

```text
<type>(<scope>): <summary title>

### Context & Architectural Intent
Detailed exposition of why this work was undertaken, the subsystem affected,
and how this fulfills the invariant contracts of the project.

### Structural & Code Modifications
- BLOCK 1 (Metadata): Updated provenance, identity, and invariant assertions.
- BLOCK 2 (Opening/Types): Defined state structures, enums, and constants.
- BLOCK 3 (Body Logic): Implemented core simulation and execution routines.
- BLOCK 4 (Audit Procedures): Added diagnostic tripwires and self-tests.
- BLOCK 5 (Closing/CLI): Integrated command-line flags and teardown.

### Verification Telemetry
- ctest results and pass rates.
- Diagnostic audit status (e.g. ./build/tenebris-lux --audit output).
- Compiler diagnostic check (-Wall -Wextra -Wpedantic clean).
```
