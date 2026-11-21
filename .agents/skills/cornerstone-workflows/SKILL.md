---
name: cornerstone-workflows
description: >-
  Opt-in domain compass for building, testing, verifying, and maintaining the Cornerstone
  Game Engine and Tenebris and Lux RPG (Track 6 cartridge floor). Activate when working inside
  Tenebris-and-Lux/ or Track 6 engine/game seams; governs ALLOC_ZERO_HEAP, C23->C11 ladder,
  4-Tier verification suite, ELF profiling, and non-blocking Git hooks without constraining non-game work.
---

<!--
================================================================================
BLOCK 1: METADATA BLOCK
( Opt-In Domain Compass for Cornerstone C23/C11 Engine & Tenebris and Lux )
================================================================================
@project       OmniCode Framework / Tenebris and Lux Track 6 Cartridge
@file          .agents/skills/cornerstone-workflows/SKILL.md
@id            cog.agent.skill.cornerstone_workflows
@layer         Portable Agent Substrate - Opt-In Domain Skill (Cornerstone)
@archetype     domain_compass | game_engine_and_rpg_cartridge
@version       0.1.3-dev
@interface_rev Rev 5
@created       2026-11-17
@updated       2026-11-20
@author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
@repository    https://github.com/Novaking34/OmniCode_Framework
@system        Cornerstone Engine (C23/C11) + Tenebris and Lux (Track 6)
@role          Opt-In Engineering Manual When Stepping Into Engine or Game Work
@maturity      Rev 5 Operational Skill (Coordinate v0.1.3)
@chassis       zero-heap-silicon | Layer 0/1 C23->C11 + Rust 2024 tools/
@seams         bios: ../../README.md |
               covenant: ../cpi-si-covenant/SKILL.md |
               mirror: ../../reports/architecture_and_verification/
                       mirroring_engine_tests_macro_micro_architecture.md |
               active_ports: ../../plans/active/
                             toolchain_rust_port_phases_2_to_4.md |
               completed_base: ../../plans/completed/
                               toolchain_rust_foundation_phases_1_to_1_5.md
@invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5, <=80 col walls).
               2. Line-1 YAML frontmatter preserved for native discovery.
               3. Opt-In Domain Scope: Cornerstone is Track 6 inside the Big
                  House — NEVER lock non-game sessions into game-dev loops.
               4. ALLOC_ZERO_HEAP strict on Layer 0 & Layer 1 C23/C11 cogs.
               5. Keep exact observed counts separate from preserved snapshots.
================================================================================
-->

# Cornerstone Workflows & Engineering Compass

> **Project**: *Tenebris and Lux* | **Engine**: **Cornerstone Game Engine** (`engine/`, `Track 6` Cartridge)  
> **Scope**: **Opt-In Domain Compass** — activates cleanly when editing `Tenebris-and-Lux/` C23/C11 engine, `game/`, `tools/` Rust validators, or `Track 6` cartridge ABI seams. *Game dev is a prized track of our sovereign publishing house, never a cage around the rest of the Big House.*  
> **Core Philosophy**: *"You learn more when having fun."*

---

<!--
================================================================================
BLOCK 2: OPENING BLOCK
( Scoping Cornerstone Properly Inside the Sovereign Publishing House )
================================================================================
-->

## Block 2 (Opening): Precision Without Locking the Rest of the Workshop

In a multi-domain workshop, treating game-engine build habits as universal law forces every grammar, bootstrap, hardware calibration, or network session into the wrong shape. This skill stays **opt-in on the `Cornerstone` / *Tenebris and Lux* floor**:

* When writing `.agents/` substrate specs, Go tools, OmniCode grammar, or working-bench studies, stay guided by cross-cutting craft skills ([`cpi-si-covenant`](../cpi-si-covenant/SKILL.md), [`cpi-si-substrate-carpentry`](../cpi-si-substrate-carpentry/SKILL.md), [`cpi-si-workshop-spark`](../cpi-si-workshop-spark/SKILL.md)).
* When stepping into `Tenebris-and-Lux/` C23/C11 memory chasses, `CTest`, ELF sizing, Config ROMs, or Rust `tl_tools` validation, this manual snaps you immediately to the physical floor rules of Layer 0, Layer 1, and `Track 6`.

