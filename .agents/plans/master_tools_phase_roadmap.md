<!--
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          .agents/plans/master_tools_phase_roadmap.md
@id            plan.tools.master_phase_roadmap
@layer         Layer 4: Toolchain Architecture & Planning Strata
@archetype     operational | architectural_plan
@version       0.1.2-dev
@interface_rev Rev 5
@date          2026-11-16
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        OmniCode Developer Toolchain // Phase Roadmap & Progress Map
@role          Master Tools Phase Roadmap: Python-to-Rust & Test Architecture
@maturity      Phase 1 Complete | Ready for Test Harness Structuring & Phase 2
@maturity      Phase 1 & Phase 1.5 Complete | Ready for Phase 2: Sensory Exoskeleton
@invariants    1. Native static binaries in tools/ (zero ad-hoc runtime dependencies).
               2. Strict conformance to Rust 2024 Edition and -D warnings.
               3. 4-Tier Macro Test Architecture (Functional, Unit, Integrated, System).
               4. Universal 5-Block Lifecycle Protocol across all files.
================================================================================
-->

# Master Tools Phase Roadmap: Where We Are on the Map

> **Project**: Tenebris and Lux | **Toolchain**: `tools/` (`tl_tools`, `validator`, `master_guide`, `launcher`)
> **Milestone**: `0.1.2-dev` | **Governance Revision**: `Rev 5`
> **Navigation Status**: **Phase 1 Complete** $\longrightarrow$ **Test Tier Structure & Cargo.toml Alignment** $\longrightarrow$ **Phase 2 Entry**
> **Navigation Status**: **Phase 1 & Phase 1.5 Complete** $\longrightarrow$ **Phase 2 Entry (Sensory Exoskeleton)**

---

## 1. Executive Roadmap & Current Coordinate

```text
 ┌─────────────────────────────────────────────────────────────────────────────┐
 │                      THE 4-PHASE TOOLCHAIN MIGRATION MAP                    │
 ├─────────────────────────────────────────────────────────────────────────────┤
 │ [PHASE 1: Toolchain Modularization & De-Monolithization]      [ COMPLETED ] │
 │   • cmd/ architecture established (assets, audit, extract, guide, validate) │
 │   • guide.rs (4,044 lines) decomposed into tools/src/guide/                 │
 │   • c_header.rs (3,463 lines) decomposed into tools/src/validator/ & syntax/│
 │   • 8-Language Lexers & Validators complete (C, C++, Rust, JSON, etc.)      │
 │   • Verification: 267 unittests, 15 integration suites, 173 doctests (100%) │
 │   • Verification: 267 unittests, 173 doctests (100% clean under -D warnings)│
 ├─────────────────────────────────────────────────────────────────────────────┤
 │ [PHASE 1.5: Macro Test Hierarchy & Cargo.toml Modernization]   [ CURRENT ]  │
 │   • Mirror engine/tests/ 4-tier macro hierarchy in tools/tests/             │
 │ [PHASE 1.5: Macro/Micro Test Hierarchy & Cargo.toml Alignment][ COMPLETED ] │
 │   • 1:1 Mirror of engine/tests/ 4-tier macro hierarchy in tools/tests/      │
 │     (common/, functional/, unit/, integrated/, system/ via Cargo main.rs)   │
 │   • Update and structure tools/Cargo.toml to OmniCode 5-Block standard      │
 │   • Zero transient artifact leaks, -D warnings compliance                   │
 │   • Micro subfolder depth in unit/ (cmd/, guide/, syntax/, validator/)      │
 │   • Dedicated code validators: c.rs, rust.rs, cmake.rs, framework, languages│
 │   • 399 integration tests passing (65 func, 208 unit, 70 integ, 56 sys)    │
 │   • Purged all 15 legacy flat test files from tools/tests/                  │
 │   • Updated tools/Cargo.toml with 5-block envelope and target registry      │
 ├─────────────────────────────────────────────────────────────────────────────┤
 │ [PHASE 2: Porting the Sensory Exoskeleton (probes/ & exoskeleton/)] [ NEXT ]│
 │ [PHASE 2: Porting the Sensory Exoskeleton (probes/ & exoskeleton/)] [ READY ]│
 │   • Hardware telemetry (CPU, GPU, VRAM, RAM via direct /proc & sysfs)       │
 │   • AST code skeletonizer (tools/src/syntax/c.rs & cmd/inspect.rs)          │
 │   • Native compiler tripwire (runner/compiler.rs)                           │
 ├─────────────────────────────────────────────────────────────────────────────┤
 │ [PHASE 3: Porting Memory & Calibration Engines (memory/ & calibration/)]    │
 │   • Symbol indexer & in-memory B-Tree store                                 │
 │   • Dynamic modelfile generator (Ollama 5-block system prompts)             │
 │   • Hardware sweet-spot calculator (0.5B vs 1.5B vs 3B vs 7B)               │
 ├─────────────────────────────────────────────────────────────────────────────┤
 │ [PHASE 4: Porting the CPI-SI Reflection Engine (agent/ & main.py)]          │
 │   • In-process tool dispatcher (zero-process overhead validation)           │
 │   • Stepped gearbox & offline Ollama client                                 │
 │   • Native REPL / orchestrator binary (omnicode_orchestrator)               │
 │   • Native GUI studio (launcher & desktop cockpit)                          │
 └─────────────────────────────────────────────────────────────────────────────┘
```

