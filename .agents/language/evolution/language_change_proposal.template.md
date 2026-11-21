<!--
================================================================================
BLOCK 1: METADATA BLOCK
( OmniCode Language Change Proposal Template - Add / Modify / Remove RFC Spec )
================================================================================
@project       OmniCode Framework / Big House Ecosystem
@file          .agents/language/evolution/
               language_change_proposal.template.md
@id            cog.agent.language.change_proposal_template
@layer         Portable Agent Substrate - Living Language Evolution Spec
@archetype     template | language_change_proposal
@version       0.1.3-dev
@interface_rev Rev 5
@created       YYYY-MM-DD
@updated       YYYY-MM-DD
@author        {{PROPOSER_AND_COVENANT_PARTNER}}
@repository    https://github.com/Novaking34/OmniCode_Framework
@system        OmniCode Living Grammar (Tier 0 / Tier 1 / Tier 2)
@role          Structured Blueprint to Weigh Language Add, Modify, or Remove
@maturity      Rev 5 Reusable Change Template (0:AWAIT Until Submitted)
@chassis       ascii-first | Zero-Churn Deliberation Over Impulse Lock-In
@seams         charter: ../language_living_charter.md |
               registry: ../language_registry.json |
               ledger: ./quarantine_and_change_ledger.md |
               index: ../../README.md
@invariants    1. Universal 5-Block Protocol (Rev 5, <=80 col headers).
               2. Never promote into Tier 0/1 from thin hunch alone; test inside
                  Tier 2 Quarantine or a real .tape / .omni file first.
               3. Evaluate all 5 Gate questions honestly — including clean
                  composition via existing tokens or clean retirement.
               4. Keep plain-ASCII keyboard accessibility unconditional.
================================================================================
-->

# Language Change Proposal (`{{PROPOSAL_ID}}: {{TITLE}}`)

> **Template Purpose:** Copy this blueprint into `./` (named `RFC_<YYYY-MM-DD>_<slug>.md`) or walk through its five gates live whenever working code shows that an OmniCode **addition**, **modification**, or **removal/retirement** deserves real deliberation.

