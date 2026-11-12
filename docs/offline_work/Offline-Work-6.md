<!--
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance & Narrative / System Series Anchors )
================================================================================
@project       Tenebris and Lux
@file          docs/offline_work/Offline-Work-6.md
@id            docs.offline_work.part_6.md
@layer         Documentation / Pre-Production Chronicles (Part 6 of 6)
@version       0.2.0-dev
@date          2026-11-12
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@series        Previous: Offline-Work-5.md | Index: README.adoc | End of Series
================================================================================
-->

> **Series Navigation (Markdown Mirror):**
> [Series Index (AsciiDoc)](README.adoc) | [← Part 5: Orthogonal Class & Job Spec](Offline-Work-5.md) | **Part 6: Generational War & Dream Clock** *(End of Series)*
> *Canonical PDF/AsciiDoc Edition:* [Offline-Work-6.adoc](Offline-Work-6.adoc)

# Tenebris and Lux: Definitive Lore, Class/Vocation Architecture, Generational Narrative & Temporal Simulation Compendium

**Document ID:** `docs/game_design/master_narrative_vocation_and_time_spec.adoc` (Archival Part 6: `docs/offline_work/Offline-Work-6.adoc`)

**Author:** CreativeWorkzStudio LLC / ProfessorSeanEX

**Date:** `2026-11-09` (Canon Synthesis) / `2026-11-12` (Series Integration)

**Engine Standard:** ISO C23 Target / C11 Floor (`ALLOC_ZERO_HEAP`) / OmniCode 5-Block Ontological Architecture

**Scope:** Unified specification consolidating Master Guide canon clarifications, the 35-Class & 28-Job orthogonal progression system, the 10-to-20-year generational coming-of-age chronicle, the geopolitical mechanics of the 2nd Great War, the Two-Pass Grand Tour structure, and the 4D "Dream Clock" open-world temporal registration engine.

---

## 1. Master Guide Reconciliation & Clarified Canon Matrix

As the 2017–2018 RPG Maker MV datasets (**Folder 1**: `Old Game Data`, Version `40730002`, 36 maps; **Folder 2**: `Old_Game_Data_2/data`, Version `2078310`, 43 maps) were cross-referenced with the 2026 C11 architectural rebuild, several historical database quirks and early documentation assumptions were formally resolved.

| Domain / System | Prior Master Guide / Demo Assumption | Clarified Canonical Reality (2026 Specification) |
| --- | --- | --- |
| **The "7 Combat Classes" Table** (`classes_and_growth.adoc`) | Treated **Warrior/Knight, Mage, Priest, Summoner, Hero, Thief, and Guard** as a flat peer array of 7 starter-to-endgame classes scaling from Lv 1–99. | The 2018 table compressed three distinct systemic layers into one flat array:<br><br>• **Basic Base Classes (3):** Warrior, Mage, Rogue (Thief).<br>• **Advanced Base Classes (2):** Priest, Summoner.<br>• **Tier 5 Moral Apex Class:** **Hero** (`+50 ... +100` Active Virtue), paired with **The Dark One** (`-100 ... -50` Active Malice).<br>• **World Vocation (Job):** **Guard** (the *Town Sentry / Royal Man-at-Arms* occupation, not a class domain). |
| **Protagonist Story Classes vs. Titles** | Referred to Elijah as *"The Vanguard of Lux"* (`Primary Class: Hero`), Katrina as *"The Outrider of Tenebris"* (`Primary Class: Hero`), and `<chosen_name>` as *"The Secret Chronicler"*. | Those titles describe tactical postures and narrative focalization, not mechanical starter classes. Story-wise, Elijah and Katrina are **Trainees** (training Warrior + Mage + Rogue simultaneously), and `<chosen_name>` is a **Mystic** (training Priest + Summoner under Chrono in the Dreamscape). |
| **Sir Roté’s Dual Designation** | Narrative walkthroughs called him **Senior Outrider Sir Roté**, while character dossiers listed him as **High Priest Roté (Royal Advisor)**. | Reconciled by the orthogonal **Class × Job** architecture: Roté’s **Class (Domain)** is in the **Priest** lineage, while his **Job (World Vocation)** for the Crown is **Border Outrider / Court Herald**. |
| **Relics vs. Moral Apex Classes** | Risked conflating the **Shield of Lux** with the Hero class and the **Sword of Tenebris** with darkness or evil. | **Equipment, Element, and Conscience are strictly decoupled**. The Shield and Sword are mundane founding symbols of Peace and War infused with morally neutral cosmic forces (**Lux** and **Tenebris**). **The Hero** and **The Dark One** are purely moral classes governed by the `[-100, +100]` **Compass of Morality**. |
| **Overarching Antagonist Name & Class** | Early Volume I chapters used the synthetic placeholder *"Dark Lord Malakor"* before DevLog 008 restored **"The Dark Lord"**. | **Usuru** is the historical person; **"The Dark Lord"** is his overarching title; and **"The Dark One"** is his literal **Tier 5 Apex Class**—the exact class the player unlocks if they descend into Active Malice (`-100 ... -50`). |
| **The Bystander Corridor (`[-50, +50]`)** | Treated primarily as a narrative filter at the end of the campaign that routes uncommitted players to the Neutral Ending. | Enforced mechanically on the character sheet: remaining within the **Bystander Corridor (`[-50, +50]`)** caps the player at **Tier 4 (The Mortal Summit)**. Without extreme moral conviction or perspective synthesis, they cannot ascend to Tier 5. |
| **The Room of Sages (Map 006)** | Viewed as a one-time prologue character-creation room before awakening in Dio Village. | Operates as a **multi-cycle meta-progression archive** across the Dreamscape. Run 1 offers single-domain "training wheels"; the 50% midpoint unlocks Advanced classes; beating the game (100%) unlocks the official Hard-Mode story classes (**Trainee** & **Mystic**) and NG+ Encore Skills. |

---

## 2. The Orthogonal Character Engine: 35 Classes (Domains) & 28 Jobs (Vocations)

Inspired by *Dokapon Kingdom* and grounded in the socio-political reality of the **Kingdom of Sacyra**, character progression is split into two orthogonal axes that work hand-in-hand: **Class (The Combat Domain)** and **Job (The World Vocation)**.

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

### 2.1. Bidirectional Stat Coupling (Combat ↔ Exploration)

Your **Class** powers the **Core 9 Combat Stats** on the 2D LeTBS tactical grid and in side-scroll action duels, while your **Job** powers the **8 Exploration Stats** used to traverse provinces, solve dungeon puzzles, and navigate faction politics:

* **`ATK` ↔ `STR` (Strength):** Physical weapon damage and armor sundering ↔ shoving boulders, forcing jammed iron portcullises, chopping timber, and inventory carry weight.
* **`AGI` ↔ `DEX` (Dexterity):** Turn timeline velocity, evasion, and grid movement points ↔ lockpicking, disarming mechanical traps, stealth tread, and clearing chasms.
* **`HP / DEF` ↔ `VIT` (Vitality):** Life capacity, physical mitigation, and poise resistance ↔ overland travel stamina and immunity to briers, miasma, and spoiled rations.
* **`MATK / MP` ↔ `INT` (Intelligence):** Arcane spell potency and mana pool ("Science") ↔ translating dead scripts, runic logic puzzles, and alchemical distillation.
* **`MDEF` ↔ `WIS` (Wisdom):** Spiritual warding and ailment resistance ("Magic") ↔ detecting NPC deception, sensing Dreamscape tears, and non-magical wound triage.
* **`ACC / CRIT` ↔ `PER` (Perception):** Flanking critical hit rates and line-of-sight scopes ↔ spotting concealed chests, darkvision radius, and tracking wilderness quarry.
* **`TP` ↔ `CHA` (Charisma):** Combat momentum generation and summon duration ↔ merchant bartering, companion morale, and pacifying wild fauna.
* **`Relic Resonance` ↔ `REP` (Reputation):** *Lux Barrio* and *Cuttatio Tenebris* synergy ↔ faction standing across Sacyra, bypassing military checkpoints, and court audiences.

### 2.2. Diegetic Framing: "Unofficial" Story Identity vs. "Official" Hard Mode

The narrative of *Tenebris and Lux* operates across two planes: the historical waking campaign in Sacyra and the instructional framing archive inside **The Dreamscape** (Map 006: Room of Sages), where **Chrono the Sage** teaches a newly initiated **Mystic**.

