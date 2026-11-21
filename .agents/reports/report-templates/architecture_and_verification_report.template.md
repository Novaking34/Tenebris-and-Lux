<!--
================================================================================
BLOCK 1: METADATA BLOCK
( Blank Template: Systems Architecture, Memory Chassis & Verification Report )
================================================================================
@project       {{PROJECT_OR_ECOSYSTEM}}
@file          .agents/reports/report-templates/
               architecture_and_verification_report.template.md
@id            cog.agent.reports.template.architecture_verification
@layer         Portable Agent Substrate - Quantitative Verification Template
@archetype     template | architecture_and_verification_report
@version       {{COORDINATE_VERSION}}
@interface_rev Rev 5
@created       YYYY-MM-DD
@updated       YYYY-MM-DD
@author        {{OPERATOR_AND_INSTANCE_AUTHORS}}
@repository    https://github.com/Novaking34/OmniCode_Framework
@system        {{TRACK_OR_ENGINE_SUBSYSTEM}}
@role          Capture Verified ELF Sizes, Memory Budgets, Test Matrices, and
               Deterministic Benchmarks Grounding Architectural Decisions
@llm_shift_observed {{UNGROUNDED_GUESSING}} -> {{TOOL_VERIFIED_TELEMETRY}}
@dissertation_seam  {{TARGET_TREATISE_OR_DISSERTATION_CHAPTER}}
@maturity      BLANK_TEMPLATE (Copy to ../architecture_and_verification/)
@chassis       zero-bloat | Observed counts, timings & ELF section bytes
@seams         charter: ../reports_charter.md |
               registry: ../reports_registry.json |
               host: ../../host/hardware_calibration.json |
               treatise: ../../identity/treatise/
                         cpi_si_foundations_and_living_architecture.adoc
@invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5, <=80 col walls).
               2. Never extrapolate unprinted timings, sizes, or test counts.
               3. Distinguish [STANDARD OS BEDROCK] from [OMNICODE ENHANCEMENT].
               4. All relative links remain strictly inside .agents/.
================================================================================
-->

# `{{VERIFICATION_REPORT_TITLE}}` — Architecture & Verification Report

> **Template Purpose (Read Before Copying):** Copy this blueprint into `../architecture_and_verification/<slug>.md` when recording binary/ELF footprints, test hierarchies, golden-vector parity checks, or multi-language verification batteries. Keep Silas Oren's dual-audience clarity without inflating figures.

---

<!--
================================================================================
BLOCK 2: OPENING BLOCK
( Plain-English Problem, System Architecture Context & Measurement Scope )
================================================================================
-->

## Block 2 (Opening): What This Report Verifies & Why It Matters

* **Plain-English Purpose:** `{{PLAIN_ENGLISH_PURPOSE}}`
* **Host Environment Probed:** Verified against [`hardware_calibration.json`](../../host/hardware_calibration.json) (`{{HOST_OS_AND_TOOLCHAIN}}`).
* **Dissertation Evidence Role:** Feeds empirical systems benchmarks into `{{TARGET_TREATISE_OR_DISSERTATION_CHAPTER}}`.

---

<!--
================================================================================
BLOCK 3: BODY BLOCK
( Subsystem Architecture, Memory Chassis Telemetry & Verification Matrix )
================================================================================
-->

## 1. Architectural Topology & Data Flow

```text
+--------------------------+        +--------------------------+
| {{SUBSYSTEM_LEFT}}       | -----> | {{SUBSYSTEM_RIGHT}}      |
| {{LEFT_ROLE}}            |        | {{RIGHT_ROLE}}           |
+--------------------------+        +--------------------------+
```

## 2. Measured Resource & Verification Matrix

| Gate / Section | Observed Result | Exact Scope & Conditions |
| :--- | :--- | :--- |
| `{{CHECK_1}}` | `{{RESULT_1}}` | `{{SCOPE_1}}` |
| `{{CHECK_2}}` | `{{RESULT_2}}` | `{{SCOPE_2}}` |
| `{{CHECK_3}}` | `{{RESULT_3}}` | `{{SCOPE_3}}` |

