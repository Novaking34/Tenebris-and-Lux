<!--
================================================================================
BLOCK 1: METADATA BLOCK
( Zero-Dep Go CPI-SI Operational Substrate CLI Overview, Topology & Test Law )
================================================================================
@project       OmniCode Framework / Big House Ecosystem
@file          .agents/tools/cpi-si/README.md
@id            cog.agent.tools.cpi_si_cli.readme
@layer         Portable Agent Substrate - Operational Go CLI Documentation
@archetype     tool_manual | four_language_line_and_micro_test_architecture
@version       0.1.3-dev
@interface_rev Rev 5
@created       2026-11-20
@updated       2026-11-20
@author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
@repository    https://github.com/Novaking34/OmniCode_Framework
@system        CPI-SI Portable Cognitive BIOS CLI & Substrate Tooling
@role          Document Go 1.26 Zero-Dep CLI Structure, Cross-OS Runners &
               1:1 4-Section Micro Test Suite Passing the OmniCode Eye Test
@maturity      Rev 5 Operational Baseline (Coordinate v0.1.3)
@chassis       zero-external-deps | Pure Go stdlib 1.26 + external cache build
@seams         bios: ../../README.md |
               mirror: ../../reports/architecture_and_verification/
                       mirroring_engine_tests_macro_micro_architecture.md |
               boot_skill: ../../skills/cpi-si-session-bootstrap/SKILL.md |
               entrypoint: ./cmd/cpi-si/main.go
@invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5, <=80 col walls).
               2. Zero compiled binaries inside .agents/ or repo root.
               3. Zero inline *_test.go inside substrate/ or cmd/; all tests
                  mirror cleanly inside tests/unit/ with 4-Section Micro depth.
               4. Pure 7-bit ASCII inside all code fences and diagrams.
================================================================================
-->

# `cpi-si` — Portable Go Substrate CLI (`tools/cpi-si/`)

> **Why Go Here Instead of Rust or Python:** Four languages carry clear lines in the sand across the Big House. **C23/C11** holds the zero-heap silicon floor (*Cornerstone*, `libtrit`, `MillenniumOS`). **Rust** powers compilers, lexers, and deep static validators (*OmniCode* toolchain). **0-dep Go (`1.26.8` stdlib)** owns sovereign networking (`FaithNet`) and portable operational substrate tooling (`tools/cpi-si/`) — compiling cross-platform in sub-second external cache passes with zero `__pycache__` or multi-GB `target/` clutter across `.agents/` mirrors. **Python** remains non-load-bearing research scratch only.

---

<!--
================================================================================
BLOCK 2: OPENING BLOCK
( Plain-English Orientation & Cross-Platform Package Structure )
================================================================================
-->

## Block 2 (Opening): Passing the OmniCode Eye Test

Every file inside `.agents/tools/cpi-si/` follows the exact structural standard already proven across `FaithNet` and `Cornerstone`:

* **Production Packages (`cmd/` & `substrate/`):** Universal 5-Block Go (`BLOCK 1..5`), dual-audience docstrings (*Architects/Non-Programmers* + *Systems Programmers*), Block 4 executable 4-micro-check `Preflight*()` gates, and Block 5 `Is*Ready()` seals — with **zero** `*_test.go` clutter mixed beside production files.
* **Cross-Platform Runners (`runners/`):** Linux/POSIX [`runners/run.sh`](runners/run.sh), macOS [`runners/macos/run.sh`](runners/macos/run.sh), Windows PowerShell [`runners/windows/run.ps1`](runners/windows/run.ps1), and Windows CMD [`runners/windows/run.cmd`](runners/windows/run.cmd) — all compiling outside `.agents/` into OS user caches (`$XDG_CACHE_HOME`, `~/Library/Caches`, `%LOCALAPPDATA%`).
* **Mirrored Micro Test Topology (`tests/`):** Strict 1:1 pairing inside `tests/unit/{cmd,substrate}/` backed by [`tests/common/fixtures.go`](tests/common/fixtures.go), dividing every test file into the four OmniCode micro sections (*Always Pass*, *Never Fail*, *Invariants & Limits*, *Emergence*).

---

<!--
================================================================================
BLOCK 3: BODY BLOCK
( Package Architecture, Cross-OS Runners & CLI Subcommands )
================================================================================
-->

## 1. Directory Topology (`tools/cpi-si/`)

```text
tools/cpi-si/
|-- go.mod                          <- 0 external deps (Go 1.26 stdlib only)
|-- README.md                       <- This Rev-5 operational tool guide
|-- cmd/cpi-si/main.go              <- Thin Layer-4 CLI entrypoint + Spec Card
|-- substrate/                      <- Exported cogs (ZERO inline *_test.go)
|   |-- types.go                    <- Layer 0 AuditSeverity & CheckResult
|   |-- paths.go                    <- Layer 1 .agents/ root & ~ resolution
|   |-- tail.go                     <- Layer 2 bounded Transfer_Session tail peek
|   |-- bench.go                    <- Layer 2 scratchpads/ bay triage
|   |-- skills.go                   <- Layer 2 Line-1 YAML parser & scope sorter
|   |-- boot.go                     <- Layer 3 compact < 2 KB BIOS boot card
|   |-- audit.go                    <- Layer 3 whole-.agents/ Rev-5 auditor
|   +-- cli.go                      <- Layer 3 verb router & 8-gate preflight
|-- runners/                        <- Cross-platform zero-clutter launchers
|   |-- run.sh                      <- Linux / universal POSIX bash 3.2+
|   |-- macos/run.sh                <- macOS ~/Library/Caches bash 3.2+
|   +-- windows/{run.ps1,run.cmd}   <- Windows PowerShell 5.1/7+ & CMD shim
+-- tests/                          <- 1:1 mirrored 4-section micro suite
    |-- common/fixtures.go          <- Shared assertions & sandbox builders
    +-- unit/{cmd,substrate}/       <- External test packages (no internal mixing)
```

