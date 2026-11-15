<!--
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          .github/pull_request_template.md
@id            github.template.pull_request
@layer         Developer Tooling & VCS Governance (Layer 4 Infrastructure)
@version       0.1.2-dev
@interface_rev Rev 5
@date          2026-11-14
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Pull Request Governance & Verification Gate
@role          Enforces Universal 5-Block Ledger & Dual-Audience Review Discipline
================================================================================
-->

## Pull Request Description (OmniCode Universal 5-Block Protocol)

### BLOCK 1: METADATA & REVISION RECORD

* **Project**: Tenebris and Lux (Flagship RPG) | **Engine**: Cornerstone Game Engine
* **Target Stratum**: (Layer 0 Core / Layer 1 Sim / Layer 2 Viewport / Layer 3 Plug / Layer 4 Verification / Tools / Docs)
* **Author / Submitter**:
* **Milestone / Version**: `v0.1.2-dev (Rev 5)`

---

### BLOCK 2: REASON FOR CHANGE & PROBLEM STATEMENT
<!--
Describe the architectural motivation, defect being resolved, or gameplay mechanic being expanded.
Why was this change required? What was the failure mode or limitation in the previous revision?
-->

---

### BLOCK 3: ARCHITECTURAL MODIFICATIONS & ELEVATIONS
<!--
List the concrete changes across the repository:
1. Engine Cogs modified or elevated into engine/
2. Game simulation systems or data schemas in game/
3. Developer tooling in tools/ or Python orchestrator in tools/orchestrator/
4. Documentation chronicles or compendiums in docs/
-->

#### Dual-Audience Impact Summary

* **For Game Designers, Writers & Ludologists**:
  <!-- How does this affect gameplay balance, tactical grid rules, narrative lore, or designer workflows? -->

* **For Systems Programmers & Engine Architects**:
  <!-- How does this affect the Controlled Memory Chassis, zero-heap bounds, reentrancy, or API seams? -->

---

### BLOCK 4: VERIFICATION, TRIPWIRES & PASS PROOF
<!--
Provide empirical pass proof from the 4-Tier Verification Suite.
-->

* [ ] **Engine Compilation**: Clean build under ISO C23 / C17 / C11 ladder (`-Wall -Wextra -Wpedantic` with 0 warnings).
* [ ] **CTest 4-Tier Battery**: **65/65 CTest suites** passing cleanly (`ctest --test-dir build -C Debug`).
* [ ] **Pre-Flight Self-Audit**: `./build/tenebris-lux --audit` passes (20 registered modules, 9/9 invariant tripwires).
* [ ] **Rust Developer Toolchain**: **31/31 Cargo unit tests** passing under `-D warnings` (`cargo test --manifest-path tools/Cargo.toml`).
* [ ] **Data & Header Validation**: `tl_tools validate all` audits 75 JSON manifests, 44 C11 headers, and commits cleanly.
* [ ] **Python Cognitive Exoskeleton**: All 3 test suites pass (`test_model_factory.py`, `test_auto_mode.py`, `test_staircase_sweep.py`).
* [ ] **Deterministic CLI Smoke Test**: `./build/tenebris-lux --cli --steps 5` executes with deterministic outcome.

```text
(Paste relevant test execution summary or pass proof telemetry here)
```

---

### BLOCK 5: INVARIANT COVENANTS & CLOSING AFFIRMATIONS
<!--
Affirm adherence to repository physical laws. Check all that apply:
-->

* [ ] **ALLOC_ZERO_HEAP**: Exactly 0 dynamic heap allocations in Layer 0 (Core/IO/Audio/Log/Debug) and Layer 1 (Simulation).
* [ ] **Downward Dependency Flow**: `engine/` contains zero imports, headers, or references to `game/`.
* [ ] **Procedural Fallback Floor**: Standalone engine boots cleanly even with zero external assets on disk via Tier-0 Safe-Mode BIOS ROMs (`*_fallback.[ch]`).
* [ ] **Authentic Canon**: Preserves canonical lore (Antagonist is The Dark Lord Usuru; twin relics are Shield of Lux & Sword of Tenebris; Metaphysical Neutrality).
* [ ] **Universal 5-Block Protocol**: Every modified or added C header, C source, and JSON config adheres to canonical 5-Block standards.
* [ ] **Repo-Relative Links**: Zero absolute `file:///` URIs in any markdown, AsciiDoc, or documentation artifacts.
