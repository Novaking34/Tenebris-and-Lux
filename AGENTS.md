<!--
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          AGENTS.md
@layer         Cognitive Anchor & Repository Governance Strata
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Universal 5-Block Lifecycle Protocol & Agent Standards
@role          Cognitive Compass, Architectural Invariants & Ontological Guide
@invariants    1. Zero dynamic heap allocations in Layer 0 (Core Util) & Layer 1 (Sim).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Explicit include pathing (No forwarder headers in include root).
               4. Dual-frontend determinism (Headless CLI and X11 share pure sim state).
               5. Authentic project canon (Sole antagonist: The Dark Lord Usuru).
               6. Universal 5-Block Lifecycle Protocol enforced across all files.
               7. Machine vs. Master Turn Gear Separation (engine/ is universal, game/ is domain).
               8. Cog Elevation & Orchestrator Protocol (Elevate reusable cogs out of game into engine).
               9. Dual Development Discipline (Waterfall bedrock physical laws; Agile iterative loop).
               10. Active Iteration Feedback Covenant (Expose seams, purge shims, compiler-guided upgrade).

PURPOSE & COGNITIVE ROLE:
  The definitive architectural manual and cognitive anchor for autonomous AI
  agents and human engineers working on Tenebris and Lux. Translates the
  OmniCode Universal 5-Block Lifecycle Protocol into an actionable mental model:
  Block 1: Metadata -> Block 2: Types/Ontology -> Block 3: Body/Execution ->
  Block 4: Audit/Tripwires -> Block 5: Closing/CLI/Teardown.
================================================================================
-->

# Repository Guidelines & Agent Standards

