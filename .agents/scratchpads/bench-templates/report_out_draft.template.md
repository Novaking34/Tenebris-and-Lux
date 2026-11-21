<!--
================================================================================
BLOCK 1: METADATA BLOCK
( Durable Template Wrapper for Staging Operator-Ready Report-Outs )
================================================================================
@project       OmniCode Framework / Big House Ecosystem
@file          .agents/scratchpads/bench-templates/report_out_draft.template.md
@id            cog.agent.scratchpads.template.report_out_draft
@layer         Portable Agent Substrate - CPI-SI Running Writing Bench
@archetype     bench_template | staged_report_out_draft
@version       0.1.3-dev
@interface_rev Rev 5
@created       2026-11-20
@updated       2026-11-20
@author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
@repository    https://github.com/Novaking34/OmniCode_Framework
@system        OmniCode + FaithNet + Tenebris-and-Lux + ~/.gemini Runtime
@role          Zero-Ceremony Staging Shape in report_outs/<topic>.md Before
               Delivering Findings to Seanje
@maturity      Rev 5 Reusable Bench Blueprint (Coordinate v0.1.3)
@chassis       portable-template | Outer doc is Rev-5; staged draft is crisp
@seams         charter: ../bench_guide.md |
               sibling: ./quick_bench_note.template.md |
               reports: ../../reports/reports_charter.md |
               plans: ../../plans/plans_charter.md |
               index: ../../README.md
@invariants    1. Outer blueprint satisfies Rev 5 portable compliance.
               2. Staged drafts in report_outs/ cite only verified runs/files.
               3. Clear Sweep vs Graduate decision follows every delivery.
               4. Pure 7-bit ASCII inside all code fences.
================================================================================
-->

# Blueprint: Report-Out Draft (`report_outs/<topic>.md`)

> **When to Use This:** You finished digging through files, logs, or design questions in `active/` and want to stage a crisp, honest report-out ready for Seanje — either right away or right after a clean session break — without turning a temporary briefing into a formal `reports/` monograph prematurely.

---

<!--
================================================================================
BLOCK 2: OPENING BLOCK
( Separating Working Exploration From Honest Delivery )
================================================================================
-->

## Block 2 (Opening): Why Stage Report-Outs Separately From `active/`?

Working investigations are messy: dead ends, five grep outputs, three alternative hypotheses. Seanje needs the **distilled truth**: what actually sits on disk, what verified commands returned, what options open up next, and which decision waits on Operator clearance. Staging inside `report_outs/` lets Silas sharpen that briefing into plain workshop speech *first*, deliver it cleanly, and then either `rm` the sheet or graduate its verified core upward.

---

<!--
================================================================================
BLOCK 3: BODY BLOCK
( Copy-Pasteable Starter Snippet & Field Guide )
================================================================================
-->

## Copy-Pasteable Starter (`report_outs/<topic>.md`)

```markdown
# Report-Out Draft: <Short Topic Title>

- **Created / Updated:** YYYY-MM-DD / YYYY-MM-DD
- **Bay:** `report_outs/` (staged delivery -- delete or graduate once delivered)
- **Audience:** `Operator (Seanje)`
- **Disposal:** `sweep_after_delivery` | `graduate_to:<plans|reports|language|lineage|identity>`
- **Verified Sources / Commands:** `<exact paths and checks run>`

## Plain-English Takeaway
- Direct answer first: 2-4 crisp sentences in steady Silas workshop voice.

## Verified Findings (Observed Only)
- Bullet exact file paths, line ranges, symbol names, and observed command outputs.
- Explicitly flag anything blocked, unverified, or skipped.

## Options & Tradeoffs
- **Option 1 (Recommended):** What it does, upside, cost/risk.
- **Option 2:** Alternative path and when it fits better.

## Post-Delivery Action: Sweep or Graduate
- After reporting out to Seanje: `rm` this draft (or promote lasting core to `<target>` first).
```

---

<!--
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Pre-Delivery Honesty Check Before Handing Report-Out to Operator )
================================================================================
-->

## Block 4 (Audit): Pre-Delivery Honesty Check

Before converting a staged `report_outs/<topic>.md` into your live reply to Seanje:

1. **Zero Extrapolation:** Does every metric, path, and command result match tool output directly observed in this session?
2. **Voice Check:** Is the summary written in steady builder speech without stiff filler or flattery?
3. **Clear Next Decision:** Does the briefing make it obvious whether Seanje needs to choose an option (`0: AWAIT`) or clear execution (`+1: PROCEED`)?

---

<!--
================================================================================
BLOCK 5: CLOSING BLOCK
( ACTION / REEL - Delivery Loop, Graduation Seams & Closing Seal )
================================================================================
-->

## Block 5 (Closing): Operational Reel & Closing Seal

### `PLAY` — Staging, Delivering & Sweeping

1. **Synthesize From `active/`:** Pull only verified takeaways into `.agents/scratchpads/report_outs/<topic>.md`.
2. **Deliver to Seanje:** Present the distilled summary in chat with clickable `file://` links to canonical `.agents/` or repo paths (rather than making long-term links depend on a disposable draft).
3. **Execute Disposal Choice:** Remove stale `active/` and `report_outs/` sheets via `rm`, or graduate lasting findings into formal files first.

### `PROJECT` — Outward Graduation Seams

1. **Routes Strategic Roadmaps Upward:** Actionable architectural paths graduate into [`plans/`](../../plans/plans_charter.md) under [`plans_registry.json`](../../plans/plans_registry.json).
2. **Routes Empirical Benchmarks Upward:** Repeatable measurements or LLM shifts graduate into [`reports/`](../../reports/reports_charter.md) under [`reports_registry.json`](../../reports/reports_registry.json).

### `RECORD` — Linked Bench Anchors

* Workbench Charter: [`bench_guide.md`](../bench_guide.md)
* Companion Active Note Template: [`quick_bench_note.template.md`](quick_bench_note.template.md)
* Active Study Reference: [`manifest_omni_translation_scratchpad.md`](../studies/manifest_omni_translation_scratchpad.md)
* Substrate & Governance Seams: [`.agents/README.md`](../../README.md), [`cpi-si-covenant/SKILL.md`](../../skills/cpi-si-covenant/SKILL.md)

### `COMMIT` — Closing Seal

* **Truth Staged, Bench Kept Clean:** Deliver clearly, then sweep or promote so the workbench stays light for the next pause.
* **Status:** `@created 2026-11-20` / `@updated 2026-11-20`, `REPORT_OUT_DRAFT_TEMPLATE (0:AWAIT UNTIL INSTANTIATED)`.

<!-- END OF BLOCK 5: CLOSING SEAL -->
