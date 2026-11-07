# Volume IV: The 2017–2018 Demo Baseline & Historical Archive

> **Author**: CreativeWorkzStudio LLC / ProfessorSeanEX  
> **Compendium**: Tenebris and Lux Master Guide — Volume IV  
> **Purpose**: Context Document for Google Gemini & NotebookLM Workflows

## Volume Synopsis

_Verified historical baseline extracted from the original RPG Maker MV datasets, detailing the complete scenario script, LeTBS mechanics, grimoire, and systems._

## Table of Contents

1. Historical Baseline Overview & Provenance
2. 7-Act Narrative Script & Dialogue Walkthrough
3. Complete Cast & Tactical Party Compendium
4. LeTBS Tactical Grid System Specification
5. Classes, Growth Curves & Learning Trees
6. The Complete Grimoire (75+ Skills & Formulas)
7. Bestiary, Destructible Props & Troop Formations
8. Armory, Inventory & Key Lore Relics
9. Tactical States & Status Ailments Directory
10. Engine Systems, Time Clock & Switch/Variable Index
11. Historical Dataset Evolution & Delta Analysis
12. Game Design Curation & Triaging Matrix (2017–2018 Demo vs C11 Engine)

---

## Chapter 1: Historical Baseline Overview & Provenance

> **Source**: `/home/professorseanex/Tenebris and Lux/docs/game_design/demo_baseline/README.adoc`

[abstract]
## Overview

This documentation module establishes the comprehensive, verified baseline of the original **2017–2018 RPG Maker MV demo** for *Tenebris and Lux* ("Darkness and Light"). Extracted directly from the archival data directories (`Old Game Data` and `Old_Game_Data_2/data`), this repository anchors our modern C11 reimplementation to its authentic gameplay mechanics, narrative scripts, character arcs, and tactical systems.

## 1. Historical Provenance & Dataset Strata

Between 2014 and 2018, *Tenebris and Lux* was conceived as an ambitious Tactical Strategy RPG (TRPG) combining top-down exploration, puzzle mechanics, and grid-based turn strategy powered by Lecode's Tactical Battle System (LeTBS).

The historical game data is preserved across two distinct repository strata:

[cols="1,2,4", options="header"]
|===
| Dataset | Directory Anchor | Architectural Role & Characteristics
| **Folder 1: Primary Narrative & Morality Build** | `Old Game Data/` | Contains 37 maps, complete dual-protagonist dialogue branches, the morality engine (Compass of Morality, moral sway continuum, honesty tests), and the core Dio Village quest line (`versionId: 40730002`).
| **Folder 2: Advanced LeTBS Tactical Build** | `Old_Game_Data_2/data/` | Contains 43 maps, extended party members (Sebastien the Thief, Angela the Mage, Lucius the Priest), advanced LeTBS action parameters, full tactical state routines, and character generator assets (`versionId: 2078310`).
|===

## 2. Documentation Taxonomy

This baseline compendium is partitioned into specialized manuals covering every facet of the historical demo:

[cols="2,4", options="header"]
|===
| Document | Focus & Coverage
| **[scenario_and_walkthrough.adoc](scenario_and_walkthrough.adoc)** | Chronological narrative walkthrough, scene-by-scene script, branching dialogue for Elijah vs Katrina, Map 001 through Map 043, puzzle mechanics, and transfer topologies.
| **[cast_and_characters.adoc](cast_and_characters.adoc)** | Comprehensive character dossiers for protagonists (Elijah & Katrina), extended party members (Sebastien, Angela, Lucius), village elders (Grandma Magolia, Monica, Eric), court figures (King Jihad, Roté), and summons.
| **[combat_and_letbs_mechanics.adoc](combat_and_letbs_mechanics.adoc)** | Grid-based battle architecture, movement point costs, directional facing, line-of-sight scopes, AOE shapes (`circle`, `line`, `square`), LeTBS action sequences, and tactical states.
| **[classes_and_growth.adoc](classes_and_growth.adoc)** | 7 combat classes (Warrior/Knight, Mage, Priest, Summoner, Hero, Thief, Guard) plus Summons; complete stat growth matrices (Lv 1–99) and level-by-level skill learnings.
| **[grimoire_and_skills.adoc](grimoire_and_skills.adoc)** | Complete grimoire of all 75+ historical spells, martial techniques, movement abilities, MP/TP costs, damage formulas, and LeTBS sequence tags.
| **[bestiary_and_troops.adoc](bestiary_and_troops.adoc)** | Bestiary of all 22+ enemies and neutral entities (Dream Eaters, Magos, Bandits, Chests, Ice Blocks), stat parameters, AI behavior routines, and troop encounter formations.
| **[armory_and_inventory.adoc](armory_and_inventory.adoc)** | Weapons (Medusa's Sword), Armors (Heart Shield, Ring of Fire), Items (Potions, Magic Water), and Key Lore Relics (Grandma's Watch, Compass of Morality, Book of Monsters, Book of Items).
| **[states_and_statuses.adoc](states_and_statuses.adoc)** | 30 tactical states, LeTBS grid modifiers (Intimidating Aura, Heavy Strike, Light Steps, Burning, Tile Regen), and status ailments.
| **[systems_switches_variables.adoc](systems_switches_variables.adoc)** | Global game engines: Monotonic Time System / Day-Night Cycle, Morality System, and an exhaustive directory of all 100+ switches and variables.
| **[game_design_curation.adoc](game_design_curation.adoc)** | Triaging matrix: RTP defaults (discard/re-architect) vs. prototype sketches (review/evolve) vs. authentic canonical pillars (reconstruct in C11).
| **[dataset_evolution_delta.adoc](dataset_evolution_delta.adoc)** | Side-by-side comparative delta analysis between `Old Game Data` and `Old_Game_Data_2/data`, detailing version discrepancies and modern C11 reconciliation strategies.
|===

## 3. Architectural Extraction Script

The entire documentation suite is fully reproducible from source data using our automated Python extraction pipeline:

[source,bash]
----
python3 docs/tools/extract_demo_data.py
----

This ensures absolute fidelity to the original 2017–2018 records as we rebuild the game in native C11.


---

## Chapter 2: 7-Act Narrative Script & Dialogue Walkthrough

> **Source**: `/home/professorseanex/Tenebris and Lux/docs/game_design/demo_baseline/scenario_and_walkthrough.adoc`

[abstract]
## Overview

This document preserves the complete chronological story script, scene transitions,
puzzle structures, and verbatim dialogues of the 2017–2018 demo. It documents both
the shared story progression and the distinct dual-protagonist narrative branches
between **Elijah Ro-enheart** (`\n[1]`) and **Lady Katrina Jaren** (`\n[2]`).


## Act I: The Castle Prophecy & Dark Ambition

### Start-Up / Walk of Fear

Roté's urgent arrival as dark omens rise.

*Associated Maps*: Map 002

#### Map 002: Start-Up

* **Speaker/Source: `The Walk of Fear `**
[listing]
----
\n<Roté>It has been years since I was needed here.<br>The king...<br>The king must know of these strange events!
----

### The Castle (Outside)

Roté observes the rising island and eerie storm prior to the Festival.

*Associated Maps*: Map 003, Map 008

#### Map 003: The Castle (Outside)

* **Speaker/Source: `Roté Observation`**
[listing]
----
\n<Roté>The storm... The rising island...
All of these events...
All before the Festival of Heroes. 
The king will know how to stop this, he must know...
----

#### Map 008: Battle Tutorial

* **Speaker/Source: `Dream Eaters`**
[listing]
----
\n<[Elijah]>I have to do. If this..thing
beats us....no. I can do this!
\n<[Elijah]>That old guy might be dead
because of that...thing, but
that doesn't matter. I can't die here, not
like this, not by that thing!
The illusion of the dream breaks. Dream Eaters
reveal themselves!
\n<[Elijah]>Wh..wha...wha...what are those
things!
\n<[Katrina]>WHAT EVEN ARE YOU!
\n<Dream Eater>Foolish mortal! We are the
Dream Eaters, servants of our Dark Lord.
We will comsume, we will devour, we will 
feast! Our lord commands us too!
----

* **Speaker/Source: `Dream Eaters`**
[listing]
----
\n<Dream Eater>Foolish mortal. We will
return. Our lord demands it! We will
return!
----

* **Speaker/Source: `Tutorial Complete`**
[listing]
----
\n<???>pant pant pant
I wasn't...I wasn't ready. We cannot talk 
here. You must defeat them...the
The man passes out.
\n<[Elijah]>...
\n<[Katrina]>...
----

* **Speaker/Source: `Tutorial Complete`**
[listing]
----
\n<???>[Elijah] our time here has come to an
end. If you have any business you need to
take care, take care of it. Talk to me when
you are ready.
\n<???>[Katrina] our time here has come to an
end. If you have any business you need to
take care, take care of it. Talk to me when
you are ready.
----

* **Speaker/Source: `Tutorial Complete`**
[listing]
----
Are you ready to leave?
\n<???>It is time. You must be brave, you
must be willing, and most of all...you
must trust your own path. Until we meet
again. AWAKEN!
----

* **Speaker/Source: `Start Final Tutorial`**
[listing]
----
\n<[Elijah]>Wh..wha...what...no... I just
met him... mister....minster!
\n<[Katrina]>Wh..wha...what...come on old man!
You haven't told me anything!
----

### The Castle (Inside) - The Throne Room

Roté warns King Jihad; King Jihad's treasonous conspiracy against the Dark Lord.

*Associated Maps*: Map 004, Map 009

#### Map 004: The Castle (Inside)

* **Speaker/Source: `The King`**
[listing]
----
\n<D.Jihad>The fools...that power...that wicked power! 
Soon I will rule over this land, and not even the Dark Lord
will have power over me!
----

* **Speaker/Source: `The Meeting`**
[listing]
----
\n<Roté>King Jihad, I am here to inform you today... 
It's here...
The darkness... it has returned
\n<Jihad>...Thank you Roté, what would I do without your 
careful watch...
\n<Jihad>Gather up the guard! We need to prepare! 
Darkness will soon return to this land. And don't tell the people of 
this discovery...
\n<Roté>Y...Y...Yes my king...
\n<Jihad>You all are dismissed.
Yes my king.
----

#### Map 009: Secret Map

* **Speaker/Source: `Treasure Glow`**
[listing]
----
All crystals in position
The crystal begins to disappear...
All crystals are not in postition
All crystals are not in postition
All crystals are not in postition
All crystals are not in postition
----

* **Speaker/Source: `Treasure Glow`**
[listing]
----
A shield with a heart-crest was in the chest 
along with 100\G
----

* **Speaker/Source: `Treasure Glow`**
[listing]
----
The chest is empty
----

* **Speaker/Source: `Moving Crystal Blue`**
[listing]
----
The Blue Crystal is in place
----

* **Speaker/Source: `Restart Switch`**
[listing]
----
Restart puzzle?
----

* **Speaker/Source: `Moving Crystal Yellow`**
[listing]
----
The Yellow crystal is in place
----

* **Speaker/Source: `Moving Crystal Green`**
[listing]
----
The Green Crystal is in place
----

* **Speaker/Source: `Moving Crystal Red`**
[listing]
----
The Red Crystal is in place
----

* **Speaker/Source: `Aura`**
[listing]
----
Strange magic blocks off the main crystal. Find a
way to move the rocks.
----

* **Speaker/Source: `Aura`**
[listing]
----
Strange magic blocks off the main crystal. Move 
the outer crystals to remove the rocks. If you make a
mistake, use the red button
----


## Act II: The Dream Realm & The Ancient Archives

### Intro / The Void

The cosmic darkness and threshold.

*Associated Maps*: Map 001, Map 002

#### Map 002: Start-Up

* **Speaker/Source: `The Walk of Fear `**
[listing]
----
\n<Roté>It has been years since I was needed here.<br>The king...<br>The king must know of these strange events!
----

### Character Select & The Room of Sages

Meeting Chrono; selecting archetype crystals; reviewing ancient archives.

*Associated Maps*: Map 006, Map 011

#### Map 006: Character Select

* **Speaker/Source: `Chrono`**
[listing]
----
\n<???>So...a new hero is to be chosen.
I have seen this when the sages spoke of the darkness returning. 
But...who did the sage's say again?
\n<???>A young man. The sages spoke 
of a young man that would help us. Now, if 
I just look in the archives...
\n<???>His name, his name, what was his 
name?
\n<???>Ah yes! His name was [Elijah]. The
sages said that they will decide their 
fate, what will they become I wonder...
Click on the objects in the room to select a class.
When you pick a class, talk to the old man once again.
\n<???>A young woman. The sages 
spoke of a young woman that would help us. 
Now, if I just look in the archives...
\n<???>Her name, her name, what was her 
name?
\n<???>Ah yes! Her name was [Katrina]. The
sages said that they will decide their 
fate, what will they become I wonder...
Click on the objects in the room to select a class.
When you pick a class, talk to the old man once again.
----

* **Speaker/Source: `Chrono`**
[listing]
----
\n<???>[Elijah], did you pick your fate?
\n<???>[Katrina], did you pick your fate?
----

* **Speaker/Source: `Chrono`**
[listing]
----
\n<???>That is perfect! Now, we need to
prepare...[Elijah], follow me.
\n<???>That is perfect! Now, we need to
prepare...[Katrina], follow me.
----

* **Speaker/Source: `Warrior's Stone`**
[listing]
----
The crystal began to glow
The Warrior. Tactical and strong, the warrior is known
to lead troops into victory. With various command
formation and tactics, they can turn the tide of a 
battle!
Do you wish to be a Warrior?
Speak to the old man once more.
Take your time. This is an important choice.
----

* **Speaker/Source: `Warrior's Stone`**
[listing]
----
The crystal stopped glowing.
----

* **Speaker/Source: `Mage's Stone`**
[listing]
----
The crystal began to glow
The Mage. Wise and creative, they rely on the powers
of the magical arts to handle problems and to aid 
others Wielding a staff and wearing cloaks and robes, 
they assist in battle with magic of various elements
and keep the battle flowing.
Do you wish to be a Mage?
Speak to the old man once more.
Take your time. This is an important choice.
----

* **Speaker/Source: `Mage's Stone`**
[listing]
----
The crystal stopped glowing.
----

* **Speaker/Source: `Hero's Stone`**
[listing]
----
The crystal began to glow
The Hero. The one who's fate is to save the world from
darkness. The hero fights with valor and truth, using 
various skills and magic to restore balance to the 
world!
Do you wish to be the Hero?
Speak to the old man once more.
Take your time. This is an important choice.
----

* **Speaker/Source: `Hero's Stone`**
[listing]
----
The crystal stopped glowing.
----

* **Speaker/Source: `Thief's Stone`**
[listing]
----
The crystal began to glow
The Thief. Sly and lurking in the shadows, the thief
takes what they want in order to survive. They are 
great at setting up traps and gathering information 
that everyone can't find on their own.
Do you wish to be a Thief?
Speak to the old man once more.
Take your time. This is an important choice.
----

* **Speaker/Source: `Thief's Stone`**
[listing]
----
The crystal stopped glowing.
----

* **Speaker/Source: `Priest's Stone`**
[listing]
----
The crystal began to glow
The Priest. With the power of the church and holy
intervention, the priest using holy magic to purify
lands, obejcts, and even people. They also use their
magic to heal and protect others.
Do you wish to be a Priest?
Speak to the old man once more.
Take your time. This is an important choice.
----

* **Speaker/Source: `Priest's Stone`**
[listing]
----
The crystal stopped glowing.
----

* **Speaker/Source: `Ancient Book`**
[listing]
----
I can't read what this book says!
This book says [Elijah]. That's... me.
----

* **Speaker/Source: `Ancient Book`**
[listing]
----
I can't read what this book says!
This book says [Katrina]. That's... me.
----

* **Speaker/Source: `Ancient Book`**
[listing]
----
I can't read what this book says!
This book says [Katrina]. That's... me.
----

* **Speaker/Source: `Ancient Book`**
[listing]
----
I can't read what this book says!
This book says [Katrina]. That's... me.
----

* **Speaker/Source: `Ancient Book`**
[listing]
----
I can't read what this book says!
This book says [Elijah]. That's... me.
----

* **Speaker/Source: `Ancient Book`**
[listing]
----
I can't read what this book says!
This book says [Elijah]. That's... me.
----

* **Speaker/Source: `Hero's Stone`**
[listing]
----
The crystal began to glow
The Summoner. A being of many mystery. With the great 
ablilty to summon beings from other worlds, the 
Summoner does his best to maintain a balance between 
the worlds and often knows that world's culture
Do you wish to be the Hero?
Speak to the old man once more.
Take your time. This is an important choice.
----

* **Speaker/Source: `Hero's Stone`**
[listing]
----
The crystal stopped glowing.
----

* **Speaker/Source: `Block`**
[listing]
----
You're not done here.
----

* **Speaker/Source: `Block`**
[listing]
----
You're not done here.
----

* **Speaker/Source: `Block`**
[listing]
----
You're not done here.
----

* **Speaker/Source: `Block`**
[listing]
----
You're not done here.
----

* **Speaker/Source: `Block`**
[listing]
----
You're not done here.
----

* **Speaker/Source: `Ancient Book`**
[listing]
----
I can't read what this book says!
This book says [Elijah]. That's... me.
----

