<!--
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance & Narrative / System Series Anchors )
================================================================================
@project       Tenebris and Lux
@file          docs/offline_work/Offline-Work-4.md
@id            docs.offline_work.part_4.md
@layer         Documentation / Pre-Production Chronicles (Part 4 of 6)
@version       0.2.0-dev
@date          2026-11-12
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@series        Previous: Offline-Work-3.md | Index: README.adoc | Next: Offline-Work-5.md
================================================================================
-->

> **Series Navigation (Markdown Mirror):**
> [Series Index (AsciiDoc)](README.adoc) | [← Part 3: Morality & Bystander Corridor](Offline-Work-3.md) | **Part 4: Master Stat Sheet** | [Part 5: Orthogonal Class & Job Spec →](Offline-Work-5.md)
> *Canonical PDF/AsciiDoc Edition:* [Offline-Work-4.adoc](Offline-Work-4.adoc)

## The Master Stat & Category Sheet (*Tenebris and Lux*)

### 1. Character Identity, Growth & Vocation Affinity

Tracks who the unit is, their macro-progression, and the innate mechanical bonuses granted by their discipline or Dreamscape Monolith attunement.

| Field / Category | Elijah Ro-enheart (Actor 1) | Lady Katrina Jaren (Actor 2) | Custom Bearer (`<chosen_name>`) | Playtest Baseline & Bonuses |
| --- | --- | --- | --- | --- |
| **Level (LV)** | `1` (Tactical) / `5` (Story)

 | `1` (Tactical) / `5` (Story)

 | `1` | Caps at `Lv 99`.

 |
| **Experience (EXP)** | `0` | `0` | `0` | Standard curve to next level. |
| **Primary Class** | **Hero** (Class 5)

 | **Hero** (Class 5)

 | **Hero** (Class 5) | Balanced all-stats growth; unlocks Relic Stances.

 |
| **Secondary / Sub-Class** | **Knight / Warrior** (Class 1)

 | **Outrider / Start** (Class 7) | **Chosen Monolith** | Governs specialized skill tree & passive modifiers.

 |
| **Vocation / Monolith Affinity** | **Vanguard (Aegis):** $+15\%$ DEF, Heavy ZOC, Shield Intercept | **Outrider (Shadow):** $+1$ Move, Ignores ZOC, $+25\%$ Obscured EVA

 | **Aegis / Shadow / Aether / Equilibrium** | Companion classes: **Thief** (Sebastien), **Mage** (Angela), **Priest** (Lucius), **Summoner**.

 |

---

### 2. The Core 9 Combat Stats & Bidirectional 8 Exploration Stats

Paired side-by-side so combat growth and overworld capability mirror each other at Level 1 / Level 5 baselines:

| Core 9 Combat Stat | Lv 1 (Hero / Warrior)

 | Lv 5 (Hero / Warrior)

 | Paired Exploration Stat | Raw Baseline (`1–20` Scale) | Base Modifier (`MOD`) | Combat $\leftrightarrow$ Exploration Function |
| --- | --- | --- | --- | --- | --- | --- |
| **Health Points (HP)** | `450` / `100`

 | `650` / `145`

 | **Vitality (VIT)** | `14` (Elijah)<br>

<br>`10` (Katrina) | `+2` / `+0` | **Combat:** Life pool & physical damage mitigation.

<br>

<br>**World:** Hazard/poison endurance, sprint stamina. |
| **Physical Defense (DEF)** | `16` / `8`

 | `21` / `12`

 | *(Shares **VIT**)* | *(Derived from VIT)* | `+2` / `+0` | **Combat:** Reduces incoming Slash/Bash/Pierce damage. |
| **Magic Points (MP)** | `90` / `30`

 | `130` / `46`

 | **Intelligence (INT)** | `10` (Elijah)<br>

<br>`12` (Katrina) | `+0` / `+1` | **Combat:** Spell pool & arcane potency ("Science").

<br>

<br>**World:** Deciphering archives, rune logic, alchemy. |
| **Magical Attack (MATK)** | `16` / `3`

 | `21` / `5`

 | *(Shares **INT**)* | *(Derived from INT)* | `+0` / `+1` | **Combat:** Scales elemental spells & magic skills. |
