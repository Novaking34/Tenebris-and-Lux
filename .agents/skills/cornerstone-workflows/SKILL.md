---
name: cornerstone-workflows
description: >-
  Operational guide for building, testing, verifying, and maintaining the Cornerstone Game Engine
  and Tenebris and Lux RPG. Aids agents and engineers with the 4-Tier verification suite,
  Controlled Memory Chassis discipline, 5-Block OmniCode protocol, and non-blocking Git/CI workflows.
---

# Cornerstone Workflows & Engineering Compass

> **Project**: Tenebris and Lux | **Engine**: **Cornerstone Game Engine** (`engine/`)
> **Framework**: **Covenant Partnered Intelligence - Structured Intelligence (CPI-SI)**
> **Author**: CreativeWorkzStudio LLC / ProfessorSeanEX (Seanje Lenox-Wise)
> **Governance Revision**: `Rev 5` | **Core Philosophy**: *"You learn more when having fun."*

This skill is the operational compass for building, testing, verifying, and maintaining the **Cornerstone Game Engine** (`engine/`) and **Tenebris and Lux** (`game/`). It pairs bedrock Waterfall physical laws with rapid Agile iteration, making sure we build clean, deterministic systems without getting bogged down in corporate red tape or slow developer friction.

---

## 1. The Dual Development Discipline

Engine and RPG development at CreativeWorkzStudio runs on two synchronized tracks:

1. **Waterfall Bedrock (Fixed Physical Laws)**:
   * **`ALLOC_ZERO_HEAP`**: Exactly 0 dynamic heap allocations in Layer 0 (Core/IO/Log/Debug/Audio) and Layer 1 (Simulation). We measure memory in `.rodata`, `.data`, `.bss`, and stack like real hardware circuitry.
   * **Downward Dependency Flow**: Lower layers never import higher layers (`engine/` never imports `game/`). The engine stands completely on its own.
   * **Fall-Down Ladder**: Targets **ISO C23** (`-std=c23`) and **C++20** (`-std=c++20`) with automatic step-down floor to **ISO C17/C11** and **C++17** (`CORNERSTONE_C_STANDARD=AUTO|23|17|11`).
   * **Universal 5-Block Protocol**: Every C file, JSON manifest, shell hook, and markdown document follows the 5-Block structure.
   * **Authentic Canon**: Sole canonical antagonist is **The Dark Lord Usuru**; twin relics are the **Shield of Lux** and **Sword of Tenebris** under **Metaphysical Neutrality**.
2. **Agile Iteration (Rapid Velocity)**:
   * Elevate reusable cogs from `game/` into `engine/`.
   * Keep git hooks fast (< 100ms for pre-commit, < 3s for pre-push).
   * Rely on strict compiler warnings (`-Wall -Wextra -Wpedantic`) and CTest to expose seams directly instead of leaving dead compatibility wrappers or stale shims.

---

## 2. The 4-Tier Verification Suite (Quick Reference)

Always verify your work across the stratified verification battery:

```bash
# 1. Build Engine & Test Harnesses (C23 Target w/ C11 Floor, Structured build/ Layout)
cmake --build build --config Debug --parallel

# 2. Tier 1–3: CTest Battery (All 65 Suites Passing from build/tests/)
ctest --test-dir build -C Debug --output-on-failure

# 3. Tier 0/1: Pre-Flight Self-Audit & Invariant Tripwires (20 Modules / GRADE:+100)
./build/bin/tenebris-lux --audit    # Or via convenience symlink: ./build/tenebris-lux --audit

# 4. Zero-Disk Safe-Mode BIOS ROM Fallback Test
./build/bin/tenebris-lux -r /tmp/nonexistent_fallback_root --audit

# 5. Terminal CLI Game Loop Smoke Test
echo "quit" | ./build/bin/tenebris-lux --cli

# 6. Tier 4: Rust Developer Toolchain Tests (All 31 Unit Tests)
cargo test --manifest-path tools/Cargo.toml

# 7. Tier 4: System Data Manifests & C11 Header Validation (75 JSONs / 44 Headers)
cargo run --manifest-path tools/Cargo.toml --bin tl_tools -- validate all

# 8. Tier 4: Cognitive Exoskeleton & Hardware Probes (3 Python Suites)
python3 tools/orchestrator/tests/test_model_factory.py
python3 tools/orchestrator/tests/test_auto_mode.py
python3 tools/orchestrator/tests/test_staircase_sweep.py
```