* **Speaker/Source: `Ancient Book`**
[listing]
----
I can't read what this book says!
This book says [Elijah]. That's... me.
----


## Act III: The Trial of Mechanisms & The Dream Eaters

### The Switches Puzzle Chamber

Boulders, pressure plates, jumping ledges with 'Z', and knowledge gates.

*Associated Maps*: Map 007, Map 040

#### Map 007: Switches

* **Speaker/Source: `Special Room Switch`**
[listing]
----
Press the button?
The strange stones to the big room was removed!
----

* **Speaker/Source: `Bonus Gold`**
[listing]
----
100\G were found!
----

* **Speaker/Source: `Restart Switch`**
[listing]
----
Restart puzzle?
----

* **Speaker/Source: `Moving Stone`**
[listing]
----
The boulder is in position. The rocks move.
----

* **Speaker/Source: `Chrono`**
[listing]
----
\n<???>Come, we have much to do.
Move the stone onto the green switch so
we can remove the rocks. The red switches
has their own ablities.
One red switch opens another door, the other will
fix your mistakes.
----

* **Speaker/Source: `Chrono`**
[listing]
----
\n<???>Let us continue. I fear our time 
here is coming to a close.
\n<???>You must move the stone so we can
access the Room of the Great Sage. It
should provide us with knowledge.
----

* **Speaker/Source: `Aura`**
[listing]
----
Move the stone onto the green switch so
we can remove the rocks. The red switches
has their own ablities.
One red switch opens another door, the other will
fix your mistakes.
----

* **Speaker/Source: `Aura`**
[listing]
----
Move the stone onto the green switch so
we can remove the rocks. The red switches
has their own ablities.
One red switch opens another door, the other will
fix your mistakes.
----

* **Speaker/Source: `Aura`**
[listing]
----
Move the stone onto the green switch so
we can remove the rocks. The red switches
has their own ablities.
One red switch opens another door, the other will
fix your mistakes.
----

* **Speaker/Source: `Aura`**
[listing]
----
Move the stone onto the green switch so
we can remove the rocks. The red switches
has their own ablities.
One red switch opens another door, the other will
fix your mistakes.
----

* **Speaker/Source: `Aura`**
[listing]
----
By clicking Z, you can jump across certain ledges.
----

#### Map 040: Switches

* **Speaker/Source: `Special Room Switch`**
[listing]
----
Press the button?
The strange stones to the big room was removed!
----

* **Speaker/Source: `Bonus Gold`**
[listing]
----
100\G were found!
----

* **Speaker/Source: `Restart Switch`**
[listing]
----
Restart puzzle?
----

* **Speaker/Source: `Moving Stone`**
[listing]
----
The boulder is in position. The rocks move.
----

* **Speaker/Source: `Chrono`**
[listing]
----
\n<???>Come [Elijah], we have much to do.
Move the stone onto the green switch so
we can remove the rocks. The red switches
has their own ablities.
One red switch opens another door, the other will
fix your mistakes.
----

* **Speaker/Source: `Chrono`**
[listing]
----
\n<???>Let us continue. I fear our time 
here is coming to a close.
\n<???>You must move the stone so we can
access the Room of the Great Sage. It
should provide us with knowledge.
----

* **Speaker/Source: `Aura`**
[listing]
----
Move the stone onto the green switch so
we can remove the rocks. The red switches
has their own ablities.
One red switch opens another door, the other will
fix your mistakes.
----

* **Speaker/Source: `Aura`**
[listing]
----
Move the stone onto the green switch so
we can remove the rocks. The red switches
has their own ablities.
One red switch opens another door, the other will
fix your mistakes.
----

* **Speaker/Source: `Aura`**
[listing]
----
Move the stone onto the green switch so
we can remove the rocks. The red switches
has their own ablities.
One red switch opens another door, the other will
fix your mistakes.
----

* **Speaker/Source: `Aura`**
[listing]
----
Move the stone onto the green switch so
we can remove the rocks. The red switches
has their own ablities.
One red switch opens another door, the other will
fix your mistakes.
----

* **Speaker/Source: `Aura`**
[listing]
----
By clicking Z, you can jump across certain ledges.
----

### The Secret Crystal Chamber

Elemental alignment (Blue, Yellow, Green, Red) to unlock the Heart Shield.

*Associated Maps*: Map 009, Map 039

#### Map 009: Secret Map

* **Speaker/Source: `Treasure Glow`**
[listing]
----
All crystals in position
The crystal begins to disappear...
All crystals are not in postition
All crystals are not in postition
All crystals are not in postition
All crystals are not in postition
----

* **Speaker/Source: `Treasure Glow`**
[listing]
----
A shield with a heart-crest was in the chest 
along with 100\G
----

* **Speaker/Source: `Treasure Glow`**
[listing]
----
The chest is empty
----

* **Speaker/Source: `Moving Crystal Blue`**
[listing]
----
The Blue Crystal is in place
----

* **Speaker/Source: `Restart Switch`**
[listing]
----
Restart puzzle?
----

* **Speaker/Source: `Moving Crystal Yellow`**
[listing]
----
The Yellow crystal is in place
----

* **Speaker/Source: `Moving Crystal Green`**
[listing]
----
The Green Crystal is in place
----

* **Speaker/Source: `Moving Crystal Red`**
[listing]
----
The Red Crystal is in place
----

* **Speaker/Source: `Aura`**
[listing]
----
Strange magic blocks off the main crystal. Find a
way to move the rocks.
----

* **Speaker/Source: `Aura`**
[listing]
----
Strange magic blocks off the main crystal. Move 
the outer crystals to remove the rocks. If you make a
mistake, use the red button
----

#### Map 039: Secret Map

* **Speaker/Source: `Treasure Glow`**
[listing]
----
All crystals in position
The crystal begins to disappear...
All crystals are not in postition
All crystals are not in postition
All crystals are not in postition
All crystals are not in postition
----

* **Speaker/Source: `Treasure Glow`**
[listing]
----
A shield with a heart-crest was in the chest 
along with 100\G
----

* **Speaker/Source: `Treasure Glow`**
[listing]
----
The chest is empty
----

* **Speaker/Source: `Moving Crystal Blue`**
[listing]
----
The Blue Crystal is in place
----

* **Speaker/Source: `Restart Switch`**
[listing]
----
Restart puzzle?
----

* **Speaker/Source: `Moving Crystal Yellow`**
[listing]
----
The Yellow crystal is in place
----

* **Speaker/Source: `Moving Crystal Green`**
[listing]
----
The Green Crystal is in place
----

* **Speaker/Source: `Moving Crystal Red`**
[listing]
----
The Red Crystal is in place
----

* **Speaker/Source: `Aura`**
[listing]
----
Strange magic blocks off the main crystal. Find a
way to move the rocks.
----

* **Speaker/Source: `Aura`**
[listing]
----
Strange magic blocks off the main crystal. Move 
the outer crystals to remove the rocks. If you make a
mistake, use the red button
----

### The Battle Tutorial & Dream Eater Ambush

Dream illusion shatters; Dream Eaters assault; Chrono passes out; 'AWAKEN!'

*Associated Maps*: Map 008, Map 041, Map 012, Map 042

#### Map 008: Battle Tutorial

* **Speaker/Source: `Dream Eaters`**
[listing]
----
\n<[Elijah]>I have to do. If this..thing
beats us....no. I can do this!
\n<[Elijah]>That old guy might be dead
because of that...thing, but
that doesn't matter. I can't die here, not
like this, not by that thing!
The illusion of the dream breaks. Dream Eaters
reveal themselves!
\n<[Elijah]>Wh..wha...wha...what are those
things!
\n<[Katrina]>WHAT EVEN ARE YOU!
\n<Dream Eater>Foolish mortal! We are the
Dream Eaters, servants of our Dark Lord.
We will comsume, we will devour, we will 
feast! Our lord commands us too!
----

* **Speaker/Source: `Dream Eaters`**
[listing]
----
\n<Dream Eater>Foolish mortal. We will
return. Our lord demands it! We will
return!
----

* **Speaker/Source: `Tutorial Complete`**
[listing]
----
\n<???>pant pant pant
I wasn't...I wasn't ready. We cannot talk 
here. You must defeat them...the
The man passes out.
\n<[Elijah]>...
\n<[Katrina]>...
----

* **Speaker/Source: `Tutorial Complete`**
[listing]
----
\n<???>[Elijah] our time here has come to an
end. If you have any business you need to
take care, take care of it. Talk to me when
you are ready.
\n<???>[Katrina] our time here has come to an
end. If you have any business you need to
take care, take care of it. Talk to me when
you are ready.
----

* **Speaker/Source: `Tutorial Complete`**
[listing]
----
Are you ready to leave?
\n<???>It is time. You must be brave, you
must be willing, and most of all...you
must trust your own path. Until we meet
again. AWAKEN!
----

* **Speaker/Source: `Start Final Tutorial`**
[listing]
----
\n<[Elijah]>Wh..wha...what...no... I just
met him... mister....minster!
\n<[Katrina]>Wh..wha...what...come on old man!
You haven't told me anything!
----

#### Map 041: Battle Tutorial

* **Speaker/Source: `Dream Eaters`**
[listing]
----
\n<[Elijah]>I have to do. If this..thing
beats us....no. I can do this!
\n<[Elijah]>That old guy might be dead
because of that...thing, but
that doesn't matter. I can't die here, not
like this, not by that thing!
The illusion of the dream breaks. Dream Eaters
reveal themselves!
\n<[Elijah]>Wh..wha...wha...what are those
things!
\n<[Katrina]>WHAT EVEN ARE YOU!
\n<Dream Eater>Foolish mortal! We are the
Dream Eaters, servants of our Dark Lord.
We will comsume, we will devour, we will 
feast! Our lord commands us too!
----

* **Speaker/Source: `Dream Eaters`**
[listing]
----
\n<Dream Eater>Foolish mortal. We will
return. Our lord demands it! We will
return!
----

* **Speaker/Source: `Tutorial Complete`**
[listing]
----
\n<???>pant pant pant
I wasn't...I wasn't ready. We cannot talk 
here. You must defeat them...the
The man passes out.
\n<[Elijah]>...
\n<[Katrina]>...
----

* **Speaker/Source: `Tutorial Complete`**
[listing]
----
\n<???>[Elijah] our time here has come to an
end. If you have any business you need to
take care, take care of it. Talk to me when
you are ready.
\n<???>[Katrina] our time here has come to an
end. If you have any business you need to
take care, take care of it. Talk to me when
you are ready.
----

* **Speaker/Source: `Tutorial Complete`**
[listing]
----
Are you ready to leave?
\n<???>It is time. You must be brave, you
must be willing, and most of all...you
must trust your own path. Until we meet
again. AWAKEN!
----

* **Speaker/Source: `Start Final Tutorial`**
[listing]
----
\n<[Elijah]>Wh..wha...what...no... I just
met him... mister....minster!
\n<[Katrina]>Wh..wha...what...come on old man!
You haven't told me anything!
----


## Act IV: Awakening in Dio Village & Family Roots

### Grandma's House Upstairs

Awakening from the dream; opening Grandma's chest for Grandma's Watch & Compass of Morality.

*Associated Maps*: Map 020, Map 004

#### Map 020: Grandma's House Upstairs

* **Speaker/Source: `Grandma's Chest`**
[listing]
----
\n<[Elijah]>I shouldn't go in this chest. I 
need to talk to Grandma first.
She's very particular about her things.
\n<[Katrina]>I shouldn't go in this chest. I 
need to talk to Grandma first.
Batty old woman seems to remind that chest.
----

* **Speaker/Source: `Grandma's Chest`**
[listing]
----
An old watch was in the chest.
\n<[Elijah]>Oh.. it's an old watch.
What's so special about it anyway?
\n<[Katrina]>She made me....grab...an 
OLD WATCH! I thought it was
something important like her will!
----

* **Speaker/Source: `Grandma's Chest`**
[listing]
----
There is still something inside of the chest
OBTAINED THE COMPASS OF MORALITY
----

* **Speaker/Source: `Grandma's Chest`**
[listing]
----
It's all empty now.
----

* **Speaker/Source: `Wierd Dream`**
[listing]
----
\n<[Elijah]>That dream....what did it mean...
It could just be my imagination but...It 
felt like...something more. A hero
... Me...?
\n<[Katrina]>WHAT! WHAT THE...ME. Man, my mind
needs to get out of lala land and settle 
into reality. I can't be a hero, 
I won't be a hero...
----

#### Map 004: The Castle (Inside)

* **Speaker/Source: `The King`**
[listing]
----
\n<D.Jihad>The fools...that power...that wicked power! 
Soon I will rule over this land, and not even the Dark Lord
will have power over me!
----

* **Speaker/Source: `The Meeting`**
[listing]
----
\n<Roté>King Jihad, I am here to inform you today... 
It's here...
The darkness... it has returned
\n<Jihad>...Thank you Roté, what would I do without your 
careful watch...
\n<Jihad>Gather up the guard! We need to prepare! 
Darkness will soon return to this land. And don't tell the people of 
this discovery...
\n<Roté>Y...Y...Yes my king...
\n<Jihad>You all are dismissed.
Yes my king.
----

### Grandma's House Downstairs

Conversation with Grandma Magolia; memories of Commander Gareth Ro-enheart; errand to Monica.

*Associated Maps*: Map 021, Map 007

#### Map 021: Grandma's House Downstairs

* **Speaker/Source: `Grandma`**
[listing]
----
\n<Grandma>Hello sweetie, could you please
go upstairs and search in the chest
and grab what's inside? It's something that
has great signifinance.
\n<[Katrina]>The will. I love you gran,
but a girl gotta get out of this dead-end
town.
\n<[Elijah]>Maybe is is something
about dad! I haven't seen him since my 
10th birthday.
\n<[Elijah]>Sure thing. This might
be my chance to get answers!
\n<[Katrina]>Why not! Crazy old woman...
That will is mine!
\n<[Elijah]>Well...uhh...but... 
But if it is about him...
\n<[Katrina]>Gran, why can't you do it? 
Why doesn't she do it?
If she wants it she can get it.
----

* **Speaker/Source: `Grandma`**
[listing]
----
It's been a while since we've talk. 
What do you want to talk about?
\n<Grandma>Oh me and Monica go way back.
She was such a wild child, always getting
into mischief. You'll love her.
\n<Grandma>This town used to be full of
life. sighs The kingdom doesn't
pay much attention to this place anymore...
\c[5]Not avalible in the demo
\n<Grandma>Have you been upstairs yet?
The chest should still be there.
----

* **Speaker/Source: `Grandma`**
[listing]
----
\n<Grandma>Thank you honey. This watch has 
been in the family for a long time. Oh, 
how it brings back memories of better days.
\n<Grandma>I think you should keep it. It's
my gift to you. And here's a bit of money.
I know that you'll need it more than little
ol' me.
----

* **Speaker/Source: `Grandma`**
[listing]
----
\n<Grandma>[Katrina] could you do me a favor?
Lately, the people have been stirred up
about people being attacked by monsters.
\n<Grandma>[Elijah] could you do me a favor?
Lately, the people have been stirred up
about people being attacked by monsters.
\n<Grandma>My friend, Monica, 
would know what is causing these strange 
rumors. Can you go to her house and see 
what she has to say about all of this?
\n<[Elijah]>Sure thing. It will be nice to
finally leave the house. I might even
get to leave town!
\n<[Katrina]>Why not! I would love to~ 
I'm out of this poor slum of a town!
\n<[Elijah]>Well..are you sure you can't?
I don't even know her...
\n<[Katrina]>Gran, I'm not your errand girl! 
 This is YOUR friend.
----

* **Speaker/Source: `Block`**
[listing]
----
I should say something to Grandma before I go.
----

* **Speaker/Source: `Block`**
[listing]
----
Grandma still seems worried. Maybe I can talk to
her one more time.
----

* **Speaker/Source: `Block`**
[listing]
----
I should say something to Grandma before I go.
----

* **Speaker/Source: `Block`**
[listing]
----
Grandma still seems worried. Maybe I can talk to
her one more time.
----

* **Speaker/Source: `Block`**
[listing]
----
I should say something to Grandma before I go.
----

* **Speaker/Source: `Block`**
[listing]
----
Grandma still seems worried. Maybe I can talk to
her one more time.
----

#### Map 007: Switches

* **Speaker/Source: `Special Room Switch`**
[listing]
----
Press the button?
The strange stones to the big room was removed!
----

* **Speaker/Source: `Bonus Gold`**
[listing]
----
100\G were found!
----

* **Speaker/Source: `Restart Switch`**
[listing]
----
Restart puzzle?
----

* **Speaker/Source: `Moving Stone`**
[listing]
----
The boulder is in position. The rocks move.
----

* **Speaker/Source: `Chrono`**
[listing]
----
\n<???>Come, we have much to do.
Move the stone onto the green switch so
we can remove the rocks. The red switches
has their own ablities.
One red switch opens another door, the other will
fix your mistakes.
----

* **Speaker/Source: `Chrono`**
[listing]
----
\n<???>Let us continue. I fear our time 
here is coming to a close.
\n<???>You must move the stone so we can
access the Room of the Great Sage. It
should provide us with knowledge.
----

