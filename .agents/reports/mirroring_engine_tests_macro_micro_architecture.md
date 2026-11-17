<!--
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          .agents/reports/mirroring_engine_tests_macro_micro_architecture.md
@id            report.architecture.test_mirroring_macro_micro
@layer         Layer 4: Verification Architecture & Toolchain Strata
@archetype     architectural_report | educational_synthesis
@version       0.1.2-dev
@interface_rev Rev 5
@date          2026-11-17
@author        CreativeWorkzStudio LLC / ProfessorSeanEX (Seanje Lenox-Wise)
@repository    https://github.com/Novaking34/tenebris-lux
@system        OmniCode Developer Toolchain // Test Architecture Synthesis
@role          Macro-to-Micro Test Mirroring Architecture Report
@maturity      Rev 5 Baseline | 4x4 Ontological Matrix Formulation
@invariants    1. Four-Tier Macro Strata: Functional, Unit, Integrated, System.
               2. Four-Section Micro Strata: Always Pass, Never Fail, Invariants, Emergence.
               3. Dual Language Parity: Pure C11 (Cornerstone) <-> Pure Rust 2024 (tl_tools).
               4. CPI-SI = Covenant Partnered Intelligence - Structured Intelligence.
               5. Tests are diagnostic sensors, not vanity trophies.

PURPOSE & ARCHITECTURAL ROLE:
  Definitive architecture report detailing how the Rust developer toolchain tests
  (`tools/tests/`) natively mirror the 4-tier macro hierarchy and 4-section micro
  testing protocol of the C11 engine (`engine/tests/`). Ensures zero toolchain
  fragmentation while maintaining strict ISO C11 and Rust 2024 boundary integrity.
================================================================================
-->

# Mirroring `engine/tests/` to `tools/tests/`: Macro Strata & Micro Projection

> **Project**: Tenebris and Lux (RPG) | **Engine**: Cornerstone Game Engine (`engine/`)
> **Toolchain**: `tools/` (`tl_tools`, `validator`, `master_guide`, `asset_generator`, `demo_extractor`)
> **Framework**: **Covenant Partnered Intelligence - Structured Intelligence (CPI-SI)**
> **Author**: CreativeWorkzStudio LLC / ProfessorSeanEX (Seanje Lenox-Wise)
> **Date**: 2026-11-17 | **Governance Revision**: `Rev 5`

---

## 1. The Core Idea: Macro Folders vs. Micro Sections

Testing in this codebase isn't about slapping together a few asserts just to get a green light. It’s organized along two clear axes so you always know what altitude you're testing at and exactly how to stress the code:

1. **The Macro Layer (Vertical Folders — At What Altitude Are You Testing?)**:
   - **Tier 1: Functional** (`functional/`): High-level smoke tests. Verifies the public API from the user's perspective before digging into internals.
   - **Tier 2: Unit** (`unit/`): Fine-grained isolation. Tests individual cogs, internal structs, state machines, buffer limits, and fallback BIOS ROMs.
   - **Tier 3: Integrated** (`integrated/`): Cross-subsystem data pipelines. Validates how modules hand off data to one another.
   - **Tier 4: System** (`system/`): Full application stress testing, long-running stability, high burst throughput, and watchdogging.

2. **The Micro Layer (Horizontal Sections — How Does Each Test Verify Its Target?)**:
   No matter which folder a test file lives in, its **Block 3 Body** always follows the exact same 4-section discipline:
   - **Section 1: Always Pass** (Nominal golden path; checks that things work when everything is normal).
   - **Section 2: Never Fail** (Fault inversion; feeds bad inputs, missing files, corrupted buffers, and tests graceful fallback).
   - **Section 3: Invariants & Limits** (Buffer saturation, mathematical laws, hard capacity ceilings).
   - **Section 4: Emergence** (Complex multi-step workflows, state machines, and end-to-end integration).

---

## 2. Bedrock Exemplars in the C Engine (`engine/tests/`)

Looking at `log/`, `debug/`, and `io/` in `engine/tests/` shows how this 4-tier matrix works in practice:

