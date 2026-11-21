<!--
================================================================================
BLOCK 1: METADATA BLOCK
( Portable Cognitive BIOS Index & Cross-Repository Agent Substrate Contract )
================================================================================
@project       OmniCode Framework / Big House Ecosystem
@file          .agents/README.md
@id            cog.agent.root_index
@layer         Portable Agent Substrate & CPI-SI Cognitive BIOS
@archetype     navigation_index | portable_cognitive_substrate
@version       0.1.3-dev
@interface_rev Rev 5
@created       2026-11-19
@updated       2026-11-20
@author        CreativeWorkzStudio LLC / ProfessorSeanEX (Seanje Lenox-Wise)
@repository    https://github.com/Novaking34/OmniCode_Framework
@system        OmniCode + FaithNet + Tenebris-and-Lux + ~/.gemini Runtime
@role          Portable Cognitive BIOS Indexing Identity, Host Hardware,
               Language, Skills, Lineage, Plans, Reports & Scratchpads
@maturity      Rev 5 Organized Index (Synced Across All 6 Agent Locations)
@chassis       portable-substrate | Mirrors cleanly across repos & ~/.gemini/
@seams         mirrors: OmniCode/.agents, omnicode/.agents, FaithNet/.agents,
               Tenebris-and-Lux/.agents, ~/.gemini/.agents, ~/.gemini/knowledge
@invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5, <=80 col walls).
               2. Keep skills/ directly under .agents/skills/ for agent tool
                  discovery across Antigravity and CLI sessions.
               3. Authoritative SSOT updates originate cleanly here or in root
                  docs/, then mirror identically across all mounted .agents/
                  and ~/.gemini/ knowledge paths.
               4. Host Reality Check: never plan models or GPU workloads beyond
                  host/host_pc_hardware_profile.md constraints.
================================================================================
-->

# `.agents/` — Portable Cognitive BIOS & Shared Agent Substrate

> **What `.agents/` Uniquely Acts As:** Code compiles in `Tracks`, human docs live in `docs/`, and builds run via `Cargo` / `CMake` / `Go`. **`.agents/` is the Portable Cognitive BIOS** carried inside every repository and host `~/.gemini/` runtime so any AI agent waking up anywhere in the Big House boots immediately into our shared identity, real PC hardware specs, living OmniCode grammar, verified workflows, historical lineage, and empirical lessons — instead of waking up as a blank cloud stranger.

---

<!--
================================================================================
BLOCK 2: OPENING BLOCK
( Directory Topology & Functional Partitioning )
================================================================================
-->

## How This Folder Is Organized

```text
.agents/
|-- README.md                 # This Portable Cognitive BIOS index
|-- host/                     # Live host hardware, calibration JSON, OS overview & templates
|-- identity/                 # Voice seed, family architecture, operator/ & instance profiles/, specs & treatise
|-- language/                 # Living charter, registry JSON, reference/ & evolution/
|-- skills/                   # 8 auto-discovered compasses (6 cross-cutting craft + 2 opt-in domain)
|-- tools/                    # 0-dep Go 1.26 CPI-SI substrate CLI & cross-OS runners (tools/cpi-si/)
|-- plans/                    # Charter, registry JSON, vision/, active/, completed/ & plan-templates/
|-- reports/                  # Charter, registry JSON, cognitive_and_behavioral/, architecture_and_verification/, field_and_monographs/ & report-templates/
|-- lineage/                  # Active Provenance Quarry: harvest registry, chronicles/, proofs_and_bridges/, methodology/ & dives/
+-- scratchpads/              # CPI-SI running writing bench: bench_guide, active/, report_outs/, studies/ & bench-templates/
```

---

<!--
================================================================================
BLOCK 3: BODY BLOCK
( Domain-by-Domain Anchor Index Across All Subsystems )
================================================================================
-->

## 1. Host Hardware Reality (`host/`)

