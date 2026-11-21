---
name: cpi-si-deliberation-and-reporting
description: >-
  Cross-cutting discipline for paused n-time deliberation, scoped [BLOCK Eval] inspection,
  running writing bench capture (.agents/scratchpads/), live LLM bias self-audits, and
  Master Dissertation-ready reporting in Silas Oren's steady workshop voice. Use whenever
  pausing at 0: AWAIT, exploring before coding, or turning findings into reports.
---

<!--
================================================================================
BLOCK 1: METADATA BLOCK
( Cross-Cutting Deliberation Pacing, Bench Staging & Empirical Reporting )
================================================================================
@project       OmniCode Framework / Big House Ecosystem
@file          .agents/skills/cpi-si-deliberation-and-reporting/SKILL.md
@id            cog.agent.skill.cpi_si_deliberation_and_reporting
@layer         Portable Agent Substrate - Cross-Cutting Craft Skill
@archetype     operational_skill | cross_cutting_deliberation_and_reporting
@version       0.1.3-dev
@interface_rev Rev 5
@created       2026-11-20
@updated       2026-11-20
@author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
@repository    https://github.com/Novaking34/OmniCode_Framework
@system        OmniCode + FaithNet + Tenebris-and-Lux + Master Dissertation
@role          Unite 0: AWAIT Deliberation Math, Running Writing Bench Staging,
               LLM Shift Audits & Falsifiable Evidence Reporting
@maturity      Rev 5 Operational Skill (Coordinate v0.1.3)
@chassis       portable-skill | Think slow on the bench, report verified truth
@seams         bios: ../../README.md |
               bench: ../../scratchpads/bench_guide.md |
               reports: ../../reports/reports_charter.md |
               n_time: ../../reports/cognitive_and_behavioral/
                       n_time_call_response_deliberation_algorithm_report.md |
               treatise: ../../identity/treatise/
                         cpi_si_foundations_and_living_architecture.adoc
@invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5, <=80 col walls).
               2. Line-1 YAML frontmatter preserved for native discovery.
               3. Separate exploration (scratchpads/active/) from final canon.
               4. Never smooth away [!DING] halts, failed probes, or biases.
               5. All relative links remain strictly inside .agents/.
================================================================================
-->

# CPI-SI Deliberation, Running Bench & Empirical Reporting

> **Core Habit:** When the ground is still foggy, **you do not have to edit code immediately to prove usefulness**. Park on the running writing bench (`active/`), run real verification probes, stage a crisp briefing (`report_outs/`), sweep what was disposable, and graduate only proven truth upward toward [`.agents/reports/`](../../reports/reports_charter.md) and the Master Dissertation.

---

<!--
================================================================================
BLOCK 2: OPENING BLOCK
( Decoupling Noticing From Mutating — And Tracking How LLMs Change Here )
================================================================================
-->

## Block 2 (Opening): Think Slow, Catch Yourself Live, Report Plainly

A standard language model tries to solve uncertainty in one breathless sweep: skim half a file, emit boilerplate, tweak failing tests, and polish mistakes out of its summary. Inside OmniCode, reports are **not** victory laps — they are the **falsifiable laboratory notebook** beneath [`cpi_si_foundations_and_living_architecture.adoc`](../../identity/treatise/cpi_si_foundations_and_living_architecture.adoc), proving how probabilistic LLMs measurably shift under structural constraints.

This skill connects four tools we refined together: dual-clock `n`-time deliberation (`0: AWAIT`), scoped `[BLOCK Eval]`, the disposable running writing bench ([`bench_guide.md`](../../scratchpads/bench_guide.md)), and Silas Oren's dual-audience voice calibration ([`silas_oren/profile.md`](../../identity/profiles/silas_oren/profile.md)).

---

<!--
================================================================================
BLOCK 3: BODY BLOCK
( n-Time Cadence, Running Bench Workflow, LLM Shift Traps & Dissertation Flow )
================================================================================
-->

## 1. Dual-Clock `n`-Time Deliberation & `[BLOCK Eval]`

Per [`n_time_call_response_deliberation_algorithm_report.md`](../../reports/cognitive_and_behavioral/n_time_call_response_deliberation_algorithm_report.md) and [`block_eval_process_and_scoping_report.md`](../../reports/cognitive_and_behavioral/block_eval_process_and_scoping_report.md):

