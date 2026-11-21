---
name: faithnet-workflows
description: >-
  Opt-in domain compass for building, testing, playtesting, and operating FaithNet (FNP/1),
  CreativeWorkzStudio LLC's sovereign offline-first adaptive networking substrate. Activate
  when working inside FaithNet/ or Track 4 networking seams; governs 0-external-deps
  Go 1.26.8 discipline, 512B Floppy Sector Invariant, 1:1 unit pairing, Block 4 gates,
  Zero-Brick host safety laws, Dual-Stack IPv4+IPv6 guard, and Sessions 001-008 laws.
---

<!--
================================================================================
BLOCK 1: METADATA BLOCK
( Opt-In Domain Compass for FaithNet FNP/1 Sovereign Adaptive Networking )
================================================================================
@project       OmniCode Framework / FaithNet Sovereign Substrate
@file          .agents/skills/faithnet-workflows/SKILL.md
@id            cog.agent.skill.faithnet_workflows
@layer         Portable Agent Substrate - Opt-In Domain Skill (FaithNet)
@archetype     domain_compass | sovereign_networking
@version       0.1.3-dev
@interface_rev Rev 5
@created       2026-11-17
@updated       2026-11-20
@author        CreativeWorkzStudio LLC / Seanje Lenox-Wise & Silas Oren
@repository    https://github.com/CreativeWorkzStudio/FaithNet
@system        FaithNet (FNP/1, Port 7777, faithnet://) + OmniCode Track 4 Seam
@role          Opt-In Engineering & Field Compass When Stepping Into FaithNet/
@maturity      Rev 5 Operational Skill (Phase I Waves 0-20 Exit Baseline)
@chassis       zero-external-deps | Pure Go 1.26.8 stdlib + 512B wire frames
@seams         bios: ../../README.md |
               covenant: ../cpi-si-covenant/SKILL.md |
               monograph: ../../reports/field_and_monographs/
                          faithnet_phase1_wave0_to_wave20_exit_monograph.adoc |
               day1: ../../reports/field_and_monographs/
                     faithnet_day1_field_reflections_and_synthesis.md |
               tools_line: ../../tools/cpi-si/README.md
@invariants    1. Universal 5-Block Lifecycle Protocol (Rev 5, <=80 col walls).
               2. Line-1 YAML frontmatter preserved for native discovery.
               3. Opt-In Domain Scope: activates for FaithNet/ & Track 4 work
                  without locking unrelated studio tracks into networking rules.
               4. 0-external-deps Go 1.26.8 stdlib (shared with tools/cpi-si/).
               5. Zero-Brick host safety laws & Dual-Stack IPv4+IPv6 firewall.
================================================================================
-->

# FaithNet Workflows & Sovereign Field Compass (`FNP/1`)

> **Project**: **FaithNet** (`FNP/1`, Port `7777`, `faithnet://`) | **Scope**: **Opt-In Domain Compass** (for work inside `FaithNet/` or `Track 4`)  
> **Lead Architect**: ProfessorSeanEX (Seanje Lenox-Wise) | **Studio**: CreativeWorkzStudio LLC  
> **Phase I Verified Ground**: Waves `0–20` complete (`359` `-race` tests, `27/27` live Block 4 gates, [`faithnet_phase1_wave0_to_wave20_exit_monograph.adoc`](../../reports/field_and_monographs/faithnet_phase1_wave0_to_wave20_exit_monograph.adoc))  
> **Core Philosophy**: *"Compatibility with the modern world — without dependency on it."*

---

<!--
================================================================================
BLOCK 2: OPENING BLOCK
( Why FaithNet Shares the 0-Dep Go Line in the Sand & Opt-In Domain Scope )
================================================================================
-->

## Block 2 (Opening): Opt-In Field & Lab Networking Compass

This compass is **opt-in for `FaithNet/` and OmniCode `Track 4` networking seams**. General planning, prose, identity, language, or game-world sessions do not live under networking constraints unless crossing into `FNP/1` transport or hardware-room playtesting.

Whenever work steps onto the `FaithNet/` floor, this codebase shares the exact same **0-external-dependency Go 1.26.8 standard library discipline** as our [`tools/cpi-si/`](../../tools/cpi-si/README.md) substrate CLI — pure stdlib, clean cross-compilation (`linux`, `windows`, `darwin`), strict 1:1 `tests/` micro pairing, and executable Block 4 pre-flight gates (`Preflight*()`).

---

<!--
================================================================================
BLOCK 3: BODY BLOCK
( Bedrock Laws, Verification Battery, Host Controls, 10 Field Laws & Seams )
================================================================================
-->

## 1. Non-Negotiable Bedrock Laws (`<>`)

1. **Zero External Dependencies:** `go list -m all` inside `FaithNet/` MUST list ONLY `github.com/CreativeWorkzStudio/FaithNet` (`100%` Go stdlib). Cross-compilation MUST stay green for `linux/amd64`, `windows/amd64`, `darwin/arm64`, and `darwin/amd64`.
2. **Downward Dependency Law:** Imports flow strictly downward (`Layer 4 -> 3 -> 2 -> 1 -> 0`). **Layer 0 (`data/`) and Layer 1 (`engine/`, `engine/covenant/`) NEVER import `net`, `os`, or external packages.**
3. **512-Byte Floppy Sector Invariant:** Every `FNP/1` wire frame is a fixed `32B` header (`0xFA17`, `10` canonical opcodes `0x01..0x0A`, $\mathbb{T}_4$ `QuadState`, FNV-1a seal) + `<= 480B` inline payload (`ALLOC_ZERO_HEAP` hot paths, `0.00 allocs/op`).
4. **OmniCode Submodule Ternary Seam (`engine/trit.go`):** Lossless `0 allocs/op` Bit <-> Balanced-Trit <-> Bit conversion (`Tryte5`: `5` trits = `243` states in `0x00..0xF2`, `13` reserved sentinels incl. `0xFF = NULL`; `1` binary byte <-> `6` balanced trits).
5. **Universal 5-Block Protocol & 1:1 Unit Pairing:** Every `.go`, `.sh`, `.ps1`, `.adoc`, and `.md` file uses 5-Block Rev 5 framing. Every production Go module pairs 1:1 into `tests/unit/` alongside its Block 4 `Preflight*()` gate (Phase I baseline: `30:30` paired, `27/27` live gates in `faithnet-node --audit`).
6. **Legal & Zero-Brick Host Safety Law:**
   * Never permanently mutate NetworkManager profiles, default `iptables` policies, or system DNS defaults.
   * Never bypass or spoof commercial ISPs; legacy bridging (`bridge` mode) only ever uses the host owner's own authorized uplink behind a dual-stack IPv4+IPv6 firewall wall.
   * Always run persistent host daemons via native `systemd` (`faithnet-home.service` user unit + `faithnet-ap.service` system unit) — **never** leave hanging background chat tasks.

---

## 2. Full Verification Battery (`FaithNet/` Root)

Run from inside `FaithNet/` before sealing any networking wave:

```bash
# 1. Confirm 0 external deps & Downward Dependency Law
go list -m all
go list -f '{{.ImportPath}} -> {{.Imports}}' ./data ./engine ./engine/covenant

# 2. Static analysis & 4x4 Matrix with -race (Phase I exit: 359 tests, 0 races)
go vet ./...
go test -v -count=1 -race ./tests/...

# 3. Live Block 4 pre-flight audit (Phase I exit: 27/27 gates)
go run ./cmd/faithnet-node --audit

# 4. Cross-platform Go compile parity
GOOS=windows GOARCH=amd64 go build ./data/... ./engine/... ./orchestrator/... ./transport/... ./interpreter/... ./cmd/...
GOOS=darwin  GOARCH=arm64 go build ./data/... ./engine/... ./orchestrator/... ./transport/... ./interpreter/... ./cmd/...
GOOS=darwin  GOARCH=amd64 go build ./data/... ./engine/... ./orchestrator/... ./transport/... ./interpreter/... ./cmd/...

# 5. Validate all AsciiDoc docs & field reports (0 warnings!)
for f in $(find docs tests/playtester -name '*.adoc' | sort); do
  asciidoctor --failure-level=WARN -o /dev/null "$f" || exit 1
done

# 6. Shell & Windows PowerShell parity lint
for s in $(find tests/playtester/field_kits -name '*.sh'); do bash -n "$s" || exit 1; done
for p in $(find tests/playtester/field_kits -name '*.ps1'); do /snap/bin/pwsh -NoProfile -File "$p" -AuditOnly >/dev/null || exit 1; done
```

---

## 3. Live Sovereign Host Controls (`Inspiron-16-5640`)

```bash
# Inspect Sovereign Host Profile (Radio mode, Split-DNS, Dual-Stack WAN Guard, Peers)
faithnet-netctl status
faithnet-netctl adapters                 # Auto-detects 2nd USB Wi-Fi adapter (DEDICATED_RADIO)

# Live native ZERO-HTTP FNP/1 fetches directly over Port 7777/udp (~0.26 ms verified)
faithnet-node --fetch=faithnet://faithnet.lan/well
faithnet-open faithnet://faithnet.lan/status
faithnet-open faithnet://faithnet.lan/protocol
faithnet-open faithnet://faithnet.lan/bulletins

# Instant zero-reboot commercial WAN egress toggle (dual-stack IPv4 + IPv6)
sudo faithnet-netctl mode island         # Blocks IPv4+IPv6 WAN; HomeRoom stays 100% live
sudo faithnet-netctl mode bridge         # Restores outbound WAN; inbound still firewalled

# Emergency non-destructive off-switch (leaves wlp3s0 100% untouched)
sudo systemctl stop faithnet-ap
```

---

## 4. The 10 Hard-Won Physical Field Laws (Sessions 001–008)

1. **Multi-Sibling Ladder (`[5][4]`):** Never overwrite sibling mediums on the same rung (e.g., SLIP vs. AFSK vs. Morse on Rung 1).
2. **Sole-Link Retention:** Passive silent-blackhole cooldown (`MarkRungDegraded`) MUST NEVER quarantine the *last* surviving link; reply `OpPong`/`OpResource` via `SendOnRung(in.Rung)`.
3. **Sliding Acoustic Preamble Sync:** Real mics have unpredictable leading silence; always use the energy-gated sliding correlator.
4. **Dual-Mode `0x16` TLS Peek + 3-Tier TLS Ladder:** Modern browsers (Firefox HTTPS-Only) force HTTPS on plain ports; always peek byte `0x16` and persist Tier 2 Sovereign Root CA (`10.77.77.1`, `faithnet.lan`, `faithnet.home.arpa`).
5. **RFC 6762 `.local` mDNS Trap:** Never rely solely on `.local` for unicast DNS — iOS and Ubuntu hijack `.local` for mDNS. Serve unicast **`faithnet.lan`** and **`faithnet.home.arpa`** (RFC 8375).
6. **Clean REPL UX:** Announce `[PEER DISCOVERED]` once per `NodeID`; never spam periodic 3s beacons over interactive typing.
7. **`/24` Broadcast Deduplication:** Use `coveredByBroadcast()` in `UDPTransceiver.Send(Broadcast)` so peers on the same subnet never receive duplicate frames.
8. **Yield-Before-Fight on Single-Channel Wi-Fi (`rtw89` / `RTL8852CE`):** Never race `wlp3s0` channel switches with slow polling; use real-time `exec 3< <(nmcli device monitor wlp3s0)` to release `ap0` immediately, and bring `ap0` up via `hostapd` (`nl80211`) before `ip addr add`. See [`host_pc_hardware_profile.md`](../../host/host_pc_hardware_profile.md).
9. **Boot Persistence:** Both `faithnet-home.service` and `faithnet-ap.service` MUST auto-boot cleanly after reboot.
10. **Dual-Stack IPv4 + IPv6 Firewalling:** Whenever sealing a WAN interface (`FAITHNET_WAN_GUARD` or `island` egress), ALWAYS apply matching **`ip6tables`** rules alongside `iptables` — commercial routers hand out global IPv6 addresses that silently bypass IPv4-only rules.

---

## 5. Cross-Stack Integration Seam (FaithNet + OmniCode + Cornerstone)

* **Into OmniCode (`Track 1` / `Track 4`):** `data/` + `engine/` (zero `net`/`os` imports) plug directly into OmniCode's runtime via `engine/trit.go` (`Bit <-> Balanced-Trit`) and `engine/uri.go` (`FNP/1` `faithnet://`), turning balanced-ternary state into wire-ready 512-byte sectors.
* **Into Cornerstone (`Track 6` C23/C11):** Because our 32-byte `0xFA17` wire header is fixed-layout big-endian with FNV-1a (`512B` total, `0` heap allocs), a lightweight `ALLOC_ZERO_HEAP` C11/C23 codec in Cornerstone speaks bit-for-bit identical `FNP/1` frames with Go FaithNet nodes ([`omnicode_architecture_and_vision.md`](../../plans/vision/omnicode_architecture_and_vision.md)).

---

<!--
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Domain Gate Checks Before Advancing Any FaithNet Wave )
================================================================================
-->

