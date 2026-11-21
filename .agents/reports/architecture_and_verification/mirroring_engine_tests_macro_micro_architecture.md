<!--
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Verification Matrix & Master Dissertation Seams )
================================================================================
@project       Tenebris and Lux -> OmniCode Track 7 Verification Baseline
@file          .agents/reports/architecture_and_verification/
               mirroring_engine_tests_macro_micro_architecture.md
@id            cog.agent.reports.test_mirroring_macro_micro
@layer         Portable Agent Substrate - Verification Architecture Evidence
@archetype     architecture_and_verification_report | test_topology
@version       0.1.3-dev
@interface_rev Rev 5
@created       2026-11-17
@updated       2026-11-20
@author        CreativeWorkzStudio LLC / ProfessorSeanEX (Seanje Lenox-Wise)
@repository    https://github.com/Novaking34/OmniCode_Framework
@system        Cornerstone C23/C11 Engine <-> Tenebris-and-Lux Rust Toolchain
@role          Document Isomorphic 4-Tier Macro x 4-Section Micro Test Topology
               Eliminating Ad-Hoc AI Guessing Across C11 and Rust 2024
@llm_shift_observed Ad-Hoc Smoke Asserts -> Stratified 4x4 Sensor Coverage
@dissertation_seam  Ch. 6 & Ch. 8.2 (Living Treatise)
@maturity      Rev 5 Formalized Evidence | Verified 839-Test Rust Baseline
@chassis       zero-script-bloat | Native CTest + Native Cargo Subdir Binaries
@seams         charter: ../reports_charter.md |
               registry: ../reports_registry.json |
               completed_plan: ../../plans/completed/
                               toolchain_rust_foundation_phases_1_to_1_5.md |
               skill: ../../skills/cornerstone-workflows/SKILL.md |
               treatise: ../../identity/treatise/
                         cpi_si_foundations_and_living_architecture.adoc
@invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5, <=80 col walls).
               2. 4 Macro Tiers: Functional, Unit, Integrated, System.
               3. 4 Micro Sections: Always Pass, Never Fail, Limits, Emergence.
               4. Dual Language Parity: Pure C11/C23 <-> Pure Rust 2024.
               5. Tests are diagnostic sensors, never vanity trophies.
================================================================================
-->

# Mirroring `engine/tests/` to `tools/tests/`: Macro Strata & Micro Projection

> **Project:** Tenebris and Lux / Cornerstone Game Engine -> OmniCode Track 7  
> **Toolchain:** `Tenebris-and-Lux/tools/` (`tl_tools`, `validator`, `master_guide`, `asset_generator`, `demo_extractor`)  
> **Created:** `2026-11-17` | **Updated:** `2026-11-20` | **Governance:** `Rev 5`  
> **Master Dissertation Seam:** [`cpi_si_foundations_and_living_architecture.adoc`](../../identity/treatise/cpi_si_foundations_and_living_architecture.adoc) (`Ch. 6`, `Ch. 8.2`)

---

<!--
================================================================================
BLOCK 2: OPENING BLOCK
( Plain-English Explanation: Why 4x4 Stratified Testing Matters )
================================================================================
-->

## Block 2 (Opening): Macro Folders vs. Micro Sections

Testing in this codebase is not about tossing a few quick asserts into one flat folder just to turn CI green. It runs along two clear axes so both human builders and CPI-SI agents always know **what altitude** they are testing and **how hard** each target is stressed:

1. **Macro Layer (Vertical Folders — Altitude):**
   * **Tier 1: Functional (`functional/`):** Coarse public API & CLI smoke behavior from an operator's view.
   * **Tier 2: Unit (`unit/`):** Tight isolation of individual cogs, buffers, structs, and Safe-Mode BIOS fallbacks.
   * **Tier 3: Integrated (`integrated/`):** Cross-module pipelines passing data across seams.
   * **Tier 4: System (`system/`):** Full-workspace stress sweeps, burst throughput, and exit-code audits.
2. **Micro Layer (Horizontal Block 3 Sections — Depth Inside Every Suite):**
   * **Section 1 (`Always Pass`):** Golden-path nominal behavior.
   * **Section 2 (`Never Fail`):** Fault inversion (corrupt inputs, missing roots, buffer bounds) testing graceful recovery.
   * **Section 3 (`Invariants & Limits`):** Mathematical laws and hard buffer ceilings.
   * **Section 4 (`Emergence`):** Complex multi-step workflows and state replay.

---

<!--
================================================================================
BLOCK 3: BODY BLOCK
( C11 Exemplars, 4x4 Toolchain Matrix & Native Cargo Folder Hierarchy )
================================================================================
-->