### Key Architectural Takeaways

1. `{{TAKEAWAY_1}}`
2. `{{TAKEAWAY_2}}`

---

<!--
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Exact Re-Run Commands & T4 Pass/Rollback Gates )
================================================================================
-->

## Block 4 (Audit): Re-Verification Commands & `T4` Gates

```bash
# Exact commands required to reproduce every metric reported above
echo "REPRODUCE_BENCHMARK_SUITE_PLACEHOLDER"
```

| `T4` State | Observed Benchmark Condition | Required Action |
| :--- | :--- | :--- |
| **`+1:PROCEED`** | Every reported command ran after final edit with zero skipped assertions | Register in `reports_registry.json` and link from active plan/dissertation |
| **`0:AWAIT`** | Filtered/mocked run only OR hardware mismatch vs. `host/` profile | Mark scope explicitly as filtered/mocked before publishing |
| **`-1:ROLLBACK`** | Memory footprint or latency regressed past Track chassis budget | Halt feature promotion and diagnose seam regression |
| **`NULL:HALT`** | Extrapolated numbers or edited output counts detected | Discard unverified table and re-run live commands |

---

<!--
================================================================================
BLOCK 5: CLOSING BLOCK
( ACTION / REEL - Instantiation Steps, Outward Seams & Portable Seal )
================================================================================
-->

## Block 5 (Closing): Operational Reel & Closing Seal

### `PLAY` — Publishing This Verification Report

1. Copy this blueprint into `../architecture_and_verification/<slug>.md`, fill `@created` / `@updated`, and replace every `{{PLACEHOLDER}}`.
2. Record exact raw outputs and execution scope cleanly without rounding away failures.
3. Register the finished report in [`reports_registry.json`](../reports_registry.json).

### `PROJECT` — Outward System Seams

1. **Feeds Master Dissertation Quantitative Claims:** Grounds theoretical claims in real binary sizes and test matrices inside [`cpi_si_foundations_and_living_architecture.adoc`](../../identity/treatise/cpi_si_foundations_and_living_architecture.adoc).
2. **Guides Active Plans & Clean-Room Migration:** Supplies exact baseline proof for `omnicode/` graduation gates in `../../plans/active/`.
3. **Supports Offline Model Grounding:** Clear separation of OS bedrock vs. OmniCode enhancements feeds future Track 7 books.

### `RECORD` — Linked Anchors Inside `.agents/`

* Reports Charter & Registry: [`reports_charter.md`](../reports_charter.md), [`reports_registry.json`](../reports_registry.json)
* Host Telemetry & Living Treatise: [`hardware_calibration.json`](../../host/hardware_calibration.json), [`cpi_si_foundations_and_living_architecture.adoc`](../../identity/treatise/cpi_si_foundations_and_living_architecture.adoc)
* Cognitive BIOS Root Index: [`.agents/README.md`](../../README.md)

### `COMMIT` — Closing Seal

* **Zero Unverified Counts:** Only directly observed outputs enter this ledger.
* **Charter Compliance:** Instantiated reports register in `reports_registry.json` with explicit `@created`/`@updated` tags and relative links.
* **Fidelity Guard:** Commands, binary metrics, and suite tallies must distinguish fresh post-edit runs from historical baselines.
* **Diagram Discipline:** Keep all embedded ASCII tables and Mermaid diagrams strictly 7-bit clean (`ord <= 127`) inside fenced blocks.
* **Rev-5 Parity:** Maintain `<= 80`-col comment banners and `>= 20` non-empty Block 5 lines upon instantiation.
* **Status:** `@created YYYY-MM-DD` / `@updated YYYY-MM-DD`, `ARCHITECTURE_VERIFICATION_REPORT_TEMPLATE (0:AWAIT UNTIL INSTANTIATED)`.

<!-- END OF BLOCK 5: CLOSING SEAL -->
