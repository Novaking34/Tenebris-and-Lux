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
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Universal 5-Block Lifecycle Protocol & Agent Standards
@role          Cognitive Compass, Architectural Invariants & Ontological Guide
@invariants    1. Zero dynamic heap allocations in Layer 0 (Util) & Layer 1 (Sim).
               2. Progressive ISO C23 target with graceful C17/C11 floor (-Wall -Wextra -Wpedantic, C++20/17 headers).
               3. Explicit include pathing (No forwarder headers in include root).
               4. Dual-frontend determinism (Headless CLI and X11 share pure sim state).
               5. Authentic project canon (Sole antagonist: The Dark Lord Usuru).
               6. Universal 5-Block Lifecycle Protocol enforced across all files.
               7. Machine vs. Master Turn Gear separation (engine/ vs. game/).
               8. Cog Elevation & Orchestrator Protocol (reusable cogs -> engine/).
               9. Dual Development Discipline (Waterfall bedrock laws; Agile sprints).
               10. Active Iteration Feedback Covenant (expose seams, purge shims).
               11. Standalone Engine Boundary Covenant (zero game/ bleed in engine/).
               12. Procedural Fallback Floor & Safe-Mode BIOS ROM Vault (zero-disk boot).
               13. Dual Data Taxonomy & Layered VFS Precedence (engine/config vs game/data).
               14. Native Compiled Toolchain Invariant (pure Rust/C11 in tools/).
               15. Universal Sub-Assembly Architecture (2.1..2.4 [optional 2.5], 3.X, 4.1..4.2, 5.1..5.2).
               16. Quarantined Fallback Isolation (*_fallback.[ch], *_types.h, loaded_from_data=false).
               17. Dual Audience Documentation Covenant (Designer & Programmer sections).
               18. Strict ISO C-Style Comment Invariant (only /* ... */, /** ... */, /**< ... */).
               19. Controlled Memory Chassis Discipline (measure .rodata/.data/.bss/stack).
               20. Autonomous Probe -> Safe-Mode Downgrade -> DEBUG_STEP_AWAIT Tripwire Loop.
               21. Cornerstone Engine Identity & C++ Header Compatibility (extern "C", Cornerstone guards).

PURPOSE & COGNITIVE ROLE:
  Definitive architectural compass and engineering governance standard for
  autonomous AI agents and human engineers working on Tenebris and Lux.
  Organizes the OmniCode Universal 5-Block Lifecycle Protocol across both
  mindset ("Why") and execution ("How"):
    Philosophy -> Block 1 (Metadata & Hygiene) -> Block 2 (Types & Strata) ->
    Block 3 (Body & Canon) -> Block 4 (Audits & Probes) -> Block 5 (Lifecycle & Workflows).
================================================================================
-->

# Repository Guidelines & Agent Standards

