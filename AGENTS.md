# Repository Guidelines & Agent Standards

> **Project**: Tenebris and Lux  
> **Standard**: OmniCode 5-Block Ontological Architecture & Layered Invariant Discipline  
> **Author**: CreativeWorkzStudio LLC / ProfessorSeanEX  
> **Repository**: [https://github.com/Novaking34/tenebris-lux](https://github.com/Novaking34/tenebris-lux)

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
   - Reports detailed audit manifests without relying on crude runtime crashes.

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
   - Zero-dependency foundational primitives:
     - `math_utils`: Clamping (`math_clamp_int`), saturating arithmetic (`math_saturating_add`, `math_saturating_sub`), equilibrium nudging (`math_nudge_toward`), ratio scaling (`math_scale_ratio`).
     - `string_utils`: Safe copying (`str_safe_copy`), in-place trimming (`str_trim_inplace`), case-insensitive matching (`str_case_eq`, `str_contains_nocase`).
     - `geom_utils`: 2D bounding boxes (`geom_rect_t`), point-in-rect hit testing (`geom_point_in_rect`), AABB intersection (`geom_rect_intersects`), and viewport bounds validation (`geom_rect_within_bounds`).
     - `time_utils`: High-resolution monotonic timing (`time_get_monotonic_ms`, `time_get_monotonic_us`) and millisecond sleep (`time_sleep_ms`).
   - *Invariant*: Zero heap allocations, pure reentrant C11, no GUI or game dependencies.

2. **Layer 1: Simulation Engine (`src/sim/`, `include/sim/`)**:
   - Implements game state (`player_state_t`), twin armaments, stance mechanics, moral sway continuum, and location topology.
   - Fully decoupled from windowing or rendering. Can run headless, in CLI, or under simulation test harnesses.
   - *Header*: `include/sim/game.h`
   - *Source*: `src/sim/game.c`

3. **Layer 2: Presentation & Viewports (`src/gui/`, `include/gui/`)**:
   - Native X11 double-buffered desktop interface. Translates mouse clicks and keystrokes into universal simulation commands.
   - Renders health/resonance meters, region chronicle, stance visualizers, and interactive controls.
   - *Header*: `include/gui/gui.h`
   - *Source*: `src/gui/gui.c`

4. **Layer 3: Application Entry & Presentation Router (`src/main.c`)**:
   - Entry point parsing CLI flags (`--help`, `--version`, `--audit`, `--cli`, `--gui`).
   - Implements automatic headless fallback: launches GUI when X11 display is available, gracefully falls back to terminal CLI otherwise.

5. **Layer 4: Verification & Test Suite (`tests/`)**:
   - Automated game development verification suite integrated into CTest:
     - `test_utils`: Verifies Layer 0 mathematical, string, geometric, and timing invariants.
     - `test_game_invariants`: Verifies Layer 1 state limits, stance deterministic transitions, and audit tripwires.
     - `test_gameplay_scenarios`: Verifies deterministic replayability, softlock immunity under 1,000 chaotic commands, and 60 FPS latency budget (< 50 microseconds per simulation turn).

---

## 3. Strict Include Pathing & Root Hygiene

1. **Explicit Pathing (No Forwarders)**:
   - Header includes must explicitly declare their stratum:
     - `#include "sim/game.h"`
     - `#include "gui/gui.h"`
     - `#include "util/math_utils.h"`
     - `#include "util/geom_utils.h"`
   - Never generate lazy forwarder headers in `include/` root.
2. **Root Directory Hygiene**:
   - The repository root must ONLY contain essential project anchors:
     - `CMakeLists.txt`
     - `.gitignore`
     - `README.md`
     - `AGENTS.md`
   - All source code lives in `src/<layer>/`.
   - All public headers live in `include/<layer>/`.
   - All tests live in `tests/`.
   - All documentation lives in `docs/<category>/`.

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
4. **Setting: The Kingdom of Sacyra**:
   - The story takes place in the chivalric **Kingdom of Sacyra** (pronounced *"saw-kee-ruh"*, `/sɔːˈkiːrə/`).
   - Governed by the Crown and the **Royal Guard**, balanced against competing regional **Noble Houses** on the Royal Court, and the independent **Adventurers' Guild**.
   - The overarching existential threat is the re-awakening **Dark Lord Malakor**, whose void vanguard advances from the Abyssal Threshold.
5. **Dual Protagonists**:
   - **Elijah Ro-enheart**: Descendant of a proud royal army family; son of the former Head of the Royal Guard. Frontline heavy vanguard bearer of the Shield of Lux.
   - **Katrina Jaren**: Daughter of noble House Jaren; groomed for an aristocratic seat on the Royal Court, but passionately aspires to join the local Adventurers' Guild. Agile outrider wielder of the Sword of Tenebris.

---

## 5. Game Development Testing Philosophy

Game development testing is fundamentally distinct from standard software CRUD testing:

1. **State Machine Completeness**: Every stance (`STANCE_WARDING`, `STANCE_STRIKING`, `STANCE_EQUILIBRIUM`, `STANCE_NEUTRAL`) must form a strongly connected, deterministic transition graph.
2. **Conserved Resources & Bounded Invariants**: Shield power and sword edge must strictly respect `[0, MAX_RESONANCE]`. Moral sway must remain in `[-100, +100]`.
3. **Softlock Immunity & Chaotic Stress**: Subjecting the engine to thousands of arbitrary, invalid, or chaotic inputs must never crash, deadlock, or induce undefined state.
4. **Frame-Budget Latency Headroom**: Simulation ticks must execute in under 50 microseconds to guarantee ample frame-time budget for 60 FPS presentation.

---

## 6. Documentation Standards (AsciiDoc)

- All technical manuals, architecture designs, dev logs, and lore documents are standardized in **AsciiDoc (`.adoc`)** format.
- Located within `docs/`:
  - `docs/architecture/`: System design, layered strata, dual-frontend architecture.
  - `docs/lore/`: World chronicle, Kingdom geography, relic genesis.
  - `docs/game_design/`: Combat mechanics, stances, topographical map, test criteria.
  - `docs/dev_logs/`: Chronological engineering logs and milestone postmortems.
  - `docs/code/`: Module walkthroughs and subsystem code documentation.

---

## 7. Build, Test, and Quality Commands

CMake 3.24+ is configured for C11 with zero-warning tolerance:

- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`: Configure build directory.
- `cmake --build build --config Debug`: Compile executable `./build/tenebris-lux` and test suites.
- `ctest --test-dir build -C Debug --output-on-failure`: Run full automated test suite (6/6 tests).
- `./build/tenebris-lux --audit`: Run internal diagnostic self-audit suite (5/5 checks).
- `./build/tenebris-lux --version`: Display version information.
- `./build/tenebris-lux --help`: Display command-line options.
- `./build/tenebris-lux --cli`: Force text-based CLI terminal mode.
- `./build/tenebris-lux --gui`: Launch native X11 double-buffered desktop GUI.

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
