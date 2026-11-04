<!--
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          AGENTS.md
@layer         Cognitive Anchor & Repository Governance Strata
@version       0.1.0-dev
@interface_rev Rev 3
@date          2026-11-03
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

---

## 0. Philosophy: The Cognitive Anchor & Compass

This document (`AGENTS.md`) is **not** an inflexible bureaucratic straitjacket or a rigid legal statute meant to override developer judgment or stifle pragmatic engineering.

Rather, it is a **cognitive anchor and compass**. In complex AI-assisted pair-programming workflows involving multi-layered C11 engines, Rust developer tooling, and deep narrative worldbuilding, autonomous agents frequently suffer from **context drift**:

* Inventing synthetic hallucinations (e.g., deprecated placeholder names like "Malakor" instead of the canonical **Dark Lord Usuru**);
* Violating architectural constraints (e.g., introducing heap allocations like `malloc` into zero-heap Layer 0 or Layer 1);
* Polluting repository hygiene (e.g., placing source files or forwarder headers at the repository root);
* Breaking determinism (e.g., coupling rendering viewports into pure simulation logic).

`AGENTS.md` exists to **re-ground you in proven patterns and axiomatic invariants**:

* **Clarity of Architectural Intent**: Understanding *why* code is layered and *why* contracts exist prevents well-meaning changes from breaking subtle guarantees.
* **Cognitive Consistency**: Grounding names, themes, and narrative logic in authentic project canon ensures all systems harmonize like an integrated guidebook.
* **Engineering Hygiene**: Maintaining root directory cleanliness, explicit pathing, zero warnings, and 100% CTest pass rates keeps the codebase pristine.

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

The repository root must remain immaculate. According to project standards, the root directory may ONLY contain essential project anchors:

* `CMakeLists.txt` (Root C11 build orchestrator)
* `.gitignore` (VCS exclusion rules with Block 1 metadata)
* `README.md` (Root documentation anchor with expanded YAML frontmatter)
* `CHANGELOG.md` (Evolutionary chronicle across DevLogs 001–014)
* `LICENSE` (CreativeWorkzStudio Commercial Game & Source-Available Agreement)
* `AGENTS.md` (This cognitive anchor and governance standard)

All other materials belong in their respective strata:

* `engine/`: Zero-heap foundational utilities (`engine/include/util/`, `engine/src/util/`) and Presentation Viewports (`engine/include/gui/`, `engine/src/gui/`).
* `game/`: Tactical Simulation Engine (`game/include/sim/`, `game/src/sim/`) and Application Router (`game/src/main.c`).
* `data/`: Data-driven JSON manifests, subsystem profiles, and tactical schemas (`data/SCHEMAS.md`).
* `tests/<stratum>/`: All automated test suites (`functional/`, `unit/`, `integrated/`, `system/`).
* `docs/<category>/`: All technical manuals and realm chronicles in AsciiDoc format.
* `assets/<type>/`: All fonts (`fonts/`), high-res icons (`icons/`), banners (`branding/`), and screenshots (`screenshots/`).
* `tools/`: Developer toolchain in Rust (`master_guide`, `extractor`).
* `.github/`: Issue templates and linter configurations.

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
Layer 2: Presentation Viewports   [ engine/src/gui/   <-> engine/include/gui/ ]
           │
Layer 1: Simulation Engine        [ game/src/sim/     <-> game/include/sim/ ]
           │
