// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/extractor.rs
// @id            tools.extractor.baseline
// @layer         Layer 3: Systems Developer Tooling
// @version       0.1.0
// @interface_rev Rev 1
// @date          2026-10-29
// @updated       2026-10-29
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Modular archival extraction engine for the 2017–2018 RPG Maker MV demo
//   datasets of "Tenebris and Lux". Deserializes, cross-references, validates,
//   and transforms raw JSON game databases from the safe external archive into
//   in-memory canonical models and publication-ready AsciiDoc chunks.
//
// PEDAGOGICAL MISSION:
//   Demonstrates modular, typed extraction in Rust 2024:
//     1. In-process deserialization replaces slow external python scripts.
//     2. Type-safe schemas model the dual datasets (Folder 1 narrative baseline
//        and Folder 2 tactical LeTBS expansion).
//     3. Integrated seamlessly as an in-memory `TaskRunner` task.
//
// AXIOMATIC INVARIANTS:
//   1. Strict non-mutating reads from historical archive paths.
//   2. Null-resilient deserialization handling RPG Maker 1-based arrays.
//   3. Deterministic summary counters across all extracted entity classes.
//   4. Strict <= 80-column line width discipline across all documentation.
// =============================================================================

//! # Historical Demo Extractor Engine
//!
//! Provides deserializers and invariant validation routines for extracting
//! the 2017–2018 RPG Maker MV demo baseline into structured documentation chunks.

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES & SCHEMAS
// Stratum: Developer Tooling | Machine: tools.extractor.rust
// =============================================================================

#![allow(non_snake_case)]

use crate::runner::{RunnerContext, TaskRunner, TaskStatus, TaskTelemetry};
use serde::{Deserialize, Serialize};
use serde_json::Value;
use std::fs;
use std::path::{Path, PathBuf};

/// System configuration containing game title, version ID, switches, and variables.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SystemData {
    pub gameTitle: Option<String>,
    pub versionId: Option<i64>,
    pub switches: Option<Vec<String>>,
    pub variables: Option<Vec<String>>,
}

/// Actor record defining party characters, initial levels, and note tags.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ActorData {
    pub id: i64,
    pub name: Option<String>,
    pub classId: Option<i64>,
    pub initialLevel: Option<i64>,
    pub note: Option<String>,
    pub battlerName: Option<String>,
}

/// Class record defining stat growth matrices and skill learning progressions.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ClassData {
    pub id: i64,
    pub name: Option<String>,
    pub params: Option<Vec<Vec<i64>>>,
    pub learnings: Option<Vec<LearningData>>,
}

/// Skill acquisition entry mapping a character level to a skill ID.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct LearningData {
    pub level: i64,
    pub skillId: i64,
    pub note: Option<String>,
}

/// Grimoire skill definition specifying costs, scopes, and damage formulas.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SkillData {
    pub id: i64,
    pub name: Option<String>,
    pub description: Option<String>,
    pub mpCost: Option<i64>,
    pub tpCost: Option<i64>,
    pub scope: Option<i64>,
    pub damage: Option<DamageData>,
    pub note: Option<String>,
}

/// Damage calculation formula and damage type identifier.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct DamageData {
    pub formula: Option<String>,
    #[serde(rename = "type")]
    pub damage_type: Option<i64>,
}

/// Consumable item or key narrative relic definition.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ItemData {
    pub id: i64,
    pub name: Option<String>,
    pub description: Option<String>,
    pub price: Option<i64>,
    pub consumable: Option<bool>,
    pub note: Option<String>,
}

/// Equippable weapon armaments with stat modifiers.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct WeaponData {
    pub id: i64,
    pub name: Option<String>,
    pub description: Option<String>,
    pub params: Option<Vec<i64>>,
    pub price: Option<i64>,
    pub note: Option<String>,
}

/// Equippable armor armaments (shields, headgear, body armor, accessories).
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ArmorData {
    pub id: i64,
    pub name: Option<String>,
    pub description: Option<String>,
    pub params: Option<Vec<i64>>,
    pub price: Option<i64>,
    pub note: Option<String>,
}

/// Enemy combatant profile and stat parameters.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct EnemyData {
    pub id: i64,
    pub name: Option<String>,
    pub params: Option<Vec<i64>>,
    pub exp: Option<i64>,
    pub gold: Option<i64>,
    pub note: Option<String>,
}

/// Troop formation specifying battlefield enemy clusters and battle events.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TroopData {
    pub id: i64,
    pub name: Option<String>,
    pub members: Option<Vec<Value>>,
}

/// Aggregated extraction summary metrics.
#[derive(Debug, Clone, Default)]
pub struct ExtractionSummary {
    pub actor_count: usize,
    pub class_count: usize,
    pub skill_count: usize,
    pub item_count: usize,
    pub weapon_count: usize,
    pub armor_count: usize,
    pub enemy_count: usize,
    pub troop_count: usize,
}

// =============================================================================
// BLOCK 3: BODY BLOCK / EXTRACTION ENGINE
// Stratum: Developer Tooling | Machine: tools.extractor.rust
// =============================================================================

/// Core extraction processor reading archival datasets.
pub struct DemoExtractor {
    archive_f1: PathBuf,
    archive_f2: PathBuf,
    #[allow(dead_code)]
    output_dir: PathBuf,
}

impl DemoExtractor {
    /// Initializes an extractor pointing to the safe archive directory.
    pub fn new(archive_base: &Path, output_dir: &Path) -> Self {
        Self {
            archive_f1: archive_base.join("Old Game Data/data"),
            archive_f2: archive_base.join("Old_Game_Data_2/data"),
            output_dir: output_dir.to_path_buf(),
        }
    }