---

## 2. Phase 1 Accomplishments: The Foundation is Solid
## 2. Phase 1 & 1.5 Accomplishments: Bedrock Solidified

Phase 1 was the largest single pass because it had to set the architectural baseline for the entire toolchain:
1. **`tools/src/cmd/`**: Created the central execution sub-assemblies (`assets.rs`, `audit.rs`, `extract.rs`, `guide.rs`, `validate.rs`).
2. **`tools/src/guide/`**: Split the 4,044-line monolith into focused, decoupled modules (`compiler.rs`, `manifest.rs`, `pdf.rs`, `snapshot.rs`, `theme.rs`).
3. **`tools/src/syntax/`**: Established universal 5-block syntax parsing (`framework/`) and tokenizers for 8 concrete languages (`c.rs`, `cpp.rs`, `rust.rs`, `json.rs`, `adoc.rs`, `md.rs`, `toml.rs`, `cmake.rs`).
4. **`tools/src/validator/`**: Decomposed `c_header.rs` into the unified validation framework (`framework/`) and dedicated language validators with Block 4 self-audits and Block 5 readiness probes.
5. **Empirical Proof**:
   - `cargo test --manifest-path tools/Cargo.toml`: **267 unittests**, **15 integration suites**, **173 doctests** passed (0 errors, 0 warnings under `-D warnings`).
Phase 1 and 1.5 established the architectural bedrock and macro/micro verification posture:
1. **`tools/src/cmd/`**: Modular command dispatchers (`assets.rs`, `audit.rs`, `extract.rs`, `guide.rs`, `validate.rs`).
2. **`tools/src/guide/`**: Decoupled book generator (`compiler.rs`, `manifest.rs`, `pdf.rs`, `snapshot.rs`, `theme.rs`).
3. **`tools/src/syntax/`**: Universal 5-block framework and multi-language lexers (`c.rs`, `cpp.rs`, `rust.rs`, `json.rs`, `adoc.rs`, `md.rs`, `toml.rs`, `cmake.rs`).
4. **`tools/src/validator/`**: Multi-language rules engines with self-audits and readiness probes.
5. **`tools/tests/` 4-Tier Macro Hierarchy**:
   - `tests/common/`: Canonical 5-block shared test scaffolding and fixtures.
   - `tests/functional/`: Surface CLI and engine command line tests (**65 tests**).
   - `tests/unit/`: Component isolation with subfolder depth for heavy subsystems:
     - `cmd/`: CLI argument parser and execution envelopes (**36 tests**).
     - `guide/`: Compiler, manifest, pdf, snapshot, and theme units (**48 tests**).
     - `syntax/`: 5-block framework and 8 language tokenizers (**32 tests**).
     - `validator/`: `c.rs`, `rust.rs`, `cmake.rs`, `framework.rs`, `languages.rs` (**92 tests**).
     - Total `unit` tests: **208 tests**.
   - `tests/integrated/`: Cross-boundary pipeline integration tests (**70 tests**).
   - `tests/system/`: End-to-end full-pipeline integration tests (**56 tests**).