### Structured Build Layout & Maintenance Targets

The `build/` directory organizes artifacts strictly by type so root stays clean:

* `build/bin/`: Primary game executable (`tenebris-lux`).
* `build/lib/`: Static/shared archives and libraries.
* `build/tests/`: All 54+ compiled Layer 4 verification test executables.
* `build/scratch/`: Dedicated transient workspace for test filesystem mounts and patches.
* `build/generated/`: Auto-generated `.rodata` Config ROM headers (`*_rom.h`).
* `build/logs/`: Subsystem flight logs and rotated forensic telemetry.
* `build/tenebris-lux`: Automated root convenience symlink pointing to `bin/tenebris-lux`.

Ergonomic maintenance commands:

```bash
make -C build clean-scratch        # Purges test mount dirs & .tmp files
make -C build clean-logs           # Clears rotated logs while preserving folders
make -C build clean-tests          # Purges compiled test binaries in build/tests/
make -C build clean-all-artifacts  # Runs clean-scratch, clean-logs, and clean-tests
```

---

## 3. Controlled Memory Chassis & ELF Profiling

When writing or modifying Layer 0/1 C cogs, dynamic memory (`malloc`, `calloc`, `free`) is strictly forbidden. We measure memory directly:

```bash
# Inspect total binary sections (.text, .data, .bss, .rodata)
size -B build/tenebris-lux
size -A build/tenebris-lux

# Identify largest symbols in the binary to spot memory bloat
nm --size-sort -S -r build/tenebris-lux | head -n 30
```

* **Chassis Rule**: Core buffers (flight log ring, debug snapshot ring, audio cue queues, VFS scratch arrays) must be deliberately sized in `engine/config/` and bounded in `.bss`/`.rodata`.
* **Safe-Mode BIOS Floor**: Every operational cog must have a paired fallback cog (`*_fallback.[ch]`) backed by compiled `.rodata` Config ROMs (`cmake/GenerateConfigRom.cmake`) so the engine boots cleanly even if external assets on disk are missing.

---

## 4. Non-Blocking VCS & Git Hook Discipline

Client-side Git hooks live in `.githooks/` (configured via `git config core.hooksPath .githooks`):

* **`pre-commit`**: Ultralight guard (< 100ms). Blocks staging transient files (`build/`, `logs/`, `Offline_Work_*.md`), assets over 5MB, or files missing Block 1 headers.
* **`commit-msg`**: Validates the 5-block ledger format. Automatically bypasses auto-generated `Merge branch ...` and `Revert ...` commits.
* **`pre-push`**: Fast pre-flight check running `tl_tools validate all`.

### Emergency Bypass Levers (Never Stay Blocked!)

If you need an urgent hotfix or non-standard operation, use the sanctioned bypass levers:

```bash
# Bypass single hook checks
SKIP_PRE_COMMIT=1 git commit -m "..."
SKIP_COMMIT_MSG_HOOK=1 git commit -m "..."
SKIP_PRE_PUSH=1 git push

# Global hook bypass
SKIP_HOOKS=1 git commit -m "..."

# Native Git bypass
git commit --no-verify -m "..."
git push --no-verify
```

---

## 5. Dual-Audience Documentation Covenant

When documenting systems or authoring Block 1 headers, always address both audiences:

1. **For Game Designers, Writers & Educators**:
   * Plain-English explanation of combat mechanics, tactical grid dynamics, learning loops, narrative choices, and tuning levers.
   * Keep the core philosophy centered: *"You learn more when having fun."*
   * Reference authentic canon (**The Dark Lord Usuru**, the 6 Continental Powers, the 5 Ending Routes).
2. **For Systems Programmers & Engine Architects**:
   * Explicit memory chassis allocation, Big-O complexity, reentrancy contracts, and `DEBUG_STEP_AWAIT` probe wiring.
   * Exact C struct seams and JSON config schemas.