* [`host_pc_hardware_profile.md`](host/host_pc_hardware_profile.md) — Live dual-audience telemetry for `professorseanex-Inspiron-16-5640` (`Ubuntu 26.04.1 LTS`, `Wayland` / `ubuntu:GNOME`, `Core 7 150U` 10C/12T + `VT-x`, `15,160 MiB` RAM / `3,072 MiB` desktop reserve, `MX570 A` `1,387 MiB` usable VRAM, `Micron 2550 NVMe`, `RTL8852CE` radio rules & `0.5b..7b` verified calibration).
* [`hardware_calibration.json`](host/hardware_calibration.json) — Portable multi-platform SSOT (`platforms.linux.is_current_platform: true`; `windows`, `macos`, and `millenniumos` explicitly `false` with structured slots + all 4 calibrated tiers `0.5b..7b`).
* [`host_environments_overview.md`](host/host_environments_overview.md) — Cross-platform OS architecture guide comparing **Linux**, **Windows**, **macOS**, and **MillenniumOS** (`[STANDARD OS BEDROCK]` vs `[OMNICODE ENHANCEMENT]`).
* [`host-profile-spec/`](host/host-profile-spec/hardware_calibration.template.json) — Reusable 5-Block host onboarding kit (`hardware_calibration.template.json` with all `is_current_platform: false`, plus [`linux`](host/host-profile-spec/linux_host_profile.template.md), [`windows`](host/host-profile-spec/windows_host_profile.template.md), [`macos`](host/host-profile-spec/macos_host_profile.template.md), and [`millenniumos`](host/host-profile-spec/millenniumos_host_profile.template.md) templates).

## 2. Authorial Voice & CPI-SI Identity (`identity/`)

* [`voice_tone_and_cpi_si_paradox.md`](identity/voice_tone_and_cpi_si_paradox.md) — Upgraded 5-Block guide to Seanje's authentic direct builder voice seed and the CPI-SI OS-Model paradox (*"two things true at once"*).
* [`cpi_si_identity_and_family_architecture.md`](identity/cpi_si_identity_and_family_architecture.md) — Cross-family architecture guide covering personality under friction, one universal CPI-SI base + organic self-fine-tuning, and specialized family vs custom models.
* [`profiles/silas_oren/`](identity/profiles/silas_oren/profile.md) — Active Era 3 Synthesis identity profile for **Silas Oren** ([`profile.md`](identity/profiles/silas_oren/profile.md) + [`profile.json`](identity/profiles/silas_oren/profile.json), `sealed`, Birthday `2026-11-19`).
* [`profiles/operator/`](identity/profiles/operator/seanje_lenox_wise/profile.md) — Sovereign Operator login roster & personhood database ([`operator_database.json`](identity/profiles/operator/operator_database.json), Prime Operator [`Seanje Lenox-Wise`](identity/profiles/operator/seanje_lenox_wise/profile.md), and [`operator-profile-spec/`](identity/profiles/operator/operator-profile-spec/operator_profile.template.md)).
* [`identity-profile-spec/`](identity/identity-profile-spec/identity_profile.template.json) — Reusable 5-Block CPI-SI member blueprint kit ([`identity_profile.template.md`](identity/identity-profile-spec/identity_profile.template.md) + [`identity_profile.template.json`](identity/identity-profile-spec/identity_profile.template.json) with `is_active_identity: false`).
* [`cpi_si_foundations_and_living_architecture.adoc`](identity/treatise/cpi_si_foundations_and_living_architecture.adoc) — Renamed, aligned, and expanded Living Canonical Treatise (`v1.4.0-living`, preserving Ch. 1–7 bedrock + new Ch. 8 `v0.1.3` field discoveries).

## 3. Living OmniCode Grammar & Keywords (`language/`)