Layer 0: Core Utilities           [ engine/src/util/  <-> engine/include/util/ ]
```

1. **Layer 0: Core Utilities (`engine/src/util/`, `engine/include/util/`)**:
   * Zero-dependency foundational primitives: `math/` (bounded arithmetic), `geom/` (2D AABB geometry), `data/` (zero-heap FS and JSON tokenizer), `log/` (categorized telemetry and static ring buffer), `debug/` (snapshot buffer and FNV-1a hashing), `string/` (bounded safety), `time/` (monotonic timers), and `manifest/` (pre-flight DAG registry).
   * **Hard Invariant**: **Zero dynamic heap allocations** (`malloc`, `calloc`, `free` are prohibited). Pure reentrant ISO C11.
2. **Layer 1: Simulation Engine (`game/src/sim/`, `game/include/sim/`)**:
   * Manages player state (`player_state_t`), twin armaments, combat stance machine, moral sway continuum, provincial topology, and command dispatching.
   * **Hard Invariant**: Zero heap allocations. Completely decoupled from X11 or windowing runtimes. Runs headless under any CLI or test harness.
3. **Layer 2: Presentation & Viewports (`engine/src/gui/`, `engine/include/gui/`)**:
   * Native X11 double-buffered desktop interface. Translates mouse clicks and keystrokes into universal simulation commands.
   * High-DPI TrueType font engine (`Cinzel`, `MedievalSharp`), 640x360 target resolution with letterbox/pillarbox scalers.
4. **Layer 3: Application Entry & Presentation Router (`game/src/main.c`)**:
   * Evaluates CLI flags (`--help`, `--version`, `--audit`, `--debug`, `--cli`, `--gui`).
   * Automatically detects active X11 display, launching GUI when available and gracefully falling back to terminal CLI.
5. **Layer 4: Verification & Test Suite (`tests/`)**:
   * 19 automated CTest verification suites spanning Functional cogs, Subsystem units, Integrated DAG, and System golden replays.

### 2.2 Strict Include Pathing (No Forwarders)

All header includes must explicitly declare their stratum and subsystem. Lazy forwarder headers in include roots are strictly forbidden:

```c
#include "util/math/math_utils.h"
#include "util/math/geom_utils.h"
#include "util/data/fs_utils.h"
#include "util/data/json_utils.h"
#include "util/log/log.h"
#include "util/debug/debug.h"
#include "util/string/string_utils.h"
#include "util/time/time_utils.h"
#include "sim/game.h"
#include "gui/gui.h"
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

---

## BLOCK 4: AUDIT PROCEDURES & DIAGNOSTIC TRIPWIRES

> **Cognitive Role in the Mind: Self-Reflection, Telemetry & Invariant Defense**  
> In the mind, Block 4 acts as the self-reflective immune system: *Did any invariant breach? Is memory leaking? Is the state machine healthy? How do we catch drift before a crash?*

### 4.1 Diagnostic Pre-Flight Audits (`audit_*`)

Every subsystem must expose a dedicated diagnostic routine registered in `manifest.h`:

* Evaluates memory limits, boundary clamping, and state transitions.
* Reports telemetry manifests without relying on crude runtime crashes (`abort()`).
* Categorized logging with forensic grades (`GRADE:+003`).

### 4.2 Interactive State Debugger & Rewind

* **Snapshot Ring Buffer**: Captures frozen simulation states (up to 64 snapshots).
* **FNV-1a Deterministic Hashing**: Evaluates hash of `player_state_t` to instantly detect non-deterministic drift.
* **Time-Travel Rewind (`rewind <idx>`)**: Reverts simulation state to a historical snapshot.
* **Diagnostic ASCII Cards (`inspect`)**: Live visual readout of hero vitals, relics, and tripwires.

### 4.3 Automated Verification Strata (19/19 Passing)

Automated verification is structured into 4 strata integrated into CTest:

1. `tests/functional/test_functional`: Isolates individual cogs across utilities, simulation, and debugger with `[DEBUG-TRACE]` telemetry.
2. `tests/unit/`: Subsystem unit verification across `math`, `data`, `time`, `string`, `log`, `debug`, `combat`, `items`, `world`, `dispatch`, `gui`.
3. `tests/integrated/`: Cross-utility DAG verification (`test_utils`) and multi-subsystem simulation boot (`test_sim`).
4. `tests/system/`: Engine state limits (`test_game_invariants`), 15-step golden replay determinism, and 1,000-turn chaotic fuzzing softlock immunity (`test_gameplay_scenarios`).

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

# Execute full CTest verification suite (19/19 test suites must pass)
ctest --test-dir build -C Debug --output-on-failure

# Execute diagnostic self-audit (16 registered modules + 7 invariant tests)
./build/tenebris-lux --audit

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
- CTest results and pass rates (e.g. 19/19 suites passing).
- Diagnostic audit status (e.g. ./build/tenebris-lux --audit: 16 modules pass).
- Compiler diagnostic check (-Wall -Wextra -Wpedantic clean).
```

### 5.3 AsciiDoc Documentation Standards

All technical manuals, design logs, and realm chronicles in `docs/` are standardized in **AsciiDoc (`.adoc`)**:

* Keep lines under 100 characters where practical.
* Escape tildes (`\~` or write "approximately") to prevent unclosed subscript warnings.
* Use relative cross-references (`xref:`, `link:`) rather than absolute URLs within documentation files.
* Utilize semantic admonitions (`[NOTE]`, `[IMPORTANT]`, `[WARNING]`) and attributed quotes.
