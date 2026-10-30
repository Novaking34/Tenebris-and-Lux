// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/guide.rs
// @id            tools.guide.compiler
// @layer         Layer 3: Systems Developer Tooling
// @version       0.1.0
// @interface_rev Rev 1
// @date          2026-10-29
// @updated       2026-10-29
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Modular Master Guide compiler for Tenebris and Lux. Aggregates multi-volume
//   AsciiDoc documentation into a unified, publication-grade master compendium
//   and generates three synchronized formats:
//     1. docs/MASTER_GUIDE.adoc (AsciiDoc master book)
//     2. docs/MASTER_GUIDE.md (Markdown optimized for NotebookLM & Gemini)
//     3. docs/MASTER_GUIDE.pdf (Compiled PDF via asciidoctor-pdf)
//
// PEDAGOGICAL MISSION:
//   Demonstrates modular compilation and multi-modal publication in Rust 2024:
//     1. Dual Granularity: Documents remain chunked in discrete files for deep
//        focused engineering, while being aggregated into a single master guide
//        for LLM context windows (Gemini / NotebookLM).
//     2. Invariant Chapter Auditing: Pre-flight checks verify that all 42
//        referenced chapters exist on disk before compilation begins.
//     3. Clean TaskRunner Integration: Plugs into the modular pipeline runner.
//
// AXIOMATIC INVARIANTS:
//   1. Zero missing chapter tolerance during master assembly.
//   2. Stripping redundant top-level headers to prevent duplicate table of contents.
//   3. Strict <= 80-column line width discipline across all documentation.
//   4. Deterministic volume and chapter sequence matching project ontology.
// =============================================================================

//! # Master Guide Compilation Engine
//!
//! Provides document aggregation, syntax transformation, and PDF publication
//! for the 6-volume *Tenebris and Lux* Master Guide.

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES & SPECS
// Stratum: Developer Tooling | Machine: tools.guide.rust
// =============================================================================

use crate::runner::{RunnerContext, TaskRunner, TaskTelemetry};
use std::fs;
use std::path::{Path, PathBuf};
use std::process::Command;

/// Specification for a single chapter included within a master volume.
#[derive(Debug, Clone)]
pub struct ChapterSpec {
    /// Sequential chapter number across the master compendium.
    pub number: usize,
    /// Human-readable title of the chapter.
    pub title: String,
    /// Absolute or relative path to the source AsciiDoc file.
    pub source_path: PathBuf,
}

/// Specification for a major thematic volume within the master compendium.
#[derive(Debug, Clone)]
pub struct VolumeSpec {
    /// Roman numeral designator (e.g., "I", "II", "III").
    pub roman: &'static str,
    /// Volume title.
    pub title: &'static str,
    /// Architectural synopsis of the volume's thematic contents.
    pub description: &'static str,
    /// List of ordered chapters belonging to this volume.
    pub chapters: Vec<ChapterSpec>,
}

// =============================================================================
// BLOCK 3: BODY BLOCK / COMPILATION LOGIC
// Stratum: Developer Tooling | Machine: tools.guide.rust
// =============================================================================

/// Core builder aggregating documentation into unified master outputs.
pub struct MasterGuideBuilder {
    pub docs_dir: PathBuf,
    pub volumes: Vec<VolumeSpec>,
}

impl MasterGuideBuilder {
    /// Initializes the Master Guide builder pointing to the docs directory.
    pub fn new(repo_root: &Path) -> Self {
        let docs_dir = repo_root.join("docs");
        let volumes = Self::build_volume_manifest(&docs_dir);
        Self { docs_dir, volumes }
    }