> **Project**: Tenebris and Lux (Flagship RPG) | **Engine**: **Cornerstone Game Engine** (`engine/`)
> **Standard**: OmniCode 5-Block Ontological Architecture & Layered Invariant Discipline
> **Author**: CreativeWorkzStudio LLC / ProfessorSeanEX
> **Repository**: [https://github.com/Novaking34/tenebris-lux](https://github.com/Novaking34/tenebris-lux)
> **Engine Milestone**: `0.1.1-dev` | **Governance Revision**: `Rev 5`

---

## Navigation Index

* [0. Philosophy & Architectural Manifesto](#0-philosophy--architectural-manifesto)
* [BLOCK 1: Metadata, Hygiene & Cognitive Anchor](#block-1-metadata-hygiene--cognitive-anchor)
* [BLOCK 2: Opening, Layered Strata, BIOS/ROMs & Domain Ontology](#block-2-opening-layered-strata-biosroms--domain-ontology)
* [BLOCK 3: Body Logic, Gameplay Mechanics & Orchestrators](#block-3-body-logic-gameplay-mechanics--orchestrators)
* [BLOCK 4: Audit Procedures, Validation Probes & `AWAIT` Tripwire Loop](#block-4-audit-procedures-validation-probes--await-tripwire-loop)
* [BLOCK 5: Closing Procedures, Lifecycle, Exemptions & Git Workflows](#block-5-closing-procedures-lifecycle-exemptions--git-workflows)

---

## 0. Philosophy & Architectural Manifesto

`AGENTS.md` is a **cognitive anchor and compass**, not red tape. In multi-layered C11 engine development, Rust tooling, and deep RPG lore, autonomous sessions fail predictably through context drift: inventing non-canonical names, sneaking heap allocations into deterministic layers, leaving compatibility shims that mask broken contracts, or bloating static buffers without noticing. This guide keeps every subsystem aligned to one coherent machine.

### 0.1 Dual Development Discipline: Waterfall Bedrock & Agile Iteration

Engine engineering demands two complementary disciplines at once:

1. **Waterfall Bedrock (Fixed Physical Laws)**:
   Memory contracts (`ALLOC_ZERO_HEAP` in Layer 0/1/2), strict ISO C warning purity (`-Wall -Wextra -Wpedantic` across Cornerstone's **Fall-Down Ladder**: C23 target -> C17 natural -> C11 floor, and C++20 -> C++17 headers), downward dependency flow, Universal 5-Block file structure, 5-Block JSON envelopes, and deterministic execution rules are non-negotiable.
2. **Agile Living Iteration (Turning the Gears)**:
   Within those physical laws, we modernize one subsystem stratum at a time, testing empirically across all 4 Verification Tiers, elevating reusable cogs into `engine/`, aligning C structs to JSON configs, and exposing seams directly to the compiler.

**Engine-as-a-Java / "Fall-Down Stairs" Portability Contract**:
* Cornerstone builds forward-first for **ISO C23** (`-std=c23`) and **C++20** (`-std=c++20`) without legacy compatibility shims, anchored cleanly to an **ISO C17/C11 + C++17 floor** so older toolchains step down automatically (`CORNERSTONE_C_STANDARD=AUTO|23|17|11`, `CORNERSTONE_CXX_STANDARD=AUTO|20|17`) and step back up natively on modern systems.
* Central capability macros live inside bedrock `engine/include/core/manifest.h` (`CS_C_STD_TIER`, `CS_CXX_STD_TIER`, `CS_NODISCARD`, `CS_MAYBE_UNUSED`, `CS_FALLTHROUGH`, `CS_UNREACHABLE`, `CS_ALIGNOF`, `CS_NORETURN`, `CS_STATIC_ASSERT`, `CS_HAS_EMBED`).

**Rev 5 Subsystem Modernization Spine**:

```text
  Tier-0 BIOS Boot & Diagnostics (Done)     I/O & Boot Foundations     Domain & Presentation
  ┌──────────┐     ┌────────────┐     ┌────────┐     ┌─────────┐     ┌─────────┐     ┌──────────┐     ┌────────┐
  │  log/    │ ──> │   debug/   │ ──> │  io/   │ ──> │  core/  │ ──> │ audio/  │ ──> │ viewport/│ ──> │ game/  │
  │ ROM+Safe │     │ Probes+AWAIT│    │ VFS+ROM│     │ Leaf+ROM│     │ Bus+ROM │     │ 2D Raster│     │ TurnSim│
  └──────────┘     └────────────┘     └────────┘     └─────────┘     └─────────┘     └──────────┘     └────────┘
```

* `log/` and `debug/` established the **Rev 5 autonomous blueprint**: compiled `.rodata` Config ROM vaults (`cmake/GenerateConfigRom.cmake`), floppy-grade Safe-Mode BIOS fallbacks (`*_fallback.[ch]`), `@triggers` cross-references, Block 4 validation probes, and Safe-Mode downgrade into `DEBUG_STEP_AWAIT`.
* Upgrading `io/` immediately next closes the boot-and-ingestion contract before `core/`, `audio/`, `viewport/`, and `game/` hydrate data or wire probes through `master_loader`, `path_finder`, `json_utils`, and `fs_utils`.

### 0.2 The Big Machine (Cornerstone) & The Master Turn Gear (Tenebris and Lux)

> **The Engine (`engine/`) is the Cornerstone Game Engine (The Big Machine).**
> **The Game (`game/`) is Tenebris and Lux (The Master Turn Gear).**

```text
  ┌──────────────────────────────────────────────────────────────────────┐
  │            CORNERSTONE GAME ENGINE — THE BIG MACHINE ( engine/ )     │
  │  [Core Math/Time/Strings]   [VFS & Master Loader]   [Flight Log Ring]│
  │  [Debugger & Probes/AWAIT]  [7-Bus Audio Mixer]     [X11 Viewport]   │
  └──────────────────────────────────┬───────────────────────────────────┘
                                     │ meshes cleanly across Layer 3 Plug
                                     ▼
  ┌──────────────────────────────────────────────────────────────────────┐
  │                   THE MASTER TURN GEAR ( game/ )                     │
  │  [Turn-Based Tactical Grid] [Twin Relics & Stances] [Moral Sway]     │
  │  [Sacyra / Libira Topology] [5-Route Ending Matrix] [RPG Lore Data]  │
  └──────────────────────────────────────────────────────────────────────┘
```

* **2D/2.5D Classic Tactical Perspective**: Simulation coordinates live on pure integer tactical grids (`tile_x`, `tile_y`, `elevation_z`) in Layer 1; Layer 2 projects them onto a native 640x360 logical canvas via deterministic dimetric/letterbox scalers (`geom_utils`).
* **Future Dimensional Horizon**: Because simulation logic is independent of rendering projection, Layer 2 can evolve to true 3D rendering—and `engine/` can host completely different games—without altering core turn mechanics or utility strata.

### 0.3 Controlled Memory Chassis vs. Uncontrolled Simulation Memory

Avoiding `malloc`/`calloc`/`free` in Layers 0, 1, and 2 (`ALLOC_ZERO_HEAP`) is **not** dogma against dynamic memory—it is how we engineer a **Controlled Memory Chassis**:

1. **Measurable Chassis Budget**: With zero heap allocations, every byte consumed by the engine and game lives visibly in ELF `.text`, `.rodata`, `.data`, `.bss`, and bounded stack frames (inspectable directly via `size -A build/tenebris-lux` and `nm --size-sort -S -r`).
2. **Separating Chassis Overhead from Simulation Growth**:
   * **Controlled Memory (Chassis Infrastructure)**: Foundational buffers (`log` ring, `debug` snapshot ring, `audio` cue queue, shared VFS/JSON scratch buffers) must stay tightly sized and intentionally budgeted—never bloated by forgotten function-local `static char[32768]` scratch arrays sitting idle in `.bss`.
   * **Uncontrolled Memory (Growing Simulation Demand)**: As RPG systems expand (new vocations, skills, item catalogs, multi-unit battlefields, quest states), fixed capacities immediately make growth pressure visible. Measuring chassis vs. simulation footprints tells us unambiguously when a simulation container needs to expand cleanly rather than masking runaway demand behind fragmented heap allocations or starving gameplay behind sloppy engine buffers.

### 0.4 Cog Elevation & Universal Orchestrator Protocol

When a helper in `game/` solves a general systems problem (screen wipes, audio cue scheduling, easing, frame timing, JSON formula evaluation) without referencing domain lore (`Elijah`, `Katrina`, `Sacyra`, `Usuru`, `Lux`, `Tenebris`), **elevate it into `engine/` as an Orchestrator Cog**.

### 0.5 Active Iteration Feedback Covenant (Expose Seams, Purge Shims)

Keeping deprecated wrapper functions or legacy structs "for backward compatibility" during Rev 5 sprints hides broken contracts and stale call sites.

1. **Purge Obsolete Shims Immediately**: Remove retired structs/wrappers from active headers and sources.
2. **Let Strict Compilers Map the Seams**: Rely on `-std=c11 -Wall -Wextra -Wpedantic` (and `-Werror` during focused sweeps) plus CTest failures to pinpoint every caller requiring upgrade.
3. **Upgrade Callers Completely**: Update every call site cleanly; never leave temporary local hacks or dual code paths.

### 0.6 Fundamental Boundary Covenants

* **Standalone Engine Boundary (Anti-Domain-Bleed)**: `engine/` must **never** include `game/` headers (`hero/`, `combat/`, `world/`, `relics/`, `ending/`, `dispatch/`) or hardcode character names, story lore, or `game/` paths. `engine/` binds only to generic slots, callbacks, and data schemas.
* **Procedural Fallback Floor & Safe-Mode BIOS ROM**: The engine must boot cleanly, execute diagnostics, log telemetry, mix audio events, and render 2D geometry even with **zero external art, audio, or config files on disk**:
  * **Tier-0 Safe-Mode BIOS (`*_fallback.[ch]`)**: Minimal-dependency fallback cogs compiled into every core subsystem (`log_fallback_boot()` boots before VFS/filesystem initialization).
  * **Compiled-In `.rodata` Config ROM Vault (`cmake/GenerateConfigRom.cmake`)**: Generates `*_rom.h` assets directly from `engine/config/**/*.json` into read-only ELF `.rodata` so fallback boots have authentic structured defaults (never weak stubs or duplicated parser logic) when disk files are missing or damaged.
* **Native Compiled Toolchain Invariant**: Tooling in `tools/` is written in compiled Rust (`cargo`, clean under `-D warnings`) or C11—zero Python, Perl, or ad-hoc scripting runtimes inside the engine pipeline (`cmake/` build-time CMake scripts excluded).

### 0.7 Dual Data Taxonomy, Layered VFS & 5-Block JSON Contract

Mechanical engine settings stay strictly separated from RPG domain lore:

* **`engine/config/` (Engine Mechanical Profiles)**:
  * `viewport/`: `manifest.json`, `settings.json`, `scalers.json`, `layout.json`, `themes.json`, `typography.json`, `assets.json`.
  * `audio/`: `manifest.json`, `settings.json`, `channels.json`.
  * `input/`: `manifest.json`, `default.json`, `gamepad.json`.
  * `runtime/`: `math/` (`formulas.json`, `attributes.json`, `affinities.json`, etc.), `time/`, `string/`, `hotpatch.json`.
  * `diagnostics/`: `telemetry/`, `debugger/`, `taxonomy/`, `formats/`.
  * `standards/`: `manifest.json`, `subsystems.json` (declarative registry of all 44 Cornerstone engine headers: 31 Layer 0 + 13 Layer 2), and `blocks/block_1..5.json`.
  * Root `manifest.json` & `settings.json`.
* **`game/data/` (Game Domain Manifests)**: `entities/`, `mechanics/`, `world/`, `presentation/`.
* **Layered VFS Resolution Precedence (`path_finder` `"config"` domain)**:
  1. `game/config/<rel_path>` (User / game-specific overrides)
  2. `engine/config/<rel_path>` (Standalone engine baseline defaults)
  3. `game/data/presentation/config/<rel_path>` (Symbolic presentation data linkers avoiding duplication)
* **5-Block JSON Envelope**: All JSON configs/manifests carry:
  1. `block_1_metadata` (`project`, `file`, `id`, `layer`, `version`, `interface_rev`, `date`, `author`, `repository`, `role`, `invariants`, plus optional `where_the_data_lives` / `what_it_enables`)
  2. `block_2_opening` (`pre_conditions`, `static_declarations` w/ `schema_id`, `generic_helper_contracts`, `capacities`)
  3. `block_3_body` (pure data payload—only this block is queried by `master_loader` body getters)
  4. `block_4_audit` (`audit_metrics`/`metrics`, `verification_tripwires` `{name, rule, status}`, `self_audit_target`)
  5. `block_5_closing` (`execution_handshake`, `navigation`)

---

## BLOCK 1: METADATA, HYGIENE & COGNITIVE ANCHOR

### 1.1 File Metadata Header Covenant & Block 1 Growth Schema

Every C header, C source, JSON data manifest, CMake file, test harness, Rust tool module, and documentation file must begin with a **Block 1 Metadata Header** aligned to the canonical machine-checked schema and `c_block_1_template` stored in `engine/config/standards/blocks/block_1_metadata.json`:

```c
/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          <repo-relative path, e.g. engine/include/io/path_finder.h>
@id            <cog.<module>[.<variant>] — movement-resilient logical identity>
@layer         <Layer N: Stratum Name>
@archetype     <operational | quarantined_fallback | facade_umbrella | pure_types> | <stateful | stateless>
@version       0.1.1-dev
@interface_rev Rev 5
@date          YYYY-MM-DD
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        <Unified Subsystem Name shared identically by paired .h and .c>
@role          <.h = Public Contract & Seam | .c = Runtime Engine & Chassis>
@maturity      <Bootstrap | Hardening | Rev 5 Baseline | Frozen> | <continuity/upgrade state>
@chassis       zero-heap | <measured .bss/.data/.rodata/stack budget or public bounds>
@scale         <Big-O profile | hard capacity levers & data-driven config path>
@seams         <in: cfg/rom | uses: deps | probes: ids -> DEBUG_STEP_AWAIT | dag: macro>
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2..N. <File-specific verifiable laws only (no blind copy-pasted boilerplate)>

PURPOSE & ARCHITECTURAL ROLE:
  <Concise overview of contract boundary (.h) or runtime execution engine (.c).>

  ### For Game Designers & Non-Programmers:
  <Plain-English tuning, Safe-Mode, workflow, or diagnostic UX impact; .c complements .h without echoing.>

  ### For Systems Programmers & Engine Architects:
  <.h: Ownership & Reentrancy | Extension Seam | Explicit Non-Goal.>
  <.c: Controlled Memory Chassis | Bottleneck & Headroom | Probe & Config Wiring.>
================================================================================
*/
```

**Core Block 1 Standards (`block_1_metadata.json`)**:
1. **Movement-Resilient `@id` vs. Volatile `@file` / `@layer`**: Never bake folder paths or compilation layers into `@id` (deprecated `core.util.*` style breaks whenever cogs move or elevate from `game/` to `engine/`). Keep `@id` invariant (`cog.<module>[.<variant>]` for C cogs; `engine.config.*` / `engine.standards.*` / `game.data.*` for JSON; `tools.*` for Rust), record the current repo-relative path in `@file`, and record the current compilation stratum in `@layer`.
2. **Paired `.h` / `.c` Specialization (Anti-Duplication)**: `.h` and `.c` share identical `@id`, `@layer`, `@archetype`, `@version`, `@interface_rev`, `@date`, and `@system`. They diverge intentionally on `@role`, `@chassis`, `@scale`, `@seams`, and `PURPOSE & ARCHITECTURAL ROLE` (Header = **Contract & Seam**; Source = **Runtime Engine & Measured Chassis**).
3. **Scaling & Growth Telemetry**: Every tag value starts aligned at column 16 (`tag_column_width = 15`). `@maturity`, `@chassis`, `@scale`, and `@seams` expose architectural pressure, ELF `.bss`/`.data`/`.rodata`/stack footprints, capacity configs, and `DEBUG_STEP_AWAIT` probe wiring without enforcing arbitrary line-count ceilings or hand-maintained function lists.
4. **Dual-Audience Documentation Covenant**: Every file Block 1 header and every operational C API docstring (`Block 2` types + `Block 3`..`Block 5` functions) must clearly address both **Designers & Non-Programmers** (plain-English mechanics/experience) and **Systems Programmers & Engine Architects** (memory bounds, complexity, reentrancy, probes, invariants).

### 1.2 Repository Topology & Root Directory Hygiene

Only essential top-level anchors belong at repository root: `CMakeLists.txt`, `.gitignore`, `README.md`, `CHANGELOG.md`, `LICENSE`, `AGENTS.md`. All domain code and configuration live in their designated trees:

* `engine/` — **The Big Machine** (`engine/include/{core,io,log,debug,audio,viewport}/`, `engine/src/...`, `engine/config/{viewport,audio,input,runtime,diagnostics,standards}/`)
* `game/` — **The Master Turn Gear** (`game/include/` & `game/src/{combat,hero,relics,world,ending,dispatch}/`, `game/src/main.c`, `game/data/`, `game/assets/`)
* `cmake/` — CMake build generators (e.g., `GenerateConfigRom.cmake`)
* `tests/` — Layer 4 Verification Suite (`functional/`, `unit/`, `integrated/`, `system/`)
* `tools/` — Native Rust developer CLI suite (`tl_tools`, `validator`, `git_validator`, `asset_generator`, `master_guide`, `demo_extractor`, `launcher`, `updater`)
* `docs/` — AsciiDoc realm chronicles, architecture specs, DevLogs, and the 6-volume Master Guide (`docs/master_guide_manifest.json`)
* `.githooks/`, `.github/`, `.vscode/` — Git commit validator hooks, CI workflows, workspace configs

### 1.3 Transient Output Sandboxing & Untracked Files

* **Out-of-Source Quarantine**: All compiler outputs, runtime logs (`build/logs/`), generated ROM headers (`build/generated/`), and LSP caches (`build/.cache/`) must live under `build/` so `rm -rf build` restores a clean workspace. Never create `logs/` or `.cache/` at repo root.
* **Untracked Local Notes**: Local scratch/design manuscripts such as `Offline_Work_*.md` (e.g., `Offline_Work_5.md`, `Offline_Work_6.md`) must **never** be modified, staged, or deleted by automated workflows unless explicitly directed.

---

## BLOCK 2: OPENING, LAYERED STRATA, BIOS/ROMS & DOMAIN ONTOLOGY

### 2.1 The 5 Compilation Strata & The Engine-Game Plug

Dependencies flow strictly downward; lower layers never depend on higher layers, and `engine/` compiles cleanly without `game/`:

```text
Layer 4: Verification Suite       [ tests/{functional,unit,integrated,system}/ ]
           │
Layer 3: Application Plug/Router  [ game/src/main.c ]  (Meshes Engine <-> Game)
           │
Layer 2: Presentation Viewports   [ engine/{include,src}/viewport/ ] (X11 640x360)
           │
Layer 1: Game Simulation Engine   [ game/{include,src}/{combat,hero,relics,world,ending,dispatch}/ ]
           │
Layer 0: Core Engine & Audio Bus  [ engine/{include,src}/{core,io,log,debug,audio}/ ]
```

### 2.2 Universal Block 2 Sub-Assemblies, Cornerstone Canon & Header/Source Archetypes

Every Block 2 translation unit opens with a standardized `BLOCK 2: OPENING BLOCK` banner (`Engine: Cornerstone Game Engine`, `Stratum:`, `Module:`, `Mission:` aligned at column 15 in strict domains) and `c_block_2_templates` defined in `engine/config/standards/blocks/block_2_opening.json`:

* **Pre-Tier POSIX Gate (`.c` only, when POSIX extensions are required)**:
  Place `#ifndef _POSIX_C_SOURCE / #define _POSIX_C_SOURCE 200809L / #endif` above the Block 2 banner prior to Tier 1 headers (C++-redefinition-safe).
* **3-Tier Downward Include Hierarchy**:
  1. `Tier 1`: ISO C standard library headers (`<assert.h>` for `static_assert` floor parity, `<stdbool.h>`, `<stdint.h>`, `<stddef.h>`, etc.)
  2. `Tier 2`: External libraries (**None** in Layer 0/1; native `<X11/Xlib.h>` only in Layer 2)
  3. `Tier 3`: Internal engine headers (`"core/manifest.h"`, `*_types.h`, paired `*_fallback.h` — strict downward flow, never mid-file)
* **C++ Header Compatibility (`extern "C"`, C++20/17)**:
  Every `.h` header opens `#ifdef __cplusplus \n extern "C" { \n #endif` **immediately after Tier 3 includes** (never wrapping `#include` directives) and closes it in Block 5 right before `#endif /* ... */`. Inline helpers (`2.4`) and types (`2.3`) avoid C++ keywords, partial brace initializers causing `-Wmissing-field-initializers`, and implicit `void *` conversions. Zero-initialized module singletons rely on clean uninitialized `.bss` storage (`static foo_t s_foo;`).
* **Sub-Assembly Roles (`.h` vs. `.c` & Logging/Debugging Wiring)**:
  * **Sub-Assembly 2.1 (`Constants, Limits & Static Proofs`)**: Every `#define` carries Doxygen; `.h` binds public bounds + portable `static_assert(...)`; `.c` binds `MODULE_ID`, `LOCAL_LOG_CAT`, `LOCAL_MODULE_TAG`, internal bounds, and `static_assert`s (never `typedef`s or `.rodata` arrays).
  * **Sub-Assembly 2.2 (`Manifest Anchor [.h] / Private Ontology [.c]`)**: `.h` declares `<MOD>_MODULE_ID` + `extern const module_metadata_t *_manifest` (`operational` & stateful `quarantined_fallback`), constituent index (`facade_umbrella`), or stateless ID note (`pure_types`). `.c` declares file-private `typedef enum`/`struct`s and Block 4 probe forward declarations (`static diag_result_t ..._probe_*(void);`).
  * **Sub-Assembly 2.3 (`Public Ontology [.h] / Unified Memory Chassis [.c]`)**: `.h` declares tagged structs/enums (`typedef struct foo_s { ... } foo_t;`) with Dual-Audience `@brief` + `/**< ... */` per field/enumerator. `.c` unifies **100% of static ELF storage** (`s_*_invariants[]`, `const module_metadata_t *_manifest`, lookup tables, `.bss` state `s_state`, `s_*_probes[]`) matching Block 1 `@chassis`.
  * **Sub-Assembly 2.4 (`Pre-Conditional Helper Inlines & Convenience Macros`)**: Pure, zero-heap, `O(1)` `static inline` bounds predicates, clamps, enum stringifiers, public ergonomic `#define` macros (e.g. `LOG_RECORD`..`LOG_CHAR_MSG`), and scoped frontier logging/tripwire macros (`*_EMIT_LOG`, `LOG_SET_TRIPWIRE`). Keeps Block 2 actively guarding contracts at the frontier; convenience macros never spill into Block 3.
  * **Sub-Assembly 2.5 (`Monitored Support Infrastructure — Optional, .c ONLY`)**: When a `.c` file requires non-inline `static` internal helpers prior to Block 3 so Block 3 remains **pure business logic**, place them strictly in `Sub-Assembly 2.5` — never inside `2.4` and never in `.h` headers. Every `2.5` section MUST declare explicit `Support Role:` and `Pattern Watch:` notes (and `Linkage Rule:` = strictly `static` helpers, zero module-global `.bss` arrays), ensuring support mechanics never become a lazy drawer and cohesive patterns (e.g., span scanners, POSIX symlink guards) visibly signal when to split out a dedicated cog/file later.

**6 Specialized Opening Block Profiles + 1 Universal Fallback** (`engine/config/standards/blocks/block_2_opening.json`):
* **`.h` Archetypes**: `operational` (`cog.h`), `quarantined_fallback` (`cog_fallback.h`), `facade_umbrella` (`subsys.h`), and `pure_types` (`cog_types.h`).
* **`.c` Archetypes**: `operational` (`cog.c`) and `quarantined_fallback` (`cog_fallback.c`).
* **Universal Fallback**: Baseline C/H skeleton (`universal_fallback`) for unclassified or future Layer 1/3 modules.

### 2.3 Explicit Pathing, Include Guards & Strict ISO C Commenting

* **Explicit Include Pathing**: Always specify stratum directories (`#include "core/manifest.h"`, `#include "io/path_finder.h"`). Root forwarder headers are banned.
* **Include Guards**: Modernized Cornerstone headers (`engine/`, strict domains `log/`, `debug/`, `io/`) use dir-qualified `#ifndef CORNERSTONE_<SUBSYS>_<FILE>_H` paired with `#endif /* CORNERSTONE_<SUBSYS>_<FILE>_H */`. `game/` headers (and un-migrated legacy engine headers during transition) use `TENEBRIS_LUX_<PATH>_H`.
* **Strict ISO C Comment Invariant (Zero `//`)**: Every `.c` and `.h` file uses strictly `/* ... */`, `/** ... */`, and `/**< ... */`. C++ `//` line comments and `///` doc comments are forbidden (the sole `//` exception is `https://` / `rom://` URL schemes).

### 2.4 Quarantined Safe-Mode Fallbacks, Pure-Types Splits & Compiled `.rodata` Config ROM Vault

Fallbacks (`*_fallback.[ch]`) are **not** minimal dummy stubs—they are the **Tier-0 Safe-Mode BIOS** capable of running standalone even on a bare boot medium without external config directories:

1. **Quarantine Include & `pure_types` Extraction Rule**: `*_fallback.h` includes ONLY `core/manifest.h`, shared `*_types.h` (`log/log_types.h`, `debug/debug_types.h`), or sibling `*_fallback.h` in Tier 3, and never non-fallback primary headers. Primary headers include both `*_types.h` and paired `*_fallback.h` cleanly in Tier 3 (never mid-file) so fallbacks compile standalone and defaults are always compiled in.
2. **Stateful BIOS Manifest Parity**: Any `quarantined_fallback` marked `"stateful": true` in `subsystems.json` (`log_fallback`, `debug_fallback`, `io_fallback`) exposes `extern const module_metadata_t *_fallback_manifest;` consistently.
3. **Provenance Guarantee**: Emergency fallback configs must set `loaded_from_data = false` so diagnostics never misidentify Safe-Mode state as disk-loaded configuration.
4. **ROM-First Hydration (`cmake/GenerateConfigRom.cmake`)**: Build-time CMake embeds canonical JSON configs (`engine/config/...`) into generated `*_rom.h` `.rodata` tables compiled into fallback modules.
5. **Zero-Duplication Parser Sharing**: Pure validation/parsing helpers live in one canonical path shared across disk-load and `.rodata`-ROM hydration—never copy-pasted across `.c` files.

### 2.5 Canonical World Lore & Ontology

* **The Twin Relics**: *Shield of Lux* (light, barrier resonance, poise, deflection) & *Sword of Tenebris* (darkness, sharpness, severance, abyssal mist).
* **Metaphysical Neutrality**: Light and Darkness are morally neutral cosmic forces; intent determines morality (`moral_sway` in `[-100, +100]`).
* **The Thematic Triad ("Trauma is Inherited, Destiny is Chosen")**:
  * *Past*: **Elijah Ro-enheart** & ancient **Usuru**.
  * *Present*: **Lady Katrina Jaren** & **Prince Casatti**.
  * *Future*: **The Custom Bearer** & **Prince Mi-ren**.
* **The Geopolitical Six Powers**: Royal Capital of Sacyra, Kingdom of Libira, Great Mau Kingdom, Old Sacyra, The Academy, and the Yun-yin Provinces (*Accord of Kings*).
* **Sole Antagonist Invariant**: **The Dark Lord** is **Usuru**. Deprecated placeholders (e.g., `"Malakor"`) are strictly banned.

---

## BLOCK 3: BODY LOGIC, GAMEPLAY MECHANICS & ORCHESTRATORS

### 3.1 Body Block Purity & Archetype Boundary Contract

Block 3 (`BLOCK 3: BODY BLOCK`) is the **pure business-logic engine** between pre-conditional foundations (Block 2) and procedural execution/lifecycle wiring (Blocks 4–5). Templates for the universal fallback (`universal_fallback_{header,source}`), 4 `.h` archetypes, and 2 `.c` archetypes live in `engine/config/standards/blocks/block_3_body.json` (`c_block_3_templates`) alongside strict Cornerstone banners (`Engine:`, `Stratum:`, `Module:`, `Mission:` aligned at column 15 in strict domains):

* **Belongs in Block 3**: Public operational declarations (`.h`) and non-`static` C11 business-logic definitions (`.c`) — math/geometry/time transforms, safe string/JSON scanning, VFS resolution, manifest hydration, flight logging, snapshot/tripwire recording, audio mixing, and 2D X11 rasterization. `pure_types` headers contain a statutory empty stub; `facade_umbrella` headers contain a constituent API summary only.
* **Strictly Forbidden in Block 3**:
  * **Convenience `#define` Macros** -> Must live in **Block 2 `Sub-Assembly 2.4`** (e.g., `LOG_RECORD`..`LOG_CHAR_MSG` in `log.h`).
  * **Private `static` Helpers & Storage** -> Must live in monitored **Block 2 `Sub-Assembly 2.5`** (`.c` sources) so helper proliferation reveals future module-extraction seams.
  * **Self-Audits & Preflights (`*_audit`, `*_preflight`)** -> Must live in **Block 4**.
  * **Executable Procedures, Print Dumps & Interactive Pumps (`*_dump_state`, `gui_run`)** -> Must live in **Block 5 `Sub-Assembly 5.1`**.
  * **Lifecycle Entry/Exit (`*_init*`, `*_boot`, `*_shutdown`, `*_teardown`)** -> Must live in **Block 5 `Sub-Assembly 5.2`**.

### 3.2 Sub-Assembly 3.X Parity & "Magically Deleted Header" Docstring Covenant

1. **1:1 `.h` <-> `.c` Sub-Assembly Parity**: Every `operational` and `quarantined_fallback` pair must expose identical `Sub-Assembly 3.1..3.N` numbers, titles, and symbol placement between header and source so C tooling can audit parity mechanically.
2. **Reconstructible `.c` Documentation**: Assume the `.h` file was deleted and must be regenerated solely from the `.c` file. Every non-static `.c` function definition (and every `.h` declaration) must carry full Doxygen (`@brief`, `### For Game Designers & Non-Programmers:`, `### For Systems Programmers & Engine Architects:`, `@param[in|out|in,out]`, `@return`, `@triggers`), with **complementary** `.c` vs `.h` prose (runtime mechanics & `/* Step N: ... */` *why* rationale in `.c`; caller contracts in `.h` — never verbatim copy-paste).

### 3.3 Block 3 Instrumentation, Fault Logging & Probe Observability

Every Block 3 fault, truncation, boundary saturation, fallback downgrade, and schema rejection path must remain observable without recursion or silent swallowing:

* **Reentrancy-Guarded Logging**: Operational cogs check reentrancy (`s_in_log` / `in_log_emit`) and self-test state (`in_preflight`) before emitting diagnostics via `LOG_*` or module `*_EMIT_LOG` / `log_fallback_record` / `io_fallback_record_fault` / `debug_fallback_record_fault`.
* **Probe & Tripwire Integration**: Fatal contract violations latch via `LOG_SET_TRIPWIRE` / `debug_trigger_probe`, transitioning cleanly into Safe-Mode downgrade and `DEBUG_STEP_AWAIT`, while expected negative paths log without blocking unit assertions (`non_awaiting_expected_negative_rule`).

### 3.3 Canonical TRPG Simulation Rules

* **Deterministic Stance FSM**: `STANCE_NEUTRAL`, `STANCE_WARDING` (*Lux Barrio*: 25% damage absorption, reflect), `STANCE_STRIKING` (*Cuttatio Tenebris*: sweeping arc + 20% flank splash), `STANCE_EQUILIBRIUM` (*Duo-Loca Bashin n Slash*: shield bash + abyssal cleave vs. raw DEF).
* **Conserved Bounded Arithmetic**: Shield charge `[0, 100]`, Sword sharpness `[0, 100]` (`MAX_RESONANCE`), Moral Sway `[-100, +100]`; division uses saturating/safe `math_utils` helpers.
* **Midpoint Campaign Fork & 5-Route Ending Matrix**: Missing the calibrated *Dream Compass* triggers *The Forgotten Stasis*; exposing *The King's Curse* unlocks *Path of Radiance* ($\ge +75$), *Path of the Abyssal Sovereign* ($\le -75$), *The Sovereign Accord* (`[-25, +25]`), *The Cycle Shattered*, or *The Forgotten Stasis*.

### 3.4 Universal Engine Orchestrators

* **Screen Transition Orchestrator (`viewport/reel/`)**: `IDLE -> OUT -> HOLD -> IN -> IDLE`, normalized easing curves (`linear`, `quad_in/out`, `cubic_inout`), wipes/fades/iris dissolves.
* **Audio Event Bus Orchestrator (`audio/`)**: 7-channel hardware mixing bus, ducking state machine, zero-heap circular cue queue (`audio_bus_post_event()`).
* **Clock & Pacing Orchestrator (`core/clock_pacer`)**: Monotonic microsecond delta, fixed-tick accumulator with spiral-of-death clamp, render interpolation alpha.
* **Formula & Affinity Evaluator (`core/formula_eval`)**: Evaluates data-driven curves loaded from `engine/config/runtime/math/`.

---

## BLOCK 4: AUDIT PROCEDURES, VALIDATION PROBES & `AWAIT` TRIPWIRE LOOP

### 4.1 Two-Stage Audit Architecture & Side-Effect Freedom

Every `operational` module implements two cleanly separated verification sub-assemblies in Block 4 (`block_4_audit.json`):

1. **Sub-Assembly 4.1 (`Contract, Input & Cog Verification Cogs`)**: Isolated unit/contract validators (`<subsys>_audit()`, `<subsys>_validate_*()`) verifying null safety, schemas, bounds, and tripwire states.
2. **Sub-Assembly 4.2 (`Machine Self-Audit & Pre-Flight Validation Tripwires`)**: Holistic `<subsys>_preflight(void)` returning `diag_result_t` (`SEVERITY_PASS`..`SEVERITY_FATAL`) for `manifest_run_preflight()`.

All Block 4 routines **MUST be side-effect-free and idempotent** (restore any temporary test mutations, allocate zero heap, never call `abort()`/`exit()`).

### 4.2 Autonomous Validation Probes, `@triggers` & `DEBUG_STEP_AWAIT` Loop

To bridge runtime anomaly detection with Layer 4 automated testing:

1. **Bi-Directional `@triggers` Annotation Contract**:
   * In **Block 3** (source/header sub-assemblies) and **Block 4** (`<subsys>_validate_*` docstrings), document `@triggers` linking:
     * Runtime tripwire / condition -> **Probe ID** (e.g., `"log.setting_profile"`)
     * Associated **Block 4 validator** (`validate_fn`)
     * Corresponding **Layer 4 test file & Ontological Section** (e.g., `tests/unit/log/test_log_unit.c` Sec 2)
2. **Execution Pipeline on Anomaly Detection (`debug_trigger_probe()`)**:

   ```text
   Runtime / Fail-Test Anomaly
        │  fires LOG_SET_TRIPWIRE / debug_trigger_probe(probe_id, ...)
        ▼
   Invoke Registered Block 4 `validate_fn` (recursion-guarded)
        │  pins emergency forensic snapshot (`locked`) + records fault telemetry
        ▼
   Safe-Mode Fallback Downgrade (`*_fallback`)
        │  finishes the interrupted frame/state cleanly with zero crash
        ▼
   Engage `DEBUG_STEP_AWAIT` Latch
        │  blocks `DEBUG_STEP_RUN` / stepping (`debug_is_awaiting() == true`)
        ▼
   State Repair / Rewind (`debug_resolve_await(restored_fields, ...)`)
           re-runs invariants & probe clearance -> resumes cleanly on PASS
   ```

3. **Interactive State Debugger & Rewind**: Up to 64 snapshots in `.bss`, FNV-1a deterministic state hashing (`debug_compute_hash` / snapshot hashes), time-travel `rewind`, and 60-column ASCII inspection cards.

### 4.3 Four-Tier Verification Strata & 4 Ontological Testing Sections

Tests in `tests/` mirror system confidence across 4 CTest strata (`functional/`, `unit/`, `integrated/`, `system/`), and **every test file follows the 4 Ontological Testing Sections**:

1. **SECTION 1: Passing / Nominal Cases** — Nominal paths, baseline invariants, happy paths.
2. **SECTION 2: Failing / Boundary & Error Cases** — NULL pointers, buffer overflows, malformed/missing files, graceful Safe-Mode fallback & probe firing.
3. **SECTION 3: Invariant Assertions & State Proofs** — Clamped math invariants, closed state machines, zero-heap proofs, deterministic FNV-1a hashes.
4. **SECTION 4: Emergence & Stress / Fuzzing Cases** — Multi-turn end-to-end loops, capacity saturation, randomized fuzzing, 1,000-turn softlock immunity.

---

## BLOCK 5: CLOSING PROCEDURES, LIFECYCLE, EXEMPTIONS & GIT WORKFLOWS

### 5.1 Sub-Assembly 5.1 & 5.2 Execution Rules

* **Sub-Assembly 5.1 (`Executable Procedures, Inspection & Diagnostic Cards`)**:
  * Hosts operational execution loops (`gui_run()`), programmatic inspectors (`<subsys>_execute_inspect()`), and 60-column headless diagnostic cards (`<subsys>_execute_dump_card()`).
  * **Dump Card Contract**: Zero heap allocations (`snprintf` into caller buffer), displays pre-flight `PASSED`/`FAILED` status, and adheres to 60-column framing. Existing subsystems return either char count (`size_t` returning `0` on invalid in `io/`/`audio/`, `int` returning `-1` on invalid in `viewport/`) or `bool` pass/fail (`log/`, `debug/`, most `core/`). Keep each subsystem consistent with its facade.
* **Sub-Assembly 5.2 (`Subsystem Lifecycle Initialization & Teardown`)**:
  * **Stateful Modules** (`log`, `debug`, `fs_utils`, `json_utils`, `master_loader`, `path_finder`, `audio_bus`, `gui_assets`, `gui_fonts`, `reel`) implement idempotent `_init()` and clean `_shutdown()`.
  * **Stateless Exemption**: Pure stack-executing modules (and quarantined `*_fallback` BIOS utilities unless managing dedicated Safe-Mode rings) emit the exact literal marker:
    `None. (Stateless module / pure stack-allocated execution).`

### 5.2 Header Archetype Exemptions Reference (`pure_types` & `facade_umbrella`)

Instead of duplicating function declarations or Block 3–5 boilerplate across non-operational headers (`engine/config/standards/blocks/`):

| Archetype | Block 3 | Block 4 (`4.1` / `4.2`) | Block 5 (`5.1` / `5.2`) |
| --- | --- | --- | --- |
| `pure_types` (`log_types.h`, `debug_types.h`, `audio_types.h`, `gui_types.h`) | Statutory note: defines purely static types & inline predicates; APIs live in operational headers. | Statutory note pointing to paired operational headers for `_audit` & `_preflight`. | Statutory note pointing to paired operational headers for dump cards & lifecycle. |
| `facade_umbrella` (`core.h`, `io.h`, `audio.h`, `viewport.h`) | Cross-references all constituent domain headers (zero duplicate prototypes). | Lists constituent Sub-Assembly 4.1 (`_audit`) & 4.2 (`_preflight`) cross-references. | Lists constituent Sub-Assembly 5.1 (procedures/dump cards) & 5.2 (`_init`/`_shutdown`) cross-references. |

### 5.3 Build, Audit, Data Validation & Documentation Commands

```bash
# Configure and compile (AUTO probes C23 -> C17 -> C11 cleanly; Linux default generator)
cmake -S . -B build
cmake --build build -j"$(nproc)"

# Explicit standard ladder & C++20/17 header verification sweeps
cmake -S . -B build -DCORNERSTONE_C_STANDARD=11 -DCORNERSTONE_ENABLE_WERROR=ON
cmake -S . -B build -DCORNERSTONE_C_STANDARD=17 -DCORNERSTONE_ENABLE_WERROR=ON
cmake -S . -B build -DCORNERSTONE_C_STANDARD=AUTO -DCORNERSTONE_VERIFY_CPP_HEADERS=ON -DCORNERSTONE_ENABLE_WERROR=ON

# Clean rebuild zero-warning verification & ELF chassis footprint audit
cmake --build build --clean-first -j"$(nproc)"
size -B build/tenebris-lux && size -A build/tenebris-lux

# Execute full CTest verification suite (100% pass rate required; currently 65+ suites)
ctest --test-dir build --output-on-failure

# Execute full binary diagnostic pre-flight & invariant audit
./build/tenebris-lux --audit

# Validate Rust tooling tests, JSON manifests/configs, and presentation assets
RUSTFLAGS="-D warnings" cargo test --manifest-path tools/Cargo.toml
cargo run --manifest-path tools/Cargo.toml --bin tl_tools -- validate all
cargo run --manifest-path tools/Cargo.toml --bin asset_generator -- --audit

# Compile the 6-volume Master Guide (74 chapters in docs/master_guide_manifest.json)
cargo run --manifest-path tools/Cargo.toml --bin master_guide

# Runtime frontends
./build/tenebris-lux           # Auto-detects X11 display, falls back to CLI
./build/tenebris-lux --cli     # Force terminal CLI mode
./build/tenebris-lux --gui     # Force native X11 GUI mode
./build/tenebris-lux --debug   # Interactive CLI state debugger enabled
```

### 5.4 Canonical 5-Block Git Commit & Tag Protocol (`.githooks/commit-msg`)

Every repository commit is audited automatically by `.githooks/commit-msg` invoking `tools/src/bin/git_validator.rs` (`tools/src/validator/git_commit.rs`). Commits missing required headers or fields are rejected at commit time:

* **Subject Prefix**: Must begin with one of `feat(`, `fix(`, `docs(`, `refactor(`, `test(`, `chore(`, `perf(`, `build(`.
* **Required 5 Commit Blocks & Metadata Fields**:

```text
<type>(<scope>): <concise summary title>

BLOCK 1: METADATA & REVISION RECORD
Project:       Tenebris and Lux
Author:        CreativeWorkzStudio LLC / ProfessorSeanEX
Date:          YYYY-MM-DD
Version:       0.1.1-dev (Rev 5)

BLOCK 2: REASON FOR CHANGE & PROBLEM STATEMENT
<Why this change exists, which seams/gaps/invariants motivated it.>

BLOCK 3: ARCHITECTURAL MODIFICATIONS & ELEVATIONS
<Concrete structural changes, cog elevations, ROMs, fallbacks, probes.>

BLOCK 4: VERIFICATION, TRIPWIRES & PASS PROOF
<Exact post-edit commands and observed results: clean build warnings, CTest X/X, --audit, size.>

BLOCK 5: INVARIANT COVENANTS & CLOSING AFFIRMATIONS
<Confirmation of ALLOC_ZERO_HEAP, C23/17/11 & C++20/17 tier parity, zero // comments, root/ephemeral hygiene.>
```

* **Annotated Release Tags** (must begin with `v`) use:
  * `BLOCK 1: METADATA BLOCK` (requiring `@release` and `@date`)
  * `BLOCK 2: RELEASE CONTEXT & ONTOLOGY`
  * `BLOCK 3: ARCHITECTURAL MODIFICATIONS & ELEVATIONS`
  * `BLOCK 4: AUDIT PROCEDURES & VERIFICATION PROOFS`
  * `BLOCK 5: CLOSING & CANONICAL ARTIFACT LINKS`

### 5.5 AsciiDoc Documentation Standards

All manuals, architecture guides, and realm chronicles in `docs/` are authored in **AsciiDoc (`.adoc`)**:

* Target lines under ~100 characters where practical.
* Escape tildes (`\~` or spell `"approximately"`) to prevent unclosed subscript warnings.
* Prefer relative `xref:` / `link:` cross-references over brittle hardcoded external URLs or volatile counts.
* Use semantic admonitions (`[NOTE]`, `[IMPORTANT]`, `[WARNING]`) and attributed quotes.

### 5.6 Rev 5 Subsystem Audit Checkpoints & Common Gaps to Spot

When modernizing any subsystem stratum (next in sequence: remaining `io/` modules — `json_utils`, `path_finder`, `master_loader` — followed by `core/`, `audio/`, `viewport/`, and `game/`), explicitly check for these recurring Rev 5 failure modes before signing off:

1. **Silent Telemetry Macros & Logger Reentrancy Guards**: Defining `LOCAL_LOG_CAT` / `LOCAL_MODULE_TAG` without real `LOG_INFO`/`LOG_WARN`/`LOG_ERROR` emissions leaves subsystems dark during failures. Whenever filesystem, VFS, or path modules emit telemetry back into `log/`, guard emission paths with `in_log_emit` and `in_preflight` flags so `fs -> LOG -> log_dispatch_category_file -> fs_ensure_directory` never recurses or floods `stderr` during passing self-audits.
2. **Quarantined Fallback Silos & Cross-BIOS Coordination**: Keeping `*_fallback.h` quarantined (`core/manifest.h` only) must **not** isolate `*_fallback.c` from upstream Tier-0 receivers. Fallback `.c` modules should include upstream Tier-0 headers (`"log/log.h"` before `"log/log_fallback.h"`, `"debug/debug.h"` before `"debug/debug_fallback.h"`) to forward faults across BIOS rings, and `log_fallback_boot()` (across both `opt_cfg == NULL` and configured paths) must honor `io_fallback_is_disk_read_only()` cleanly without calling `mkdir`/`fopen`.
3. **Negative-Path Telemetry vs. `DEBUG_STEP_AWAIT` Halts**: Expected negative outcomes (missing optional disk overrides, read-only media write blocks) should record telemetry and fire non-awaiting `DEBUG_REACTION_LOG_ONLY` probes (`await_on_trigger = false`, e.g., `io.fs.readonly_guard`), reserving hard `DEBUG_STEP_AWAIT` latches (`await_on_trigger = true`, e.g., `io.fs.rom_parity`, `io.fallback.rom_integrity`) strictly for genuinely corrupt on-disk files, damaged ROMs, or invariant breaches while `debug_is_active()`.
4. **Header-vs-Source Doc & `@triggers` Parity**: Passing `header_validator` on `.h` files alone is insufficient—audit `.c` Sub-Assembly banners and implementation helpers for crisp Designer/Programmer notes and accurate `@triggers` probe/test cross-references matching `log_fallback.c` and `debug_fallback.c`.
5. **Hardcoded Drift Tables & Unconsumed JSON Fields**: Never hardcode partial mirror tables in C when canonical JSON/ROM already exists (e.g., drifting a hardcoded 15-dir table against 31 domains in `vfs_routes.json`) or parse schema keys into unused struct fields. Hydrate tables/contracts directly from `.rodata` ROM on BIOS boot and live disk JSON during primary init, enforce parsed flags (`posix_dir_mode`, `atomic_write`, `flush_on_write`, `block_path_traversal`, `max_config_bytes`), and verify hydrated counts inside `*_preflight()`.
6. **Stack Scratch Bloat & Duplicated Implementations**: Inspect ELF symbols (`nm --size-sort -S -r build/tenebris-lux`) and `.c` stack frames regularly. Replace multi-KB per-call copies (`buf[8192]`, `meta[1536]`) with single-pass `[start, end)` span scanners, consolidate duplicated POSIX wrappers into parameterized shared helpers, and audit legacy multi-KB `.bss` scratch buffers (e.g., in `json_utils`, `path_finder`, `master_loader`) as each module is modernized.
7. **Upstream Taxonomy, Diagnostics & Brittle ROM Byte Tests**: Adding a log category (`LOG_CAT_IO`), taxonomy error code, tripwire rule (`<= DEBUG_FALLBACK_MAX_TRIPWIRE_RULES`), or ROM preset requires synchronizing:
   * `log.h` (`LOG_CAT_COUNT`), `log.c` (preserving fixed-width 9-char `padded_label` alignment such as `"IO       "` + aliases), and `engine/config/diagnostics/telemetry/*.json`;
   * `engine/config/diagnostics/taxonomy/{error_codes,tripwire_matrix}.json`;
   * Layer 4 ROM tests: avoid hardcoding brittle exact ROM byte totals (`total_rom_bytes == 53649U`) in tests; assert generated ROM macros or lower bounds (`>= 50000U`) alongside exact asset counts so telemetry JSON updates never break unrelated tests;
   * All 5 verification gates after the final edit: clean CMake build, `ctest --output-on-failure`, `./build/tenebris-lux --audit`, `RUSTFLAGS="-D warnings" cargo test --manifest-path tools/Cargo.toml`, and `cargo run --manifest-path tools/Cargo.toml --bin tl_tools -- validate all`.