* **Speaker/Source: `Aura`**
[listing]
----
Move the stone onto the green switch so
we can remove the rocks. The red switches
has their own ablities.
One red switch opens another door, the other will
fix your mistakes.
----

* **Speaker/Source: `Aura`**
[listing]
----
Move the stone onto the green switch so
we can remove the rocks. The red switches
has their own ablities.
One red switch opens another door, the other will
fix your mistakes.
----

* **Speaker/Source: `Aura`**
[listing]
----
Move the stone onto the green switch so
we can remove the rocks. The red switches
has their own ablities.
One red switch opens another door, the other will
fix your mistakes.
----

* **Speaker/Source: `Aura`**
[listing]
----
Move the stone onto the green switch so
we can remove the rocks. The red switches
has their own ablities.
One red switch opens another door, the other will
fix your mistakes.
----

* **Speaker/Source: `Aura`**
[listing]
----
By clicking Z, you can jump across certain ledges.
----


## Act V: Dio Village Upper & The Outskirts Crisis

### Dio Village Upper / Main Square

The town in disrepair; Fighter Kid blocking the lower gate; visiting Monica.

*Associated Maps*: Map 022, Map 023, Map 010, Map 012, Map 013

#### Map 023: Main Square

* **Speaker/Source: `Fighter Kid`**
[listing]
----
... He's just ignoring you
----

* **Speaker/Source: `Fighter Kid`**
[listing]
----
You want me to move? I'm blocking the way for others?
Big woop! No one can stop me so I'm staying!
----

* **Speaker/Source: `Fighter Kid`**
[listing]
----
You're no fun! I'm going home!
----

* **Speaker/Source: `Mystic's Home`**
[listing]
----
The door is locked.
----

* **Speaker/Source: `Leave Town`**
[listing]
----
I should see if Monica needs anything else.
----

* **Speaker/Source: `Lower Dio`**
[listing]
----
I have to stop that kid first!
----

* **Speaker/Source: `Lower Dio`**
[listing]
----
I still have to talk to Monica first.
----

* **Speaker/Source: `Lower Dio`**
[listing]
----
I have to stop that kid first!
----

* **Speaker/Source: `Lower Dio`**
[listing]
----
I still have to talk to Monica first.
----

* **Speaker/Source: `Lower Dio`**
[listing]
----
I have to stop that kid first!
----

* **Speaker/Source: `Lower Dio`**
[listing]
----
I still have to talk to Monica first.
----

* **Speaker/Source: `Lower Dio`**
[listing]
----
I have to stop that kid first!
----

* **Speaker/Source: `Lower Dio`**
[listing]
----
I still have to talk to Monica first.
----

* **Speaker/Source: `Lower Dio`**
[listing]
----
I have to stop that kid first!
----

* **Speaker/Source: `Lower Dio`**
[listing]
----
I still have to talk to Monica first.
----

* **Speaker/Source: `Lower Dio`**
[listing]
----
I have to stop that kid first!
----

* **Speaker/Source: `Lower Dio`**
[listing]
----
I still have to talk to Monica first.
----

* **Speaker/Source: `Monica!`**
[listing]
----
\n<[Elijah]>This is great! The town is still
sad with the storm but it's my home. I 
need to stay focus and find Monica's house.
Grandma said her house is in front of ours.
\n<[Katrina]>Ugghh. I can't believe we still
live in this dump. Doesn't help that storm
destoryed everything. Now Gran said Monica
live in the house in front of us. Peachy...
----

* **Speaker/Source: `Leave Town`**
[listing]
----
I should see if Monica needs anything else.
----

* **Speaker/Source: `Leave Town`**
[listing]
----
I have to stop that kid first!
----

* **Speaker/Source: `Block`**
[listing]
----
STAY AWAY FROM ME!
----

* **Speaker/Source: `Block`**
[listing]
----
STAY AWAY FROM ME!
----

* **Speaker/Source: `Block`**
[listing]
----
STAY AWAY FROM ME!
----

### Monica's House & The Mystic's Brew

Meeting Monica; the fallen soldiers totem; offering for Goddess Lyri; errand for a Potion.

*Associated Maps*: Map 027, Map 029

#### Map 027: Monica's House

* **Speaker/Source: `Monica`**
[listing]
----
\n<Monica>And who in the blaze are YOU!
\n<Monica>...
...
...
\n<Monica>So you are Magolia's grandson.
She made you sound more handsome that
you are.
\n<[Elijah]>HEY! That's not a nice thing
to say!
\n<Monica>Tis only a joke. Now why have 
you came here? Do you have some milk?
\n<[Elijah]>Nn-...I'm here because my grandma
thinks that evil has returned.
\n<Monica>A wise woman she is. Lyri has
been whispering in me ear about the forces
of darkness. I'll help but first, get that
holigan from the city limits.
\n<Monica>Not only is he preventing folks
from leaving, he's also messing with me
business. Can you handle that?
\n<Monica>Good. I'll see you when you
return.
\n<Monica>I'll explain it once more.
\n<Monica>I'll help you if you get that 
holigan from the city limits.
\n<Monica>Not only is he preventing folks
from leaving, he's also messing with me
business. Can you handle that?
\n<Monica>So you are Magolia's 
granddaughter. She made you sound prettier 
that you are.
\n<[Katrina]>LISTEN HER YOU OLD-!
\n<Monica>Tis only a joke. Now why have 
you came here? Do you have some milk?
\n<[Katrina]>What is wrong with you... I'm
just here because my grandma is crazy.
She's babbling about evil forces and all
that nonsense.
\n<Monica>A wise woman she is. Lyri has
been whispering in me ear about the forces
of darkness. I'll help but first, get that
holigan from the city limits.
\n<Monica>Not only is he preventing folks
from leaving, he's also messing with me
business. Can you handle that?
\n<Monica>Good. I'll see you when you
return.
\n<Monica>I'll explain it once more.
\n<Monica>I'll help you if you get that 
holigan from the city limits.
\n<Monica>Not only is he preventing folks
from leaving, he's also messing with me
business. Can you handle that?
----

* **Speaker/Source: `Monica`**
[listing]
----
\n<Monica>You have to stop that kid! Our 
village will be more devastated than
even with him blocking the road!
----

* **Speaker/Source: `Monica`**
[listing]
----
\n<Monica>Good job! I think I have an idea.
Now that the people can, residences and
newcomers have arrived. I notice that a 
shopkeeper has came into our town as well.
\n<Monica>I need you to go a buy a potion.
I will use this as a base for my offering
to Lyri. She'll be able to help us in this
dark hour.
Obtained the Book of Monsters
----

* **Speaker/Source: `Monica`**
[listing]
----
\n<Monica>Oh yes. This is exactly what I
need for my offering.
\n<Monica>There is one more thing that I
need in order to make this brew suitable
for Lyri. Go out of the town to the west 
path. A man has what I need. Get it please.
\n<Monica>If you want to get the potion,
you'll need to go to the lower parts of the
town.
----

* **Speaker/Source: `Monica`**
[listing]
----
\n<Monica>Do you need something?
\n<Monica>Me? Why, I'm one of the original
mages for the King. The kingdom was not as
peaceful as is it now. But then, the 
Dark Lord tried to take the kingdom.
\n<Monica>He was born from the darkness
that this world had. He planned to take 
over this world and fill it with darkness.
The Hero saved this world and brought us
\n<Monica>our peace. The shield and sword.
Oh don't mind this old hag's ranmbling.
Not avalible in demo
Not avalible in demo
----

* **Speaker/Source: `The Secret Room`**
[listing]
----
Get your snooping nose away from me room!
This room is currently unavalible for the demo
----

* **Speaker/Source: `The Secret Room`**
[listing]
----
Get your snooping nose away from me room!
This room is currently unavalible for the demo
----

* **Speaker/Source: `The Secret Room`**
[listing]
----
Get your snooping nose away from me room!
This room is currently unavalible for the demo
----

* **Speaker/Source: `Lyri`**
[listing]
----
It is the deity Lyri, the Goddess of Peace.
----

* **Speaker/Source: `Totem`**
[listing]
----
The totem has the names of fallen soldiers. One
name has a heart by it and it reads "Miss you".
\n<[Elijah]>Did Monica have a husband?
\n<[Katrina]>Someone married that crazy bat!
----

* **Speaker/Source: `Totem`**
[listing]
----
The totem is full of offerings.
----

### The Secret Sword Cache

Discovering Medusa's Sword in the hidden glade.

*Associated Maps*: Map 024, Map 025, Map 017

#### Map 024: The Sword Map

* **Speaker/Source: `Medusa's Sword`**
[listing]
----
A strange sword was in the chest.
\n<[Elijah]>I wonder who this sword 
belongs to.
\n<[Katrina]>Free sword for your girl~
----

#### Map 025: The Sword Map

* **Speaker/Source: `Medusa's Sword`**
[listing]
----
A strange sword was in the chest.
\n<[Elijah]>I wonder who this sword 
belongs to.
\n<[Katrina]>Free sword for your girl~
----


## Act VI: Moral Crossroads of Dio Lower Ward

### Eric the Lumberjack's House

Self-Aware Pot; Test of Honesty: 10G life savings vs 500G rent donation; Ring of Fire reward.

*Associated Maps*: Map 028

#### Map 028: Eric's House

* **Speaker/Source: `Eric`**
[listing]
----
We've been seeing a lot of monster appear near the 
recently. I wonder what's that all about. And with
jobs being tight, money is hard to come by.
----

* **Speaker/Source: `Eric`**
[listing]
----
\n<[Elijah]>I think that that should be 
enough to help you.
Thank you. Thank you so much, here I believe that 
this will be more useful to you than me.
\n<[Elijah]>Thank you sir, but I...
\n<Eric>Don't mention it, and by the way, the name
is Eric. I used to be a lumberjack until that weird
portal appeared. I found this ring and held on to it
every since.
\n<n[1]>Thank you. Thank you so much!
\n<[Katrina]>I think that that should be 
enough to help you.
Thank you. Thank you so much, here I believe that 
this will be more useful to you than me.
\n<[Katrina]>Woah! This is a cool ring dude!
\n<Eric>The name is Eric and that ring has a story.
I used to be a lumberjack until that weird
portal appeared. I found this ring and held on to it
every since.
\n<n[2]>Thanks dude.
----

* **Speaker/Source: `Eric`**
[listing]
----
\n<Eric>I'm sure that ring's fire power will come in 
handy if you plan on leaving the village
----

* **Speaker/Source: `Self-Aware Pot`**
[listing]
----
What kind of game do you think this is! You can't
just steal whatever you want! How rude!
----

* **Speaker/Source: `Test of Honesty`**
[listing]
----
Something is glowing in the pot. Take the pots content?
You recieve 10G
THAT'S ALL MY MONEY! GET OUT NOW!
\n<\n1>I am so sorry! Please, I am not 
trying to...
\n<n[2]>AH!
\n<n[2]>Hey, where did you even...
\n<n[2]>AH!
\n<[Elijah]>..steal all your money.
\n<n[1]>I'm sorry...
\n<n[2]>...come from...
\n<n[2]>It was just a few coins anyway.
That guy definately overreacted
You decide to leave the pot alone.
\n<n[1]>I can be quick, discret maybe
Elijah slowly reaches into the jar, stopping when
he saw how little money he had.
\n<n[1]>Maybe I should't...
\n<n[1]>I would be wrong of me to take this
money. But why does he only have a little 
money...
Hey want are you doing!
\n<n[1]>I am so sorry, I wasn't trying to
take your money, I was just looking at the 
pot.
It's fine kid... I just have stumble one hard
time every since the monster began showing up. 
It makes working hard and I can't afford to 
live here anymore.
\n<n[1]>Maybe... Maybe I can help!
Don't worry about it, it's not for you to worry about.
Just make sure you become something.
\n<n[1]>Is this what the voice meant,
to be a hero...How much do you need.
Just 500G, that will be enough for me to make it for 
the month.
\n<n[1]>I'll leave the money in the pot, 
that way you'll have it.
Tha-tha-thank you!
\n<n[1]>I would be wrong of me to take this
money. But why does he only have a little 
money...
Hey want are you doing!
\n<n[1]>I am so sorry, I wasn't trying to
take your money, I was just looking at the 
pot.
It's fine kid... I just have stumble one hard
time every since the monster began showing up. 
It makes working hard and I can't afford to 
live here anymore.
\n<n[1]>Maybe... Maybe I can help!
Don't worry about it, it's not for you to worry about.
Just make sure you become something
\n<n[1]>I have to help him somehow...
How much do you need.
Just 500G, that will be enough for me to make it for 
the month
\n<n[1]>I'll leave the money in the pot, 
that way you'll have it
Tha-tha-thank you!
\n<n[2]>The money is right there! This 
guy won't see it coming.
As Katrina was about to take the money, she stopped
for just a moment
\n<n[2]>Ugghhh, I can't do it.
\n<n[2]>Come on [Katrina], its just a few bucks,
you can give him some more when you get 
more.
Hey want are you doing!
\n<n[2]>Look man, I was just looking at
the stupid thing and saw some pocket change
in the pot. I was going to give you 
something back...eventually.
That would've been all of my money, my life savings!
That's my money for the whole week!
\n<n[2]>Hold on, you mean to tell me that
you only have 10G to your name. Dude, how
can you afford to live here!
Don't worry about that, the monster attacks have made
living in town alot cheaper...even without my job...
\n<n[2]>Is this what the voice meant,
to be a hero...How much do rent here
cost...
500G, but I won't be able to make that kind of money
in time to pay the rent
\n<n[2]>sigh I'll get the 
money, just don't do...anything
Really? Thanks...who are you?
\n<n[2]>The girl next door...actually no,
just someone who understands hard times
\n<n[2]>Come on [Katrina], its just a few bucks,
you can give him some more when you get 
more.
Hey want are you doing!
\n<n[2]>Look man, I was just looking at
the stupid thing and saw some pocket change
in the pot. I was going to give you 
something back...eventually.
That would've been all of my money, my life savings!
That's my money for the whole week!
\n<n[2]>Hold on, you mean to tell me that
you only have 10G to your name. Dude, how
can you afford to live here!
Don't worry about that, the monster attacks have made
living in town alot cheaper...even without my job...
\n<n[2]>I have to help him somehow...
How much do rent here cost...
500G, but I won't be able to make that kind of money
in time to pay the rent
\n<n[2]>sigh I'll get the 
money, just don't do...anything
Really? Thanks...who are you?
\n<n[2]>The girl next door...actually no,
just someone who understands hard times
----

* **Speaker/Source: `Test of Honesty`**
[listing]
----
Would you like to add to the pot?
You have given \v[14] to the man.
How much will you add?
You have given \v[14] to the man.
You have filled the pot! Talk to the man once more.
The pot isn't full yet.
You don't have that much to give
You leave the pot alone.
----

* **Speaker/Source: `Test of Honesty`**
[listing]
----
The pot is filled with gold. It can cover rent for 
2 months!
----

* **Speaker/Source: `Empty Pot`**
[listing]
----
The pot is empty
----

* **Speaker/Source: `Empty Pot`**
[listing]
----
The pot is empty
----

### Shopping District & Lower Village

Buying the offering potion; the shopkeeper's note; St. Baron's locked mansion.

*Associated Maps*: Map 029, Map 030, Map 016

#### Map 030: Shopping District

* **Speaker/Source: `St. Baron's Home`**
[listing]
----
\n<[Elijah]>I can't believe that I am doing
this,...the baron is the richest 
man in the villiage...but he's been 
using the town! This is for the good!
\n<[Katrina]>Ok...here we go...let's do this. I 
have to do at least one good thing today.
\n<[Elijah]>I can believe the baron
would take all this money for himself! Not
anymore!
\n<[Elijah]>So the baron can still
keep a house while the rest of just 
struggle. This is gonna be fun.
The door is locked. Mayhbe you should come back at
a different time
----

* **Speaker/Source: `Shop Menu`**
[listing]
----
Welcoem to my shop! How can I help you today?
Thanks for coming! See you later!
----

* **Speaker/Source: `Shopkeeper's Wife Dialouge`**
[listing]
----
We've just got into town so we are still working 
on getting the funds for a bed. But my hubby
will not rest until we have that bed!
----

* **Speaker/Source: `Got the Potion`**
[listing]
----
\n<[Elijah]>I wonder why she needs a 
potion. Could she be a witch?
\n<[Katrina]>I can't believe she made me 
waste my money...
----

### John the Thief's Hideout

John's sidequest proposal to overthrow the corrupt Baron.

*Associated Maps*: Map 031

#### Map 031: John the Thief (Sidequest)

* **Speaker/Source: `John the Thief`**
[listing]
----
\n<John>What do you want kid! Can't you
see that I'm busy!
\n<John>What do you want kid! Can't you
see that I'm busy!
\n<John>So you're the one that's helping the little
people of this village out.<br>I have a little task 
for you.<br>You see, the Baron has outlived his
welcome.<br>Think you can help with this...issue
\n<John>So you're the one that's helping the little
people of this village out.<br>I have a little task 
for you.<br>You see, the Baron has outlived his
welcome.<br>Think you can help with this...issue
\n<John>So you're the one that's helping the little
people of this village out.<br>I have a little task 
for you.<br>You see, the Baron has outlived his
welcome.<br>Think you can help with this...issue
\n<John>So you're the one that's helping the little
people of this village out.<br>I have a little task 
for you.<br>You see, the Baron has outlived his
welcome.<br>Think you can help with this...issue
\n<John>What do you want kid! Can't you
see that I'm busy!
----