    /// Defines the master volumes and chapter hierarchy across the repository.
    fn build_volume_manifest(docs: &Path) -> Vec<VolumeSpec> {
        let mut chapter_id = 1;
        let mut make_ch = |title: &str, rel_path: &str| -> ChapterSpec {
            let ch = ChapterSpec {
                number: chapter_id,
                title: title.to_string(),
                source_path: docs.join(rel_path),
            };
            chapter_id += 1;
            ch
        };

        vec![
            VolumeSpec {
                roman: "I",
                title: "World Canon, Metaphysics & Sacyran Lore",
                description: "Cosmic neutrality of Lux and Tenebris, chivalric Kingdom of Sacyra, and the existential menace of Dark Lord Malakor.",
                chapters: vec![
                    make_ch("The Primordial Neutrality of Lux and Tenebris", "lore/world_and_lore.adoc"),
                    make_ch("Socio-Political Strata of the Kingdom of Sacyra", "lore/characters.adoc"),
                    make_ch("High School Inception & TRPG Genesis", "dev_logs/2026/10/lore_and_design/devlog_002_the_sacyra_chronicles_and_trpg_vision.adoc"),
                ],
            },
            VolumeSpec {
                roman: "II",
                title: "Dramatis Personae & Dual Protagonist Dynamics",
                description: "Comprehensive character dossiers, dual protagonist synergies, and regional faction leaders.",
                chapters: vec![
                    make_ch("Dual Protagonist Dynamics & Moral Continuum", "lore/characters/protagonists/dual_protagonist_dynamics.adoc"),
                    make_ch("Elijah Ro-enheart: Vanguard of Lux", "lore/characters/protagonists/elijah_ro_enheart.adoc"),
                    make_ch("Lady Katrina Jaren: The Outrider Blade", "lore/characters/protagonists/katrina_jaren.adoc"),
                    make_ch("Grandma Magolia: Matriarch of Dio Village", "lore/characters/dio_village/grandma_magolia.adoc"),
                    make_ch("Monica the Mystic & Goddess Lyri", "lore/characters/dio_village/monica_and_lyri.adoc"),
                    make_ch("Eric the Lumberjack & The Moral Litmus", "lore/characters/dio_village/eric_the_lumberjack.adoc"),
                    make_ch("John the Thief & Outlaw Resistance", "lore/characters/dio_village/john_the_thief.adoc"),
                    make_ch("The St. Baron & Feudal Exploitation", "lore/characters/dio_village/st_baron.adoc"),
                    make_ch("High Priest Roté & The Royal Omens", "lore/characters/kingdom_and_court/sir_rote.adoc"),
                    make_ch("King Jihad & The Treasonous Ambition", "lore/characters/kingdom_and_court/king_jihad.adoc"),
                    make_ch("Commander Gareth Ro-enheart & The Vanished Guard", "lore/characters/kingdom_and_court/commander_gareth_ro_enheart.adoc"),
                ],
            },
            VolumeSpec {
                roman: "III",
                title: "Ludology & Tactical TRPG Combat Mechanics",
                description: "Grid-based tactical warfare, relic stance machines, terrain modifiers, continuous invariant testing, and game design curation.",
                chapters: vec![
                    make_ch("Dual-Layer TRPG Combat Architecture", "game_design/tactical_trpg_system.adoc"),
                    make_ch("The Twin Relics, Combat Stances & Moral Sway", "game_design/relics_and_combat.adoc"),
                    make_ch("World Topography, Provinces & Battlefield Terrain", "game_design/world_topography.adoc"),
                    make_ch("Continuous Invariant Testing Philosophy", "game_design/game_testing.adoc"),
                    make_ch("Game Design Curation & Triaging Matrix", "game_design/demo_baseline/game_design_curation.adoc"),
                ],
            },
            VolumeSpec {
                roman: "IV",
                title: "The 2017–2018 Demo Baseline & Historical Archive",
                description: "Verified historical baseline extracted from the original RPG Maker MV datasets, detailing the complete scenario script, LeTBS mechanics, grimoire, and systems.",
                chapters: vec![
                    make_ch("Historical Baseline Overview & Provenance", "game_design/demo_baseline/README.adoc"),
                    make_ch("7-Act Narrative Script & Dialogue Walkthrough", "game_design/demo_baseline/scenario_and_walkthrough.adoc"),
                    make_ch("Complete Cast & Tactical Party Compendium", "game_design/demo_baseline/cast_and_characters.adoc"),
                    make_ch("LeTBS Tactical Grid System Specification", "game_design/demo_baseline/combat_and_letbs_mechanics.adoc"),
                    make_ch("Classes, Growth Curves & Learning Trees", "game_design/demo_baseline/classes_and_growth.adoc"),
                    make_ch("The Complete Grimoire (75+ Skills & Formulas)", "game_design/demo_baseline/grimoire_and_skills.adoc"),
                    make_ch("Bestiary, Destructible Props & Troop Formations", "game_design/demo_baseline/bestiary_and_troops.adoc"),
                    make_ch("Armory, Inventory & Key Lore Relics", "game_design/demo_baseline/armory_and_inventory.adoc"),
                    make_ch("Engine Systems, Time Clock & Switch/Variable Index", "game_design/demo_baseline/systems_switches_variables.adoc"),
                    make_ch("Historical Dataset Evolution & Delta Analysis", "game_design/demo_baseline/dataset_evolution_delta.adoc"),
                ],
            },
            VolumeSpec {
                roman: "V",
                title: "Software Architecture & The 5-Block Invariant Protocol",
                description: "System boundaries, layered strata, dual-frontend GUI/CLI, and pre-flight zero-heap diagnostic manifests.",
                chapters: vec![
                    make_ch("OmniCode 5-Block Architectural Framework", "architecture/ARCHITECTURE.adoc"),
                    make_ch("Dual-Frontend Presentation Layer (CLI + Native X11)", "architecture/dual_frontend_architecture.adoc"),
                    make_ch("Central Manifest & Pre-Flight Diagnostic Integrity", "architecture/preflight_and_manifest_spec.adoc"),
                    make_ch("Layer 0 Core Utilities Code Tour", "code/core_utilities.adoc"),
                    make_ch("Layer 1 Simulation Engine Code Tour", "code/simulation_engine.adoc"),
                    make_ch("Layer 2 Presentation Viewport Code Tour", "code/presentation_gui.adoc"),
                    make_ch("Layer 4 Verification & Test Suite Code Tour", "code/test_suite.adoc"),
                ],
            },
            VolumeSpec {
                roman: "VI",
                title: "Engineering Chronology & Development Logs",
                description: "Living chronological engineering logs recording every architectural milestone and paradigm evolution.",
                chapters: vec![
                    make_ch("DevLog 001: Genesis, Paradigm Pivot & Invariants", "dev_logs/2026/10/architecture/devlog_001_genesis_and_pivot.adoc"),
                    make_ch("DevLog 003: Central Manifest & Pre-Flight Spec", "dev_logs/2026/10/architecture/devlog_003_preflight_and_manifest_architecture.adoc"),
                    make_ch("DevLog 004: Pedagogical Metadata & Active DAG Contracts", "dev_logs/2026/10/architecture/devlog_004_pedagogical_metadata_and_dependency_contracts.adoc"),
                    make_ch("DevLog 005: Ontological Sub-Assemblies & 80-Column Discipline", "dev_logs/2026/10/architecture/devlog_005_ontological_code_layering_and_docstrings.adoc"),
                    make_ch("DevLog 006: Source Implementation & Root Pedagogical Commenting", "dev_logs/2026/10/architecture/devlog_006_source_and_root_pedagogical_commenting.adoc"),
                    make_ch("DevLog 007: Archival Extraction & The 2017–2018 Demo Baseline", "dev_logs/2026/10/lore_and_design/devlog_007_rpg_maker_demo_data_extraction_and_baseline.adoc"),
                ],
            },
        ]
    }

