---
name: "Game Bug / Engine Tripwire Report"
about: "Report a crash, combat softlock, visual artifact, or relic resonance invariant breach"
title: "[BUG]: "
labels: ["bug", "game-engine"]
assignees: []
---

# Bug Report

## 1. Protagonist & Realm Context

- **Active Hero**: [ ] Elijah Ro-enheart (Shield) | [ ] Lady Katrina Jaren (Sword) | [ ] Custom Bearer
- **Current Realm**: (e.g., The Dreamscape, Dio Village, Whispering Woods, Sacyran Marches)
- **Active Combat Stance**: [ ] Neutral | [ ] Warding | [ ] Striking | [ ] Equilibrium
- **Interface Mode**: [ ] Native X11 Desktop GUI | [ ] Terminal CLI (`--cli`)

## 2. Issue Description & Reproduction Steps

A clear, concise description of the anomalous behavior encountered during simulation:

1. Ran command `...`
2. Executed combat action `...`
3. Observed unexpected behavior: `...`

## 3. Expected vs. Actual Behavior

- **Expected**: (e.g., Shield of Lux absorbs 25% damage and clamps within [0, 100])
- **Actual**: (e.g., Relic resonance underflowed or game looped infinitely)

## 4. Forensic Telemetry & Debugger Trace

If reproducible via the interactive CLI debugger (`./build/tenebris-lux --debug`) or self-audit (`./build/tenebris-lux --audit`):

```text
(Paste diagnostic output, error log from logs/tenebris_lux_dev.log, or inspect snapshot here)
```

## 5. System Environment

- **OS / Distribution**: Linux (Kernel version, Distro)
- **Display Server**: X11 / Wayland / Headless TTY
- **Build Configuration**: CMake Debug / Release
