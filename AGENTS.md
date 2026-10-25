# Repository Guidelines & Agent Standards

> **Project**: Tenebris and Lux  
> **Standard**: OmniCode 5-Block Ontological Architecture & Invariant Discipline  
> **Author**: CreativeWorkzStudio LLC / ProfessorSeanEX  
> **Repository**: [https://github.com/Novaking34/tenebris-lux](https://github.com/Novaking34/tenebris-lux)

---

## 1. Architectural Philosophy: The 5-Block Lifecycle Protocol

Every source file, header, and configuration file in this repository strictly adheres to the **Universal 5-Block Lifecycle Protocol** adapted from the OmniCode framework. This structure eliminates ambiguity, guarantees self-documenting code, and enforces compile-time and runtime invariants.

```text
┌────────────────────────────────────────────────────────────────────────┐
│                        BLOCK 1: METADATA BLOCK                         │
│  Identity, Provenance, Invariant Boundary Contracts & System Anchors   │
├────────────────────────────────────────────────────────────────────────┤
│                    BLOCK 2: OPENING / TYPES BLOCK                      │
│  Includes, Preconditions, Enums, State Structures & Global Constants   │
├────────────────────────────────────────────────────────────────────────┤
│                         BLOCK 3: BODY BLOCK                            │
│  Core Logic, State Transitions, Execution Routines & Gameplay Loop     │
├────────────────────────────────────────────────────────────────────────┤
│                   BLOCK 4: AUDIT PROCEDURES BLOCK                      │
│  Diagnostic Audits, State Invariant Verifications & Telemetry Traces   │
├────────────────────────────────────────────────────────────────────────┤
│                BLOCK 5: CLOSING / TEST & CLI BLOCK                     │
│  CLI Entry Point, Automated Invariant Unit Tests & Execution Teardown  │
└────────────────────────────────────────────────────────────────────────┘
```

### Block Breakdown

1. **BLOCK 1: METADATA BLOCK**
   - Must contain file identity tags (`@project`, `@file`, `@layer`, `@version`, `@author`, `@system`, `@role`, `@invariants`).
   - Outlines the architectural intent and axiomatic invariants governing the module.

2. **BLOCK 2: OPENING / TYPES BLOCK**
   - Explicit includes of standard headers.
   - Type definitions, enums, structs, and compile-time constants (`#define`).
   - Preconditions and memory layout definitions.

3. **BLOCK 3: BODY BLOCK**
   - The implementation logic, algorithm definitions, and state transitions.
   - Pure, modular, single-responsibility functions.

4. **BLOCK 4: AUDIT PROCEDURES BLOCK**
   - Dedicated diagnostic routines (e.g. `audit_*` functions).
   - Asserts and validates data structure integrity (e.g., verifying Lux remains within `[0, MAX_LUX]`, room IDs are within valid bounds).
   - Reports detailed audit manifests without relying on crude crashes.

5. **BLOCK 5: CLOSING / TEST & CLI BLOCK**
   - For executables (`src/main.c`): Command-line argument parsing (`--audit`, `--help`, `--version`, `--smoke-test`), teardown, and return codes.
   - For modular units: Standalone unit tests and verification suite.
   - For headers (`.h`): Include guard closing `#endif`.

---

## 2. Invariant & Quality Discipline

1. **Form Precedes Fullness**:
   - Define exact data types and invariants before writing procedural logic.
   - State representation must be clear, bounded, and deterministic.

2. **Zero-Warning Tolerance**:
   - Code must compile cleanly with `-Wall -Wextra -Wpedantic` under `-std=c11`.
   - Never suppress compiler warnings without documented rationale.

3. **Diagnostic Audit Requirement**:
   - Every core subsystem must provide an audit function that can be invoked via CLI (`--audit`) or CI/CTest to verify state validity.

4. **Line-Length Discipline**:
   - Keep line lengths bounded (recommended $\le 80$ characters, strict limit $\le 100$ characters) for high readability across split-view IDEs.

5. **Indentation & Formatting**:
   - Four spaces for indentation (no hard tabs).
   - Function signatures, braces, and control structures follow K&R/Linux kernel style with 4-space indent.

---

## 3. Coding Style & Naming Conventions

- **Functions & Variables**: `snake_case` (e.g., `game_init`, `player_state`, `lumina_shards`).
- **Types & Enums**: `snake_case` with `_t` suffix (e.g., `player_state_t`, `room_id_t`, `game_audit_report_t`).
- **Constants & Macros**: `UPPER_SNAKE_CASE` (e.g., `APP_NAME`, `MAX_LUX`, `INITIAL_LUX`).
- **Header Guards**: `#ifndef TENEBRIS_LUX_<FILENAME>_H` `#define TENEBRIS_LUX_<FILENAME>_H` ... `#endif`.
- **Memory & Safety**:
  - Always check buffer sizes and bounds before string manipulations (`snprintf`, `strncpy`).
  - No undefined behaviors, dangling pointers, or unchecked return values.

---

## 4. Build, Test, and Development Commands

CMake 3.24+ is configured for C11:

- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`: Configure build directory.
- `cmake --build build --config Debug`: Compile executable `./build/tenebris-lux`.
- `ctest --test-dir build -C Debug --output-on-failure`: Run full automated test suite.
- `./build/tenebris-lux --audit`: Run internal diagnostic self-audit suite.
- `./build/tenebris-lux --version`: Display version information.
- `./build/tenebris-lux --help`: Display command-line options.
- `gcc -std=c11 -Wall -Wextra -Wpedantic -fsyntax-only -Iinclude src/main.c`: Fast syntax and diagnostics check without producing a binary.

---

## 5. Structured Git Commit & Release Tagging Guidelines

Do **not** use terse or vague one-line commit messages. Every commit must serve as an enduring, self-contained record of engineering rationale, architectural modifications, and verification telemetry that subsequent agents and human developers can build upon.

### Commit Message Format

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
- ctest results and pass rates.
- Diagnostic audit status (e.g. ./build/tenebris-lux --audit output).
- Compiler diagnostic check (-Wall -Wextra -Wpedantic clean).
```

### Milestone & Release Tagging

- The current development phase is **v0.0.1** (Early Foundational Text Prototype).
- Every major structural milestone must be explicitly tagged in Git (e.g., `git tag -a v0.0.1 -m "..."`).