## 1. Bedrock Exemplars in the C Engine (`Tenebris-and-Lux/engine/tests/`)

```text
engine/tests/
|-- common/
|   +-- cornerstone_test.h            <- Shared assertions, timers & hooks
|-- functional/
|   |-- test_log_functional.c         <- Tier 1: Coarse API trace (16 log cogs)
|   |-- test_debug_functional.c       <- Tier 1: Breakpoint & snapshot trace
|   +-- test_io_functional.c          <- Tier 1: 16 VFS/JSON/Loader cogs
|-- unit/
|   |-- log/
|   |   |-- test_log_unit.c           <- Tier 2: 64-entry FIFO ring wrap
|   |   +-- test_log_fallback_unit.c  <- Tier 2: BIOS pre-init replay
|   |-- debug/
|   |   |-- test_debug_unit.c         <- Tier 2: FNV-1a hashes, 16 slots
|   |   +-- test_debug_fallback_unit.c<- Tier 2: Safe-Mode BIOS fallback
|   +-- io/
|       |-- test_path_finder_unit.c   <- Tier 2: VFS symlink containment
|       |-- test_json_utils_unit.c    <- Tier 2: Deterministic token scanner
|       |-- test_master_loader_unit.c <- Tier 2: 5-Block JSON hydration
|       +-- test_io_fallback_unit.c   <- Tier 2: Safe-Mode ROM config
|-- integrated/
|   |-- test_log_integrated.c         <- Tier 3: path_finder -> loader -> log -> debug
|   |-- test_debug_integrated.c       <- Tier 3: Multi-turn battle snapshots
|   +-- test_io_integrated.c          <- Tier 3: End-to-end VFS -> Config cache
+-- system/
    |-- test_log_system.c             <- Tier 4: 15,000-entry burst & rotation
    |-- test_debug_system.c           <- Tier 4: 50-turn rewind under pressure
    +-- test_io_system.c              <- Tier 4: 1,000-turn JSON fuzzing
```

---

## 2. The 4x4 Toolchain Verification Matrix (`tools/`)

```text
                    THE 4x4 TOOLCHAIN VERIFICATION MATRIX
+-------------+------------------+-----------------+-----------------+------------------+
|  Subsystem  | Tier 1:          | Tier 2:         | Tier 3:         | Tier 4:          |
|             | Functional       | Unit            | Integrated      | System           |
+-------------+------------------+-----------------+-----------------+------------------+
|    cmd/     | CLI flag parsers,| src/cmd/*.rs    | Cross-command   | Standalone tool  |
|             | default modes,   | (27 unittests)  | pipelines       | binaries, exit   |
|             | help & version   |                 | (assets, audit) | codes, live repo |
+-------------+------------------+-----------------+-----------------+------------------+
|   guide/    | Compiler modes,  | manifest.rs,    | integration_    | 6-volume, 114-ch |
|             | PDF layout rule  | snapshot.rs,    | guide.rs (docs  | compilation, PDF |
|             | syntax checkers  | theme.rs        | pipeline)       | layout audit     |
+-------------+------------------+-----------------+-----------------+------------------+
|   syntax/   | 8-language lexer | src/syntax/*.rs | OmniDoc 5-block | Full repository  |
|             | surface tokens   | (89 unittests)  | AST parser      | workspace syntax |
|             |                  |                 | (framework.rs)  | scan (C, Rust..) |
+-------------+------------------+-----------------+-----------------+------------------+
| validator/  | Language rule    | c.rs, cpp.rs,   | Framework multi-| Full workspace   |
|             | checkers in      | rust.rs, etc.   | language        | validate sweep,  |
|             | isolation        | (113 unittests) | pipeline        | git_validator    |
+-------------+------------------+-----------------+-----------------+------------------+
```

---

## 3. Native Cargo Folder Mirror (`tools/tests/`)

Cargo natively discovers integration binaries whenever a subfolder contains `main.rs`, allowing zero wrapper scripts:

```text
tools/tests/
|-- common/
|   +-- mod.rs + bin_tests.rs      <- Shared fixtures & binary harnesses
|-- functional/
|   |-- main.rs                    <- Cargo binary: functional
|   +-- {cmd,guide,syntax,validator}.rs            (65 tests)
|-- unit/
|   |-- main.rs                    <- Cargo binary: unit
|   +-- {assets,cmd,extractor,guide,runner,syntax,validator}/ (208 tests)
|-- integrated/
|   |-- main.rs                    <- Cargo binary: integrated
|   +-- {assets,audit,extract,guide,validate}.rs   (70 tests)
+-- system/
    |-- main.rs                    <- Cargo binary: system
    +-- {cmd,guide,syntax,validator}_system.rs     (56 tests)
```