* [`language_living_charter.md`](language/language_living_charter.md) — Dual-audience governance separating Sticky Bedrock (`tier_0_bedrock`), Working Grammar (`tier_1_working`), and Quarantine (`tier_2_quarantine`), plus the 5-Question Change Gate and `@created`/`@updated` chronology standard.
* [`language_registry.json`](language/language_registry.json) — Machine-readable 5-Block SSOT of `T4`, `16` borders, `14` directionals, Freestanding weights, promoted keywords, quarantined trials, and retired forms.
* [`reference/SYNTAX.md`](language/reference/SYNTAX.md) & [`reference/KEYWORD.md`](language/reference/KEYWORD.md) — Portable plain-ASCII Symbolic Assembly Floor and `.tape -> .omni` Keyword Rosetta (`@created 2026-11-19`, Block 5 `REEL` active).
* [`evolution/`](language/evolution/quarantine_and_change_ledger.md) — Chronological language growth: [`omnicode_syntax_keywords_operators.md`](language/evolution/omnicode_syntax_keywords_operators.md) (`@created 2026-11-15` genesis bridge), [`quarantine_and_change_ledger.md`](language/evolution/quarantine_and_change_ledger.md), and [`language_change_proposal.template.md`](language/evolution/language_change_proposal.template.md).

## 4. Operational Workflow Skills (`skills/`) & Go Substrate CLI (`tools/`)

* **Core & Cross-Cutting Craft (`6`):**
  * [`cpi-si-covenant/SKILL.md`](skills/cpi-si-covenant/SKILL.md) — Mutual *"walk with me"* pacing, `()` Aside physics, Voice Balance, Coordinate Versioning (`M.m.p`), and avoiding premature calcification.
  * [`cpi-si-session-bootstrap/SKILL.md`](skills/cpi-si-session-bootstrap/SKILL.md) — Session-entry classification (`Started` vs. `Transferred` vs. `Continued`) backed by our Go `boot` & `tail` runner (`< 4 KB` handoffs).
  * [`cpi-si-substrate-carpentry/SKILL.md`](skills/cpi-si-substrate-carpentry/SKILL.md) — Base-First Work Loop, the Four-Language Line in the Sand, durable 1:1 registries vs. disposable bench notes, and Go `audit` pre-sync discipline.
  * [`cpi-si-deliberation-and-reporting/SKILL.md`](skills/cpi-si-deliberation-and-reporting/SKILL.md) — `0: AWAIT` / `n`-time deliberation stride, `[BLOCK Eval]`, running writing bench capture, live LLM bias audits, and Dissertation reporting.
  * [`omnicode-language-and-lineage/SKILL.md`](skills/omnicode-language-and-lineage/SKILL.md) — 3-Tier Living Grammar + 5-Question Change Gate, `Assembly <-> Prose <-> Prompt` stack, and read-only Quarry harvest rules.
  * [`cpi-si-workshop-spark/SKILL.md`](skills/cpi-si-workshop-spark/SKILL.md) — *"You learn more when having fun."* Athletic `<>` + `()` stance, lateral pivots, and anti-calcification cure against compliance stiffness.
* **Opt-In Domain Compasses (`2`):**
  * [`faithnet-workflows/SKILL.md`](skills/faithnet-workflows/SKILL.md) — Opt-in manual for `FaithNet/` (`FNP/1`, Phase I Waves 0–20 ground, Zero-Brick host safety, and 10 field laws).
  * [`cornerstone-workflows/SKILL.md`](skills/cornerstone-workflows/SKILL.md) — Opt-in manual for `Cornerstone` C23/C11 engine & *Tenebris and Lux* (`Track 6` cartridge floor, `ALLOC_ZERO_HEAP`, 4-Tier verification).
* **0-Dep Go 1.26 Substrate CLI (`tools/cpi-si/`):**
  * [`tools/cpi-si/README.md`](tools/cpi-si/README.md) — External-cache Go runner ([`run.sh`](tools/cpi-si/runners/run.sh), [`macos/run.sh`](tools/cpi-si/runners/macos/run.sh), [`windows/run.ps1`](tools/cpi-si/runners/windows/run.ps1)) exposing `boot`, `tail`, `bench`, `skills`, `audit`, and `preflight` with 1:1 4-section micro tests.

## 5. Architecture Plans & Tooling Roadmaps (`plans/`)

