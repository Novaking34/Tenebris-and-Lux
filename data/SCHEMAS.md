<!--
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          data/SCHEMAS.md
@layer         Layer 1 / Layer 0 Integration (Data-Driven Architecture)
@version       0.1.0-dev
@interface_rev Rev 1
@date          2026-11-03
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Tactical RPG Data Schemas Specification & Roadmap Note
@role          Architectural Design Specification for Tactical Grid Data Schemas
@invariants    1. Zero dynamic heap allocations in parsing runtime.
               2. Static schema bounds: Max 64x64 grid cells, max 8 formation slots.
               3. Strict JSON conformance parsed via Layer 0 json_utils tokenizer.
               4. Bounded integer coordinates and non-negative movement costs.
               5. Dual relic & stance affinity modifiers strictly normalized.

PURPOSE & ARCHITECTURAL INTENT:
  Establishes the architectural design blueprint and conceptual contract for
  upcoming tactical data schemas in Tenebris and Lux. Formalizes the data
  models for discrete 2D grid topologies, Movement Points (MP) budgeting,
  Zone-of-Control (ZOC) physics, terrain friction modifiers, and tactical
  party formation matrices prior to full C11 parser implementation.
================================================================================
-->

# Tactical Strategy RPG Data Schemas Specification

> **Project**: Tenebris and Lux  
> **Milestone**: `0.1.0-dev`  
> **Standard**: OmniCode 5-Block Ontological Architecture & Layered Invariant Discipline  
> **Author**: CreativeWorkzStudio LLC / ProfessorSeanEX  
> **Repository**: [https://github.com/Novaking34/tenebris-lux](https://github.com/Novaking34/tenebris-lux)  
> **Subsystem**: Data-Driven Tactical Simulation Contracts (`data/`)

---

## 1. Executive Summary & Design Vision

As *Tenebris and Lux* advances from a turn-based stance combat loop into a deep **Tactical Strategy RPG Grid Engine** across continental Sacyra, the underlying simulation requires formal, externalized data schemas.

Rather than hardcoding grid dimensions, terrain penalties, formation offsets, or unit mobility into C11 source files, the **Data-Driven Architecture** externalizes these mechanics into validated, zero-heap-parseable JSON manifests. This architectural note serves as the design contract and roadmap for upcoming tactical schemas.

```text
                  ┌─────────────────────────────────────────┐
                  │          data/manifest.json             │
                  │        (Root Subsystem Catalog)         │
                  └────────────────────┬────────────────────┘
                                       │
        ┌──────────────────────────────┼──────────────────────────────┐
        ▼                              ▼                              ▼
┌───────────────┐              ┌───────────────┐              ┌───────────────┐
│  data/world/  │              │ data/tactical/│              │ data/classes/ │
│ Map Topologies│              │ Formations &  │              │ Movement (MP) │
│ Terrain Grids │              │ Zone-of-Ctrl  │              │ Archetypes    │
└───────────────┘              └───────────────┘              └───────────────┘
```

---

## BLOCK 2: DOMAIN ONTOLOGY & TYPE SPECIFICATION

> **Cognitive Role in the Mind: Framing & Conceptual Invariants**  
> Formulates the grammatical contracts, memory bounds, and data primitives governing tactical battlefield interactions.

### 2.1 Coordinate Space & Discrete 2D Grids

* **Coordinate System**: Discrete Cartesian integer coordinates `(x, y)` bounded within `[0, MAX_GRID_X)` and `[0, MAX_GRID_Y)`.
* **Standard Grid Scale**: Baseline maps default to 32x32 or 64x64 cells.
* **Metric Distance**: Manhattan Distance ($|x_1 - x_2| + |y_1 - y_2|$) for strict cardinal movement; Chebyshev Distance ($\max(|x_1 - x_2|, |y_1 - y_2|$) for 8-directional area-of-effect (AoE) calculations.

### 2.2 Movement Point (MP) Budgeting

Each actor possesses a turn-based mobility budget determined by class, relic stance, and active encumbrance:

* **Base Movement Points ($MP_{base}$)**: Allocated at the dawn of each tactical turn (e.g., Scout = 6 MP, Warrior = 4 MP, Guardian = 3 MP).
* **Step Cost ($Cost_{step}$)**: Consumed per grid cell traversed:
  $$Cost_{step} = Cost_{terrain} \times Multiplier_{stance} + Penalty_{hazard}$$
* **Saturating Bounds**: Remaining MP is clamped within `[0, MAX_MP]`.

### 2.3 Zone-of-Control (ZOC) Physics

Zone-of-Control represents the immediate kinetic threat perimeter maintained by armed combatants:

* **ZOC Radius**: Standard melee units exert a 1-tile orthogonal Zone-of-Control. Polearm or Greatsword combatants exert a 2-tile frontal cone.
* **Movement Interception**:
  1. Entering an enemy ZOC consumes +1 additional MP.
  2. Attempting to traverse directly between two contiguous enemy ZOC tiles halts movement immediately ("Pinning").
  3. Disengaging without a defensive stance (e.g. *Lux Barrio*) triggers an automatic Opportunity Reaction Strike.

### 2.4 Terrain Properties & Tactical Modifiers

Every grid cell references a canonical terrain profile defining:

* **Passability**: Traversable, blocked (walls, deep chasm), or conditional (requires flight or levitation catalyst).
* **Cost Multiplier**: Movement friction (Plains = 1.0, Forest = 1.5, Mud/Swamp = 2.0, Mountain Path = 2.5).
* **Defensive Cover**: Direct damage mitigation percentage (Plains = 0%, Bush = +10%, Fortification/Rampart = +30%).
* **Evasion Bonus**: Percentage bonus to stance evasion or deflection (High Ground = +15%).
* **Relic Resonance Multipliers**: Planar rift nodes amplifying Lux (+20% barrier potency) or Tenebris (+20% edge sharpness).

### 2.5 Party Formation Positioning Matrices

Tactical deployment utilizes an 8-slot relative positioning grid representing tactical combat squads:

```text
       [ FRONT LINE / VANGUARD ]
  Slot 0: Left Flank    Slot 1: Center Front    Slot 2: Right Flank
  
       [ MID LINE / INTERCEPT ]
  Slot 3: Shield Guard  Slot 4: Tactician / Bearer  Slot 5: Support
  
       [ REAR LINE / RESERVES ]
  Slot 6: Rear Artillery       Slot 7: Rear Arcanist
```

* **Vanguard Perks**: +10% Stance Strike damage, takes brunt of frontline ZOC threat.
* **Rearguard Protection**: Cannot be targeted by direct single-target melee strikes while Vanguard slots are occupied.

---

## BLOCK 3: SCHEMA DEFINITIONS & JSON CONTRACTS

> **Cognitive Role in the Mind: Concrete Data Layouts**  
> Defines the JSON structure adhering to the OmniCode 5-Block schema format for zero-heap parsing in Layer 0.

### 3.1 Tactical Terrain Schema Contract (`data/world/terrains/default.json`)

```json
{
  "_metadata": {
    "schema_version": "1.0.0",
    "domain": "tactical.terrain",
    "description": "Standard continental terrain modifiers and friction matrix",
    "invariants": ["Zero-heap parsing", "Friction >= 1.0", "Cover [0, 50]"]
  },
  "_types": {
    "target_struct": "terrain_profile_t",
    "terrain_id_enum": ["PLAINS", "FOREST", "SWAMP", "RAMPART", "CHASM", "RIFT_NODE"]
  },
  "setting": {
    "terrains": [
      {
        "id": "PLAINS",
        "name": "Sacyran Grassland",
        "passable": true,
        "movement_cost": 1.0,
        "defense_cover_pct": 0,
        "evasion_bonus_pct": 0,
        "lux_resonance_mult": 1.0,
        "tenebris_resonance_mult": 1.0
      },
      {
        "id": "FOREST",
        "name": "Weeping Canopy",
        "passable": true,
        "movement_cost": 1.5,
        "defense_cover_pct": 15,
        "evasion_bonus_pct": 10,
        "lux_resonance_mult": 0.9,
        "tenebris_resonance_mult": 1.1
      },
      {
        "id": "RAMPART",
        "name": "Casatti Stone Bastion",
        "passable": true,
        "movement_cost": 1.0,
        "defense_cover_pct": 30,
        "evasion_bonus_pct": 5,
        "lux_resonance_mult": 1.25,
        "tenebris_resonance_mult": 0.8
      },
      {
        "id": "CHASM",
        "name": "Planar Fissure",
        "passable": false,
        "movement_cost": 99.0,
        "defense_cover_pct": 0,
        "evasion_bonus_pct": 0,
        "lux_resonance_mult": 0.0,
        "tenebris_resonance_mult": 2.0
      }
    ]
  },
  "_audit": {
    "max_terrains": 32,
    "static_buffer_bytes": 4096
  },
  "_navigation": {
    "parent_menu": "data/manifest.json",
    "next_subsystem": "data/tactical/formations/manifest.json"
  }
}
```

### 3.2 Party Formation Schema Contract (`data/tactical/formations/default.json`)

```json
{
  "_metadata": {
    "schema_version": "1.0.0",
    "domain": "tactical.formations",
    "description": "Standard 8-slot tactical squad combat formation matrix",
    "invariants": ["Max 8 slots", "Unique slot IDs", "Normalized stat modifiers"]
  },
  "_types": {
    "target_struct": "formation_matrix_t",
    "line_category_enum": ["VANGUARD", "MIDLINE", "REARGUARD"]
  },
  "setting": {
    "formation_id": "BALANCED_VANGUARD",
    "name": "The Casatti Bastion Array",
    "description": "Founding formation balancing frontline poise with protected relic wielders.",
    "slots": [
      {
        "slot_id": 0,
        "name": "Left Vanguard",
        "line": "VANGUARD",
        "rel_x": -1,
        "rel_y": 1,
        "stat_modifiers": { "atk_pct": 5, "def_pct": 10, "mp_bonus": 0 }
      },
      {
        "slot_id": 1,
        "name": "Point Vanguard",
        "line": "VANGUARD",
        "rel_x": 0,
        "rel_y": 1,
        "stat_modifiers": { "atk_pct": 10, "def_pct": 15, "mp_bonus": 0 }
      },
      {
        "slot_id": 2,
        "name": "Right Vanguard",
        "line": "VANGUARD",
        "rel_x": 1,
        "rel_y": 1,
        "stat_modifiers": { "atk_pct": 5, "def_pct": 10, "mp_bonus": 0 }
      },
      {
        "slot_id": 3,
        "name": "Bearer Focal Slot",
        "line": "MIDLINE",
        "rel_x": 0,
        "rel_y": 0,
        "stat_modifiers": { "atk_pct": 0, "def_pct": 5, "mp_bonus": 1 }
      },
      {
        "slot_id": 4,
        "name": "Rearguard Strategist",
        "line": "REARGUARD",
        "rel_x": 0,
        "rel_y": -1,
        "stat_modifiers": { "atk_pct": -5, "def_pct": -5, "mp_bonus": 2 }
      }
    ]
  },
  "_audit": {
    "max_slots": 8,
    "static_buffer_bytes": 2048
  },
  "_navigation": {
    "parent_menu": "data/manifest.json",
    "next_subsystem": "data/world/terrains/manifest.json"
  }
}
```

---

## BLOCK 4: AUDIT PROCEDURES & VALIDATION ASSERTIONS

> **Cognitive Role in the Mind: Defensive Tripwires & Invariant Guarantees**  
> Specifies the pre-flight invariants that the C11 data loader (`game/src/sim/`) must verify upon ingesting schemas.

1. **Zero-Heap Invariant**: All tactical grid, terrain, and formation buffers must load into static or stack-allocated C structures (`ALLOC_ZERO_HEAP`). No dynamic allocation (`malloc`) is permitted.
2. **Coordinate Boundedness**: Every unit placement and relative formation offset must satisfy:
   $$0 \le x < MAX\_GRID\_X \quad \text{and} \quad 0 \le y < MAX\_GRID\_Y$$
3. **Terrain Movement Friction**: Movement friction must never be zero or negative ($Cost \ge 1.0$), eliminating zero-cost infinite loops.
4. **ZOC Collision Independence**: Two hostile units cannot occupy the identical `(x, y)` tile simultaneously under any circumstance.
5. **Relic Multiplier Normalization**: Relic resonance multipliers must remain strictly within $[0.0, 2.0]$.

---

## BLOCK 5: ROADMAP & IMPLEMENTATION HOOKS

> **Cognitive Role in the Mind: Engineering Execution & Milestone Linkage**  
> Tracks integration steps as the engine moves through the `v0.1.0-dev` lifecycle.

* [x] **Architecture Blueprint**: Establish formal data schema specification (`data/SCHEMAS.md`).
* [ ] **Domain Directory Scaffolding**: Provision `data/tactical/formations/` and `data/world/terrains/` menus.
* [ ] **C11 Struct Definitions**: Define `tactical_terrain_t` and `formation_matrix_t` in `game/include/sim/world/` and `game/include/sim/combat/`.
* [ ] **Zero-Heap Tokenizer Integration**: Wire `json_utils` into `world_loader.c` and `combat_loader.c` to parse tactical schemas.
* [ ] **Tripwire Audit Routine**: Register `audit_tactical_schemas()` in `engine/include/util/manifest.h` and verify under `./build/tenebris-lux --audit`.