| **Tactical Points (TP)** | `0 / 100` | `0 / 100` | **Charisma (CHA)** | `12` (Elijah)<br>

<br>`13` (Katrina) | `+1` / `+1` | **Combat:** Momentum pool for martial arts & auras.<br>

<br>**World:** Persuasion, bartering, squad morale. |
| **Physical Attack (ATK)** | `16` / `12`

 | `21` / `17`

 | **Strength (STR)** | `14` (Elijah)<br>

<br>`11` (Katrina) | `+2` / `+0` | **Combat:** Melee weapon & bash damage.

<br>

<br>**World:** Pushing boulders, carry weight, forced entry. |
| **Magical Defense (MDEF)** | `16` / `8`

 | `21` / `11`

 | **Wisdom (WIS)** | `13` (Elijah)<br>

<br>`11` (Katrina) | `+1` / `+0` | **Combat:** Spiritual warding & status resist ("Magic").

<br>

<br>**World:** Reading NPC motives, Dreamscape intuition.

 |
| **Agility (AGI)** | `32` / `27`

 | `42` / `27`

 | **Dexterity (DEX)** | `11` (Elijah)<br>

<br>`16` (Katrina) | `+0` / `+3` | **Combat:** Turn order speed, evasion, grid mobility.<br>

<br>**World:** Lockpicking, trap disarming, stealth, jumps. |
| **Luck (LUK)** | `32` / `32`

 | `42` / `42`

 | **Perception (PER)** | `12` (Elijah)<br>

<br>`15` (Katrina) | `+1` / `+2` | **Combat:** Accuracy, Crit Rate, loot rolls, grazing hits.<br>

<br>**World:** Spotting hidden chests, dark vision, scouting. |
| *(Relic & Moral Sync)* | *See Sec. 5* | *See Sec. 5* | **Reputation (REP)** | `35` (Elijah)<br>

<br>`20` (Katrina) | `+3` / `+2` | **Combat:** Relic resonance gain & tandem synergy.<br>

<br>**World:** Public fame/infamy & faction reception. |

---

### 3. Combat Resolution, Tactical Grid & Sub-Surface Stats

The secondary and hidden parameters governing hit resolution, positioning on the 2D LeTBS grid, and side-view action duels:

| Stat Name | Elijah Baseline | Katrina Baseline | Custom Bearer | System Scope & Playtest Notes |
| --- | --- | --- | --- | --- |
| **Accuracy (ACC)** | `95%` | `98%` | `95%` | Modified by **PER**, weapon weight, and *Blind* (`-50%`). |
| **Evasion (EVA)** | `5%` | `15%` (`40%` Stealth) | `10%` | Boosted by **DEX/AGI**, thicket cover (`+15%`), or stealth (`+25%`).

 |
| **Critical Hit Rate (CRIT%)** | `5%` | `12%` | `8%` | Flank strikes add `+15%`; Rear strikes or `0` Poise guarantee crits. |
| **Critical Damage (CDMG%)** | `150%` | `175%` | `150%` | Amplified by *STANCE_STRIKING* and Tenebris edge skills.

 |
| **Move Points (`move_points`)** | `4 MP` (`3–4` tiles)

 | `4 MP` (`4–5` tiles)

 | `4 MP` (`3–5` tiles)

 | Adjusted by terrain cost (dunes `-1`) and skills (*Light Steps* `+1`). |
| **Facing & Orientation** | `N / S / E / W` | `N / S / E / W` | `N / S / E / W` | Front (`1.0x` or Phalanx `-40%` dmg), Flank (`+15%` Crit), Rear (Unguardable).

 |
| **Zone of Control (ZOC)** | **Heavy ZOC** (Halts foes) | **Infiltrator** (Ignores ZOC)

 | **Dynamic** (By Stance)

 | Dictates whether adjacent enemies can pass or are pinned.

 |
| **Poise / Flank Vigor** | `90 / 100` | `50 / 100` | `70 / 100` | Posture bar; reaching `0` triggers *Staggered Vulnerability*. |
| **Threat / Aggro Weight** | `150%` (High) | `70%` (Low) | `100%` (Neutral) | Spiked by *Provoke* (*Focused* state); dropped by *Shadow Walk*. |
| **Counter / Deflect Rate** | `15%` (`70%` Aura) | `10%` | `10%` | Frame-perfect *Timed Ward* in action duel grants `100%` block. |