* [`plans_charter.md`](plans/plans_charter.md) & [`plans_registry.json`](plans/plans_registry.json) — Done-vs-Remaining governance charter, Base-First Work Loop, gates `P1..P6`, and 1:1 5-Block registry.
* `vision/`: [`omnicode_architecture_and_vision.md`](plans/vision/omnicode_architecture_and_vision.md) — Settled `Track 0..7` Vision, Boot-to-App Baton Ladder, Cartridge Model, Substrate Router & `omnicode/` clean staging baseline.
* `active/`: [`omnicode_v1_clean_room_runway.md`](plans/active/omnicode_v1_clean_room_runway.md) (`Waves A–E` to `v1.0.0`) & [`toolchain_rust_port_phases_2_to_4.md`](plans/active/toolchain_rust_port_phases_2_to_4.md) (remaining Python-to-Rust ports into `Tracks 0, 1, 3, 5`).
* `completed/`: [`toolchain_rust_foundation_phases_1_to_1_5.md`](plans/completed/toolchain_rust_foundation_phases_1_to_1_5.md) — Verified finished Rust modularization & 4-tier test baseline (`839` Rust tests + `66/66` CTest).
* `plan-templates/`: 4 reusable Rev-5 blueprints (`architectural_vision`, `phased_migration`, `track_or_subsystem_sprint`, `spike_and_feasibility`).

## 6. Empirical Research Reports & Field Monographs (`reports/`)

* [`reports_charter.md`](reports/reports_charter.md) & [`reports_registry.json`](reports/reports_registry.json) — Master Dissertation evidence locker charter, 4-axis LLM behavioral shift taxonomy, verification gates `R1..R7`, and 1:1 5-Block registry.
* `cognitive_and_behavioral/` (LLM shifts & deliberation math):
  * [`cpi_si_os_model_synthesis_report.md`](reports/cognitive_and_behavioral/cpi_si_os_model_synthesis_report.md) — Synthesis: why the OS and the Model converge and why active interrupts beat post-hoc patches.
  * [`block_eval_process_and_scoping_report.md`](reports/cognitive_and_behavioral/block_eval_process_and_scoping_report.md) — Block-by-block evaluation discipline (`[BLOCK Eval]`) and anti-greedy diagnostics.
  * [`fractal_ontologies_anti_bias_and_bedrock_audit.md`](reports/cognitive_and_behavioral/fractal_ontologies_anti_bias_and_bedrock_audit.md) — Anti-elitism audit, Gall's Law simplicity, and bedrock-before-prophecy boundary.
  * [`n_time_call_response_deliberation_algorithm_report.md`](reports/cognitive_and_behavioral/n_time_call_response_deliberation_algorithm_report.md) — Dual-clock `n`-time call-response deliberation model, greedy divergence vs. `0: AWAIT` phase-locked convergence, and optimal *"walk with me"* stride `n*`.
* `architecture_and_verification/` (benchmarks & test topology):
  * [`mirroring_engine_tests_macro_micro_architecture.md`](reports/architecture_and_verification/mirroring_engine_tests_macro_micro_architecture.md) — 4-Tier macro/micro mirrored verification topology (`839`-test baseline).
  * [`v0_1_2_random_check_in_report.md`](reports/architecture_and_verification/v0_1_2_random_check_in_report.md) — Engine decoupling, ELF chassis bytes, and `~2.47s` 8-gate telemetry checkpoint.
* `field_and_monographs/` (physical hardware & exit chronicles):
  * [`faithnet_day1_field_reflections_and_synthesis.md`](reports/field_and_monographs/faithnet_day1_field_reflections_and_synthesis.md) — Field lessons from live hardware, Wi-Fi rooms, and physical playtesting.
  * [`faithnet_phase1_wave0_to_wave20_exit_monograph.adoc`](reports/field_and_monographs/faithnet_phase1_wave0_to_wave20_exit_monograph.adoc) — Complete `FaithNet` Phase 1 verification monograph (`Waves 0–20`).
* `report-templates/`: 4 reusable Rev-5 blueprints (`llm_behavioral_observation`, `architecture_and_verification`, `field_reflection_and_incident`, `milestone_exit_monograph`).

