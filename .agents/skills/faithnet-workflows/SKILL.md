---
name: faithnet-workflows
description: >-
  Operational compass for building, testing, playtesting, and operating FaithNet (FNP/1),
  CreativeWorkzStudio LLC's sovereign offline-first adaptive networking substrate. Governs
  the 0-external-deps Go 1.26.8 discipline, 512B Floppy Sector Invariant, 30:30 1:1 unit
  pairing, 24 live Block 4 gates, Zero-Brick host safety laws, Dual-Stack IPv4+IPv6 WAN
  guard, and the 10 hard-won field lessons from Sessions 001-008.
---

# FaithNet Workflows & Sovereign Field Compass (`FNP/1`)

> **Project**: **FaithNet** (`FNP/1`, Port `7777`, `faithnet://`)
> **Studio**: CreativeWorkzStudio LLC (Sovereign Publishing House & In-House Stack)
> **Lead Architect**: ProfessorSeanEX (Seanje Lenox-Wise)
> **Governance Revision**: `Rev 5` | **Core Philosophy**: *"Compatibility with the modern world—without dependency on it."*

This skill is the living operational compass for developing, verifying, and running **FaithNet** across lab, local-OS playtest harnesses, and live physical hardware.

---

## 1. Non-Negotiable Bedrock Laws (`<>`)

1. **Zero External Dependencies:** `go list -m all` MUST list ONLY `github.com/CreativeWorkzStudio/FaithNet` (100% Go standard library). Cross-compilation MUST stay green for `linux/amd64`, `windows/amd64`, `darwin/arm64`, and `darwin/amd64`.
2. **Downward Dependency Law:** Imports flow strictly downward (`Layer 4 -> 3 -> 2 -> 1 -> 0`). **Layer 0 (`data/`) and Layer 1 (`engine/`, `engine/covenant/`) NEVER import `net`, `os`, or external packages.**
3. **512-Byte Floppy Sector Invariant:** Every `FNP/1` wire frame is a fixed `32B` header (`0xFA17`, `10` canonical opcodes `0x01..0x0A`, $\mathbb{T}_4$ `QuadState`, FNV-1a seal) + `<= 480B` inline payload (`ALLOC_ZERO_HEAP` hot paths, `0.00 allocs/op`).
4. **OmniCode Submodule Ternary Seam (`engine/trit.go`):** Lossless `0 allocs/op` Bit <-> Balanced-Trit <-> Bit conversion (`Tryte5`: `5` trits = `243` states in `0x00..0xF2`, `13` reserved sentinels incl. `0xFF = NULL`; `1` binary byte <-> `6` balanced trits).
5. **Universal 5-Block Lifecycle Protocol (Rev 5) & Strict 1:1 Unit Pairing:** Every `.go`, `.sh`, `.ps1`, `.adoc`, and `.md` file uses 5-Block Rev 5 structure. Every production Go file has a strict 1:1 Tier-2 unit test (`30:30` paired) and Block 4 `Preflight*()` gate (`24/24` live gates in `faithnet-node --audit`).
6. **Legal & Zero-Brick Host Safety Law:**
   * Never permanently mutate NetworkManager profiles, default `iptables` policies, or system DNS defaults.
   * Never bypass or spoof commercial ISPs; legacy bridging (`bridge` mode) only ever uses the host owner's own authorized uplink behind a dual-stack IPv4+IPv6 firewall wall.
   * Always run persistent host daemons via native `systemd` (`faithnet-home.service` user unit + `faithnet-ap.service` system unit)—**never** leave hanging background chat tasks!

---

## 2. Full Verification Battery (Run Before Sealing Any Wave)