---

### 4. Elemental, Physical & Status Affinity Matrix

Tracks **Weakness (`150%–200%`), Neutral (`100%`), Resistance (`50%`), Immunity (`0%`), or Absorption (`<0%`)**:

| Category | Tracked Elements / Types | Starting Baseline (Playtest Default) |
| --- | --- | --- |
| **The 6 Main Elements** | **Earth**, **Wind**, **Fire**, **Water**, **Light (Lux)**, **Darkness (Tenebris)** | All units start at `100%` (Neutral); Elijah holds innate `75%` Lux resist; Katrina holds `75%` Tenebris resist. |
| **Combined / Secondary Elements** | • **Lightning / Spark** (*Wind + Fire/Light*)<br>

<br>• **Ice / Frost** (*Water + Wind/Earth*)<br>

<br>• **Poison / Venom** (*Water/Earth + Darkness*)<br>

<br>• **Magma / Lava** (*Fire + Earth*)<br>

<br>• **Mist / Haze** (*Water + Wind*)<br>

<br>• **Nature / Brier** (*Earth + Water*)<br>

<br>• **Twilight / Equilibrium** (*Lux + Tenebris*)

 | `100%` (Neutral) baseline; dynamically modified by equipped **Embellishment Catalysts** (*Ring of Fire*, *Medusa's Sword*, *Lyri's Offering*).

 |
| **Physical Damage Types** | **Slash** (Blades), **Bash / Blunt** (Shields/Maces), **Pierce / Projectile** (Bows/Spears) | Heavy plate resists **Slash/Pierce** (`80%`) but is vulnerable to **Bash** (`120%`). |
| **Status Resistances** | **Blind**, **Silence**, **Paralyze**, **Petrify / Calcify**, **Burn**, **Freeze**, **Poison**, **Stagger**, **Charm** | Derived from **Wisdom (WIS)** and **Vitality (VIT)** modifiers (`Base 0%–20%` resist). |

---

### 5. Relic Resonance, Stances & Moral Telemetry

The core bounded invariants tracked by the C11 simulation engine and the *Compass of Morality*:

| Tracker / Gauge | Variable / Range | Starting Value | Mechanical & Narrative Function |
| --- | --- | --- | --- |
| **Shield of Lux Power** | `shield_power` $[0, 100]$

 | `50` | Builds on Guard/Ward (`+10` to `+20`); powers *Lux Barrio* (`25%` mitigation for `2–5` turns). Dissipates on malice/panic.

 |
| **Sword of Tenebris Edge** | `sword_edge` $[0, 100]$

 | `50` | Consumed on severance strikes; powers *Cuttatio Tenebris* (`20%` splash cleave). Erodes user if untempered by Lux.

 |
| **Active Combat Stance** | `combat_stance_t` (`0–3`)

 | `STANCE_NEUTRAL` (`0`)

 | `0`: Neutral | `1`: Warding (`+DEF`) | `2`: Striking (`+ATK/Pen`) | `3`: Equilibrium (`+15% ATK & DEF`, unlocks *Duo-Loca Bashin n Slash*).

 |
| **Relic Embellishment Slots** | `3 Slots per Relic` | `Empty [0, 0, 0]` | Holds elemental/ideological weaves (*Ring of Fire*, *Medusa's Sword*, *Lyri's Offering*).

 |
| **Moral Sway Continuum** | `moral_sway` $[-100, +100]$

 | Elijah: `+35`<br>

<br>Katrina: `-20`<br>

<br>Custom: `0`

 | Net trajectory across the 13 tiers (`-100` *Sovereign of Ruin* $\leftrightarrow$ `[-50, +50]` *Bystander Corridor* $\leftrightarrow$ `+100` *Saint of Mercy*).

 |
| **Cumulative Virtue & Malice** | `virtue_acc` $[0, \infty)$<br>

<br>`malice_acc` $[0, \infty)$ | `0` / `0` | Tracks total selfless vs. ruthless acts separately so the engine distinguishes active *Equilibrium* from passive *Bystander* inertia.

 |

---

### 6. Interpersonal Bonds & Geopolitical Factions