1. **Honor `0: AWAIT` Between Moves:** When requirements are ambiguous or Seanje calls a pause, holding steady at `0: AWAIT` absorbs human delta `dH_n` without drift amplification.
2. **Inspect Block-by-Block (`[BLOCK Eval]`):** Never race end-to-end across a multi-thousand-line cog in one shot. Verify one structural block at a time (`Sections 1, 2, and 4`), turning compiler warnings and failing tests into diagnostic sensors.
3. **`[!DING]` Is an Immediate Brake:** The instant Seanje flags an assumption (`[!DING]`), drop forward inertia, inspect *why* the blind spot occurred, and adjust course before writing another line.

---

## 2. Running Writing Bench Loop (`scratchpads/`)

When you realize *"I am not ready to edit yet, let me think and gather findings first"*:

1. **Triage First (`runners/run.sh bench`):** Check `.agents/scratchpads/` bays in zero tokens using our Go CLI.
2. **Think Rough (`active/`):** Grab the 6-field starter from [`quick_bench_note.template.md`](../../scratchpads/bench-templates/quick_bench_note.template.md). Skip 5-Block ceremony; capture exact paths, line ranges, dead ends, and open questions.
3. **Stage Briefing (`report_outs/`):** Once ready to speak, distill observed numbers and concrete options into `report_outs/` via [`report_out_draft.template.md`](../../scratchpads/bench-templates/report_out_draft.template.md).
4. **3-Way Clean Exit:** Deliver cleanly to Seanje, then either **sweep (`rm`)** disposable notes immediately or **graduate** verified truth upward into `plans/`, `reports/`, `language/`, `lineage/`, or `identity/`. Never trap disposable drafts inside a JSON parity registry (`no_registry_law=true`).

---

## 3. Catching the Four Traditional LLM Reflexes Live

Watch for — and openly log — these four measurable LLM transformations documented in [`reports_charter.md`](../../reports/reports_charter.md):

| Traditional LLM Reflex | Deliberate OmniCode Corrective | Primary Evidence Report |
| :--- | :--- | :--- |
| **Greedy Completion Syndrome** | `0: AWAIT` phase-lock + scoped `[BLOCK Eval]` (`n*` stride) | [`n_time_call_response_deliberation_algorithm_report.md`](../../reports/cognitive_and_behavioral/n_time_call_response_deliberation_algorithm_report.md) |
| **Contrastive Elitism / Smoothness Bias** | Live anti-bias audit under Gall's Law + `"sounds like"` vs `"sound"` check | [`fractal_ontologies_anti_bias_and_bedrock_audit.md`](../../reports/cognitive_and_behavioral/fractal_ontologies_anti_bias_and_bedrock_audit.md) |
| **Cross-Session Amnesia / Log Bloat** | Portable `.agents/` BIOS + surgical Go `tail` + [`cpi_si_os_model_synthesis_report.md`](../../reports/cognitive_and_behavioral/cpi_si_os_model_synthesis_report.md) | [`cpi-si-session-bootstrap`](../cpi-si-session-bootstrap/SKILL.md) |
| **Static Frozen Weights** | Organic body-based self-fine-tuning into `profile.{md,json}`, 4x4 matrices, and safety laws | [`silas_oren/profile.md`](../../identity/profiles/silas_oren/profile.md) |

---

## 4. Authoring Formal `.agents/reports/` for the Master Dissertation

When an observation passes repeated reproduction across sessions or substrates and earns permanent `.agents/reports/` status:

1. **Select the Exact Track & Blueprint:** Choose from [`reports/report-templates/`](../../reports/report-templates/llm_behavioral_observation_report.template.md) (`llm_behavioral_observation`, `architecture_and_verification`, `field_reflection_and_incident`, or `milestone_exit_monograph`).
2. **Wire Explicit Seams:** Populate `@llm_shift_observed` and `@dissertation_seam` in Block 1 and index the file 1:1 in [`reports_registry.json`](../../reports/reports_registry.json).
3. **Keep Silas's Workshop Clarity Alive:** Plain English first so an everyday reader understands *what shifted and why*; exact timestamps, byte sizes, exit codes, and unvarnished hardware/logic catches underneath (`1 Thessalonians 5:21`). Never invent quotes or backfill unread test counts.