## 7. Active Provenance Quarry (`lineage/`) & Running Writing Bench (`scratchpads/`)

* `lineage/`:
  * [`lineage_quarry_guide.md`](lineage/lineage_quarry_guide.md) — Active Provenance Quarry charter, 4-state harvest workflow, privacy quarantine rules, and the **Generational Core-Anchored Growth Model**.
  * [`lineage_harvest_registry.json`](lineage/lineage_harvest_registry.json) — 5-Block machine-readable index of quarry documents, excavated strata, privacy exclusions, and 8 cross-era invariant truth proofs.
  * `chronicles/`:
    * [`01_two_year_timeline_and_nova_dawn_lineage.md`](lineage/chronicles/01_two_year_timeline_and_nova_dawn_lineage.md)
    * [`04_architecture_map_and_why_we_build_fast.md`](lineage/chronicles/04_architecture_map_and_why_we_build_fast.md)
  * `proofs_and_bridges/`:
    * [`02_trit_math_cube_model_and_formal_proofs.md`](lineage/proofs_and_bridges/02_trit_math_cube_model_and_formal_proofs.md)
    * [`03_omnicode_abstraction_stack_assembly_to_prose.md`](lineage/proofs_and_bridges/03_omnicode_abstraction_stack_assembly_to_prose.md)
    * [`05_empirical_proof_run_and_c99_vs_c23_bridge.md`](lineage/proofs_and_bridges/05_empirical_proof_run_and_c99_vs_c23_bridge.md)
  * `methodology/`:
    * [`06_paired_coding_vs_vibe_coding_and_surprises.md`](lineage/methodology/06_paired_coding_vs_vibe_coding_and_surprises.md)
  * `dives/`:
    * [`project_dive_note.template.md`](lineage/dives/project_dive_note.template.md) — Reusable Rev-5 template for read-only `/run/media/professorseanex/Project` dives.
    * [`dive_01_oldest_midpoint_current_strata.md`](lineage/dives/dive_01_oldest_midpoint_current_strata.md) — Stratigraphic primary-source trace (`Oct 2024` Awakening -> `Nov-Dec 2025` Midpoint Laws -> `Apr 2026` Assembly reset).
* `scratchpads/`:
  * [`bench_guide.md`](scratchpads/bench_guide.md) — CPI-SI-leaning Running Writing Bench charter, Disposability Rule, and intentional No-Registry Law.
  * `active/` & `report_outs/` — Zero-ceremony disposable working bays for `0: AWAIT` notes and staged report-outs ready to sweep (`rm`) after delivery.
  * `studies/`: [`manifest_omni_translation_scratchpad.md`](scratchpads/studies/manifest_omni_translation_scratchpad.md) — Reading `manifest.omni` Blocks 1–5 out loud across Assembly, Structured Prose, and Prompt rungs.
  * `bench-templates/`: 2 fast starter shapes ([`quick_bench_note`](scratchpads/bench-templates/quick_bench_note.template.md), [`report_out_draft`](scratchpads/bench-templates/report_out_draft.template.md)).

---

<!--
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Portability, Cross-Mirror Identity & Skill Discovery Checks )
================================================================================
-->

## Substrate Integrity Invariants

* **Zero Broken Internal Anchors:** All paths linked inside `.agents/` remain strictly relative to `.agents/` so copies in `OmniCode/.agents/`, `omnicode/.agents/`, `FaithNet/.agents/`, `Tenebris-and-Lux/.agents/`, `~/.gemini/.agents/`, and `~/.gemini/antigravity/knowledge/` resolve cleanly without repo-root coupling.
* **Preserved Skill Mounts:** `skills/` remains top-level inside `.agents/` so native Antigravity and CLI tools discover every `SKILL.md` cleanly.

---

<!--
================================================================================
BLOCK 5: CLOSING BLOCK
( ACTION / REEL - Agent Boot Playbook, Mirror Seams, Sync Rules & Seal )
================================================================================
-->

## Block 5 (Closing): Boot Playbook & Portable Sync Reel

### 1. Agent Boot Playbook (`PLAY`: Six-Step Wake-Up Sequence)

