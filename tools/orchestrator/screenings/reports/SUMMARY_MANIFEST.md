# OmniCode Cognitive Continuum: 7-Tier Floor Screening Manifest

> **Milestone**: Rev 5 Architecture // CPI-SI Governance  
> **Evaluation Period**: 2026-11-13  
> **Chassis Baseline**: Intel Core 7 150U + NVIDIA GeForce MX570 A (2GB GDDR6) + 16GB DDR5  
> **Author**: CreativeWorkzStudio LLC / ProfessorSeanEX  
> **Governing Law**: $\mathbf{Local\ First \longrightarrow Cloud\ (If\ Able)}$

---

## 1. Executive Master Roster

All 7 candidates across the local and cloud spectrum have been screened, calibrated, and certified:

| Tier | Candidate Model | Stratum | Tok/s | Key Specialization | Primary Archetype |
| :---: | :--- | :---: | :---: | :--- | :--- |
| **1** | `qwen2.5-coder:0.5b` | Local | **159.3** | High-speed task decomposition, DAG slicing, JSON parsing | `TASK_MICRO_SIFT` |
| **2** | `omnicode-fast-1.5b` | Local | **32.8** | Native Bedrock workhorse, CLI tooling, 32k Golden Ratio | `TASK_INTERACTIVE_CODE` |
| **3** | `omnicode-polymath` (3B) | Local | **10.9** | Balanced C11 author, dimetric projection, clean math | `TASK_SYSTEMS_LOGIC` |
| **4** | `omnicode-architect-7b` | Local | **4.8** | Local PC hardware ceiling, deep invariant audit, multi-struct | `TASK_INVARIANT_AUDIT` |
| **5** | `omnicode-cloud-14b` | Cloud | **38.5** | Macro-refactor, large-scale codebase migration (Free T4 base) | `TASK_MACRO_REFACTOR` |
| **6** | `omnicode-cloud-32b` | Cloud | **22.4** | State-of-the-Art C11 authoring & full subsystem synthesis (5.5T) | `TASK_CODER_FLAGSHIP` |
| **7** | `omnicode-cloud-72b` | Cloud | **18.7** | Macro-architectural reasoning, cross-subsystem proofs (18T tokens) | `TASK_STRATEGIC_SUMMIT` |

---

## 2. The Invariant 18 Discovery & Exoskeleton Necessity

Across all tiers, an essential empirical truth was established:
1. **Open-weights models cannot reliably enforce negative constraints through prompt instructions alone.**
   * For smaller models (0.5B and 1.5B), the probability of generating `//` line comments remains high during code authoring, even when explicitly instructed not to.
   * By Tier 3 (3B) and Tier 4 (7B), instruction-following improves dramatically, reaching 100% block-comment compliance in Tier 6 (32B) and Tier 7 (72B).
2. **The Exoskeleton Tripwire Is Non-Negotiable**:
   * The deterministic compiler tripwire (`tools/orchestrator/exoskeleton/invariant_verifier.py`) acts as the external physical immune system, validating every file before disk commit regardless of which model authored it.

---

## 3. The Sovereign Local Handshake

The bi-directional stepped staircase ensures that cloud compute is strictly an **elevation booster**, never an external dependency:
* Tiers 1 through 4 execute natively, offline, and air-gapped on the local laptop chassis.
* Tiers 5 through 7 activate when task complexity or context demands exceed local memory bounds.
* **The Sovereign Handshake**: The moment a cloud task drops back into local capability ($\le 7\text{B}$), the system automatically routes execution back to the local sovereign chassis.

---

## 4. Screening Reports Index

* [Tier 1: 0.5B Micro-Scout](tier_01_0_5b_micro.md)
* [Tier 2: 1.5B Native Bedrock](tier_02_1_5b_bedrock.md)
* [Tier 3: 3B Systems Polymath](tier_03_3b_polymath.md)
* [Tier 4: 7B Local PC Ceiling](tier_04_7b_pc_ceiling.md)
* [Tier 5: 14B Cloud Workhorse](tier_05_14b_cloud_workhorse.md)
* [Tier 6: 32B Coder Flagship](tier_06_32b_coder_flagship.md)
* [Tier 7: 72B Strategic Architecture Summit](tier_07_72b_strategic_summit.md)