```bash
# 1. Confirm 0 external deps & Downward Dependency Law
go list -m all
go list -f '{{.ImportPath}} -> {{.Imports}}' ./data ./engine ./engine/covenant

# 2. Static analysis & 4x4 Matrix with -race (48 files, 346+ tests, 0 races)
go vet ./...
go test -v -count=1 -race ./tests/...

# 3. Live 24-gate Block 4 pre-flight audit
go run ./cmd/faithnet-node --audit

# 4. Cross-platform Go compile parity
GOOS=windows GOARCH=amd64 go build ./data/... ./engine/... ./orchestrator/... ./transport/... ./interpreter/... ./cmd/...
GOOS=darwin  GOARCH=arm64 go build ./data/... ./engine/... ./orchestrator/... ./transport/... ./interpreter/... ./cmd/...
GOOS=darwin  GOARCH=amd64 go build ./data/... ./engine/... ./orchestrator/... ./transport/... ./interpreter/... ./cmd/...

# 5. Validate ALL AsciiDoc documentation & reports (0 warnings!)
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

# Live native ZERO-HTTP FNP/1 fetches directly over Port 7777/udp (~0.26–0.68 ms!)
faithnet-node --fetch=faithnet://faithnet.lan/well
faithnet-open faithnet://faithnet.lan/status
faithnet-open faithnet://faithnet.lan/protocol
faithnet-open faithnet://faithnet.lan/bulletins

# Instant zero-reboot commercial WAN egress toggle (dual-stack IPv4 + IPv6!)
sudo faithnet-netctl mode island         # Blocks IPv4+IPv6 WAN; HomeRoom stays 100% live
sudo faithnet-netctl mode bridge         # Restores outbound WAN; inbound still firewalled

# Emergency non-destructive off-switch (leaves wlp3s0 100% untouched)
sudo systemctl stop faithnet-ap
```

---

## 4. The 10 Hard-Won Physical Field Laws (Never Repeat These Bugs!)

1. **Multi-Sibling Ladder (`[5][4]`):** Never overwrite sibling mediums on the same rung (e.g., SLIP vs. AFSK vs. Morse on Rung 1).
2. **Sole-Link Retention:** Passive silent-blackhole cooldown (`MarkRungDegraded`) MUST NEVER quarantine the *last* surviving link; reply `OpPong`/`OpResource` via `SendOnRung(in.Rung)`.
3. **Sliding Acoustic Preamble Sync:** Real mics have unpredictable leading silence; always use the energy-gated sliding correlator (`0..4,000` samples verified).
4. **Dual-Mode `0x16` TLS Peek + 3-Tier TLS Ladder:** Modern browsers (Firefox HTTPS-Only) force HTTPS on plain ports; always peek byte `0x16` and persist Tier 2 Sovereign Root CA (`10.77.77.1`, `faithnet.lan`, `faithnet.home.arpa`).
5. **RFC 6762 `.local` mDNS Trap:** Never rely solely on `.local` for unicast DNS—iOS and Ubuntu hijack `.local` for mDNS! Always serve unicast **`faithnet.lan`** and **`faithnet.home.arpa`** (RFC 8375).
6. **Clean REPL UX:** Announce `[PEER DISCOVERED]` once per `NodeID`; never spam periodic 3s beacons over interactive typing.
7. **`/24` Broadcast Deduplication:** Use `coveredByBroadcast()` in `UDPTransceiver.Send(Broadcast)` so peers on the same subnet never receive duplicate frames.
8. **Yield-Before-Fight on Single-Channel Wi-Fi (`rtw89`):** Never race `wlp3s0` channel switches with slow polling; use real-time `exec 3< <(nmcli device monitor wlp3s0)` to release `ap0` *immediately* on `connecting`/`disconnected`, and bring `ap0` up via `hostapd` (`nl80211`) *before* `ip addr add`.
9. **Boot Persistence:** Both `faithnet-home.service` and `faithnet-ap.service` MUST auto-boot cleanly so paired devices reconnect automatically after reboot.
10. **Dual-Stack IPv4 + IPv6 Firewalling:** Whenever sealing a WAN interface (`FAITHNET_WAN_GUARD` or `island` egress), ALWAYS apply matching **`ip6tables`** rules alongside `iptables`—commercial routers hand out global IPv6 addresses that silently bypass IPv4-only rules!

---

## 5. Cross-Stack Integration Seam (FaithNet + OmniCode + Cornerstone)

* **Into OmniCode (Submodule):** `data/` + `engine/` (zero `net`/`os` imports) plug directly into OmniCode's runtime via `engine/trit.go` (Bit <-> Balanced-Trit <-> Bit) and `engine/uri.go` (`FNP/1` `faithnet://`), turning balanced-ternary state into wire-ready 512-byte sectors.
* **Into Cornerstone Engine (C23/C11):** Because our 32-byte `0xFA17` wire header is fixed-layout big-endian with FNV-1a (`512B` total, `0` heap allocs), a ~150-line `ALLOC_ZERO_HEAP` C11/C23 codec in Cornerstone speaks **bit-for-bit identical `FNP/1` frames** with Go FaithNet nodes—unlocking zero-cloud local co-op, LAN save/mod sharing, live studio playtesting over `FaithNet-HomeRoom`, and offline publishing distribution for *Tenebris and Lux*!
