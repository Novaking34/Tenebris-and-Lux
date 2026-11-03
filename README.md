---
project_title: "Tenebris and Lux"
subtitle: "An Atmospheric Tactical Strategy RPG & Custom Engine Architecture Written in C"
version: "0.0.1"
organization: "CreativeWorkzStudio LLC"
repository: "https://github.com/Novaking34/tenebris-lux"
license: "MIT"
---

# Tenebris and Lux

> **"The Shield protects and confines; the Sword severs and defends. The metal knows neither virtue nor malice—only the hand that grips the hilt."**  
> *"Trauma is Inherited; Destiny is Chosen."*

*Tenebris and Lux* ("Darkness and Light") is a custom-engineered **Tactical Strategy RPG (TRPG)** and C11 game engine architecture. Conceived between 2014–2018 in RPG Maker as the foundational project of **CreativeWorkzStudio**, the story chronicles the defense of the chivalric **Kingdom of Sacyra** (pronounced *"saw-kee-ruh"*, `/sɔːˈkiːrə/`) and its neighboring continental powers against the re-awakened **Dark Lord** (the ancient Libiran sovereign Usuru).

The campaign features a **Tri-Perspective Narrative Architecture** exploring three interconnected temporal axes:

* **The Past (Healing the Frontier Wound)**: **Elijah Ro-enheart**, frontline heavy vanguard of the Royal Defense Corps bearing the **Shield of Lux**, seeking truth behind the disappearance of his father, Commander Gareth Ro-enheart.
* **The Present (Severing Complacency)**: **Lady Katrina Jaren**, noble court aspirant renouncing aristocratic privilege for the **Adventurers' Guild**, wielding the **Sword of Tenebris** to cut through courtly decay.
* **The Future (The Steward of Balance)**: **The Custom Bearer** (`<chosen_name>`), an unburdened soul awakening within the ethereal **Dream Chamber** under the guidance of **Chrono**, free from ancestral grudges to forge an unwritten destiny.

---

## Manifest Metadata & Covenant Identity

