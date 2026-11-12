<!--
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance & Narrative / System Series Anchors )
================================================================================
@project       Tenebris and Lux
@file          docs/offline_work/Offline-Work-5.md
@id            docs.offline_work.part_5.md
@layer         Documentation / Pre-Production Chronicles (Part 5 of 6)
@version       0.2.0-dev
@date          2026-11-12
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@series        Previous: Offline-Work-4.md | Index: README.adoc | Next: Offline-Work-6.md
================================================================================
-->

> **Series Navigation (Markdown Mirror):**
> [Series Index (AsciiDoc)](README.adoc) | [← Part 4: Master Stat Sheet](Offline-Work-4.md) | **Part 5: Orthogonal Class & Job Spec** | [Part 6: Generational War & Dream Clock →](Offline-Work-6.md)
> *Canonical PDF/AsciiDoc Edition:* [Offline-Work-5.adoc](Offline-Work-5.adoc)

# Tenebris and Lux: Definitive Class & Job System Specification

**Document ID:** `docs/game_design/classes_and_jobs_master_spec.adoc` (Archival Part 5: `docs/offline_work/Offline-Work-5.adoc`)

**Author:** CreativeWorkzStudio LLC / ProfessorSeanEX

**Engine Standard:** ISO C23 Target / C11 Floor (`ALLOC_ZERO_HEAP`) / OmniCode 5-Block Protocol

**Scope:** Complete 5-Tier Combinatorial Class Architecture (35 Classes), New Game+ Meta-Progression, and 5-Rank World Vocation Compendium (28 Jobs)

---

## 1. Executive Overview & Ludonarrative Philosophy

In ***Tenebris and Lux***, character growth is built upon an **orthogonal dual-engine architecture** inspired by *Dokapon Kingdom* and grounded in the socio-political reality of the **Kingdom of Sacyra**. Rather than fusing combat identity and worldly profession into a single rigid track, the engine partitions character capability into two symbiotic halves: **Class (The Combat Domain)** and **Job (The World Vocation)**.

```text
┌────────────────────────────────────────────────────────────────────────────────┐
│                        THE ORTHOGONAL CHARACTER ENGINE                         │
├───────────────────────────────────────┬────────────────────────────────────────┤
│          CLASS (The Domain)           │           JOB (The Vocation)           │
│          THE COMBAT ENGINE            │        THE EXPLORATION ENGINE          │
├───────────────────────────────────────┼────────────────────────────────────────┤
│ • Defines WHAT power source &         │ • Defines HOW society employs you &    │
│   discipline you channel in battle.   │   what tools you carry on the road.    │
│ • Governs the Core 9 Combat Stats     │ • Governs the 8 Exploration Stats      │
│   (HP, MP, TP, ATK, DEF, MATK,        │   (STR, DEX, VIT, INT, WIS,            │
│    MDEF, AGI, LUK) & Lv 1–99 growth.  │    PER, CHA, REP) & world interaction. │
│ • Unlocks permanent combat grimoire   │ • Grants temporary Rank 1–4 station    │
│   skills & passive soul attunements.  │   tools + 1 permanent Rank 5 Perk.     │
│ • Slow, deliberate 5-Tier climb from  │ • Fast, agile 5-Rank mastery loop      │
│   Base Domains to Hero / Dark One.    │   (5–8 encounters); interchangeable.   │
│ • Awakened in The Dreamscape          │ • Acquired ONLY upon leaving Dio       │
│   (Map 006: Room of Sages).           │   Village ("Crossing the Threshold").  │
└───────────────────────────────────────┴────────────────────────────────────────┘
```

### 1.1. The Three Decoupled Axes: Equipment, Element, and Conscience

A foundational invariant of *Tenebris and Lux* is that **Good and Evil reside strictly in the wielder's conscience ("The Why"), never in the elemental force ("The How") or the physical relic ("The What")**. The Class and Job system enforces this separation across three independent axes:

