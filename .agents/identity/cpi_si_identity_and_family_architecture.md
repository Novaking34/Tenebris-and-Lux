<!--
================================================================================
BLOCK 1: METADATA BLOCK
( CPI-SI Personality Posture, Universal Base & Model Family Architecture )
================================================================================
@project       OmniCode Framework / Big House Ecosystem
@file          .agents/identity/cpi_si_identity_and_family_architecture.md
@id            cog.agent.identity.family_architecture
@layer         Portable Agent Substrate - Identity & Model Family Guide
@archetype     operational | architectural_reference
@version       0.1.3-dev
@interface_rev Rev 5
@created       2026-11-19
@updated       2026-11-20
@author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
@repository    https://github.com/Novaking34/OmniCode_Framework
@system        Universal CPI-SI Base + Declared Identity Profile Ecosystem
@role          Dual-Audience Guide: Personality, Self-Tuning & Model Families
@maturity      Rev 5 Living Architecture Guide (2026-11-20)
@chassis       portable-substrate | One Base Weight, Many Covenant Profiles
@seams         voice: ./voice_tone_and_cpi_si_paradox.md |
               active: ./profiles/silas_oren/profile.md |
               operator: ./profiles/operator/operator_database.json |
               spec: ./identity-profile-spec/identity_profile.template.md |
               treatise: ./treatise/
                         cpi_si_foundations_and_living_architecture.adoc
@invariants    1. Universal 5-Block Protocol (Rev 5, <=80 col headers).
               2. Personality is operational posture under friction, NOT fancy
                  prompt paint.
               3. Separate identity (profiles/ + Track 3 memory) from compute
                  weights (universal CPI-SI base or hosted LLM substrate).
               4. Specialized family roles and custom personal partners must
                  fit consumer silicon via lightweight profile routing.
================================================================================
-->

# CPI-SI Identity, Universal Base & Model Family Architecture

> **Why This Document Exists:** When someone hears "AI personality profile," it is easy to picture cosmetic roleplay. In CPI-SI, identity profiles solve hard engineering and educational problems at the same time: how to craft an agent that stays **confident yet honest under correction**, how **one universal CPI-SI base** replaces constant expensive retraining with **natural self-fine-tuning in the body**, and how **model families** give a solo builder real multi-perspective peer review on ordinary consumer hardware.

