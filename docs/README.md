<!--
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Audience & Master Knowledge Portal Anchors )
================================================================================
@project       Tenebris and Lux
@file          docs/README.md
@id            docs.root.index.md
@layer         Documentation / Master Knowledge Portal
@version       0.1.2-dev
@interface_rev Rev 5
@date          2026-11-14
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Cornerstone Living Documentation & Book Publication Hub
@role          Repository Knowledge Map & Multi-Audience Portal
@audience      Everyone: Designers, Writers, Artists, Testers, Students & C/Rust Engineers
@invariants    1. Universal 5-Block Literary Architecture.
               2. Mirrors docs/README.adoc taxonomy and cross-volume navigation.
================================================================================
-->

# Documentation Architecture & Living Repository Index

> **Project**: Tenebris and Lux (Flagship RPG) | **Engine**: Cornerstone Game Engine  
> **Repository**: [https://github.com/Novaking34/tenebris-lux](https://github.com/Novaking34/tenebris-lux)  
> **Milestone**: `v0.1.2-dev` | **Governance**: `Rev 5`

---

<!--
================================================================================
BLOCK 2: OPENING BLOCK
( Role-Based Entry Points & Quick Orientation )
================================================================================
-->

Welcome to the living knowledge portal of **Tenebris and Lux** (`game/`) and the **Cornerstone Game Engine** (`engine/`).

Every technical and narrative document follows our **Dual-Audience Discipline**:
* **For Designers, Writers & Scripters**: Plain-English mental models, high-concept lore, world geography, and gameplay balance parameters.
* **For Systems Programmers & Architects**: Concrete memory layouts, zero-heap contracts (`ALLOC_ZERO_HEAP`), Big-O complexity, and live code snapshot plates.

> [!TIP]
> **New to the repository?**
> * Read the [Installation & Build Guide](INSTALL.adoc) to compile and boot the game.
> * Read the [System Architecture Overview](architecture/ARCHITECTURE.adoc) for an overview of the Cornerstone Game Engine.
> * Explore the [Master Guide Compendium](books/README.adoc) in `docs/books/`.

---

<!--
================================================================================
BLOCK 3: BODY BLOCK
( Directory Taxonomy, Publishing Pipeline & Volume Map )
================================================================================
-->

## 1. Documentation Strata & Directory Taxonomy

All project knowledge is organized into clear role-based directories:

| Directory | Target Domain | Description |
| :--- | :--- | :--- |
| [`docs/INSTALL.adoc`](INSTALL.adoc) | Setup & Tooling | Prerequisites, CMake configuration presets, Rust CLI suite, and verification commands. |
| [`docs/lore/`](lore/) | Volumes I & II | Canonical story bibles, character dossiers, continental geography (Sacyra, Libira), and theology. |
| [`docs/game_design/`](game_design/) | Volume III | Tactical TRPG combat, 35 classes, 28 jobs, relic stances, moral sway, and progression specs. |
| [`docs/dev_logs/`](dev_logs/) | Volume VI | Chronological engineering logs (001–020) across October and November 2026. |
| [`docs/architecture/`](architecture/) | Volume V (Foundations) | Technical architecture of Cornerstone: 5 Compilation Strata, Safe-Mode BIOS, probes, and VFS. |
| [`docs/code/`](code/) | Volume V (Implementation) | Subsystem-by-subsystem C implementation tours (`core`, `log`, `debug`, `io`, `audio`, `viewport`). |
| [`docs/snapshots/`](snapshots/) | Volume V Appendix A | 26 live code snapshot plates extracted verbatim from source files across 5 granularities. |
| [`docs/books/`](books/) | Publication Vault | Unified compendiums ([`MASTER_GUIDE.adoc`](books/MASTER_GUIDE.adoc), [`.md`](books/MASTER_GUIDE.md), [`.pdf`](books/MASTER_GUIDE.pdf)) and future book deliverables. |
| [`docs/volumes/`](volumes/) | Anthology Outputs | Standalone six-volume books (`VOLUME_I` through `VOLUME_VI`) in `.adoc`, `.md`, and `.pdf`. |
| [`docs/offline_work/`](offline_work/) | Living Mobile Scratch | Mobile app design manuscripts (`Offline-Work-1..6`) used for ongoing collaborative world mapping. |
| [`docs/archive/`](archive/) | Archival Research Vault | Historical model floor screenings (`floor_screening/`), tok/s benchmarks, and research logs. |

---

## 2. Data-Driven Publishing Pipeline

Rather than hardcoding layout rules in Rust, our publishing toolchain (`tools/src/bin/master_guide.rs`) is driven by structured 5-block JSON manifests:

* **`master_guide_manifest.json`**: Master book manifest defining volume titles, directory scan order, and excluded directories.
* **`guide_theme_config.json`**: Data-driven PDF book theme compiling into `docs/themes/cornerstone-book-theme.yml`.
* **`code_snapshots_manifest.json`**: Declarative code snapshot catalog supporting `whole`, `block`, `subassembly`, `function`, and `group` extraction modes.
* **`adoc_pdf_layout_rules.json`**: Geometry rules guarding single-page budgets, code column widths, and delimiter balance.
* **`adoc_5block_standard.json`**: The canonical specification for formatting AsciiDoc technical manuscripts under the 5-block standard.

---

<!--
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Validation Commands & Integrity Gates )
================================================================================
-->

## 3. Verification & Linting Gates

Documentation integrity is continuously audited by native developer tools:

```bash
# Scan and verify all 114 chapters registered in master_guide_manifest.json
cargo run --manifest-path tools/Cargo.toml --bin master_guide -- --scan

# Audit single-page PDF printable budgets and AsciiDoc delimiter hygiene
cargo run --manifest-path tools/Cargo.toml --bin master_guide -- --validate-layout

# Validate JSON manifest 5-block structures and C header standards
cargo run --manifest-path tools/Cargo.toml --bin tl_tools -- validate all
```

---

<!--
================================================================================
BLOCK 5: CLOSING BLOCK
( Navigation & Cross-Volume Links )
================================================================================
-->

## 4. Navigation Links

* **Root Repository Index**: [README.md](../README.md)
* **Master Guide Publication Vault**: [docs/books/README.adoc](books/README.adoc)
* **Historical Research Archive**: [docs/archive/README.adoc](archive/README.adoc)
* **Mobile Design Manuscripts**: [docs/offline_work/README.adoc](offline_work/README.adoc)
