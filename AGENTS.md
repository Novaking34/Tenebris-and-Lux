<!--
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          AGENTS.md
@layer         Cognitive Anchor & Repository Governance Strata
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
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
               11. Standalone Engine Boundary Covenant (engine/ defines generic slots; zero domain bleed).
               12. Procedural Fallback Floor Covenant (100% playable with zero external art/audio assets).
               13. Dual Data Taxonomy Covenant (engine/config/ mechanical tuning vs game/data/ RPG lore).
               14. Native Compiled Toolchain Invariant (Pure compiled Rust/C11; zero dynamic interpreters).
               15. Universal Sub-Assembly Architecture (2.1 Constants -> 2.2 Hooks -> 2.3 Types -> 2.4 Inlines).
               16. Quarantined Fallback Isolation (Dedicated emergency fallback cogs with loaded_from_data=false).
               17. Dual Audience Documentation Covenant (Every type and file features Designer & Programmer sections).
               18. Strict ISO C-Style Comment Invariant (Strictly /* ... */ and /** ... */; zero // line comments).

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
> **Engine Milestone**: `0.1.1-dev` (Standalone Engine Decoupling, Layered Config Architecture & Continental Topology)  
> **Governance Revision**: `Rev 5` (The Big Machine Decoupling, Layered Config VFS, Audio Orchestrator & Procedural Floor)

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

### 0.6 The Standalone Engine Boundary Covenant (Anti-Domain-Bleed)

The Big Machine (`engine/`) is an independent, reusable software apparatus. It must **never** contain hardcoded game character names (e.g. Elijah, Katrina), specific story relic identifiers (e.g. Shield of Lux, Sword of Tenebris), narrative strings, or hardcoded relative paths to game-specific assets:

* **Engine Provides Generic Cogs & Slots**: The engine exposes generic asset slots (`ASSET_SLOT_0`..`ASSET_SLOT_MAX` or dynamic ID registries), parameterized HUD gauges (primary resource, secondary resource, stance meter), and generic diagnostic descriptors.
* **Game Binds Domain Lore**: The Master Turn Gear (`game/`) supplies the specific names, narrative strings, and manifest bindings via `game/data/` manifests and game initialization routines.
* **Decoupled Viewport Compilation**: `engine/` headers must **never** include `game/` headers (`hero/`, `combat/`, `world/`, `relics/`, `ending/`, `dispatch/`). The engine must compile to a self-contained static or shared library (`libtenebris_engine.a`) without requiring any files from `game/`.

---

### 0.7 The Procedural Fallback Floor Covenant

The engine must boot, initialize its native double-buffered X11 viewport, process audio bus events, and remain **100% playable and auditable even if ZERO external art or audio files exist on disk**:

* **Progressive Enhancement**: External bitmap PNGs and audio WAV files are strictly progressive enhancements layered on top of the procedural floor.
* **Seamless Fallback**: In the absence of external asset files, the engine operates on `ASSET_KIND_PROCEDURAL` (geometric vector-rasterized character silhouettes, tile grids, and synthesized waveform fallbacks) with zero fatal halts or memory breaches.
* **Audit Invariance**: Running asset audits must remain completely idempotent and side-effect free, preserving the active procedural/bitmap state.

---

### 0.8 Dual Data Taxonomy Covenant (Engine Configs vs. Game Domain Data)

The repository strictly separates mechanical engine configuration profiles from high fantasy RPG narrative content:

* **Engine Configuration Profiles (`engine/config/`)**:
  * `viewport/`: Target display resolutions (640x360 retro to 4K UHD), window presentation modes, letterbox scalers, typography catalogs, master UI themes, and generic asset slots.
    * `manifest.json`: Viewport sub-domain orchestrator.
    * `settings.json`: Master display & viewport settings profile (active preset, window mode, target FPS, theme, UI scale).
    * `scalers.json`, `layout.json`, `themes.json`, `typography.json`, `assets.json`: Mechanical definitions.
  * `audio/`: Multi-channel mixing bus architecture (7 canonical buses: Master, BGM, Ambience, Combat SFX, Tactical SFX, UI, Stinger), hardware limiter protection, and procedural synthesis.
    * `manifest.json`: Audio sub-domain orchestrator.
    * `settings.json`: Master audio settings profile (volume sliders 0-100, mute toggles, sample rate, driver preference, ducking).
    * `channels.json`: Hardware mixing bus parameters and polyphony caps.
  * `input/`: Decoupled action mappings, directional grid navigation, tactical hotkeys, and controller profiles.
    * `manifest.json`: Input sub-domain orchestrator.
    * `default.json`: Default keyboard and mouse action bindings (Arrow/WASD, Confirm, Cancel, Stances, Fullscreen, Mute).
    * `gamepad.json`: Standard dual-analog gamepad mappings, D-pad navigation, shoulder bumpers, and deadzones.
  * `runtime/`: Mathematical epsilons, clock pacer policies (30/60/120fps), string buffer limits, and the hotpatch incubator.
    * `math/`: Continuum bounds, moral sway [-100, +100], resonance [0, 100], Tri-Vitals, formulas, exploration stats, affinities.
    * `time/`: Monotonic pacing profiles (default 60fps, high_refresh 120fps, low_spec 30fps), pacing policy, and calendar.
    * `string/`: Safe bounded string utilities, token lexicon, and dialogue formatting.
    * `hotpatch.json`: Emerging runtime prototype incubator.
  * `diagnostics/`: Telemetry severity weights, logging ring capacities, tripwire matrices, snapshot rewind, and serialization formats.
    * `telemetry/`: Telemetry profiles (default, development, production, quiet, combat_focus, forensic), severity weights, engine/game channels.
    * `debugger/`: Snapshot policies (up to 64 states), deterministic FNV-1a hashing, inspection cards, interactive trace.
    * `taxonomy/`: Error taxonomy codes (Engine 0x0xxx vs Game 0x1xxx) and tripwire matrix.
    * `formats/`: Fixed-column plaintext, NDJSON streaming, CSV export, replay event streams, and audit reports.
  * `manifest.json`: Root Engine Configuration Manifest orchestrating all 5 sub-domains.
  * `settings.json`: Master Settings Registry providing a single-point dispatch for user preferences and options UI.
* **Game Domain Manifests (`game/data/`)**:
  * `entities/`: Character classes, protagonists (Elijah, Katrina), companions, enemy rosters.
  * `mechanics/`: Combat stances, turn algorithms, item catalogs, campaign routes.
  * `world/`: Battlefields, geography, provinces (Sacyra, Libira), terrain definitions.
  * `presentation/`: Game-specific sprite mappings, sound cue manifests, music playlists.
* **Layered VFS Resolution Precedence**:
  The `path_finder` virtual filesystem implements a strict 3-tier resolution sequence for the `"config"` domain:
  1. `game/config/<rel_path>` (Tier 1: user or game-specific configuration overrides).
  2. `engine/config/<rel_path>` (Tier 2: standalone engine baseline defaults).
  3. `game/data/presentation/config/<rel_path>` (Tier 3: presentation symbolic data linkers).
* **Presentation Data Linker Architecture (`game/data/presentation/config/`)**:
  To eliminate data duplication between standalone engine viewport/audio profiles (`engine/config/viewport/`, `engine/config/audio/`, `engine/config/input/`) and game presentation catalogs, `game/data/presentation/config/` establishes 5-block symbolic Data Linkers:
  * `layout.json` -> `engine/config/viewport/layout.json`
  * `themes.json` -> `engine/config/viewport/themes.json`
  * `scalers.json` -> `engine/config/viewport/scalers.json`
  * `typography.json` -> `engine/config/viewport/typography.json`
  * `channels.json` -> `engine/config/audio/channels.json`
  * `settings.json` -> `engine/config/viewport/settings.json`
  * `audio_settings.json` -> `engine/config/audio/settings.json`
  * `input.json` -> `engine/config/input/default.json`
  The engine loads mechanical configurations directly from `engine/config/` (domain `"config"`), while game presentation links to them cleanly without duplication.

---

### 0.8.1 The Universal 5-Block JSON Data File Protocol

All `.json` manifests, configuration profiles, catalogs, and data linkers across `engine/config/` and `game/data/` must strictly implement the **OmniCode Universal 5-Block Lifecycle Protocol**:

1. **Block 1: Metadata Block (`block_1_metadata`)**:
   * `project`: `"Tenebris and Lux"`
   * `file`: Relative path from repository root (e.g., `"engine/config/viewport/settings.json"`).
   * `id`: Canonical dot-separated identifier within namespace (e.g., `"engine.config.viewport.settings"`).
   * `layer`: Architectural stratum (e.g., `"Layer 0: Core Utilities / Viewport Presentation"`).
   * `version`: Engine milestone (e.g., `"0.1.1-dev"`).
   * `interface_rev`: Governance revision (e.g., `"Rev 5"`).
   * `date`: ISO date (`"YYYY-MM-DD"`).
   * `author`: `"CreativeWorkzStudio LLC / ProfessorSeanEX"`.
   * `repository`: `"https://github.com/Novaking34/tenebris-lux"`.
   * `role`: Single-responsibility description.
   * `where_the_data_lives`: Topology and file paths.
   * `what_it_enables`: Array of concrete capabilities enabled by this file.
   * `invariants`: Array of axiomatic boundary contracts.

2. **Block 2: Opening Block (`block_2_opening`)**:
   * `pre_conditions`: Required initialized subsystems, memory layout, and timing guarantees.
   * `static_declarations`: `schema_id` (e.g., `"tenlux.engine.viewport.settings.v1"`), target struct, header bindings.
   * `generic_helper_contracts`: Associated C11 loader and query functions (e.g., `"gui_display_load_settings"`).
   * `capacities`: Static fixed buffer allocations and counts.

3. **Block 3: Body Block (`block_3_body`)**:
   * The pure configuration data payload.
   * In settings profiles: structured into active properties (e.g., `display`, `volumes`, `behavior`) paired with an `options_catalog` defining valid choices, ranges, steps, and references to mechanical files for UI consumption.
   * In manifests: contains `sub_domains`, `subsystems`, `profiles`, or `catalogs` lists.

4. **Block 4: Audit Block (`block_4_audit`)**:
   * `audit_metrics` or `metrics`: Verification counts and flags.
   * `verification_tripwires`: Array of `{ "name", "rule", "status" }` invariants.
   * `self_audit_target`: Test command (e.g., `"./build/tenebris-lux --audit"`).

5. **Block 5: Closing Block (`block_5_closing`)**:
   * `execution_handshake`: CLI invocation flags (`--audit`, `--debug`).
   * `navigation`: Menu traversal anchors (`parent_menu`, `home_dir`, `root_manifest`).

---

### 0.9 Native Compiled Toolchain & Zero-Interpreter Invariant

Developer tooling in `tools/` must be authored in compiled languages (Rust / C11) leveraging standard library primitives:

* **Zero Dynamic Interpreters**: Ad-hoc dynamic runtime interpreters (e.g., Python, Perl, Bash scripting) are strictly prohibited in the engine toolchain pipeline.
* **Native Compiled Binaries**: All asset generation, inspection, manifest validation, and documentation compilation are executed via native compiled binaries:
  * `tl_tools`: Unified developer CLI orchestrator (`assets`, `validate`, `doc`, `clean`).
  * `asset_generator`: Standalone pure Rust visual and audio procedural synthesizer (`--audit`, `--generate`).
  * `master_guide`: AsciiDoc 67-chapter technical manual compiler.
  * `extractor`: Code and schema metadata extraction utility.
* **Hygiene**: All Rust tools must build completely warning-free under `-D warnings` with `cargo check` and `cargo test`.

---

## BLOCK 1: METADATA & COGNITIVE ANCHOR BLOCK

> **Cognitive Role in the Mind: Orientation & Invariant Grounding**  
> Before writing a single line of executable logic, the engineer or agent must orient: *Who am I? What module is this? What contracts bind this space? What are the non-negotiable constraints?*

### 1.1 The File Metadata Header Covenant

Every C source file (`.c`), header (`.h`), data manifest (`.json`), build configuration, test harness, script, and technical documentation file must begin with an explicit **Block 1 Metadata Header**:

```c
/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          <filename>.<ext>
@id            <stratum>.<subsystem>.<component>
@layer         Layer <N>: <Stratum Name>
@version       0.1.1-dev
@interface_rev Rev <N>
@date          YYYY-MM-DD
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        <Subsystem Identity & Name>
@role          <Single Responsibility Role Summary>
@invariants    1. <Invariant 1 (e.g., Zero dynamic heap allocations in Layer N)>
               2. <Invariant 2 (e.g., Strict ISO C11 adherence)>
               3. <Invariant 3 (e.g., Quarantined execution path / bounds)>
               ...
               N. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  <Detailed multi-paragraph exposition of architectural mission, rationale, and scope>

  ### For Game Designers & Non-Programmers:
  <Plain-English explanation of what this file does, how it impacts gameplay,
  visuals, storytelling, balance, or user experience. Avoid low-level jargon
  and ground the explanation in narrative or mechanical terms.>

  ### For Systems Programmers & Engine Architects:
  - 100% Static Compile-Time Constants: Zero dynamic heap allocation.
  - Strict Memory Contracts: Stack allocation, alignment, bounded structures.
  - Reentrancy & Concurrency: Pure functions, zero global mutable state.
  - Pre-flight Tripwires: Categorized diagnostic audit hooks.
================================================================================
*/
```

#### Metadata Field Specification:
| Field Tag | Required Content | Purpose & Cognitive Anchor |
|---|---|---|
| `@project` | `"Tenebris and Lux"` | System anchor binding all code to canonical project root. |
| `@file` | Basename (e.g., `gui_fallback.h`) | Source file identity matching physical filesystem path. |
| `@id` | Dot-notated ID (e.g., `viewport.raster.fallback`) | Global DAG identifier for pre-flight registry and telemetry. |
| `@layer` | Layer 0 to Layer 4 (with stratum name) | Explicit architectural layer defining allowable downward dependencies. |
| `@version` | Semantic version (e.g., `0.1.1-dev`) | Codebase evolution milestone. |
| `@interface_rev` | Contract counter (e.g., `Rev 5`) | ABI/API breaking revision tracker. |
| `@date` | ISO 8601 Date (`YYYY-MM-DD`) | Last updated datestamp. |
| `@author` | Studio & Lead Architect attribution | Provenance and copyright stewardship. |
| `@repository` | GitHub canonical URL | Repository provenance (the sole permitted `//` sequence in C files). |
| `@system` | Mechanical subsystem name | Domain context (e.g., "Universal Audio Event Bus & Mixer Architecture"). |
| `@role` | Concise single responsibility summary | Architectural purpose preventing scope creep. |
| `@invariants` | Numbered list of non-negotiables | Bedrock laws that must never be violated in active sprints. |

#### The Dual-Audience Documentation Covenant:
Every Block 1 header must communicate clearly to **two distinct audiences**:
1. **For Game Designers & Non-Programmers**:
   Game systems are collaborative. Writers, level designers, combat tuners, and producers must understand what a module does without deciphering C pointers or bitwise math. This section explains mechanics, visual flow, UI presentation, and player experience in plain, evocative language.
2. **For Systems Programmers & Engine Architects**:
   Systems engineers need immediate clarity on physical computing constraints: memory footprint, heap bans (`ALLOC_ZERO_HEAP`), cache line locality, algorithmic complexity $O(n)$, thread safety, reentrancy, and pre-flight tripwire integration.

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
  * `engine/include/`: Public engine headers (`core/`, `io/`, `log/`, `debug/`, `audio/`, `viewport/`).
  * `engine/src/`: Engine implementation modules (`core/`, `io/`, `log/`, `debug/`, `audio/`, `viewport/`).
  * `engine/config/`: Standalone engine mechanical configuration profiles (`runtime/`, `diagnostics/`, `viewport/`, `audio/`).
* `game/`: Tenebris and Lux TRPG game implementation (The Master Turn Gear):
  * `game/include/`: Domain headers (`combat/`, `hero/`, `relics/`, `world/`, `ending/`, `dispatch/`).
  * `game/src/`: Game implementation modules and router entry point (`game/src/main.c`).
  * `game/data/`: Data-driven JSON manifests, subsystem profiles, and tactical schemas (`game/data/SCHEMAS.md`).
  * `game/assets/`: Visual assets, fonts (`fonts/`), high-res icons (`icons/`), banners (`branding/`), and screenshots (`screenshots/`).
* `tests/<stratum>/`: All automated test suites (`functional/`, `unit/`, `integrated/`, `system/`).
* `docs/<category>/`: All technical manuals, 67-chapter Master Guide, and realm chronicles in AsciiDoc format.
* `tools/`: Developer toolchain in Rust (`tl_tools`, `asset_generator`, `master_guide`, `extractor`).
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

Dependencies strictly flow downwards. Higher layers may call lower layers; lower layers **never** import or depend on higher layers. Furthermore, the Big Machine (`engine/`) must compile independently without importing headers from the Master Turn Gear (`game/`).

```text
Layer 4: Verification Suite           [ tests/ ]
           │
Layer 3: Application Router           [ game/src/main.c ]
           │
Layer 2: Presentation Viewports       [ engine/src/viewport/ <-> engine/include/viewport/ ]
           │
Layer 1: Simulation Engine            [ game/src/{combat,hero,relics,world,ending,dispatch}/ ]
           │
Layer 0: Core Utilities & Audio Bus   [ engine/src/{core,io,log,debug,audio}/ ]
```

1. **Layer 0: Core Utilities & Audio Orchestrator (`engine/src/{core,io,log,debug,audio}/`, `engine/include/{core,io,log,debug,audio}/`)**:
   * Part of **The Big Machine**. Zero-dependency foundational primitives:
     * `core/`: Bounded math, 2D AABB geometry, safety strings, monotonic time, pre-flight DAG registry, formula evaluation, transition curves, and clock pacing.
     * `io/`: Zero-heap VFS, path resolution (`path_finder`), JSON tokenizer (`json_utils`), and master manifest loader (`master_loader`).
     * `log/`: Categorized flight telemetry, log rotations, ring buffers (`log`).
     * `debug/`: Invariant tripwires, snapshot ring buffers, deterministic state hashing (FNV-1a), ASCII cards, and state debugger (`debug`).
     * `audio/`: Multi-channel bus (Master, BGM, Ambience, Combat SFX, Tactical SFX, UI, Stinger), ducking state machine, and discrete event queue (`audio`).
   * **Hard Invariant**: **Zero dynamic heap allocations** (`malloc`, `calloc`, `free` are prohibited). Pure reentrant ISO C11.
2. **Layer 1: Simulation Engine (`game/src/{combat,hero,relics,world,ending,dispatch}/`, `game/include/`)**:
   * Part of **The Master Turn Gear**. Manages hero state (`hero/sim_state.h`), combat stances (`combat/`), twin armaments and inventory (`relics/`), provincial topology (`world/`), command routing (`dispatch/`), and campaign resolution (`ending/`).
   * **Hard Invariant**: Zero heap allocations. Completely decoupled from X11 or windowing runtimes. Runs headless under any CLI or test harness.
3. **Layer 2: Presentation & Viewports (`engine/src/viewport/`, `engine/include/viewport/`)**:
   * Part of **The Big Machine**. Native X11 double-buffered desktop interface. Translates mouse clicks and keystrokes into universal simulation commands.
   * High-DPI TrueType font engine (`Cinzel`, `MedievalSharp`), 640x360 target resolution with letterbox/pillarbox scalers.
   * Procedural rasterizers (`gui_assets`) ensuring the procedural fallback floor.
   * **Domain Decoupling Invariant**: `viewport/` headers must **never** `#include` `game/` headers (`hero/`, `ending/`). Viewports render generic data descriptors or consume state via Layer 3 application adapters.
4. **Layer 3: Application Entry & Presentation Router (`game/src/main.c`)**:
   * Meshes the Master Turn Gear into the Big Machine. Evaluates CLI flags (`--help`, `--version`, `--audit`, `--debug`, `--cli`, `--gui`).
   * Binds simulation state (`player_state_t`) to engine viewports and dispatches turns.
   * Automatically detects active X11 display, launching GUI when available and gracefully falling back to terminal CLI.
5. **Layer 4: Verification & Test Suite (`tests/`)**:
   * Automated CTest verification suites spanning Functional cogs, Subsystem units, Integrated DAG, and System golden replays.

### 2.2 Block 2 Structural Anatomy & The 4 Sub-Assemblies

Block 2 in every engine header and source file defines the domain ontology, boundary limits, and pre-conditions. It must begin with the standard banner:

```c
/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer <N> (<Stratum Name>) | Machine: <Subsystem Identifier>
================================================================================
Mission: <Clear, actionable scope statement describing this module's mission>
================================================================================
*/
```

Block 2 is organized into **4 standardized Sub-Assemblies**:

```text
┌────────────────────────────────────────────────────────────────────────┐
│               BLOCK 2: SUB-ASSEMBLY STRUCTURAL PIPELINE                │
├────────────────────────────────────────────────────────────────────────┤
│  [ Include Hierarchy ] 3-Tier Downward Include Ladder                  │
│           │                                                            │
│           ▼                                                            │
│  [ Sub-Assembly 2.1 ]  Pre-Conditions, Constants & Buffer Limits       │
│           │            (Every #define has its own Doxygen docstring)   │
│           ▼                                                            │
│  [ Sub-Assembly 2.2 ]  Manifest & Pre-Flight Hooks                     │
│           │            (Defines <SUBSYSTEM>_MODULE_ID string)          │
│           ▼                                                            │
│  [ Sub-Assembly 2.3 ]  Structural Entity & Object Definitions          │
│           │            (Typedef structs/enums with Designer/Programmer)│
│           ▼                                                            │
│  [ Sub-Assembly 2.4 ]  Pre-Conditional Helper Cogs & Non-Business      │
│                        (Static inline validation predicates & guards)  │
└────────────────────────────────────────────────────────────────────────┘
```

1. **Sub-Assembly 2.1: Pre-Conditions, Constants & Buffer Limits**:
   * Compile-time sizing limits, array capacities, timing bounds, and default dimensions.
   * **Individual Docstring Invariant**: **Every single `#define` constant MUST have an individual Doxygen docstring (`/** ... */`) immediately preceding it.** Naked, undocumented, or grouped `#define` blocks without individual docstrings are strictly prohibited.
   * Provide dual-audience descriptions for designers (screen dimensions, time durations, volume caps) and systems programmers (.rodata placement, zero heap guarantees).

2. **Sub-Assembly 2.2: Manifest & Pre-Flight Hooks**:
   * Every header and module must define its canonical unique module identifier string:
     ```c
     /** Unique module identifier string for the <Subsystem Name> subsystem. */
     #define <NAME>_MODULE_ID  "<stratum>.<subsystem>"
     ```
   * Binds the module to the central diagnostic registry in `engine/include/core/manifest.h` for pre-flight verification and telemetry logging.

3. **Sub-Assembly 2.3: Structural Entity & Object Definitions**:
   * All `typedef struct` and `typedef enum` declarations, as well as opaque forward declarations (e.g. `typedef struct player_state_s player_state_t;`).
   * **Strict Type Docstring Invariant**: Every `struct`, `enum`, and forward-declared type must feature a complete docstring with:
     * Explicit `@struct <name>` or `@enum <name>`.
     * Comprehensive `@brief <summary>` description.
     * `For Designers:` Section explaining what game mechanics, narrative concepts, or UI elements this entity controls.
     * `For Programmers:` Section detailing memory invariants (stack-allocated, zero heap), byte alignment, pointer safety, and invariant guarantees.
     * Every individual struct field and enum value must be documented using trailing (`/**< ... */`) or block comments.

4. **Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines**:
   * Pure `static inline` inspection predicates and non-business query helpers (e.g., `<name>_is_valid()`, `<name>_is_active()`, `<name>_clamp()`).
   * **Invariants**: Zero dynamic heap allocations, pure reentrancy, zero global state modification, and fast execution for pre-flight assertions and boundary validation before entering Block 3 execution logic.

---

### 2.3 The 3-Tier Downward Include Hierarchy

Every header and source file must strictly organize its `#include` directives into the **3-Tier Downward Include Hierarchy**:

```c
/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
--------------------------------------------------------------------------------
Tier 1: ISO C11 Standard Library Headers
Tier 2: External Library Dependencies (None in Layer 0/1; native X11 in Layer 2)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* Tier 1: ISO C11 Standard Library Headers */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/* Tier 2: External Library Dependencies (None) */

/* Tier 3: Internal Engine Foundation Headers */
#include "core/manifest.h"
#include "<subsystem>/<header>.h"
```

* **Tier 1 (Standard C Library)**: Standard ISO C11 headers (`<stdbool.h>`, `<stdint.h>`, `<stddef.h>`, `<string.h>`, `<stdlib.h>`, `<stdio.h>`).
* **Tier 2 (External Libraries)**: Third-party dependencies (strictly None in Layer 0 and Layer 1; `<X11/Xlib.h>` in Layer 2 presentation).
* **Tier 3 (Internal Engine)**: Engine foundation headers (`"core/manifest.h"`, child cogs). Dependencies must flow strictly downward.

---

### 2.4 Subsystem Umbrella Facades (Parity Standard)

To maintain a consistent developer experience across all engine strata, every subsystem folder in `engine/include/` provides a primary umbrella facade header matching its folder name:

| Facade Header | Stratum | Unified Subsystems & Capabilities |
|---|---|---|
| [`engine/include/core/core.h`](file:///home/professorseanex/Tenebris%20and%20Lux/engine/include/core/core.h) | Layer 0 (Core) | Mathematical bounds, 2D geometry, monotonic time, safe bounded strings, frame pacer, and formula evaluators. |
| [`engine/include/io/io.h`](file:///home/professorseanex/Tenebris%20and%20Lux/engine/include/io/io.h) | Layer 0 (I/O) | Zero-heap filesystem I/O, pathfinder 3-tier VFS, JSON tokenizer, and master data manifest loader. |
| [`engine/include/audio/audio.h`](file:///home/professorseanex/Tenebris%20and%20Lux/engine/include/audio/audio.h) | Layer 0 (Audio) | 7-channel hardware mixing bus, audio event bus queue, ducking state machine, and audio fallback profile. |
| [`engine/include/viewport/viewport.h`](file:///home/professorseanex/Tenebris%20and%20Lux/engine/include/viewport/viewport.h) | Layer 2 (Viewport) | Native X11 double-buffered rasterizer, 2D viewports, HUD widgets, layout, Movie Reel, font blitter, styling palettes, procedural assets, and GUI fallbacks. |

---

### 2.5 Quarantined Fallback Isolation Standard

Every core engine subsystem must be immune to filesystem corruption, missing configuration files, or headless test execution by maintaining dedicated **Quarantined Fallback Cogs** (`*_fallback.[ch]`):

* **Quarantine Invariant**: Fallback headers (`*_fallback.h`) must **never** include main production headers (`gui.h`, `reel.h`, `audio_bus.h`) to prevent circular inclusion loops.
* **Production Integration**: Main production headers include their corresponding fallback header in Tier 3, making emergency presets immediately accessible.
* **Provenance Invariant**: Fallback configuration structures must unconditionally enforce `loaded_from_data = false`. This guarantees diagnostic telemetry never misidentifies emergency defaults as data-driven configurations.
* **Fallback Verification**: Every fallback cog must have a dedicated unit test in `tests/unit/` verifying its baseline defaults, fault-tolerant guards, and pre-flight audit tripwires.

#### Current Engine Fallback Roster:
* `core/math_utils_fallback.[ch]` (Mathematical saturating bounds)
* `core/geom_utils_fallback.[ch]` (Reference 640x360 viewports & letterbox scalers)
* `core/time_utils_fallback.[ch]` (Emergency epoch timestamps)
* `core/string_utils_fallback.[ch]` (Bounded string formatting)
* `core/clock_pacer_fallback.[ch]` (60 FPS default pacing policy)
* `core/formula_eval_fallback.[ch]` (Canonical combat damage formulas)
* `log/log_fallback.[ch]` (Emergency static flight recorder)
* `debug/debug_fallback.[ch]` (Emergency snapshot and breakpoint inspection)
* `audio/audio_fallback.[ch]` (Emergency 7-channel volume defaults)
* `viewport/raster/gui_fallback.[ch]` (Canonical 860x620 window, layout & obsidian palette synthesis, headless `$DISPLAY` detection)
* `viewport/reel/reel_fallback.[ch]` (0.25s transition duration, 1.05x pop/0.95x sink scaling, smoothstep easing)

---

### 2.6 Strict ISO C-Style Commenting Invariant (Zero `//` Comments)

To guarantee strict compliance with standard ISO C11 and avoid comment-style divergence across tools, linters, and compilers:

* **Strict C Block Comments**: All comments in C source files (`.c`) and C headers (`.h`) must use standard C block comment syntax:
  * Section dividers and inline comments: `/* ... */`
  * Doxygen docstrings: `/** ... */`
  * Field docstrings: `/**< ... */`
* **Zero `//` Comments**: C++ style single-line comments (`//`) are **strictly prohibited** in all engine and game C code. The single permitted exception is repository URL protocols within Block 1 metadata (e.g. `@repository https://github.com/...`).

---

### 2.7 Strict Include Pathing (No Forwarders)

All header includes must explicitly declare their stratum and subsystem. Lazy forwarder headers in include roots are strictly forbidden:

```c
#include "core/core.h"
#include "core/math_utils.h"
#include "core/geom_utils.h"
#include "core/time_utils.h"
#include "core/string_utils.h"
#include "core/manifest.h"
#include "io/io.h"
#include "io/fs_utils.h"
#include "io/json_utils.h"
#include "io/path_finder.h"
#include "io/master_loader.h"
#include "log/log.h"
#include "debug/debug.h"
#include "audio/audio.h"
#include "viewport/viewport.h"
#include "combat/combat.h"
#include "hero/sim_state.h"
#include "relics/relics.h"
#include "world/world.h"
#include "dispatch/dispatcher.h"
#include "ending/game.h"
```

### 2.8 Continental Lore & Canon Invariants

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

## BLOCK 3: BODY / CORE BUSINESS LOGIC & EXECUTION RULES

> **Cognitive Role in the Mind: Algorithmic Execution & Domain Mechanics**  
> In the mind, Block 3 implements the core engine and domain calculations: *What are the business rules? What calculations, transformations, and algorithms execute here? What non-execution functions drive the simulation and presentation?*

### 3.1 The Body Block Contract (Non-Execution & Non-Verification Invariant)

Block 3 across all engine files (`engine/include/` and `engine/src/`) is strictly reserved for **pure business logic, calculations, and non-execution based algorithms**:

* **What Belongs in Block 3**:
  * Pure mathematical transformations and bounded calculations (`math_utils`, `geom_utils`, `time_utils`).
  * Kinematics, interpolation, and easing curve evaluation (`reel`, `clock_pacer`).
  * String sanitization, token scanning, and hashing (`string_utils`, `json_utils`).
  * 3-tier VFS path resolution and file reading (`path_finder`, `fs_utils`).
  * Manifest hydration and configuration parsing (`master_loader`, `formula_eval`).
  * Telemetry ring buffer writes and state hashing (`log`, `debug`).
  * Audio event queue enqueueing and volume attenuation calculations (`audio_bus`).
  * Viewport backbuffer drawing, rasterization, font blitting, and dimetric projection (`gui_view2d`, `gui_hud`, `gui_widgets`, `gui_screens`, `gui_palette`, `gui_assets`).
* **Strict Exclusions from Block 3**:
  1. **Zero Verification / Validation Logic**: Pre-flight checks, self-tests, invariant audits, bounding verification, and diagnostic tripwires belong **strictly in Block 4 (Audit Block)** under Sub-Assembly 4.1 (`*_audit()`, `*_preflight()`).
  2. **Zero Execution Loops**: Continuous execution loops (such as the 60 FPS presentation pump `gui_run()`) belong **strictly in Block 5 (Closing Block)** under Sub-Assembly 5.1.
  3. **Zero Lifecycle Initializers or Teardown**: Startup allocators/constructors (`*_init()`) and destruction/cleanup hooks (`*_teardown()`, `*_shutdown()`) belong **strictly in Block 5 (Closing Block)** under Sub-Assembly 5.2.

### 3.2 Block 3 Sub-Assembly Structural Anatomy

Every Block 3 section in headers and implementation files must begin with the standard banner specifying its stratum and machine mission:

```c
/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer <N> (<Stratum Name>) | Machine: <Subsystem Identifier>
================================================================================
Mission: <Clear, actionable scope statement describing functional domain>
All execution loops and lifecycle hooks are quarantined in Block 5.
================================================================================
*/
```

Within Block 3, functions are organized into **numbered, domain-specific Sub-Assemblies** reflecting functional clusters:

```text
┌────────────────────────────────────────────────────────────────────────┐
│               BLOCK 3: SUB-ASSEMBLY STRUCTURAL PIPELINE                │
├────────────────────────────────────────────────────────────────────────┤
│  [ Sub-Assembly 3.1 ]  Primary Functional Category / Kinematics / Math │
│           │            (Pure domain algorithms, transformations)       │
│           ▼                                                            │
│  [ Sub-Assembly 3.2 ]  Secondary Domain Category / Raster / Data       │
│           │            (Entity blitting, parsing, queue operations)     │
│           ▼                                                            │
│  [ Sub-Assembly 3.3 ]  Read-Only Queries & State Telemetry Accessors   │
│                        (Safe getters, status checks, progress queries)  │
└────────────────────────────────────────────────────────────────────────┘
```

Each sub-assembly is marked with a standard 80-column header divider:
```c
/*
--------------------------------------------------------------------------------
Sub-Assembly 3.X: <Descriptive Sub-Assembly Name>
--------------------------------------------------------------------------------
<Concise mission statement explaining what operations reside here>
--------------------------------------------------------------------------------
*/
```

### 3.3 The Dual-Audience Function Docstring Covenant

Every function declared in a header or defined in a source file within Block 3 **MUST feature a complete, standardized Doxygen docstring containing explicit Designer and Programmer sections**:

```c
/**
 * @brief <Concise 1-line imperative summary of function behavior>.
 *
 * ### For Game Designers & Non-Programmers:
 * <Plain-English explanation of what this function achieves mechanically,
 * narratively, visually, or experientially. Avoid low-level technical jargon;
 * ground the explanation in game balance, visual motion, sound, or design rules.>
 *
 * ### For Systems Programmers & Engine Architects:
 * <Technical specification detailing algorithmic complexity O(n), memory contracts
 * (ALLOC_ZERO_HEAP), pointer null-safety, numeric bounds clamping, reentrancy,
 * cache locality, or hardware considerations.>
 *
 * @param[in]     <param_name> <Description of input parameter>.
 * @param[out]    <param_name> <Description of output buffer/pointer>.
 * @param[in,out] <param_name> <Description of in-out state pointer>.
 * @return <Description of return value, error conditions, or boolean success>.
 */
```

#### Docstring Quality Checklist:
1. **Never omit either audience**: Omitting `### For Game Designers & Non-Programmers:` or `### For Systems Programmers & Engine Architects:` violates the documentation invariant.
2. **Standard Directional Tags**: All pointer parameters must be tagged with `@param[in]`, `@param[out]`, or `@param[in,out]`.
3. **Pure C Comment Invariant**: Docstrings must strictly use `/** ... */`; never `///`.

### 3.4 Facade Header Cross-Reference Standard (Parity Across Strata)

For subsystem umbrella facade headers (`core.h`, `io.h`, `audio.h`, `viewport.h`), Block 3 does not declare redundant function prototypes. Instead, it provides a comprehensive cross-reference manifest linking every functional domain header included in Block 2:

```c
/*
================================================================================
BLOCK 3: BODY BLOCK / API DECLARATIONS
Stratum: Layer <N> (<Stratum Name>) | Machine: <Subsystem Umbrella>
================================================================================
Note: As the master aggregator umbrella for Layer <N>, all functional APIs are
declared in their respective domain headers included in Block 2:
- <subsystem>/<header_1>.h (<function_1>, <function_2>)
- <subsystem>/<header_2>.h (<function_3>, <function_4>)
================================================================================
*/
```

### 3.5 Deterministic Stance State Machine

Every stance transition forms a strongly connected, deterministic finite state machine:

* `STANCE_NEUTRAL`: Baseline posture, balanced stamina recovery.
* `STANCE_WARDING` (*Lux Barrio*): Raises radiant dome, absorbs 25% damage, kinetic reflect.
* `STANCE_STRIKING` (*Cuttatio Tenebris*): Lowers center of gravity, sweeping arc + 20% flank splash.
* `STANCE_EQUILIBRIUM` (*Duo-Loca Bashin n Slash*): Harmonic shield bash + abyssal double-cleave targeting raw DEF.

### 3.6 Bounded Arithmetic & Conserved Resources

* Shield warding charge: Strictly clamped in `[0, 100]` (`MAX_RESONANCE`).
* Sword edge sharpness: Strictly clamped in `[0, 100]` (`MAX_RESONANCE`).
* Moral Sway continuum: Strictly bounded in `[-100, +100]`.
* Zero-divisor guards: All mathematical operations must use `safe_div()` or saturating math from `math_utils.h`.

### 3.7 Midpoint Campaign Fork & The 5-Route Ending Matrix

* **The King's Curse**: The open planar wound beneath the High Throne and possession of King Jihad.
* **The Dream Compass**: Evolved from Grandma's Watch through nocturnal calibration.
  * Entering the Royal Capital without the calibrated Compass triggers the tragic *Neutral Path (The Forgotten Stasis)*.
  * Exposing the wound with the calibrated Compass unlocks the *True Ending Paths*:
    1. *Path of Radiance (Lux Overload)*: Moral sway $\ge +75$; light crystallization.
    2. *Path of the Abyssal Sovereign (Tenebrous Eclipse)*: Moral sway $\le -75$; absolute shadow.
    3. *The Sovereign Accord (Dual Harmonic Balance)*: Equilibrium maintained `[-25, +25]`.
    4. *The Cycle Shattered (Planar Dissolution)*: Transcending both relics; healing the 3,000-year breach.
    5. *The Forgotten Stasis (The King's Slumber)*: Default neutral closure; dynastic repetition.

### 3.8 Universal Orchestrators & Engine Tick Synchronization

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
  * Ingests mathematical curves from `engine/config/runtime/math/` (`formulas.json`, `attributes.json`, `affinities.json`).
  * Pure reentrant function: `formula_eval_damage(attacker, defender, stance, affinity)`.

---

## BLOCK 4: AUDIT PROCEDURES & DIAGNOSTIC TRIPWIRES

> **Cognitive Role in the Mind: Verification, Validation, Telemetry & Invariant Defense**  
> In the universal lifecycle protocol, Block 4 stands as the crucial gateway between pure algorithm declaration (Block 3) and runtime execution (Block 5). It acts as the self-reflective immune system:
>
> 1. **VERIFYING (Sub-Assembly 4.1)**: Ensures that the incoming information, configurations, and parameter contracts match data provenance schemas, and verifies that the functions declared in Block 3 execute properly as standalone, isolated unit cogs.
> 2. **VALIDATING (Sub-Assembly 4.2)**: Validates that the verified functions and configuration data harmonize holistically across the subsystem, checking system stability, invariant tripwires, and providing the formal pre-flight clearance handshake before advancing to the final step of executable procedures and lifecycles in Block 5.

```text
┌────────────────────────────────────────────────────────────────────────────────────────┐
│                        BLOCK 4: AUDIT PROCEDURES ARCHITECTURE                          │
├────────────────────────────────────────────────────────────────────────────────────────┤
│ Sub-Assembly 4.1: Contract, Input & Cog Verification Cogs                             │
│   • Mission: Verify received parameters, configuration contracts, and data provenance;│
│     verify that declared subsystem cogs run properly as standalone units.             │
│   • Cogs: <subsystem>_audit(), schema validation, parameter bounds queries.            │
├────────────────────────────────────────────────────────────────────────────────────────┤
│ Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires                 │
│   • Mission: Validate holistic system invariants, tripwires, and subsystem stability;  │
│     provide pre-flight clearance handshake before advancing to Block 5 execution.      │
│   • Cogs: <subsystem>_preflight() -> diag_result_t or bool pass/fail handshake.       │
└────────────────────────────────────────────────────────────────────────────────────────┘
```

### 4.1 Sub-Assembly 4.1: Contract, Input & Cog Verification Cogs

Sub-Assembly 4.1 hosts all verification functions that evaluate incoming parameters, external manifests, data provenance, and isolated unit function execution:

* **Scope & Boundaries**:
  * Evaluates received arguments against bounds limits and pre-conditions.
  * Verifies that declared subsystem cogs run properly as standalone units without relying on external system state.
  * Validates JSON schema versions and manifest envelopes (`master_loader_validate_schema()`, `master_loader_validate_version()`).
  * Queries and inspects diagnostic tripwire flags (`<subsystem>_get_tripwire_status()`, `<subsystem>_get_tripwire_details()`, `<subsystem>_reset_tripwire()`).
* **Function Naming & Return Archetypes**:
  * `<subsystem>_audit(...)` returning `bool` (or taking parameter structs for modular bounds verification, e.g. `gui_layout_audit(layout)`, `gui_widgets_audit(buttons, count, w, h)`).
  * Contract and schema validators returning `bool` or specialized validation enum codes.

### 4.2 Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires

Sub-Assembly 4.2 hosts holistic self-audits and pre-flight validation tripwires that test the complete subsystem in harmony and issue the operational clearance handshake:

* **Scope & Boundaries**:
  * Runs automated startup self-tests across the subsystem's interconnected cogs.
  * Tests nominal, boundary, and responsive configurations (e.g. testing layout across 1080p, 720p, and 480p in `gui_layout_preflight()`).
  * Checks hardware accessibility and environmental fallbacks (e.g. X11 `$DISPLAY` probing in `gui_preflight()`).
  * Serves as the official pre-flight clearance handshake required by the module manifest system (`manifest.h`) before invoking executable procedures in Block 5.
* **Function Naming & Return Archetypes**:
  * `<subsystem>_preflight(void)` returning `diag_result_t` (reporting `SEVERITY_PASS`, `WARNING`, `CRITICAL`, or `FATAL` along with check names and diagnostic descriptions) or `bool` pass/fail.
  * Every operational engine module must expose both a verification cog (`_audit`) and a validation pre-flight tripwire (`_preflight`).

### 4.3 Pure Type Headers (Ontological Definition Exemption)

Headers dedicated purely to type definitions, data enumerations, packet structures, and inline predicates (such as `audio_types.h` and `gui_types.h`) do not contain business logic or executable routines. To maintain 5-block parity across all strata:

* Blocks 3, 4, and 5 in pure type headers must feature explicit explanatory notes defining their ontological, non-business, non-procedural nature:
  ```c
  /*
  ================================================================================
  BLOCK 4: AUDIT PROCEDURES BLOCK
  Stratum: Layer <N> (<Stratum Name>) | Subsystem: <Types Subsystem>
  ================================================================================
  Mission: Verify incoming parameters, contracts, and schema invariants;
  validate standalone function execution and system stability for Block 5.
  ================================================================================
  (<Subsystem> core types define purely static structures, enums, and bounds-checking
  inlines. Functional audits, tripwires, and pre-flight routines are declared in
  <subsystem>/<operational_header_1>.h and <subsystem>/<operational_header_2>.h.)
  ================================================================================
  */
  ```

### 4.4 Subsystem Facade Umbrella Standard (Two-Stage Cross-References)

Master aggregator umbrella headers (`core.h`, `io.h`, `audio.h`, `viewport.h`) mirror this two-stage division with explicit cross-references rather than redundant function declarations:

* **Sub-Assembly 4.1: Contract, Input & Cog Verification Cross-References**: Comprehensive catalog of all verification cogs declared across the subsystem's modular headers.
* **Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Cross-References**: Comprehensive catalog of all pre-flight clearance tripwires across the subsystem.

### 4.5 Dual-Audience Documentation Covenant for Block 4

Every function declared in Block 4 **MUST** feature a standardized Doxygen docstring containing both designer and programmer sections:

```c
/**
 * @brief <Concise 1-line summary of audit or pre-flight behavior>.
 *
 * ### For Game Designers & Non-Programmers:
 * <Plain-English explanation of what this diagnostic check tests, what game
 * rules or visual assets it protects, and why passing it guarantees stability.>
 *
 * ### For Systems Programmers & Engine Architects:
 * <Technical specification detailing tested invariants, memory bounds (ALLOC_ZERO_HEAP),
 * tripwire failure severities, side-effect-free guarantees, or display requirements.>
 *
 * @return <diag_result_t or bool outcome reporting audit pass/fail status>.
 */
```

### 4.6 Audit Invariance & Side-Effect Freedom

All Block 4 audit and pre-flight functions must be strictly **side-effect free and idempotent**:

1. **State Preservation**: An audit must not leave modified global state, altered registers, or leaked resources behind. If a test temporarily modifies state (e.g. `gui_assets_audit` temporarily binding external art), it must restore the original state before returning.
2. **Zero Dynamic Allocation**: Conforms strictly to `ALLOC_ZERO_HEAP` in Layer 0/1/2. All test structures and scratch memory must be stack-allocated.
3. **Graceful Diagnostics**: Never invoke `abort()` or `exit()` inside an audit function. Always report failures via boolean false or `diag_result_t` with structured descriptions.

### 4.7 Interactive State Debugger & Rewind

* **Snapshot Ring Buffer**: Captures frozen simulation states (up to 64 snapshots).
* **FNV-1a Deterministic Hashing**: Evaluates hash of `player_state_t` to instantly detect non-deterministic drift.
* **Time-Travel Rewind (`rewind <idx>`)**: Reverts simulation state to a historical snapshot.
* **Diagnostic ASCII Cards (`inspect`)**: Live visual readout of hero vitals, relics, and tripwires formatted to 60-column terminal-friendly cards.

### 4.8 Automated Verification Strata (64/64 Passing)

Automated verification is structured into 4 strata integrated into CTest:

1. `tests/functional/`: Isolates individual cogs across utilities, simulation, audio, and debugger with forensic `[DEBUG-TRACE]` telemetry (7 suites).
2. `tests/unit/`: Subsystem unit verification across `math`, `time`, `string`, `geom`, `pacer`, `formula`, `io` (`fs`, `json`, `path_finder`, `master_loader`), `log`, `debug`, `audio`, `viewport` (`palette`, `layout`, `widgets`, `fonts`, `assets`, `view2d`, `hud`, `screens`, `raster`, `reel`, and dedicated fallbacks), and game simulation (`combat`, `items`, `world`, `dispatch`) (39 suites).
3. `tests/integrated/`: Cross-utility DAG verification (`test_utils`), multi-subsystem simulation boot (`test_sim`), data-loader integration (`test_io`), logging, debugging, audio bus, and viewport integration (8 suites).
4. `tests/system/`: Engine state limits (`test_game_invariants`), 15-step golden replay determinism, 1,000-turn chaotic fuzzing softlock immunity (`test_gameplay_scenarios`), and long-running endurance runs across `core`, `io`, `log`, `debug`, `audio`, and `viewport` (10 suites).

### 4.9 The 4 Ontological Testing Sections

Every test file in `tests/` must strictly implement the **4 Ontological Testing Sections**:

1. **SECTION 1: Passing / Nominal Cases**: Standard expected operations, nominal inputs, happy paths, and baseline state transitions.
2. **SECTION 2: Failing / Boundary & Error Cases**: Null pointers, empty strings, buffer overflows, out-of-range bounds, corrupted data, and graceful degradation.
3. **SECTION 3: Invariant Assertions & State Proofs**: Mathematical bounds checking, state machine closed loops, zero dynamic heap proofs, and deterministic hash invariance.
4. **SECTION 4: Emergence & Stress / Fuzzing Cases**: Multi-turn sequences, stress-testing static limits, high-frequency call saturation, and randomized input fuzzing.

---

## BLOCK 5: CLOSING / EXECUTABLE PROCEDURES, LIFECYCLE & VERIFICATION

> **Cognitive Role in the Mind: Operational Execution, Diagnostic Cards & Subsystem Lifecycle**  
> In the universal lifecycle protocol, Block 5 is the final culmination and operational delivery phase. Prior blocks established identity (Block 1), static types and constants (Block 2), algorithms and business rules (Block 3), and contract verification with pre-flight clearance (Block 4). Block 5 advances the verified cogs into live executable procedures, terminal diagnostic status reporting, and subsystem lifecycle management:
>
> 1. **EXECUTABLE PROCEDURES & DIAGNOSTICS (Sub-Assembly 5.1)**: Runs high-level operational execution pipelines (such as `gui_run`), formats 60-column terminal-friendly ASCII status cards (`_execute_dump_card`), and constructs structured telemetry inspection objects (`_execute_inspect`).
> 2. **SUBSYSTEM LIFECYCLE & TEARDOWN (Sub-Assembly 5.2)**: Manages subsystem startup initialization (`_init`) and graceful teardown (`_shutdown`), opening file handles, mounting rings, and clearing buffers. For stateless modules, an explicit standard exemption is enforced.

```text
┌────────────────────────────────────────────────────────────────────────────────────────┐
│                        BLOCK 5: CLOSING PROCEDURES ARCHITECTURE                        │
├────────────────────────────────────────────────────────────────────────────────────────┤
│ Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards                 │
│   • Mission: Run operational execution procedures, format terminal-friendly ASCII      │
│     status cards, and construct structured telemetry inspection objects.               │
│   • Cogs: <subsystem>_execute_dump_card(), <subsystem>_execute_inspect(), gui_run().   │
├────────────────────────────────────────────────────────────────────────────────────────┤
│ Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown                        │
│   • Mission: Manage subsystem lifecycle, allocate runtime ring buffers,                │
│     open/close file streams, and tear down hardware handles.                           │
│   • Cogs: <subsystem>_init(), <subsystem>_shutdown().                                  │
│   • Stateless Exemption: "None. (Stateless module / pure stack-allocated execution)."   │
└────────────────────────────────────────────────────────────────────────────────────────┘
```

### 5.1 Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards

Sub-Assembly 5.1 hosts all operational execution entry points, headless diagnostic cards, and structured inspection queries:

* **Scope & Boundaries**:
  * Top-level presentation loops, runtime pump execution, and batch processing pipelines (e.g. `gui_run` in `viewport/raster/gui.h`, `master_loader_execute_load_verified` in `io/master_loader.h`).
  * Emits 60-column terminal-friendly ASCII diagnostic dump cards for live engine inspection, CI traces, and headless testing without graphical surfaces.
  * Constructs programmatic telemetry inspection objects (`_execute_inspect`) populating structured metrics into caller-allocated memory.
* **The Universal Diagnostic Dump Card Standard**:
  * Every operational and fallback engine module implements `<subsystem>_execute_dump_card(char *buffer, size_t capacity)` (or with an instance pointer for contextual state, e.g. `const reel_t *reel`).
  * **Zero Heap Guarantee**: Writes exclusively into the caller's stack-allocated or static buffer via bounded `snprintf`.
  * **Safe Error Handling**: Returns the exact number of characters written on success, or `-1` if `buffer == NULL` or `capacity == 0`.
  * **60-Column ASCII Framing**: Formatted with standard 60-column header and footer divider bars:
    ```text
    ============================================================
     VIEWPORT HUD SUBSYSTEM DIAGNOSTIC CARD                    
    ============================================================
     Subsystem       : HUD Presentation & Chronicle Log         
     Status          : OPERATIONAL                              
     Max Log Lines   : 16                                       
     Max Line Length : 128                                      
     Pre-Flight Audit: PASSED                                   
    ============================================================
    ```
  * **Pre-Flight Invariant Visibility**: Every dump card evaluates the module's pre-flight status (`_preflight` or `_audit`) and explicitly displays `PASSED` or `FAILED`.

### 5.2 Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown

Sub-Assembly 5.2 hosts the initialization, hardware binding, resource allocation, and teardown procedures for the subsystem:

* **Scope & Boundaries**:
  * **Stateful Modules**: Modules possessing static registers, ring buffers, filesystem mounts, or hardware handles (`log`, `debug`, `fs_utils`, `json_utils`, `master_loader`, `path_finder`, `audio_bus`, `gui_assets`, `gui_fonts`, `reel`) implement:
    * `<subsystem>_init(...)`: Sets initial capacity, clears ring buffers, registers canonical entities, and sets initialization flags. Must be **idempotent** (repeated calls do not double-initialize or leak).
    * `<subsystem>_shutdown(...)`: Flushes pending streams, frees X11 font structures, resets internal counters, and clears state memory via `memset`.
* **The Stateless Exemption Standard**:
  * Modules that perform pure mathematical evaluation, geometric transformations, string manipulation, time measurement, or pure stack-allocated layout without persistent static state (`math_utils`, `geom_utils`, `string_utils`, `time_utils`, `clock_pacer`, `formula_eval`, `gui_hud`, `gui_layout`, `gui_widgets`, `gui_view2d`, `gui_palette`, `gui_screens`, and all quarantined `*_fallback` modules) are exempt from lifecycle hooks.
  * **Mandatory Standard Comment**: To maintain 5-block architectural consistency, stateless modules **MUST** include the following standard note under Sub-Assembly 5.2:
    ```c
    /*
    --------------------------------------------------------------------------------
    Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
    Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
    open/close file streams, and tear down hardware handles.
    --------------------------------------------------------------------------------
    None. (Stateless module / pure stack-allocated execution).
    --------------------------------------------------------------------------------
    */
    ```

### 5.3 Pure Type Headers (Ontological Definition Exemption)

Headers dedicated purely to type definitions and inline predicates (`audio_types.h`, `gui_types.h`) do not declare procedural routines or lifecycle hooks. In Block 5, they must feature an explicit explanatory note:

```c
/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer <N> (<Stratum Name>) | Subsystem: <Types Subsystem>
================================================================================
Mission: Execute operational pipelines, format ASCII diagnostic cards,
construct programmatic inspection objects, and manage subsystem lifecycles.
================================================================================
(Pure type definition header: purely static structures, enums, and inline predicates.
Operational execution procedures, subsystem lifecycle initialization, and diagnostic
dump cards are declared in <subsystem>/<operational_header>.h.)
================================================================================
*/
```

### 5.4 Subsystem Facade Umbrella Standard (Two-Stage Cross-References)

Master aggregator umbrella headers (`core.h`, `io.h`, `audio.h`, `viewport.h`) mirror this two-stage division with explicit cross-references cataloging constituent execution entry points, dump cards, and lifecycle hooks:

* **Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards Cross-References**: Comprehensive catalog of all operational execution loops, programmatic inspection functions, and ASCII dump cards declared across the subsystem.
* **Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown Cross-References**: Catalog of all initialization and destruction hooks, accompanied by notes identifying which cogs operate with pure stack allocation.

### 5.5 Dual-Audience Documentation Covenant for Block 5

Every function declared in Block 5 **MUST** feature a standardized Doxygen docstring containing both designer and programmer sections:

```c
/**
 * @brief <Concise 1-line summary of procedure or lifecycle action>.
 *
 * ### For Game Designers & Non-Programmers:
 * <Plain-English explanation of what this procedure executes, what screen or
 * sound is triggered, or how this function initializes/shuts down the game system.>
 *
 * ### For Systems Programmers & Engine Architects:
 * <Technical specification detailing buffer capacity, memory management (ALLOC_ZERO_HEAP),
 * idempotency guarantees, thread-safety, or X11 backbuffer blitting parameters.>
 *
 * @param[out] buffer Output destination buffer.
 * @param[in] capacity Maximum buffer capacity in bytes.
 * @return <int character count, 0 on success, or negative on error>.
 */
```

---

### 5.6 Build, Test, and Quality Commands

The project compiles under CMake 3.24+ for ISO C11 with zero warnings (`-Wall -Wextra -Wpedantic`):

```bash
# Configure and compile
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug

# Execute full CTest verification suite (64/64 test suites must pass)
ctest --test-dir build -C Debug --output-on-failure

# Execute diagnostic self-audit (registered modules + invariant tests)
./build/tenebris-lux --audit

# Validate all game data manifests and schemas (20/20 Rust tool tests pass)
cargo test --manifest-path tools/Cargo.toml

# Audit and verify on-disk presentation assets (41/41 visual/audio assets valid)
cargo run --manifest-path tools/Cargo.toml --bin asset_generator -- --audit

# Execute unified developer CLI toolchain
cargo run --manifest-path tools/Cargo.toml --bin tl_tools -- assets
cargo run --manifest-path tools/Cargo.toml --bin tl_tools -- validate

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

### 5.7 Structured Git Commit Guidelines

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
- CTest results and pass rates (e.g. 64/64 suites passing).
- Diagnostic audit status (e.g. ./build/tenebris-lux --audit: 18 modules pass, 7 invariant tests pass).
- Data validation status (e.g. cargo test: 46/46 data files pass).
- Compiler diagnostic check (-Wall -Wextra -Wpedantic clean).
```

### 5.8 AsciiDoc Documentation Standards

All technical manuals, design logs, and realm chronicles in `docs/` are standardized in **AsciiDoc (`.adoc`)**:

* Keep lines under 100 characters where practical.
* Escape tildes (`\~` or write "approximately") to prevent unclosed subscript warnings.
* Use relative cross-references (`xref:`, `link:`) rather than absolute URLs within documentation files.
* Utilize semantic admonitions (`[NOTE]`, `[IMPORTANT]`, `[WARNING]`) and attributed quotes.