---

## 2. Available CLI Subcommands

| Verb | Primary Layer / File | Purpose |
| :--- | :--- | :--- |
| `boot` | [`substrate/boot.go`](substrate/boot.go) | Prints compact `< 2 KB` BIOS orientation (`host/`, `operator/`, `silas_oren/`, skills, bench, tail tip) |
| `tail` | [`substrate/tail.go`](substrate/tail.go) | Streaming read-only `Transfer_Session` tail peek (`bufio.Reader`, `1<=pairs<=6`, `50<=max-chars<=1500`) |
| `bench` | [`substrate/bench.go`](substrate/bench.go) | Spatial triage across `.agents/scratchpads/` bays (`active/`, `report_outs/`, `studies/`, `bench-templates/`) |
| `skills` | [`substrate/skills.go`](substrate/skills.go) | Validates Line-1 YAML frontmatter and groups skills into *Cross-Cutting* vs. *Opt-In Domain* |
| `audit` | [`substrate/audit.go`](substrate/audit.go) | Verifies JSON 5-Block parity, Rev-5 Markdown/AsciiDoc rules, `<= 80`-col walls, ASCII fences, and links |
| `preflight` | [`substrate/cli.go`](substrate/cli.go) | Executes all 8 built-in Block 4 module self-audits in-memory / isolated temp sandboxes |
| `version` | [`substrate/types.go`](substrate/types.go) | Prints active `cpi-si-cli` coordinate and zero-dependency stdlib stamp |

---

<!--
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Verification Gates: Toolchain Hygiene, Micro Matrix & Cross-OS Parity )
================================================================================
-->

## Block 4 (Audit): Complete Tool Verification Suite

Run from `.agents/tools/cpi-si/`:

```bash
go list -m all
test -z "$(gofmt -l .)"
go vet ./...
go test -v -count=1 -race ./tests/...
GOOS=windows GOARCH=amd64 go build ./... && \
  GOOS=darwin GOARCH=arm64 go build ./... && \
  GOOS=darwin GOARCH=amd64 go build ./...
```

Lint and smoke-test all three OS runner families without writing binaries inside `.agents/`:

```bash
bash -n runners/run.sh && bash -n runners/macos/run.sh
./runners/run.sh --audit-only
./runners/macos/run.sh --audit-only
pwsh -NoProfile -File ./runners/windows/run.ps1 -AuditOnly
./runners/run.sh preflight
```

---

<!--
================================================================================
BLOCK 5: CLOSING BLOCK
( ACTION / REEL - Cross-Platform Seam, Linked Anchors & Closing Seal )
================================================================================
-->

## Block 5 (Closing): Operational Reel & Closing Seal

### `PLAY` — Daily Zero-Clutter Tool Operation

1. **Always Run Through `runners/`:** Invoke `.agents/tools/cpi-si/runners/run.sh` (POSIX/Linux), `runners/macos/run.sh` (macOS), or `runners/windows/run.ps1` (Windows) so compiled executables stay isolated in your OS cache rather than dirtying `.agents/` across 6 mirrors.
2. **Verify Micro Parity After Edits:** Any edit under `substrate/` or `cmd/` must update its 1:1 paired test inside `tests/unit/` across all four micro sections before advancing.

### `PROJECT` — Outward Ecosystem Seams

1. **Matches FaithNet & Cornerstone Stratum Law:** Shares FaithNet's zero-external-dep Go 1.26 discipline and mirrors Cornerstone's 4-section micro verification topology ([`mirroring_engine_tests_macro_micro_architecture.md`](../../reports/architecture_and_verification/mirroring_engine_tests_macro_micro_architecture.md)).
2. **Feeds Portable Cognitive BIOS Grounding:** Supplies deterministic `boot`, `tail`, `bench`, `skills`, and `audit` subcommands consumed across all six Big House `.agents/` mounts.

### `RECORD` — Linked Substrate Anchors

* Root Substrate Index: [`.agents/README.md`](../../README.md)
* Session Bootstrap Compass: [`cpi-si-session-bootstrap/SKILL.md`](../../skills/cpi-si-session-bootstrap/SKILL.md)
* Running Writing Bench Charter: [`bench_guide.md`](../../scratchpads/bench_guide.md)
* Empirical Test Mirror Architecture: [`mirroring_engine_tests_macro_micro_architecture.md`](../../reports/architecture_and_verification/mirroring_engine_tests_macro_micro_architecture.md)
* Entrypoint & Shared Test Fixtures: [`cmd/cpi-si/main.go`](cmd/cpi-si/main.go), [`tests/common/fixtures.go`](tests/common/fixtures.go)

### `COMMIT` — Closing Seal

* **Zero External Deps, Zero Binary Residue:** Pure Go stdlib compiled out-of-tree across Linux, macOS, and Windows with strict 1:1 micro test mirroring.
* **Status:** `@created 2026-11-20` / `@updated 2026-11-20`, `CPI_SI_TOOLS_EYE_TEST_ARCHITECTURE_READY (+1:PROCEED)`.

<!-- END OF BLOCK 5: CLOSING SEAL -->