## Act VII: The Frontier, Wilderness & Final Dungeon

### World Map Frontier

Demo completion gateway; thank-you message; frontier exits.

*Associated Maps*: Map 033, Map 034, Map 035, Map 005, Map 018

### Wilderness Battle Zones (Demo Areas 3 & 4)

Combat zones featuring bats, slimes, magos, and bandits.

*Associated Maps*: Map 014, Map 015, Map 016, Map 034, Map 035, Map 036, Map 019, Map 020, Map 021, Map 022, Map 033

#### Map 020: Grandma's House Upstairs

* **Speaker/Source: `Grandma's Chest`**
[listing]
----
\n<[Elijah]>I shouldn't go in this chest. I 
need to talk to Grandma first.
She's very particular about her things.
\n<[Katrina]>I shouldn't go in this chest. I 
need to talk to Grandma first.
Batty old woman seems to remind that chest.
----

* **Speaker/Source: `Grandma's Chest`**
[listing]
----
An old watch was in the chest.
\n<[Elijah]>Oh.. it's an old watch.
What's so special about it anyway?
\n<[Katrina]>She made me....grab...an 
OLD WATCH! I thought it was
something important like her will!
----

* **Speaker/Source: `Grandma's Chest`**
[listing]
----
There is still something inside of the chest
OBTAINED THE COMPASS OF MORALITY
----

* **Speaker/Source: `Grandma's Chest`**
[listing]
----
It's all empty now.
----

* **Speaker/Source: `Wierd Dream`**
[listing]
----
\n<[Elijah]>That dream....what did it mean...
It could just be my imagination but...It 
felt like...something more. A hero
... Me...?
\n<[Katrina]>WHAT! WHAT THE...ME. Man, my mind
needs to get out of lala land and settle 
into reality. I can't be a hero, 
I won't be a hero...
----

#### Map 021: Grandma's House Downstairs

* **Speaker/Source: `Grandma`**
[listing]
----
\n<Grandma>Hello sweetie, could you please
go upstairs and search in the chest
and grab what's inside? It's something that
has great signifinance.
\n<[Katrina]>The will. I love you gran,
but a girl gotta get out of this dead-end
town.
\n<[Elijah]>Maybe is is something
about dad! I haven't seen him since my 
10th birthday.
\n<[Elijah]>Sure thing. This might
be my chance to get answers!
\n<[Katrina]>Why not! Crazy old woman...
That will is mine!
\n<[Elijah]>Well...uhh...but... 
But if it is about him...
\n<[Katrina]>Gran, why can't you do it? 
Why doesn't she do it?
If she wants it she can get it.
----

* **Speaker/Source: `Grandma`**
[listing]
----
It's been a while since we've talk. 
What do you want to talk about?
\n<Grandma>Oh me and Monica go way back.
She was such a wild child, always getting
into mischief. You'll love her.
\n<Grandma>This town used to be full of
life. sighs The kingdom doesn't
pay much attention to this place anymore...
\c[5]Not avalible in the demo
\n<Grandma>Have you been upstairs yet?
The chest should still be there.
----

* **Speaker/Source: `Grandma`**
[listing]
----
\n<Grandma>Thank you honey. This watch has 
been in the family for a long time. Oh, 
how it brings back memories of better days.
\n<Grandma>I think you should keep it. It's
my gift to you. And here's a bit of money.
I know that you'll need it more than little
ol' me.
----

* **Speaker/Source: `Grandma`**
[listing]
----
\n<Grandma>[Katrina] could you do me a favor?
Lately, the people have been stirred up
about people being attacked by monsters.
\n<Grandma>[Elijah] could you do me a favor?
Lately, the people have been stirred up
about people being attacked by monsters.
\n<Grandma>My friend, Monica, 
would know what is causing these strange 
rumors. Can you go to her house and see 
what she has to say about all of this?
\n<[Elijah]>Sure thing. It will be nice to
finally leave the house. I might even
get to leave town!
\n<[Katrina]>Why not! I would love to~ 
I'm out of this poor slum of a town!
\n<[Elijah]>Well..are you sure you can't?
I don't even know her...
\n<[Katrina]>Gran, I'm not your errand girl! 
 This is YOUR friend.
----

* **Speaker/Source: `Block`**
[listing]
----
I should say something to Grandma before I go.
----

* **Speaker/Source: `Block`**
[listing]
----
Grandma still seems worried. Maybe I can talk to
her one more time.
----

* **Speaker/Source: `Block`**
[listing]
----
I should say something to Grandma before I go.
----

* **Speaker/Source: `Block`**
[listing]
----
Grandma still seems worried. Maybe I can talk to
her one more time.
----

* **Speaker/Source: `Block`**
[listing]
----
I should say something to Grandma before I go.
----

* **Speaker/Source: `Block`**
[listing]
----
Grandma still seems worried. Maybe I can talk to
her one more time.
----

### The Final Dungeon & Boss Threshold

Multi-level dungeon fortress leading to the ultimate demo boss confrontation.

*Associated Maps*: Map 017, Map 018, Map 023, Map 024, Map 025, Map 026, Map 027, Map 028, Map 037, Map 038

#### Map 023: Main Square

* **Speaker/Source: `Fighter Kid`**
[listing]
----
... He's just ignoring you
----

* **Speaker/Source: `Fighter Kid`**
[listing]
----
You want me to move? I'm blocking the way for others?
Big woop! No one can stop me so I'm staying!
----

* **Speaker/Source: `Fighter Kid`**
[listing]
----
You're no fun! I'm going home!
----

* **Speaker/Source: `Mystic's Home`**
[listing]
----
The door is locked.
----

* **Speaker/Source: `Leave Town`**
[listing]
----
I should see if Monica needs anything else.
----

* **Speaker/Source: `Lower Dio`**
[listing]
----
I have to stop that kid first!
----

* **Speaker/Source: `Lower Dio`**
[listing]
----
I still have to talk to Monica first.
----

* **Speaker/Source: `Lower Dio`**
[listing]
----
I have to stop that kid first!
----

* **Speaker/Source: `Lower Dio`**
[listing]
----
I still have to talk to Monica first.
----

* **Speaker/Source: `Lower Dio`**
[listing]
----
I have to stop that kid first!
----

* **Speaker/Source: `Lower Dio`**
[listing]
----
I still have to talk to Monica first.
----

* **Speaker/Source: `Lower Dio`**
[listing]
----
I have to stop that kid first!
----

* **Speaker/Source: `Lower Dio`**
[listing]
----
I still have to talk to Monica first.
----

* **Speaker/Source: `Lower Dio`**
[listing]
----
I have to stop that kid first!
----

* **Speaker/Source: `Lower Dio`**
[listing]
----
I still have to talk to Monica first.
----

* **Speaker/Source: `Lower Dio`**
[listing]
----
I have to stop that kid first!
----

* **Speaker/Source: `Lower Dio`**
[listing]
----
I still have to talk to Monica first.
----

* **Speaker/Source: `Monica!`**
[listing]
----
\n<[Elijah]>This is great! The town is still
sad with the storm but it's my home. I 
need to stay focus and find Monica's house.
Grandma said her house is in front of ours.
\n<[Katrina]>Ugghh. I can't believe we still
live in this dump. Doesn't help that storm
destoryed everything. Now Gran said Monica
live in the house in front of us. Peachy...
----

* **Speaker/Source: `Leave Town`**
[listing]
----
I should see if Monica needs anything else.
----

* **Speaker/Source: `Leave Town`**
[listing]
----
I have to stop that kid first!
----

* **Speaker/Source: `Block`**
[listing]
----
STAY AWAY FROM ME!
----

* **Speaker/Source: `Block`**
[listing]
----
STAY AWAY FROM ME!
----

* **Speaker/Source: `Block`**
[listing]
----
STAY AWAY FROM ME!
----

#### Map 024: The Sword Map

* **Speaker/Source: `Medusa's Sword`**
[listing]
----
A strange sword was in the chest.
\n<[Elijah]>I wonder who this sword 
belongs to.
\n<[Katrina]>Free sword for your girl~
----

#### Map 025: The Sword Map

* **Speaker/Source: `Medusa's Sword`**
[listing]
----
A strange sword was in the chest.
\n<[Elijah]>I wonder who this sword 
belongs to.
\n<[Katrina]>Free sword for your girl~
----

#### Map 026: Start Up Area

* **Speaker/Source: `The Beginning`**
[listing]
----
It was a month before the Festival...<br>The tower...<br>The terror...
----

* **Speaker/Source: `The Beginning`**
[listing]
----
The darkness...<br>The darkness...<br>The darkness returned once more.
----

#### Map 027: Monica's House

* **Speaker/Source: `Monica`**
[listing]
----
\n<Monica>And who in the blaze are YOU!
\n<Monica>...
...
...
\n<Monica>So you are Magolia's grandson.
She made you sound more handsome that
you are.
\n<[Elijah]>HEY! That's not a nice thing
to say!
\n<Monica>Tis only a joke. Now why have 
you came here? Do you have some milk?
\n<[Elijah]>Nn-...I'm here because my grandma
thinks that evil has returned.
\n<Monica>A wise woman she is. Lyri has
been whispering in me ear about the forces
of darkness. I'll help but first, get that
holigan from the city limits.
\n<Monica>Not only is he preventing folks
from leaving, he's also messing with me
business. Can you handle that?
\n<Monica>Good. I'll see you when you
return.
\n<Monica>I'll explain it once more.
\n<Monica>I'll help you if you get that 
holigan from the city limits.
\n<Monica>Not only is he preventing folks
from leaving, he's also messing with me
business. Can you handle that?
\n<Monica>So you are Magolia's 
granddaughter. She made you sound prettier 
that you are.
\n<[Katrina]>LISTEN HER YOU OLD-!
\n<Monica>Tis only a joke. Now why have 
you came here? Do you have some milk?
\n<[Katrina]>What is wrong with you... I'm
just here because my grandma is crazy.
She's babbling about evil forces and all
that nonsense.
\n<Monica>A wise woman she is. Lyri has
been whispering in me ear about the forces
of darkness. I'll help but first, get that
holigan from the city limits.
\n<Monica>Not only is he preventing folks
from leaving, he's also messing with me
business. Can you handle that?
\n<Monica>Good. I'll see you when you
return.
\n<Monica>I'll explain it once more.
\n<Monica>I'll help you if you get that 
holigan from the city limits.
\n<Monica>Not only is he preventing folks
from leaving, he's also messing with me
business. Can you handle that?
----

* **Speaker/Source: `Monica`**
[listing]
----
\n<Monica>You have to stop that kid! Our 
village will be more devastated than
even with him blocking the road!
----

* **Speaker/Source: `Monica`**
[listing]
----
\n<Monica>Good job! I think I have an idea.
Now that the people can, residences and
newcomers have arrived. I notice that a 
shopkeeper has came into our town as well.
\n<Monica>I need you to go a buy a potion.
I will use this as a base for my offering
to Lyri. She'll be able to help us in this
dark hour.
Obtained the Book of Monsters
----

* **Speaker/Source: `Monica`**
[listing]
----
\n<Monica>Oh yes. This is exactly what I
need for my offering.
\n<Monica>There is one more thing that I
need in order to make this brew suitable
for Lyri. Go out of the town to the west 
path. A man has what I need. Get it please.
\n<Monica>If you want to get the potion,
you'll need to go to the lower parts of the
town.
----

* **Speaker/Source: `Monica`**
[listing]
----
\n<Monica>Do you need something?
\n<Monica>Me? Why, I'm one of the original
mages for the King. The kingdom was not as
peaceful as is it now. But then, the 
Dark Lord tried to take the kingdom.
\n<Monica>He was born from the darkness
that this world had. He planned to take 
over this world and fill it with darkness.
The Hero saved this world and brought us
\n<Monica>our peace. The shield and sword.
Oh don't mind this old hag's ranmbling.
Not avalible in demo
Not avalible in demo
----

* **Speaker/Source: `The Secret Room`**
[listing]
----
Get your snooping nose away from me room!
This room is currently unavalible for the demo
----

* **Speaker/Source: `The Secret Room`**
[listing]
----
Get your snooping nose away from me room!
This room is currently unavalible for the demo
----

* **Speaker/Source: `The Secret Room`**
[listing]
----
Get your snooping nose away from me room!
This room is currently unavalible for the demo
----

* **Speaker/Source: `Lyri`**
[listing]
----
It is the deity Lyri, the Goddess of Peace.
----

* **Speaker/Source: `Totem`**
[listing]
----
The totem has the names of fallen soldiers. One
name has a heart by it and it reads "Miss you".
\n<[Elijah]>Did Monica have a husband?
\n<[Katrina]>Someone married that crazy bat!
----

* **Speaker/Source: `Totem`**
[listing]
----
The totem is full of offerings.
----

#### Map 028: Eric's House

* **Speaker/Source: `Eric`**
[listing]
----
We've been seeing a lot of monster appear near the 
recently. I wonder what's that all about. And with
jobs being tight, money is hard to come by.
----

* **Speaker/Source: `Eric`**
[listing]
----
\n<[Elijah]>I think that that should be 
enough to help you.
Thank you. Thank you so much, here I believe that 
this will be more useful to you than me.
\n<[Elijah]>Thank you sir, but I...
\n<Eric>Don't mention it, and by the way, the name
is Eric. I used to be a lumberjack until that weird
portal appeared. I found this ring and held on to it
every since.
\n<n[1]>Thank you. Thank you so much!
\n<[Katrina]>I think that that should be 
enough to help you.
Thank you. Thank you so much, here I believe that 
this will be more useful to you than me.
\n<[Katrina]>Woah! This is a cool ring dude!
\n<Eric>The name is Eric and that ring has a story.
I used to be a lumberjack until that weird
portal appeared. I found this ring and held on to it
every since.
\n<n[2]>Thanks dude.
----

* **Speaker/Source: `Eric`**
[listing]
----
\n<Eric>I'm sure that ring's fire power will come in 
handy if you plan on leaving the village
----

* **Speaker/Source: `Self-Aware Pot`**
[listing]
----
What kind of game do you think this is! You can't
just steal whatever you want! How rude!
----

* **Speaker/Source: `Test of Honesty`**
[listing]
----
Something is glowing in the pot. Take the pots content?
You recieve 10G
THAT'S ALL MY MONEY! GET OUT NOW!
\n<\n1>I am so sorry! Please, I am not 
trying to...
\n<n[2]>AH!
\n<n[2]>Hey, where did you even...
\n<n[2]>AH!
\n<[Elijah]>..steal all your money.
\n<n[1]>I'm sorry...
\n<n[2]>...come from...
\n<n[2]>It was just a few coins anyway.
That guy definately overreacted
You decide to leave the pot alone.
\n<n[1]>I can be quick, discret maybe
Elijah slowly reaches into the jar, stopping when
he saw how little money he had.
\n<n[1]>Maybe I should't...
\n<n[1]>I would be wrong of me to take this
money. But why does he only have a little 
money...
Hey want are you doing!
\n<n[1]>I am so sorry, I wasn't trying to
take your money, I was just looking at the 
pot.
It's fine kid... I just have stumble one hard
time every since the monster began showing up. 
It makes working hard and I can't afford to 
live here anymore.
\n<n[1]>Maybe... Maybe I can help!
Don't worry about it, it's not for you to worry about.
Just make sure you become something.
\n<n[1]>Is this what the voice meant,
to be a hero...How much do you need.
Just 500G, that will be enough for me to make it for 
the month.
\n<n[1]>I'll leave the money in the pot, 
that way you'll have it.
Tha-tha-thank you!
\n<n[1]>I would be wrong of me to take this
money. But why does he only have a little 
money...
Hey want are you doing!
\n<n[1]>I am so sorry, I wasn't trying to
take your money, I was just looking at the 
pot.
It's fine kid... I just have stumble one hard
time every since the monster began showing up. 
It makes working hard and I can't afford to 
live here anymore.
\n<n[1]>Maybe... Maybe I can help!
Don't worry about it, it's not for you to worry about.
Just make sure you become something
\n<n[1]>I have to help him somehow...
How much do you need.
Just 500G, that will be enough for me to make it for 
the month
\n<n[1]>I'll leave the money in the pot, 
that way you'll have it
Tha-tha-thank you!
\n<n[2]>The money is right there! This 
guy won't see it coming.
As Katrina was about to take the money, she stopped
for just a moment
\n<n[2]>Ugghhh, I can't do it.
\n<n[2]>Come on [Katrina], its just a few bucks,
you can give him some more when you get 
more.
Hey want are you doing!
\n<n[2]>Look man, I was just looking at
the stupid thing and saw some pocket change
in the pot. I was going to give you 
something back...eventually.
That would've been all of my money, my life savings!
That's my money for the whole week!
\n<n[2]>Hold on, you mean to tell me that
you only have 10G to your name. Dude, how
can you afford to live here!
Don't worry about that, the monster attacks have made
living in town alot cheaper...even without my job...
\n<n[2]>Is this what the voice meant,
to be a hero...How much do rent here
cost...
500G, but I won't be able to make that kind of money
in time to pay the rent
\n<n[2]>sigh I'll get the 
money, just don't do...anything
Really? Thanks...who are you?
\n<n[2]>The girl next door...actually no,
just someone who understands hard times
\n<n[2]>Come on [Katrina], its just a few bucks,
you can give him some more when you get 
more.
Hey want are you doing!
\n<n[2]>Look man, I was just looking at
the stupid thing and saw some pocket change
in the pot. I was going to give you 
something back...eventually.
That would've been all of my money, my life savings!
That's my money for the whole week!
\n<n[2]>Hold on, you mean to tell me that
you only have 10G to your name. Dude, how
can you afford to live here!
Don't worry about that, the monster attacks have made
living in town alot cheaper...even without my job...
\n<n[2]>I have to help him somehow...
How much do rent here cost...
500G, but I won't be able to make that kind of money
in time to pay the rent
\n<n[2]>sigh I'll get the 
money, just don't do...anything
Really? Thanks...who are you?
\n<n[2]>The girl next door...actually no,
just someone who understands hard times
----