6. **Empirical Proof Across Repository**:
   - `cargo test --manifest-path tools/Cargo.toml`: **839 tests passed** (267 lib, 399 integration, 173 doctests, 0 failed, 0 warnings).
   - `ctest --test-dir build -C Debug`: **66/66 passed** (100%).
   - `./build/bin/tenebris-lux --audit`: **9/9 passed** (+ preflight clean).
   - `./build/bin/tenebris-lux --audit`: **9/9 checks + Pre-Flight passed** (100%).

---

## 3. Immediate Mission: The Phase 1.5 Test Structuring Plan
## 3. Runway to Phase 2: Sensory Exoskeleton (`probes/` & `exoskeleton/`)

To complete Phase 1 and prepare the runway for Phase 2:

### Step 3.1: Structure `tools/tests/` into 4 Macro Tiers
Using Cargo's native subdirectory test compilation feature (`main.rs` per subfolder), organize tests to physically mirror `engine/tests/`:
```text
tools/tests/
├── common/
│   └── mod.rs              <- Shared test fixtures, workspace resolvers, sample doc generators
├── functional/
│   ├── main.rs             <- Cargo test binary 'functional'
│   ├── cmd.rs              <- CLI surface & argument flags
│   ├── guide.rs            <- Guide compiler mode flags
│   ├── syntax.rs           <- 8-language lexer token surface
│   └── validator.rs        <- Rule validator surface
├── unit/
│   ├── main.rs             <- Cargo test binary 'unit'
│   ├── guide_manifest.rs
│   ├── guide_compiler.rs
│   ├── guide_snapshot.rs
│   ├── guide_pdf.rs
│   ├── guide_theme.rs
│   ├── syntax_framework.rs
│   ├── syntax_languages.rs
│   ├── validator_c.rs
│   ├── validator_framework.rs
│   └── validator_languages.rs
├── integrated/
│   ├── main.rs             <- Cargo test binary 'integrated'
│   ├── assets.rs           <- cmd::assets pipeline
│   ├── audit.rs            <- cmd::audit pipeline
│   ├── extract.rs          <- cmd::extract pipeline
│   ├── guide.rs            <- cmd::guide multi-cog pipeline
│   └── validate.rs         <- cmd::validate full validation pipeline
└── system/
    ├── main.rs             <- Cargo test binary 'system'
    ├── cmd_dispatch.rs     <- Standalone tool binary execution & exit code validation
    ├── guide_system.rs     <- Full 6-volume, 114-chapter generation & PDF layout audit
    ├── syntax_system.rs    <- Full workspace syntax scan
    └── validator_system.rs <- Full workspace validation audit
```

### Step 3.2: Modernize `tools/Cargo.toml`
1. Upgrade metadata block to the latest Rev 5 5-Block schema.
2. Cleanly configure all binary targets, library targets, profiles, and lints.
3. Verify test compilation with zero warnings under `-D warnings`.

### Step 3.3: Verification Sweep
Execute full Cargo, CTest, and engine binary audits to confirm 100% clean health.
With the 4-tier macro test suite in place, Phase 2 brings the sensory perception subsystem from Python into native Rust:
1. **`tools/src/probes/`**: Direct `/proc` and `sysfs` telemetry for CPU, RAM, GPU, and VRAM detection without external process execution.
2. **`tools/src/exoskeleton/`**: Fast AST skeletonization for C, C++, and Rust files, creating concise structural representations for cognitive agents.
3. **`tools/src/runner/compiler.rs`**: Native compiler tripwires integrating with `gcc`, `clang`, `cl.exe`, and `rustc`.
4. **Test Alignment**: Extending `tools/tests/{functional,unit,integrated,system}/` with corresponding `probes` and `exoskeleton` test suites.
