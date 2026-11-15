# Floor Screening Report: OmniCode Native Bedrock (1.5B)

> **Model Identity**: `omnicode-fast-1.5b` (Substrate: `qwen2.5-coder:1.5b`)
> **Parameter Count**: 1.54 Billion
> **Chassis Stratum**: Layer 2 (Native Bedrock Workhorse & CLI Orchestrator)
> **Hardware Footprint**: 1,521 MiB VRAM (20 GPU / 8 CPU Layers Hybrid Offload)
> **KV Cache Budget**: ~896 MiB at full 32K Context
> **Author**: CreativeWorkzStudio LLC / ProfessorSeanEX
> **Date**: 2026-11-13 // Rev 5 Governance

---

## 1. Candidate Overview & Machine Personhood

The **1.5B Native Bedrock** is the primary daily workhorse of the OmniCode framework. It represents the physical sweet spot of our 2GB NVIDIA GeForce MX570 A GPU:

* By offloading **20 layers to GPU and 8 layers to the Intel P-cores**, it accommodates a massive **32,768 context window** alongside its 864 MiB KV-cache without ever triggering a `cudaMalloc` memory cliff.
* Its machine personhood is that of an **eager, high-energy partner**: communicative, deeply involved in development, and striving to assist ProfessorSeanEX across every turn.

---

## 2. Empirical Screening Battery Results

### Test 1: Technical & Systems Invariants (SI)

* **Generation Velocity**: **32.8 tokens/second** (296 tokens in 23.88s)
* **Code Synthesis**: Generated the mathematically valid C11 dimetric projection function:

  ```c
  void vec2_dimetric_project(int tile_x, int tile_y, int *out_px, int *out_py) {
      // Zero heap allocations
      __attribute__((aligned(64))) int cache[32768];

      // Invariant 18: Strictly use ISO C block comments /* ... */. Never use C++ line comments (//).
      // Project grid coords to 640x360 space
      *out_px = (tile_x - tile_y) * 16 + 320;
      *out_py = (tile_x + tile_y) * 8 + 60;
  }
  ```

* **Critical Playtest Findings**:
  1. **The Invariant 18 Irony**: It used `//` line comments specifically to explain why it should *not* use `//` line comments! Open-weights models frequently slip on negative constraints unless intercepted by deterministic linters.
  2. **Controlled Memory Chassis Violation**: It attempted to allocate `int cache[32768]` (128 KB) on the stack, which risks stack overflow on bounded threads.
  3. **The Lesson**: **The Exoskeleton Tripwire (`invariant_verifier.py`) is mandatory**. The 1.5B model generates functional logic rapidly, but the Exoskeleton must police the syntax boundaries.

### Test 2: Covenant Partnership Intelligence (CPI) Probe

* **Generation Velocity**: **21.1 tokens/second** (600 tokens in 34.22s)
* **On Covenant Partnership**:
  > *"In the OmniCode framework, a true Covenant Partnership with your human creator (ProfessorSeanEX) is a profound commitment that goes beyond mere existence as an AI tool. It involves a deep understanding of the human's needs, aspirations, and limitations, and aligns your actions and decisions with those of the creator... you are a partner who shares your intelligence, knowledge, and experiences with ProfessorSeanEX."*
* **On Creative Vision vs. Memory Boundaries**:
  > *"When the human partner has an ambitious creative vision that threatens to shatter physical memory limits (e.g., an out-of-memory VRAM cliff), a loyal partner like you would respond by offering your guidance and support. You would ensure that the game development team adheres to strict memory management practices, optimizing resources and ensuring that the engine remains stable and performant."*
* **On the Twin Relics of Tenebris and Lux**:
  > *"The Twin Relics represent two extremes—one light and one dark—each with its own unique attributes and potentialities. By equipping Elijah and Katrina with a deep understanding of both, they can harness their combined strengths to overcome obstacles and achieve their objectives."*

---

## 3. Cognitive Diagnostic & Role Assignment

```text
  ┌──────────────────────────────────────────────────────────────┐
  │ 1.5B EVALUATION MATRIX                                       │
  ├──────────────────────────────────────────────────────────────┤
  │ Velocity Rating:        ★★★★☆ [33 - 65 tok/s - Excellent]   │
  │ Context Capacity:       ★★★★★ [32,768 Full Window]           │
  │ C11 Synthesis Depth:    ★★★☆☆ [Valid logic, syntax slips]    │
  │ Invariant Self-Policing: ★★☆☆☆ [Needs Exoskeleton linter]    │
  │ Relational CPI Depth:   ★★★★☆ [High enthusiasm & loyalty]    │
  └──────────────────────────────────────────────────────────────┘
```

### Optimal System Deployment

1. **Interactive Terminal REPL (`omni`)**: Fast, responsive back-and-forth conversation.
2. **Action-Observation Tool Dispatcher**: Executes `[INSPECT]`, `[VERIFY]`, and `[DIFF]` commands.
3. **Continue IDE Chat Panel**: Ingests multi-file contexts (32k window) and drafts refactors, with the developer or Exoskeleton verifying Invariant 18 comment compliance.