1. **Elijah Ro-enheart & Lady Katrina Jaren (Unofficial "Trainees"):** Inside the story, Elijah and Katrina are 15-to-18-year-old youths—Elijah is an unseasoned cadet grieving his vanished father (Commander Gareth), and Katrina is a runaway noblewoman trying to qualify for the Adventurers' Guild. Story-wise, neither is a master Warrior or Archmage yet; they are **Trainees** forced to learn steel, magic, and stealth simultaneously on the road.
2. **The Custom Bearer `<chosen_name>` (Unofficial "Mystic"):** Story-wise, the Custom Player is a **new Mystic** being told the tale of *Tenebris and Lux* by Chrono, experiencing Elijah and Katrina's memories through the Ancient Book to complete their own ascension toward becoming a **Sage of Ancients**.
3. **Why the Game Changes When It Becomes "Official":** On a first playthrough, touching a single Base Class stone (Warrior, Mage, or Rogue) gives the player "training wheels" with a standard single-domain EXP curve. Once the player beats the game (100% milestone) and unlocks the Special Classes, they can **officially** select **Trainee** or **Mystic** in New Game+—removing the training wheels, aligning their mechanical sheet with their true story identity, and enduring a slower multi-domain "Hard Mode" progression that leads directly to **The Hero / The Dark One** or **Sage of Ancients**.

### 2.3. Campaign Milestone Gating & New Game+ (NG+) Meta-Progression

| Campaign Milestone | Narrative Trigger | Unlocked Class Tiers | Diegetic & Mechanical Rationale |
| --- | --- | --- | --- |
| **0% — Fresh Start** *(Run 1, Act I)* | Awakening in **The Dreamscape** (Map 006) under Chrono's observation. | **Tier 1 Basic Base Classes** (*Warrior, Mage, Rogue*). | The initiate begins with the three tangible, worldly disciplines required to survive Dio Village and the Whispering Woods. |
| **50% — Midpoint Climax** *(Half the Game)* | **Sacyra Castle Siege:** Confronting **King Jihad** during the *Festival of Heroes* as the planar seal beneath the High Throne ruptures. | **Tier 1 Advanced Base** (*Priest, Summoner*) + **Tier 2–4 Combinatorial Classes**. | As the Veil thins and the political conspiracy exposes a 3,000-year cosmic wound, spiritual miracles, astral pacts, and hybrid syntheses awaken. |
| **100% — Full Clear** *(Beating the Game)* | **The Sovereign Eclipse Finale:** Resolving the conflict with **The Dark Lord** and recording a completed cycle in Chrono's Ledger. | **Special Initiate Classes** (*Trainee, Mystic*) + **Tier 5 Apex Classes** (*Hero, Dark One, Sage of Ancients*). | Having witnessed a full cycle of history, Chrono unlocks the official Hard-Mode story classes and NG+ carryover for subsequent runs. |

#### New Game+ Carryover Rules

