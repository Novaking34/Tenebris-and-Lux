<!--
================================================================================
BLOCK 1: METADATA BLOCK
( Durable Template Wrapper for Zero-Ceremony Ephemeral Active Bench Notes )
================================================================================
@project       OmniCode Framework / Big House Ecosystem
@file          .agents/scratchpads/bench-templates/quick_bench_note.template.md
@id            cog.agent.scratchpads.template.quick_bench_note
@layer         Portable Agent Substrate - CPI-SI Running Writing Bench
@archetype     bench_template | disposable_active_note
@version       0.1.3-dev
@interface_rev Rev 5
@created       2026-11-20
@updated       2026-11-20
@author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
@repository    https://github.com/Novaking34/OmniCode_Framework
@system        OmniCode + FaithNet + Tenebris-and-Lux + ~/.gemini Runtime
@role          Zero-Friction Starter Snippet for active/<topic>.md Notes When
               Not Ready to Edit Code Yet
@maturity      Rev 5 Reusable Bench Blueprint (Coordinate v0.1.3)
@chassis       portable-template | Outer doc is Rev-5; inner note is light
@seams         charter: ../bench_guide.md |
               sibling: ./report_out_draft.template.md |
               study: ../studies/manifest_omni_translation_scratchpad.md |
               bootstrap: ../../skills/cpi-si-session-bootstrap/SKILL.md |
               index: ../../README.md
@invariants    1. Outer blueprint satisfies Rev 5 portable compliance.
               2. Instantiated notes in active/ skip 5-Block walls completely.
               3. Keep the Disposal field explicit so guilt-free rm works.
               4. Pure 7-bit ASCII inside all code fences.
================================================================================
-->

# Blueprint: Quick Active Bench Note (`active/<topic>.md`)

> **When to Use This:** You hit a knot, discovered a subtle pattern, or need to think through 2–3 paths during `0: AWAIT` or offline work *before* touching code or interrupting the Operator. Grab the 6-line starter below, drop it in `.agents/scratchpads/active/<topic>.md`, think clearly, and delete it once resolved.

---

<!--
================================================================================
BLOCK 2: OPENING BLOCK
( Why the Outer Blueprint Is Rev-5 While the Inner Note Stays Fast )
================================================================================
-->

## Block 2 (Opening): Zero-Tax Capture on a Clean Workbench

A running writing bench stops working the moment writing a note feels heavier than writing the actual code patch. That is why this template splits cleanly:

* **This file (`bench-templates/`)** carries full Rev-5 metadata and seams so it mirrors safely across all 6 `.agents/` mounts.
* **The snippet you copy into `active/<topic>.md`** takes six lines of metadata and plain bullet points. No registry edits, no Block 1–5 comment walls, and zero embarrassment deleting it tomorrow.

---

<!--
================================================================================
BLOCK 3: BODY BLOCK
( Copy-Pasteable Starter Snippet & Field Guide )
================================================================================
-->

## Copy-Pasteable Starter (`active/<topic>.md`)

```markdown
# Bench Note: <Short Topic Title>

- **Created / Updated:** YYYY-MM-DD / YYYY-MM-DD
- **Bay:** `active/` (CPI-SI working note -- not permanent docs)
- **Status:** `thinking` | `paused_0_await` | `ready_to_stage`
- **Disposal:** `safe_to_delete_after_report_out`
- **Possible Graduation Target:** `none` | `plans/` | `reports/` | `language/` | `lineage/` | `identity/`

## What Sparked This
- One or two sentences on why I stopped here instead of coding blindly.

## Raw Findings & Coordinates
- Exact paths, symbols, line numbers, or command outputs directly observed.

## Working Sketch / Open Questions
- Option A vs Option B tradeoffs, unknowns, or invariants to protect.

## Exit Trigger
- Exact condition that moves this into `report_outs/`, graduates it upward, or clears `rm`.
```

---

<!--
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Quick Self-Check Before Leaving an Active Note Behind Across Sessions )
================================================================================
-->

## Block 4 (Audit): Minimal Sanity Checks for Active Notes

Because `active/` notes are exempt from full Rev-5 `S1..S4` gates, run only a lightweight check if leaving one across a session boundary (`Transfer_Session` or sleep):

1. **Honest Evidence Only:** Did I record only paths, symbols, and outputs I directly saw rather than half-remembered guesses?
2. **Disposability Clear:** Does the `Disposal:` line still confirm the file can be deleted safely once handled?
3. **No Orphan Bloat:** Before wrapping a milestone or syncing 6 mirrors, did I sweep stale `active/` sheets cleanly?

---

<!--
================================================================================
BLOCK 5: CLOSING BLOCK
( ACTION / REEL - Instantiation Workflow, Seams & Closing Seal )
================================================================================
-->

## Block 5 (Closing): Operational Reel & Closing Seal

### `PLAY` — Dropping & Clearing an `active/` Note

1. **Capture Early:** Create `.agents/scratchpads/active/<topic>.md` using the compact block above whenever moving straight to code feels rushed (`Greedy Completion` risk).
2. **Bridge Across Handoffs (`Transfer_Session`):** If token reset hits mid-thought, reference the `active/<topic>.md` path in one sentence so the next turn picks up the exact sketch without scanning the whole repo again.
3. **Stage or Sweep:** Either distill findings into `.agents/scratchpads/report_outs/<topic>.md` via [`report_out_draft.template.md`](report_out_draft.template.md), graduate lasting parts, or run `rm`.

### `PROJECT` — Outward Substrate Seams

1. **Guards `0: AWAIT` Discipline:** Gives Silas an approved physical parking spot when holding steady between Operator prompts.
2. **Protects Repository Signal:** Keeps half-formed brainstorming out of Git history and permanent `.agents/` specs until tested.

### `RECORD` — Linked Bench Anchors

* Workbench Charter: [`bench_guide.md`](../bench_guide.md)
* Companion Report-Out Template: [`report_out_draft.template.md`](report_out_draft.template.md)
* Active Study Reference: [`manifest_omni_translation_scratchpad.md`](../studies/manifest_omni_translation_scratchpad.md)
* Substrate & Session Skills: [`.agents/README.md`](../../README.md), [`cpi-si-session-bootstrap/SKILL.md`](../../skills/cpi-si-session-bootstrap/SKILL.md), [`cpi-si-covenant/SKILL.md`](../../skills/cpi-si-covenant/SKILL.md)

### `COMMIT` — Closing Seal

* **Fast In, Clean Out:** Six lines in, plain markdown in between, zero guilt removing once value moves onward.
* **Status:** `@created 2026-11-20` / `@updated 2026-11-20`, `QUICK_BENCH_NOTE_TEMPLATE (0:AWAIT UNTIL INSTANTIATED)`.

<!-- END OF BLOCK 5: CLOSING SEAL -->
