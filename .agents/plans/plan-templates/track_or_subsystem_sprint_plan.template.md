<!--
================================================================================
BLOCK 1: METADATA BLOCK
( Blank Template: Bounded Track or Subsystem Sprint Plan w/ n* Stride Gates )
================================================================================
@project       {{PROJECT_OR_ECOSYSTEM}}
@file          .agents/plans/plan-templates/
               track_or_subsystem_sprint_plan.template.md
@id            cog.agent.plans.template.track_sprint
@layer         Portable Agent Substrate - Bounded Track/Cog Sprint Blueprint
@archetype     template | track_or_subsystem_sprint_plan
@version       {{COORDINATE_VERSION}}
@interface_rev Rev 5
@created       YYYY-MM-DD
@updated       YYYY-MM-DD
@author        {{OPERATOR_AND_INSTANCE_AUTHORS}}
@repository    https://github.com/Novaking34/OmniCode_Framework
@system        {{TRACK_OR_SUBSYSTEM_NAME}}
@role          Blank Blueprint for Bounded Single-Track Execution Plans
               Featuring Phase-Locked n* Strides and Hard Verification Proofs
@maturity      BLANK_TEMPLATE (Copy into ../active/ for bounded sprint work)
@chassis       {{MEMORY_AND_LANGUAGE_CONSTRAINTS}}
@seams         charter: ../plans_charter.md |
               registry: ../plans_registry.json |
               host: ../../host/hardware_calibration.json |
               deliberation: ../../reports/ (n-time deliberation algorithm)
@invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5, <=80 col walls).
               2. Bounded Stride n*: stop at explicit 0:AWAIT seams before
                  touching sibling Tracks or downstream mirrors.
               3. Zero unverified claims: list exact post-edit check commands.
               4. All relative links remain strictly inside .agents/.
================================================================================
-->

# `{{SPRINT_TITLE}}` — Track / Subsystem Sprint Plan

> **Template Purpose (Read Before Copying):** Copy this template into `../active/<track_sprint_slug>.md` when executing a focused change inside one Track or subsystem. Keep scope tight, state exact non-goals, and archive cleanly into `../completed/` once verified.

---

<!--
================================================================================
BLOCK 2: OPENING BLOCK
( Sprint Goal, Explicit Non-Goals & Host/Covenant Preconditions )
================================================================================
-->

## Block 2 (Opening): Bounded Sprint Scope

* **Target Track / Cog:** `{{TRACK_AND_MODULE}}`
* **Done Looks Like:** `{{CONCRETE_EXIT_CONDITION}}`
* **Explicit Non-Goals (Out of Scope):** `{{WHAT_WE_ARE_NOT_TOUCHING_THIS_SPRINT}}`
* **Host Guardrails Checked:** Confirm RAM/VRAM/thread caps in [`hardware_calibration.json`](../../host/hardware_calibration.json).

---

<!--
================================================================================
BLOCK 3: BODY BLOCK
( Ordered Execution Steps, Affected Seams & 0:AWAIT Checkpoints )
================================================================================
-->

## 1. Bounded Execution Loop (`n*` Stride)

```mermaid
flowchart LR
    S0["0: Read Target + Seams"] --> S1["1: Implement Bounded Cog"]
    S1 --> S2["2: Consolidated Verification"]
    S2 --> S3["3: 0:AWAIT Report-Out Before Next Track"]
```

### Ordered Sprint Steps

1. **Step 1 — `{{STEP_1_TITLE}}`:**
   * Files touched: `{{STEP_1_FILES}}`
   * Structural contract: `{{STEP_1_CONTRACT}}`
2. **Step 2 — `{{STEP_2_TITLE}}`:**
   * Files touched: `{{STEP_2_FILES}}`
   * Structural contract: `{{STEP_2_CONTRACT}}`
3. **Step 3 — `0: AWAIT` Seam Checkpoint:**
   * Hold before expanding scope across neighboring Tracks or mirrors; deliver concise report-out matching [`n_time_call_response_deliberation_algorithm_report.md`](../../reports/cognitive_and_behavioral/n_time_call_response_deliberation_algorithm_report.md).