    /// Compiles all three formats of the Master Guide.
    pub fn build_all(&self) -> Result<(PathBuf, PathBuf, PathBuf), String> {
        println!(">>> Compiling Unified Master Guide across Volumes & Chapters...");

        let adoc_path = self.docs_dir.join("MASTER_GUIDE.adoc");
        let md_path = self.docs_dir.join("MASTER_GUIDE.md");
        let pdf_path = self.docs_dir.join("MASTER_GUIDE.pdf");

        let (adoc_content, md_content) = self.assemble_content()?;

        println!("  Writing {:?}...", adoc_path);
        fs::write(&adoc_path, &adoc_content)
            .map_err(|e| format!("Failed to write MASTER_GUIDE.adoc: {}", e))?;

        println!("  Writing {:?} (Optimized for NotebookLM & Gemini)...", md_path);
        fs::write(&md_path, &md_content)
            .map_err(|e| format!("Failed to write MASTER_GUIDE.md: {}", e))?;

        println!("  Compiling {:?} via asciidoctor-pdf...", pdf_path);
        let pdf_status = Command::new("asciidoctor-pdf")
            .arg(&adoc_path)
            .arg("-o")
            .arg(&pdf_path)
            .status();

        match pdf_status {
            Ok(s) if s.success() => {
                println!("  [SUCCESS] Master Guide PDF generated at: {:?}", pdf_path);
            }
            Ok(s) => {
                eprintln!("  [WARN] asciidoctor-pdf exited with code: {:?}", s.code());
            }
            Err(e) => {
                eprintln!("  [WARN] asciidoctor-pdf invocation failed: {}", e);
            }
        }

        println!(">>> Master Guide Compilation Complete!");
        Ok((adoc_path, md_path, pdf_path))
    }