Tracks companion trust and socio-political standing across the Kingdom of Sacyra and the Six-Power regions:

| Category | Specific Tracked Entity | Scale / Range | Starting Baseline | Gameplay Impact |
| --- | --- | --- | --- | --- |
| **Protagonist Dualism** | **Elijah $\leftrightarrow$ Katrina Bond** | $[0, 100]$ | `25` (Cautious) | Unlocks tandem grid maneuvers, follow-up strikes, and *Stance of Equilibrium* sync.

 |
| **Party Companion Affinity** | • **Sebastien** (Thief)<br>

<br>• **Angela** (Mage)<br>

<br>• **Lucius** (Priest)

 | $[-50, +100]$ | `0` (Upon Recruitment) | Governs squad morale, combo assist rates, camp conversations, and loyalty during moral shifts.

 |
| **Mentor & Key NPC Trust** | **Grandma Magolia**, **High Priest Roté**, **Monica the Mystic**, **John the Thief**, **Chrono**

 | $[0, 100]$ | Magolia: `80` (Elijah) / `40` (Katrina)<br>

<br>Others: `10–25` | Unlocks confidential lore, safe-house access, underground fencing, and Dreamscape guidance.

 |
| **The 4 Kingdom Pillars** | 1. **Crown & Royal Guard**<br>

<br>2. **House Jaren & High Nobles**<br>

<br>3. **The High Priesthood**<br>

<br>4. **The Adventurers' Guild**

 | $[-100, +100]$ | Guard: `+40` (E) / `-10` (K)<br>

<br>Nobles: `+10` (E) / `-25` (K)<br>

<br>Guild: `0` (E) / `+30` (K) | Dictates town guard hostility, shop tariffs, royal access, and guild contract availability. |
| **Regional & Six-Power Standing** | **Dio Village (Upper/Lower)**, **Central Sacyra**, **Libira**, **Great Mau**, **Old Sacyra**, **The Academy**, **Yun-yin**

 | $[-100, +100]$ | Dio: `+25`<br>

<br>Other Regions: `0` | Governs border passage, regional trade inventory, and side-quest chains. |

---

### 7. Economy, Diegetic Archives & Temporal World-State

Tracks logistics, compendium completion, and the monotonic clock/veil state:

| Subsystem | Tracked Variables | Starting Baseline | System Role |
| --- | --- | --- | --- |
| **Currency & Logistics** | • **Gold (G)**<br>

<br>• **Contraband / Stolen Goods**<br>

<br>• **Herbalism & Catalyst Stock** | `100 G` (or `0 G` pre-chest)<br>