* **Speaker/Source: `Test of Honesty`**
[listing]
----
Would you like to add to the pot?
You have given \v[14] to the man.
How much will you add?
You have given \v[14] to the man.
You have filled the pot! Talk to the man once more.
The pot isn't full yet.
You don't have that much to give
You leave the pot alone.
----

* **Speaker/Source: `Test of Honesty`**
[listing]
----
The pot is filled with gold. It can cover rent for 
2 months!
----

* **Speaker/Source: `Empty Pot`**
[listing]
----
The pot is empty
----

* **Speaker/Source: `Empty Pot`**
[listing]
----
The pot is empty
----


---

## Chapter 3: Complete Cast & Tactical Party Compendium

> **Source**: `/home/professorseanex/Tenebris and Lux/docs/game_design/demo_baseline/cast_and_characters.adoc`

[abstract]
## Overview

This compendium catalogues every character, party companion, village elder, court
antagonist, and mystical summon present in the 2017–2018 demo archives, detailing
their combat statistics, class associations, sprite bindings, and narrative significance.

## 1. Dual Protagonists

### Elijah Ro-enheart (The Steadfast Vanguard)
* **ID**: `Actor 1`
* **Class**: Hero (`Class 5`) / Knight (`Class 1`)
* **Relic Affinity**: Bearer of the **Shield of Lux**
* **Initial Level**: Lv 5 (Folder 1) / Lv 1 (Folder 2)
* **Movement Points**: 4 MP
* **Sprite / Battler**: `Actor1_1` / `MainCharacter` (custom generator)
* **Profile & Personality**:
  Son of the vanished Commander Gareth Ro-enheart (former Head of the Royal Guard).
  Earnest, chivalric, deeply burdened by his father's mysterious disappearance on his
  10th birthday. Elijah seeks answers through service and duty, instinctively stepping
  forward to protect the weak and uphold the light of Sacyra.
* **Canonical LeTBS Specification**:
[source,text]
----
<letbs>
move_points: 4
auto_turn_order_face
auto_status_sprite
</letbs>
----

### Lady Katrina Jaren (The Outrider Blade)
* **ID**: `Actor 2`
* **Class**: Hero (`Class 5`) / Start (`Class 7`)
* **Relic Affinity**: Wielder of the **Sword of Tenebris**
* **Initial Level**: Lv 5 (Folder 1) / Lv 1 (Folder 2)
* **Movement Points**: 4 MP
* **Initial Equipment**: Weapon 1 (Sword), Armor 2 (Hat), Armor 3 (Cloth)
* **Sprite / Battler**: `Actor1_8`
* **Profile & Personality**:
  Noble daughter of aristocratic House Jaren. Rebellious, razor-sharp, pragmatic, and
  fiercely independent. Refusing a cloistered aristocratic marriage or court seat, Katrina
  escaped to Dio Village with eyes on joining the independent Adventurers' Guild. Her
  caustic wit masks a deep empathy for ordinary folk crushed by royal neglect.
* **Canonical LeTBS Specification**:
[source,text]
----
<letbs>
move_points: 4
auto_turn_order_face
auto_status_sprite
</letbs>
----

## 2. Extended Tactical Party Companions (Folder 2 Additions)

In `Old_Game_Data_2`, three specialized tactical party members were integrated to expand the grid-based TRPG party:

### Sebastien (The Infiltrator Outlaw)
* **ID**: `Actor 4`
* **Class**: Thief (`Class 6`)
* **Initial Level**: Lv 4
* **Sprite / Battler**: `Actor3_7`
* **Tactical AI & Role**: Outrider flanker equipped with `ranged_fighter` AI, trap disarmament, and precision critical strikes.

### Angela (The Evoker Elementalist)
* **ID**: `Actor 5`
* **Class**: Mage (`Class 2`)
* **Initial Level**: Lv 4
* **Movement Scope**: `square(2)` (4 MP)
* **Sprite / Battler**: `Actor3_8` (custom cast, atk, hit, victory animations)
* **Action Palette**: Full tactical command set (`Move`, `Skill[42]`, `Items`, `Scan`, `Pass`).
* **Tactical Role**: Long-range elemental artillery, crowd control, and battlefield terrain modification (ice walls, volcano ground).

### Lucius (The Battle Priest)
* **ID**: `Actor 6`
* **Class**: Priest (`Class 3`)
* **Initial Level**: Lv 4
* **Sprite / Battler**: `Actor2_7`
* **Tactical Role**: Turn order manipulation (`Accelerate`, `Decelerate`, `Emergency Turn`), long-distance allied teleportation, and barrier restoration.

## 3. Summoned Celestial Entities

### Wind Spirit (Actor 11)
* **Class**: `Class 12` | **Sprite**: `Nature` (Index 4)
* **Skills**: Air Compression (`Skill 132`), Great Wind (`Skill 133` - pushback), Transposition (`Skill 134` - swap places).

### Pixie (Actor 12)
* **Class**: `Class 13` | **Sprite**: `Nature` (Index 5)
* **Skills**: Healing Wave (`Skill 135` - AOE restoration).

### Fire Chicken (Actor 13)
* **Class**: `Class 14` | **Sprite**: `Nature` (Index 2)
* **Characteristics**: Explosive combat decoy (`loop_animation: 262`), Bomb (`Skill 136`), Fire Arrow (`Skill 137`).

## 4. Key Lore Figures & NPCs

[cols="1,2,4", options="header"]
|===
| Name | Role & Allegiance | Historical Significance
| **Grandma Magolia** | Matriarch of Dio Village | Mother of vanished Commander Gareth Ro-enheart; grandmother to Elijah. Bestows *Grandma's Watch* and *Compass of Morality*.
| **Monica the Mystic** | Former Royal Battlemage | Eccentric hermit and priestess of Goddess Lyri. Guides protagonists in awakening relic powers through ritual offerings.
| **Eric the Lumberjack** | Impoverished Dio Laborer | Loses his livelihood to monster raids; serves as the moral litmus test for player empathy in the *Test of Honesty*. Bestows the *Ring of Fire*.
| **John the Thief** | Local Outlaw Leader | Operates in Lower Dio; plots to unseat the predatory St. Baron to return wealth to the villagers.
| **The St. Baron** | Aristocratic Landowner | Feudal aristocrat bleeding Dio Village dry; symbol of court corruption and moral decay.
| **Chrono** | Archival Sage & Guardian | Guide of the Dream Chamber / Ancient Archives. Oversees archetype stone selection and mechanics trials.
| **High Priest Roté** | Royal Advisor | Court prelate who witnesses the cosmic omens and hastens to warn King Jihad.
| **King Jihad** | Monarch of Sacyra | Feigns royal mobilization while falling victim to the King's Curse, secretly coveting the returned primordial darkness to surpass The Dark Lord.
|===


---

## Chapter 4: LeTBS Tactical Grid System Specification

> **Source**: `/home/professorseanex/Tenebris and Lux/docs/game_design/demo_baseline/combat_and_letbs_mechanics.adoc`

[abstract]
## Overview

This document formalizes the grid-based Tactical Strategy RPG combat architecture
implemented via Lecode's Tactical Battle System (LeTBS) in the 2017–2018 demo.
It details movement costs, action economy, scope geometries, terrain manipulation,
and turn manipulation invariants.

## 1. Battle Field Architecture & Action Economy

* **Grid Structure**: 2D tile-based tactical grid mapped over encounter maps.
* **Movement Points (MP)**: Consumable per-turn budget determining movement range:
  - Baseline Protagonists & Allies: `move_points: 4`.
  - Heavy Impact Reductions: Heavy Hit reduces target MP by 3 (`move_points+: -3`).
  - Outrider Haste: Light Steps increases target MP by 1 (`move_points+: 1`).
* **Turn Timeline**: Monotonic timeline ordering turns by entity agility (`auto_turn_order_face`).
* **Directional Facing**: Facing impacts defense and positional abilities (e.g., *Phantom Slash* teleports directly behind the target before striking).

## 2. Scope & Area of Effect (AOE) Geometries

The demo configured rich geometric targeting parameters across skills:

[cols="2,2,3", options="header"]
|===
| Scope Pattern | Example Skill | Geometric Definition
| `circle(r)` | *Fire* (`Skill 42`), *Heal* (`Skill 72`) | Manhattan or Chebyshev circle of radius `r` centered on the caster.
| `circle(max_r, min_r)` | *Ghost Arrow* (`Skill 79`), *Dagger Throw* (`Skill 169`) | Annular targeting ring; cannot target adjacent cells within `min_r`.
| `line(n)` | *Explosive Mark* (`Skill 76`), *Great Wind* (`Skill 133`) | Cardinal linear projection up to `n` cells from caster.
| `square(r)` | *Angela Move Scope* (`Actor 5`) | Full square grid extent of radius `r`.
| `custom(pattern)` | *Fire Arrows* (`Skill 82`) | Scripted multi-target projectile pattern hitting all visible adversaries.
| `through_obstacles` | *Teleportation* (`Skill 73`) | Ignores intermediate terrain obstacles, requiring only a free destination cell.
|===

## 3. Terrain State Modifications & Dynamic Hazards

Combat encounters feature active environmental manipulation:

* **Volcano Ground (`Skill 47`)**: Lays hazardous fiery embers across multiple ground cells dealing continuous fire damage.
* **Ice Wall / Ice Prison (`Skill 51`, `Skill 52`)**: Summons physical destructible barricades (`HP: 20`) that obstruct enemy line-of-sight and movement corridors.
* **Explosive Mark & Magic Trap (`Skill 76`, `Skill 80`)**: Proximity-triggered grid landmines that detonate upon enemy entry.
* **Burning Tile Spread (`State 110`)**: Dynamic fire propagation expanding with `spread: 100% circle(1) all`.

## 4. Turn Timeline Manipulation

Unique priest techniques alter the flow of tactical battle:

* **Accelerate (`Skill 87`)**: Advances target position along the turn timeline by 100%.
* **Decelerate (`Skill 88`)**: Delays target position along the turn timeline by 100%.
* **Emergency Turn (`Skill 89`)**: Instantly grants an immediate action turn to an allied battler.
* **Extra Turn (`Skill 86`)**: Grants the caster or target an additional action cycle upon turn conclusion.


---

## Chapter 5: Classes, Growth Curves & Learning Trees

> **Source**: `/home/professorseanex/Tenebris and Lux/docs/game_design/demo_baseline/classes_and_growth.adoc`

[abstract]
## Overview

This document details the 7 primary player and NPC classes plus 4 auxiliary summon/guard
classes from `Classes.json`. It includes base stat formulas across 8 core attributes
(MHP, MMP, ATK, DEF, MAT, MDF, AGI, LUK) and skill learning progressions.

## Class 1: Warrior

[cols="^1,^1,^1,^1,^1,^1,^1,^1,^1", options="header"]
|===
| Level | MHP | MMP | ATK | DEF | MAT | MDF | AGI | LUK
| 1 | 100 | 30 | 12 | 8 | 3 | 8 | 27 | 32
| 5 | 145 | 46 | 17 | 12 | 5 | 11 | 27 | 42
| 10 | 202 | 64 | 23 | 17 | 8 | 15 | 27 | 55
| 20 | 314 | 102 | 35 | 26 | 13 | 22 | 27 | 80
| 50 | 650 | 215 | 71 | 54 | 27 | 44 | 27 | 155
| 99 | 1200 | 400 | 130 | 100 | 50 | 80 | 27 | 277
|===

### Skill Learnings for Warrior
[cols="^1,2,3", options="header"]
|===
| Level | Skill Name | Strategic Role
| Lv 1 | **Repulsive Blow** (`ID: 11`) | Innate class technique
| Lv 1 | **Flash Jump** (`ID: 12`) | Innate class technique
| Lv 1 | **Rush** (`ID: 13`) | Innate class technique
| Lv 1 | **Intimidating Aura** (`ID: 14`) | Innate class technique
| Lv 1 | **Heavy Hit** (`ID: 15`) | Innate class technique
| Lv 1 | **Provoke** (`ID: 16`) | Innate class technique
| Lv 1 | **Phantom Slash** (`ID: 17`) | Innate class technique
|===

## Class 2: Mage

[cols="^1,^1,^1,^1,^1,^1,^1,^1,^1", options="header"]
|===
| Level | MHP | MMP | ATK | DEF | MAT | MDF | AGI | LUK
| 1 | 60 | 90 | 2 | 7 | 13 | 8 | 26 | 32
| 5 | 83 | 130 | 4 | 10 | 18 | 12 | 26 | 42
| 10 | 110 | 180 | 6 | 13 | 24 | 17 | 26 | 55
| 20 | 165 | 280 | 10 | 20 | 36 | 26 | 26 | 80
| 50 | 330 | 580 | 21 | 39 | 72 | 54 | 26 | 155
| 99 | 600 | 1070 | 40 | 70 | 130 | 100 | 26 | 277
|===

### Skill Learnings for Mage
[cols="^1,2,3", options="header"]
|===
| Level | Skill Name | Strategic Role
| Lv 1 | **Fire** (`ID: 42`) | Innate class technique
| Lv 1 | **Sparkle** (`ID: 43`) | Innate class technique
| Lv 1 | **Lightning Storm** (`ID: 44`) | Innate class technique
| Lv 1 | **Fire Storm** (`ID: 45`) | Innate class technique
| Lv 1 | **Fire Support** (`ID: 46`) | Innate class technique
| Lv 1 | **Volcano Ground** (`ID: 47`) | Innate class technique
| Lv 1 | **Ice Fury** (`ID: 49`) | Innate class technique
| Lv 1 | **Walking Fire** (`ID: 50`) | Innate class technique
| Lv 1 | **Ice Wall** (`ID: 51`) | Innate class technique
| Lv 1 | **Ice Prison** (`ID: 52`) | Innate class technique
| Lv 1 | **Whitch's Escape** (`ID: 53`) | Innate class technique
| Lv 1 | **Fire Chickens** (`ID: 54`) | Innate class technique
| Lv 1 | **Water (charged)** (`ID: 55`) | Innate class technique
| Lv 1 | **Mega Sparkle** (`ID: 56`) | Innate class technique
| Lv 1 | **Multi Fire** (`ID: 57`) | Innate class technique
|===

## Class 3: Priest

[cols="^1,^1,^1,^1,^1,^1,^1,^1,^1", options="header"]
|===
| Level | MHP | MMP | ATK | DEF | MAT | MDF | AGI | LUK
| 1 | 65 | 60 | 6 | 8 | 10 | 7 | 32 | 32
| 5 | 95 | 87 | 10 | 13 | 15 | 11 | 32 | 42
| 10 | 133 | 119 | 15 | 18 | 20 | 16 | 32 | 55
| 20 | 208 | 185 | 25 | 28 | 31 | 26 | 32 | 80
| 50 | 433 | 380 | 53 | 59 | 63 | 54 | 32 | 155
| 99 | 800 | 700 | 100 | 110 | 115 | 100 | 32 | 277
|===

### Skill Learnings for Priest
[cols="^1,2,3", options="header"]
|===
| Level | Skill Name | Strategic Role
| Lv 1 | **Heal** (`ID: 72`) | Innate class technique
| Lv 1 | **Teleportation** (`ID: 73`) | Innate class technique
| Lv 1 | **Salvation** (`ID: 74`) | Innate class technique
| Lv 1 | **Teleportation II** (`ID: 75`) | Innate class technique
| Lv 1 | **Explosive Mark** (`ID: 76`) | Innate class technique
| Lv 1 | **Light Steps** (`ID: 78`) | Innate class technique
| Lv 1 | **Ghost Arrow** (`ID: 79`) | Innate class technique
| Lv 1 | **Magic Trap** (`ID: 80`) | Innate class technique
| Lv 1 | **Fire Arrows** (`ID: 82`) | Innate class technique
| Lv 1 | **Wind Spirit** (`ID: 83`) | Innate class technique
| Lv 1 | **Summon Pixie** (`ID: 84`) | Innate class technique
| Lv 1 | **Shamanism** (`ID: 85`) | Innate class technique
| Lv 1 | **Extra Turn** (`ID: 86`) | Innate class technique
| Lv 1 | **Accelerate** (`ID: 87`) | Innate class technique
| Lv 1 | **Emergency Turn** (`ID: 89`) | Innate class technique
| Lv 1 | **Ally Teleport.** (`ID: 90`) | Innate class technique
| Lv 1 | **Mana Ball** (`ID: 92`) | Innate class technique
|===