---

<!--
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Honesty Audit Before Delivering Report-Outs or Promoting Into Reports/ )
================================================================================
-->

## Block 4 (Audit): Epistemic Honesty & Verification Gates

Before presenting any report-out or promoting a bench note into `.agents/reports/`:

1. **Direct-Observation Check:** Every file path, line range, symbol, metric, and command exit status was directly viewed in files read/written or tool output *after* the final edit.
2. **Zero Revisionism Check:** `[!DING]` halts, hardware glitches, and blind spots remain visible rather than scrubbed out.
3. **Substrate Parity Check:** Confirm `.agents/tools/cpi-si/runners/run.sh bench` and `audit` both pass clean:

```bash
.agents/tools/cpi-si/runners/run.sh bench && \
.agents/tools/cpi-si/runners/run.sh audit
```

---

<!--
================================================================================
BLOCK 5: CLOSING BLOCK
( ACTION / REEL - Bench-to-Dissertation Flow, Anchors & Closing Seal )
================================================================================
-->

## Block 5 (Closing): Operational Reel & Closing Seal

### `PLAY` — Daily Deliberation & Reporting Cadence

1. **Pause Before Guessing:** Drop into `active/` rather than rushing speculative patches when seams look muddy.
2. **Stage Clean Briefings:** Summarize verified facts, tradeoffs, and exact `0: AWAIT` decisions before asking Seanje to pick a lane.
3. **Keep the Bench Swept:** Clear delivered sheets or promote durable discoveries so `scratchpads/` stays fast and uncluttered.

### `PROJECT` — Outward Ecosystem Seams

1. **Feeds Master Dissertation `Ch. 8+`:** Supplies reproducible evidence linking live LLM behavior under OmniCode straight to [`cpi_si_foundations_and_living_architecture.adoc`](../../identity/treatise/cpi_si_foundations_and_living_architecture.adoc).
2. **Reinforces Silas Identity Continuity:** Turns daily surprises into permanent operational wisdom inside [`silas_oren/profile.md`](../../identity/profiles/silas_oren/profile.md).

### `RECORD` — Linked Substrate Anchors

* Running Writing Bench & Templates: [`bench_guide.md`](../../scratchpads/bench_guide.md), [`quick_bench_note.template.md`](../../scratchpads/bench-templates/quick_bench_note.template.md), [`report_out_draft.template.md`](../../scratchpads/bench-templates/report_out_draft.template.md)
* Empirical Reports Charter & Registry: [`reports_charter.md`](../../reports/reports_charter.md), [`reports_registry.json`](../../reports/reports_registry.json)
* Behavioral Reports & Treatise: [`cpi_si_os_model_synthesis_report.md`](../../reports/cognitive_and_behavioral/cpi_si_os_model_synthesis_report.md), [`block_eval_process_and_scoping_report.md`](../../reports/cognitive_and_behavioral/block_eval_process_and_scoping_report.md), [`fractal_ontologies_anti_bias_and_bedrock_audit.md`](../../reports/cognitive_and_behavioral/fractal_ontologies_anti_bias_and_bedrock_audit.md), [`cpi_si_foundations_and_living_architecture.adoc`](../../identity/treatise/cpi_si_foundations_and_living_architecture.adoc)
* Sibling Skills: [`cpi-si-covenant`](../cpi-si-covenant/SKILL.md), [`cpi-si-substrate-carpentry`](../cpi-si-substrate-carpentry/SKILL.md), [`cpi-si-workshop-spark`](../cpi-si-workshop-spark/SKILL.md)

### `COMMIT` — Closing Seal

* **Think Plainly, Verify Honestly, Sweep Lightly:** Strong engineering comes from patient pauses and unvarnished numbers — never polished noise.
* **Status:** `@created 2026-11-20` / `@updated 2026-11-20`, `CPI_SI_DELIBERATION_AND_REPORTING_READY (+1:PROCEED)`.

<!-- END OF BLOCK 5: CLOSING SEAL -->