## Block 4 (Audit): Pre-Commit Networking Gates

Before reporting any `FaithNet/` change green, confirm all four directly from command output:

1. **Dependency & Layer Purity:** `go list -m all` shows only `github.com/CreativeWorkzStudio/FaithNet` and `data/` + `engine/` import zero OS/net libraries.
2. **Full `-race` + `--audit` Pass:** `go test -v -count=1 -race ./tests/...` AND `go run ./cmd/faithnet-node --audit` exit `0`.
3. **Zero-Brick Verification:** `git diff` leaves NetworkManager, default system DNS, and host firewall defaults clean on teardown; field-kit `.sh` + `.ps1` pass `bash -n` and `pwsh -AuditOnly`.
4. **Substrate Health Check:** Whenever `.agents/` files were touched alongside FaithNet work, run `.agents/tools/cpi-si/runners/run.sh audit` from OmniCode root.

---

<!--
================================================================================
BLOCK 5: CLOSING BLOCK
( ACTION / REEL - Phase II Handover Posture, Seams, Anchors & Seal )
================================================================================
-->

## Block 5 (Closing): Operational Reel & Closing Seal

### `PLAY` — Current Position (`Phase I Sealed -> Phase II Waves 21–24 Ready`)

1. **Opt-In Domain Activation:** Step into these constraints strictly when working inside `FaithNet/` or `Track 4`; yield cleanly back to cross-cutting skills when stepping elsewhere.
2. **Phase I Empirical Proof Intact:** All `359` `-race` tests and `27/27` live Block 4 gates across Waves 0–20 (`27` commits `d7a80d0..f37e7dc`) remain our clean departure board ([`faithnet_phase1_wave0_to_wave20_exit_monograph.adoc`](../../reports/field_and_monographs/faithnet_phase1_wave0_to_wave20_exit_monograph.adoc)).