---

<!--
================================================================================
BLOCK 3: BODY BLOCK
( Dual Discipline, 4-Tier Living Suite, Memory Chassis, Hooks & Dual Audience )
================================================================================
-->

## 1. The Dual Development Discipline

Engine and RPG development at CreativeWorkzStudio runs on two synchronized tracks:

1. **Waterfall Bedrock (Fixed Physical Laws):**
   * **`ALLOC_ZERO_HEAP`:** Exactly `0` dynamic heap allocations (`malloc`, `calloc`, `realloc`, `free`) in Layer 0 (Core/IO/Log/Debug/Audio) and Layer 1 (Simulation). Memory lives transparently in `.rodata`, `.data`, `.bss`, and stack.
   * **Downward Dependency Flow:** Lower layers never import higher layers (`engine/` never imports `game/`). The engine stands independently as the `Track 6` cartridge runtime.
   * **Fall-Down C/C++ Standard Ladder:** Targets **ISO C23** (`-std=c23`) and **C++20** (`-std=c++20`) with deterministic fallback down to **ISO C17/C11** and **C++17** (`CORNERSTONE_C_STANDARD=AUTO|23|17|11`).
   * **Universal 5-Block Protocol:** Every C header/source file, JSON manifest, shell hook, Rust validator, and Markdown document carries Rev-5 5-Block framing.
   * **Authentic Canon:** Sole canonical antagonist is **The Dark Lord Usuru**; twin relics are the **Shield of Lux** and **Sword of Tenebris** under **Metaphysical Neutrality** across the 6 Continental Powers and 5 Ending Routes.
2. **Agile Iteration (Rapid Velocity):**
   * Elevate reusable cogs from `game/` downward into `engine/` cleanly.
   * Keep client git hooks fast (`< 100ms` pre-commit, `< 3s` pre-push).
   * Let strict compiler warnings (`-Wall -Wextra -Wpedantic`) and CTest expose seam bugs early rather than piling up dead compatibility wrappers.

---

## 2. The 4-Tier Living Verification Battery (`Tenebris-and-Lux/`)

Never hardcode stale test tallies into claims — run the live battery directly and report exact observed pass/fail numbers from your run (historical `2026-11-17` milestones of `66/66` CTest and `839` Rust tests are preserved inside [`mirroring_engine_tests_macro_micro_architecture.md`](../../reports/architecture_and_verification/mirroring_engine_tests_macro_micro_architecture.md)):

```bash
# 1. Build Engine & 4-Tier Test Harnesses (C23 Target w/ C11 Step-Down Floor)
cmake --build build --config Debug --parallel

# 2. Tier 1-3: Full CTest Battery (Functional, Unit, Integrated, System)
ctest --test-dir build -C Debug --output-on-failure

# 3. Tier 0/1: Pre-Flight Self-Audit & Invariant Tripwires
./build/bin/tenebris-lux --audit    # Or root convenience symlink: ./build/tenebris-lux --audit

# 4. Zero-Disk Safe-Mode BIOS ROM Fallback Test
./build/bin/tenebris-lux -r /tmp/nonexistent_fallback_root --audit

# 5. Terminal CLI Game Loop Smoke Test
echo "quit" | ./build/bin/tenebris-lux --cli

# 6. Tier 4: Pure Rust 2024 Developer Toolchain (tests/{functional,unit,integrated,system})
cargo test --manifest-path tools/Cargo.toml

# 7. Tier 4: Full System Manifest & C Header Rev-5 Validation
cargo run --manifest-path tools/Cargo.toml --bin tl_tools -- validate all
```

