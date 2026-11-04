<!--
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Version History & Release Cadence Contracts )
================================================================================
@project       Tenebris and Lux
@file          CHANGELOG.md
@layer         Documentation & Release Strata (Layer 3)
@version       0.1.0-dev
@interface_rev Rev 2
@date          2026-11-03
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Dual-Frontend C11 Tactical RPG Engine & Continental Campaign
@role          Evolutionary Milestone Chronicle & Pedagogical Dev Log Index
@invariants    1. Adheres to Semantic Versioning (SemVer 2.0.0).
               2. Conforms to Keep a Changelog (v1.1.0) formatting.
               3. Grounded in chronological Developer Logs (DevLogs 001–014).
               4. Enforces the Universal 5-Block Lifecycle Protocol.
               5. Authentic project canon (No deprecated synthetic placeholders).

PURPOSE & LUDOLOGICAL MISSION:
  Chronicle the evolutionary milestones, ludological breakthroughs, engine
  architectural invariants, and release notes for Tenebris and Lux.
  Organized using the Universal 5-Block Documentation Architecture:
  Block 1: Metadata -> Block 2: Roadmap -> Block 3: Releases ->
  Block 4: Audit Procedures -> Block 5: Dev Log Compendium Index.

THEMATIC INVARIANT:
  "Trauma is Inherited; Destiny is Chosen."
================================================================================
-->

# Changelog & Engineering Chronicle

All notable evolutionary milestones, architectural breakthroughs, and ludological revisions for *Tenebris and Lux* are documented in this file.