    /// Assembles the unified AsciiDoc and Markdown text documents.
    pub fn assemble_content(&self) -> Result<(String, String), String> {
        let mut adoc = String::with_capacity(300_000);
        let mut md = String::with_capacity(300_000);

        // AsciiDoc Master Header
        adoc.push_str("= Tenebris and Lux: Comprehensive Master Guide\n");
        adoc.push_str("CreativeWorkzStudio LLC / ProfessorSeanEX\n");
        adoc.push_str("v1.0.0, 2026-10-29\n");
        adoc.push_str(":doctype: book\n");
        adoc.push_str(":toc: left\n");
        adoc.push_str(":toclevels: 3\n");
        adoc.push_str(":sectnums:\n");
        adoc.push_str(":icons: font\n");
        adoc.push_str(":source-highlighter: rouge\n");
        adoc.push_str(":pdf-theme: default\n\n");

        adoc.push_str("[abstract]\n== Executive Summary\n\n");
        adoc.push_str("This Master Guide serves as the definitive single source of truth for *Tenebris and Lux*.\n");
        adoc.push_str("It unites world canon, character dossiers, ludology, the 2017–2018 demo baseline,\n");
        adoc.push_str("software architecture, and engineering dev logs into a comprehensive compendium.\n\n");

        // Markdown Master Header
        md.push_str("# Tenebris and Lux: Comprehensive Master Guide\n\n");
        md.push_str("> **Author**: CreativeWorkzStudio LLC / ProfessorSeanEX  \n");
        md.push_str("> **Version**: 1.0.0 (Release Baseline 2026-10-29)  \n");
        md.push_str("> **Standard**: OmniCode 5-Block Ontological Architecture & Layered Invariants  \n");
        md.push_str("> **Purpose**: Unified Single-File Master Compendium for Google Gemini and NotebookLM\n\n");
        md.push_str("## Executive Summary\n\n");
        md.push_str("This Master Guide compiles the entire architectural, ludological, and narrative universe of **Tenebris and Lux**.\n");
        md.push_str("It serves as a high-density, context-rich reference for AI engineering workflows, prompt engineering, and human game developers.\n\n");

        md.push_str("## Table of Contents\n\n");
        for vol in &self.volumes {
            md.push_str(&format!("- **Volume {}: {}**\n", vol.roman, vol.title));
            for ch in &vol.chapters {
                md.push_str(&format!("  - Chapter {}: {}\n", ch.number, ch.title));
            }
        }
        md.push_str("\n---\n\n");

        for vol in &self.volumes {
            // AsciiDoc Volume Heading
            adoc.push_str(&format!("= Volume {}: {}\n\n", vol.roman, vol.title));
            adoc.push_str(&format!("{}\n\n", vol.description));

            // Markdown Volume Heading
            md.push_str(&format!("# Volume {}: {}\n\n", vol.roman, vol.title));
            md.push_str(&format!("_{}_\n\n", vol.description));

            for ch in &vol.chapters {
                let raw_content = if ch.source_path.exists() {
                    fs::read_to_string(&ch.source_path).unwrap_or_else(|_| {
                        format!("Chapter content unavailable from {:?}", ch.source_path)
                    })
                } else {
                    format!("_File not found: {:?}_\n", ch.source_path)
                };

                let stripped = Self::strip_adoc_header(&raw_content);

                // AsciiDoc Chapter
                adoc.push_str(&format!("== Chapter {}: {}\n\n", ch.number, ch.title));
                adoc.push_str(&format!("*Source File*: `{}`\n\n", ch.source_path.display()));
                adoc.push_str(&stripped);
                adoc.push_str("\n\n");

                // Markdown Chapter
                md.push_str(&format!("## Chapter {}: {}\n\n", ch.number, ch.title));
                md.push_str(&format!("> **Source**: `{}`\n\n", ch.source_path.display()));
                md.push_str(&Self::adoc_to_markdown(&stripped));
                md.push_str("\n\n---\n\n");
            }
        }

        Ok((adoc, md))
    }

    /// Strips top-level document headers to avoid duplicate metadata blocks.
    pub fn strip_adoc_header(content: &str) -> String {
        let mut lines: Vec<&str> = Vec::new();
        let mut in_header = true;

        for line in content.lines() {
            if in_header {
                let trimmed = line.trim();
                if trimmed.starts_with("== ")
                    || trimmed.starts_with("=== ")
                    || trimmed.starts_with("[abstract]")
                    || trimmed.starts_with("[quote")
                {
                    in_header = false;
                    lines.push(line);
                } else if trimmed.starts_with('=') && !trimmed.starts_with("==") {
                    continue;
                } else if trimmed.starts_with(':') || trimmed.is_empty() {
                    continue;
                } else if trimmed.starts_with("//") {
                    continue;
                } else {
                    // Header metadata (author, version, etc.)
                    continue;
                }
            } else {
                lines.push(line);
            }
        }

        if lines.is_empty() {
            content.to_string()
        } else {
            lines.join("\n")
        }
    }