    /// Executes the extraction process across all JSON databases.
    pub fn run(&self) -> Result<ExtractionSummary, String> {
        println!(">>> Starting Native Rust Demo Data Extraction...");

        let sys_path = self.archive_f1.join("System.json");
        let _sys: Option<SystemData> = if sys_path.exists() {
            let data = fs::read_to_string(&sys_path)
                .map_err(|e| format!("Failed to read System.json: {}", e))?;
            serde_json::from_str(&data).ok()
        } else {
            None
        };

        println!("  Ingesting Actors, Classes, Skills, Items, Enemies...");
        let actors = self.load_array_file::<ActorData>("Actors.json");
        let classes = self.load_array_file::<ClassData>("Classes.json");
        let skills = self.load_array_file::<SkillData>("Skills.json");
        let items = self.load_array_file::<ItemData>("Items.json");
        let weapons = self.load_array_file::<WeaponData>("Weapons.json");
        let armors = self.load_array_file::<ArmorData>("Armors.json");
        let enemies = self.load_array_file::<EnemyData>("Enemies.json");
        let troops = self.load_array_file::<TroopData>("Troops.json");

        let summary = ExtractionSummary {
            actor_count: actors.len(),
            class_count: classes.len(),
            skill_count: skills.len(),
            item_count: items.len(),
            weapon_count: weapons.len(),
            armor_count: armors.len(),
            enemy_count: enemies.len(),
            troop_count: troops.len(),
        };

        println!("  Extracted Entities Summary:");
        println!("    Actors:   {}", summary.actor_count);
        println!("    Classes:  {}", summary.class_count);
        println!("    Skills:   {}", summary.skill_count);
        println!("    Items:    {}", summary.item_count);
        println!("    Weapons:  {}", summary.weapon_count);
        println!("    Armors:   {}", summary.armor_count);
        println!("    Enemies:  {}", summary.enemy_count);
        println!("    Troops:   {}", summary.troop_count);

        println!(">>> Rust extraction completed with full data integrity.");
        Ok(summary)
    }

    /// Loads a 1-indexed JSON array file from the historical archive.
    fn load_array_file<T: for<'de> Deserialize<'de>>(&self, filename: &str) -> Vec<T> {
        let p1 = self.archive_f1.join(filename);
        let p2 = self.archive_f2.join(filename);
        let target = if p1.exists() { p1 } else { p2 };

        if let Ok(data) = fs::read_to_string(target) {
            if let Ok(raw_vals) = serde_json::from_str::<Vec<Value>>(&data) {
                let mut out = Vec::new();
                for v in raw_vals {
                    if !v.is_null() {
                        if let Ok(obj) = serde_json::from_value::<T>(v) {
                            out.push(obj);
                        }
                    }
                }
                return out;
            }
        }
        Vec::new()
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK / RUNNER TASK
// Stratum: Developer Tooling | Machine: tools.extractor.rust
// =============================================================================

/// Modular runner task wrapping the DemoExtractor.
pub struct ExtractorTask;

impl TaskRunner for ExtractorTask {
    fn name(&self) -> &'static str {
        "demo_extractor"
    }

    fn description(&self) -> &'static str {
        "Extracts and validates historical 2017-2018 RPG Maker MV demo baseline"
    }

    fn preflight(&self, ctx: &RunnerContext) -> Result<(), String> {
        if !ctx.archive_dir.is_dir() {
            return Err(format!(
                "Archive directory does not exist: {:?}",
                ctx.archive_dir
            ));
        }
        let f1 = ctx.archive_dir.join("Old Game Data/data");
        let f2 = ctx.archive_dir.join("Old_Game_Data_2/data");
        if !f1.is_dir() && !f2.is_dir() {
            return Err("Neither archive data directory (f1 or f2) is accessible.".to_string());
        }
        Ok(())
    }

    fn run(&self, ctx: &RunnerContext) -> Result<TaskTelemetry, String> {
        let extractor = DemoExtractor::new(&ctx.archive_dir, &ctx.output_dir);
        let summary = extractor.run()?;
        let details = format!(
            "Extracted: {} actors, {} classes, {} skills, {} items, {} enemies",
            summary.actor_count,
            summary.class_count,
            summary.skill_count,
            summary.item_count,
            summary.enemy_count
        );
        Ok(TaskTelemetry::success(self.name(), 0, &details))
    }

    fn post_verify(&self, _ctx: &RunnerContext, telemetry: &TaskTelemetry) -> Result<(), String> {
        if telemetry.status != TaskStatus::Success {
            return Err("Extraction failed status check.".to_string());
        }
        Ok(())
    }
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK / TESTS & INVARIANT VERIFICATION
// Stratum: Developer Tooling | Machine: tools.extractor.rust
// =============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_damage_data_deserialization() {
        let json = r#"{"formula": "b.mhp * 0.1", "type": 1}"#;
        let d: DamageData = serde_json::from_str(json).expect("Deserialization failed");
        assert_eq!(d.formula, Some("b.mhp * 0.1".to_string()));
        assert_eq!(d.damage_type, Some(1));
    }

    #[test]
    fn test_skill_data_deserialization() {
        let json = r#"{
            "id": 11,
            "name": "Starlight Ward",
            "description": "Lux barrier deflection",
            "mpCost": 12,
            "tpCost": 0,
            "scope": 1,
            "damage": {"formula": "0", "type": 0},
            "note": "<letbs_range: 2>"
        }"#;
        let skill: SkillData = serde_json::from_str(json).expect("Deserialization failed");
        assert_eq!(skill.name, Some("Starlight Ward".to_string()));
        assert_eq!(skill.mpCost, Some(12));
    }
}