<br>`0` Contraband | Used for gear, charity tests (Eric's `10 G` pot / `500 G` donation), or fencing with John the Thief.

 |
| **Diegetic Archives** | • **Book of Monsters** (`0–22+` foes)<br>

<br>• **Book of Items** (`%` cataloged)<br>

<br>• **Chronicle Ledger** (Chapters/Endings)

 | `0%` Unlocked (Carries over in NG+)

 | Reveals enemy HP, DEF/MDEF splits, and Poise on the tactical grid; unlocks timeline bookmark jumps in NG+.

 |
| **Monotonic Clock (*Grandma's Watch*)** | `sec` $[0,59]$, `min` $[0,59]$, `hour` $[0,23]$, `day` $[1,7]$, `week` $[1,4]$, `month` $[1,12]$, `year`

 | `08:00 AM, Day 1` (Festival Countdown) | Drives day/night NPC schedules, shop hours, and the nocturnal Dreamscape window (`20:00` to `04:00`).

 |
| **Cosmological Veil (*The Dream Compass*)** | • **Veil Stability** $[0, 100]$<br>

<br>• **Truth Discovery Index** $[0, 100]$ | Stability: `100` (Thins near Festival)<br>

<br>Truth: `0` | Gates nocturnal slumber trials and determines eligibility for the Midpoint Fork (**Neutral Path** vs. **True Ending "The Ugly Truth"**).

 |

---

### 1. Character Growth & Class Affinity Categories

Rather than just raising raw numbers, growth tracks how a character’s martial discipline shapes their capabilities and what innate bonuses their vocation grants.

* **Character Level & Experience (EXP):** The macro-measure of combat seasoning and survival across the campaign.

* **Archetype / Monolith Attunement (The Custom Bearer’s Foundation):** Tracks the primordial discipline chosen in the Dreamscape under Chrono’s observation:

* *Aegis Monolith (Vanguard):* Affinity for heavy barrier interception, physical defense, and holding the frontline.

* *Shadow Monolith (Outrider):* Affinity for high grid mobility, flanking severance, and evasion.

* *Aether Monolith (Mystic):* Affinity for elemental burst, magic potency, and area-of-effect zoning.

* *Equilibrium Monolith (Steward):* Affinity for hybrid physical/magical balance and seamless stance chaining.

* **Vocation / Class Affinity (Job Bonuses & Skill Trees):** Derived from the 7 core combat classes plus auxiliary callings, determining which weapon types, passive traits, and active techniques a unit naturally gravitates toward:

* *Hero (Protagonist Baseline):* All-around leadership, balanced growth, and relic resonance.

* *Warrior / Knight & Guard:* Frontline poise, shield mastery, zone-of-control enforcement, and threat generation (*Provoke*, *Intimidating Aura*, *Heavy Hit*).

* *Thief / Outrider:* Stealth infiltration, contraband retrieval (*Pick Up* / *Push*), lockpicking, and agile flanking.

* *Mage:* Arcane reservoir depth, elemental synthesis, and academic spellcasting.

* *Priest:* Spiritual warding, status cleansing, and restorative preservation.

* *Summoner:* Ethereal pact-making and calling auxiliary entities (such as elemental spirits or tactical decoys) onto the grid.

---

### 2. Moral & Philosophical Telemetry (The Conscience Ledger)

Tracked via the **Compass of Morality**, this category separates *intent* from *elemental expression* so the game knows not just where the player stands, but *how* they arrived there:

* **Moral Sway (Overall Continuum):** The net trajectory across the13 canonical tiers (from *Sovereign of Ruin* through the *Bystander Corridor* to *Saint of Mercy*).

* **Cumulative Virtue (Moral Good):** A permanent ledger of selfless, protective, charitable, and merciful acts (such as gifting charity to Eric the Lumberjack or shielding allies).

* **Cumulative Malice (Moral Evil):** A permanent ledger of ruthless, self-serving, thieving, or tyrannical acts (such as looting Eric’s Self-Aware Pot or executing fleeing foes).

* *Why tracking both separately matters:* It distinguishes a "True Equilibrium" player (who actively balances profound mercy and necessary justice) from a "Passive Bystander" (who simply does nothing) or an "Erratic Opportunist" (who commits atrocities and tries to buy them off with donations).

---

### 3. Relic Resonance & Embellishment Categories

Because Elijah, Katrina, and the Custom Bearer receive the ancestral relics early and consciously choose how to attune them, the relics have their own evolving stat sheet:

* **Relic Charge / Temperament:**
* *Shield of Lux Power:* Tracks radiant cohesion, barrier stability, and warding readiness (which wavers or dissipates if the wielder acts out of malice or panic).

* *Sword of Tenebris Edge:* Tracks umbral sharpness, armor-piercing severance, and kinetic momentum (which risks eroding the wielder if untempered by Lux).

* **Stance Mastery & Bias:** Tracks the player's habitual combat posture across the four stances—**Neutral**, **Warding**, **Striking**, and **Equilibrium**—reflecting whether they lean toward Prince Mi-ren’s preservation, Prince Casatti’s severance, or unified synthesis.

* **Relic Embellishment Slots (Ideological & Elemental Weaves):** Categories tracking what catalysts, regional artifacts, and secondary elemental infusions the player has forged into the Shield and Sword, altering how signature skills (*Lux Barrio*, *Cuttatio Tenebris*, *Duo-Loca Bashin n Slash*) behave in battle.

---

### 4. Interpersonal Relationships & Party Bonds

Tracks the emotional trust, ideological friction, and battlefield synergy between the protagonists, their companions, and key mentors:

* **Protagonist Dualism (Elijah $\leftrightarrow$ Katrina Dynamic):** Measures the evolving partnership between the rule-bound soldier of the Royal Army and the rebellious noblewoman of the Adventurers' Guild—tracking whether they sharpen each other’s conscience or pull each other into cynicism.

* **Companion Affinity & Morale (Extended Party):** Individual trust and alignment meters for party members like **Sebastien (Thief)**, **Angela (Mage)**, and **Lucius (Priest)**. This governs tandem grid attacks, follow-up strikes, camp dialogues, and whether a companion questions or supports your moral decisions.

* **Key NPC & Mentor Standing:** Personal bonds with pivotal figures—such as **Grandma Magolia** (domestic trust/respect), **High Priest Roté** (confiding royal omens), **Monica the Mystic**, **John the Thief** (underground trust), and **Chrono the Sage** (awakening readiness).

---

### 5. Geopolitical & Faction Standing

In the Kingdom of Sacyra, reputation is not a single number; it is split across competing socio-political pillars and historical fault lines:

* **The Four Capital & Kingdom Pillars:**

1. *The Crown & Royal Guard:* Standing with the military hierarchy and royalist defenders (complicated by King Jihad’s covert decrees and Commander Gareth’s disappearance).

2. *House Jaren & The High Nobles:* Standing within aristocratic court intrigue, tax monopolies, and feudal authorities like The St. Baron.

3. *The High Priesthood:* Standing with the church hierarchy and spiritual orthodoxy.

4. *The Adventurers' Guild:* Standing with the commoner-focused brotherhood operating outside courtly vanity.

* **Provincial & Outlaw Standing:** Local trust in communities like **Upper and Lower Dio Village**, including gray-market networks (John the Thief’s contraband ring vs. the St. Baron’s bailiffs).

* **The Six-Power Historical Regions (Macro-World Factions):** Broader diplomatic standing across the confederation born from The Great War—**Central Sacyra, Libira, Great Mau, Old Sacyra, The Academy, and Yun-yin**—affecting regional trade, safe passage, and side-quest availability.

---

### 6. Economy, Logistics & Diegetic Archives

Tracks the tangible resources and accumulated knowledge the party uses to survive and decode the world:

* **Currency & Trade Ledger:**
* *Gold (G):* Standard realm coinage used for provisions, gear, charity, or bribes.

* *Recovered Contraband / Stolen Goods:* Special tactical loot retrieved from battlefields that can be returned to rightful owners, turned over to authorities, or fenced through John the Thief for supplies.

* **Relic Alchemy & Herbalism Stock:** Inventory categories for Sacyran herbs, stance elixirs, and embellishment catalysts (replacing generic RPG potions).

* **Diegetic Compendiums (Knowledge Progression):**
* *Book of Monsters (Bestiary Completion):* Tracks tactical familiarity with Sacyran fauna and Abyssal aberrations (unlocking visibility into enemy weaknesses, DEF/MDF splits, and Flank Vigor).

* *Book of Items & Lore Archives:* Tracks discovered recipes, historical treatises on the Magic vs. Science schism, and fragments of the 3,000-year dynastic history.

---

### 7. Temporal & Cosmological World-State Categories

Driven by *Grandma’s Watch* (evolving into *The Dream Compass*), the game tracks time and metaphysical awareness as living stats:

* **The Monotonic Calendar & Clock:** Tracks **Seconds, Minutes, Hours (24-hour cycle), Days of the Week, Weeks, Months, and Years**—governing NPC schedules, nocturnal events, shop hours, and the countdown toward the annual Festival.

* **Veil Thinning & Dreamscape Synchronization:** Tracks the proximity and stability of the boundary between physical Sacyra and **The Dreamscape**, determining when nocturnal slumber meditations open and how aggressive void incursions (*Dream Eaters*) become.

* **The Dream Compass Calibration (Lore Discovery / "Ugly Truth" Index):** Tracks how deeply the player has investigated the planar wound beneath the High Throne and the ancient tragedy of Usuru, Mi-ren, and Casatti—serving as the key gatekeeper for the Midpoint Campaign Fork between the **Neutral Path** and the **True Ending Path**.

---

> **Series Navigation (Markdown Mirror):**
> [Series Index (AsciiDoc)](README.adoc) | [← Part 3: Morality & Bystander Corridor](Offline-Work-3.md) | **Part 4: Master Stat Sheet** | [Part 5: Orthogonal Class & Job Spec →](Offline-Work-5.md)