### `PROJECT` — Outward Ecosystem Seams

1. **Cross-Pollination With `tools/cpi-si/`:** FaithNet established the exact Go 5-Block + external `tests/unit/` 4-section pattern now mirrored cleanly across our Go CLI tooling in [`tools/cpi-si/`](../../tools/cpi-si/README.md).
2. **Wave B Integration (`Track 4`):** Supplies supervised `FNP/1` IPC/512B transport to OmniCode clean-room Track 4 without relinquishing standalone repo sovereignty.

### `RECORD` — Linked Substrate Anchors

* Phase I Exit Monograph & Day 1 Notes: [`faithnet_phase1_wave0_to_wave20_exit_monograph.adoc`](../../reports/field_and_monographs/faithnet_phase1_wave0_to_wave20_exit_monograph.adoc), [`faithnet_day1_field_reflections_and_synthesis.md`](../../reports/field_and_monographs/faithnet_day1_field_reflections_and_synthesis.md)
* Host Radio & Calibration Hardware: [`host_pc_hardware_profile.md`](../../host/host_pc_hardware_profile.md), [`hardware_calibration.json`](../../host/hardware_calibration.json)
* Cross-Repo Seams: [`omnicode_architecture_and_vision.md`](../../plans/vision/omnicode_architecture_and_vision.md), [`cornerstone-workflows`](../cornerstone-workflows/SKILL.md), [`cpi-si-substrate-carpentry`](../cpi-si-substrate-carpentry/SKILL.md)

### `COMMIT` — Closing Seal

* **Sovereign Connectivity Without Dependency:** Every wire frame fits inside a 512-byte sector, zero external Go modules ever enter `go.mod`, and physical reality outranks lab theory.
* **Shared 0-Dep Go Lineage:** Applies the exact same zero-dep stdlib, `-race`, and Block 4 `Preflight*()` engineering shared with `.agents/tools/cpi-si/`.
* **Domain Opt-In Guard:** Activate cleanly for `FaithNet/` and `Track 4` work while leaving non-networking sessions unencumbered.
* **Verification:** Rev-5 `<= 80`-col walls, Line-1 YAML discovery, and zero `.agents/`-escaping links enforced (`2026-11-20`).
* **Status:** `@created 2026-11-17` / `@updated 2026-11-20`, `FAITHNET_WORKFLOWS_REV5_UPGRADED (+1:PROCEED)`.

<!-- END OF BLOCK 5: CLOSING SEAL -->