## Class 4: Summoner

[cols="^1,^1,^1,^1,^1,^1,^1,^1,^1", options="header"]
|===
| Level | MHP | MMP | ATK | DEF | MAT | MDF | AGI | LUK
| 1 | 450 | 90 | 16 | 16 | 16 | 16 | 32 | 32
| 5 | 650 | 130 | 21 | 21 | 21 | 21 | 42 | 42
| 10 | 900 | 180 | 28 | 28 | 28 | 28 | 55 | 55
| 20 | 1400 | 280 | 40 | 40 | 40 | 40 | 80 | 80
| 50 | 2900 | 580 | 77 | 77 | 77 | 77 | 155 | 155
| 99 | 5350 | 1070 | 138 | 138 | 138 | 138 | 277 | 277
|===

### Skill Learnings for Summoner
[cols="^1,2,3", options="header"]
|===
| Level | Skill Name | Strategic Role
| Lv 1 | **Sentinel Totem** (`ID: 102`) | Innate class technique
|===

## Class 5: Hero

[cols="^1,^1,^1,^1,^1,^1,^1,^1,^1", options="header"]
|===
| Level | MHP | MMP | ATK | DEF | MAT | MDF | AGI | LUK
| 1 | 300 | 90 | 16 | 16 | 16 | 16 | 32 | 32
| 5 | 507 | 130 | 21 | 21 | 21 | 21 | 42 | 42
| 10 | 764 | 180 | 28 | 28 | 28 | 28 | 55 | 55
| 20 | 1280 | 280 | 40 | 40 | 40 | 40 | 80 | 80
| 50 | 2825 | 580 | 77 | 77 | 77 | 77 | 155 | 155
| 99 | 5350 | 1070 | 138 | 138 | 138 | 138 | 277 | 277
|===

### Skill Learnings for Hero
[cols="^1,2,3", options="header"]
|===
| Level | Skill Name | Strategic Role
| Lv 1 | **Heal** (`ID: 72`) | Innate class technique
| Lv 5 | **Repulsive Blow** (`ID: 11`) | Innate class technique
|===

## Class 6: Thief

[cols="^1,^1,^1,^1,^1,^1,^1,^1,^1", options="header"]
|===
| Level | MHP | MMP | ATK | DEF | MAT | MDF | AGI | LUK
| 1 | 450 | 90 | 16 | 16 | 16 | 16 | 32 | 32
| 5 | 650 | 130 | 21 | 21 | 21 | 21 | 42 | 42
| 10 | 900 | 180 | 28 | 28 | 28 | 28 | 55 | 55
| 20 | 1400 | 280 | 40 | 40 | 40 | 40 | 80 | 80
| 50 | 2900 | 580 | 77 | 77 | 77 | 77 | 155 | 155
| 99 | 5350 | 1070 | 138 | 138 | 138 | 138 | 277 | 277
|===

## Class 7: Start

[cols="^1,^1,^1,^1,^1,^1,^1,^1,^1", options="header"]
|===
| Level | MHP | MMP | ATK | DEF | MAT | MDF | AGI | LUK
| 1 | 1 | 0 | 1 | 999 | 1 | 999 | 1 | 1
| 5 | 1 | 1 | 1 | 999 | 1 | 999 | 1 | 1
| 10 | 1 | 1 | 1 | 999 | 1 | 999 | 1 | 1
| 20 | 1 | 1 | 1 | 999 | 1 | 999 | 1 | 1
| 50 | 1 | 1 | 1 | 999 | 1 | 999 | 1 | 1
| 99 | 1 | 1 | 1 | 999 | 1 | 999 | 1 | 1
|===

## Class 10: ---Special---

[cols="^1,^1,^1,^1,^1,^1,^1,^1,^1", options="header"]
|===
| Level | MHP | MMP | ATK | DEF | MAT | MDF | AGI | LUK
| 1 | 450 | 90 | 16 | 16 | 16 | 16 | 32 | 32
| 5 | 650 | 130 | 21 | 21 | 21 | 21 | 42 | 42
| 10 | 900 | 180 | 28 | 28 | 28 | 28 | 55 | 55
| 20 | 1400 | 280 | 40 | 40 | 40 | 40 | 80 | 80
| 50 | 2900 | 580 | 77 | 77 | 77 | 77 | 155 | 155
| 99 | 5350 | 1070 | 138 | 138 | 138 | 138 | 277 | 277
|===

## Class 11: Guard

[cols="^1,^1,^1,^1,^1,^1,^1,^1,^1", options="header"]
|===
| Level | MHP | MMP | ATK | DEF | MAT | MDF | AGI | LUK
| 1 | 100 | 30 | 12 | 8 | 3 | 8 | 27 | 32
| 5 | 145 | 46 | 17 | 12 | 5 | 11 | 27 | 42
| 10 | 202 | 64 | 23 | 17 | 8 | 15 | 27 | 55
| 20 | 314 | 102 | 35 | 26 | 13 | 22 | 27 | 80
| 50 | 650 | 215 | 71 | 54 | 27 | 44 | 27 | 155
| 99 | 1200 | 400 | 130 | 100 | 50 | 80 | 27 | 277
|===

### Skill Learnings for Guard
[cols="^1,2,3", options="header"]
|===
| Level | Skill Name | Strategic Role
| Lv 1 | **Heavy Hit** (`ID: 15`) | Innate class technique
|===

## Class 12: Wind Spirit

[cols="^1,^1,^1,^1,^1,^1,^1,^1,^1", options="header"]
|===
| Level | MHP | MMP | ATK | DEF | MAT | MDF | AGI | LUK
| 1 | 1 | 50 | 1 | 1 | 1 | 1 | 1 | 1
| 5 | 1 | 79 | 1 | 1 | 1 | 1 | 1 | 1
| 10 | 1 | 115 | 1 | 1 | 1 | 1 | 1 | 1
| 20 | 1 | 186 | 1 | 1 | 1 | 1 | 1 | 1
| 50 | 1 | 400 | 1 | 1 | 1 | 1 | 1 | 1
| 99 | 1 | 750 | 1 | 1 | 1 | 1 | 1 | 1
|===

### Skill Learnings for Wind Spirit
[cols="^1,2,3", options="header"]
|===
| Level | Skill Name | Strategic Role
| Lv 1 | **Air Compression** (`ID: 132`) | Innate class technique
| Lv 1 | **Great Wind** (`ID: 133`) | Innate class technique
| Lv 1 | **Transposition** (`ID: 134`) | Innate class technique
|===

## Class 13: Pixie

[cols="^1,^1,^1,^1,^1,^1,^1,^1,^1", options="header"]
|===
| Level | MHP | MMP | ATK | DEF | MAT | MDF | AGI | LUK
| 1 | 1 | 50 | 1 | 1 | 1 | 1 | 1 | 1
| 5 | 1 | 79 | 1 | 1 | 1 | 1 | 1 | 1
| 10 | 1 | 115 | 1 | 1 | 1 | 1 | 1 | 1
| 20 | 1 | 186 | 1 | 1 | 1 | 1 | 1 | 1
| 50 | 1 | 400 | 1 | 1 | 1 | 1 | 1 | 1
| 99 | 1 | 750 | 1 | 1 | 1 | 1 | 1 | 1
|===

### Skill Learnings for Pixie
[cols="^1,2,3", options="header"]
|===
| Level | Skill Name | Strategic Role
| Lv 1 | **Healing Wave** (`ID: 135`) | Innate class technique
|===

## Class 14: Fire Chicken

[cols="^1,^1,^1,^1,^1,^1,^1,^1,^1", options="header"]
|===
| Level | MHP | MMP | ATK | DEF | MAT | MDF | AGI | LUK
| 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1
| 5 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1
| 10 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1
| 20 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1
| 50 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1
| 99 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1
|===

### Skill Learnings for Fire Chicken
[cols="^1,2,3", options="header"]
|===
| Level | Skill Name | Strategic Role
| Lv 1 | **Bomb** (`ID: 136`) | Innate class technique
|===


---

## Chapter 6: The Complete Grimoire (75+ Skills & Formulas)

> **Source**: `/home/professorseanex/Tenebris and Lux/docs/game_design/demo_baseline/grimoire_and_skills.adoc`

[abstract]
## Overview

This document records the complete grimoire of all 75+ active techniques, spells,
and summon abilities defined in `Skills.json`. It provides mathematical damage
formulas, MP/TP costs, scope parameters, and tactical LeTBS sequences.


## Basic Martial Maneuvers

[cols="^1,2,^1,^1,3,4", options="header"]
|===
| ID | Skill Name | MP | TP | Damage Formula | Description & Tactical Note
| `1` | **Attack** | 0 | 0 | `a.atk * 4 - b.def * 2` |  + 
_Note_: `Skill #1 will be used when you select the Attack command.`
| `2` | **Guard** | 0 | 0 | `0` |  + 
_Note_: `Skill #2 will be used when you select the Guard command.`
| `3` | **Dual Attack** | 0 | 0 | `a.atk * 4 - b.def * 2` | 
| `4` | **Double Attack** | 0 | 0 | `a.atk * 4 - b.def * 2` | 
| `5` | **Triple Attack** | 0 | 0 | `a.atk * 4 - b.def * 2` | 
| `6` | **Escape** | 0 | 0 | `0` | 
| `7` | **Wait** | 0 | 0 | `0` | 
|===


## Knight Martial Techniques

[cols="^1,2,^1,^1,3,4", options="header"]
|===
| ID | Skill Name | MP | TP | Damage Formula | Description & Tactical Note
| `11` | **Repulsive Blow** | 0 | 0 | `this.phyDmg(120)` | Hits and push the target for two cell. + 
_Note_: `<letbs> scope: circle(1) sequence: atk_push(2) scope_color: #FFF </letbs>  <l...`
| `12` | **Flash Jump** | 0 | 0 | `this.phyDmg(110) + this.magDmg(10)` | Jumps to an available position in the scope and invoke  a thunder to damage nearby fighters. + 
_Note_: `<letbs> scope: circle(3,1) aoe: circle(1) scope_options: need_free_cell aoe_o...`
| `13` | **Rush** | 0 | 0 | `this.phyDmg(80)` | Rushes toward the target. + 
_Note_: `<letbs> scope: custom(rush) sequence: rush </letbs>  <letbs_AI> type: offense...`
| `14` | **Intimidating Aura** | 0 | 0 | `(a.atk * 4 - b.def * 2) * 1.2` | Casts an aura that reduce enemies' ATK by 15%. Grants as well 70% counter attack chance. + 
_Note_: `<letbs> scope: circle(0) scope_options: include_center </letbs>  <letbs_AI> t...`
| `15` | **Heavy Hit** | 0 | 0 | `this.phyDmg(135)` | Hits the target and remove 3 move points. + 
_Note_: `<letbs> scope: circle(1) sequence: atk </letbs>  <Learn Require Level: 5>  <l...`
| `16` | **Provoke** | 0 | 0 | `(a.atk * 4 - b.def * 2) * 1.2` | Draw the attention of enemies upon the user. Can use another action after this skill. + 
_Note_: `<letbs> scope: circle(0) scope_options: include_center extra_action </letbs> ...`
| `17` | **Phantom Slash** | 0 | 0 | `this.phyDmg(60)` | Use dark arts to reach the back of the target. + 
_Note_: `<letbs> scope: line(4) sequence: phantom_slash </letbs>  <Learn Require Level...`
|===


## Mage Elemental Spells

[cols="^1,2,^1,^1,3,4", options="header"]
|===
| ID | Skill Name | MP | TP | Damage Formula | Description & Tactical Note
| `42` | **Fire** | 0 | 0 | `this.magDmg(105)` | Deals fire damages to a single target in a medium range. + 
_Note_: `<letbs> scope: circle(3);line(4) </letbs>  <letbs_AI> type: offense </letbs_A...`
| `43` | **Sparkle** | 0 | 0 | `this.magDmg(80)` | Deals thunder damages to targets in a small area. + 
_Note_: `<letbs> scope: circle(4) aoe: circle(1) sequence: spark </letbs> <Learn Requi...`
| `44` | **Lightning Storm** | 0 | 0 | `this.magDmg(60)` | Attacks 3 times a random battler in the area. + 
_Note_: `<letbs> scope: line(4) aoe: circle(2) sequence: lightning_storm </letbs> <Lea...`
| `45` | **Fire Storm** | 0 | 0 | `this.magDmg(200)` | Attacks 6 times randomly in the area. + 
_Note_: `<letbs> scope: circle(4,2) aoe: square(1) sequence: fire_storm </letbs> <Lear...`
| `46` | **Fire Support** | 0 | 0 | `this.magDmg(80)` | Deals fire damages to a single target in a medium range with the help of close allies. + 
_Note_: `<letbs> scope: circle(4) sequence: fire_support </letbs> <Learn Require Level...`
| `47` | **Volcano Ground** | 0 | 0 | `(a.mat * 2 - b.mdf * 2)*2` | Lays multiple fire spots on the ground. + 
_Note_: `<letbs> scope: circle(0) scope_options: include_center aoe: custom(volcano_gr...`
| `48` | **Volcano Ground (dmg)** | 0 | 0 | `this.magDmg(25)` | 
| `49` | **Ice Fury** | 0 | 0 | `this.magDmg(115)` | Deals ice damages in a row. + 
_Note_: `<letbs> scope: line(4) aoe: custom(ice_fury) sequence: skill </letbs> <Learn ...`
| `50` | **Walking Fire** | 0 | 0 | `this.magDmg(105)` | Deals fire damages in a medium path. + 
_Note_: `<letbs> scope: circle(4) scope_options:through_obstacles aoe: path sequence: ...`
| `51` | **Ice Wall** | 0 | 0 | `(a.mat * 2 - b.mdf * 2)*1.15` | Summon a wall of ice. + 
_Note_: `<letbs> scope: line(3) scope_options: need_free_cell aoe: custom(ice_wall) ao...`
| `52` | **Ice Prison** | 0 | 0 | `(a.mat * 2 - b.mdf * 2)*1.15` | Summon a prison of ice. + 
_Note_: `<letbs> scope: circle(4) aoe: custom(ice_prison) aoe_options: need_free_cell ...`
| `53` | **Whitch's Escape** | 0 | 0 | `(a.mat * 2 - b.mdf * 2)*1.20` | Teleports then casts Spark at the old location. The scope is based on current move points. + 
_Note_: `<letbs> scope: circle(e.getMovePoints()+1) scope_options: need_free_cell sequ...`
| `54` | **Fire Chickens** | 0 | 0 | `(a.mat * 2 - b.mdf * 2)*1.15` | Summon explosive chickens + 
_Note_: `<letbs> scope: circle(2) aoe: circle(1) scope_options: need_free_cell aoe_opt...`
| `55` | **Water (charged)** | 0 | 0 | `this.magDmg(135)` |  + 
_Note_: `<letbs> scope: circle(4) sequence: map_skill charge: 2, 261 </letbs> <Learn R...`
| `56` | **Mega Sparkle** | 0 | 0 | `this.magDmg(60)` | Deals thunder damage on 3 different areas. + 
_Note_: `<letbs> scope: circle(4) aoe: circle(1) sequence: mega_spark 3 selections: di...`
| `57` | **Multi Fire** | 0 | 0 | `this.magDmg(105)` | Invoke 3 columns of fire. + 
_Note_: `<letbs> scope_select: enemies scope: circle(3); 2 selections: append; Select ...`
|===


## Priest Holy Arts & Turn Manipulation