This project adheres to [Semantic Versioning (SemVer 2.0.0)](https://semver.org/spec/v2.0.0.html) and follows the [Keep a Changelog (v1.1.0)](https://keepachangelog.com/en/1.1.0/) standard, organized through the **OmniCode Universal 5-Block Documentation Architecture**.

---

## Version Cadence & Milestone Matrix

| Version | Milestone Name | Status | Release Date | Core Strata & Focus | DevLog Citations |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **`0.1.0-dev`** | Tactical Grid Engine & Continental Topology | In Progress | 2026-Q4 | 2D Grid, ZOC, Formations, 5-Screen UI, 10 Endings | DevLogs 013, 014 |
| **`0.0.1`** | Dual-Frontend Interactive Prototype | Verified | 2026-11-03 | C11 Zero-Heap L0/L1, X11 GUI, CLI Debugger, 19 CTests | DevLogs 001–014 |
| **`0.0.0`** | Historical Genesis & Demo Baseline | Archival | 2014–2018 | RPG Maker MV Prototype, Storyboard, Data Extraction | DevLogs 002, 007 |

---

## BLOCK 2: ROADMAP & FUTURE HORIZONS (PREMISE: v0.1.0-dev)

### [0.1.0-dev] - In Active Development (Milestone: Tactical Grid Engine)

The `v0.1.0` milestone transforms the verified interactive prototype into a full Tactical Strategy RPG (TRPG) grid engine, integrating the mechanics articulated in [DevLog 013](docs/dev_logs/2026/11/lore_and_design/devlog_013_morality_architecture_bystander_corridor_and_ten_endings.adoc) and [DevLog 014](docs/dev_logs/2026/11/architecture/devlog_014_high_fantasy_presentation_engine_typography_scaling_and_5_screen_architecture.adoc).

#### 1. Tactical TRPG Grid Engine (Layer 1 Simulation)

- **Discrete 2D Battlefield Coordinates**: Transition from abstract topological room navigation (`move north`) to discrete tile coordinates (`(x, y)` cells) across 32x32 continental zone grids.
- **Movement Points (MP) & Terrain Scalers**: Tile-based movement budgets evaluated against biome terrain costs (plains: 1 MP, dense forest: 2 MP, stone ramparts: 1.5 MP, river ford: 3 MP).
- **Directional Facing & Spatial Geometry**: Cardinal facing (North, South, East, West) calculating flank (+20% damage) and rear (+40% critical chance) hit profiles using `geom_utils.h`.
- **Zone-of-Control (ZOC) Mechanics**: Hostile units project a kinetic engagement radius; moving past an unengaged enemy clamps movement and inflicts poise strain unless shielded by *Lux Barrio*.

#### 2. Party Roster & Formation Dynamics (Layer 1 Simulation)

- **Multi-Unit Deployment**: Squad roster management transitioning between world-map travel and grid deployment.
- **Formation Synergy Buffs**: Positioning Elijah Ro-enheart (Vanguard Shield) adjacent to Lady Katrina Jaren (Abyssal Flanker) establishes a kinetic corridor granting +15% barrier deflection and +10% critical severance.
- **Diegetic Item System Integration**: Grandma's Watch evolving into the *Dream Compass* through nocturnal calibration; *Compass of Morality* reading 13-tier continuum alignment.

#### 3. High Fantasy Presentation & 5-Screen Architecture (Layer 2 GUI)

- **5-Screen Navigation Loop**: Full implementation of the UI screens specified in DevLog 014:
  1. *Tactical Battlefield Screen*: Dynamic 3x widescreen battlefield zoom with 96x96 sprites, ground foundation aprons, and health/resonance meters.
  2. *Hero Dossier Screen*: Comprehensive character stat card, relic resonance, bound skills, and moral sway continuum readouts.
  3. *32-Slot Bag / Inventory Screen*: Zero-heap grid inventory with diegetic tooltips, quest catalysts, and herbal remedies.
  4. *Provincial Map Screen*: Continental chart of the Six Powers displaying territorial tensions and caravan supply lines.
  5. *Deeds Codex Screen*: Historical timeline tracking honored oaths, provincial choices, and active ending trajectory.
- **High-DPI TrueType Font Engine**: Multi-tiered typography scaling utilizing embedded fonts (`CinzelDecorative-Bold.ttf`, `Cinzel-Bold.ttf`, `MedievalSharp.ttf`).

#### 4. Expanded Ending Topology (Narrative Strata)

- **10-Ending Trajectory Engine**: Implementation of the 3x3 moral matrix + 10th True Ending (*"The Ugly Truth"*) codified in DevLog 013:
  - *The Bystander Corridor (`[-50, +50]`)*: Quantifying moral inertia where refusal to take a stand leads to default dynastic stagnation.
  - *The Butterfly Effect*: Provincial micro-choices breaking the 3,000-year cycle of the King's Curse.

---

## BLOCK 3: BODY CHRONICLE: RELEASES & EVOLUTIONARY MILESTONES

### [0.0.1] - 2026-11-03 (Milestone: Dual-Frontend Interactive Prototype)

Milestone `0.0.1` represents the complete architectural foundation of *Tenebris and Lux*, establishing a pure ISO C11 engine with zero dynamic heap allocations in core layers, verified by 19 automated CTest test suites and a 16-module diagnostic self-audit.

#### Layer 0: Core Utilities (`engine/src/{core,io,log,debug}/`, `engine/include/`)

- **Central Manifest & Pre-Flight DAG** ([DevLog 003](docs/dev_logs/2026/10/architecture/devlog_003_preflight_and_manifest_architecture.adoc), [DevLog 004](docs/dev_logs/2026/10/architecture/devlog_004_pedagogical_metadata_and_dependency_contracts.adoc)):
  - Built zero-heap Central Manifest Registry (`core/manifest.h`, `core/manifest.c`) validating subsystem integrity.
  - Implemented boot-time Directed Acyclic Graph (DAG) upstream dependency validation (`dependencies[4]`) halting on missing prerequisites.
  - Formulated 5-tier diagnostic severity hierarchy (`TRACE`, `INFO`, `WARN`, `CRITICAL`, `FATAL`) and 4-tier engine startup policies.
- **Domain Subsystem Modularization** ([DevLog 010](docs/dev_logs/2026/11/architecture/devlog_010_layer_0_subsystem_modularization_and_data_driven_architecture.adoc)):
  - Decomposed utility monoliths into dedicated zero-dependency sub-assemblies:
    - `core/math_utils.h`: Clamping (`clamp_int`), saturating arithmetic, zero-divisor guards, and normalization.
    - `core/geom_utils.h`: 2D spatial geometry, Axis-Aligned Bounding Box (AABB) hit-testing, and coordinate containment.
    - `io/fs_utils.h`: Zero-heap filesystem I/O with fixed-size line buffers (no `malloc`).
    - `io/json_utils.h`: Streaming, zero-heap JSON tokenizer and recursive value parser.
    - `log/log.h`: Categorized forensic telemetry (`CAT:LOG`, `CAT:CHARACTER`, `CAT:DEBUG`), grade mapping (`GRADE:+003`), static ring buffer, and quarantined fallback monoliths (`log_fallback.h`).
    - `debug/debug.h`: Interactive state debugger, frozen snapshot ring buffer (up to 64 snapshots), FNV-1a state hashing, and time-travel rewind.
    - `core/string_utils.h`: Bounded buffer safety, safe copying, whitespace trimming, and path joining.
    - `core/time_utils.h`: Monotonic sub-millisecond clock timers, ISO-8601 timestamps, and frame profiles (60/120 FPS).
- **Pedagogical Standards & Code Hygiene** ([DevLog 005](docs/dev_logs/2026/10/architecture/devlog_005_ontological_code_layering_and_docstrings.adoc), [DevLog 006](docs/dev_logs/2026/10/architecture/devlog_006_source_and_root_pedagogical_commenting.adoc)):
  - Formulated the Machine-Cog ontological framework: Stratum &rarr; Machine &rarr; Assembly &rarr; Cog &rarr; Fasteners.
  - Enforced strict 80-column line width discipline across 100% of the C codebase.
  - Standardized rich Markdown LSP hover docstrings across all headers and sources.

#### Layer 1: Simulation Engine (`game/src/{combat,hero,relics,world,ending,dispatch}/`, `game/include/`)

- **Tri-Perspective Protagonist Architecture** ([DevLog 002](docs/dev_logs/2026/10/lore_and_design/devlog_002_the_sacyra_chronicles_and_trpg_vision.adoc), [DevLog 009](docs/dev_logs/2026/11/lore_and_design/devlog_009_macro_history_thematic_triad_and_multi_ending_architecture.adoc)):
  - Modeled state for Elijah Ro-enheart (The Past), Lady Katrina Jaren (The Present), and The Custom Bearer (The Future) in `hero/sim_state.h`.
  - Implemented dynamic hero designation (`name <bearer>`).
- **Twin Relic Metaphysics & Resonance Conservation** ([DevLog 001](docs/dev_logs/2026/10/architecture/devlog_001_genesis_and_pivot.adoc), [DevLog 008](docs/dev_logs/2026/10/lore_and_design/devlog_008_canonical_skills_and_dreamscape_baseline.adoc)):
  - Conserved Relic resources: Shield of Lux warding and Sword of Tenebris edge sharpness strictly bounded in `[0, 100]`.
  - Moral Sway Continuum: Bounded tracking in `[-100, +100]`.
  - Signature Arts: *Lux Barrio* (-25% damage absorption, 2-turn barrier dome), *Cuttatio Tenebris* (sweeping arc, 20% splash), and harmonic *Duo-Loca Bashin n Slash* (blinding shield bash + abyssal double-cleave targeting raw DEF).
- **Deterministic Stance State Machine** ([DevLog 001](docs/dev_logs/2026/10/architecture/devlog_001_genesis_and_pivot.adoc), [DevLog 008](docs/dev_logs/2026/10/lore_and_design/devlog_008_canonical_skills_and_dreamscape_baseline.adoc)):
  - 4-posture strongly connected deterministic state machine: `STANCE_NEUTRAL`, `STANCE_WARDING`, `STANCE_STRIKING`, `STANCE_EQUILIBRIUM` in `combat/stances.h`.
- **Topological World & Command Dispatching** ([DevLog 008](docs/dev_logs/2026/10/lore_and_design/devlog_008_canonical_skills_and_dreamscape_baseline.adoc), [DevLog 010](docs/dev_logs/2026/11/architecture/devlog_010_layer_0_subsystem_modularization_and_data_driven_architecture.adoc)):
  - Built world orchestrator and topological room scaler (`world/world.h`, `world/world.c`) managing The Dreamscape and Dio Village Square.
  - Implemented dynamic command router (`dispatch/dispatcher.h`, `dispatch/dispatcher.c`) translating aliases into canonical domain verbs.

#### Layer 2: Presentation & Viewports (`engine/src/viewport/`, `engine/include/viewport/`)

- **Native Double-Buffered X11 Desktop GUI** ([DevLog 001](docs/dev_logs/2026/10/architecture/devlog_001_genesis_and_pivot.adoc), [DevLog 014](docs/dev_logs/2026/11/architecture/devlog_014_high_fantasy_presentation_engine_typography_scaling_and_5_screen_architecture.adoc)):
  - Built native X11 double-buffered interface (`viewport/gui.h`, `viewport/gui.c`) rendering at 640x360 target resolution with letterbox/pillarbox scalers.
  - Real-time rendering of health meters, relic resonance bars, stance indicators, hostile threat readouts, and provincial chronicles.
  - Decoupled from simulation logic: GUI translates user events into simulation commands without owning game state.

#### Layer 3: Application Entry & Router (`game/src/main.c`)

- **Dual-Frontend Presentation Router** ([DevLog 001](docs/dev_logs/2026/10/architecture/devlog_001_genesis_and_pivot.adoc)):
  - Automatic environment detection: launches X11 GUI when `DISPLAY` is available, with graceful fallback to terminal CLI.
  - Command-line argument parser: `--help`, `--version`, `--audit`, `--debug`, `--cli`, `--gui`.
- **Interactive CLI State Debugger** ([DevLog 012](docs/dev_logs/2026/11/architecture/devlog_012_interactive_debugging_system_and_state_rewind_architecture.adoc)):
  - In-game debugger CLI commands: `inspect` (ASCII status card), `snap <label>` (freeze state snapshot), `history` (snapshot timeline), and `rewind <idx>` (revert simulation state).

#### Layer 4: Verification Suite (`tests/`)

- **Four-Tier Verification Strata** ([DevLog 011](docs/dev_logs/2026/11/architecture/devlog_011_four_tier_verification_strata_and_ontological_testing_framework.adoc)):
  - Reorganized automated verification into 4 explicit strata integrated into CTest (19/19 suites passing):
    - *Functional*: `test_functional` isolating individual cogs across utilities, simulation, and debugger with `[DEBUG-TRACE]` telemetry.
    - *Unit*: Subsystem unit verification across `math`, `data`, `time`, `string`, `log`, `debug`, `combat`, `items`, `world`, `dispatch`, `gui`.
    - *Integrated*: Cross-utility DAG verification (`test_utils`) and multi-subsystem simulation boot (`test_sim`).
    - *System*: Engine state limits (`test_game_invariants`), 15-step golden replay determinism, and 1,000-turn chaotic fuzzing softlock immunity (`test_gameplay_scenarios`).

#### Developer Tooling & Documentation Compendium (`tools/`, `docs/`)

- **Master Guide Compiler Toolchain** ([DevLog 009](docs/dev_logs/2026/11/lore_and_design/devlog_009_macro_history_thematic_triad_and_multi_ending_architecture.adoc)):
  - Standalone Rust 2021 compiler (`tools/src/guide.rs`) compiling 67 chapters across 6 volumes into:
    - [`docs/MASTER_GUIDE.adoc`](docs/MASTER_GUIDE.adoc) (AsciiDoc unified book)
    - [`docs/MASTER_GUIDE.md`](docs/MASTER_GUIDE.md) (GitHub Markdown mirror)
    - [`docs/MASTER_GUIDE.pdf`](docs/MASTER_GUIDE.pdf) (Production publication PDF via `asciidoctor-pdf` with `rouge` syntax highlighting)
- **Archival Extraction Utilities** ([DevLog 007](docs/dev_logs/2026/10/lore_and_design/devlog_007_rpg_maker_demo_data_extraction_and_baseline.adoc)):
  - Built Python extractor (`docs/tools/extract_demo_data.py`) parsing historical RPG Maker MV datasets into engine JSON schemas.

---

### [0.0.0] - Historical Genesis (2014–2018)

The foundation of *Tenebris and Lux* was established during 2014–2018 in RPG Maker MV as the flagship game of CreativeWorkzStudio before its publishing evolution ([DevLog 002](docs/dev_logs/2026/10/lore_and_design/devlog_002_the_sacyra_chronicles_and_trpg_vision.adoc), [DevLog 007](docs/dev_logs/2026/10/lore_and_design/devlog_007_rpg_maker_demo_data_extraction_and_baseline.adoc)).

#### Historical Milestones

- **Thematic Provenance**: Conceived the Twin Relics (Shield of Lux, Sword of Tenebris) and metaphysical neutrality (Light and Darkness as cosmic forces).
- **Continental Worldbuilding**: Formulated the Kingdom of Sacyra, the 3,000-year dynastic wave from the First King to King Jihad, and the Accord of Kings spanning the Six Powers.
- **Narrative Storyboard**: Codified the 20-Scene Master Storyboard Sequence from the Dreamscape awakening to the confrontation beneath the High Throne.
- **Archival Data Preservation**: Historic map trees, skill databases, actor profiles, and item lists recovered and archived in `docs/game_design/demo_baseline/` and `docs/offline_work/`.

---

## BLOCK 4: AUDIT PROCEDURES, TRIPWIRES & TELEMETRY MANIFEST

Every milestone release is governed by rigorous pre-flight audits and invariant tripwires. The current telemetry baseline confirms 100% verification satisfaction:

### Pre-Flight Diagnostic Audit Manifest (16 Subsystems Registered)

```text
================================================================================
           Tenebris and Lux - Subsystem Invariant Pre-Flight Audit              
================================================================================
Subsystem            Source File     Rev  Memory      Status  Description
--------------------------------------------------------------------------------
core.util.manifest   manifest.h      r2   Zero-Heap   [PASS]  Pre-flight manifest DAG orchestrator & registry verified.
core.util.fs         fs_utils.h      r2   Zero-Heap   [PASS]  Zero-heap filesystem I/O, file read, line buffer & write verified.
core.util.json       json_utils.h    r2   Zero-Heap   [PASS]  JSON utilities: token scan, string, int, bool, float & array verified.
core.util.time       time_utils.h    r2   Zero-Heap   [PASS]  Monotonic clock stable; sub-ms resolution & ISO timestamp verified.
core.util.math       math_utils.h    r2   Zero-Heap   [PASS]  Clamping, saturation, zero-divisor safety & normalization verified.
core.util.geom       geom_utils.h    r2   Zero-Heap   [PASS]  2D AABB geometry & bounds containment verified.
core.util.string     string_utils.h  r2   Zero-Heap   [PASS]  Buffer safety, NULL resilience, trimming & path joining verified.
core.util.log        util/log/log.h  r2   Zero-Heap   [PASS]  Grade mapping, labels, and static ring buffer verified.
core.util.debug      debug.h         r1   Zero-Heap   [PASS]  Debugger preflight: Zero-heap snapshots, deterministic hashing verified.
sim.engine.state     sim_state.h     r4   Zero-Heap   [PASS]  State initialization and invariant audit tripwires passed.
sim.engine.combat    combat.h        r5   Zero-Heap   [PASS]  All modular combat sub-assemblies satisfied pre-flight.
sim.engine.items     items.h         r2   Zero-Heap   [PASS]  All item sub-assemblies audited cleanly.
sim.engine.world     world.h         r5   Zero-Heap   [PASS]  World orchestrator, scaler, topology, and map catalogs verified.
sim.engine.dispatcher dispatcher.h    r4   Zero-Heap   [PASS]  Dispatcher orchestrator, domain routers, and alias tables verified.
sim.engine.core      game.h          r4   Zero-Heap   [PASS]  All modular simulation sub-assemblies satisfied pre-flight.
gui.viewport.x11     gui.h           r2   Zero-Heap   [PASS]  X11 display connected; 2.5D viewport & panel geometry verified.
--------------------------------------------------------------------------------
Telemetry: 16 modules registered | 16 Pass | 0 Warn | 0 Critical | 0 Fatal
Policy Verdict: ALL PRE-FLIGHT INVARIANTS SATISFIED (System Ready)
================================================================================
Audit Invariant Checks:  7/7 Invariant Checks Passed (Relics, Stances, Tripwires)
CTest Suite Pass Rate:   19/19 Verification Suites Passed (100% Pass Rate)
Execution Latency Budget: < 50µs per simulation turn
Compiler Status:         -Wall -Wextra -Wpedantic Clean (0 Warnings, 0 Errors)
================================================================================
```

---

## BLOCK 5: ARCHIVAL RETROSPECTIVE & DEV LOG COMPENDIUM

The following master index maps every chronological Developer Log in `docs/dev_logs/` directly to its subsystem, release impact, and engineering deliverable:

| DevLog | Date | Domain | Title & Core Engineering Deliverable | Primary Subsystems Affected |
| :--- | :--- | :--- | :--- | :--- |
| **[DevLog 001](docs/dev_logs/2026/10/architecture/devlog_001_genesis_and_pivot.adoc)** | 2026-10-24 | Architecture | *Genesis, The Paradigm Pivot & Game Dev Invariants* | ISO C11 engine boot, 5-block structure, X11 GUI, CLI |
| **[DevLog 002](docs/dev_logs/2026/10/lore_and_design/devlog_002_the_sacyra_chronicles_and_trpg_vision.adoc)** | 2026-10-26 | Lore & Design | *The Sacyra Chronicles, Dual Protagonists & TRPG Evolution* | Sacyra canon, Elijah & Katrina state modeling, TRPG vision |
| **[DevLog 003](docs/dev_logs/2026/10/architecture/devlog_003_preflight_and_manifest_architecture.adoc)** | 2026-10-26 | Architecture | *Central Manifest, Executable Block 1 & Pre-Flight Architecture* | `manifest.h`, `manifest.c`, 5-tier diagnostic severity |
| **[DevLog 004](docs/dev_logs/2026/10/architecture/devlog_004_pedagogical_metadata_and_dependency_contracts.adoc)** | 2026-10-26 | Architecture | *Pedagogical Metadata, ISO 8601 Timelines & DAG Contracts* | Boot DAG dependency validation, `interface_rev`, contracts |
| **[DevLog 005](docs/dev_logs/2026/10/architecture/devlog_005_ontological_code_layering_and_docstrings.adoc)** | 2026-10-26 | Architecture | *Ontological Sub-Assemblies, LSP Docstrings & 80-Col Discipline* | Machine-Cog framework, rich LSP docstrings, 80-column rule |
| **[DevLog 006](docs/dev_logs/2026/10/architecture/devlog_006_source_and_root_pedagogical_commenting.adoc)** | 2026-10-27 | Architecture | *Source Implementation & Root Pedagogical Commenting* | Step-by-step comments in `src/`, root configuration hygiene |
| **[DevLog 007](docs/dev_logs/2026/10/lore_and_design/devlog_007_rpg_maker_demo_data_extraction_and_baseline.adoc)** | 2026-10-29 | Lore & Design | *Archival Extraction & The 2017–2018 Demo Baseline* | `extract_demo_data.py`, 10-volume demo baseline documentation |
| **[DevLog 008](docs/dev_logs/2026/10/lore_and_design/devlog_008_canonical_skills_and_dreamscape_baseline.adoc)** | 2026-10-30 | Lore & Design | *Canonical Skills, The Dreamscape & Ground-Truth Pacing* | *Lux Barrio*, *Cuttatio Tenebris*, The Dark Lord identity |
| **[DevLog 009](docs/dev_logs/2026/11/lore_and_design/devlog_009_macro_history_thematic_triad_and_multi_ending_architecture.adoc)** | 2026-11-02 | Lore & Design | *Macro-History, Thematic Triad & Multi-Ending Architecture* | 3,000-year dynastic wave, Thematic Triad, 5-route ending matrix |
| **[DevLog 010](docs/dev_logs/2026/11/architecture/devlog_010_layer_0_subsystem_modularization_and_data_driven_architecture.adoc)** | 2026-11-02 | Architecture | *Layer 0 Subsystem Modularization & Data-Driven Architecture* | `math/`, `geom/`, `data/`, `log/`, `debug/`, JSON tokenizer |
| **[DevLog 011](docs/dev_logs/2026/11/architecture/devlog_011_four_tier_verification_strata_and_ontological_testing_framework.adoc)** | 2026-11-02 | Architecture | *Four-Tier Verification Strata & Ontological Testing Framework* | Functional cogs, Unit, Integrated, System golden replay |
| **[DevLog 012](docs/dev_logs/2026/11/architecture/devlog_012_interactive_debugging_system_and_state_rewind_architecture.adoc)** | 2026-11-02 | Architecture | *Interactive Debugging System & State Rewind Architecture* | Snapshot ring buffer, FNV-1a hashing, time-travel rewind |
| **[DevLog 013](docs/dev_logs/2026/11/lore_and_design/devlog_013_morality_architecture_bystander_corridor_and_ten_endings.adoc)** | 2026-11-03 | Lore & Design | *Morality Architecture, Bystander Corridor & Ten-Ending Matrix* | `[-50, +50]` Bystander Corridor, 10 endings, diegetic relics |
| **[DevLog 014](docs/dev_logs/2026/11/architecture/devlog_014_high_fantasy_presentation_engine_typography_scaling_and_5_screen_architecture.adoc)** | 2026-11-03 | Architecture | *High Fantasy Presentation Engine & 5-Screen Architecture* | High-DPI TrueType fonts, 3x zoom, 5-screen navigation UI |