**Quick Navigation:**
[Opening (Problem & Intent)](#block-2-opening-what-friction-triggered-this-proposal) |
[1. Change Specification (Before vs After)](#1-proposed-change-specification) |
[2. The 5-Question Gate Audit](#2-the-5-question-gate-evaluation) |
[Audit & Compiler Impact](#block-4-audit-falsification--compiler-test-checks) |
[Closing Decision Reel](#block-5-closing-decision-handoff--ledger-reel)

---

<!--
================================================================================
BLOCK 2: OPENING BLOCK
( Change Classification, Origin Story & Why Existing Grammar Struggled )
================================================================================
-->

## Block 2 (Opening): What Friction Triggered This Proposal

* **Proposal Action:** `{{ADD | MODIFY | REMOVE}}`
* **Target Stability Tier:** `{{tier_0_bedrock | tier_1_working | tier_2_quarantine}}` -> `{{NEW_TIER_OR_RETIRED}}`
* **Origin Evidence (`Track` / `.tape` / `.omni` file):** `{{FILE_OR_EXPERIENCE_THAT_REVEALED_NEED}}`

> [!NOTE]
> **Plain-English Explanation:** Explain in two plain sentences what felt clunky, ambiguous, missing, or redundant while writing real code or teaching someone to read OmniCode — and why touching the grammar fixes the root instead of papering over it.

---

<!--
================================================================================
BLOCK 3: BODY BLOCK
( Exact Before / After Forms, 5-Question Gate & Reversibility Plan )
================================================================================
-->

## 1. Proposed Change Specification

### Before vs. After (`.tape` and `.omni` Examples)

```text
# BEFORE (Current Grammar):
{{BEFORE_EXAMPLE}}

# AFTER (Proposed Change):
{{AFTER_EXAMPLE}}
```

### Registry Entry Diff (`../language_registry.json`)

* **Target Section:** `{{e.g. block_3_body.tier_1_working.keyword_groups}}`
* **Exact Token(s) Touched:** `{{TOKENS_LIST}}`

---

## 2. The 5-Question Gate Evaluation

| Gate Question | Honest Answer (`Pass / Caution / Fail`) | Evidence & Reasoning |
| :--- | :---: | :--- |
| **1. Irreducibility (Gall's Law):** Could existing tokens compose this cleanly, or is it truly atomic? | `{{STATUS_Q1}}` | `{{EVIDENCE_Q1}}` |
| **2. Standard-Keyboard Floor:** 100% plain 7-bit printable ASCII without Alt-key tricks? | `{{STATUS_Q2}}` | `{{EVIDENCE_Q2}}` |
| **3. Zero Semantic Collision:** Clean separation from existing symbols (no `%` vs `MOD` overlap, etc.)? | `{{STATUS_Q3}}` | `{{EVIDENCE_Q3}}` |
| **4. Compiler & Readability Payoff:** Improves both `.tape -> .omni` determinism and read-aloud joy? | `{{STATUS_Q4}}` | `{{EVIDENCE_Q4}}` |
| **5. Reversibility / Retirement Memory:** Trials cleanly in Quarantine, or leaves clean retirement trail? | `{{STATUS_Q5}}` | `{{EVIDENCE_Q5}}` |

<details>
<summary>Expand Impact Drawer: Compiler Lexer, Existing Tapes &amp; Teaching Impact</summary>

* **Existing `.omni` / `.tape` Files Affected:** `{{AFFECTED_FILES}}`
* **Rust Exoskeleton / Lexer Changes (`Track 1` / `Track 7`):** `{{LEXER_IMPACT}}`
* **Rollback Plan if Field Trial Fails (`-1:ROLLBACK`):** `{{ROLLBACK_PLAN}}`

</details>

---

<!--
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Verification Against Existing Registry, Flipped Pairs & Case Registers )
================================================================================
-->

## Block 4 (Audit): Falsification & Compiler Test Checks

Before voting `+1:PROCEED` on this proposal, run against `../language_registry.json`:

```bash
python3 -c '
import json
reg = json.load(open("../language_registry.json"))
print("Active Tier 1 keys:", list(reg["block_3_body"]["tier_1_working"].keys()))
# Verify proposed token does not silently duplicate any retired or Tier-1 token here
'
```

| T4 Verdict | Meaning for This Proposal | Next Step |
| :--- | :--- | :--- |
| **`+1:PROCEED`** | All 5 Gates clear AND real `.tape`/`.omni` test succeeds | Promote / modify / retire in `language_registry.json` & log in ledger |
| **`0:AWAIT`** | Needs more real programs in `tier_2_quarantine` before deciding | Keep concept quarantined cleanly without hard-locking |
| **`-1:ROLLBACK`** | Clashes with an existing operator, breaks keyboard floor, or bloats vocabulary | Reject cleanly and record why in `quarantine_and_change_ledger.md` |
| **`NULL:HALT`** | Undermines `T4` truth, order sensitivity, or `5-Block` integrity | Hard veto immediately |

---

<!--
================================================================================
BLOCK 5: CLOSING BLOCK
( ACTION / REEL - Deliberation Playbook, Spec Updates, Memory & Seal )
================================================================================
-->

## Block 5 (Closing): Decision Handoff & Ledger Reel

### 1. Deliberation Playbook (`PLAY`: Before Sealing Any Change)

* **Let Real Code Vote First:** Never approve a keyword or symbol change just because it looks neat in isolation — write at least one realistic `.tape` and `.omni` snippet testing both readable flow and edge-case failure.
* **No Pride Over Retirements:** Removing or reshaping a redundant symbol makes the language stronger, not weaker.

### 2. Outward Spec Seams (`PROJECT`: If Proposal Earns `+1:PROCEED`)

* Update [`language_registry.json`](../language_registry.json) first so the machine SSOT stays authoritative.
* Update portable [`SYNTAX.md`](../reference/SYNTAX.md) / [`KEYWORD.md`](../reference/KEYWORD.md) alongside root repository `docs/`.
* Sync Track 1 parser/validator tables and Track 2 lexicon examples.

### 3. Permanent Memory Recording (`RECORD`)

Append one timestamped entry (`@created` / verdict / rationale) inside [`quarantine_and_change_ledger.md`](./quarantine_and_change_ledger.md) regardless of whether the outcome is **Adopted**, **Modified**, or **Retired**.

### 4. Reconciliation & Portable Anchors (`COMMIT`)

* [`language_living_charter.md`](../language_living_charter.md) — Three Stability Tiers & Living Governance
* [`language_registry.json`](../language_registry.json) — Machine-readable grammar SSOT
* [`quarantine_and_change_ledger.md`](./quarantine_and_change_ledger.md) — Chronological ledger of experiments & retirements
* [`.agents/README.md`](../../README.md) — Portable Cognitive BIOS root index

<!--
================================================================================
END OF BLOCK 5: CLOSING SEAL
Status: LANGUAGE_CHANGE_PROPOSAL_TEMPLATE (0:AWAIT UNTIL SUBMITTED)
================================================================================
-->