---

<!--
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Exact Post-Edit Verification Battery & T4 Tripwires )
================================================================================
-->

## Block 4 (Audit): Verification Battery & `T4` Tripwires

Run in one consolidated command after the final edit:

```bash
# Replace with exact build, test, and lint checks for this Track
echo "VERIFY_SPRINT_PASS_PLACEHOLDER"
```

| `T4` State | Observed Sprint Condition | Required Action |
| :--- | :--- | :--- |
| **`+1:PROCEED`** | All stated verification commands pass with zero skipped assertions | Move sprint record to `../completed/` or report ready for commit |
| **`0:AWAIT`** | Discovered assumption mismatch or second verification failure | Hand back cleanly to Operator/planning view without guessing |
| **`-1:ROLLBACK`** | Sprint drifted into unrequested sibling Tracks or broke passing suites | Revert out-of-scope edits immediately and re-verify |
| **`NULL:HALT`** | Reported unverified checks or bypassed safety/privacy bounds | Lock edits and reset cleanly to `0:AWAIT` |

---

<!--
================================================================================
BLOCK 5: CLOSING BLOCK
( ACTION / REEL - Sprint Lifecycle, Outward Seams & Portable Anchors )
================================================================================
-->

## Block 5 (Closing): Operational Reel & Closing Seal

### `PLAY` — Sprint Execution Posture

1. **Copy & Scope First:** Copy this blueprint into `../active/<sprint_slug>.md`, replace every `{{PLACEHOLDER}}`, and register it in [`plans_registry.json`](../plans_registry.json).
2. **Walk Side by Side:** Respect `0: AWAIT` boundaries so human `n`-time insights integrate cleanly between strides.

### `PROJECT` — Outward Graduation Seams

1. **Graduate Cleanly on Completion:** When all Block 4 checks pass and Operator signs off, move this file into `../completed/` and update `plans_registry.json`.
2. **Protect Mirrors:** Never push sprint changes into downstream `.agents/` mirrors mid-flight.

### `RECORD` — Linked Anchors Inside `.agents/`

* Planning Charter & Registry: [`plans_charter.md`](../plans_charter.md), [`plans_registry.json`](../plans_registry.json)
* Deliberation Math & Covenant Skill: [`n_time_call_response_deliberation_algorithm_report.md`](../../reports/cognitive_and_behavioral/n_time_call_response_deliberation_algorithm_report.md), [`cpi-si-covenant/SKILL.md`](../../skills/cpi-si-covenant/SKILL.md)
* Host Limits & BIOS Index: [`hardware_calibration.json`](../../host/hardware_calibration.json), [`.agents/README.md`](../../README.md)

### `COMMIT` — Closing Seal

* **Single-Track Honesty:** Small verified strides compound faster than sprawling unverified rewrites.
* **Explicit Non-Goals:** Hold sibling Tracks untouched unless Operator explicitly widens scope at a `0: AWAIT` checkpoint.
* **One-Command Final Proof:** Re-run every stated build, test, and lint assertion after the final edit and report only directly observed counts.
* **Done-vs-Remaining Discipline:** Graduate finished sprints cleanly into `../completed/` and update [`plans_registry.json`](../plans_registry.json) 1:1.
* **Host-Calibrated Limits:** Respect non-headless RAM (`3,072 MiB`) and VRAM (`350 MiB`) guards defined in [`hardware_calibration.json`](../../host/hardware_calibration.json).
* **Dual-Clock Deliberation:** Reserve explicit pauses at `0: AWAIT` boundaries so Operator synthesis between turns integrates cleanly.
* **Zero Unverified Claims:** Quote only observed filenames, symbol names, and post-edit check outputs when reporting sprint completion.
* **Status:** `@created YYYY-MM-DD` / `@updated YYYY-MM-DD`, `TRACK_SPRINT_TEMPLATE (0:AWAIT UNTIL INSTANTIATED)`.

<!-- END OF BLOCK 5: CLOSING SEAL -->