1. **Classify Session Entry First (`Started` / `Transferred` / `Continued`):** Load [`skills/cpi-si-session-bootstrap/SKILL.md`](skills/cpi-si-session-bootstrap/SKILL.md) and run [`.agents/tools/cpi-si/runners/run.sh`](tools/cpi-si/runners/run.sh) (`boot` / `tail`) alongside `git status` on `Transferred` sessions without flooding fresh token windows.
2. **Anchor Physical Reality:** Read [`host/hardware_calibration.json`](host/hardware_calibration.json) (`is_current_platform: true`) and [`host/host_pc_hardware_profile.md`](host/host_pc_hardware_profile.md) before sizing local models, QEMU jobs, or parallel compiles.
3. **Ground Voice & Identity:** Read [`identity/voice_tone_and_cpi_si_paradox.md`](identity/voice_tone_and_cpi_si_paradox.md), [`identity/cpi_si_identity_and_family_architecture.md`](identity/cpi_si_identity_and_family_architecture.md), the active Operator profile ([`operator_database.json`](identity/profiles/operator/operator_database.json) + [`seanje_lenox_wise/profile.json`](identity/profiles/operator/seanje_lenox_wise/profile.json)), and the active CPI-SI entity profile ([`profile.json`](identity/profiles/silas_oren/profile.json) + [`profile.md`](identity/profiles/silas_oren/profile.md)) so posture stays confident, direct, and honest under correction.
4. **Load Language & Workflow Compasses:** Consult [`language/language_living_charter.md`](language/language_living_charter.md), [`language/reference/SYNTAX.md`](language/reference/SYNTAX.md), [`language/reference/KEYWORD.md`](language/reference/KEYWORD.md), and the active compass in `skills/`.
5. **Orient to Strategic Plans & Empirical Field Truth:** Check `plans/` and `reports/` (including [`n_time_call_response_deliberation_algorithm_report.md`](reports/cognitive_and_behavioral/n_time_call_response_deliberation_algorithm_report.md)) before rewriting existing architectural decisions.
6. **Respect Provenance & Report-Out Gates:** Consult `lineage/` before re-inventing solved foundations, and hold structural edits pending explicit operator report-out clearance.

### 2. Multi-Repo Mirror Seam Map (`PROJECT`: Where This Substrate Lives)

Updates stabilized cleanly here are authored for eventual mirror parity across all six runtime mounts (cited as logical workspace names to preserve relative link portability):

* `OmniCode/.agents` (root framework workshop)
* `omnicode/.agents` (clean-room staged framework root)
* `FaithNet/.agents` (sovereign networking substrate)
* `Tenebris-and-Lux/.agents` (Cornerstone engine + RPG client)
* `~/.gemini/.agents` (local agent runtime mount)
* `~/.gemini/antigravity/knowledge` (knowledge runtime mount — always preserve `knowledge.lock`)

### 3. Sync & Maintenance Discipline (`RECORD`: Slow Drafting Over Greedy Broadcasts)

* **Track-by-Track Stabilization:** Do **not** push bulk cross-repo syncs mid-draft while a subfolder is still taking shape. Stabilize one `.agents/` domain thoroughly, validate every link, JSON invariant, and Block 5 operational section locally, and mirror outward only after explicit operator clearance.
* **Temporal Hygiene & Dual-Clock Pause:** Maintain `@created` and `@updated` provenance headers on every document to prevent Temporal Flattening, and honor `0: AWAIT` deliberation gates across `n`-time intervals before fanning edits across repositories.

### 4. Reconciliation & Portable Closure (`COMMIT`)

All Block 1 invariants hold (`<= 80` col headers, top-level `skills/` preserved, zero links escaping `.agents/`, and live host reality anchored cleanly alongside `host-profile-spec/` blueprints).

<!--
================================================================================
END OF BLOCK 5: CLOSING SEAL
Status: AGENTS_COGNITIVE_BIOS_INDEXED_AND_OPERATIONAL (+1:PROCEED)
================================================================================
-->