> **Project**: Tenebris and Lux  
> **Standard**: OmniCode 5-Block Ontological Architecture & Layered Invariant Discipline  
> **Author**: CreativeWorkzStudio LLC / ProfessorSeanEX  
> **Repository**: [https://github.com/Novaking34/tenebris-lux](https://github.com/Novaking34/tenebris-lux)  
> **Engine Milestone**: `0.1.0-dev` (Tactical Strategy RPG Grid Engine & Continental Topology)  
> **Governance Revision**: `Rev 4` (The Machine & Turn Gear Architecture, Active Iteration Protocol)

---

## 0. Philosophy: The Cognitive Anchor & Compass

This document (`AGENTS.md`) is **not** an inflexible bureaucratic straitjacket or a rigid legal statute meant to override developer judgment or stifle pragmatic engineering.

Rather, it is a **cognitive anchor and compass**. In complex AI-assisted pair-programming workflows involving multi-layered C11 engines, Rust developer tooling, and deep narrative worldbuilding, autonomous agents frequently suffer from **context drift**:

* Inventing synthetic hallucinations (e.g., deprecated placeholder names like "Malakor" instead of the canonical **Dark Lord Usuru**);
* Violating architectural constraints (e.g., introducing heap allocations like `malloc` into zero-heap Layer 0 or Layer 1);
* Polluting repository hygiene (e.g., placing source files or forwarder headers at the repository root);
* Breaking determinism (e.g., coupling rendering viewports into pure simulation logic);
* Clinging to obsolete compatibility shims during active architectural refactoring, obscuring the seams where data and code must cleanly align.

`AGENTS.md` exists to **re-ground you in proven patterns and axiomatic invariants**:

* **Clarity of Architectural Intent**: Understanding *why* code is layered and *why* contracts exist prevents well-meaning changes from breaking subtle guarantees.
* **Cognitive Consistency**: Grounding names, themes, and narrative logic in authentic project canon ensures all systems harmonize like an integrated guidebook.
* **Engineering Hygiene**: Maintaining root directory cleanliness, explicit pathing, zero warnings, and 100% CTest pass rates keeps the codebase pristine.

---

### 0.1 The Dual Development Discipline: Waterfall Invariants & Agile Iteration

Game engine development is uniquely demanding because it demands **both Waterfall and Agile engineering disciplines simultaneously**:

```text
┌────────────────────────────────────────────────────────────────────────┐
│                   WATERFALL BEDROCK (Cast-Iron Strata)                 │
│  • Memory Contracts (ALLOC_ZERO_HEAP in Layer 0/1)                     │
│  • ISO C11 Adherence (-std=c11, -Wall -Wextra -Wpedantic)              │
│  • Layer Invariants (Dependencies flow strictly downward)               │
│  • Universal 5-Block File Protocol (Metadata -> Types -> Body -> ...)   │
│  • Data Schemas & Contracts (tenlux.data.* namespace, 5-block envelope) │
└────────────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
┌────────────────────────────────────────────────────────────────────────┐
│                   AGILE LIVING ITERATION (Turning the Gears)           │
│  • Active Iteration Feedback Loops (Subsystem-by-subsystem sprints)    │
│  • Empirical Test-Driven Refinement (4 Ontological Testing Sections)   │
│  • Cog Discovery & Elevation (Extracting universal cogs from game/)    │
│  • Data-Driven Alignment (JSON manifests driving C struct hydration)   │
│  • Ruthless Shim Purging (Expose seams to let the compiler guide us)   │
└────────────────────────────────────────────────────────────────────────┘
```

1. **Waterfall Bedrock (The Cast-Iron Frame)**:
   The structural invariants are fixed and immovable. Memory limits, layer boundaries, language standards, schema envelopes, and deterministic execution order cannot be negotiated or compromised during a sprint. You cannot "agilely" introduce dynamic memory leaks into Layer 0, allow circular dependency spaghetti across headers, or invent loose JSON schemas that violate data contracts. These are the physical laws of our machine.

2. **Agile Living Iteration (Turning the Gears)**:
   Within that cast-iron frame, development proceeds iteratively, empirically, and responsively. We build, test, observe runtime behavior, measure performance, and refine. Testing reveals emerging patterns, exposing where subsystems need richer data ingestion, where shared helpers should be promoted into general engine orchestrators, and where old code must be refactored. We do not design speculative monoliths; we modernize concrete subsystem strata one by one (`io/` -> `log/` -> `debug/` -> `core/` -> `game/`), using the empirical feedback of tests to guide the next sprint.

---

### 0.2 The Big Machine & The Master Turn Gear (Engine vs. Game Philosophy)

To build both a robust standalone game engine and a compelling tactical RPG, we maintain a clear mental model:

> **The Engine is the Big Machine.**  
> **The Game is the Master Turn Gear.**

```text
  ┌──────────────────────────────────────────────────────────────┐
  │                 THE BIG MACHINE ( engine/ )                  │
  │                                                              │
  │   [Clock & Pacer]  [Virtual File System]  [Flight Log Ring]  │
  │   [Math Evaluator] [Screen Transitions]   [Audio Event Bus]  │
  │   [State Debugger] [Double-Buffered GUI]  [Letterbox Scaler] │
  └──────────────────────────────┬───────────────────────────────┘
                                 │ drives / meshes with
                                 ▼
  ┌──────────────────────────────────────────────────────────────┐
  │               THE MASTER TURN GEAR ( game/ )                 │
  │                                                              │
  │   [Tactical Turn Machine]   [Combat Stances & Relic Poise]   │
  │   [Hero Simulation State]   [Moral Sway Continuum (-100,+100)]│
  │   [Provincial Topology]     [Five-Route Ending Resolution]   │
  │   [Domain Data Manifests]   [Sacyra / Libira Campaign Lore]  │
  └──────────────────────────────────────────────────────────────┘
```

* **The Big Machine (`engine/`)**:
  A universal, reusable, game-agnostic mechanical apparatus. It contains:
  * Foundational math, vector geometry, monotonic time, and safe string utilities (`core/`).
  * Universal orchestrators for screen transitions, audio bus event dispatching, and formula evaluation.
  * Zero-heap filesystem I/O, path resolution, and master JSON parsing (`io/`).
  * Categorized telemetry, flight-recorder rotation, and ring logging (`log/`).
  * State snapshots, FNV-1a deterministic hashing, and diagnostic tripwires (`debug/`).
  * Double-buffered X11 rendering, TrueType font blitting, and letterbox/pillarbox viewport scaling (`viewport/`).

* **The Master Turn Gear (`game/`)**:
  The specific game instance (*Tenebris and Lux*). It brings its own domain gears, ratchets, and cogs:
  * The tactical grid turn loop (`combat/turn_based.c`, `combat/tactical.c`).
  * The hero simulation state and inventory (`hero/sim_state.c`, `relics/inventory.c`).
  * The twin armaments (Shield of Lux, Sword of Tenebris), stance state machine, and moral sway continuum.
  * Provincial world geography (Sacyra, Libira, Great Mau), campaign routing, and the 5-route ending matrix.
  * Game-specific data manifests in `game/data/` (`mechanics/`, `world/`, `entities/`, `presentation/`).

When the Master Turn Gear is slotted into the Big Machine, the engine provides the power train (monotonic tick pacing, display surfaces, audio buses, file loading) while the game drives the simulation rules, tactical decisions, and narrative resolution.

---

### 0.3 2D/2.5D Classic Perspective Foundation & Future Dimensional Horizons

The engine's initial spatial architecture is engineered specifically for a **2D/2.5D classic Game Boy / Nintendo-esque tactical strategy RPG**:

* **Perspective & Vanishing Points**: Fixed orthographic or dimetric/isometric grid vanishing points, representing tactical battlefields with discrete tile coordinates, fixed elevation steps, and billboarded sprite entities.
* **Aspect Ratio & Resolution**: A native 640x360 logical rendering frame (16:9 widescreen or classic integer-scaled 4:3 sub-windows) scaled with zero visual distortion via letterbox/pillarbox presentation.
* **Separation of Sim Grid and Display Projection**: Simulation coordinates (`tile_x`, `tile_y`, `elevation_z`) are pure integer grid units in Layer 1. Screen coordinates (`screen_x`, `screen_y`) are projected in Layer 2 by mathematical scaler cogs.

**The Future Horizon**:  
Because the Big Machine is strictly decoupled from domain lore and simulation logic, once the 2D/2.5D grid engine is verified and rock-solid:
1. The engine can be upgraded to true 3D polygonal rendering, perspective camera matrices, and dynamic depth buffers simply by augmenting the viewport stratum (`engine/src/viewport/`).
2. The engine can power entirely different games and genres (e.g., action RPGs, space sims, visual novels) by slotting in a completely different Master Turn Gear while retaining the same robust core utilities, I/O pipelines, logging rings, and diagnostic harnesses.

---

### 0.4 The Cog Elevation & Orchestrator Protocol

As gameplay systems are authored inside `game/`, developers frequently write local helper logic (e.g., screen fading routines, sound playback hooks, damage formulas, grid distance math).

Autonomous agents and engineers must actively practice **Cog Elevation**:

> **Cog Elevation Invariant**: When a function or subsystem in `game/` performs a task that is universal, game-agnostic, and reusable across any project, it must **not** remain buried in game code. It must be elevated into `engine/` as an **Orchestrator Cog**.

#### Elevation Evaluation Rubric:
| Trait | Belongs in `game/` (Domain Gear) | Elevate to `engine/` (Universal Machine) |
|---|---|---|
| **Domain Lore** | Mentions Lux, Tenebris, Sacyra, Usuru, Relics | Zero mentions of game lore; operates on generic IDs |
| **Data Format** | Specific game schemas (`tenlux.data.hero.*`) | Universal engine schemas or generic callbacks |
| **Mechanism** | Turn-based combat stances, moral sway | Screen wipes, audio cue routing, easing curves, clocks |
| **Reusability** | Unique to *Tenebris and Lux* | Usable in any 2D/2.5D/3D engine application |

#### The Orchestrator Pattern:
Universal engine services are structured as **Orchestrators**:
* **Screen Transition Orchestrator**: Manages screen fading, wipes, iris dissolves, and easing curves (`EASE_LINEAR`, `EASE_QUAD_IN`, `EASE_CUBIC_OUT`) between game scenes.
* **Audio Event Bus Orchestrator**: Manages zero-heap audio cue queues, volume channels (`MASTER`, `BGM`, `SFX`, `AMBIENCE`), and sound trigger routing without binding directly to a low-level audio driver.
* **Clock & Pacing Orchestrator**: Manages monotonic frame deltas, fixed-timestep simulation tick accumulators, and render interpolation alphas.
* **Formula & Affinity Evaluator**: Parses and computes bounded mathematical formulas from JSON data definitions (`formulas.json`) rather than hardcoding game arithmetic.

---

### 0.5 The Active Iteration Feedback Covenant (Exposing Seams vs. Preserving Shims)

A frequent failure mode in automated software engineering is **defensive shim preservation**: when refactoring a subsystem, the engineer attempts to keep legacy wrapper functions, deprecated structs, and obsolete signatures around "just in case" or "for backward compatibility."

During active architectural modernization sprints, this practice is **prohibited**:

> [!CAUTION]
> **Defensive shims mask architectural seams.** Keeping deprecated wrappers creates a false impression of stability while hiding broken contracts, stale signatures, unaligned data, and half-updated call sites. It prevents you from seeing where old code genuinely needs to be upgraded.

#### The Active Iteration Rules of Engagement:
1. **Purge Obsolete Shims Immediately**: When upgrading an engine subsystem to Rev 3/Rev 4 standards, remove deprecated functions and obsolete structs. Do not leave compatibility wrappers in the main headers or source files.
2. **Expose Seams Directly to the Compiler**: Let the compiler's strict diagnostic flags (`-std=c11 -Wall -Wextra -Wpedantic -Werror`) break immediately at every caller that uses the old interface.
3. **Follow the Diagnostic Trace**: Use compile errors and CTest failures as a precise, empirical map of all call sites needing modernization.
4. **Upgrade Callers Completely**: Update every calling site to the new tagged types, proper error handling, 3-tier includes, and pre-flight audits. Never patch a caller with a local hack.
5. **Quarantined Fallback Isolation**: Fallback implementations belong **exclusively** in isolated `*_fallback.[ch]` modules. They serve as zero-dependency compilation safety nets and standalone test harnesses—never runtime crutches intermingled with production engine cogs.

---

## BLOCK 1: METADATA & COGNITIVE ANCHOR BLOCK

> **Cognitive Role in the Mind: Orientation & Invariant Grounding**  
> Before writing a single line of executable logic, the engineer or agent must orient: *Who am I? What module is this? What contracts bind this space? What are the non-negotiable constraints?*

### 1.1 The File Metadata Header Covenant

Every C source file (`.c`), header (`.h`), script, and documentation file must begin with an explicit **Block 1 Metadata Header**:

```c
/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          <filename>.<ext>
@layer         Layer <N>: <Stratum Name>
@version       0.1.0-dev
@interface_rev Rev <N>
@date          YYYY-MM-DD
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        <Subsystem Identity>
@role          <Single Responsibility Role>
@invariants    <Axiomatic Invariant List>

PURPOSE & ARCHITECTURAL ROLE:
  <Detailed exposition of architectural mission and design rationale>
================================================================================
*/
```

### 1.2 The Root Directory Hygiene Invariant

The repository root must remain immaculate. The root directory may ONLY contain essential project anchors:

* `CMakeLists.txt` (Root C11 build orchestrator)
* `.gitignore` (VCS exclusion rules with Block 1 metadata)
* `README.md` (Root documentation anchor with expanded YAML frontmatter)
* `CHANGELOG.md` (Evolutionary chronicle across DevLogs 001–015)
* `LICENSE` (CreativeWorkzStudio Commercial Game & Source-Available Agreement)
* `AGENTS.md` (This cognitive anchor and governance standard)

All other materials belong in their respective strata:

* `engine/`: Bespoke C11 engine architecture (The Big Machine):
  * `engine/include/`: Public engine headers (`core/`, `io/`, `log/`, `debug/`, `viewport/`).
  * `engine/src/`: Engine implementation modules (`core/`, `io/`, `log/`, `debug/`, `viewport/`).
* `game/`: Tenebris and Lux TRPG game implementation (The Master Turn Gear):
  * `game/include/`: Domain headers (`combat/`, `hero/`, `relics/`, `world/`, `ending/`, `dispatch/`).
  * `game/src/`: Game implementation modules and router entry point (`game/src/main.c`).
  * `game/data/`: Data-driven JSON manifests, subsystem profiles, and tactical schemas (`game/data/SCHEMAS.md`).
  * `game/assets/`: Visual assets, fonts (`fonts/`), high-res icons (`icons/`), banners (`branding/`), and screenshots (`screenshots/`).
* `tests/<stratum>/`: All automated test suites (`functional/`, `unit/`, `integrated/`, `system/`).
* `docs/<category>/`: All technical manuals, 67-chapter Master Guide, and realm chronicles in AsciiDoc format.
* `tools/`: Developer toolchain in Rust (`master_guide`, `extractor`).
* `.github/`: Platform governance, issue templates, linter rules, and GitHub Actions CI pipelines (`workflows/ci.yml`).
* `.vscode/`: Canonical team IDE workspace definitions (`settings.json`, `tasks.json`, `launch.json`, `extensions.json`).

### 1.3 The Ephemeral Output & Transient Sandboxing Invariant

Transient outputs generated by compilation, tests, language servers, or runtime simulation are strictly quarantined into `build/` and excluded by `.gitignore`:

* `build/`: Dedicated CMake out-of-source compilation workspace.
* `build/logs/`: All runtime telemetry, flight recorder rotations (`tenebris_lux_dev.log.*`), and diagnostic traces.
* `build/.cache/`: Language server AST caches and index data (`clangd`).
* **Root Hygiene Invariant**: Transient runtime logs (`logs/`) and compiler index caches (`.cache/`) must **never** linger at the repository root. All ephemeral state is quarantined inside `build/` so a single `rm -rf build` leaves the source tree pristine.
* **Untracked Sensitive Files**: Specific untracked local documents (e.g., `Offline_Work_4.md`) must **never** be edited, staged, modified, or deleted by any automated tool or commit.

---

## BLOCK 2: OPENING / TYPES & DOMAIN ONTOLOGY BLOCK

> **Cognitive Role in the Mind: Framing & State Representation**  
> In the mind, Block 2 defines the grammar and boundaries of the problem space: *What data structures exist? What are the memory layouts? What headers are imported? What is the ontological vocabulary?*

### 2.1 The 5 Layered Strata & Hierarchy

Dependencies strictly flow downwards. Higher layers may call lower layers; lower layers **never** import or depend on higher layers.

```text
Layer 4: Verification Suite       [ tests/ ]
           │
Layer 3: Application Router       [ game/src/main.c ]
           │
Layer 2: Presentation Viewports   [ engine/src/viewport/ <-> engine/include/viewport/ ]
           │
Layer 1: Simulation Engine        [ game/src/{combat,hero,relics,world,ending,dispatch}/ ]
           │
Layer 0: Core Utilities           [ engine/src/{core,io,log,debug}/ ]
```

1. **Layer 0: Core Utilities (`engine/src/{core,io,log,debug}/`, `engine/include/{core,io,log,debug}/`)**:
   * Part of **The Big Machine**. Zero-dependency foundational primitives:
     * `core/`: Bounded math, 2D AABB geometry, safety strings, monotonic time, pre-flight DAG registry, formula evaluation, transition curves, and audio event dispatch.
     * `io/`: Zero-heap VFS, path resolution (`path_finder`), JSON tokenizer (`json_utils`), and master manifest loader (`master_loader`).
     * `log/`: Categorized flight telemetry, log rotations, ring buffers (`log`).
     * `debug/`: Invariant tripwires, snapshot ring buffers, deterministic state hashing (FNV-1a), ASCII cards, and state debugger (`debug`).
   * **Hard Invariant**: **Zero dynamic heap allocations** (`malloc`, `calloc`, `free` are prohibited). Pure reentrant ISO C11.
2. **Layer 1: Simulation Engine (`game/src/{combat,hero,relics,world,ending,dispatch}/`, `game/include/`)**:
   * Part of **The Master Turn Gear**. Manages hero state (`hero/sim_state.h`), combat stances (`combat/`), twin armaments and inventory (`relics/`), provincial topology (`world/`), command routing (`dispatch/`), and campaign resolution (`ending/`).
   * **Hard Invariant**: Zero heap allocations. Completely decoupled from X11 or windowing runtimes. Runs headless under any CLI or test harness.
3. **Layer 2: Presentation & Viewports (`engine/src/viewport/`, `engine/include/viewport/`)**:
   * Part of **The Big Machine**. Native X11 double-buffered desktop interface. Translates mouse clicks and keystrokes into universal simulation commands.
   * High-DPI TrueType font engine (`Cinzel`, `MedievalSharp`), 640x360 target resolution with letterbox/pillarbox scalers.
4. **Layer 3: Application Entry & Presentation Router (`game/src/main.c`)**:
   * Meshes the Master Turn Gear into the Big Machine. Evaluates CLI flags (`--help`, `--version`, `--audit`, `--debug`, `--cli`, `--gui`).
   * Automatically detects active X11 display, launching GUI when available and gracefully falling back to terminal CLI.
5. **Layer 4: Verification & Test Suite (`tests/`)**:
   * Automated CTest verification suites spanning Functional cogs, Subsystem units, Integrated DAG, and System golden replays.

### 2.2 Strict Include Pathing (No Forwarders)

All header includes must explicitly declare their stratum and subsystem. Lazy forwarder headers in include roots are strictly forbidden:

```c
#include "core/math_utils.h"
#include "core/geom_utils.h"
#include "core/time_utils.h"
#include "core/string_utils.h"
#include "core/manifest.h"
#include "io/fs_utils.h"
#include "io/json_utils.h"
#include "io/path_finder.h"
#include "io/master_loader.h"
#include "log/log.h"
#include "debug/debug.h"
#include "viewport/gui.h"
#include "combat/combat.h"
#include "hero/sim_state.h"
#include "relics/relics.h"
#include "world/world.h"
#include "dispatch/dispatcher.h"
#include "ending/game.h"
```

### 2.3 Continental Lore & Canon Invariants

* **The Twin Relics**:
  * **The Shield of Lux**: Light, barrier resonance, poise, kinetic deflection, protection.
  * **The Sword of Tenebris**: Darkness, edge sharpness, armor severance, abyssal mist.
* **Metaphysical Neutrality**: Neither Light nor Darkness is inherently good or evil. They are primal cosmic apex forces. Morality exists solely in wielder intent (`moral_sway` in `[-100, +100]`).
* **The Thematic Triad: "Trauma is Inherited, Destiny is Chosen"**:
  * *The Past (Frontier Neglect)*: **Elijah Ro-enheart** and ancient wanderer **Usuru**.
  * *The Present (Severing Complacency)*: **Lady Katrina Jaren** and founding **Prince Casatti**.
  * *The Future (The Steward of Balance)*: **The Custom Bearer** (`<chosen_name>`) and founding architect **Prince Mi-ren**.
* **The Geopolitical Six Powers**:
  * Royal Capital of Sacyra, Kingdom of Libira, Great Mau Kingdom, Old Sacyra, The Academy, and the Yun-yin Provinces—bound by the *Accord of Kings* ("Joint Realm, Separate Kingdoms").
* **Existential Antagonist Truth**: The sole overarching antagonist is **"The Dark Lord"** (the ancient Libiran sovereign Usuru). Deprecated placeholder names (e.g., "Malakor") are non-canonical and prohibited.

---

## BLOCK 3: BODY / EXECUTION LOGIC & GAMEPLAY LOOPS

> **Cognitive Role in the Mind: Algorithmic Execution & State Transitions**  
> In the mind, Block 3 implements the core engine routines: *How does state transition? What are the mathematical bounds? How do combat and mechanics resolve?*

### 3.1 Deterministic Stance State Machine

Every stance transition forms a strongly connected, deterministic finite state machine:

* `STANCE_NEUTRAL`: Baseline posture, balanced stamina recovery.
* `STANCE_WARDING` (*Lux Barrio*): Raises radiant dome, absorbs 25% damage, kinetic reflect.
* `STANCE_STRIKING` (*Cuttatio Tenebris*): Lowers center of gravity, sweeping arc + 20% flank splash.
* `STANCE_EQUILIBRIUM` (*Duo-Loca Bashin n Slash*): Harmonic shield bash + abyssal double-cleave targeting raw DEF.

### 3.2 Bounded Arithmetic & Conserved Resources

* Shield warding charge: Strictly clamped in `[0, 100]` (`MAX_RESONANCE`).
* Sword edge sharpness: Strictly clamped in `[0, 100]` (`MAX_RESONANCE`).
* Moral Sway continuum: Strictly bounded in `[-100, +100]`.
* Zero-divisor guards: All mathematical operations must use `safe_div()` or saturating math from `math_utils.h`.

### 3.3 Midpoint Campaign Fork & The 5-Route Ending Matrix

* **The King's Curse**: The open planar wound beneath the High Throne and possession of King Jihad.
* **The Dream Compass**: Evolved from Grandma's Watch through nocturnal calibration.
  * Entering the Royal Capital without the calibrated Compass triggers the tragic *Neutral Path (The Forgotten Stasis)*.
  * Exposing the wound with the calibrated Compass unlocks the *True Ending Paths*:
    1. *Path of Radiance (Lux Overload)*: Moral sway $\ge +75$; light crystallization.
    2. *Path of the Abyssal Sovereign (Tenebrous Eclipse)*: Moral sway $\le -75$; absolute shadow.
    3. *The Sovereign Accord (Dual Harmonic Balance)*: Equilibrium maintained `[-25, +25]`.
    4. *The Cycle Shattered (Planar Dissolution)*: Transcending both relics; healing the 3,000-year breach.
    5. *The Forgotten Stasis (The King's Slumber)*: Default neutral closure; dynastic repetition.

### 3.4 Universal Orchestrators & Engine Tick Synchronization

The Big Machine ticks forward deterministically, driving universal orchestrator cogs that coordinate presentation and execution without contaminating Layer 1 simulation logic:

* **Screen Transition Orchestrator**:
  * Finite state transitions: `TRANSITION_STATE_IDLE` -> `TRANSITION_STATE_OUT` -> `TRANSITION_STATE_HOLD` -> `TRANSITION_STATE_IN` -> `TRANSITION_STATE_IDLE`.
  * Easing evaluation: `ease_linear()`, `ease_quad_in()`, `ease_quad_out()`, `ease_cubic_inout()` providing smooth normalized progress `[0.0, 1.0]`.
  * Transition types: Fade to Black, Cross-Fade, Horizontal/Vertical Wipe, and Iris Dissolve.
* **Audio Event Bus Orchestrator**:
  * Static circular event queue of sound cues: `audio_bus_post_event(channel, cue_id, volume, pan)`.
  * Dedicated volume channels: `AUDIO_CHANNEL_MASTER`, `AUDIO_CHANNEL_BGM`, `AUDIO_CHANNEL_SFX`, `AUDIO_CHANNEL_UI`, `AUDIO_CHANNEL_AMBIENCE`.
  * Zero-heap decoupled architecture: The simulation posts events by symbolic ID; the audio orchestrator buffers them for consumption by whatever sound backend is linked.
* **Clock & Pacing Orchestrator**:
  * Monotonic microsecond clock tracking frame time deltas (`dt`).
  * Fixed-timestep physics/turn accumulator with spiral-of-death clamp.
  * Alpha accumulator ratio passed to viewports for jitter-free sub-tick interpolation.
* **Formula & Affinity Evaluator**:
  * Ingests mathematical curves from `game/data/system/runtime/math/` (`formulas.json`, `attributes.json`, `affinities.json`).
  * Pure reentrant function: `formula_eval_damage(attacker, defender, stance, affinity)`.

---

## BLOCK 4: AUDIT PROCEDURES & DIAGNOSTIC TRIPWIRES

> **Cognitive Role in the Mind: Self-Reflection, Telemetry & Invariant Defense**  
> In the mind, Block 4 acts as the self-reflective immune system: *Did any invariant breach? Is memory leaking? Is the state machine healthy? How do we catch drift before a crash?*

### 4.1 Diagnostic Pre-Flight Audits (`audit_*` / `*_preflight`)

Every subsystem must expose a dedicated diagnostic routine registered in `manifest.h`:

* Evaluates memory limits, boundary clamping, and state transitions.
* Reports telemetry manifests without relying on crude runtime crashes (`abort()`).
* Categorized logging with forensic grades (`GRADE:+003`, `GRADE:+100`).
* Dynamic validation of subsystem profiles, catalogs, and schema compliance.

### 4.2 Interactive State Debugger & Rewind

* **Snapshot Ring Buffer**: Captures frozen simulation states (up to 64 snapshots).
* **FNV-1a Deterministic Hashing**: Evaluates hash of `player_state_t` to instantly detect non-deterministic drift.
* **Time-Travel Rewind (`rewind <idx>`)**: Reverts simulation state to a historical snapshot.
* **Diagnostic ASCII Cards (`inspect`)**: Live visual readout of hero vitals, relics, and tripwires formatted to 60-column terminal-friendly cards.

### 4.3 Automated Verification Strata (36/36 Passing)

Automated verification is structured into 4 strata integrated into CTest:

1. `tests/functional/`: Isolates individual cogs across utilities, simulation, and debugger with forensic `[DEBUG-TRACE]` telemetry.
2. `tests/unit/`: Subsystem unit verification across `math`, `data`, `time`, `string`, `log`, `debug`, `combat`, `items`, `world`, `dispatch`, `gui`.
3. `tests/integrated/`: Cross-utility DAG verification (`test_utils`), multi-subsystem simulation boot (`test_sim`), and data-loader integration (`test_debug_integrated`).
4. `tests/system/`: Engine state limits (`test_game_invariants`), 15-step golden replay determinism, and 1,000-turn chaotic fuzzing softlock immunity (`test_gameplay_scenarios`).

### 4.4 The 4 Ontological Testing Sections

Every test file in `tests/` must strictly implement the **4 Ontological Testing Sections**:

1. **SECTION 1: Passing / Nominal Cases**: Standard expected operations, nominal inputs, happy paths, and baseline state transitions.
2. **SECTION 2: Failing / Boundary & Error Cases**: Null pointers, empty strings, buffer overflows, out-of-range bounds, corrupted data, and graceful degradation.
3. **SECTION 3: Invariant Assertions & State Proofs**: Mathematical bounds checking, state machine closed loops, zero dynamic heap proofs, and deterministic hash invariance.
4. **SECTION 4: Emergence & Stress / Fuzzing Cases**: Multi-turn sequences, stress-testing static limits, high-frequency call saturation, and randomized input fuzzing.

---

## BLOCK 5: CLOSING / CLI & VERIFICATION PROTOCOLS

> **Cognitive Role in the Mind: Execution Handshake, Toolchains & Teardown**  
> In the mind, Block 5 is the delivery and exit contract: *How is this module invoked? How is it verified? How does it return cleanly to the operating system or caller?*

### 5.1 Build, Test, and Quality Commands

The project compiles under CMake 3.24+ for ISO C11 with zero warnings (`-Wall -Wextra -Wpedantic`):

```bash
# Configure and compile
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug

# Execute full CTest verification suite (36/36 test suites must pass)
ctest --test-dir build -C Debug --output-on-failure

# Execute diagnostic self-audit (18 registered modules + 7 invariant tests)
./build/tenebris-lux --audit

# Validate all game data manifests and schemas (46/46 data files verified)
cargo test --manifest-path tools/Cargo.toml

# Launch options
./build/tenebris-lux           # Auto-detects GUI, falls back to CLI
./build/tenebris-lux --cli     # Explicit terminal CLI mode
./build/tenebris-lux --gui     # Explicit native X11 desktop GUI mode
./build/tenebris-lux --debug   # Launch with interactive CLI state debugger enabled
./build/tenebris-lux --version # Display engine version
./build/tenebris-lux --help    # Display command-line manual

# Compile 67-chapter unified Master Guide
cargo run --manifest-path tools/Cargo.toml --bin master_guide
```

### 5.2 Structured Git Commit Guidelines

Do **not** use terse or vague commit messages. Every commit must serve as an enduring record of engineering rationale, structural modifications, and verification telemetry:

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
- CTest results and pass rates (e.g. 36/36 suites passing).
- Diagnostic audit status (e.g. ./build/tenebris-lux --audit: 18 modules pass, 7 invariant tests pass).
- Data validation status (e.g. cargo test: 46/46 data files pass).
- Compiler diagnostic check (-Wall -Wextra -Wpedantic clean).
```

### 5.3 AsciiDoc Documentation Standards

All technical manuals, design logs, and realm chronicles in `docs/` are standardized in **AsciiDoc (`.adoc`)**:

* Keep lines under 100 characters where practical.
* Escape tildes (`\~` or write "approximately") to prevent unclosed subscript warnings.
* Use relative cross-references (`xref:`, `link:`) rather than absolute URLs within documentation files.
* Utilize semantic admonitions (`[NOTE]`, `[IMPORTANT]`, `[WARNING]`) and attributed quotes.