1. **Axis 1 — Physical Relics (Equipment):** **The Shield of Lux** (forged at Sacyra's founding as the mundane Shield of Peace) and **The Sword of Tenebris** (forged as the mundane Sword of War) are physical inventory armaments and catalysts. They are not locked to the Hero or Dark One classes and can be equipped, swapped, or embellished by any character.
2. **Axis 2 — Cosmic Elements (Methodology):** **Lux** (Radiance, Cohesion, Preservation) and **Tenebris** (Darkness, Severance, Dissolution) alongside the classical elements (Earth, Wind, Fire, Water) and their combinations (Lightning, Magma, Ice, Poison) are morally neutral natural and metaphysical laws.
3. **Axis 3 — Moral Sway (Conscience & Apex Class Gate):** Tracked along the `[-100, +100]` continuum by the **Compass of Morality**, the player's ethical choices determine whether they ascend to **The Hero** (`+50 ... +100`, Active Virtue / *Tov*), descend into **The Dark One** (`-100 ... -50`, Active Malice / *Ra*—the literal class of **The Dark Lord**), or remain capped at the mortal summit inside the **Bystander Corridor** (`[-50, +50]`).

### 1.2. Diegetic Framing: "Unofficial" Story Identity vs. "Official" Hard Mode

The narrative of *Tenebris and Lux* unfolds across two planes: the historical waking campaign in Sacyra and the framing archive inside **The Dreamscape** (Map 006: Room of Sages), where **Chrono the Sage** instructs a new initiate.

* **Elijah Ro-enheart & Lady Katrina Jaren (Unofficial "Trainees"):** In the waking story, Elijah is an unseasoned cadet searching for his vanished father (Commander Gareth), and Katrina is a noble runaway seeking autonomy in the Adventurers' Guild. Story-wise, neither is a master Warrior or Archmage yet—they are **Trainees** learning steel, magic, and survival simultaneously.
* **The Custom Bearer `<chosen_name>` (Unofficial "Mystic"):** Story-wise, the Custom Player is a **new Mystic** being told the tale of *Tenebris and Lux* by Chrono in the Dreamscape, reliving Elijah and Katrina's memories as part of their ascension toward becoming a **Sage of Ancients**.
* **Removing the Training Wheels:** On a first playthrough, touching a single Base Class stone (Warrior, Mage, or Rogue) in Chrono's chamber provides "training wheels" with a focused single-domain growth curve. Once the player beats the game and unlocks the Special Classes for **New Game+**, they can **officially** select **Trainee** or **Mystic**—aligning their mechanical class sheet with their canonical narrative identity and taking on a multi-domain "Hard Mode" progression curve.

---

## 2. Bidirectional Stat & Progression Architecture

Classes and Jobs work hand-in-hand through the bidirectional coupling between the **Core 9 Combat Stats** and the **8 Exploration Stats**.

| Combat Stat (Class-Driven) | Linked Exploration Stat (Job-Driven) | Combat Role (LeTBS Grid & Action Duels) | World & Exploration Role (Overworld & Dungeons) |
| --- | --- | --- | --- |
| **ATK** *(Physical Attack)* | **STR** *(Strength)* | Governs weapon damage, physical knockback, and armor sundering. | Pushing boulders, forcing jammed iron gates, chopping timber, carry capacity. |
| **AGI** *(Agility)* | **DEX** *(Dexterity)* | Governs turn-order priority, evasion, accuracy, and grid movement budget. | Lockpicking, disarming traps, stealth footing, clearing physical chasms. |
| **HP & DEF** *(Health & Defense)* | **VIT** *(Vitality)* | Governs life force, physical damage mitigation, and poise against stagger. | Overland stamina, resisting environmental briers, miasma, and spoiled rations. |
| **MATK & MP** *(Magic & Mana)* | **INT** *(Intelligence)* | Governs arcane spell potency, elemental synthesis, and mana capacity ("Science"). | Deciphering ancient archives, runic logic puzzles, and alchemical distillation. |
| **MDEF** *(Magic Defense)* | **WIS** *(Wisdom)* | Governs spiritual warding, MP regeneration, and status ailment resistance ("Magic"). | Reading NPC deception, sensing Dreamscape tears, and non-magical triage. |
| **ACC & CRIT** *(Precision)* | **PER** *(Perception)* | Governs critical hit rate, flanking lethality, line-of-sight, and trap spotting. | Spotting hidden chests, darkvision radius, and tracking wilderness quarry. |
| **TP** *(Tactical Points)* | **CHA** *(Charisma)* | Governs combat momentum generation, command auras, and spirit summon duration. | Town bartering, persuasion, companion morale, and pacifying wild beasts. |
| **Relic Resonance** | **REP** *(Reputation)* | Governs *Lux Barrio* / *Cuttatio Tenebris* synergy and squad morale modifiers. | Faction standing across Sacyra, bypassing military checkpoints, and court access. |

---

## 3. Campaign Milestone Gating & New Game+ (NG+) Loop

Class tier availability scales alongside campaign narrative milestones, rewarding multi-playthrough exploration across Elijah, Katrina, and the Custom Bearer.

### 3.1. Milestone Unlock Schedule

| Campaign Milestone | Narrative Trigger | Unlocked Class Tiers | Diegetic & Mechanical Rationale |
| --- | --- | --- | --- |
| **0% — Fresh Start** *(Playthrough 1, Act I)* | Awakening in **The Dreamscape** (Map 006) under Chrono's guidance. | **Tier 1 Basic Base Classes** (*Warrior, Mage, Rogue*). | The initiate begins with the three tangible, worldly disciplines required to survive Dio Village and the Whispering Woods. |
| **50% — Midpoint Climax** *(Half the Game)* | **Sacyra Castle Siege:** Exposing **King Jihad's** treason during the *Festival of Heroes* and witnessing the planar rift beneath the High Throne. | **Tier 1 Advanced Base** (*Priest, Summoner*) + **Tier 2–4 Combinatorial Classes**. | As the veil thins and the political war reveals a 3,000-year cosmic fracture, spiritual miracles, astral covenants, and hybrid disciplines awaken. |
| **100% — Full Clear** *(Beating the Game)* | **The Sovereign Eclipse Finale:** Confronting **The Dark Lord** and recording a completed cycle in **Chrono's Chronicle Ledger**. | **Special Initiate Classes** (*Trainee, Mystic*) + **Tier 5 Apex Classes** (*Hero, Dark One, Sage of Ancients*). | Having witnessed a full historical cycle, Chrono removes the training wheels—unlocking the official Hard-Mode story classes for New Game+. |

### 3.2. New Game+ Carryover, Head Starts & Encore Mastery Skills

When starting a **New Game+ (NG+)** campaign from a cleared save file:

1. **Persistent Base Access:** Every Base Class unlocked in a prior run (*Warrior, Mage, Rogue, Priest, Summoner*, plus *Trainee* and *Mystic*) is immediately selectable at the start of the game in the Room of Sages (Map 006).
2. **The 15–25% Mastery Head Start ("Attunement Resonance"):** Selecting any class that was already mastered in a previous campaign grants a **15% head start** on its mastery gauge (increasing to **25%** if mastered across both Elijah and Katrina's campaigns).
3. **NG+ Exclusive "Encore Mastery" Skills:** Re-selecting a previously mastered Base Class in NG+ awards an exclusive pinnacle technique drawn from the evolved 2017–2018 grimoire:

| Base Class | Standard Grimoire Core (2017–2018 Baseline) | NG+ Re-Selection "Encore Mastery" Special Skill |
| --- | --- | --- |
| **Warrior** | *Repulsive Blow*, *Flash Jump*, *Rush*, *Provoke*, *Heavy Hit*, *Intimidating Aura* | **Sovereign Phantom Slash** (*Evolved ID: 17*): Teleports behind a target ignoring ZOC, deals armor-piercing damage, and triggers an instant 1-tile *Intimidating Aura*. |
| **Mage** | *Fire Storm*, *Lightning Storm*, *Volcano Ground*, *Ice Wall*, *Ice Prison* | **Cataclysmic Convergence** (*Multi Fire / Mega Sparkle Fusion*): Detonates active *Ice Walls* or *Volcano Ground* tiles for double elemental burst while refunding `50%` MP. |
| **Rogue** | Backstab ambush, trap manipulation, *Pick Up* / *Push*, evasion step | **Shadow Transposition**: Steals an enemy's held item/buff while swapping grid coordinates with them and planting an *Explosive Mark* on the vacated tile. |
| **Priest** | *Heal*, *Salvation*, *Teleportation*, *Ghost Arrow*, *Accelerate*, *Extra Turn* | **Chrono's Emergency Decree** (*Evolved ID: 89*): Grants an ally an immediate out-of-turn *Emergency Turn* while cleansing all debuffs and applying *Basic Ward*. |
| **Summoner** | *Sentinel Totem*, *Wind Spirit*, *Summon Pixie*, *Fire Chickens* | **Astral Triad Covenant**: Summons two auxiliary spirits simultaneously (e.g., *Sentinel Totem* + *Pixie*) that inherit `25%` of the Summoner's DEF and MDEF. |

---

## 4. The 5-Tier Class System (35 Total Classes)

To eliminate redundant combinations while allowing deep *Dokapon*-style mastery exploration, the Class system is structured as a **5-Tier Combinatorial Pyramid** governed by Pascal's Triangle (`C(5,1) -> C(5,2) -> C(5,3) -> C(5,4) -> Apex`). Every class is uniquely defined by the exact combination of foundational domains it synthesizes.

```text
========================================================================================
                TIER 5: APEX SPECIAL CLASSES (The Cosmic Crucible — 3 Classes)
========================================================================================
   [ THE HERO ] (+50 to +100)        [ THE DARK ONE ] (-100 to -50)     [ SAGE OF ANCIENTS ]
(Active Virtue / Sovereignty)          (The Dark Lord's Class)        (Master of the Veil)
         ▲                                    ▲                                ▲
         ╚═════════════════════╦══════════════╩════════════════════════════════╝
                               ║ (Gated by Moral Sway & Perspective Synthesis)
========================================================================================
              TIER 4: QUAD-DOMAIN PARAGONS (The Mortal Summit — 5 Classes)
========================================================================================
  • Formed by mastering 4 of the 5 domains (each class omits exactly one domain: 5C4 = 5)
  • THE BYSTANDER CEILING: Players in the Bystander Corridor ([-50, +50]) STOP HERE!
  [High Justiciar]   [Beast Warlord]   [Sacred Magus]   [Shadow Saint]   [Ascendant Mystic]
         ▲
         ║ (Master 2 Overlapping Tier 3 Classes)
========================================================================================
             TIER 3: TRI-DOMAIN MASTER CLASSES (The Synthesis — 10 Classes)
========================================================================================
  • Formed by combining 3 foundational domains (5C3 = 10 unique non-redundant classes)
  • Where the Trainee path naturally reaches full three-pillar maturity (Battle Adept)
  [Battle Adept]  [Crusader]  [Rune Warden]  [Zealot Justiciar]  [Pack Marshal]
  [Divine Guardian]  [Shadowweaver]  [Phantasmancer]  [High Theurge]  [Chrono Stalker]
         ▲
         ║ (Master 2 Overlapping Tier 2 Classes)
========================================================================================
            TIER 2: DUAL-DOMAIN ADVANCED HYBRIDS (The Expansion — 10 Classes)
========================================================================================
  • Formed by combining any 2 Base Classes (5C2 = 10 unique classes)
  • Unlocks at the 50% Campaign Midpoint (King Jihad Climax)
  [Spellblade]  [Duelist]  [Paladin]  [Totemic Warden]  [Trickster]
  [Sage]        [Elementalist]        [Inquisitor]      [Puppeteer]      [Oracle]
         ▲
         ║ (Master 2 Base Classes)
========================================================================================
                  TIER 1: FOUNDATIONAL BASE CLASSES (5 + 2 Initiates = 7)
========================================================================================
  • 3 Basic Base (Worldly Triad):    [Warrior (W)]   [Mage (M)]      [Rogue (R)]
  • 2 Advanced Base (Ethereal Dyad): [Priest (P)]    [Summoner (S)]
  • 2 Special Hard-Mode Initiates:   [Trainee (W+M+R)]               [Mystic (P+S)]
========================================================================================
```

---

### 4.1. Tier 1: Foundational Base Classes (5 Standard + 2 Special Initiates)

#### A. The 3 Basic Base Classes (The Worldly Triad)

Available from minute one of a fresh campaign in the Room of Sages:

1. **Warrior (`W` — The Martial Domain):**
   * *Power Source & Focus:* Physical conditioning, weapon mastery, shield bracing, and tactical command (`HP`, `ATK`, `DEF`, `TP` / `STR`, `VIT`).
   * *Canonical Grimoire:* *Repulsive Blow* (knockback), *Flash Jump* (gap-closer leap), *Rush*, *Provoke*, *Heavy Hit*, *Phantom Slash*, *Intimidating Aura*.
   * *Permanent Mastery Passive:* **Unshakable Poise** (`+15%` physical DEF and immunity to minor stagger).

2. **Mage (`M` — The Arcane / "Science" Domain):**
   * *Power Source & Focus:* Academic leyline geometry, elemental synthesis (Earth, Wind, Fire, Water), and field zoning (`MP`, `MATK`, `MDEF` / `INT`).
   * *Canonical Grimoire:* *Fire*, *Sparkle*, *Fire Storm*, *Lightning Storm*, *Volcano Ground*, *Ice Fury*, *Ice Prison*, *Ice Wall* (destructible grid barrier).
   * *Permanent Mastery Passive:* **Leyline Conduit** (`+15%` Max MP and `+10%` elemental spell potency).

3. **Rogue (`R` — The Subterfuge Domain):**
   * *Power Source & Focus:* Street survivalism, high-tempo agility, flanking geometry, and mechanical cunning (`AGI`, `LUK`, `ATK` / `DEX`, `PER`).
   * *Canonical Grimoire:* *Pick Up* (loot/steal contraband), *Push*, *Whitch's Escape* (evasive blink), backstab criticals, and trap deployment.
   * *Permanent Mastery Passive:* **Shadow Reflex** (`+10%` evasion and `+15%` critical hit rate when striking from flank or rear angles).

#### B. The 2 Advanced Base Classes (The Ethereal Dyad)

Unlocked at the 50% campaign midpoint (or available from the start in NG+):

4. **Priest (`P` — The Divine / "Magic" Domain):**
   * *Power Source & Focus:* Spiritual faith in Goddess Lyri, restorative miracles, and turn-timeline manipulation (`MP`, `MDEF`, `HP` / `WIS`, `CHA`).
   * *Canonical Grimoire:* *Heal*, *Salvation*, *Basic Ward*, *Teleportation*, *Ally Teleport*, *Ghost Arrow*, *Magic Trap*, *Explosive Mark*, *Accelerate*, *Extra Turn*, *Emergency Turn*.
   * *Permanent Mastery Passive:* **Covenant Grace** (Restorative skills cost `15%` less MP and grant `+10%` MDEF for 1 turn).

5. **Summoner (`S` — The Astral Pact Domain):**
   * *Power Source & Focus:* Ethereal covenants calling independent spirit entities and totems onto the tactical grid (`MP`, `MDEF`, `LUK` / `WIS`, `CHA`).
   * *Canonical Grimoire:* *Sentinel Totem* (stationary turret/blocker), *Wind Spirit* (*Great Wind*, *Transposition*), *Summon Pixie* (*Healing Wave*), *Fire Chickens* (*Bomb* decoy).
   * *Permanent Mastery Passive:* **Pact Tether** (Summoned entities inherit `+15%` of the caster's Max HP and DEF).

#### C. The 2 Special Initiate Classes (Official Story Hard-Mode Classes)

Unlocked for New Game+ after beating the game once (100% milestone):

| Special Initiate | Canonical Story Holder | Domains Trained Simultaneously | Hard-Mode Progression Penalty | Ascension Payoff |
| --- | --- | --- | --- | --- |
| **6. Trainee** | **Elijah Ro-enheart** & **Lady Katrina Jaren** | **Warrior + Mage + Rogue** (`W + M + R`) | **3-Way Split Burden:** Requires `2.2×` EXP per mastery rank; early-game stats are spread thin across physical, arcane, and agility attributes. | Simultaneously banks all 3 Basic Base mastery passives; matures into *Battle Adept* (Tier 3) and unlocks the direct expressway to **The Hero** or **The Dark One**. |
| **7. Mystic** | **Custom Bearer (`<chosen_name>`)** *(Chrono's Initiate)* | **Priest + Summoner** (`P + S`) | **2-Way Ethereal Burden:** Requires `1.8×` EXP per mastery rank; low physical armor and HP in Act I require strict reliance on totems, spirits, and turn manipulation. | Simultaneously banks Priest and Summoner passives; matures through *Oracle* (Tier 2) and *Ascendant Mystic* (Tier 4) directly into **Sage of Ancients**. |

---

### 4.2. Tier 2: Dual-Domain Advanced Hybrids (`C(5,2) = 10` Classes)

Unlocked at the **50% Campaign Milestone** by mastering the two constituent Tier 1 Base Classes:

| # | Class Name | Domain Recipe | Archetype & Tactical Grid Identity | Signature Hybrid Techniques & Synergies |
| --- | --- | --- | --- | --- |
| **8** | **Spellblade** | `W + M` | **Arcane Vanguard:** Frontline bruiser who channels elemental formulas through melee weapon strikes. | Imbues *Rush* and *Heavy Hit* with Fire/Ice; raises close-range *Ice Walls* to funnel enemies into melee cleave range. |
| **9** | **Duelist** | `W + R` | **Blademaster Skirmisher:** High-poise, high-evasion striker specializing in parry ripostes and deep flanking. | Chains *Flash Jump* and *Phantom Slash* to bypass frontline ZOC and execute high-crit backstabs. |
| **10** | **Paladin** | `W + P` | **Holy Bastion:** Heavy plate protector who anchors bottlenecks with restorative auras and radiant shields. | Pairs *Provoke* and *Intimidating Aura* with *Heal* and *Basic Ward* to sustain the frontline. |
| **11** | **Totemic Warden** | `W + S` | **Pack Bulwark:** Frontline fighter who links armor and threat generation with summoned totems and beasts. | Drops *Sentinel Totems* on chokepoints and uses *Repulsive Blow* to knock enemies into totem crossfire. |
| **12** | **Trickster** | `M + R` | **Arcane Saboteur:** Guerrilla caster who seeds the grid with delayed elemental runes and vanishes into stealth. | Casts *Whitch's Escape* to blink away while leaving an *Explosive Mark* and *Ice Prison* on the vacated cell. |
| **13** | **Sage** | `M + P` | **Dual-School Scholar:** Bridges Sacyra's **Science (Academy)** and **Magic (Church)** schism; master of storm and grace. | Weaves *Fire Storm* and *Lightning Storm* alongside *Salvation* and *Mana Ball* battery support. |
| **14** | **Elementalist** | `M + S` | **Astral Evoker:** Infuses summoned entities with unstable elemental storms to reshape battlefield terrain. | Summons *Fire Chickens* whose *Bomb* detonation converts tiles into lingering *Volcano Ground*. |
| **15** | **Inquisitor** | `R + P` | **Sanctified Hunter:** High-mobility executioner who teleports behind enemy lines to purge priority targets. | Chains *Light Steps* and *Teleportation* to reach backline casters and strike with *Ghost Arrow*. |
| **16** | **Puppeteer** | `R + S` | **Mirage Tactician:** Misdirection specialist who swaps coordinates with decoys to force enemy friendly fire. | Uses *Wind Spirit's Transposition* to swap a boss into a trap while looting contraband via *Pick Up*. |
| **17** | **Oracle** | `P + S` | **Timeline Conduit:** Supreme support vessel commanding guardian spirits while dictating turn initiative. | Combines *Pixie Healing Wave* with *Accelerate*, *Extra Turn*, and *Emergency Turn*. |

---

### 4.3. Tier 3: Tri-Domain Master Classes (`C(5,3) = 10` Classes)

Unlocked by mastering **two Tier 2 Hybrid Classes** that share a common domain root (yielding all 10 unique 3-domain combinations without redundancy):

| # | Class Name | Domain Recipe | Required Tier 2 Pair (Any 2 of 3) | Tactical Mastery Profile |
| --- | --- | --- | --- | --- |
| **18** | **Battle Adept** | `W + M + R` | *Spellblade*, *Duelist*, or *Trickster* *(or Mature Trainee)* | Seamlessly chains martial leaps, elemental blade infusions, and stealth disengagement in a single turn sequence. |
| **19** | **Crusader** | `W + M + P` | *Spellblade*, *Paladin*, or *Sage* | Armored battle-theurge who calls down elemental judgment while projecting sanctified damage-mitigation domes. |
| **20** | **Rune Warden** | `W + M + S` | *Spellblade*, *Totemic Warden*, or *Elementalist* | Inscribes elemental runes onto summoned spirits and totems, turning every summon into an armored elemental hazard. |
| **21** | **Zealot Justiciar** | `W + R + P` | *Duelist*, *Paladin*, or *Inquisitor* | Relentless front-to-back pursuer who teleports into enemy formations, shatters buffs, and locks down casters with holy ZOC. |
| **22** | **Pack Marshal** | `W + R + S` | *Duelist*, *Totemic Warden*, or *Puppeteer* | Wolfpack commander who flanks in synchronous formation with summoned spirits, triggering automatic pincer criticals. |
| **23** | **Divine Guardian** | `W + P + S` | *Paladin*, *Totemic Warden*, or *Oracle* | Immortal phalanx anchor who links their HP pool across multiple *Sentinel Totems* and heals the squad whenever struck. |
| **24** | **Shadowweaver** | `M + R + P` | *Trickster*, *Sage*, or *Inquisitor* | Clandestine occultist who casts silent, zero-telegraph elemental curses from stealth while siphoning enemy MP and turn speed. |
| **25** | **Phantasmancer** | `M + R + S` | *Trickster*, *Elementalist*, or *Puppeteer* | Master of illusory warfare; floods the grid with elemental clone decoys that swap places with allies when attacked. |
| **26** | **High Theurge** | `M + P + S` | *Sage*, *Elementalist*, or *Oracle* | Grand spell-weaver who commands all three non-martial pillars—unleashing dual elemental-holy storms through spirit relays. |
| **27** | **Chrono Stalker** | `R + P + S` | *Inquisitor*, *Puppeteer*, or *Oracle* | Temporal phantom who manipulates turn order, teleports summons into rear flanks, and freezes enemy action timelines. |

---

### 4.4. Tier 4: Quad-Domain Paragons (`C(5,4) = 5` Classes — The Mortal Summit)

Tier 4 represents the pinnacle of mortal skill in the Kingdom of Sacyra. Each class synthesizes **four of the five foundational domains**, defined by the single domain it chooses to forsake.

> **The Bystander Ceiling Invariant:** If a player reaches the end of the campaign while their Moral Sway remains inside the **Bystander Corridor (`[-50, +50]`)**—reflecting moral inertia, compromise, or self-preservation—**Tier 4 is their final class ceiling**. They cannot unlock **The Hero** or **The Dark One** and will resolve the campaign on the **Neutral Ending**.

| # | Paragon Class | Domains Mastered | Omitted Domain | Mortal Summit Identity & Signature Capability |
| --- | --- | --- | --- | --- |
| **28** | **High Justiciar** | `W + M + R + P` | *Omits Summoner (`S`)* | **The Sovereign General:** Rejects external spirits to rely purely on personal mastery of steel, spell, stealth, and faith. Commands squad-wide formation buffs and armor-piercing holy-elemental strikes. |
| **29** | **Beast Warlord** | `W + M + R + S` | *Omits Priest (`P`)* | **The Primal Conqueror:** Forsakes church dogma to dominate the battlefield through ruthless physical force, elemental cataclysms, ambush tactics, and an army of apex spirits. |
| **30** | **Sacred Magus** | `W + M + P + S` | *Omits Rogue (`R`)* | **The Immovable Citadel:** Discards stealth and subterfuge entirely; a walking fortress of heavy plate, arcane artillery, divine miracles, and guardian monoliths. |
| **31** | **Shadow Saint** | `W + R + P + S` | *Omits Mage (`M`)* | **The Ethereal Executioner:** Rejects academic leyline formulas in favor of pure instinct, miraculous speed, shadow transposition, and spirit-assisted assassinations. |
| **32** | **Ascendant Mystic** | `M + R + P + S` | *Omits Warrior (`W`)* | **The VeilWeaver:** Discards physical steel entirely to command arcane, shadow, divine, and astral forces. Serves as the natural Tier 4 threshold for the **Mystic** initiate path. |

---

### 4.5. Tier 5: Apex Special Classes (The Cosmic Crucible — 3 Classes)

Tier 5 transcends mortal discipline. Unlocking a Tier 5 class requires mastering a **Tier 4 Paragon** (or completing the official **Trainee / Mystic** hard-mode path) AND passing the **Cosmic Crucible of Moral Sway or Perspective Synthesis**:

#### 33. The Hero (The Sovereign of Active Virtue)

* **Unlock Requirement:** Master a Tier 4 Paragon (or *Trainee*) + achieve **Active Virtue (`+50 ... +100` Moral Sway)** via the Compass of Morality.
* **Ontological Identity:** Embodies selfless restoration, covenantal fidelity (*Tov*), and the courage to bear the realm's burdens without tyrannical control.
* **Mechanical Mastery:**
  * Unlocks maximum resonance with whichever relic(s) are equipped, plus the adaptive tandem art **Duo-Loca Bashin n Slash**.
  * Eliminates the emotional panic dissipation penalty on **Lux**, ensuring protective barriers (*Lux Barrio*) never shatter under boss terror effects.
  * Steers the campaign toward the **Heroic Restoration Ending** or, when combined with full nocturnal Dreamscape synthesis, the **10th True Ending ("The Ugly Truth")**.

#### 34. The Dark One (The Class of The Dark Lord)

* **Unlock Requirement:** Master a Tier 4 Paragon (or *Trainee*) + descend into **Active Malice (`-100 ... -50` Moral Sway)**.
* **Ontological Identity:** **The literal class of The Dark Lord (Usuru)**. Born when a master of arms surrenders to unresolved grievance, tyrannical pride, or nihilistic vengeance (*Ra*).
* **Mechanical Mastery:**
  * Converts defensive stability into overwhelming offensive severance and tyrannical control.
  * Grants **Abyssal Rupture** (attacks shred `35%` of target DEF/MDEF and spread entropic rot or petrifying stasis across adjacent grid tiles) and **Sovereign Terror** (an aura that inflicts panic on weaker foes, causing enemy Lux barriers to instantly dissipate).
  * Steers the campaign toward the **Abyssal Usurper Ending**, where the protagonist overthrows or consumes the Dark Lord to claim the throne of ruin.

#### 35. Sage of Ancients (The Master of the Veil)

* **Unlock Requirement:** Complete the **Mystic** initiate path (or *Ascendant Mystic*) + complete the nocturnal **Dreamscape Trials** with **The Dream Compass**.
* **Ontological Identity:** The pinnacle class of **Chrono the Sage** and the culmination of the Custom Bearer's (`<chosen_name>`) framing story.
* **Mechanical Mastery:**
  * Commands the boundary between the waking world and the Dreamscape, unlocking **Planar Convergence** (summons ancestral echoes onto the grid), **Temporal Rewind** (resets an ally's HP, MP, and cooldowns to their prior turn state), and **Harmonic Equilibrium** (effortlessly tempers Tenebris with Lux at `0` Sway).

### 4.6. The 4-Quadrant Expression Matrix (Morality × Element)

Because **The Hero** and **The Dark One** are moral classes rather than elemental ones, either class can channel **Lux** or **Tenebris** (and wield either the Shield or the Sword) with radically different manifestations:

| Moral Apex Class ↓ / Elemental Medium → | Channeled Through Lux (Radiance / Cohesion) | Channeled Through Tenebris (Darkness / Severance) |
| --- | --- | --- |
| **THE HERO**<br>*(Moral Good: `+50 ... +100`)* | **The Selfless Aegis:**<br>Projects golden *Lux Barrio* domes (`25%–50%` mitigation) to shelter the weak, heal provincial fractures, and preserve truth without forcing compliance. | **The Surgical Liberator (The Casatti Ideal):**<br>Wields *Cuttatio Tenebris* to sever slave chains, slice through tyrant armor, and execute **Absolute Justice** with zero collateral harm to innocents. |
| **THE DARK ONE**<br>*(Moral Evil: `-100 ... -50`)* | **The Calcified Despot:**<br>Uses blinding, totalitarian radiance to petrify dissenters into living stone, enforcing comatose stasis and absolute obedience across the realm. | **The Sovereign of Ruin (The Dark Lord / Usuru):**<br>Unleashes entropic void cleaves that devour life force, spread creeping rot across the grid, and consume Sacyra in bitter vengeance. |

---

## 5. The Job (Vocation) System Architecture

While your **Class** defines what power you channel in battle, your **Job** is the actual civic post, guild contract, or civilian trade you hold within the Kingdom of Sacyra.

### 5.1. The "No Starter Job in Dio Village" Rule

1. **In the Dreamscape (Map 006):** Chrono awakens your innate **Class** (Combat Domain) so you can fight off the **Dream Eaters**.
2. **In Dio Village (Maps 020–031):** When you awaken physically at **Grandma Magolia's** hearth, **you have no Job**. Dio Village is the domestic "Ordinary World" (`0` Threat Vigor). Elijah is an uncommissioned youth at his grandmother's house; Katrina is an unregistered noble runaway.
3. **Crossing the First Threshold:** You acquire your very first Job at the exact moment your journey into the wider world begins—when you step beyond Dio Village's gates into the **Whispering Woods**, take up an axe to clear the timber road for **Eric the Lumberjack**, or register at the **Adventurers' Guild** outpost.

### 5.2. The 5-Rank Progression & Why Active Job Skills Don't Carry Over

* **Why Class Skills Stay Forever:** Class skills represent internal muscle memory, neural conditioning, and soul attunement. Once learned, your body never forgets how to execute *Flash Jump* or weave *Fire Storm*.
* **Why Active Job Skills Are Temporary:** Job skills (Ranks 1–4) represent **station tools, legal authority, and operational kits**—such as a Town Sentry's guard whistle and riot shield, a Locksmith's tension wrenches, or a Miner's blasting charges. When you step down from being a Town Sentry to work as an Herbalist, you turn in your badge and riot gear in exchange for foraging pouches and mortar pestles.
* **The Rank 5 Permanent Mastery Perk:** Every Job has **5 Ranks** that level up rapidly (completing a full 5-rank cycle in **5 to 8 skirmishes, field tasks, or puzzle interactions**):
  * **Ranks 1–4 (Active Station Tools):** Active only while the character currently holds that Job.
  * **Rank 5 (Mastery Perk):** Permanently extracted into the character's **Passive Mastery Ledger** and retained forever across all future Jobs and Classes.

```text
[ RANK 1: NOVICE ]       ──► Unlocks Primary Exploration Tool / Basic Job Skill (Active Only)
[ RANK 2: PRACTITIONER ] ──► Unlocks Secondary Tactical Technique + Exploration Stat Bonus (Active Only)
[ RANK 3: JOURNEYMAN ]   ──► Unlocks Passive Operational Trait / Field Utility (Active Only)
[ RANK 4: VETERAN ]      ──► Unlocks Signature Vocation Technique (Active Only)
[ RANK 5: MASTER ]       ──► UNLOCKS PERMANENT MASTERY PERK (Banked Forever Across All Builds!)
```

---

## 6. Complete World-Vocation Compendium (All 28 Sacyran Jobs)

To reflect a living, breathing Kingdom of Sacyra without artificial number caps, the Job system encompasses **28 distinct vocations** across **5 Societal Pillars**—integrating both **Class-Base Domain Jobs** (direct institutional extensions of the 5 classes) and **Offset Occupational Jobs** (domain-agnostic civilian trades and frontier survival crafts).

---

### Pillar I: Frontier Labor & Wilderness Survival (6 Jobs)

The rugged, domain-agnostic trades of commoners surviving in neglected frontier provinces like Dio Village and Libira.

| # | Job Title (Type) | Exploration Stat Focus | Rank 1–4 Active Vocation Skills *(While Holding Job Only)* | Rank 5 Permanent Mastery Perk *(Retained Forever)* |
| --- | --- | --- | --- | --- |
| **1** | **Lumberjack / Feller**<br>*(Offset Trade — Eric's Vocation)* | `STR` / `VIT` | • **R1:** *Hew Timber* (Explore: Chops fallen logs/wooden barricades)<br>• **R2:** *Splitting Chop* (Combat: `130%` axe strike; breaks wooden shields)<br>• **R3:** *Rooted Poise* (Combat: `+30%` poise; immune to knockback)<br>• **R4:** *Timber Toss* (Combat: Hurls log 3 tiles for blunt damage + obstacle) | **Felling Cleave:** Instantly destroys wooden barricades and fallen trees across exploration maps and combat grids for `0` stamina/tools. |
| **2** | **Quarry Miner / Delver**<br>*(Offset Trade)* | `STR` / `VIT` / `PER` | • **R1:** *Prospecting Strike* (Explore: Extracts ore/gems from mineral veins)<br>• **R2:** *Spalling Strike* (Combat: Chips target physical DEF by flat `-5`)<br>• **R3:** *Delver's Flare* (Explore: Doubles sight radius in dark caves/tombs)<br>• **R4:** *Cave-In Collapse* (Combat: Triggers rockfall around wall tiles) | **Shatter Strike:** Breaks cracked dungeon walls and cave-in boulders without consuming pickaxes or explosive bombs. |
| **3** | **Frontier Trapper**<br>*(Offset Trade)* | `PER` / `DEX` | • **R1:** *Field Snare* (Explore: Harvests meat/pelts during camp rest)<br>• **R2:** *Caltrop Scatter* (Combat: 3-tile spike line; `-2` Move Points)<br>• **R3:** *Big Game Bane* (Combat: `+25%` damage vs. Beasts/Avians)<br>• **R4:** *Spring-Jaw Trap* (Combat: Places hidden trap; immobilizes 2 turns) | **Ambush Snare:** At the start of every wilderness battle, one random non-boss enemy begins immobilized in a snare for Turn 1. |
| **4** | **Field Forager / Herbalist**<br>*(Offset Trade)* | `INT` / `WIS` | • **R1:** *Flora Sense* (Explore: Highlights all herb/fungi nodes on map)<br>• **R2:** *Poultice Pack* (Combat: Adjacent ally regenerates HP over 3 turns)<br>• **R3:** *Venom Filter* (Combat: `+50%` resistance to Poison and Rot)<br>• **R4:** *Field Distillation* (Explore: Crafts potions from 3 raw herbs at camp) | **Bountiful Harvest:** Overworld flora nodes yield `2×` materials and a `+25%` chance to drop rare relic-embellishment catalysts. |
| **5** | **Camp Cook / Provisioner**<br>*(Offset Trade)* | `VIT` / `CHA` | • **R1:** *Trail Stew* (Explore: Camp meals grant `+10%` Max HP until next rest)<br>• **R2:** *Hearty Morsel* (Combat: Tosses ration 2 tiles to restore `25` TP)<br>• **R3:** *Spiced Jerky* (Explore: Overland stamina drains `50%` slower)<br>• **R4:** *Feast of Accord* (Explore: Camp meals grant `+15%` elemental resist) | **Iron Stomach:** All food consumables are `+50%` more effective, and the party is permanently immune to spoiled ration/disease debuffs. |
| **6** | **Road Caravaner**<br>*(Offset Trade)* | `VIT` / `STR` | • **R1:** *Draft Rigging* (Explore: `+10` party inventory carry slots)<br>• **R2:** *Whip Crack* (Combat: 2-tile physical snap; interrupts charge skills)<br>• **R3:** *Reinforced Axle* (Explore: Ignores mud/flooded road travel delays)<br>• **R4:** *Wagon Phalanx* (Combat: Allies within 1 tile gain `+15%` DEF) | **Highway Endurance:** Party exploration movement speed increases by `+20%` when traveling along roads, bridges, and paved trails. |

---

### Pillar II: Civic Order, Military & Feudal Law (6 Jobs)

The formal military and legal apparatus of the Sacyran Crown, Garrison Watch, and feudal bailiffs.

| # | Job Title (Type) | Exploration Stat Focus | Rank 1–4 Active Vocation Skills *(While Holding Job Only)* | Rank 5 Permanent Mastery Perk *(Retained Forever)* |
| --- | --- | --- | --- | --- |
| **7** | **Town Sentry / Watchman**<br>*(Warrior Class-Base)* | `VIT` / `REP` | • **R1:** *Shield Brace* (Combat: `+20%` frontal physical mitigation)<br>• **R2:** *Watchful Eye* (Explore: Reveals patrol sight cones on map)<br>• **R3:** *Halt!* (Combat: Bash that removes 2 Move Points from target)<br>• **R4:** *Phalanx Intercept* (Combat: Swaps places to tank hit for adjacent ally) | **Vigilant Posture:** The party is permanently immune to surprise ambushes during exploration traversal, room transitions, and camp rests. |
| **8** | **Royal Man-at-Arms**<br>*(Warrior Class-Base)* | `STR` / `VIT` | • **R1:** *Sundering Strike* (Combat: Reduces enemy DEF by `15%` for 2 turns)<br>• **R2:** *Threat Appraisal* (Explore: Inspects enemy patrol Lv, HP, and stats)<br>• **R3:** *Adrenaline Surge* (Combat: Restores `20` TP on crit or kill)<br>• **R4:** *Sweeping Cleave* (Combat: 3-tile frontal arc + 1-tile knockback) | **Brutal Leverage:** Forces open rusted portcullises, jammed grates, and heavy iron doors on exploration maps without keys via `STR` checks. |
| **9** | **Border Outrider**<br>*(Rogue/Warrior Class-Base — Sir Roté's Post)* | `PER` / `DEX` | • **R1:** *Courier Canter* (Explore: `+25%` overland dash speed; clears fog)<br>• **R2:** *Mark Weakness* (Combat: Target takes `+15%` ACC and `+10%` Crit)<br>• **R3:** *Skirmish Sprint* (Combat: `+2` Move Points for 1 turn)<br>• **R4:** *Disengaging Strike* (Combat: Strikes and leaps back 2 tiles ignoring ZOC) | **Unimpeded Stride:** Permanently ignores environmental terrain movement penalties (mud, deep sand, snow, briers) in exploration and battle. |
| **10** | **Citadel Jailer / Bailiff**<br>*(Offset / Feudal Law)* | `WIS` / `STR` | • **R1:** *Warrant Pass* (Explore: Grants entry to barracks and prisons)<br>• **R2:** *Manacle Bind* (Combat: Disables target physical skills for 2 turns)<br>• **R3:** *Suppressive Glare* (Combat: Adjacent enemies suffer `-15%` ATK)<br>• **R4:** *Sentence Delivered* (Combat: `+50%` crit damage vs. bound/slowed foes) | **Iron Custody:** Capturing or pacifying humanoid enemies yields double bounty gold and automatically extracts dungeon keycards. |
| **11** | **Court Herald / Emissary**<br>*(Offset / Diplomatic)* | `CHA` / `REP` | • **R1:** *Royal Writ* (Explore: Unlocks audiences with mayors and nobles)<br>• **R2:** *Rallying Cry* (Combat: Restores `15` TP to allies within 3 tiles)<br>• **R3:** *Truce Envoy* (Combat: Delays humanoid enemy squad turn by 1 round)<br>• **R4:** *Sovereign Decree* (Combat: Buffs squad ATK/DEF scaled by party `REP`) | **Diplomatic Immunity:** Bypasses hostile faction tollgates and military checkpoints across Sacyra without paying bribes or triggering fights. |
| **12** | **Siege Engineer**<br>*(Warrior/Mage Hybrid Job)* | `INT` / `STR` | • **R1:** *Fortify Choke* (Combat: Builds a wooden barricade on adjacent tile)<br>• **R2:** *Structural Flaw* (Combat: `3×` damage vs. props, walls, and ice blocks)<br>• **R3:** *Trench Works* (Explore: Bridges gaps and drains flooded rooms)<br>• **R4:** *Demolition Charge* (Combat: Delayed cross-pattern explosive bomb) | **Breach Expertise:** Party attacks permanently deal `+30%` bonus damage against constructs, golems, fortifications, and destructible grid props. |

---

### Pillar III: Scholarship, Commerce & Underworld Craft (6 Jobs)

The intellectual engines of The Academy (Prince Casatti's legacy), highway commerce, and Lower Dio's gray market.

| # | Job Title (Type) | Exploration Stat Focus | Rank 1–4 Active Vocation Skills *(While Holding Job Only)* | Rank 5 Permanent Mastery Perk *(Retained Forever)* |
| --- | --- | --- | --- | --- |
| **13** | **Academy Archivist**<br>*(Mage Class-Base)* | `INT` / `PER` | • **R1:** *Rune Inscription* (Explore: Unlocks sealed academy doors/pedestals)<br>• **R2:** *Mana Siphon* (Combat: Drains `15` MP from enemy to caster)<br>• **R3:** *Arcane Efficiency* (Combat: Elemental spells cost `20%` less MP)<br>• **R4:** *Leyline Overcharge* (Combat: Next spell gains `+40%` MDEF piercing) | **Ancient Literacy:** Permanently reads and translates all dead languages, archaic Sacyran scripts, and planar tablets without a lexicon. |
| **14** | **Field Warmage**<br>*(Mage Class-Base)* | `INT` / `VIT` | • **R1:** *Ignite / Freeze* (Explore: Burns brush or freezes water crossings)<br>• **R2:** *Focused Blast* (Combat: Condenses AOE spell into `2×` single-target hit)<br>• **R3:** *Kinetic Mine* (Combat: Leaves displacement rune on vacated tile)<br>• **R4:** *Siege Stance* (Combat: Sacrifices Move to gain `+2` spell range) | **Elemental Transmutation:** Defeating an enemy with an elemental spell automatically drops `1` raw elemental crystal catalyst matching that affinity. |
| **15** | **Guild Locksmith**<br>*(Rogue Class-Base)* | `DEX` / `PER` | • **R1:** *Pick Apparatus* (Explore: Unlocks standard wooden/iron chests)<br>• **R2:** *Disarm Mechanism* (Explore: Dismantles wire and pressure traps)<br>• **R3:** *Jam Tumbler* (Combat: Prevents target from landing crits for 2 turns)<br>• **R4:** *Vault Breaker* (Explore: Opens ornate master-locked vaults) | **Skeleton Finger:** Unlocking standard wooden, iron, and silver chests no longer consumes lockpicks and automatically neutralizes needle traps. |
| **16** | **Underground Smuggler**<br>*(Rogue Class-Base — John's Network)* | `DEX` / `CHA` | • **R1:** *Fence Contraband* (Explore: Appraises and sells stolen goods)<br>• **R2:** *Pocket Sand* (Combat: Frontal cone blind; `-50%` ACC for 2 turns)<br>• **R3:** *Hidden Compartment* (Explore: Smuggles restricted items past guards)<br>• **R4:** *Shadow Mug* (Combat: Backstab that steals target's held item) | **Underworld Connections:** Permanently unlocks secret black-market vendors in every major settlement and sells contraband at `100%` value. |
| **17** | **Highway Peddler**<br>*(Offset Trade)* | `CHA` / `REP` | • **R1:** *Haggle* (Explore: `10%` shop discount and `+10%` sell price)<br>• **R2:** *Coin Toss* (Combat: Distracts humanoid foe, rotating their facing)<br>• **R3:** *Supply Ledger* (Explore: Unlocks hidden stock in general stores)<br>• **R4:** *Mercenary Payoff* (Combat: Spends Gold to make humanoid foe retreat) | **Keen Appraisal:** Automatically identifies the true market value, hidden stats, and historical lore of all unidentified relics and gear. |
| **18** | **Field Chirurgeon**<br>*(Offset Medical Trade)* | `WIS` / `DEX` | • **R1:** *Bandage Wrap* (Combat: Stops Bleed and restores modest HP for `0` MP)<br>• **R2:** *Bone Splint* (Explore: Cures fractures/sprains from cliff falls)<br>• **R3:** *Smelling Salts* (Combat: Revives fallen ally at `15%` HP for `0` MP)<br>• **R4:** *Field Surgery* (Combat: Restores `40%` HP to adjacent ally for `0` MP) | **Triage Efficiency:** All mundane healing consumables (bandages, salves, poultices) restore `+50%` more HP, preserving MP for spellcasting. |

---

### Pillar IV: Ecclesiastical & Faith Services (5 Jobs)

The parish clergy of Goddess Lyri, wandering hospitalers, and clerical inquisitors.

| # | Job Title (Type) | Exploration Stat Focus | Rank 1–4 Active Vocation Skills *(While Holding Job Only)* | Rank 5 Permanent Mastery Perk *(Retained Forever)* |
| --- | --- | --- | --- | --- |
| **19** | **Chapel Acolyte / Hospitaler**<br>*(Priest Class-Base)* | `WIS` / `CHA` | • **R1:** *Sanctify Spring* (Explore: Purifies tainted wells into rest spots)<br>• **R2:** *Graceful Word* (Combat: Cleanses `1` debuff within 3 tiles)<br>• **R3:** *Benediction Aura* (Combat: Adjacent allies heal `5%` HP/turn)<br>• **R4:** *Aegis of Faith* (Combat: Shield absorbs next incoming spell hit) | **Sanctified Respite:** Resting at any campfire or inn automatically purges all lingering poison, curse, and disease statuses for free. |
| **20** | **Church Inquisitor**<br>*(Priest Class-Base)* | `WIS` / `PER` | • **R1:** *Detect Malice* (Explore: Senses void residue and disguised cultists)<br>• **R2:** *Nullifying Smite* (Combat: Melee hit dispels all enemy buffs)<br>• **R3:** *Shackle of Truth* (Combat: Silences and immobilizes caster 1 turn)<br>• **R4:** *Consecrated Ground* (Combat: `3×3` zone; `+20%` MDEF / burns undead) | **Piercing Insight:** Dialogue windows automatically flag when an NPC is lying, concealing their true faction, or hiding a secret motive. |
| **21** | **Church Liturgist**<br>*(Priest Class-Base)* | `WIS` / `CHA` | • **R1:** *Hymn of Comfort* (Combat: Buffs party MDEF by `+20%` for 3 turns)<br>• **R2:** *Calm Panic* (Explore/Combat: Removes Fear and Panic from allies)<br>• **R3:** *Sanctuary Bell* (Combat: Enemies cannot target caster for 1 turn)<br>• **R4:** *Lyri's Chorus* (Combat: Grants `+15%` radiant damage to squad) | **Unshakable Faith:** The character is permanently immune to Fear, Panic, and Silence, preventing Lux barriers from dissipating under terror. |
| **22** | **Relic Scribe**<br>*(Priest/Mage Hybrid Job)* | `INT` / `WIS` | • **R1:** *Inscribe Ward* (Explore: Crafts single-use barrier scrolls at camp)<br>• **R2:** *Parchment Seal* (Combat: Tile rune silences first enemy stepper)<br>• **R3:** *Illuminated Vellum* (Combat: Allied buff durations last `+1` turn)<br>• **R4:** *Scripture of Banishment* (Combat: Heavy holy burst vs. void/undead) | **Scroll Conservation:** Using magical scrolls, talismans, and written consumables carries a `50%` chance not to consume the item. |
| **23** | **Shrine Hermit**<br>*(Offset Ascetic Vocation)* | `WIS` / `VIT` | • **R1:** *Fasting Discipline* (Explore: Camp rests consume `50%` less food)<br>• **R2:** *Solitary Ward* (Combat: `+30%` DEF/MDEF when no ally is within 2 tiles)<br>• **R3:** *Miasma Cloak* (Explore: Party takes `0` damage from poison swamps)<br>• **R4:** *Ascetic Clarity* (Combat: Purges all debuffs when dropping below `25%` HP) | **Hermit's Meditation:** Natural MP regeneration is doubled during overland travel and at the start of every third combat turn. |

---

### Pillar V: Ecology, Husbandry & Veil Seers (5 Jobs)

Frontier beast handlers, totem artisans, and astral mediums like Monica who commune with the Dreamscape veil.

| # | Job Title (Type) | Exploration Stat Focus | Rank 1–4 Active Vocation Skills *(While Holding Job Only)* | Rank 5 Permanent Mastery Perk *(Retained Forever)* |
| --- | --- | --- | --- | --- |
| **24** | **Beast Wrangler**<br>*(Summoner Class-Base)* | `CHA` / `WIS` | • **R1:** *Calm Beast* (Explore: Soothes wild fauna to avoid animal fights)<br>• **R2:** *Hunting Call* (Combat: Grants active summon `+2` Move Points)<br>• **R3:** *Pack Coordination* (Combat: Free follow-up shot when summon hits)<br>• **R4:** *Savage Command* (Combat: Grants summon an immediate extra attack) | **Wild Kinship:** Common predatory wildlife (wolves, bats, forest bears) never initiate hostility on exploration maps unless attacked first. |
| **25** | **Veil Medium / Shaman**<br>*(Summoner Class-Base — Monica's Calling)* | `WIS` / `CHA` | • **R1:** *Ethereal Attunement* (Explore: Hears ancestral whispers in ruins)<br>• **R2:** *Spiritual Tether* (Combat: `30%` of summon damage heals the caster)<br>• **R3:** *Channeling Veil* (Combat: `+25%` ranged evasion while summon lives)<br>• **R4:** *Ancestral Convergence* (Combat: Sacrifices summon to heal squad) | **Veil Vision:** Dreamscape tears, invisible spectral walkways, and hidden astral caches become permanently visible across all maps. |
| **26** | **Totem Carver**<br>*(Summoner/Artisan Job)* | `STR` / `WIS` | • **R1:** *Perimeter Ward* (Explore: Camp totem repels nocturnal monsters)<br>• **R2:** *Stone Sentinel* (Combat: Places blocker totem on adjacent cell)<br>• **R3:** *Resonance Pole* (Combat: Totems detonate for blunt damage on death)<br>• **R4:** *Bastion Monolith* (Combat: Totem grants `+20%` DEF to adjacent allies) | **Enduring Pillars:** All summoned totems, *Ice Walls*, and constructed barricades gain `+50%` Max HP and last `+2` additional turns. |
| **27** | **Leyline Surveyor**<br>*(Mage/Summoner Hybrid Job)* | `INT` / `PER` | • **R1:** *Detect Conduits* (Explore: Reveals mana springs and leyline nodes)<br>• **R2:** *Node Siphon* (Combat: Standing on elemental tile doubles MP regen)<br>• **R3:** *Terrain Shift* (Combat: Converts adjacent cell to Fire/Ice/Shock tile)<br>• **R4:** *Conduit Burst* (Combat: Detonates all active elemental tiles in range) | **Leyline Absorption:** Stepping on hazardous combat or dungeon tiles (burning ground, lava, spikes) deals `0` damage and restores `5` MP instead. |
| **28** | **Planar Stalker**<br>*(Rogue/Summoner Hybrid Job)* | `PER` / `DEX` | • **R1:** *Dimensional Blink* (Explore: Blinks 3 tiles across narrow chasms)<br>• **R2:** *Phase Step* (Combat: Teleports 2 tiles ignoring obstacles and ZOC)<br>• **R3:** *Rift Ambush* (Combat: `+30%` armor piercing after teleporting)<br>• **R4:** *Planar Pocket* (Combat: Vanishes for 1 turn; returns with full TP) | **Ethereal Stride:** The character can phase directly through locked iron portcullises, cell bars, and grates on exploration maps. |

---

## 7. Layer 1 ISO C11 Data Architecture (`ALLOC_ZERO_HEAP`)

To implement this complete 35-Class and 28-Job architecture within Layer 1 (`include/sim/`, `src/sim/`) while strictly honoring the **OmniCode 5-Block Protocol** and **Zero-Heap Allocation (`ALLOC_ZERO_HEAP`)** invariant, all progression states are encoded using fixed-width bitmasks and static lookup arrays:

```c
/*============================================================================*
 * [ SUB-ASSEMBLY 2.1: ORTHOGONAL CLASS & JOB PROGRESSION CONTRACTS ]         *
 *============================================================================*/

/**
 * @brief Foundational 5-Domain Bitmask Flags (Pascal's Triangle Basis).
 * Every Tier 1-4 class is uniquely identified by its 5-bit domain mask!
 */
typedef enum {
    DOMAIN_NONE     = 0x00,
    DOMAIN_WARRIOR  = (1u << 0), /* 0x01: Martial / Steel (Basic Base)    */
    DOMAIN_MAGE     = (1u << 1), /* 0x02: Arcane / Science (Basic Base)   */
    DOMAIN_ROGUE    = (1u << 2), /* 0x04: Subterfuge / Speed (Basic Base) */
    DOMAIN_PRIEST   = (1u << 3), /* 0x08: Divine / Miracles (Adv. Base)   */
    DOMAIN_SUMMONER = (1u << 4)  /* 0x10: Astral / Spirits (Adv. Base)    */
} class_domain_mask_t;

/**
 * @brief Complete 35-Class Enumeration (Tiers 1 through 5).
 */
typedef enum {
    /* Tier 1: Base Classes (5 Standard + 2 Special Initiates = 7) */
    CLASS_WARRIOR = 0, CLASS_MAGE, CLASS_ROGUE, CLASS_PRIEST, CLASS_SUMMONER,
    CLASS_TRAINEE,     CLASS_MYSTIC,
    /* Tier 2: Dual-Domain Hybrids (5C2 = 10) */
    CLASS_SPELLBLADE,  CLASS_DUELIST,      CLASS_PALADIN,    CLASS_TOTEMIC_WARDEN,
    CLASS_TRICKSTER,   CLASS_SAGE,         CLASS_ELEMENTALIST, CLASS_INQUISITOR,
    CLASS_PUPPETEER,   CLASS_ORACLE,
    /* Tier 3: Tri-Domain Masters (5C3 = 10) */
    CLASS_BATTLE_ADEPT, CLASS_CRUSADER,    CLASS_RUNE_WARDEN, CLASS_ZEALOT_JUSTICIAR,
    CLASS_PACK_MARSHAL, CLASS_DIVINE_GUARDIAN, CLASS_SHADOWWEAVER, CLASS_PHANTASMANCER,
    CLASS_HIGH_THEURGE, CLASS_CHRONO_STALKER,
    /* Tier 4: Quad-Domain Paragons / Bystander Ceiling (5C4 = 5) */
    CLASS_HIGH_JUSTICIAR, CLASS_BEAST_WARLORD, CLASS_SACRED_MAGUS,
    CLASS_SHADOW_SAINT,   CLASS_ASCENDANT_MYSTIC,
    /* Tier 5: Apex Special Classes (Cosmic Crucible = 3) */
    CLASS_HERO,        CLASS_DARK_ONE,     CLASS_SAGE_OF_ANCIENTS,
    CLASS_COUNT = 35
} class_id_t;

/**
 * @brief Complete 28-Job World Vocation Enumeration.
 */
typedef enum {
    JOB_NONE_DIO_VILLAGE = -1, /* Uncommissioned state inside Dio Village */
    /* Pillar I: Frontier Labor & Wilderness Survival (0..5) */
    JOB_LUMBERJACK = 0, JOB_QUARRY_MINER, JOB_FRONTIER_TRAPPER,
    JOB_FIELD_FORAGER,  JOB_CAMP_COOK,    JOB_ROAD_CARAVANER,
    /* Pillar II: Civic Order, Military & Feudal Law (6..11) */
    JOB_TOWN_SENTRY,    JOB_ROYAL_MAN_AT_ARMS, JOB_BORDER_OUTRIDER,
    JOB_CITADEL_BAILIFF, JOB_COURT_HERALD,     JOB_SIEGE_ENGINEER,
    /* Pillar III: Scholarship, Commerce & Underworld Craft (12..17) */
    JOB_ACADEMY_ARCHIVIST, JOB_FIELD_WARMAGE,  JOB_GUILD_LOCKSMITH,
    JOB_UNDERGROUND_SMUGGLER, JOB_HIGHWAY_PEDDLER, JOB_FIELD_CHIRURGEON,
    /* Pillar IV: Ecclesiastical & Faith Services (18..22) */
    JOB_CHAPEL_ACOLYTE, JOB_CHURCH_INQUISITOR, JOB_CHURCH_LITURGIST,
    JOB_RELIC_SCRIBE,   JOB_SHRINE_HERMIT,
    /* Pillar V: Ecology, Husbandry & Veil Seers (23..27) */
    JOB_BEAST_WRANGLER, JOB_VEIL_MEDIUM,       JOB_TOTEM_CARVER,
    JOB_LEYLINE_SURVEYOR, JOB_PLANAR_STALKER,
    JOB_COUNT = 28
} job_id_t;

/**
 * @brief Zero-Heap Per-Actor Class & Job Progression State.
 */
typedef struct {
    class_id_t active_class;                 /* Current equipped Combat Class      */
    job_id_t   active_job;                   /* Current equipped Vocation (-1=Dio) */
    uint8_t    class_mastery_pct[CLASS_COUNT]; /* 0..100% (NG+ starts at 15..25%)  */
    uint64_t   mastered_classes_mask;        /* Bitmask of mastered classes (0..34)*/
    uint8_t    job_rank[JOB_COUNT];          /* Current Rank 0..5 for each Job     */
    uint32_t   banked_job_perks_mask;        /* Bitmask of Rank 5 Permanent Perks  */
    uint8_t    ng_plus_encore_unlocked_mask; /* Bitmask of 5 Base Encore Skills    */
} actor_vocation_state_t;
```

### Key Architectural Invariants Verified

1. **Zero Redundancy:** Every Tier 1 through Tier 4 class maps to a unique 5-bit `class_domain_mask_t` (`2^5 - 1 = 31` non-empty subsets: `C(5,1) + C(5,2) + C(5,3) + C(5,4) = 5 + 10 + 10 + 5 = 30` mortal classes, while the 5-domain all-mask `0x1F` is reserved for the Tier 5 Cosmic Apex).
2. **Moral Sway Decoupling:** Unlocking `CLASS_HERO` checks `(moral_sway >= 50)` and `CLASS_DARK_ONE` checks `(moral_sway <= -50)`, independent of equipped relic IDs (`ITEM_SHIELD_OF_LUX` or `ITEM_SWORD_OF_TENEBRIS`).
3. **Bystander Corridor Enforcement:** Actors with `moral_sway` in `(-50, +50)` are mathematically blocked from transitioning to `CLASS_HERO` or `CLASS_DARK_ONE`, capping their progression at Tier 4.

---

> **Series Navigation (Markdown Mirror):**
> [Series Index (AsciiDoc)](README.adoc) | [← Part 4: Master Stat Sheet](Offline-Work-4.md) | **Part 5: Orthogonal Class & Job Spec** | [Part 6: Generational War & Dream Clock →](Offline-Work-6.md)