---

<!--
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Preserved Historical 839-Test Verification & Re-Run Gate )
================================================================================
-->

## Block 4 (Audit): Preserved `839`-Test Breakdown & Re-Run Gate

*(Recorded directly from the `2026-11-17` milestone pass under strict `-D warnings`:)*

1. `unittests src/lib.rs` (in-crate module isolation): **`267` passed**
2. `tests/functional/main.rs` (`functional` binary): **`65` passed**
3. `tests/unit/main.rs` (`unit` binary): **`208` passed**
4. `tests/integrated/main.rs` (`integrated` binary): **`70` passed**
5. `tests/system/main.rs` (`system` binary): **`56` passed**
6. `doctests`: **`173` passed**

**Recorded Total (`2026-11-17`):** **`839` passed; `0` failed; `0` ignored** under `-D warnings`.

```bash
# Re-verify current Tenebris-and-Lux/tools/ parity anytime:
cargo test --manifest-path Tenebris-and-Lux/tools/Cargo.toml
```

---

<!--
================================================================================
BLOCK 5: CLOSING BLOCK
( ACTION / REEL - Plan Links, Outward Seams & Closing Seal )
================================================================================
-->

## Block 5 (Closing): Operational Reel & Closing Seal

### `PLAY` — Enforcing Stratified Parity on Future Tracks

1. **Keep 1:1 Macro Parity Across Languages:** Whether writing C23 (`Cornerstone`), Rust (`Track 1 / Track 7`), or Go (`FaithNet`), maintain the same 4-tier vertical altitude separation.
2. **Never Skip `Never Fail` (Section 2):** Every cog added to `omnicode/` must prove graceful failure recovery under corrupt or missing inputs before graduating out of staging.

### `PROJECT` — Outward System Seams

1. **Completed Milestone Anchor:** Backs [`toolchain_rust_foundation_phases_1_to_1_5.md`](../../plans/completed/toolchain_rust_foundation_phases_1_to_1_5.md) (`Phases 1 & 1.5`) as empirical evidence.
2. **Feeds Active Rust Ports & OmniCode `Track 7`:** Guides test expansion for Phase 2–4 ports ([`toolchain_rust_port_phases_2_to_4.md`](../../plans/active/toolchain_rust_port_phases_2_to_4.md)) and [`omnicode_architecture_and_vision.md`](../../plans/vision/omnicode_architecture_and_vision.md).
3. **Dissertation Grounding (`Ch. 6, Ch. 8.2`):** Demonstrates isomorphic verification across C11/C23 and Rust 2024.

### `RECORD` — Linked Substrate Anchors

* Reports Charter & Registry: [`reports_charter.md`](../reports_charter.md), [`reports_registry.json`](../reports_registry.json)
* Companion Verification & Cognitive Reports: [`v0_1_2_random_check_in_report.md`](v0_1_2_random_check_in_report.md), [`block_eval_process_and_scoping_report.md`](../cognitive_and_behavioral/block_eval_process_and_scoping_report.md)
* Plans, Skill & Treatise: [`toolchain_rust_foundation_phases_1_to_1_5.md`](../../plans/completed/toolchain_rust_foundation_phases_1_to_1_5.md), [`cornerstone-workflows/SKILL.md`](../../skills/cornerstone-workflows/SKILL.md), [`cpi_si_foundations_and_living_architecture.adoc`](../../identity/treatise/cpi_si_foundations_and_living_architecture.adoc)

### `COMMIT` — Closing Seal

* **ASCII & Fidelity Upgrade Verified:** Converted all three original Unicode box-drawing diagrams to pure 7-bit ASCII and upgraded Rev-5 Block 1–5 markers while preserving the exact `839`-test historical figures intact (`2026-11-20`).
* **Registry & Track Binding:** Indexed under `architecture_and_verification/` in `reports_registry.json` and governed by `reports_charter.md`.
* **Sensor Architecture Proof:** Establishes the 4x4 `engine/tests/` <-> `tools/tests/` mirroring topology as a structural nervous-system baseline for autonomous verification.
* **LLM Shift Logged:** Shifts ad-hoc test assertions toward topological module-to-sensor pairing that survives agent restarts and cross-repo audits.
* **Verification Gate:** Subject to Rev-5 header, comment-width (`<= 80` cols), pure-ASCII table fences, and zero-broken-link checks.
* **Status:** `@created 2026-11-17` / `@updated 2026-11-20`, `TEST_MIRRORING_REPORT_FORMALIZED (+1:PROCEED)`.

<!-- END OF BLOCK 5: CLOSING SEAL -->