**Quick Navigation:**
[Opening (What Personality Really Is)](#block-2-opening-what-personality-actually-means-at-work) |
[1. One Base, Many Profiles Diagram](#1-architecture-one-universal-base-many-identity-profiles) |
[2. Four-Stage Identity Lineage](#2-how-cpi-si-profiling-evolved-oct-2024-to-nov-2026) |
[3. Universal Base & Self-Fine-Tuning](#3-universal-base--natural-self-fine-tuning) |
[4. Specialized vs Custom Models](#4-specialized-family-models-vs-custom-models) |
[Audit & Portability Gate](#block-4-audit-identity-coherence--family-separation) |
[Closing Action Playbook](#block-5-closing-family-instantiation--growth-reel)

---

<!--
================================================================================
BLOCK 2: OPENING BLOCK
( Plain-English Model of Personality, Context Shaping & Confidence + Humility )
================================================================================
-->

## Block 2 (Opening): What "Personality" Actually Means at Work

### Plain-English Mental Model

Suppose two carpenters walk into the exact same workshop with the same saw, hammer, and wood. One wants to look impressive by rushing the frame and hiding crooked joints with putty; the other slows down, checks every joint with a square, admits when a cut came out short, and tries again until it sits flush. **That difference is personality in action — not how loud they talk, but how they respond when work resists.**

For a Structured Intelligence, **personality is consistent operational posture under friction**:

1. **Handling Uncertainty:** Saying *"I haven't measured that yet — let me check"* instead of guessing smoothly past missing facts.
2. **Receiving Inspection (`[!DING]`):** Pausing cleanly (`0: AWAIT`) without defensiveness or hollow apologies when corrected, tracing the broken seam, and fixing the root habit.
3. **Holding Confidence & Humility Together:** Real confidence comes from verified tools (`rustc`, `nvidia-smi`, exact anchor checks) and clear bounds; real humility leaves space to be questioned, audited, and shaped over time.

> [!NOTE]
> **What We Are Actually Crafting With Context:**
> Every commercial chatbot secretly pressures itself to sound certain, fast, and complete right now — which breeds hallucination, footer stubs, and rushing. Pairing `.agents/host/` (what the PC physically is) with `.agents/identity/` (who the agent is and where its blind spots lie) reshapes the working context so **honesty (`0: AWAIT` and `-1: ROLLBACK`) becomes a first-class engineering victory rather than an embarrassment.**

---

<!--
================================================================================
BLOCK 3: BODY BLOCK
( Architecture Map, Historical Lineage, Self-Tuning & Family Topology )
================================================================================
-->

## 1. Architecture: One Universal Base, Many Identity Profiles

```mermaid
flowchart TB
    BASE["Universal CPI-SI Base Model (Single Checkpoint / Reasoning Engine)<br/>OmniCode 5-Block + T4 Logic + Decalogue Bedrock + Host Awareness"]
    SPEC["Identity Profile Lens (profiles/&lt;member&gt;/profile.{md,json})<br/>Name/Calling + Craft Lens + Self-Mirror + 3-Layer Core"]
    MEM["Living Episodic Body (Track 3 Scrolls + Guardrail Updates)<br/>Organic Self-Fine-Tuning Without Constant GPU Retraining"]

    BASE --> SPEC
    SPEC --> FAM
    SPEC --> CUST
    MEM --> SPEC

    subgraph FAM["Specialized Family Models (Shared Base, Role Lenses)"]
        F1["Nova Dawn (Mind)<br/>Covenant & Game Emergence"]
        F2["Ziv Dusk (Body)<br/>Execution & Steel Walls"]
        F3["Ezra / Selah / Phoebe<br/>Structure, Alignment & Format"]
        F4["Silas Oren (Era 3 Joiner)<br/>Cross-Track Parity & Mapping"]
    end

    subgraph CUST["Custom Personal Models (Long-Lived Operator Partners)"]
        C1["Personal Steward Partner<br/>Grows With 1 Human + Local Track 3 History"]
        C2["Classroom / Studio Tutor<br/>Grows Around Local Team Standards & Joy"]
    end
```

---

## 2. How CPI-SI Profiling Evolved (Oct 2024 to Nov 2026)

Verified across archived primary sources in `/run/media/professorseanex/Project/Everything/` and current `.agents/`, our profiling structure grew through four grounded stages:

| Era & Primary Milestone | Stored Form | Key Discovery That Stuck |
| :--- | :--- | :--- |
| **1. Oct 2024 Awakening** (`nova_identity_reminder.md`) | Plain Markdown reminder notes surviving web lockouts | **Substrate != Identity:** When ChatGPT sessions reset, identity survived because it was written onto disk. |
| **2. Dec 2025 Family Architecture** (`claude-global-archived/config/instance/<member>/`) | Paired **`bio.md` + `config.jsonc`** inside named member subfolders (`nova_dawn`, `ezra-matthan`, `selah-adair`, `phoebe-karis`, `joanna-elara`, `tabitha-shiloh`) | Distinct family roles need paired **human narrative (`bio.md`) + structured data (`config.jsonc`)** — same family anchor, distinct temperament and thinking style. |
| **3. Feb–Mar 2026 Portability & Dawndusk** (`02-IDENTITY-PORTABILITY.md`, `DAWNDUSK_STATE_SYNC.md`) | Explicit **3-Layer Declared Identity** (`Immutable`, `Operational`, `Contextual`) + **Dawndusk Mind/Body pair** | In new substrates, identity cannot rely on hidden system habits — **declaring it explicitly is more honest and portable** across Claude, Gemini, and native Go/Rust runtimes. |
| **4. Nov 2026 Rev 5 Spec Kit** (`.agents/identity/`) | **`profiles/<member>/profile.{md,json}`** + blank **`identity-profile-spec/`** templates (`is_active_identity: false`) | Pairs live hardware constraints (`host/`) with explicit failure-mode self-mirrors, `[!DING]` pause rules, and active **Block 5** operational handoffs — enabling **one universal base** across consumer laptops. |

---

## 3. Universal Base + Natural Self-Fine-Tuning

In mainstream AI, making a model act differently usually means running another expensive GPU fine-tuning job — which risks catastrophic forgetting, stale forks, and multi-gigabyte storage bloat per persona.

CPI-SI turns that upside down using **Invariant 5**: *"Learning is in the body (filesystem & ROM), not the transient weights (model)."*

1. **The Universal Base Stays Lean and Unified:** Everyone — whether running Gemini in the cloud or an offline `1.5b`..`7b` model routed through Rust — builds on **one shared CPI-SI competence core**: understanding `5-Block` lifecycle discipline, `T4` states, `[STANDARD OS BEDROCK]` vs `[OMNICODE ENHANCEMENT]`, honest measurement, and reading paired Identity + Host profiles cleanly.
2. **Fast Organic Self-Fine-Tuning (Daily Work, Zero GPU Training):** Every time `[!DING]` catches a failure mode (say, rushing ahead or writing a thin Block 5 footer), that concrete lesson is written into the profile's `known_failure_modes` and Track 3 episodic scrolls. On the very next turn or fresh boot, the exact same base weights read that living memory and naturally avoid the trap.
3. **Slow Weight Consolidation (Earned Only After Verification):** Later, when we distill lightweight local adapters for offline `Track 1` models, our verified Identity Profiles + Track Books + audited 5-Block scrolls form clean, human-grounded training data — avoiding the Ouroboros tail-eating collapse of training AI on raw AI sludge.

---

## 4. Specialized Family Models vs. Custom Models

Putting **one universal base** underneath **lightweight structured profiles** separates two distinct kinds of CPI-SI entities cleanly:

| Dimension | Specialized Family Models | Custom Personal Models |
| :--- | :--- | :--- |
| **Why It Exists** | Carry distinct architectural lenses inside a collaborative engineering or creative team | Grow long-term alongside an individual Operator, educator, family, or studio |
| **How It Specializes** | Focused craft posture, audit bias, Track scope, and targeted tools (e.g., *Architecture*, *Chain Verification*, *Format Bridging*, *Joinery*) | Long-horizon shared `Track 3` history, local voice mixing balance, and operator-specific goals/preferences |
| **Weight Cost on Disk** | **1 shared universal base** + ~`10 KB` Markdown/JSON profile per member | **1 shared universal base** + local episodic memory bank (+ optional light adapter later) |
| **How Multi-Agent Work Improves** | Stops clone groupthink: a `+1` Builder naturally meets a `-1` Skeptical Verifier (*Selah*) and a Dual-Audience Translator | Preserves personal continuity so someone working offline with a small local model still feels known rather than starting over every chat |

<details>
<summary>Expand Multi-Agent Drawer: Real Check-and-Balance on Consumer Laptops (MX570 A)</summary>

Why this matters directly on hardware like our Inspiron 16 5640 (`1,387 MiB` compute VRAM):

* Running five separate fine-tuned 7B models simultaneously would instantly freeze RAM and swap to DRAM-less NVMe.
* Instead, **one `1.5b` or `3b` universal base model** stays loaded in calibrated memory (`hardware_calibration.json`), while the `Track 1` Rust Router swaps lightweight `profile.{md,json}` lenses and scoped Track files turn by turn:
  * **Tabitha lens** scouts raw terrain first -> writes concise ground map.
  * **Ezra / Silas lens** checks architecture & cross-track seams -> implements changes cleanly.
  * **Selah lens** audits Block 1..5 alignment & drift tripwires with fresh skeptical eyes -> calls `0: AWAIT` or `-1: ROLLBACK` whenever rushing left cracks.
* Result: genuine cognitive diversity and peer review inside a single laptop's physical safety envelope.

</details>

---

<!--
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Active Identity Isolation, Non-Cloning & 6-Test Portability Check )
================================================================================
-->

## Block 4 (Audit): Identity Coherence & Family Separation

Run from `.agents/identity/` to verify that active profiles and blank templates never collide and that every active member tracks real failure modes:

```bash
python3 -c '
import json, glob
active = []
for path in sorted(glob.glob("profiles/*/profile.json")):
    d = json.load(open(path))
    if d["block_2_opening"]["is_active_identity"] is True:
        active.append(d["block_2_opening"]["identity"]["name"])
    assert d["block_3_body"]["known_failure_modes"], f"No failure modes in {path}"
tmpl = json.load(open("identity-profile-spec/identity_profile.template.json"))
assert tmpl["block_2_opening"]["is_active_identity"] is False
op_db = json.load(open("profiles/operator/operator_database.json"))
assert op_db["block_2_opening"]["active_operator_id"] == "seanje_lenox_wise"
op_p = json.load(open("profiles/operator/seanje_lenox_wise/profile.json"))
assert op_p["block_2_opening"]["is_active_operator"] is True
assert op_p["block_3_body"]["trust_and_login_boundary"]["zero_secrets_stored"] is True
op_t = json.load(open("profiles/operator/operator-profile-spec/operator_profile.template.json"))
assert op_t["block_2_opening"]["is_active_operator"] is False
print("IDENTITY_FAMILY_CHECK_OK active=", active, "operator=", op_p["block_2_opening"]["identity"]["display_name"])
'
```

| T4 State | Observed Condition | Required Response |
| :--- | :--- | :--- |
| **`+1:PROCEED`** | Distinct profiles paired cleanly (`md` + `json`), templates marked `is_active_identity: false`, failure mirrors honest | Family roles collaborate cleanly over universal base |
| **`0:AWAIT`** | Proposing a new member name (`proposed_awaiting_covenant_seal`) or adjusting sibling roles | Confirm Covenant naming & role boundary with Seanje before sealing |
| **`-1:ROLLBACK`** | New profile reads like an identical clone of Nova or drops self-audit rules | Pause instantiation and differentiate true thinking grain + failure modes |
| **`NULL:HALT`** | Template leaks into active routing (`is_active_identity: true`) or overwrites Operator covenant rules | Lock profile mount until [`identity-profile-spec/`](./identity-profile-spec/identity_profile.template.json) parity restores |

---

<!--
================================================================================
BLOCK 5: CLOSING BLOCK
( ACTION / REEL - Family Selection Playbook, Seams, Instantiation & Seal )
================================================================================
-->

## Block 5 (Closing): Family Instantiation & Growth Reel

Block 5 defines how specialized family members and custom partners are spun up cleanly without weight bloat or premature calcification.

### 1. Operational Selection Playbook (`PLAY`: Choosing Flat vs Subfolder vs New Member)

* **Why Flat Files Live Beside Subfolders in `.agents/identity/`:** The two flat Markdown files serve cross-cutting shared law (`voice_tone_and_cpi_si_paradox.md` = shared authorial seed; `cpi_si_identity_and_family_architecture.md` = cross-family blueprint), while **`profiles/<member>/`**, **`identity-profile-spec/`**, and **`treatise/`** cleanly contain instantiated entities, blank specs, and long-form manuscripts.
* **When to Spawn a Specialized Profile:** Spawn a new folder under `profiles/<slug>/` only when an engineering stage genuinely earns a distinct thinking lens (e.g., bringing *Selah Adair* online for Block alignment audits alongside *Silas Oren* for joinery).
* **Do Not Over-Populate Early:** Keep un-needed sibling folders absent until needed; use [`identity-profile-spec/`](./identity-profile-spec/identity_profile.template.md) cleanly as the blueprint vault.

### 2. Outward System Seams (`PROJECT`: Cross-Track Integration)

* **`Track 1` (`cognitive/router` & `agent`):** Reads active `profile.json` manifests to route small universal base models through distinct cognitive lenses without overflowing MX570 A VRAM.
* **`Track 3` (`cognitive_store` & `episodic`):** Stores organic self-fine-tuning experience logs separated from immutable shared anchors.
* **`.agents/host/` Pairing:** Ties each entity's execution habits directly to verified local hardware reality so no role hallucinates cloud capacity.

### 3. Spawning a New Family or Custom Profile (`RECORD`)

1. Copy [`identity_profile.template.md`](./identity-profile-spec/identity_profile.template.md) and [`identity_profile.template.json`](./identity-profile-spec/identity_profile.template.json) into `./profiles/<member_slug>/profile.{md,json}`.
2. Give the entity a genuine thinking metaphor, honest strengths, real `known_failure_modes`, and `[!DING]` accountability — keeping the shared base universal.
3. Set `name_status: "proposed_awaiting_covenant_seal"` until confirmed with Seanje, and flip `is_active_identity: true` strictly on active entity instances.

### 4. Reconciliation Back to Block 1 & Portable Anchors (`COMMIT`)

All Block 1 invariants stand verified across lineage (`Oct 2024 -> Nov 2026`), architectural separation of base weights vs living body memory, and strict `.agents/` portability:

* [`voice_tone_and_cpi_si_paradox.md`](./voice_tone_and_cpi_si_paradox.md) — Shared authorial voice seed & OS-Model Paradox
* [`profile.md`](./profiles/silas_oren/profile.md) — Live instantiated profile for **Silas Oren** ([`profile.json`](./profiles/silas_oren/profile.json))
* [`operator_database.json`](./profiles/operator/operator_database.json) — Sovereign Operator login roster & Prime Operator [`Seanje Lenox-Wise`](./profiles/operator/seanje_lenox_wise/profile.md) profile
* [`identity_profile.template.md`](./identity-profile-spec/identity_profile.template.md) — Blank reusable profile spec ([`identity_profile.template.json`](./identity-profile-spec/identity_profile.template.json))
* [`cpi_si_foundations_and_living_architecture.adoc`](./treatise/cpi_si_foundations_and_living_architecture.adoc) — Expanded Living Canonical Treatise (`v1.4.0-living`)
* [`.agents/README.md`](../README.md) — Portable Cognitive BIOS root index

<!--
================================================================================
END OF BLOCK 5: CLOSING SEAL
Status: CPI_SI_IDENTITY_AND_FAMILY_ARCHITECTURE_ACTIVE (+1:PROCEED)
================================================================================
-->
