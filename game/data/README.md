# Hierarchical Data Architecture & 5-Domain Manifest System

> **Project**: Tenebris and Lux  
> **Standard**: OmniCode 5-Block Ontological Architecture & Layered Invariants  
> **Author**: CreativeWorkzStudio LLC / ProfessorSeanEX  
> **Repository**: [https://github.com/Novaking34/tenebris-lux](https://github.com/Novaking34/tenebris-lux)  
> **Milestone**: 0.1.0-dev (Tactical Strategy RPG Grid Engine & Continental Topology)

---

## 1. Overview & Architectural Rationale

The `game/data/` directory forms the declarative foundation of the engine.
Instead of hardcoding simulation values, invariant limits, tactical schemas,
and presentation catalogs into compiled C11 binaries, all systems, rules,
and balance models are externalized into structured, zero-heap JSON files.

The architecture abandons flat folder sprawl in favor of a 5-domain
hierarchical tree reflecting operational data flow ("baton passing"):

```text
game/data/
├── manifest.json              # Master Catalog linking the 5 Domain Manifests
│
├── system/                    # 1. System Domain: Platform & Core Invariants
│   ├── manifest.json          # System Master Menu
│   ├── runtime/               # Engine utilities (math, time, string)
│   └── diagnostics/           # Telemetry logs and interactive debugger
│
├── world/                     # 2. World Domain: Geography & Battlefields
│   ├── manifest.json          # World Master Menu
│   ├── geography/             # Overworld realm topology & provinces
│   ├── battlefields/          # Tactical maps & grid descriptors
│   └── terrains/              # Surface friction, cover & rift nodes
│
├── entities/                  # 3. Entities Domain: Actors & Growth
│   ├── manifest.json          # Entities Master Menu
│   ├── roster/                # Protagonists, companions & bestiary
│   └── classes/               # Tiered class hierarchy & elemental summons
│
├── mechanics/                 # 4. Mechanics Domain: Simulation Rules & Flow
│   ├── manifest.json          # Mechanics Master Menu
│   ├── tactics/               # 8-slot squad formations & Zone of Control
│   ├── combat/                # Stances, damage formulas, skills & states
│   ├── items/                 # Twin Relics, catalysts, consumables
│   └── campaign/              # Morality matrix, progression & dispatcher
│
└── presentation/              # 5. Presentation Domain: Sensory Viewports
    ├── manifest.json          # Presentation Master Menu
    ├── ui/                    # HUD layout, panels, font typography & scalers
    ├── audio/                 # Channels, SFX cues, BGM themes
    └── visuals/               # Spritesheets, tilesets, biome & relic palettes
```

---

## 2. Dynamic Data Pathfinding (`fs_resolve_data_path`)

All engine loaders in Layer 0 and Layer 1 resolve data files dynamically via
[`fs_resolve_data_path()`](file:///home/professorseanex/Tenebris%20and%20Lux/engine/include/io/fs_utils.h#L233-L261).
Loaders never rely on rigid static path literals.

The dynamic resolver executes a 5-stage probing pipeline:
1. **Direct Path Check**: Tests for immediate existence on disk.
2. **Prefix Normalization**: Strips redundant `game/`, `../`, `data/` tokens.
3. **Legacy Translation Table**: Transparently remaps legacy subpaths
   (e.g., `data/items/` to `mechanics/items/`).
4. **Base Root Probing**: Evaluates stripped paths across candidate workspace
   roots (`game/data/`, `../game/data/`, `data/`, `../data/`).
5. **Domain Subtree Probing**: Searches across 36 candidate domain and
   sub-assembly prefixes for bare filenames or relative subpaths.

This ensures zero test breakage whether executables are run from the
repository root, `build/`, or external CI sandboxes, with zero dynamic heap
allocation (`ALLOC_ZERO_HEAP`).

---

## 3. OmniCode 5-Block Lifecycle Protocol in JSON

Every JSON file in the data hierarchy mirrors the repository's 5-Block
Lifecycle Protocol:

1. **`_metadata` (Block 1)**: Identity, provenance, version, and invariants.
2. **`_types` (Block 2)**: Schema bindings, enum sets, target C struct models.
3. **`menu` / `setting` / payload (Block 3)**: Core operational parameters.
4. **`_audit` (Block 4)**: Diagnostic bounds, memory limits (`ALLOC_ZERO_HEAP`).
5. **`_navigation` (Block 5)**: Hierarchical parent menu links and fallbacks.

---

## 4. The 5 Operational Domains

### Domain 1: System (`game/data/system/`)
* **`runtime/math/`**: Resonance clamping `[0, 100]`, moral sway `[-100, +100]`.
* **`runtime/time/`**: 60/120 FPS budgets, monotonic clock, sub-50µs budgets.
* **`runtime/string/`**: Command tokens, stance lexicon, buffer boundaries.
* **`diagnostics/telemetry/`**: Categorized telemetry, static ring buffers.
* **`diagnostics/debugger/`**: Snapshot ring buffer, FNV-1a hashing, rewind.

### Domain 2: World (`game/data/world/`)
* **`geography/`**: Sacyran provinces, node graphs, travel vigor costs.
* **`battlefields/`**: 2D Cartesian tactical grids, deployment zones.
* **`terrains/`**: Friction rates, defensive cover bonuses, line of sight.

### Domain 3: Entities (`game/data/entities/`)
* **`roster/`**: Elijah Ro-enheart, Lady Katrina Jaren, Custom Bearer, bestiary.
* **`classes/`**: Basic, upgraded, advanced classes, NPC guards, summons.

### Domain 4: Mechanics (`game/data/mechanics/`)
* **`tactics/`**: 8-slot squad formations, pinning Zone of Control (ZOC).
* **`combat/`**: Stances (*Lux Barrio*, *Cuttatio Tenebris*, *Duo-Loca*), skills.
* **`items/`**: Shield of Lux, Sword of Tenebris, Grandma's Watch, catalysts.
* **`campaign/`**: 5-route ending matrix, morality inertia, dispatch routing.

### Domain 5: Presentation (`game/data/presentation/`)
* **`ui/`**: 2.5D isometric diamond layout, HUD gauges, TrueType typography.
* **`audio/`**: 4-channel audio bus mixer, diegetic SFX cues, orchestral BGM.
* **`visuals/`**: 32x32 sprite sheets, terrain tilesets, 24/32-bit hex palettes.