*(Note on Python probes inside `tools/orchestrator/`: those probes are scheduled for Phase 2–4 clean-room Rust migration into OmniCode `Track 0` / `Track 1` per [`toolchain_rust_port_phases_2_to_4.md`](../../plans/active/toolchain_rust_port_phases_2_to_4.md).)*

### Structured Build Layout & Maintenance Targets

Artifact folders under `build/` stay partitioned by kind:

* `build/bin/`: Primary executable (`tenebris-lux`)
* `build/lib/`: Static/shared libraries
* `build/tests/`: Compiled Layer 4 verification test binaries
* `build/scratch/`: Transient mount test sandboxes
* `build/generated/`: Auto-generated `.rodata` Config ROM C headers (`*_rom.h`)
* `build/logs/`: Subsystem flight logs & rotated telemetry
* `build/tenebris-lux`: Convenience symlink pointing to `bin/tenebris-lux`

```bash
make -C build clean-scratch        # Purges transient test mount dirs
make -C build clean-logs           # Clears rotated telemetry logs
make -C build clean-tests          # Purges compiled test executables
make -C build clean-all-artifacts  # Runs all three build cleanup targets
```

---

## 3. Controlled Memory Chassis & ELF Profiling

Dynamic heap allocation (`malloc`, `calloc`, `free`) is strictly forbidden inside Layer 0/1 C cogs. Inspect real section budgets straight from the compiled ELF binary:

```bash
size -B build/tenebris-lux
size -A build/tenebris-lux
nm --size-sort -S -r build/tenebris-lux | head -n 30
```

* **Chassis Rule:** Flight log rings, debug snapshot rings, audio cue queues, and VFS arrays sit explicitly bounded in `.bss`/`.rodata` via `engine/config/` (captured in [`v0_1_2_random_check_in_report.md`](../../reports/architecture_and_verification/v0_1_2_random_check_in_report.md)).
* **Safe-Mode BIOS Floor:** Every operational engine cog pairs with a `*_fallback.[ch]` backed by compiled `.rodata` Config ROMs (`cmake/GenerateConfigRom.cmake`) so the engine boots cleanly even when disk assets are absent.

---

## 4. Non-Blocking VCS & Git Hook Discipline

Client-side hooks inside `Tenebris-and-Lux/.githooks/` (`git config core.hooksPath .githooks`) keep feedback fast without locking urgent fixes:

* **`pre-commit`** (`< 100ms`): Guards against transient `build/` / `logs/` / `Offline_Work_*.md` files, assets over 5 MB, or missing Block 1 headers.
* **`commit-msg`**: Enforces the 5-block ledger format while cleanly bypassing `Merge branch ...` and `Revert ...` auto-messages.
* **`pre-push`**: Runs `tl_tools validate all`.

```bash
# Urgent bypass levers (never stay blocked during emergency hotfixes):
SKIP_PRE_COMMIT=1 git commit -m "..."
SKIP_COMMIT_MSG_HOOK=1 git commit -m "..."
SKIP_PRE_PUSH=1 git push
SKIP_HOOKS=1 git commit -m "..."
```

---

## 5. Dual-Audience Documentation Covenant

Every subsystem doc and Block 1 `PURPOSE & ARCHITECTURAL ROLE` section addresses both voices cleanly:

1. **For Game Designers, Writers & Educators:** Plain-English combat mechanics, tactical grid behavior, learning loops (*"You learn more when having fun."*), authentic canon (**The Dark Lord Usuru**, 6 Continental Powers, 5 Ending Routes), and player-facing tuning levers.
2. **For Systems Programmers & Engine Architects:** Exact `.bss`/`.rodata` budgets, Big-O bounds, reentrancy contracts, C23->C11 fallback headers, and `DEBUG_STEP_AWAIT` probes.

---

<!--
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Verification Gates Before Claiming Engine / Game Work Ready )
================================================================================
-->

## Block 4 (Audit): Pre-Report-Out Cornerstone Gates

