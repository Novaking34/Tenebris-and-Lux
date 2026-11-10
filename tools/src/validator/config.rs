// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/validator/config.rs
// @id            tools.validator.config
// @layer         Layer 3: Systems Developer Tooling Core Library
// @version       0.1.0
// @interface_rev Rev 1
// @date          2026-11-09
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Data-driven configuration loader and rule merger for the OmniCode
//   validator. Implements layered resolution: loads generic protocol baselines
//   from `tools/config/` and overlays project-specific specializations from
//   `engine/config/standards/`.
// =============================================================================

//! # Validator Data-Driven Configuration Loader
//!
//! Merges generic toolchain defaults (`tools/config/`) with project-specific
//! engine standards (`engine/config/standards/`) into typed rule sets.

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES & STRUCTURES
// Stratum: Developer Tooling | Machine: tools.validator.config
// =============================================================================

use std::collections::HashMap;
use std::fs;
use std::path::Path;
use serde::{Deserialize, Serialize};

/// Archetype category of a C11 header.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum HeaderArchetype {
    /// Operational functional cog exposing APIs, audits, and dump cards.
    Operational,
    /// Statutory pure-type header defining static structs and inline predicates.
    PureTypes,
    /// Master aggregator facade umbrella header re-exporting modules.
    FacadeUmbrella,
    /// Quarantined zero-dependency emergency fallback cog.
    QuarantinedFallback,
}

impl Default for HeaderArchetype {
    fn default() -> Self {
        HeaderArchetype::Operational
    }
}

/// Architectural metadata record for a registered C11 header.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SubsystemRecord {
    pub subsystem: String,
    pub layer: usize,
    pub archetype: HeaderArchetype,
    pub stateful: bool,
}

/// Registry mapping header relative paths to their architectural records.
#[derive(Debug, Clone, Default)]
pub struct SubsystemRegistry {
    pub headers: HashMap<String, SubsystemRecord>,
}

/// Consolidated rules configuration for C header validation.
#[derive(Debug, Clone)]
pub struct HeaderRulesConfig {
    pub mandatory_metadata_tags: Vec<String>,
    pub mandatory_metadata_sections: Vec<String>,
    pub canonical_project: String,
    pub canonical_author: String,
    pub canonical_repository: String,
    pub banned_comment_tokens: Vec<String>,
    pub url_whitelists: Vec<String>,
    pub include_guard_prefix: String,
    pub dual_audience_sections: Vec<String>,
    pub audit_pattern: String,
    pub preflight_pattern: String,
    pub dump_card_suffix: String,
    pub stateless_exemption_literal: String,
    pub forbidden_domain_prefixes: Vec<String>,
}

impl Default for HeaderRulesConfig {
    fn default() -> Self {
        Self {
            mandatory_metadata_tags: vec![
                "@project".to_string(),
                "@file".to_string(),
                "@layer".to_string(),
                "@version".to_string(),
                "@interface_rev".to_string(),
                "@date".to_string(),
                "@author".to_string(),
                "@repository".to_string(),
                "@system".to_string(),
                "@role".to_string(),
                "@invariants".to_string(),
            ],
            mandatory_metadata_sections: vec![
                "PURPOSE & ARCHITECTURAL ROLE:".to_string(),
            ],
            canonical_project: "Tenebris and Lux".to_string(),
            canonical_author: "CreativeWorkzStudio LLC / ProfessorSeanEX".to_string(),
            canonical_repository: "https://github.com/Novaking34/tenebris-lux".to_string(),
            banned_comment_tokens: vec!["//".to_string()],
            url_whitelists: vec!["https://github.com/Novaking34/tenebris-lux".to_string()],
            include_guard_prefix: "TENEBRIS_LUX_".to_string(),
            dual_audience_sections: vec![
                "### For Game Designers & Non-Programmers:".to_string(),
                "### For Systems Programmers & Engine Architects:".to_string(),
            ],
            audit_pattern: "_audit".to_string(),
            preflight_pattern: "_preflight".to_string(),
            dump_card_suffix: "_execute_dump_card".to_string(),
            stateless_exemption_literal: "None. (Stateless module / pure stack-allocated execution).".to_string(),
            forbidden_domain_prefixes: vec![
                "hero/".to_string(),
                "combat/".to_string(),
                "world/".to_string(),
                "relics/".to_string(),
                "ending/".to_string(),
                "dispatch/".to_string(),
            ],
        }
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK / CONFIGURATION LOADER
// Stratum: Developer Tooling | Machine: tools.validator.config
// =============================================================================

/// Loads the subsystem registry from `engine/config/standards/subsystems.json`.
pub fn load_subsystems_registry(repo_root: &Path) -> Result<SubsystemRegistry, String> {
    let path = repo_root.join("engine/config/standards/subsystems.json");
    if !path.exists() {
        return Err(format!("Subsystems registry not found at {:?}", path));
    }

    let content = fs::read_to_string(&path)
        .map_err(|e| format!("Failed to read subsystems registry at {:?}: {}", path, e))?;

    let root: serde_json::Value = serde_json::from_str(&content)
        .map_err(|e| format!("Invalid JSON in subsystems registry at {:?}: {}", path, e))?;

    let headers_obj = root
        .get("block_3_body")
        .and_then(|b| b.get("headers"))
        .and_then(|h| h.as_object())
        .ok_or_else(|| "Missing block_3_body.headers in subsystems.json".to_string())?;

    let mut registry = SubsystemRegistry::default();

    for (header_path, record_val) in headers_obj {
        let record: SubsystemRecord = serde_json::from_value(record_val.clone())
            .map_err(|e| format!("Failed to parse subsystem record for '{}': {}", header_path, e))?;
        registry.headers.insert(header_path.clone(), record);
    }

    Ok(registry)
}

/// Loads and merges header rules from `tools/config/` and `engine/config/standards/`.
pub fn load_header_rules_config(repo_root: &Path) -> HeaderRulesConfig {
    let mut config = HeaderRulesConfig::default();

    // Layer 1: Check tools/config/blocks/block_1_metadata.json
    let tools_b1 = repo_root.join("tools/config/blocks/block_1_metadata.json");
    if let Ok(content) = fs::read_to_string(&tools_b1) {
        if let Ok(val) = serde_json::from_str::<serde_json::Value>(&content) {
            if let Some(tags) = val.pointer("/block_2_opening/mandatory_tags").and_then(|v| v.as_array()) {
                config.mandatory_metadata_tags = tags.iter()
                    .filter_map(|t| t.as_str().map(|s| s.to_string()))
                    .collect();
            }
        }
    }

    // Layer 2: Check engine/config/standards/blocks/block_5_closing.json
    let eng_b5 = repo_root.join("engine/config/standards/blocks/block_5_closing.json");
    if let Ok(content) = fs::read_to_string(&eng_b5) {
        if let Ok(val) = serde_json::from_str::<serde_json::Value>(&content) {
            if let Some(lit) = val.pointer("/block_3_body/stateless_exemption_literal").and_then(|v| v.as_str()) {
                config.stateless_exemption_literal = lit.to_string();
            }
        }
    }

    config
}