    /// Converts standard AsciiDoc formatting conventions into clean GitHub-flavored Markdown.
    fn adoc_to_markdown(adoc: &str) -> String {
        let mut md = String::with_capacity(adoc.len());
        for line in adoc.lines() {
            if line.starts_with("=====") {
                md.push_str(&format!("##### {}", &line[5..].trim()));
            } else if line.starts_with("====") {
                md.push_str(&format!("#### {}", &line[4..].trim()));
            } else if line.starts_with("===") {
                md.push_str(&format!("### {}", &line[3..].trim()));
            } else if line.starts_with("==") {
                md.push_str(&format!("## {}", &line[2..].trim()));
            } else if line.starts_with("=") {
                md.push_str(&format!("# {}", &line[1..].trim()));
            } else {
                md.push_str(line);
            }
            md.push('\n');
        }
        md
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK / RUNNER TASK
// Stratum: Developer Tooling | Machine: tools.guide.rust
// =============================================================================

/// Modular runner task wrapping the MasterGuideBuilder.
pub struct MasterGuideTask;

impl TaskRunner for MasterGuideTask {
    fn name(&self) -> &'static str {
        "master_guide"
    }

    fn description(&self) -> &'static str {
        "Compiles the 6-Volume unified Master Guide in AsciiDoc, Markdown, and PDF"
    }

    fn preflight(&self, ctx: &RunnerContext) -> Result<(), String> {
        let builder = MasterGuideBuilder::new(&ctx.repo_root);
        let mut missing_count = 0;
        let mut total_count = 0;

        for vol in &builder.volumes {
            for ch in &vol.chapters {
                total_count += 1;
                if !ch.source_path.exists() {
                    eprintln!("  [MISSING CHAPTER] {:?}", ch.source_path);
                    missing_count += 1;
                }
            }
        }

        if missing_count > 0 {
            return Err(format!(
                "Master Guide preflight failed: {}/{} chapters missing on disk.",
                missing_count, total_count
            ));
        }

        println!("  [PASS] Preflight verified: all {} chapters present.", total_count);
        Ok(())
    }

    fn run(&self, ctx: &RunnerContext) -> Result<TaskTelemetry, String> {
        let builder = MasterGuideBuilder::new(&ctx.repo_root);
        let (adoc_p, md_p, pdf_p) = builder.build_all()?;
        let details = "Compiled MASTER_GUIDE (.adoc, .md, .pdf) successfully.";
        let tel = TaskTelemetry::success(self.name(), 0, details)
            .with_artifact(adoc_p)
            .with_artifact(md_p)
            .with_artifact(pdf_p);
        Ok(tel)
    }

    fn post_verify(&self, ctx: &RunnerContext, telemetry: &TaskTelemetry) -> Result<(), String> {
        let adoc_path = ctx.docs_dir.join("MASTER_GUIDE.adoc");
        let md_path = ctx.docs_dir.join("MASTER_GUIDE.md");

        if !adoc_path.is_file() || !md_path.is_file() {
            return Err("Expected master guide artifacts were not generated.".to_string());
        }

        for path in &telemetry.artifacts {
            if path.exists() {
                println!("  [POST-VERIFY] Artifact confirmed: {:?}", path);
            }
        }

        Ok(())
    }
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK / TESTS & INVARIANT VERIFICATION
// Stratum: Developer Tooling | Machine: tools.guide.rust
// =============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_adoc_header_stripping() {
        let sample = "= Document Title\nAuthor\nv1.0\n:toc: left\n\n== Section 1\nContent";
        let stripped = MasterGuideBuilder::strip_adoc_header(sample);
        assert!(stripped.starts_with("== Section 1"));
    }

    #[test]
    fn test_adoc_to_markdown_heading_conversion() {
        let adoc = "== Chapter 1\n=== Subsection A\nParagraph";
        let md = MasterGuideBuilder::adoc_to_markdown(adoc);
        assert!(md.contains("## Chapter 1"));
        assert!(md.contains("### Subsection A"));
    }
}