1. **Zero-Heap & Layer Isolation:** Grep edited Layer 0/1 C files to verify zero `malloc`/`calloc`/`free` calls and zero upward `game/` includes.
2. **Fresh Binary Proof:** Never report historical test numbers (`839` or `66/66`) as fresh runs — execute `ctest --output-on-failure`, `./build/tenebris-lux --audit`, and `cargo test` after your final edit and report exact printed totals.
3. **Cartridge Seam Boundary (`Track 6`):** Keep shared engine abstractions clean so OmniCode `Track 6` can host Cornerstone without game-specific leaks.

---

<!--
================================================================================
BLOCK 5: CLOSING BLOCK
( ACTION / REEL - Opt-In Discipline, Outward Seams, Anchors & Seal )
================================================================================
-->

## Block 5 (Closing): Operational Reel & Closing Seal

### `PLAY` — Stepping Onto & Off the Game Engine Floor

1. **Opt-In Activation:** Reach for this compass when entering `Tenebris-and-Lux/` or `Track 6` cartridge work; set it aside cleanly when pivoting back to `.agents/` tooling, grammar, or general substrate tasks.
2. **Keep Joy Next to Rigor:** Clean deterministic C memory chasses and *"You learn more when having fun"* gameplay tuning sharpen each other — never bury good game iteration under needless red tape.

### `PROJECT` — Outward Ecosystem Seams

1. **Feeds Goal #2 (`Track 6` Cartridge Launch):** [`omnicode_v1_clean_room_runway.md`](../../plans/active/omnicode_v1_clean_room_runway.md) Waves B–D converge toward launching *Tenebris and Lux* through the clean `cornerstone_cartridge_v1` ABI seam.
2. **Feeds FaithNet C/Go Co-Op Bridge:** Fixed `32B` `0xFA17` headers let Cornerstone C11/C23 modules exchange 512-byte sectors directly with Go `FaithNet` nodes (`ALLOC_ZERO_HEAP` intact).

### `RECORD` — Linked Substrate Anchors

* Verification Mirror Topology & ELF Baseline: [`mirroring_engine_tests_macro_micro_architecture.md`](../../reports/architecture_and_verification/mirroring_engine_tests_macro_micro_architecture.md), [`v0_1_2_random_check_in_report.md`](../../reports/architecture_and_verification/v0_1_2_random_check_in_report.md)
* Rust Toolchain Plans (Done vs. Active): [`toolchain_rust_foundation_phases_1_to_1_5.md`](../../plans/completed/toolchain_rust_foundation_phases_1_to_1_5.md), [`toolchain_rust_port_phases_2_to_4.md`](../../plans/active/toolchain_rust_port_phases_2_to_4.md)
* Clean-Room Runway & Cross-Stack Compasses: [`omnicode_v1_clean_room_runway.md`](../../plans/active/omnicode_v1_clean_room_runway.md), [`faithnet-workflows`](../faithnet-workflows/SKILL.md), [`cpi-si-workshop-spark`](../cpi-si-workshop-spark/SKILL.md)

### `COMMIT` — Closing Seal

* **Zero Heap, Real Sensors, Clean Joy:** Deterministic silicon budgets underneath; unhurried studio craft on top.
* **Domain Opt-In Guard:** Govern `Tenebris-and-Lux/` and `Track 6` floors cleanly without locking non-game sessions into engine loops.
* **Fresh Run Honesty:** Report only live `ctest`, `--audit`, and `cargo test` counts observed after final edits.
* **Verification:** Rev-5 `<= 80`-col walls, Line-1 YAML discovery, and zero `.agents/`-escaping links enforced (`2026-11-20`).
* **Status:** `@created 2026-11-17` / `@updated 2026-11-20`, `CORNERSTONE_WORKFLOWS_REV5_UPGRADED (+1:PROCEED)`.

<!-- END OF BLOCK 5: CLOSING SEAL -->
