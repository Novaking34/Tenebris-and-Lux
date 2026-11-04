# Data-Driven Architecture & Manifest System

> **Project**: Tenebris and Lux  
> **Standard**: OmniCode 5-Block Ontological Architecture & Layered Invariant Discipline  
> **Author**: CreativeWorkzStudio LLC / ProfessorSeanEX  
> **Repository**: [https://github.com/Novaking34/tenebris-lux](https://github.com/Novaking34/tenebris-lux)

---

## 1. Overview & Purpose

The `data/` directory forms the foundation of the **Data-Driven Architecture** for *Tenebris and Lux*. Instead of hardcoding simulation values, telemetry thresholds, combat mechanics, and entity definitions directly within compiled source code, all configurations, game systems, and parameter menus are externalized into structured, validated JSON data files.

```text
data/
├── manifest.json              # Root Data Catalog (Master Menu)
└── log/                       # Logging Subsystem Domain
    ├── manifest.json          # Subsystem "Main Menu" (Settings Index)
    ├── default.json           # Setting 1: Baseline C11 Engine Default
    ├── development.json       # Setting 2: Interactive Verbose Debugging
    ├── production.json        # Setting 3: Shipping Release Profile
    ├── forensic_telemetry.json # Setting 4: Full Flight Recorder Audit
    ├── quiet.json             # Setting 5: Headless CI & Benchmark Profile
    └── combat_focus.json      # Setting 6: Tactical Combat & Stance Focus
```

---

## 2. The "Manifest as Main Menu" & "Files as Settings" Pattern

This subsystem adopts the **Menu-to-Settings Ontological Pattern**:

1. **Manifest File (`data/log/manifest.json`) = The "Main Menu"**:
   - Acts as an interactive directory and dispatch menu.
   - Declares the active default profile, metadata invariants, hotkey bindings, and human-readable descriptions for each option.
   - Enforces pre-flight validation rules (e.g. valid severity levels, category bindings, rotation limits).

2. **Data Files (`*.json`) = The "Settings"**:
   - Each individual file represents a discrete, self-contained configuration profile ("Setting").
   - Directly maps to the Layer 0 `log_config_t` structure defined in [`engine/include/log/log.h`](file:///home/professorseanex/Tenebris%20and%20Lux/engine/include/log/log.h).
   - Allows runtime hot-swapping between developer trace mode, high-speed automated CI benchmarking (< 50µs tick), and production distribution without recompilation.

---

## 3. OmniCode 5-Block Structure in JSON Data Files

Every JSON file in the data hierarchy strictly mirrors the repository's 5-Block Lifecycle Protocol:

1. **`_metadata` (Block 1)**: Provenance, file identity, semantic versioning, and axiomatic invariants.
2. **`_types` (Block 2)**: Schema bindings, enum sets, and target C struct references (`log_config_t`).
3. **`menu` / `setting` (Block 3)**: Core operational payload (menu items or concrete configuration values).
4. **`_audit` (Block 4)**: Diagnostic thresholds, memory allocations (`ALLOC_ZERO_HEAP`), and capacity bounds.
5. **`_navigation` (Block 5)**: Hierarchical parent menu links and fallback targets.

---

## 4. Subsystem Profiles Matrix

| Setting ID | File | Min Severity | Console Echo | Category Segregation | Max Archive Size | Purpose |
| :--- | :--- | :---: | :---: | :---: | :---: | :--- |
| `default` | [`default.json`](file:///home/professorseanex/Tenebris%20and%20Lux/data/log/default.json) | `TRACE` | `true` | `true` | 1 MB | Baseline C engine defaults |
| `development` | [`development.json`](file:///home/professorseanex/Tenebris%20and%20Lux/data/log/development.json) | `TRACE` | `true` (ANSI) | `true` | 2 MB | Local feature development & turn trace |
| `production` | [`production.json`](file:///home/professorseanex/Tenebris%20and%20Lux/data/log/production.json) | `INFO` | `false` | `true` | 5 MB | Clean end-user release distribution |
| `forensic_telemetry` | [`forensic_telemetry.json`](file:///home/professorseanex/Tenebris%20and%20Lux/data/log/forensic_telemetry.json) | `TRACE` | `true` | `true` | 10 MB | Invariant breach & monotonic clock audit |
| `quiet` | [`quiet.json`](file:///home/professorseanex/Tenebris%20and%20Lux/data/log/quiet.json) | `WARNING` | `false` | `false` | 1 MB | Headless CTest & benchmark (< 50µs target) |
| `combat_focus` | [`combat_focus.json`](file:///home/professorseanex/Tenebris%20and%20Lux/data/log/combat_focus.json) | `DEBUG` | `true` | `true` | 2 MB | Relic stance & action formula balancing |

---

## 5. Active Subsystem Catalogs

In addition to logging, the data-driven framework encompasses the following active subsystems:

1. **`data/math/`**: Resonance limits (0 to 100), moral sway continuum bounds (-100 to +100), equilibrium bands (+/- 25), and HUD scaling.
2. **`data/time/`**: Monotonic clock frame pacing, 60/120 FPS targets, and sub-50µs simulation turn budgets.
3. **`data/string/`**: Buffer limits, command tokens, and canonical action verb dictionaries.
4. **`data/debug/`**: Interactive debugger profiles, snapshot ring buffer capacities, breakpoint watchpoints, and tripwire assertion traps:
   - `default.json`: Balanced developer debugging baseline with 64-snapshot rewind ring buffer.
   - `strict.json`: Halts immediately on any state breach, stance shift, or diagnostic warning.
   - `trace.json`: Deep micro-step cog-level tracing across all engine channels.

---

## 6. Tactical Strategy RPG Schemas Roadmap

For milestone `v0.1.0-dev`, the tactical strategy RPG grid engine design specifications are documented in:

- **[`game/data/SCHEMAS.md`](file:///home/professorseanex/Tenebris%20and%20Lux/game/data/SCHEMAS.md)**: Comprehensive design contracts for discrete 2D grid topologies, Movement Points (MP) budgeting, Zone-of-Control (ZOC) physics, terrain friction modifiers, and 8-slot party formation positioning matrices.