```text
engine/tests/
├── common/
│   └── cornerstone_test.h         <- Shared assertions, section timers, diagnostic hooks
├── functional/
│   ├── test_log_functional.c      <- Tier 1: Coarse API trace (16 log cogs)
│   ├── test_debug_functional.c    <- Tier 1: Surface breakpoint & snapshot trace
│   └── test_io_functional.c       <- Tier 1: 16 coarse I/O cogs across VFS/JSON/Loader
├── unit/
│   ├── log/
│   │   ├── test_log_unit.c        <- Tier 2: 64-entry FIFO ring wrap, rotation limits
│   │   └── test_log_fallback_unit.c <- Tier 2: BIOS pre-init replay, ROM profiles
│   ├── debug/
│   │   ├── test_debug_unit.c      <- Tier 2: FNV-1a hashes, 16-slot breakpoints, lock quota
│   │   └── test_debug_fallback_unit.c <- Tier 2: Safe-Mode BIOS ROM fallback
│   └── io/
│       ├── test_path_finder_unit.c <- Tier 2: VFS domain resolution, symlink containment
│       ├── test_json_utils_unit.c <- Tier 2: Deterministic token scanner
│       ├── test_master_loader_unit.c <- Tier 2: 5-Block JSON envelope hydration
│       └── test_io_fallback_unit.c <- Tier 2: Safe-Mode ROM config resolution
├── integrated/
│   ├── test_log_integrated.c      <- Tier 3: path_finder -> master_loader -> log -> debug AWAIT
│   ├── test_debug_integrated.c    <- Tier 3: Debugger snapshots across multi-turn battle sim
│   └── test_io_integrated.c       <- Tier 3: End-to-end VFS -> Loader -> Config cache
└── system/
    ├── test_log_system.c          <- Tier 4: 15,000-entry burst, archive rotation cap
    ├── test_debug_system.c        <- Tier 4: 50-turn rewind replay under memory pressure
    └── test_io_system.c           <- Tier 4: 1,000-turn JSON fuzzing, 100-pass bitwise replay
```

---

## 3. The 4x4 Toolchain Verification Matrix for `tools/`

In `tools/src/`, our four core subsystems (`cmd/`, `guide/`, `syntax/`, `validator/`) map into the exact same 4 tiers:

```text
                    THE 4x4 TOOLCHAIN VERIFICATION MATRIX
┌─────────────┬──────────────────┬─────────────────┬─────────────────┬──────────────────┐
│  Subsystem  │ Tier 1:          │ Tier 2:         │ Tier 3:         │ Tier 4:          │
│             │ Functional       │ Unit            │ Integrated      │ System           │
├─────────────┼──────────────────┼─────────────────┼─────────────────┼──────────────────┤
│    cmd/     │ CLI flag parsers,│ src/cmd/*.rs    │ Cross-command   │ Standalone tool  │
│             │ default modes,   │ (27 unittests)  │ pipelines       │ binaries, exit   │
│             │ help & version   │                 │ (assets, audit) │ codes, live repo │
├─────────────┼──────────────────┼─────────────────┼─────────────────┼──────────────────┤
│   guide/    │ Compiler modes,  │ manifest.rs,    │ integration_    │ 6-volume, 114-ch │
│             │ PDF layout rule  │ snapshot.rs,    │ guide.rs (docs  │ compilation, PDF │
│             │ syntax checkers  │ theme.rs        │ pipeline)       │ layout audit     │
├─────────────┼──────────────────┼─────────────────┼─────────────────┼──────────────────┤
│   syntax/   │ 8-language lexer │ src/syntax/*.rs │ OmniDoc 5-block │ Full repository  │
│             │ surface tokens   │ (89 unittests)  │ AST parser      │ workspace syntax │
│             │                  │                 │ (framework.rs)  │ scan (C, Rust..) │
├─────────────┼──────────────────┼─────────────────┼─────────────────┼──────────────────┤
│ validator/  │ Language rule    │ c.rs, cpp.rs,   │ Framework multi-│ Full workspace   │
│             │ checkers in      │ rust.rs, etc.   │ language        │ validate sweep,  │
│             │ isolation        │ (113 unittests) │ pipeline        │ git_validator    │
└─────────────┴──────────────────┴─────────────────┴─────────────────┴──────────────────┘
```

---

## 4. How Cargo Natively Mirrors the Engine Folder Hierarchy

