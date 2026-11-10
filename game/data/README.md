# Hierarchical Data Architecture & 5-Domain Manifest System

> **Project**: Tenebris and Lux
> **Standard**: OmniCode 5-Block Ontological Architecture & Layered Invariants
> **Author**: CreativeWorkzStudio LLC / ProfessorSeanEX
> **Repository**: [https://github.com/Novaking34/tenebris-lux](https://github.com/Novaking34/tenebris-lux)
> **Milestone**: 0.1.0 (Tactical Strategy RPG Grid Engine & Continental Topology)

---

## 1. Overview & Architectural Rationale

The `game/data/` directory forms the declarative foundation of the engine.
Instead of hardcoding simulation values, invariant limits, tactical schemas,
and presentation catalogs into compiled C11 binaries, all systems, rules,
and balance models are externalized into structured, zero-heap JSON files.

The architecture organizes data into a 5-domain hierarchical tree reflecting
operational data flow ("baton passing") across the simulation pipeline:

```text
engine/config/                  # Standalone Engine Configuration Profiles (Mechanical Strata)
├── manifest.json              # Engine Master Manifest orchestrating runtime, diagnostics, viewport, audio
├── runtime/                   # Bounded math, timing, clocks, string lexicons
├── diagnostics/               # Telemetry logs, error taxonomy, debugger, stream formats
├── viewport/                  # High-DPI font scalers, layout dimensions, asset slots
└── audio/                     # Mixing buses, polyphony limits, sound cue manifests

game/data/                     # Game Domain Content (Layers 1-2 RPG Simulation & Presentation)
├── manifest.json              # Game Domain Master Manifest
├── world/                     # Geography, continental provinces, battlefields, terrains
├── entities/                  # Protagonists, companions, enemy rosters, classes
├── mechanics/                 # Formations, ZOC, combat stances, relics, items, campaign
└── presentation/              # Desktop viewports, UI layouts, palettes, soundscapes
```

---

## 2. The Tri-Vital Model & Hex-Base Attribute Continuum

Defined in [`engine/config/runtime/math/attributes.json`](file:///home/professorseanex/Tenebris%20and%20Lux/engine/config/runtime/math/attributes.json) and enforced across Layer 1 simulation:

### 2.1 The Tri-Vitals (`HP`, `MP`, `TP`)

* **HP (Health Points)**: Vital physical/spiritual integrity reservoir `[0, 9999]`. Depletion results in incapacitation.
* **MP (Magic Points)**: Ethereal and planar resonance pool `[0, 999]`. Expended to channel relics, spells, and arts.
* **TP (Tactical Points)**: Strategic overworld and grid action currency `[0, 100]`.
  * **Overworld Grid Expenditure**: Spent to traverse grid cells (1 TP * terrain friction), sprint (2 TP/cell), or brace defenses.
  * **Overworld-to-Battle Impact**: Actions performed in the tactical overworld directly determine initial conditions in the action-battle / side-scroll turn encounter:
    * *Tactical Ambush* (15 TP): Grants first-strike initiative, +20% ATK, and 50% initial foe poise stagger.
    * *Tactical Scouting* (10 TP): Grants Fortune's Blessing (+30% LUK, +20% crit chance).
    * *Tactical Pinning / ZOC*: Depriving foes of TP halts traversal and inflicts -20% Bastion DEF.
    * *Tactical Exhaustion*: Depleting TP below 10 doubles grid traversal cost and increases MP consumption by 20%.

### 2.2 The Hex-Base Attributes

| Base Stat | Lore-Accurate Canonical Name | Metaphysical Domain | Mechanical Influence |
| :--- | :--- | :--- | :--- |
| **ATK** | **Might** (*Vigor Arcana*) | Physical Martial Force | Melee damage, armor penetration, physical stagger |
| **DEF** | **Bastion Poise** (*Lux Ward*) | Radiant Physical Deflection | Kinetic damage reduction, block threshold, stagger recovery |
| **MATK** | **Resonance** (*Harmonic Surge*) | Planar Channeling Aperture | Spell damage, relic art amplification, elemental splash |
| **MDEF** | **Astral Warding** (*Aether Shell*) | Cosmic Aversion Poise | Magical damage mitigation, status resistance, warding persistence |
| **AGI** | **Celerity** (*Shadow Cadence*) | Swiftness & Action Flow | Turn order queue, side-scroll action gauge refill, evasion |
| **LUK** | **Fortune** (*Sovereign Grace*) | Cosmic Probability Alignment | Critical hit probability, status infliction, rare item loot drop |

---

## 3. OmniCode Universal 5-Block Lifecycle Protocol in Manifests

Every manifest across all 5 domains strictly implements the 5-Block architecture at version `0.1.0`:

1. **`block_1_metadata`**: Identity, provenance, version (`0.1.0`), interface revision, where the data lives, what it enables, and axiomatic invariants.
2. **`block_2_opening`**: Pre-conditions, static declarations (`subsystem`, `schema_id`, counts, capacities), and generic helper contracts.
3. **`block_3_body`**: Core domain catalogs, operational datasets, subsystem arrays, and functional parameters.
4. **`block_4_audit`**: Verification tripwires, integrity assertions, diagnostic bounds, and zero-heap compliance metrics.
5. **`block_5_closing`**: Hierarchical navigation links (`parent_menu`, `root_manifest`), sub-assembly menus, and execution hooks.

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

## 4. The Layered Domains

### Engine Configuration (`engine/config/`)

* **`runtime/math/`**: Resonance clamping `[0, 100]`, moral sway `[-100, +100]`.
* **`runtime/time/`**: 60/120 FPS budgets, monotonic clock, sub-50µs budgets.
* **`runtime/string/`**: Command tokens, stance lexicon, buffer boundaries.
* **`diagnostics/telemetry/`**: Categorized telemetry, static ring buffers.
* **`diagnostics/debugger/`**: Snapshot ring buffer, FNV-1a hashing, rewind.
* **`viewport/`**: Resolution letterbox presets, typography scalers, generic asset slots.
* **`audio/`**: Multi-channel mixing buses, polyphony caps, ducking matrix.

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