| Attribute | Specification |
| :--- | :--- |
| **Engine Version** | `0.0.1` (Interactive Prototype Milestone) |
| **Genre** | Tactical Strategy RPG (TRPG) — 2D Grid Battlefield + Cinematic Side-View Duels |
| **Organization** | CreativeWorkzStudio LLC |
| **Repository** | [https://github.com/Novaking34/tenebris-lux](https://github.com/Novaking34/tenebris-lux) |
| **Core Standard** | ISO C11 (`-std=c11`, zero heap allocations in Layer 0/1, zero external runtime dependencies) |
| **Build System** | CMake 3.24+ (C11 engine) & Rust 2021 (Master Guide compilation toolchain) |
| **Structural Paradigm** | OmniCode Universal 5-Block Code Lifecycle Protocol |
| **Layered Strata** | Layer 0 (Core Util), Layer 1 (Sim Engine), Layer 2 (GUI), Layer 3 (Entry), Layer 4 (Tests) |
| **Author / Lead** | ProfessorSeanEX / CreativeWorkzStudio LLC |
| **Documentation** | 63-Chapter Unified Compendium ([`MASTER_GUIDE.adoc`](docs/MASTER_GUIDE.adoc), [`MASTER_GUIDE.md`](docs/MASTER_GUIDE.md), [`MASTER_GUIDE.pdf`](docs/MASTER_GUIDE.pdf)) |
| **License** | MIT License |

---

## 1. Project Vision, Metaphysics & Continental Lore

In *Tenebris and Lux*, Light (Lux) and Darkness (Tenebris) are not moral absolutes of good and evil; they are primal cosmic forces personified. Morality exists solely in the **intent of the wielder** (`moral_sway` in `[-100, +100]`).

### The Three Thematic Pillars

* **The Past**: Parallels between ancient **Usuru the Wanderer** and **Elijah Ro-enheart**—examining how unattended generational neglect on the frontier curdles into volcanic grief.
* **The Present**: Parallels between founding **Prince Casatti** and **Lady Katrina Jaren**—demonstrating the necessity of decisive severance against aristocratic hypocrisy.
* **The Future**: Parallels between founding **Prince Mi-ren** and **The Custom Bearer**—bridging institutional order with individual agency to heal the 3,000-year dynastic chasm.

### Geopolitical Landscape & The Six Powers

The game is set across the continental expanse governed by the historic **Accord of Kings** (*"Joint Realm, Separate Kingdoms"*):

1. **The Royal Capital of Sacyra**: Central white-stone citadel of the High Throne, spiritual center of Lux.
2. **The Kingdom of Libira**: Arid western frontier, birthplace of Usuru, home of "The Quiet Complaint".
3. **The Great Mau Kingdom**: Eastern river merchant empire and mercenary banking cartels.
4. **Old Sacyra**: Highland ancestral cradle of pureblood Sacyran traditionalists.
5. **The Academy**: Northern glacial spire dedicated to arcanotech and empirical science.
6. **The Yun-yin Provinces**: Southeastern misty archipelago mastering spiritual balance and ki flow.

---

## 2. The Universal 5-Block Architectural Standard

Inheriting the structural discipline of the **OmniCode** framework, every source module, header, and configuration file in this repository is partitioned into 5 explicit blocks:

```text
┌────────────────────────────────────────────────────────────────────────┐
│                        BLOCK 1: METADATA BLOCK                         │
│  Identity, Provenance, Invariant Boundary Contracts & System Anchors   │
├────────────────────────────────────────────────────────────────────────┤
│                    BLOCK 2: OPENING / TYPES BLOCK                      │
│  Includes, Preconditions, Enums, State Structures & Global Constants   │
├────────────────────────────────────────────────────────────────────────┤
│                         BLOCK 3: BODY BLOCK                            │
│  Core Logic, State Transitions, Execution Routines & Gameplay Loop     │
├────────────────────────────────────────────────────────────────────────┤
│                   BLOCK 4: AUDIT PROCEDURES BLOCK                      │
│  Diagnostic Audits, State Invariant Verifications & Telemetry Traces   │
├────────────────────────────────────────────────────────────────────────┤
│                BLOCK 5: CLOSING / TEST & CLI BLOCK                     │
│  CLI Entry Point, Automated Invariant Unit Tests & Execution Teardown  │
└────────────────────────────────────────────────────────────────────────┘
```

For in-depth architectural specifications, see [ARCHITECTURE.adoc](docs/architecture/ARCHITECTURE.adoc) and [dual_frontend_architecture.adoc](docs/architecture/dual_frontend_architecture.adoc).

---

## 3. Campaign Structure & The 5-Route Ending Matrix

* **Two-Half Campaign Architecture**:
  * **Act I (The Frontier of Whispers)**: Awakening in the Dreamscape, Dio Village, Whispering Woods, and Sacyran Marches.
  * **Midpoint Campaign Fork**: Confronting **The King's Curse** (the open planar wound beneath the High Throne). Entering without the calibrated *Dream Compass* triggers the tragic *Neutral Path*; possessing the Compass unlocks the *True Ending Paths*.
* **The 5 Deterministic Ending Routes**:
  1. *Path of Radiance (Lux Overload)*: Total moral sway $\ge +75$; light crystallization.
  2. *Path of the Abyssal Sovereign (Tenebrous Eclipse)*: Total moral sway $\le -75$; absolute dominion over shadow.
  3. *The Sovereign Accord (Dual Harmonic Balance)*: Equilibrium maintained `[-25, +25]`; renewal of the Accord of Kings.
  4. *The Cycle Shattered (Planar Dissolution)*: Transcending both relics; healing the 3,000-year rift forever.
  5. *The Forgotten Stasis (The King's Slumber)*: Default neutral closure; perpetuation of the dynastic cycle.

---

## 4. Roadmap & Development Milestones

* [x] **Phase 1: Dual-Frontend Prototype (Current - v0.0.1)**
  * Pure C11 interactive simulation with zero dynamic heap allocations in core layers.
  * Native double-buffered X11 GUI and headless/terminal CLI running side-by-side.
  * Tri-perspective state modeling (Elijah, Katrina, Custom Bearer).
  * Layer 0 Core Utilities (`manifest`, `math_utils`, `string_utils`, `geom_utils`, `time_utils`, `log`).
  * 12-Module Pre-Flight Subsystem Integrity Check (`--audit`) reporting graded telemetry manifests.
  * Automated diagnostic audit suite and CTest integration (7/7 test suites passing).
  * Standalone Rust developer tooling (`tools/src/guide.rs`) compiling 63 chapters into PDF, AsciiDoc, and Markdown.
* [ ] **Phase 2: Tactical Strategy RPG Grid Engine**
  * Discrete 2D grid coordinates, movement budgets, terrain defense modifiers, Zone-of-Control (ZOC).
  * Party formation management across Sacyran provinces.
* [ ] **Phase 3: Cinematic Side-View Combat Resolution**
  * Grid-to-duel scene transition, 2D side-scroll action clashes, timed warding/strikes, poise meters.
* [ ] **Phase 4: Full Continental Campaign**
  * Tactical campaign across the Six Powers culminating in the planar convergence against The Dark Lord.

---

## 5. Quickstart: Build, Play & Compile Docs

For detailed prerequisites, operating system setup, and troubleshooting, refer to [INSTALL.adoc](docs/INSTALL.adoc).

### Quick Build with CMake

```bash
# 1. Configure the build directory
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

# 2. Compile the game engine and verification test suites
cmake --build build --config Debug

# 3. Launch the game (auto-detects native GUI, or falls back to CLI)
./build/tenebris-lux

# Launch explicitly in terminal CLI mode
./build/tenebris-lux --cli

# Launch explicitly in native desktop GUI mode
./build/tenebris-lux --gui
```

### Compile the Unified Master Guide (63 Chapters)

```bash
# Requires Rust toolchain (cargo) and asciidoctor-pdf
cargo run --manifest-path tools/Cargo.toml --bin master_guide
```

### In-Game Commands & Hotkeys

* `look` / **(L)**: Inspect current location, faction tensions, and surroundings.
* `status` / **(I)**: Inspect the Shield of Lux, Sword of Tenebris, Stance, and Moral Sway.
* `ward` / **(W)**: Raise the Shield of Lux to establish a radiant barrier dome (*Lux Barrio*).
* `strike` / **(S)**: Unleash the Sword of Tenebris to execute sweeping arcs (*Cuttatio Tenebris*).
* `balance` / **(B)**: Seek harmony between the twin relics (*Duo-Loca Bashin n Slash*).
* `stance <ward|strike|equilibrium>` / **(T)**: Shift combat and metaphysical stance.
* `move <n|s|e|w>` / **(N, S, R, W)**: Travel across realms (Dreamscape, Dio Village, Woods, Marches).
* `name <bearer>`: Designate bearer identity (Elijah, Katrina, or Custom).
* `help`: View all available commands.
* `quit` / **(Q)**: Exit the game.

### Automated Diagnostics & Testing

```bash
# Run pre-flight inspection and internal self-audit (12 registered modules + 6 invariant tests)
./build/tenebris-lux --audit

# Run full CTest automated verification suite (7/7 tests pass)
ctest --test-dir build -C Debug --output-on-failure
```

---

## 6. Repository Documentation Index

* **[docs/MASTER_GUIDE.adoc](docs/MASTER_GUIDE.adoc)**: The complete 63-chapter unified AsciiDoc compendium.
* **[docs/MASTER_GUIDE.pdf](docs/MASTER_GUIDE.pdf)**: Production PDF publication book compiled via `asciidoctor-pdf`.
* **[docs/README.adoc](docs/README.adoc)**: Central documentation portal and index.
* **[docs/INSTALL.adoc](docs/INSTALL.adoc)**: Comprehensive compilation, installation, and troubleshooting guide.
* **[docs/architecture/](docs/architecture/README.adoc)**: System design, 5-block lifecycle, and dual-frontend engine.
* **[docs/lore/](docs/lore/README.adoc)**: Macro-chronology, 20-scene master storyboard, thematic triad, character dossiers, and realm chronicles.
* **[docs/game_design/](docs/game_design/README.adoc)**: TRPG grid mechanics, campaign structure, relics, and 2017–2018 demo baseline.
* **[docs/dev_logs/](docs/dev_logs/README.adoc)**: Living engineering chronology (DevLogs 001–009 across October and November 2026).
* **[docs/code/](docs/code/README.adoc)**: Subsystem implementation tours for Layer 0, Layer 1, Layer 2, and Layer 4.
* **[docs/offline_work/](docs/offline_work/README.adoc)**: Archived genesis notes, storyboard transcriptions, and macro-architecture drafts.
* **[AGENTS.md](AGENTS.md)**: Repository guidelines, OmniCode invariants, and cognitive anchor standards.