We don't need clunky wrapper scripts to run these tests. Cargo natively discovers subdirectories under `tests/` whenever they contain a `main.rs` file.

This means `tools/tests/` physically mirrors `engine/tests/` directly on disk:

```text
tools/tests/
├── common/
│   └── mod.rs                     <- Shared fixtures (repo root, test args, sample docs)
├── functional/
│   ├── main.rs                    <- Cargo integration binary: 'functional'
│   ├── cmd.rs                     <- Tier 1: CLI surface & flags (15 tests)
│   ├── guide.rs                   <- Tier 1: Compilation modes (14 tests)
│   ├── syntax.rs                  <- Tier 1: Lexer token surface (16 tests)
│   └── validator.rs               <- Tier 1: Rule validation surface (20 tests)
├── unit/
│   ├── main.rs                    <- Cargo integration binary: 'unit'
│   ├── cmd/
│   │   ├── mod.rs                 <- Subsystem coordinator & readiness (7 tests)
│   │   ├── args.rs                <- Argument parsing isolation (16 tests)
│   │   └── dispatch.rs            <- Command dispatch envelopes (13 tests)
│   ├── guide/
│   │   ├── mod.rs                 <- Subsystem coordinator & readiness (7 tests)
│   │   ├── compiler.rs            <- AsciiDoc compiler logic (8 tests)
│   │   ├── manifest.rs            <- Volume manifest parser (8 tests)
│   │   ├── pdf.rs                 <- PDF layout constraints (9 tests)
│   │   ├── snapshot.rs            <- Code snapshot extractor (8 tests)
│   │   └── theme.rs               <- Theme YAML config (8 tests)
│   ├── syntax/
│   │   ├── mod.rs                 <- Subsystem coordinator & readiness (8 tests)
│   │   ├── framework.rs           <- 5-Block AST parser (12 tests)
│   │   └── languages.rs           <- 8 concrete language lexers (12 tests)
│   └── validator/
│       ├── mod.rs                 <- Subsystem coordinator & readiness (8 tests)
│       ├── c.rs                   <- C11/C23 header validator (16 tests)
│       ├── rust.rs                <- Rust 2024 code validator (18 tests)
│       ├── cmake.rs               <- CMake build validator (18 tests)
│       ├── framework.rs           <- 5-Block AST rule validator (16 tests)
│       └── languages.rs           <- Multi-language validators (16 tests)
├── integrated/
│   ├── main.rs                    <- Cargo integration binary: 'integrated'
│   ├── assets.rs                  <- Tier 3: Asset generation pipeline (14 tests)
│   ├── audit.rs                   <- Tier 3: Workspace audit pipeline (14 tests)
│   ├── extract.rs                 <- Tier 3: Demo extraction pipeline (14 tests)
│   ├── guide.rs                   <- Tier 3: Master guide multi-cog pipeline (14 tests)
│   └── validate.rs                <- Tier 3: Multi-language validation pipeline (14 tests)
└── system/
    ├── main.rs                    <- Cargo integration binary: 'system'
    ├── cmd_system.rs              <- Tier 4: Standalone binary execution & exit codes (14 tests)
    ├── guide_system.rs            <- Tier 4: Full 6-volume compilation & layout sweep (14 tests)
    ├── syntax_system.rs           <- Tier 4: Full repository syntax sweep (14 tests)
    └── validator_system.rs        <- Tier 4: Full repository validation audit (14 tests)
```

Running `cargo test --manifest-path tools/Cargo.toml` executes every tier cleanly:

1. `unittests src/lib.rs` (Tier 2 in-crate module isolation: **267 passed**)
2. `tests/functional/main.rs` (`functional` binary: **65 passed**)
3. `tests/unit/main.rs` (`unit` binary: **208 passed**)
4. `tests/integrated/main.rs` (`integrated` binary: **70 passed**)
5. `tests/system/main.rs` (`system` binary: **56 passed**)
6. `doctests` (API documentation verification: **173 passed**)

**Total Metric**: **839 passed; 0 failed; 0 ignored** under strict `-D warnings`, zero script interpreters, and sub-second execution.

This creates an isomorphic, reliable testing setup across both the C11 engine (`Cornerstone`) and the Rust developer suite (`tl_tools`).