[cols="^1,2,^1,^1,3,4", options="header"]
|===
| ID | Skill Name | MP | TP | Damage Formula | Description & Tactical Note
| `72` | **Heal** | 0 | 0 | `50 + a.mat` | Heals a single target and ends the turn. + 
_Note_: `<letbs> scope: circle(4,2) sequence: heal pass_on_use scope_options: include_...`
| `73` | **Teleportation** | 0 | 0 | `0` | Teleports the caster in a medium range. + 
_Note_: `<letbs> scope: circle(4) sequence: teleportation scope_options:through_obstac...`
| `74` | **Salvation** | 0 | 0 | `25 + a.mat` | Attracts allies and heals them in all directions. + 
_Note_: `<letbs> scope: circle(0) aoe: line(3) sequence: salvation scope_options: incl...`
| `75` | **Teleportation II** | 0 | 0 | `0` | Teleports the battler in front of the caster in  a medium range. + 
_Note_: `<letbs> scope: circle(3) sequence: teleportation_sup scope_options: through_o...`
| `76` | **Explosive Mark** | 0 | 0 | `None` | Lays a mark on the ground that deals fire damages to enemies on it. + 
_Note_: `<letbs> scope: line(5) aoe: square(1) sequence: explosive_mark </letbs> <Lear...`
| `77` | **Explosive Mark (dmg)** | 0 | 0 | `this.magDmg(50)` | 
| `78` | **Light Steps** | 0 | 0 | `50 + a.mat` | Increases target's move points for 2 turns. + 
_Note_: `<letbs> scope_options: include_center </letbs> <Learn Require Level: 5> <letb...`
| `79` | **Ghost Arrow** | 0 | 0 | `this.magDmg(75)` | Deals light damages to a single target in a medium range, ignoring any obstacle, and remove one move point. + 
_Note_: `<letbs> scope: circle(5,2) sequence: ghost_arrow scope_options: through_obsta...`
| `80` | **Magic Trap** | 0 | 0 | `None` | Lays a magic trap who deals light damage while blinding the target with 60% rate. + 
_Note_: `<letbs> scope: circle(5) scope_options:through_obstacles, need_free_cell aoe:...`
| `81` | **Magic Trap (dmg)** | 0 | 0 | `this.magDmg(160)` | 
| `82` | **Fire Arrows** | 0 | 0 | `this.magDmg(105)` | Launch a fire arrow for every enemy in the scope. + 
_Note_: `<letbs> scope: line(2) aoe: custom(fire_arrows) aoe_options: line_of_sight se...`
| `83` | **Wind Spirit** | 0 | 0 | `(a.mat * 2 - b.mdf * 2)*1.15` | Summon a wind spirit that help the party with the power of wind. + 
_Note_: `<letbs> scope: circle(2) scope_options: need_free_cell sequence: summon(wind_...`
| `84` | **Summon Pixie** | 0 | 0 | `(a.mat * 2 - b.mdf * 2)*1.15` | Summon a pixie that can heal the party. + 
_Note_: `<letbs> scope: circle(1) scope_options: need_free_cell </letbs> <Learn Requir...`
| `85` | **Shamanism** | 0 | 0 | `a.mhp * 0.1` | All allies recover 10% of their MHP for each dead battler in the area. + 
_Note_: `<letbs> sequence: shamanism scope: circle(3) aoe: circle(2) scope_options: in...`
| `86` | **Extra Turn** | 0 | 0 | `50 + a.mat` | Add an extra turn. + 
_Note_: `<letbs> scope: circle(2) scope_options: include_center extra_turn: 100% </let...`
| `87` | **Accelerate** | 0 | 0 | `50 + a.mat` | Accelerate the turn order of the target. + 
_Note_: `<letbs> scope: circle(2) scope_options: include_center accelerate_turn: 100% ...`
| `88` | **Decelerate** | 0 | 0 | `50 + a.mat` | Accelerate the turn order of the target. + 
_Note_: `<letbs> scope: circle(2) scope_options: include_center decelerate_turn: 100% ...`
| `89` | **Emergency Turn** | 0 | 0 | `50 + a.mat` | Give an instant turn to the target. + 
_Note_: `<letbs> scope: circle(3) pass_on_use scope_options: include_center instant_tu...`
| `90` | **Ally Teleport.** | 0 | 0 | `0` | Teleports an ally in a medium range. + 
_Note_: `<letbs> scope_select: allies scope: circle(4) sequence: ally_teleport scope_o...`
| `91` | **Ally Teleport. (Scope)** | 0 | 0 | `0` |  + 
_Note_: `<letbs> scope: circle(4) scope_options:through_obstacles, need_free_cell </le...`
| `92` | **Mana Ball** | 0 | 0 | `this.magDmg(80)` | Launch a bouncing mana ball + 
_Note_: `<letbs> scope: circle(3) sequence: mana_ball </letbs> <Learn Require Level: 5...`
|===


## Summoner Totems & Rites

[cols="^1,2,^1,^1,3,4", options="header"]
|===
| ID | Skill Name | MP | TP | Damage Formula | Description & Tactical Note
| `102` | **Sentinel Totem** | 0 | 0 | `(a.mat * 2 - b.mdf * 2)*1.15` | Summon a totem that strike enemies as soon as they enter the range of action. + 
_Note_: `<letbs> scope: circle(3) scope_options: need_free_cell sequence: summon(senti...`
| `103` | **Transform.** | 0 | 0 | `50 + a.mat` |  + 
_Note_: `<letbs> scope: circle(0) scope_options: include_center </letbs> <letbs_AI> ty...`
| `104` | **Devour** | 0 | 0 | `this.magDmg(105)` |  + 
_Note_: `<letbs> scope: circle(1) fusion_effect </letbs> <letbs_AI> type: offense </le...`
|===


## Summon Beast Techniques

[cols="^1,2,^1,^1,3,4", options="header"]
|===
| ID | Skill Name | MP | TP | Damage Formula | Description & Tactical Note
| `132` | **Air Compression** | 0 | 0 | `this.magDmg(135)` | Deals wind damages in a medium range with 15% of chance to blind the target. + 
_Note_: `<letbs> scope: circle(4) </letbs> <letbs_AI> type: offense </letbs_AI> <Learn...`
| `133` | **Great Wind** | 0 | 0 | `this.magDmg(140)` | Deals wind damages and push the target. The collision damage of this skill is greatly increased. + 
_Note_: `<letbs> scope: line(4) sequence: great_wind collision_formula: 2 * b.mhp * 0....`
| `134` | **Transposition** | 0 | 0 | `(a.mat * 2 - b.mdf * 2)*1.35` | Switch place with a target in a medium range. + 
_Note_: `<letbs> scope: circle(6) sequence: transposition scope_options:through_obstac...`
| `135` | **Healing Wave** | 0 | 0 | `10 + a.mat` | Heals in small area + 
_Note_: `<letbs> scope: circle(3) aoe: circle(1) scope_options: include_center sequenc...`
| `136` | **Bomb** | 4 | 0 | `a.mhp` |  + 
_Note_: `<letbs> scope: circle(1) </letbs> <letbs_AI> type: offense </letbs_AI> <letbs...`
| `137` | **Fire Arrow** | 0 | 0 | `this.phyDmg(100) + this.magDmg(100)` | Launch a fire arrow. + 
_Note_: `<letbs> scope: circle(3) sequence: projectile(fire_arrow) </letbs> <Learn Req...`
|===


## Enemy Bestial & Void Techniques

[cols="^1,2,^1,^1,3,4", options="header"]
|===
| ID | Skill Name | MP | TP | Damage Formula | Description & Tactical Note
| `162` | **Poisonous Bite** | 3 | 0 | `(a.atk * 4 - b.def * 2)*0.8` |  + 
_Note_: `<letbs> scope: circle(1) </letbs> <letbs_AI> type: offense </letbs_AI>`
| `163` | **Fire Surge** | 4 | 0 | `this.magDmg(105)` | Deals fire damages to a single target in a medium range. + 
_Note_: `<letbs> scope: circle(4) </letbs> <letbs_AI> type: offense </letbs_AI>`
| `164` | **Summon Pyra** | 0 | 0 | `(a.mat * 2 - b.mdf * 2)*1.15` |  + 
_Note_: `<letbs> scope: circle(1) scope_options: need_free_cell sequence: summon_pyra ...`
| `165` | **Evil Healing** | 0 | 0 | `15 + a.mat` |  + 
_Note_: `<letbs> scope: circle(4) scope_options: include_center </letbs> <letbs_AI> ty...`
| `166` | **Mago's Mark** | 0 | 0 | `None` |  + 
_Note_: `<letbs> scope: line(5) aoe: square(1) sequence: mago_mark </letbs> <Cooldown:...`
| `167` | **Evil Protection** | 0 | 0 | `15 + a.mat` |  + 
_Note_: `<letbs> scope: circle(4) scope_options: include_center </letbs> <letbs_AI> ty...`
| `168` | **Rabbit's Jump** | 0 | 0 | `(a.atk * 4 - b.def * 2) * 1.10 + a.mat` |  + 
_Note_: `<letbs> scope: line(3) aoe: circle(1) scope_options: need_free_cell sequence:...`
| `169` | **Dagger Throw** | 3 | 0 | `this.phyDmg(115)` | Throw a little but sharp dagger toward the target. + 
_Note_: `<letbs> scope: circle(4,2) sequence: projectile(dagger) </letbs>  <letbs_AI> ...`
|===


---

## Chapter 7: Bestiary, Destructible Props & Troop Formations

> **Source**: `/home/professorseanex/Tenebris and Lux/docs/game_design/demo_baseline/bestiary_and_troops.adoc`

[abstract]
## Overview

This document catalogs all 22+ hostile monsters, wild fauna, bandits, void entities,
and interactive battlefield props from `Enemies.json` and `Troops.json`.

## 1. Monster Bestiary

[cols="^1,2,^1,^1,^1,^1,^1,^1,^1,^1,2", options="header"]
|===
| ID | Name | HP | MP | ATK | DEF | MAT | MDF | AGI | LUK | AI / Behavior
| `1` | **Bat** | 80 | 12 | 11 | 8 | 8 | 4 | 25 | 30 | Skill 1
| `2` | **Mago** | 60 | 18 | 8 | 6 | 16 | 6 | 22 | 30 | Skill 41, Skill 1
| `3` | **Rabbit** | 55 | 0 | 10 | 8 | 8 | 4 | 27 | 30 | Skill 1
| `4` | **Great Mago** | 90 | 36 | 10 | 7 | 19 | 7 | 23 | 30 | Skill 1
| `5` | **Fire Mago** | 90 | 36 | 10 | 7 | 19 | 7 | 23 | 30 | Skill 1
| `6` | **Pyra** | 30 | 50 | 6 | 8 | 6 | 4 | 27 | 30 | Basic Attack
| `7` | **Bandit** | 80 | 12 | 14 | 10 | 10 | 7 | 25 | 30 | Skill 1, Skill 169
| `8` | **Mage Bandit** | 80 | 12 | 11 | 8 | 8 | 4 | 25 | 30 | Skill 1
| `9` | **Slime** | 250 | 0 | 30 | 30 | 30 | 30 | 30 | 30 | Skill 1
| `10` | **Orc** | 300 | 0 | 30 | 30 | 30 | 30 | 30 | 30 | Skill 1
| `11` | **Minotaur** | 500 | 0 | 30 | 30 | 30 | 30 | 30 | 30 | Skill 1
| `12` | **Random Kid** | 100 | 0 | 30 | 10 | 10 | 10 | 10 | 10 | Skill 1, Skill 3
| `13` | **Dream Eater** | 70 | 12 | 11 | 8 | 8 | 4 | 25 | 30 | Skill 1
| `32` | **Invincible Neutral** | 99999 | 0 | 8 | 6 | 14 | 6 | 22 | 30 | Basic Attack
| `33` | **Tile Effect Launcher** | 1 | 0 | 1 | 1 | 1 | 1 | 1 | 1 | Basic Attack
| `34` | **Ice Block** | 20 | 0 | 1 | 1 | 1 | 1 | 1 | 1 | Basic Attack
| `35` | **Wooden Chest** | 50 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | Basic Attack
| `36` | **Silver Chest** | 50 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | Basic Attack
| `37` | **Golden Chest** | 50 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | Basic Attack
| `38` | **Stolen Goods** | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | Basic Attack
|===

## 2. Neutral & Interactive Grid Entities

The demo utilized the battle engine to render interactive tactical grid entities:
* **Wooden Chest (`Enemy 35`)**: `HP: 50` | Yields 200 Gold upon destruction via Common Event 5.
* **Silver Chest (`Enemy 36`)**: `HP: 50` | Yields 2,000 Gold upon destruction via Common Event 6.
* **Golden Chest (`Enemy 37`)**: `HP: 50` | Yields 10,000 Gold upon destruction via Common Event 7.
* **Ice Block (`Enemy 34`)**: `HP: 20` | Destructible terrain obstacle summoned by Ice Wall.
* **Stolen Goods (`Enemy 38`)**: `HP: 1` | Objective dropped upon defeat of Mage Bandit.
* **Invincible Neutral (`Enemy 32`)**: `HP: 99,999` | Static grid anchor.

## 3. Troop Encounter Formations

[cols="^1,2,3", options="header"]
|===
| Troop ID | Troop Designation | Composition

| `1` | **Bat*2** | Bat, Bat
| `2` | **Slime*2** | Mago, Mago
| `3` | **Orc** | Rabbit
| `4` | **Minotaur** | Great Mago
| `5` | **Random Kid** | Fire Mago
| `6` | **Bat** | Bat
| `7` | **Bat*2** | Bat, Bat
| `8` | **Bandit*3, Mage Bandit** | Bandit, Bandit, Mage Bandit, Bandit
| `9` | **Dream Eater*2** | Dream Eater, Dream Eater
|===


---

## Chapter 8: Armory, Inventory & Key Lore Relics

> **Source**: `/home/professorseanex/Tenebris and Lux/docs/game_design/demo_baseline/armory_and_inventory.adoc`

[abstract]
## Overview

This document details all weapons, protective armors, restorative items, and key relics
defined in `Weapons.json`, `Armors.json`, and `Items.json`.

## 1. Weapons & Relic Armaments

[cols="^1,2,^1,^1,^1,^1,^1,4", options="header"]
|===
| ID | Weapon Name | Type | Price | ATK | DEF | AGI | Lore & Characteristics
| `1` | **Sword** | Sword | 500\G | +10 | +0 | +0 | 
| `2` | **Axe** | Axe | 500\G | +10 | +0 | +0 | 
| `3` | **Cane** | Cane | 500\G | +10 | +0 | +0 | 
| `4` | **Bow** | Bow | 500\G | +10 | +0 | +0 | 
| `5` | **Medusa's Sword** | Sword | 1600\G | +17 | +0 | +0 | The sword used to slay Medusa. It is said to have her petrifying powers.
|===

## 2. Defensive Armor & Relic Wards

[cols="^1,2,^1,^1,^1,^1,^1,4", options="header"]
|===
| ID | Armor Name | Type | Price | DEF | MDF | MHP | Lore & Characteristics
| `1` | **Shield** | Small Shield | 300\G | +10 | +0 | +0 | 
| `2` | **Hat** | General Armor | 300\G | +10 | +0 | +0 | 
| `3` | **Cloth** | General Armor | 300\G | +10 | +0 | +0 | 
| `4` | **Ring** | General Armor | 300\G | +0 | +0 | +0 | 
| `5` | **Heart Shield** | Small Shield | 300\G | +10 | +5 | +0 | 
| `6` | **Ring of Fire** | General Armor | 0\G | +0 | +5 | +0 | 
|===

## 3. Consumable Items & Quest Relics

[cols="^1,2,^1,^1,4", options="header"]
|===
| ID | Item Name | Price | Consumable | Description & Architectural Mapping
| `1` | **Potion** | 10\G | Yes | 
| `2` | **Magic Water** | 100\G | Yes | 
| `3` | **Dispel Herb** | 200\G | Yes | 
| `4` | **Stimulant** | 300\G | Yes | 
| `5` | **Grandma's Watch** | 0\G | No (Key Relic) | This was Grandma's Watch. Its a family heirloom that keeps perfect time.
| `6` | **Book of Monsters** | 0\G | No (Key Relic) | This is a big with all the enemies you've encountered
| `7` | **Book of Items** | 0\G | No (Key Relic) | This book describes all the various items
| `8` | **Compass of Morality** | 0\G | No (Key Relic) | 
|===


---

## Chapter 9: Tactical States & Status Ailments Directory

> **Source**: `/home/professorseanex/Tenebris and Lux/docs/game_design/demo_baseline/states_and_statuses.adoc`

[abstract]
## Overview

This document catalogs all 30 status ailments, tactical combat states, aura modifiers,
and battlefield terrain effects defined in `States.json` across Folder 1 and Folder 2 (LeTBS).
It distinguishes generic RPG Maker MV Runtime Package (RTP) boilerplate from authentic
tactical grid modifiers and stance dynamics.

## 1. Triage: RTP Boilerplate States vs. Authentic Tactical Grid States

[cols="1,2,3", options="header"]
|===
| Category | State IDs | Architectural Policy

| **Tier 1: RTP Default States**
| `1`–`10` (Knockout, Guard, Immortal, Poison, Blind, Silence, Rage, Confusion, Fascination, Sleep)
| Stock RPG Maker MV status conditions. Re-architect in C11 to tie directly into Sacyran relic affinities, posture break, and elemental resistances.

| **Tier 3: Authentic LeTBS Grid States**
| `11`, `31`–`34`, `46`, `62`–`63`, `78`–`79`, `94`–`95`, `109`–`110`
| Authentic tactical modifiers altering grid movement points (MP), turn order priority, damage mitigation, and tile propagation.
|===

## 2. Standard Status Conditions (Tier 1 Boilerplate)

[cols="^1,2,^1,4", options="header"]
|===
| ID | State Name | Priority | Restriction / Description

| `1` | **Knockout** | 100 | Cannot move; unit is felled upon reaching 0 HP.
| `2` | **Guard** | 0 | Damage taken reduced by 50%; expires at start of next turn.
| `3` | **Immortal** | 0 | Unit cannot drop below 1 HP; used for critical NPC story anchors.
| `4` | **Poison** | 50 | Loses 10% max HP at end of combat turn.
| `5` | **Blind** | 60 | Hit rate reduced by 50%.
| `6` | **Silence** | 70 | Magic skills cannot be cast.
| `7` | **Rage** | 80 | Attacks nearest enemy or ally indiscriminately.
| `8` | **Confusion** | 75 | Actions randomized between foes and friends.
| `9` | **Fascination** | 70 | Charmed unit attacks allies.
| `10` | **Sleep** | 90 | Cannot take turns; wakes up upon taking damage.
| `11` | **Paralyze** | 85 | Stunned; skip turn (`loop_animation: 64`).
|===