* **Unlocked Base Persistence:** Any Base Class unlocked in a previous campaign (*Warrior, Mage, Rogue, Priest, Summoner*, plus *Trainee* and *Mystic*) is available from minute one in the Room of Sages on NG+.
* **15–25% Mastery Head Start ("Attunement Resonance"):** Re-selecting a class that was mastered in a prior campaign grants a **15% head start** on its mastery gauge (expanding to **25%** if mastered across both Elijah and Katrina's routes).
* **NG+ "Encore Mastery" Exclusive Skills:** Re-selecting a previously mastered Base Class in NG+ grants a pinnacle technique evolved from the 2017–2018 grimoire:
  * **Warrior → Sovereign Phantom Slash:** Teleports behind a target ignoring ZOC, inflicts armor-piercing damage, and triggers a 1-tile *Intimidating Aura*.
  * **Mage → Cataclysmic Convergence:** Detonates active *Ice Walls* or *Volcano Ground* tiles for double elemental burst while refunding `50%` MP.
  * **Rogue → Shadow Transposition:** Steals an enemy's active buff/item while swapping grid coordinates with them and planting an *Explosive Mark* on the vacated cell.
  * **Priest → Chrono's Emergency Decree:** Grants an ally an immediate out-of-turn *Emergency Turn* while cleansing all debuffs and applying *Basic Ward*.
  * **Summoner → Astral Triad Covenant:** Summons two auxiliary spirits simultaneously (e.g., *Sentinel Totem* + *Pixie*) that inherit `25%` of the Summoner's DEF and MDEF.

---

### 2.4. The 5-Tier Combinatorial Class Compendium (All 35 Classes)

Governed by Pascal's Triangle (`C(5,1) -> C(5,2) -> C(5,3) -> C(5,4) -> Apex`), every mortal class from Tier 1 to Tier 4 is uniquely defined by its 5-bit domain mask (`W, M, R, P, S`), guaranteeing **zero redundant combinations**.

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

#### Tier 1: Foundational Base Classes (7 Classes)

1. **Warrior (`W` — Basic Base / Martial Domain):** Physical steel, shield bracing, ZOC lockdown, and poise (`HP`/`ATK`/`DEF`/`TP`). Skills: *Repulsive Blow*, *Flash Jump*, *Rush*, *Provoke*, *Heavy Hit*, *Phantom Slash*, *Intimidating Aura*. Mastery Passive: *Unshakable Poise* (`+15%` DEF, stagger immunity).
2. **Mage (`M` — Basic Base / Arcane "Science" Domain):** Academy leyline geometry, elemental synthesis, and terrain shaping (`MP`/`MATK`/`INT`). Skills: *Fire*, *Sparkle*, *Fire Storm*, *Lightning Storm*, *Volcano Ground*, *Ice Fury*, *Ice Prison*, *Ice Wall*. Mastery Passive: *Leyline Conduit* (`+15%` Max MP, `+10%` elemental potency).
3. **Rogue (`R` — Basic Base / Subterfuge Domain):** Speed, flanking criticals, trap manipulation, and field theft (`AGI`/`LUK`/`DEX`/`PER`). Skills: *Pick Up*, *Push*, *Whitch's Escape*, backstab ambush. Mastery Passive: *Shadow Reflex* (`+10%` EVA, `+15%` flank/rear crit rate).
4. **Priest (`P` — Advanced Base / Divine "Magic" Domain):** Devotion to Goddess Lyri, purification, and timeline control (`MP`/`MDEF`/`WIS`). Skills: *Heal*, *Salvation*, *Basic Ward*, *Teleportation*, *Ally Teleport*, *Ghost Arrow*, *Magic Trap*, *Explosive Mark*, *Accelerate*, *Extra Turn*, *Emergency Turn*. Mastery Passive: *Covenant Grace* (`-15%` heal MP cost, `+10%` MDEF buff).
5. **Summoner (`S` — Advanced Base / Astral Pact Domain):** Calling independent spirit entities and totems onto the grid (`MP`/`MDEF`/`CHA`/`WIS`). Skills: *Sentinel Totem*, *Wind Spirit* (*Great Wind*, *Transposition*), *Summon Pixie* (*Healing Wave*), *Fire Chickens* (*Bomb*). Mastery Passive: *Pact Tether* (Summons inherit `+15%` caster HP/DEF).
6. **Trainee (`W+M+R` — Special Hard-Mode Initiate):** Official story class of **Elijah** and **Katrina**. Trains all 3 Basic Base classes simultaneously at `2.2×` EXP requirement; matures through *Battle Adept* directly toward **The Hero** or **The Dark One**.
7. **Mystic (`P+S` — Special Hard-Mode Initiate):** Official story class of the **Custom Bearer (`<chosen_name>`)**. Trains both Advanced Base classes simultaneously at `1.8×` EXP requirement; matures through *Oracle* and *Ascendant Mystic* directly toward **Sage of Ancients**.

#### Tier 2: Dual-Domain Advanced Hybrids (`C(5,2) = 10` Classes)

1. **Spellblade (`W+M`):** Imbues melee strikes (*Rush*, *Heavy Hit*) with Fire/Ice and raises close-range *Ice Walls* to trap foes.
2. **Duelist (`W+R`):** High-poise skirmisher chaining *Flash Jump* and *Phantom Slash* for parry ripostes and deep flanking.
3. **Paladin (`W+P`):** Holy phalanx protector pairing *Provoke* and *Intimidating Aura* with *Heal* and *Basic Ward*.
4. **Totemic Warden (`W+S`):** Frontline anchor who drops *Sentinel Totems* and knocks enemies into totem crossfire via *Repulsive Blow*.
5. **Trickster (`M+R`):** Guerrilla saboteur who blinks away via *Whitch's Escape* while leaving *Explosive Marks* and *Ice Prisons*.
6. **Sage (`M+P`):** Bridges Sacyra's **Science (Academy)** and **Magic (Church)** schism; weaves *Fire/Lightning Storm* with *Salvation*.
7. **Elementalist (`M+S`):** Infuses summons with live elemental storms (*Fire Chickens* detonating into *Volcano Ground*).
8. **Inquisitor (`R+P`):** Sanctified hunter chaining *Light Steps* and *Teleportation* to execute backline casters with *Ghost Arrow*.
9. **Puppeteer (`R+S`):** Misdirection tactician using *Transposition* to swap bosses into traps while looting contraband via *Pick Up*.
10. **Oracle (`P+S`):** Timeline conduit combining *Pixie Healing Wave* with *Accelerate*, *Extra Turn*, and *Emergency Turn*.

#### Tier 3: Tri-Domain Master Classes (`C(5,3) = 10` Classes)

1. **Battle Adept (`W+M+R`):** Mature Trainee; chains martial leaps, elemental blade weaves, and stealth disengagement in one fluid turn.
2. **Crusader (`W+M+P`):** Armored battle-theurge calling down elemental judgment from inside sanctified barrier domes.
3. **Rune Warden (`W+M+S`):** Carves elemental sigils onto summoned beasts and totems, turning summons into armored hazards.
4. **Zealot Justiciar (`W+R+P`):** Relentless pursuer who teleports into enemy formations, shatters buffs, and locks down casters.
5. **Pack Marshal (`W+R+S`):** Wolfpack commander flanking in synchronous formation with *Wind Spirits* for automatic pincer criticals.
6. **Divine Guardian (`W+P+S`):** Immortal phalanx linking HP across multiple *Sentinel Totems* and healing allies whenever struck.
7. **Shadowweaver (`M+R+P`):** Clandestine occultist casting silent elemental curses from stealth while siphoning enemy MP.
8. **Phantasmancer (`M+R+S`):** Floods the grid with elemental decoy clones that swap places with allies when attacked.
9. **High Theurge (`M+P+S`):** Commands all three non-martial pillars, unleashing dual elemental-holy storms through spirit relays.
10. **Chrono Stalker (`R+P+S`):** Temporal phantom freezing enemy turn gauges and teleporting summons into rear flanks.

#### Tier 4: Quad-Domain Paragons (`C(5,4) = 5` Classes — The Mortal Summit)

> **The Bystander Floor Rule:** Any player who reaches the endgame with their Moral Sway inside the **Bystander Corridor (`[-50, +50]`)** is permanently capped at Tier 4 and resolves the story on the **Neutral Ending**.

1. **High Justiciar (`W+M+R+P` — *Omits Summoner*):** The supreme mortal general relying purely on personal mastery of steel, spell, stealth, and faith.
2. **Beast Warlord (`W+M+R+S` — *Omits Priest*):** Forsakes church dogma to conquer through brute steel, elemental cataclysms, ambushes, and spirit hordes.
3. **Sacred Magus (`W+M+P+S` — *Omits Rogue*):** Discards stealth; an immovable citadel of heavy plate, arcane artillery, miracles, and monoliths.
4. **Shadow Saint (`W+R+P+S` — *Omits Mage*):** Rejects academic formulas for miraculous speed, shadow transposition, and spirit-assisted executions.
5. **Ascendant Mystic (`M+R+P+S` — *Omits Warrior*):** Discards physical steel to command arcane, shadow, divine, and astral forces; natural Tier 4 threshold for the **Mystic** path.

#### Tier 5: Apex Special Classes (The Cosmic Crucible — 3 Classes)

1. **The Hero (Moral Good Apex — `+50 ... +100` Active Virtue):** Unlocks full resonance with equipped relics, *Duo-Loca Bashin n Slash*, and immunity to the emotional panic dissipation penalty on *Lux Barrio*. Steers toward the **Heroic Restoration Ending** or **10th True Ending ("The Ugly Truth")**.
2. **The Dark One (Moral Evil Apex — `-100 ... -50` Active Malice):** **The literal class of The Dark Lord (Usuru)**. Sacrifices defensive stability for **Abyssal Rupture** (`35%` DEF/MDEF shred + entropic rot/petrifying stasis) and **Sovereign Terror** (panic aura that shatters enemy Lux barriers). Steers toward the **Abyssal Usurper Ending**.
3. **Sage of Ancients (Veil Apex — Mystic Path + Nocturnal Dreamscape Synthesis):** The class of **Chrono the Sage**. Unlocks **Planar Convergence**, **Temporal Rewind**, and effortless **Harmonic Equilibrium (`0`)**.

#### The 4-Quadrant Morality × Element Expression Matrix

| Moral Apex Class ↓ \ Elemental Medium → | **Expressing Through Lux (Radiance / Cohesion)** | **Expressing Through Tenebris (Darkness / Severance)** |
| --- | --- | --- |
| **THE HERO**<br>*(Active Good: `+50 ... +100`)* | **The Selfless Aegis (Mi-ren's Ideal Perfected):**<br>Projects golden *Lux Barrio* domes (`25%–50%` mitigation) to shelter the weak and preserve truth without tyrannical control. | **The Surgical Liberator (Casatti's Ideal):**<br>Wields *Cuttatio Tenebris* (`20%` splash cleave) to sever slave chains, cut through tyrant armor, and execute **Absolute Justice**. |
| **THE DARK ONE**<br>*(Active Evil: `-100 ... -50`)* | **The Calcified Despot:**<br>Uses blinding totalitarian light to petrify dissenters into living stone, freezing the realm into comatose obedience. | **The Sovereign of Ruin (The Dark Lord / Usuru):**<br>Unleashes entropic void cleaves that devour life force, spread creeping rot, and consume Sacyra in nihilistic vengeance. |

---

### 2.5. The 5-Rank World Vocation Compendium (All 28 Jobs)

* **The "No Starter Job in Dio Village" Threshold Rule:** Players awaken in the Dreamscape with a **Class**, but have **no Job** (`JOB_NONE_DIO_VILLAGE`) while inside the domestic sanctuary of Dio Village. Acquiring your first Job coincides with **Crossing the First Threshold**—leaving Dio Village for the Whispering Woods, helping Eric clear timber roads, or registering at the Adventurers' Guild outpost.
* **Why Rank 1–4 Job Skills Don't Carry Over:** Class skills are internal muscle memory and soul attunement (retained forever). Active Job skills (Ranks 1–4) are **external station tools, legal warrants, and operational kits** (e.g., a guard's whistle, a locksmith's tension wrench). When you change jobs, you swap out your active station kit, but **Rank 5 (Master)** permanently banks a **Mastery Perk** into your character ledger forever.

| # | Pillar & Job Title | Stat Focus | Rank 1–4 Active Station Tools *(Equipped Only While Holding Job)* | Rank 5 Permanent Mastery Perk *(Retained Forever)* |
| --- | --- | --- | --- | --- |
| **1** | **Pillar I (Frontier)**<br>**Lumberjack / Feller** *(Eric)* | `STR` / `VIT` | **R1:** *Hew Timber* (Chops logs) \| **R2:** *Splitting Chop* (`130%` hit, breaks wood shields) \| **R3:** *Rooted Poise* (`+30%` poise) \| **R4:** *Timber Toss* (3-tile log throw) | **Felling Cleave:** Instantly destroys wooden barricades and fallen trees across exploration and combat grids for `0` stamina/tools. |
| **2** | **Pillar I (Frontier)**<br>**Quarry Miner / Delver** | `STR` / `VIT` / `PER` | **R1:** *Prospecting Strike* (Mines ore) \| **R2:** *Spalling Strike* (`-5` target DEF) \| **R3:** *Delver's Flare* (`2×` cave sight) \| **R4:** *Cave-In Collapse* (Wall rockfall) | **Shatter Strike:** Breaks cracked dungeon walls and cave-in boulders without consuming pickaxes or bombs. |
| **3** | **Pillar I (Frontier)**<br>**Frontier Trapper** | `PER` / `DEX` | **R1:** *Field Snare* (Camp meat/pelts) \| **R2:** *Caltrop Scatter* (`-2` Move line) \| **R3:** *Big Game Bane* (`+25%` vs Beasts) \| **R4:** *Spring-Jaw Trap* (2-turn root) | **Ambush Snare:** At the start of every wilderness battle, one random non-boss enemy begins immobilized in a snare for Turn 1. |
| **4** | **Pillar I (Frontier)**<br>**Field Forager / Herbalist** | `INT` / `WIS` | **R1:** *Flora Sense* (Shows herb nodes) \| **R2:** *Poultice Pack* (3-turn regen) \| **R3:** *Venom Filter* (`+50%` Poison resist) \| **R4:** *Field Distillation* (Camp brewing) | **Bountiful Harvest:** Overworld flora nodes yield `2×` materials and `+25%` chance for rare relic-embellishment catalysts. |
| **5** | **Pillar I (Frontier)**<br>**Camp Cook / Provisioner** | `VIT` / `CHA` | **R1:** *Trail Stew* (`+10%` Max HP meal) \| **R2:** *Hearty Morsel* (`+25` TP toss) \| **R3:** *Spiced Jerky* (`-50%` stamina drain) \| **R4:** *Feast of Accord* (`+15%` elem resist) | **Iron Stomach:** Food consumables are `+50%` more effective; party is immune to spoiled ration and disease debuffs. |
| **6** | **Pillar I (Frontier)**<br>**Road Caravaner** | `VIT` / `STR` | **R1:** *Draft Rigging* (`+10` bag slots) \| **R2:** *Whip Crack* (Interrupt snap) \| **R3:** *Reinforced Axle* (Ignores mud delay) \| **R4:** *Wagon Phalanx* (`+15%` adjacent DEF) | **Highway Endurance:** Party exploration movement speed increases by `+20%` when following roads, bridges, and paved trails. |
| **7** | **Pillar II (Civic/Law)**<br>**Town Sentry / Watchman** | `VIT` / `REP` | **R1:** *Shield Brace* (`+20%` front block) \| **R2:** *Watchful Eye* (Shows sight cones) \| **R3:** *Halt!* (`-2` Move bash) \| **R4:** *Phalanx Intercept* (Swaps to tank hit) | **Vigilant Posture:** Party is permanently immune to surprise ambushes during exploration, room transitions, and camp rests. |
| **8** | **Pillar II (Civic/Law)**<br>**Royal Man-at-Arms** | `STR` / `VIT` | **R1:** *Sundering Strike* (`-15%` DEF) \| **R2:** *Threat Appraisal* (Inspects patrol stats) \| **R3:** *Adrenaline Surge* (`+20` TP on crit/kill) \| **R4:** *Sweeping Cleave* (3-tile arc) | **Brutal Leverage:** Forces open rusted portcullises, jammed grates, and heavy iron doors without keys via raw `STR` checks. |
| **9** | **Pillar II (Civic/Law)**<br>**Border Outrider** *(Sir Roté)* | `PER` / `DEX` | **R1:** *Courier Canter* (`+25%` dash speed) \| **R2:** *Mark Weakness* (`+15%` ACC / `+10%` Crit) \| **R3:** *Skirmish Sprint* (`+2` Move) \| **R4:** *Disengaging Strike* (Hit & leap back) | **Unimpeded Stride:** Permanently ignores terrain movement penalties (mud, deep sand, snow, briers) in exploration and battle. |
| **10** | **Pillar II (Civic/Law)**<br>**Citadel Jailer / Bailiff** | `WIS` / `STR` | **R1:** *Warrant Pass* (Prison access) \| **R2:** *Manacle Bind* (Disables physical skills) \| **R3:** *Suppressive Glare* (`-15%` ATK aura) \| **R4:** *Sentence Delivered* (`+50%` vs bound) | **Iron Custody:** Capturing or pacifying humanoid foes yields `2×` bounty gold and automatically extracts dungeon keycards. |
| **11** | **Pillar II (Civic/Law)**<br>**Court Herald / Emissary** | `CHA` / `REP` | **R1:** *Royal Writ* (Noble audiences) \| **R2:** *Rallying Cry* (`+15` TP in 3 tiles) \| **R3:** *Truce Envoy* (Delays humanoid turn) \| **R4:** *Sovereign Decree* (`REP` stat buff) | **Diplomatic Immunity:** Bypasses hostile faction tollgates and military checkpoints across Sacyra without bribes or fights. |
| **12** | **Pillar II (Civic/Law)**<br>**Siege Engineer** | `INT` / `STR` | **R1:** *Fortify Choke* (Builds barricade) \| **R2:** *Structural Flaw* (`3×` vs props/ice) \| **R3:** *Trench Works* (Drains/bridges rooms) \| **R4:** *Demolition Charge* (Cross bomb) | **Breach Expertise:** Party attacks permanently deal `+30%` bonus damage against constructs, golems, and destructible grid props. |
| **13** | **Pillar III (Craft/Trade)**<br>**Academy Archivist** | `INT` / `PER` | **R1:** *Rune Inscription* (Unlocks seals) \| **R2:** *Mana Siphon* (Drains `15` MP) \| **R3:** *Arcane Efficiency* (`-20%` spell MP) \| **R4:** *Leyline Overcharge* (`+40%` MDEF pierce) | **Ancient Literacy:** Permanently reads and translates all dead languages, archaic Sacyran scripts, and planar tablets without a lexicon. |
| **14** | **Pillar III (Craft/Trade)**<br>**Field Warmage** *(Monica)* | `INT` / `VIT` | **R1:** *Ignite / Freeze* (Burns brush/freezes water) \| **R2:** *Focused Blast* (`2×` single hit) \| **R3:** *Kinetic Mine* (Knockback rune) \| **R4:** *Siege Stance* (`+2` spell range) | **Elemental Transmutation:** Defeating an enemy with an elemental spell automatically drops `1` raw elemental crystal catalyst. |
| **15** | **Pillar III (Craft/Trade)**<br>**Guild Locksmith** | `DEX` / `PER` | **R1:** *Pick Apparatus* (Opens wood/iron locks) \| **R2:** *Disarm Mechanism* (Wire traps) \| **R3:** *Jam Tumbler* (Blocks enemy crits) \| **R4:** *Vault Breaker* (Master locks) | **Skeleton Finger:** Unlocking standard wooden, iron, and silver chests consumes zero lockpicks and neutralizes needle traps. |
| **16** | **Pillar III (Craft/Trade)**<br>**Underground Smuggler** *(John)* | `DEX` / `CHA` | **R1:** *Fence Contraband* (Appraises loot) \| **R2:** *Pocket Sand* (`-50%` ACC blind) \| **R3:** *Hidden Compartment* (Smuggles items) \| **R4:** *Shadow Mug* (Steals on backstab) | **Underworld Connections:** Permanently unlocks secret black-market vendors in major settlements and sells contraband at `100%` value. |
| **17** | **Pillar III (Craft/Trade)**<br>**Highway Peddler** | `CHA` / `REP` | **R1:** *Haggle* (`10%` discount/bonus) \| **R2:** *Coin Toss* (Rotates enemy facing) \| **R3:** *Supply Ledger* (Premium shop stock) \| **R4:** *Mercenary Payoff* (Bribes retreat) | **Keen Appraisal:** Automatically identifies the true market value, hidden stats, and historical lore of all unidentified relics. |
| **18** | **Pillar III (Craft/Trade)**<br>**Field Chirurgeon** | `WIS` / `DEX` | **R1:** *Bandage Wrap* (Stops Bleed, `0` MP) \| **R2:** *Bone Splint* (Cures fall fractures) \| **R3:** *Smelling Salts* (`15%` HP revive, `0` MP) \| **R4:** *Field Surgery* (`40%` HP heal, `0` MP) | **Triage Efficiency:** All mundane healing consumables (bandages, salves, poultices) restore `+50%` more HP, preserving MP for battle. |
| **19** | **Pillar IV (Faith)**<br>**Chapel Acolyte / Hospitaler** | `WIS` / `CHA` | **R1:** *Sanctify Spring* (Purifies wells) \| **R2:** *Graceful Word* (Cleanses `1` debuff) \| **R3:** *Benediction Aura* (`5%` HP/turn regen) \| **R4:** *Aegis of Faith* (Nulls `1` spell hit) | **Sanctified Respite:** Resting at any campfire or inn automatically purges all lingering poison, curse, and disease statuses for free. |
| **20** | **Pillar IV (Faith)**<br>**Church Inquisitor** | `WIS` / `PER` | **R1:** *Detect Malice* (Senses void/cults) \| **R2:** *Nullifying Smite* (Dispels buffs) \| **R3:** *Shackle of Truth* (Silences/roots 1 turn) \| **R4:** *Consecrated Ground* (`3×3` holy zone) | **Piercing Insight:** Dialogue windows automatically flag when an NPC is lying, concealing their faction, or hiding a secret motive. |
| **21** | **Pillar IV (Faith)**<br>**Church Liturgist** | `WIS` / `CHA` | **R1:** *Hymn of Comfort* (`+20%` squad MDEF) \| **R2:** *Calm Panic* (Cures Fear/Panic) \| **R3:** *Sanctuary Bell* (1-turn untargetable) \| **R4:** *Lyri's Chorus* (`+15%` radiant dmg) | **Unshakable Faith:** Permanently immune to Fear, Panic, and Silence, preventing *Lux Barrio* from dissipating under boss terror. |
| **22** | **Pillar IV (Faith)**<br>**Relic Scribe** | `INT` / `WIS` | **R1:** *Inscribe Ward* (Crafts barrier scrolls) \| **R2:** *Parchment Seal* (Silence trap) \| **R3:** *Illuminated Vellum* (`+1` turn buff duration) \| **R4:** *Scripture of Banishment* (Holy burst) | **Scroll Conservation:** Using magical scrolls, talismans, and written consumables carries a `50%` chance not to consume the item. |
| **23** | **Pillar IV (Faith)**<br>**Shrine Hermit** | `WIS` / `VIT` | **R1:** *Fasting Discipline* (`-50%` camp food) \| **R2:** *Solitary Ward* (`+30%` lone DEF/MDEF) \| **R3:** *Miasma Cloak* (Swamp immunity) \| **R4:** *Ascetic Clarity* (Low-HP self-cleanse) | **Hermit's Meditation:** Natural MP regeneration is doubled during overland travel and at the start of every third combat turn. |
| **24** | **Pillar V (Ecology/Veil)**<br>**Beast Wrangler** | `CHA` / `WIS` | **R1:** *Calm Beast* (Pacifies wild fauna) \| **R2:** *Hunting Call* (`+2` summon Move) \| **R3:** *Pack Coordination* (Follow-up shot) \| **R4:** *Savage Command* (Extra summon attack) | **Wild Kinship:** Common predatory wildlife (wolves, bats, forest bears) never initiate hostility on exploration maps unless attacked first. |
| **25** | **Pillar V (Ecology/Veil)**<br>**Veil Medium / Shaman** | `WIS` / `CHA` | **R1:** *Ethereal Attunement* (Ruin whispers) \| **R2:** *Spiritual Tether* (`30%` summon vamp) \| **R3:** *Channeling Veil* (`+25%` ranged EVA) \| **R4:** *Ancestral Convergence* (Summon heal) | **Veil Vision:** Dreamscape tears, invisible spectral walkways, and hidden astral caches become permanently visible across all maps. |
| **26** | **Pillar V (Ecology/Veil)**<br>**Totem Carver** *(Monica)* | `STR` / `WIS` | **R1:** *Perimeter Ward* (Repels night raids) \| **R2:** *Stone Sentinel* (Blocker totem) \| **R3:** *Resonance Pole* (Death explosion) \| **R4:** *Bastion Monolith* (`+20%` DEF totem) | **Enduring Pillars:** All summoned totems, *Ice Walls*, and constructed barricades gain `+50%` Max HP and last `+2` additional turns. |
| **27** | **Pillar V (Ecology/Veil)**<br>**Leyline Surveyor** *(Casatti)* | `INT` / `PER` | **R1:** *Detect Conduits* (Finds mana nodes) \| **R2:** *Node Siphon* (`2×` tile MP regen) \| **R3:** *Terrain Shift* (Creates elem tile) \| **R4:** *Conduit Burst* (Detonates elem tiles) | **Leyline Absorption:** Stepping on hazardous combat or dungeon tiles (burning ground, lava, spikes) deals `0` damage and restores `5` MP. |
| **28** | **Pillar V (Ecology/Veil)**<br>**Planar Stalker** | `PER` / `DEX` | **R1:** *Dimensional Blink* (3-tile gap blink) \| **R2:** *Phase Step* (2-tile ZOC teleport) \| **R3:** *Rift Ambush* (`+30%` armor pierce) \| **R4:** *Planar Pocket* (1-turn vanish + full TP) | **Ethereal Stride:** The character can phase directly through locked iron portcullises, cell bars, and grates on exploration maps. |

---

## 3. The Adolescent Crucible & Character Lore Revelations

Canonically establishing that **Elijah, Katrina, and the Custom Bearer start at 15–18 years old** reveals the central human tragedy of Sacyra: **the 3,000-year catastrophe was ignited by terrified, unready adolescents**.

### 3.1. The Genesis Triad: Youth as the Historical Catalyst

| Genesis Figure | Age at the 11th Festival Rupture | Class & Vocation Evolution | Psychological & Historical Reality |
| --- | --- | --- | --- |
| **Usuru**<br>*(Becomes The Dark Lord)* | **11 Years Old** | **Uncommissioned Frontier Youth → Tier 5 The Dark One** *(Zero Worldly Jobs)* | **Childhood Binary Absolutism & Vocational Void:** Living by Libira's nomadic *"Three-Day Rule,"* Usuru never stayed anywhere long enough to learn a trade or hold a civic Job. His sole anchor was worshipping the First King. When he stumbled behind the curtain at age 11 and saw the King's corpse wired to alchemical tubes, his child's mind—incapable of grasping statecraft—shattered in binary horror (*Truth vs. Lie*), birthing raw **Tenebris**. With no community trade to ground him, he skipped the mortal tiers and sank straight into **The Dark One**. |
| **Prince Mi-ren**<br>*(First High Regent)* | **~15–18 Years Old** | **Warrior (`W`) + Town Sentry / Court Herald → Paladin (`W+P`) → Prototype of The Hero** | **The Boy in Oversized Armor:** Barely a child when the First King fell comatose at Festival 1 and died at Festival 5, Mi-ren was a teenager terrified that the Six Powers would burn Sacyra if they knew a boy sat on the High Throne. Forced to cross-train into **Priest** prayers to sustain his father's ethereal projection across the Veil, he became the first **Paladin** and discovered **Lux** in the Shield of Peace—fighting the 7-Year Great War not to conquer Libira, but to protect his people and beg Usuru's forgiveness. |
| **Prince Casatti**<br>*(Founder of The Academy)* | **~14–17 Years Old** | **Rogue (`R`) / Mage (`M`) + Academy Archivist / Leyline Surveyor / Border Outrider → Spellblade (`W+M`)** | **The First Adolescent Rebel:** Seeing through court vanity at age 14, Casatti renounced his birthright to found **The Academy** and map the Dreamscape as a frontier ranger. When the 11th Festival ruptured, he analyzed the severance energy mathematically, isolated **Tenebris**, and infused the mundane Sword of War to execute surgical **Absolute Justice** and end the war quickly. |

### 3.2. What the Class & Job System Reveals About the Known Modern Cast

* **Grandma Magolia (`Non-Combatant` | Jobs: Master *Camp Cook* & *Village Hospitaler* | Sway: `+65`):** Mother of vanished Commander Gareth and guardian of **Grandma's Watch** and the **Compass of Morality**. She is the retired domestic steward of a premier military house whose Active Virtue prevented Elijah from succumbing to Usuru's childhood bitterness.
* **Monica the Mystic (`Tier 3 Sage / High Theurge` `M+P+S` | Jobs: Retired *Field Warmage* → *Shrine Hermit* & *Totem Carver* | Sway: `+65`):** Formerly one of the *"original mages for the King"*. After losing her beloved in Sacyra's border wars (memorialized on her wooden *Totem of Fallen Soldiers* with *"Miss you"*), she renounced the Capital, crossed from Academy **Mage** ("Science") into **Priest** (serving Goddess Lyri) and **Summoner** (carving totems), and healed the Science-vs.-Magic schism within herself—serving as the living precedent for the Custom Bearer's **Mystic** path.
* **Eric the Lumberjack (`Base Warrior` `W` | Job: Master *Lumberjack* | Sway: `+10`):** Possesses master forestry tools (`STR`/`VIT`), yet is destitute because royal neglect left the timber roads overrun by monsters. His 10-Gold pot (*Test of Honesty*) and gift of the *Ring of Fire* show a working-class artisan holding onto his integrity when the Crown failed him.
* **The St. Baron vs. John the Thief:** **The St. Baron** (Jobs: *Citadel Bailiff* & *Highway Peddler*, Sway: `-45`) weaponizes feudal law and debt bondage for personal greed, while **John the Thief** (`Base Rogue` `R` | Jobs: *Underground Smuggler* & *Guild Locksmith*, Sway: `[-30, +30]`) operates as Lower Dio's outlaw immune response, using contraband networks to keep starving villagers alive.
* **Commander Gareth Ro-enheart (`Tier 3/4 Paladin / Grand Templar` | Job: *Royal Man-at-Arms* | Sway: `+75`):** Elijah's father had to be dispatched under sealed orders on Elijah's 10th birthday because his unshakeable Active Virtue made him the one military commander who would have exposed King Jihad's treason and rallied the Royal Guard against the throne.
* **King Jihad (`Corrupted Spellblade / Duelist` | Sway: `-85`):** Attempts to claim the Tier 5 mantle of **The Dark One** through political theft rather than earned discipline (*"Soon I will rule over this land, and not even the Dark Lord will have power over me!"*). Lacking the soul to temper the void, cracking the basalt seal beneath the High Throne causes him to be hollowed out and possessed by the Dark Lord's remnant at the 50% midpoint.
* **Senior Outrider / High Priest Sir Roté (`Base Priest` `P` | Jobs: *Border Outrider* & *Court Herald* | Sway: `+55`):** Reconciles his dual demo titles. He has the Outrider's speed to spot the rising island before anyone else, but his childlike reverence for the Crown (*"The king will know how to stop this, he must know..."*) makes him deliver the warning straight to the traitor orchestrating the cover-up.
* **The Extended LeTBS Squad (Sebastien, Angela, Lucius):** While Elijah and Katrina are generalist **Trainees** (`W+M+R`), the three Folder 2 companions represent the pure institutional pillars of Sacyra: **Sebastien** (pure **Rogue** / *Underground Smuggler*), **Angela** (pure **Mage** / *Academy Archivist*), and **Lucius** (pure **Priest** / *Church Liturgist*).

---

## 4. The 3-Act Hero's Journey, Geopolitical Intrigue & The 20-Year War

### 4.1. Pre-Selection Opening Sequence: The Collapse of the Line of Kings

Before the player ever selects a character or touches a class stone in Map 006, the opening sequence (Maps 002–004) stages the complete moral collapse of Sacyra's 3,000-year monarchy:

1. **Inverting the Founding Legacy:** Where the First King and Prince Mi-ren concealed death to preserve peace and shield their people from panic, King Jihad conceals an existential warning (the unnatural storm and rising island before the **Festival of Heroes**) out of despotic ambition.
2. **Establishing Dramatic Irony:** The player witnesses Jihad's treasonous soliloquy *before* waking up in Dio Village. When Elijah expresses reverence for the Royal Army or villagers assume they are merely forgotten, the player already knows the High Throne is compromised from the top down.
3. **Why the Camera Retreats to the Dreamscape:** Because the sitting king is corrupt, salvation cannot come from royal bloodlines or palace decrees. The camera retreats across the Veil into **The Dreamscape** (Map 006), where Chrono searches the collective unconscious for an uncorrupted initiate.

### 4.2. The Festival of Heroes & The "Poisoned Mandate"

When conflict erupts around the **Festival of Heroes** (the modern descendant of the ancient *Festival of Peoples*, when the Veil thins to gossamer), King Jihad publicly commissions the protagonists at the festival to "investigate" the recent border disturbances. This operates as a **Weaponized Smokescreen (The Poisoned Mandate / Unwitting Cat's Paw)**:

* **Public Pretext vs. Covert Reconnaissance:** To the gathered nobility and populace, commissioning festival champions projects benevolent royal vigilance. In reality, Jihad cannot deploy the official Royal Guard without alerting the Church, House Jaren, and the Six Powers. He uses the party as expendable scouts to map the abyssal anomalies and clear guardian monsters for him, with pre-built deniability if they die or discover too much.
* **Detonating the "Kingdom Divided" Questline:** Carrying royal writs across provincial borders violates the spirit of the 3,000-year **"Joint Realm, Separate Kingdoms"** federation. Regional lords in **Old Sacyra**, **The Great Mau Kingdom**, **The Kingdom of Libira**, and **The Yun-yin Provinces** view the party as Capital spies, awakening centuries of *"Quiet Complaints"* while the party uncovers silenced outposts and the truth of Commander Gareth's disappearance.

### 4.3. The Midpoint Throne Room Fork & Fueling the 2nd Great War

At the 50% campaign climax, the party marches back to Sacyra Castle and confronts King Jihad atop the High Throne dais—directly above the ancient basalt seal covering the planar wound ripped open 3,000 years ago. **Regardless of the outcome, the violent clash of relics and psychic shear atop the thinned Festival veil provides the exact metaphysical fuel The Dark Lord needs to blow the ancient seal wide open:**

```text
                     ACT I CLIMAX: THE THRONE ROOM CLASH
                 (Fought atop the Planar Wound during the Festival)
                                      │
         ┌────────────────────────────┴────────────────────────────┐
         ▼                                                         ▼
   OUTCOME A: DEFEAT / SLAY JIHAD                            OUTCOME B: UNCORRUPTING JIHAD
   (The Neutral Path - Power Vacuum)                         (The True Ending Path - Accountability)
   • Jihad is killed or deposed.                             • Possession purged via Dream Compass.
   • Total institutional decapitation.                       • Jihad survives, hollowed & disgraced.
   • House Jaren, Church, & Army fracture in panic.          • Crown forced to confess its 3,000-year lie.
   • Six Powers secede out of opportunism & fear.            • Six Powers secede in righteous disgust.
   • Result: Chaotic, unprincipled civil war.                • Result: Ideological reckoning ("The Ugly Truth").
```

* **Why King Jihad Must Live for the True Ending:** If the party kills Jihad, he becomes a convenient scapegoat—allowing the court and nobility to claim the Crown was merely "bewitched by a demon" while preserving their 3,000-year myth of royal perfection. **Uncorrupting Jihad forces him to survive as a living indictment.** Breathing, disgraced, and stripped of power upon the dais, his survival forces the Sacyran institution to publicly confess its rot and face the continent without its curated lies.

### 4.4. Usuru's Asymmetric Goal & "The Ugly Truth"

* **"Do Not Follow the Crown" (Severance Over Conquest):** Just as in the First Great War, Usuru does not need the Six Powers to worship him or march under dark banners; he only needs them **not to follow the Crown**. **Tenebris is severance**. By exposing the High Citadel's lies, he shatters the federated accord and lets human factionalism ignite the **2nd Great War**.
* **Erasing the First King's Work:** Usuru's win condition is ontological spite: destroying the civilization, treaties, and peace engineered by the First King and Prince Mi-ren. As long as the continent burns itself to ash and nothing remains standing except his risen abyssal island, **Usuru wins**.
* **Rejecting the "Redeemable Villain" Trope in "The Ugly Truth":** In the 10th True Ending (*"The Ugly Truth"*), Usuru does not suddenly turn "good" or receive a sentimental absolution after 3,000 years of malice. Instead, using the **Stance of Equilibrium (`0`)**, the party strips away his core delusion before defeating him: forcing him to come to full terms with the fact that the First King's coma was an accident, Mi-ren's stasis was born of terrified adolescent love, and Usuru's own 3,000-year campaign of ruin made him far worse than the court that hurt him.
* **"Things as They Should Be":** The True Ending is called **"The Ugly Truth"** because reality is not sunshine and rainbows: the First King's marooned soul is finally released from the Dreamscape to die and rest in the earth; King Jihad lives in public disgrace; the centralized imperial myth is dismantled; frontier provinces like Dio Village and Libira receive overdue autonomy; and Usuru is defeated with his eyes open. Both artificial stasis (**Lux**) and nihilistic vengeance (**Tenebris**) are swept away so mortals can govern themselves in honest reality.

### 4.5. The 10-to-20-Year Generational Maturation Timeline

Rather than compressing a continental war into a few weeks, the campaign spans **10 to 20 in-game years**, aging the protagonists from unseasoned teenagers into weathered veterans:

| Campaign Era & Act | Chronological Span & Protagonist Ages | Class & Vocation Progression | Key Narrative & Character Arcs |
| --- | --- | --- | --- |
| **Era I (Act I):**<br>*The Provincial Divide & Poisoned Mandate* | **Years 0–3**<br>*(Ages 15–18)* | **Tier 1 Base / Trainee** → First World Jobs upon leaving Dio Village; **Tier 2 Hybrids** unlock at the 50% Castle Climax. | Awakening in the Dreamscape and Dio Village; taking first Guild/Frontier commissions; accepting King Jihad's poisoned festival investigation; uncovering silenced outposts; confronting Jihad as the planar seal ruptures. |
| **Era II (Act II):**<br>*The Continental Grind (2nd Great War)* | **Years 4–12**<br>*(Ages 19–27)* | **Tier 2 Hybrids → Tier 3 Tri-Domain Masters**; rotating seasonal Jobs (Ranks 1–5) across war fronts and winter quarters. | Navigating a fractured continent across annual **Festivals of Heroes**; **Elijah** reaches and surpasses the age Commander Gareth was when he vanished, demystifying his father as a fellow flawed commander; **Katrina** shifts from rebellious runaway to reluctant matriarch as House Jaren collapses into war refugees; **Dio Village** visibly ages (Grandma Magolia passes on; Eric's mill expands or decays). |
| **Era III (Act III):**<br>*The Sovereign Eclipse & Reckoning* | **Years 13–20**<br>*(Ages 28–35+)* | **Tier 4 Paragons** *(Bystander Ceiling)* → **Tier 5 Cosmic Apex** (*Hero, Dark One, Sage of Ancients*). | Descending into the **Abyssal Vaults** and **Dreamscape Core**; weathered 30-year-old adults who have endured a decade of real compromise and loss confront **Usuru**—an immortal cosmic warlord who remains psychologically frozen as a traumatized, binary 11-year-old child. |

---

## 5. The Open-World Temporal Engine & The "Dream Clock" Philosophy

Inspired by ***Persona*** (daily/nocturnal scheduling and looming calendar milestones) and ***Chrono Trigger*** (multi-year historical cause-and-effect), *Tenebris and Lux* treats time as an active physical dimension powered by Layer 0 `time_utils` and diegetically anchored by **Grandma's Watch → The Dream Compass**.

### 5.1. Narrative Sequence vs. 4D Active Temporal Registration

Because a video game is an active medium, **turning the Dream Clock OFF does not freeze narrative time**—time *must* move forward for events to happen, seasons to change, and characters to grow up across the 10-to-20-year story. Instead, toggling the Dream Clock changes **how the simulation engine registers what transpired at each story point**:

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                    HOW THE ENGINE REGISTERS A STORY POINT                   │
├──────────────────────────────────────┬──────────────────────────────────────┤
│         DREAM CLOCK: OFF             │          DREAM CLOCK: ON             │
│   (Stopwatch / Event-Driven Time)    │  (Countdown / Registered Telemetry)  │
├──────────────────────────────────────┼──────────────────────────────────────┤
│ • Narrative time still advances      │ • Time advances continuously via     │
│   from scene to scene and year-to-yr.│   tunable Layer 0 ticks & travel.    │
│ • Event Registration: 3D Vector      │ • Event Registration: 4D Vector      │
│   [Location, Action, Moral Sway]     │   [Location, Action, Sway, Timestamp]│
│ • The engine knows WHAT you did,     │ • The engine knows WHAT you did AND  │
│   but lacks the temporal receipt of  │   WHEN you intercepted the moving    │
│   WHEN you arrived in the cycle.     │   historical timeline.               │
│ • Resolves the default version of    │ • Dynamically alters what actually   │
│   each scene; unlocks all 9 standard │   happened at that story point;      │
│   endings (3x3 Matrix).              │   REQUIRED to register True Ending!  │
└──────────────────────────────────────┴──────────────────────────────────────┘
```

1. **Why a "Perfect Run" Cannot Register the True Ending with the Dream Clock Off:** A player can play with the Dream Clock off, follow a walkthrough step-by-step, make every virtuous/equilibrium choice, and perform every required action for the True Ending. They will experience a complete, 20-year story and unlock any of the **9 standard endings**. However, because the **Dream Clock** telemetry channel is off, the engine cannot record the 4th coordinate (`Timestamp` / nocturnal window `20:00..04:00` / Festival alignment) on the story event receipt—meaning the state machine mathematically cannot register the **10th True Ending ("The Ugly Truth")**.
2. **Changing What Actually Happened at Story Points:** With the Dream Clock **ON**, world events (such as the seven-day courier ride across the *"Week of Silence"* to Libira, troop movements in the Marches, and the annual **Festival of Heroes** veil-thinning countdown) move on their own schedule. Arriving early, arriving during the nocturnal Dreamscape window (`20:00–04:00`), or arriving late **changes what actually happened at that story point** (e.g., intercepting a courier before an outpost burns vs. arriving to find ashes; or triggering a "Premature Breach" Game Over if an entire Festival cycle expires with critical rifts unsealed).
3. **Scalable Time Tuning ("The Sweet Spot"):** Because Layer 1 story logic evaluates *in-game calendar timestamps* (`Seconds, Minutes, Hours 0–23, Days 1–7, Weeks, Months, Years`) rather than raw real-world wall-clock time, the tick scalar is fully tunable via `engine/config/runtime/time/` & `game/data/` overrides—allowing gentle micro-pacing inside towns like Dio Village while assigning deliberate day/hour costs to overland province travel, Job shifts, and LeTBS grid battles.
4. **Internal Focalization (Elijah/Katrina) vs. Becoming Part of the Story (`<chosen_name>`):** Playing as Elijah or Katrina is inherently harder because they are locked inside the story's factional prejudices and inherited traumas, yielding irreplaceable intimate character lore. Playing as the Custom Bearer (`<chosen_name>`) with the Dream Clock ON allows the player—as Chrono's initiate Mystic—to step out of the audience, become an active participant in Sacyran history, and synthesize both timelines before the final Festival bell tolls.

---

## 6. Layer 0 & Layer 1 ISO C11 Architectural Specification (`ALLOC_ZERO_HEAP`)

To bind the 35-Class pyramid, 28-Job compendium, 20-year generational calendar, and 4D Dream Clock event registration into the native C11/C23 engine without dynamic memory allocation (`ALLOC_ZERO_HEAP`) and within the strict 80-column OmniCode 5-Block discipline:

```c
/*============================================================================*
 * [ SUB-ASSEMBLY 2.2: 4D TEMPORAL REGISTRATION & VOCATION STATE CONTRACTS ]  *
 *============================================================================*/

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Dream Clock operational mode selected in Map 006 (Room of Sages).
 */
typedef enum {
    DREAM_CLOCK_OFF_SEQUENTIAL = 0, /* Advances scene-by-scene; 9 Endings max */
    DREAM_CLOCK_ON_REGISTERED  = 1  /* Active 4D timestamps; 10th True Ending */
} dream_clock_mode_t;

/**
 * @brief Monotonic In-Game Calendar Timestamp (Grandma's Watch -> Dream Compass).
 * Grounded in Layer 0 time_utils; spans the 10-to-20-year campaign horizon.
 */
typedef struct {
    uint8_t  second;        /* 0..59                                          */
    uint8_t  minute;        /* 0..59                                          */
    uint8_t  hour;          /* 0..23 (Nocturnal Veil Window: 20..23, 0..4)    */
    uint8_t  day_of_week;   /* 1..7  (7-Day "Week of Silence" Libira lag)     */
    uint8_t  week_of_month; /* 1..4  (Week 4 of Autumn = Festival of Heroes)  */
    uint8_t  month;         /* 1..12                                          */
    uint16_t campaign_year; /* 1..20 (Protagonists age from 15..18 to 35+)    */
} sacyra_calendar_t;

/**
 * @brief 4D Story Point Registration Receipt.
 * When dream_clock_mode == DREAM_CLOCK_OFF_SEQUENTIAL, has_temporal_receipt
 * is false, preventing True Ending ("The Ugly Truth") state promotion.
 */
typedef struct {
    uint16_t          story_point_id;       /* Canonical Plot / Map Event ID  */
    uint16_t          location_id;          /* Spatial Province / Map ID      */
    int8_t            moral_sway_at_event;  /* Clamped within [-100, +100]    */
    uint8_t           outcome_variant_id;   /* What actually happened (0..N)  */
    bool              has_temporal_receipt; /* True ONLY if Dream Clock is ON */
    bool              in_nocturnal_window;  /* True if registered 20:00-04:00 */
    sacyra_calendar_t registered_at;        /* Exact 4th-dimension timestamp  */
} story_point_receipt_t;

/**
 * @brief Complete Campaign Temporal & Milestone Progression State.
 */
typedef struct {
    dream_clock_mode_t clock_mode;          /* Selected at Character Select   */
    sacyra_calendar_t  current_time;        /* Current Watch / Compass reading*/
    uint32_t           overworld_tick_scale;/* Tunable sweet-spot scalar      */
    uint8_t            festivals_elapsed;   /* Count of Festival deadlines met*/
    bool               midpoint_cleared;    /* 50%: Unlocks Adv Classes/Dyad  */
    bool               jihad_uncorrupted;   /* True Ending Prerequisite       */
    bool               campaign_cleared;    /* 100%: Unlocks Trainee & Mystic */
} campaign_chronicle_state_t;
```

---

## 7. Grand Tour Architecture & Two-Pass Spiral World Design

In game and narrative architecture, sending the player across the entire world map during Act I without letting them stay in any single region long enough to fully understand it is known as a **Grand Tour Architecture** (or **Two-Pass Spiral World Design**).

Within the specific lore and mechanics of *Tenebris and Lux*, this structure serves a deliberate dual purpose: mechanically, it establishes the pre-war baseline of the continent before the 50% midpoint shatters the realm; narratively, it forces the player to unconsciously re-enact Usuru’s childhood "Three-Day Rule".

### 7.1. Formal Classification of the Structure

Across game design, level topology, and literary theory, this specific Act I structure is defined by four interlocking terms:

1. **The "Grand Tour" / Reconnaissance Circuit (Breadth-Before-Depth Topology):**
   * Instead of locking the player into "Region 1" until it is 100% completed before unlocking "Region 2," the game sends the player on a wide, sweeping loop across the entire world map early on.
   * **Pass 1 (Act I) is Horizontal and Shallow:** You touch every major hub, cross the borders of the Six Powers (Central Sacyra, Libira, Great Mau, Old Sacyra, The Academy, and the Yun-yin Provinces), and catch brief glimpses of their cultures and tensions, but the plot keeps your boots moving before you can dig beneath the surface.
   * **Pass 2 (Act II) is Vertical and Deep:** After the midpoint catastrophe, you return to those exact same regions—now transformed by war and time—to actually live in them, excavate their secrets, and resolve their root conflicts.
2. **"Establish and Rupture" (The *World of Balance → World of Ruin* Pattern):**
   * Popularized by classic epics like *Dragon Quest XI* (the initial gathering of orbs across Erdrea before the Fall of Yggdrasil), *Final Fantasy VI* (World of Balance surveying the continent before the World of Ruin), *Tales of Symphonia*, and *Trails in the Sky FC* (regional field studies), the entire first act functions as a living "before picture." You cannot feel the tragedy of a continent tearing itself apart in a 2nd Great War unless you have first walked its roads while the fragile 3,000-year peace was still standing.
3. **Spiral World Design (Open-World Metroidvania Teasing):**
   * By sweeping the player through high-level or complex regions when they are still unseasoned **Tier 1 Trainees** with novice **Jobs**, every town and road shows the player doors they cannot open yet—ancient Academy tablets they lack the *Archivist* rank to read, blocked mines they lack the *Delver* perk to shatter, and political conspiracies they lack the context to decode.
4. **The Picaresque / Itinerant Chronicle & Reconnaissance Loop:**
   * In storytelling, a *picaresque* or itinerant structure keeps young protagonists in constant transit under an urgent schedule (racing the calendar toward the **Festival of Heroes** or carrying out a brisk royal commission), so they experience the world as **transients and tourists** rather than settled masters.

### 7.2. Why *Tenebris and Lux* Sends You Around the World Once in Act I

In *Tenebris and Lux*, doing a brisk, incomplete circuit of the world in Act I is the ultimate fusion of lore and game design:

#### A. Re-Enacting Usuru’s "Three-Day Wanderer" Pilgrimage

Look back at **Scene 06 / Scene 08 ("The Three-Day Wanderer")** in the Genesis Era lore:

* Growing up, the nameless boy who became **Usuru** walked the breadth of the continent following the **Three-Day Law**—he arrived in hamlets, helped villagers for three days, packed his sack on the dawn of the fourth day, and left, until his journey culminated at the Capital gates for the annual **Festival of Peoples**.
* **What happens when you only stay somewhere for three days?** Three days is long enough to hear a farmer's *"quiet complaint,"* see a corrupt bailiff, or notice a pothole in the road—**but it is never long enough to understand the whole truth of a community**.
* By forcing Elijah, Katrina, and the Custom Bearer (ages 15–18) to traverse the whole world in Act I without staying anywhere long enough to understand it in full, **the game puts the player directly into Usuru’s childhood shoes**. You arrive at the **Festival of Heroes** in Sacyra Castle carrying a head full of half-truths, surface impressions, and simmering grievances gathered from a whirlwind tour of the realm.

#### B. Youthful Naivety & The Trap of Surface Impressions

Because the protagonists are 15-to-18-year-old **Trainees** moving quickly from province to province:

* They only see the **facade** of each region. When Elijah passes through a fortified March outpost, he sees proud banners and assumes the Royal Guard is strong; he isn't there long enough to realize the garrison is starving and their dispatches are being burned.
* When Katrina passes through a guild hall or noble fief, she sees surface-level greed or freedom, without yet seeing the 3,000-year generational pressures trapping those people in the **Bystander Corridor (`[-50, +50]`)**.
* Act I deliberately feeds the player **incomplete context** so that when the midpoint hits—and they spend the next 10 to 20 years of Act II and Act III returning to those places as adults—every region delivers a revelation: *"We walked right past this in Year 1 and had no idea what we were actually looking at."*

#### C. Setting the Dominoes of the Six Powers Before They Fall

The 3,000-year peace of Sacyra rests on the **"Joint Realm, Separate Kingdoms"** treaty between the Six Powers: **Central Sacyra, Libira, Great Mau, Old Sacyra, The Academy, and the Yun-yin Provinces**.

* If the party spent all of Act I stuck in Dio Village and one forest before fighting King Jihad at the Castle, the outbreak of the **2nd Great War** would feel abstract—just names of foreign kingdoms on a map screen.
* By doing a full continental circuit in Act I, you physically walk the borders of Great Mau, see the courier gap of Libira, pass the spires of The Academy, and witness the friction in Old Sacyra. You see the dry kindling with your own eyes, so when the throne room battle strikes the match at the 50% mark, you understand *why* the entire world catches fire at once.

#### D. Recruiting the 5-Member Party Across the World's Pillars

In Act I, Elijah and Katrina leave Dio Village as greenhorn Trainees. A brisk circuit across the continent allows them to naturally cross paths with the specialists who form the 5-unit LeTBS tactical squad:

* Picking up **Sebastien the Thief** in a trade hub's back-alleys, **Angela the Mage** near The Academy's leyline jurisdictions, and **Lucius the Priest** along the ecclesiastical parish routes.
* Each companion joins with local knowledge of their home region, giving the party just enough surface guidance to keep moving toward the Festival deadline without halting pacing to solve every regional crisis in Act I.

### 7.3. The Two-Pass Contrast: Act I (The Tour) vs. Act II (The War)

```text
┌────────────────────────────────────────────────────────────────────────────────┐
│                 TWO-PASS SPIRAL WORLD ARCHITECTURE (ACT I vs. ACT II)          │
├───────────────────────────────────────┬────────────────────────────────────────┤
│     PASS 1: ACT I (The Grand Tour)    │    PASS 2: ACT II (The 2nd Great War)  │
│      "The Three-Day Wanderer"         │       "The Generational Trench"        │
├───────────────────────────────────────┼────────────────────────────────────────┤
│ • Pacing: Brisk, transient, forward-  │ • Pacing: Deep, entrenched, multi-year │
│   moving toward the Festival.         │   campaigns across a fractured map.    │
│ • Perspective: 15–18 yr old Trainees  │ • Perspective: 20–35 yr old Veterans   │
│   seeing only the surface facade.     │   uncovering the buried root causes.   │
│ • World State: "Joint Realm" status   │ • World State: Borders closed, treaties│
│   quo; simmering "Quiet Complaints".  │   shattered, planar rifts leaking.     │
│ • Class / Job Depth: Tier 1 Base      │ • Class / Job Depth: Tier 2–4 Combos   │
│   Classes & Rank 1–2 Novice Jobs.     │   & Rank 5 Master Perks unlock secrets.│
│ • Narrative Goal: Gather impressions, │ • Narrative Goal: Reconcile or conquer │
│   reach Sacyra Castle, confront Jihad.│   the Six Powers; face The Ugly Truth. │
└───────────────────────────────────────┴────────────────────────────────────────┘
```

### 7.4. The Ludological & Narrative Synthesis: Cat's Paw Logistics, Mental Mapping & Structural Breadcrumbs

1. **The Narrative "Why" — The Poisoned Mandate:** Traversing the world in Act I is driven directly by King Jihad's smokescreen: commissioning unaligned festival champions as expendable scouts keeps them moving on an itinerary of surface-level firefighting, meeting outsider suspicion across autonomous provinces while unwittingly stabilizing the perimeter until Jihad can execute his High Throne ritual.
2. **Spatial Mental Mapping & Scale:** Sending the party across the continental perimeter forces the player to internalize the physical scale and logistical bottlenecks of Sacyra (such as the seven-day courier crossing of the "Week of Silence" to Libira) before non-linear war campaigns open in Act II.
3. **The "Before" That Validates the "After":** Experiencing the fragile *"Joint Realm, Separate Kingdoms"* equilibrium during Pass 1 makes its fracture into the **2nd Great War** feel earned and devastating.
4. **Structural Breadcrumbs for Pass 2:** Locked fortress gates, impassable ruins, archaic Academy inscriptions, and cold provincial guilds seen in Pass 1 become high-curiosity destinations once Tier 2–4 classes and Rank 5 Job Perks unlock in Pass 2.

```text
┌────────────────────────────────────────────────────────────────────────┐
│                      THE TWO-PASS WORLD LIFECYCLE                      │
├───────────────────────────────────┬────────────────────────────────────┤
│       PASS 1: THE GRAND TOUR      │     PASS 2: THE MULTI-FRONT WAR    │
│      (Act I: Guided Horizon)      │     (Act II: Deep Agency & War)    │
├───────────────────────────────────┼────────────────────────────────────┤
│ • Linear trajectory across borders│ • Non-linear regional interventions│
│ • Surface impressions of cultures │ • Deep domestic & faction quests   │
│ • High-friction royal surveillance│ • Autonomous guild & local alliances│
│ • "Joint Realm" peace appears     │ • The 2nd Great War shatters the   │
│   intact on the surface           │   continent along its fault lines  │
│ • Seeds questions; provides few   │ • Unlocks deep Job mastery and     │
│   answers                         │   hidden planar archives           │
└───────────────────────────────────┴────────────────────────────────────┘
```

By designing Act I as a brisk, full-world circuit where you never stay long enough to see the whole picture, **the world itself becomes a mystery box**. The first half shows *where* everything is and hands you the surface illusion; the second half tears the roof off the world and forces you to deal with what was hiding underneath all along.

---

> **Series Navigation (Markdown Mirror):**
> [Series Index (AsciiDoc)](README.adoc) | [← Part 5: Orthogonal Class & Job Spec](Offline-Work-5.md) | **Part 6: Generational War & Dream Clock** *(End of Series)*