## 3. Knight & Vanguard Tactical States (Tier 3 LeTBS)

[cols="^1,2,^1,4", options="header"]
|===
| ID | State Name | Affected Stat | Tactical Mechanism & LeTBS Meta

| `31`
| **Intimidating Aura**
| ATK -15%
| Passive aura cast via Skill 14 (`auras: intimidating_aura`). Reduces adjacent enemy physical attack and grants 70% counter-attack chance.

| `32`
| **Intimidated**
| ATK -15%
| Status inflicted on foes caught within the Intimidating Aura boundary.

| `33`
| **Heavy Strike**
| Move Points: -3
| Tactical stagger inflicted by Heavy Hit (Skill 15). Severely impedes enemy mobility on the grid.

| `34`
| **Focused**
| AI Threat
| Cast by Provoke (Skill 16). Forces enemy AI to prioritize the vanguard (`focused_by_ai_for_offense: 100`).
|===

## 4. Mage & Priest Arcane States (Tier 3 LeTBS)

[cols="^1,2,^1,4", options="header"]
|===
| ID | State Name | Affected Stat | Tactical Mechanism & LeTBS Meta

| `46`
| **Charging**
| Spell Delay
| Channeling visual state for high-tier spells like Water (charged) (`loop_animation: 184`).

| `62`
| **Light Steps**
| Move Points: +1
| Mobility buff cast via Skill 78. Expands grid traversal budget for outriders and rogues.

| `63`
| **Ghost Arrow**
| Move Points: -1
| Tactical snare inflicted by Ghost Arrow (Skill 79). Reduces movement allowance.
|===

## 5. Summoner & Environmental Grid States (Tier 3 LeTBS)

[cols="^1,2,^1,4", options="header"]
|===
| ID | State Name | Affected Stat | Tactical Mechanism & LeTBS Meta

| `78`
| **Transformation**
| Form Shift
| Active metamorphosis into astral beast (`character_name: Monster`).

| `79`
| **Fusion**
| Unit Merge
| Fuses summoner with tactical summon (`sprite_name: Rabbit`).

| `94`
| **Rabbit's Jump**
| Mobility
| Leap mechanics allowing clearance over terrain obstacles.

| `95`
| **Evil Protection**
| DEF/MDF Buff
| Damage shield cast by enemy warlocks (Skill 167).

| `109`
| **Tile Regen**
| Periodic Heal
| Restores health to units occupying consecrated ground.

| `110`
| **Burning**
| Fire Dot / Spread
| Fire hazard state (`spread: 100% circle(1)`). Expands dynamically to adjacent flammable tiles.
|===

## 6. Modern C11 Architectural Mapping

In the C11 simulation engine, tactical states are decoupled from engine internals:
1. **Zero-Heap State Mask**: Active states on an actor are tracked via a bitmask and static array (`active_states[MAX_ACTIVE_STATES]`).
2. **Move Point Clamping**: `move_points` modifiers directly feed into Layer 0 spatial pathing routines.
3. **Data-Driven Configuration**: State durations, stat multipliers, and tile spread rules are loaded from `data/states/*.json`.


---

## Chapter 10: Engine Systems, Time Clock & Switch/Variable Index

> **Source**: `/home/professorseanex/Tenebris and Lux/docs/game_design/demo_baseline/systems_switches_variables.adoc`

[abstract]
## Overview

This document specifies the global state machines, monotonic time clock, morality engine,
and the complete registry of all switches and variables governing demo logic.

## 1. The Monotonic Time Engine

The time system is driven by Common Events 1 and 2, updating a 24-hour cycle:
* `Variable 1`: Seconds (0–59)
* `Variable 2`: Minutes (0–59)
* `Variable 3`: Hours (1–12)
* `Variable 4`: 24-Hour Military Format (0–23)
* `Variable 5`: Days (1–7: Sunday through Saturday)
* `Variable 6`: Weeks
* `Variable 7`: Months
* `Variable 8`: Years

In the modern C11 architecture, this corresponds to Layer 0 `time_utils` (`time_get_monotonic_ms`).

## 2. The Moral Sway Continuum Engine

The morality engine tracks the party's ethical orientation on an invariant scale:
* `Variable 9`: **Moral (Overall)**: Continuum within `[-100, +100]`.
* `Variable 10`: **Moral (Good)**: Cumulative points awarded for altruism, charity, and mercy.
* `Variable 11`: **Moral (Evil)**: Cumulative points accrued for ruthless, self-serving, or thieving acts.
* `Item 8`: **Compass of Morality**: Tool allowing the player to inspect their moral continuum in the field.

## 3. Global Switches Directory

[cols="^1,2,4", options="header"]
|===
| Switch ID | Name / Flag | Operational Role & Trigger
| `1` | **Walk of Fear** | State flag for quest / scenario progression
| `2` | **Return to Cut Scene** | State flag for quest / scenario progression
| `3` | **Walk of Fear 2** | State flag for quest / scenario progression
| `4` | **Roté Enters the Castle** | State flag for quest / scenario progression
| `5` | **The Priest Leave the Castle** | State flag for quest / scenario progression
| `6` | **D. Jihad is Summoned** | State flag for quest / scenario progression
| `7` | **??? Enters the Map** | State flag for quest / scenario progression
| `8` | **Class Selected** | State flag for quest / scenario progression
| `9` | **??? Leaves the Map** | State flag for quest / scenario progression
| `10` | **??? Explains Puzzle** | State flag for quest / scenario progression
| `11` | **(Story Plot) Remove Rocks** | State flag for quest / scenario progression
| `12` | **Switch Active** | State flag for quest / scenario progression
| `13` | **(Side Plot) Remove Rocks** | State flag for quest / scenario progression
| `14` | **??? Disappears** | State flag for quest / scenario progression
| `15` | **??? Weaken** | State flag for quest / scenario progression
| `16` | **??? Final Words** | State flag for quest / scenario progression
| `17` | **Boss Defeated** | State flag for quest / scenario progression
| `18` | **Restart** | State flag for quest / scenario progression
| `19` | **Remove Stone 1** | State flag for quest / scenario progression
| `20` | **Remove Stone 2** | State flag for quest / scenario progression
| `21` | **Remove Stone 3** | State flag for quest / scenario progression
| `22` | **Remove Stone 4** | State flag for quest / scenario progression
| `23` | **Treasure Remove** | State flag for quest / scenario progression
| `24` | **Time** | State flag for quest / scenario progression
| `25` | **Friendship Menu** | State flag for quest / scenario progression
| `26` | **Quest 1 (Grandma's Request)** | State flag for quest / scenario progression
| `27` | **Grandma (Idle)** | State flag for quest / scenario progression
| `28` | **Exit Location** | State flag for quest / scenario progression
| `29` | **Monica Event On** | State flag for quest / scenario progression
| `30` | **Final Conversation Before Map is Out** | State flag for quest / scenario progression
| `31` | **Quest 1 (Give Grandma Here Watch)** | State flag for quest / scenario progression
| `32` | **The Great Sage** | State flag for quest / scenario progression
| `33` | **More Battle Mechanics** | State flag for quest / scenario progression
| `34` | **Monica Quest Switch** | State flag for quest / scenario progression
| `35` | **Unlock Dio Village-Lower** | State flag for quest / scenario progression
| `36` | **Idle Monica** | State flag for quest / scenario progression
| `37` | **Petty Thief** | State flag for quest / scenario progression
| `38` | **Morality** | State flag for quest / scenario progression
| `39` | **Give to the Poor** | State flag for quest / scenario progression
| `40` | **A Noble Thief** | State flag for quest / scenario progression
| `41` | **Angry John** | State flag for quest / scenario progression
|===

## 4. Global Variables Directory

[cols="^1,2,4", options="header"]
|===
| Variable ID | Name | Subsystem Allocation
| `1` | **Seconds** | Global numerical state accumulator
| `2` | **Minutes** | Global numerical state accumulator
| `3` | **Hours** | Global numerical state accumulator
| `4` | **24 Hours** | Global numerical state accumulator
| `5` | **Days** | Global numerical state accumulator
| `6` | **Weeks** | Global numerical state accumulator
| `7` | **Months** | Global numerical state accumulator
| `8` | **Years** | Global numerical state accumulator
| `9` | **Moral (Overall)** | Global numerical state accumulator
| `10` | **Moral (Good)** | Global numerical state accumulator
| `11` | **Moral (Evil)** | Global numerical state accumulator
| `12` | **Region ID** | Global numerical state accumulator
| `13` | **Currency** | Global numerical state accumulator
| `14` | **Pot Amount** | Global numerical state accumulator
| `15` | **Deposit Amount** | Global numerical state accumulator
|===


---

## Chapter 11: Historical Dataset Evolution & Delta Analysis

> **Source**: `/home/professorseanex/Tenebris and Lux/docs/game_design/demo_baseline/dataset_evolution_delta.adoc`

[abstract]
## Overview

This document provides a comparative delta between `Old Game Data` (Folder 1) and
`Old_Game_Data_2/data` (Folder 2), detailing the evolution from narrative prototype
to advanced LeTBS tactical simulation, and establishing modern C11 reconciliation strategies.

## 1. Dataset Characteristics & Metrics

[cols="2,^2,^2", options="header"]
|===
| Subsystem Metric | Folder 1 (Old Game Data) | Folder 2 (Old_Game_Data_2/data)
| Game Title | `Tenebris and Lux` | `Tenebris & Lux`
| Version Identifier | `40730002` | `2078310`
| Map Count | 36 maps | 43 maps
| Active Player Actors | 2 (Elijah, Katrina) | 5 (Elijah, Katrina, Sebastien, Angela, Lucius)
| Enemies Defined | 21 enemies | 22 enemies
| States Defined | 11 states | 30 states (extensive LeTBS configurations)
| Morality Engine Items | Compass of Morality, Ring of Fire | Omitted / shifted to LeTBS combat
|===

## 2. Core Differences & Evolution Strategy

1. **Narrative vs. Tactical Pivot**:
   - **Folder 1** represents the *narrative-complete* edition of the demo. It contains all dialogue branches, the Dio Village moral tests (Eric's Honesty Pot, John the Thief vs St. Baron), and key relics like the *Compass of Morality*.
   - **Folder 2** represents the *tactical engine expansion*. It introduces full party combat with Angela, Sebastien, and Lucius, 6 extra dungeon maps, and rich LeTBS states (e.g. *Burning Tile Spread*, *Charging*, *Intimidating Aura*).

2. **C11 Reconstruction Recommendation**:
   - **Layer 1 Simulation Engine**: Combine the narrative quest topology and moral continuum from Folder 1 with the rich 5-character tactical class abilities and LeTBS movement/scope geometry from Folder 2.
   - **Layer 2 Presentation**: Maintain the authentic dialog scripts, dual-protagonist branch voices, and environmental puzzles.


---

## Chapter 12: Game Design Curation & Triaging Matrix (2017–2018 Demo vs C11 Engine)

> **Source**: `/home/professorseanex/Tenebris and Lux/docs/game_design/demo_baseline/game_design_curation.adoc`

[abstract]
## Overview

This document formalizes the architectural triage of the 2017–2018 RPG Maker MV
demo datasets. It separates generic RPG Maker Runtime Package (RTP) boilerplate
from temporary testing experiments and identifies the authentic, fleshed-out
canonical systems to be reconstructed in the native C11 game engine.

## 1. Triage Categorization Taxonomy

To prevent technical debt from legacy game engine defaults from contaminating
our custom C11 engine, all extracted game data is triaged into three strict
analytical categories:

[cols="1,2,3", options="header"]
|===
| Category | Definition | Architectural Policy
| **Tier 1: RTP Defaults & Boilerplate** | Stock RPG Maker MV assets, generic items, placeholder sounds, and default formulas. | **Discard / Re-architect**. Never port verbatim into C11.
| **Tier 2: Prototype & Testing Sketches** | Ad-hoc test maps, sandbox battlers, debug switches, and experimental scripts. | **Review & Evolve**. Extract mechanical concepts but redesign within clean strata.
| **Tier 3: Fleshed-Out Canonical Pillars** | Core story scripts, dual-protagonist dialogue dynamics, moral dilemma quests, LeTBS grid tactics, and key relics. | **Preserve & Reconstruct**. Faithfully implement as axiomatic invariants in C11.
|===

## 2. Tier 1: RPG Maker RTP Defaults (To Discard or Re-Architect)

[cols="2,2,3", options="header"]
|===
| Element | Historical Manifest | Why It Must Be Discarded / Replaced
| **Basic Consumables** | Potion (50G), Magic Water (100G), Dispel Herb (20G), Stimulant (50G) | Generic RTP recovery items. In C11, consumables must tie directly to Sacyran herbalism, relic alchemy, and stance restoration.
| **Generic Armaments** | Standard Sword (100G), Axe (100G), Cane (100G), Bow (100G), Shield (100G), Cloth (100G) | Default placeholder inventory without distinct tactical affinities or stance interactions.
| **Stock Sound Effects** | `Attack2`, `Buzzer1`, `Damage4`, `Collapse1-4`, `Shop1` | Generic MV sound library. C11 audio subsystem will support custom soundscapes.
| **Placeholder Bestiary** | Bat (`HP: 80`), Slime (`HP: 250`), Rabbit (`HP: 55`), Orc (`HP: 300`) | Standard MV demo monsters used for basic stat checks. Replaced by Sacyran fauna and Abyssal void aberrations.
| **Default Title Theme** | `Theme6` / `CrossedSwords` title screen graphic | Default MV asset packaging. Replaced by bespoke Sacyran coat of arms and twin relic artwork.
|===

## 3. Tier 2: Prototype & Testing Sketches (Concepts to Evolve)

[cols="2,2,3", options="header"]
|===
| Element | Historical Location | Tactical / Design Value to Evolve
| **Sandbox Test Maps** | `Map006 Test Areas`, `Map043 Event Map` | Used to test event triggers, movement paths, and sprite coordinates. Demonstrates intent for environmental puzzle mechanics.
| **Debug Enemies & Troops** | `Enemy 14 Test`, `Troop 10 Test`, `Enemy 32 Invincible Neutral` | Proved that neutral, invincible obstacles and destructible props function well on the tactical grid.
| **Explosive Chickens** | Fire Chicken (`Actor 13`), Bomb (`Skill 136`) | Humorous prototype mechanic demonstrating combat summons and dynamic decoy units on the tactical field.
| **Plugin Experiments** | `LeTBS/firestorm.json`, `LeTBS/test.json`, `QMaps/` | Initial experiments with Lecode's Tactical Battle System and Quasi Movement plugins. Proves grid-based TRPG was the true target genre.
|===

## 4. Tier 3: Fleshed-Out Canonical Pillars (Core Invariants to Reconstruct)

The following systems represent the authentic creative vision of *Tenebris and Lux*
and must be preserved and reconstructed in C11:

### 1. Dual-Protagonist Dynamics & Narrative Branching
* **Elijah Ro-enheart (`\n[1]`)**: Son of vanished Commander Gareth Ro-enheart.
  Dutiful, protective vanguard bearer of the Shield of Lux. Dialogue reflects
  reverence for duty, family loyalty, and hesitation to violate laws.
* **Lady Katrina Jaren (`\n[2]`)**: Daughter of noble House Jaren escaping courtly
  politics to join the Adventurers' Guild. Agile outrider wielder of the Sword of
  Tenebris. Dialogue features sardonic wit, impatience with provincial stagnation,
  and fierce defense of the vulnerable.

### 2. The Moral Sway Continuum Engine
* **Ethical Mechanics**: Discrete moral choices tracking across `Moral (Good)`
  and `Moral (Evil)` within the invariant continuum `[-100, +100]`.
* **The Test of Honesty (Eric's House, Map 28)**:
  - Stealing Eric's life savings (10G) rewards quick cash but plunges morality.
  - Donating 500G to cover Eric's rent saves a desperate worker and earns the *Ring of Fire*.
* **The Struggle of Dio Village (John the Thief vs. St. Baron, Map 30/31)**:
  - Feudal exploitation vs. outrider rebellion, forcing the heroes to choose
    between legal compliance and moral justice.

### 3. Lecode Tactical Battle System (LeTBS) Mechanics
* **Grid Movement & Range Geometries**:
  - Movement Points (4 MP baseline), modified by skills like *Light Steps* (+1) and *Heavy Hit* (-3).
  - Scope shapes: `circle(r)`, `circle(max, min)` (annular), `line(n)`, `square(r)`.
* **Terrain Hazards & Obstacles**:
  - *Ice Wall* and *Ice Prison* creating physical destructible barriers (`HP: 20`).
  - *Volcano Ground* and *Burning Tile Spread* (`spread: 100% circle(1)`).
* **Turn Timeline Manipulation**:
  - *Accelerate*, *Decelerate*, and *Emergency Turn* altering the turn queue.

### 4. The Monotonic 24-Hour Time Engine
* **Grandma's Watch**: Family heirloom activating real-time calendar and clock
  tracking (Seconds, Minutes, Hours, Days, Weeks, Months, Years), corresponding
  directly to Layer 0 `time_utils`.


---

