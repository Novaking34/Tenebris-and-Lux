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
    #[serde(default)]
    pub id: Option<String>,
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
    pub recommended_metadata_tags: Vec<String>,
    pub ordered_metadata_tags: Vec<String>,
    pub growth_tags: Vec<String>,
    pub strict_template_domains: Vec<String>,
    pub maturity_taxonomy: Vec<String>,
    pub mandatory_metadata_sections: Vec<String>,
    pub canonical_project: String,
    pub canonical_author: String,
    pub canonical_repository: String,
    pub block_1_title: String,
    pub block_1_subtitle: String,
    pub banner_rule: String,
    pub tag_column_width: usize,
    pub enforce_template_alignment: bool,
    pub enforce_tag_order: bool,
    pub validate_paired_c_sources: bool,
    pub header_template_lines: Vec<String>,
    pub source_template_lines: Vec<String>,
    pub engine_name: String,
    pub strict_include_guard_prefix: String,
    pub strict_block_2_domains: Vec<String>,
    pub forbid_2_5_in_headers: bool,
    pub require_2_5_pattern_watch: bool,
    pub warn_non_inline_in_2_4: bool,
    pub require_tier_headers: bool,
    pub require_extern_c_in_headers: bool,
    pub enforce_matching_endif: bool,
    pub enforce_quarantined_fallback_headers: bool,
    pub verify_monotonic_sub_assemblies: bool,
    pub verify_posix_macro_placement: bool,
    pub banned_comment_tokens: Vec<String>,
    pub url_whitelists: Vec<String>,
    pub include_guard_prefix: String,
    pub dual_audience_sections: Vec<String>,
    pub audit_pattern: String,
    pub preflight_pattern: String,
    pub dump_card_suffix: String,
    pub lifecycle_boot_suffix: String,
    pub stateless_exemption_literal: String,
    pub forbidden_domain_prefixes: Vec<String>,
    pub strict_block_3_domains: Vec<String>,
    pub block_3_title: String,
    pub block_3_required_banner_fields: Vec<String>,
    pub pure_types_statutory_text: String,
    pub facade_umbrella_statutory_text: String,
    pub archetype_b3_subtitles: HashMap<String, String>,
    pub triggers_cross_reference_tag: String,
    pub ban_macros_in_block_3: bool,
    pub forbid_static_in_block_3: bool,
    pub forbid_lifecycle_and_procedures_in_block_3: bool,
    pub enforce_sub_assembly_symbol_parity: bool,
    pub require_per_fn_doxygen_in_block_3: bool,
    pub require_source_reconstructible_docs: bool,
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
            recommended_metadata_tags: vec![
                "@id".to_string(),
                "@archetype".to_string(),
                "@maturity".to_string(),
                "@chassis".to_string(),
                "@scale".to_string(),
                "@seams".to_string(),
            ],
            ordered_metadata_tags: vec![
                "@project".to_string(),
                "@file".to_string(),
                "@id".to_string(),
                "@layer".to_string(),
                "@archetype".to_string(),
                "@version".to_string(),
                "@interface_rev".to_string(),
                "@date".to_string(),
                "@author".to_string(),
                "@repository".to_string(),
                "@system".to_string(),
                "@role".to_string(),
                "@maturity".to_string(),
                "@chassis".to_string(),
                "@scale".to_string(),
                "@seams".to_string(),
                "@invariants".to_string(),
            ],
            growth_tags: vec![
                "@id".to_string(),
                "@archetype".to_string(),
                "@maturity".to_string(),
                "@chassis".to_string(),
                "@scale".to_string(),
                "@seams".to_string(),
            ],
            strict_template_domains: vec![
                "log".to_string(),
                "debug".to_string(),
                "io".to_string(),
            ],
            maturity_taxonomy: vec![
                "Bootstrap".to_string(),
                "Hardening".to_string(),
                "Rev 5 Baseline".to_string(),
                "Frozen".to_string(),
            ],
            mandatory_metadata_sections: vec![
                "PURPOSE & ARCHITECTURAL ROLE:".to_string(),
            ],
            canonical_project: "Tenebris and Lux".to_string(),
            canonical_author: "CreativeWorkzStudio LLC / ProfessorSeanEX".to_string(),
            canonical_repository: "https://github.com/Novaking34/tenebris-lux".to_string(),
            block_1_title: "BLOCK 1: METADATA BLOCK".to_string(),
            block_1_subtitle: "( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )".to_string(),
            banner_rule: "=".repeat(80),
            tag_column_width: 15,
            enforce_template_alignment: true,
            enforce_tag_order: true,
            validate_paired_c_sources: true,
            header_template_lines: Vec::new(),
            source_template_lines: Vec::new(),
            engine_name: "Cornerstone Game Engine".to_string(),
            strict_include_guard_prefix: "CORNERSTONE_".to_string(),
            strict_block_2_domains: vec![
                "log".to_string(),
                "debug".to_string(),
                "io".to_string(),
            ],
            forbid_2_5_in_headers: true,
            require_2_5_pattern_watch: true,
            warn_non_inline_in_2_4: true,
            require_tier_headers: true,
            require_extern_c_in_headers: true,
            enforce_matching_endif: true,
            enforce_quarantined_fallback_headers: true,
            verify_monotonic_sub_assemblies: true,
            verify_posix_macro_placement: true,
            banned_comment_tokens: vec!["//".to_string()],
            url_whitelists: vec![
                "https://github.com/Novaking34/tenebris-lux".to_string(),
                "rom://".to_string(),
            ],
            include_guard_prefix: "TENEBRIS_LUX_".to_string(),
            dual_audience_sections: vec![
                "### For Game Designers & Non-Programmers:".to_string(),
                "### For Systems Programmers & Engine Architects:".to_string(),
            ],
            audit_pattern: "_audit".to_string(),
            preflight_pattern: "_preflight".to_string(),
            dump_card_suffix: "_execute_dump_card".to_string(),
            lifecycle_boot_suffix: "_boot".to_string(),
            stateless_exemption_literal: "None. (Stateless module / pure stack-allocated execution).".to_string(),
            forbidden_domain_prefixes: vec![
                "hero/".to_string(),
                "combat/".to_string(),
                "world/".to_string(),
                "relics/".to_string(),
                "ending/".to_string(),
                "dispatch/".to_string(),
            ],
            strict_block_3_domains: vec![
                "log".to_string(),
                "debug".to_string(),
                "io".to_string(),
            ],
            block_3_title: "BLOCK 3: BODY BLOCK".to_string(),
            block_3_required_banner_fields: vec![
                "Engine:".to_string(),
                "Stratum:".to_string(),
                "Module:".to_string(),
                "Mission:".to_string(),
            ],
            pure_types_statutory_text: "define purely static structures".to_string(),
            facade_umbrella_statutory_text: "all functional APIs are declared in their respective domain headers".to_string(),
            archetype_b3_subtitles: HashMap::from([
                ("universal_fallback_header".to_string(), "( Composable Business Logic & Core Functional Cogs )".to_string()),
                ("universal_fallback_source".to_string(), "( Composable Business Logic & Implementation Engine )".to_string()),
                ("operational_header".to_string(), "( Public Operational Cogs, State Accessors & Domain APIs )".to_string()),
                ("quarantined_fallback_header".to_string(), "( Tier-0 Safe-Mode BIOS Cogs, ROM Vault Queries & Fault Recovery )".to_string()),
                ("facade_umbrella_header".to_string(), "( Subsystem API Aggregation & Statutory Delegation Notice )".to_string()),
                ("pure_types_header".to_string(), "( Statutory Pure-Types Notice & Structural Boundary )".to_string()),
                ("operational_source".to_string(), "( Operational Business Logic, Span Scopes & Runtime Hydration )".to_string()),
                ("quarantined_fallback_source".to_string(), "( Safe-Mode BIOS Implementation, .rodata ROM Hydration & Fault Recording )".to_string()),
            ]),
            triggers_cross_reference_tag: "@triggers".to_string(),
            ban_macros_in_block_3: true,
            forbid_static_in_block_3: true,
            forbid_lifecycle_and_procedures_in_block_3: true,
            enforce_sub_assembly_symbol_parity: true,
            require_per_fn_doxygen_in_block_3: true,
            require_source_reconstructible_docs: true,
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

    let parse_str_array = |val: &serde_json::Value, ptr: &str| -> Option<Vec<String>> {
        val.pointer(ptr).and_then(|v| v.as_array()).map(|arr| {
            arr.iter()
                .filter_map(|s| s.as_str().map(|x| x.to_string()))
                .collect()
        })
    };

    // Layer 1: Check tools/config/blocks/block_1_metadata.json
    let tools_b1 = repo_root.join("tools/config/blocks/block_1_metadata.json");
    if let Ok(content) = fs::read_to_string(&tools_b1) {
        if let Ok(val) = serde_json::from_str::<serde_json::Value>(&content) {
            if let Some(tags) = parse_str_array(&val, "/block_2_opening/mandatory_tags") {
                if !tags.is_empty() {
                    config.mandatory_metadata_tags = tags;
                }
            }
            if let Some(tags) = parse_str_array(&val, "/block_2_opening/recommended_tags") {
                if !tags.is_empty() {
                    config.recommended_metadata_tags = tags;
                }
            }
            if let Some(secs) = parse_str_array(&val, "/block_2_opening/mandatory_sections") {
                if !secs.is_empty() {
                    config.mandatory_metadata_sections = secs;
                }
            }
            if let Some(secs) = parse_str_array(&val, "/block_2_opening/recommended_sections") {
                if !secs.is_empty() {
                    config.dual_audience_sections = secs;
                }
            }
            if let Some(v) = val.pointer("/block_3_body/enforce_template_alignment").and_then(|v| v.as_bool()) {
                config.enforce_template_alignment = v;
            }
            if let Some(v) = val.pointer("/block_3_body/enforce_tag_order").and_then(|v| v.as_bool()) {
                config.enforce_tag_order = v;
            }
            if let Some(v) = val.pointer("/block_3_body/validate_paired_c_sources").and_then(|v| v.as_bool()) {
                config.validate_paired_c_sources = v;
            }
        }
    }

    // Layer 2: Check engine/config/standards/blocks/block_1_metadata.json
    let eng_b1 = repo_root.join("engine/config/standards/blocks/block_1_metadata.json");
    if let Ok(content) = fs::read_to_string(&eng_b1) {
        if let Ok(val) = serde_json::from_str::<serde_json::Value>(&content) {
            if let Some(s) = val.pointer("/block_2_opening/static_declarations/project_name_expected").and_then(|v| v.as_str()) {
                config.canonical_project = s.to_string();
            }
            if let Some(s) = val.pointer("/block_2_opening/static_declarations/canonical_author_expected").and_then(|v| v.as_str()) {
                config.canonical_author = s.to_string();
            }
            if let Some(s) = val.pointer("/block_2_opening/static_declarations/canonical_repository_expected").and_then(|v| v.as_str()) {
                config.canonical_repository = s.to_string();
            }
            if let Some(s) = val.pointer("/block_2_opening/static_declarations/banner_rule").and_then(|v| v.as_str()) {
                config.banner_rule = s.to_string();
            }
            if let Some(s) = val.pointer("/block_2_opening/static_declarations/block_1_title").and_then(|v| v.as_str()) {
                config.block_1_title = s.to_string();
            }
            if let Some(s) = val.pointer("/block_2_opening/static_declarations/block_1_subtitle").and_then(|v| v.as_str()) {
                config.block_1_subtitle = s.to_string();
            }
            if let Some(w) = val.pointer("/block_2_opening/static_declarations/tag_column_width").and_then(|v| v.as_u64()) {
                config.tag_column_width = w as usize;
            }
            if let Some(urls) = parse_str_array(&val, "/block_3_body/url_whitelist_for_double_slash") {
                if !urls.is_empty() {
                    config.url_whitelists = urls;
                }
            }
            if let Some(tags) = parse_str_array(&val, "/block_3_body/ordered_tags") {
                if !tags.is_empty() {
                    config.ordered_metadata_tags = tags;
                }
            }
            if let Some(tags) = parse_str_array(&val, "/block_3_body/growth_tags") {
                if !tags.is_empty() {
                    config.growth_tags = tags;
                }
            }
            if let Some(doms) = parse_str_array(&val, "/block_3_body/strict_template_domains") {
                config.strict_template_domains = doms;
            }
            if let Some(mat) = parse_str_array(&val, "/block_3_body/maturity_taxonomy") {
                if !mat.is_empty() {
                    config.maturity_taxonomy = mat;
                }
            }
            if let Some(hdr_tpl) = parse_str_array(&val, "/block_3_body/c_block_1_template/header") {
                config.header_template_lines = hdr_tpl;
            }
            if let Some(src_tpl) = parse_str_array(&val, "/block_3_body/c_block_1_template/source") {
                config.source_template_lines = src_tpl;
            }
        }
    }

    // Layer 1: Check tools/config/blocks/block_2_opening.json
    let tools_b2 = repo_root.join("tools/config/blocks/block_2_opening.json");
    if let Ok(content) = fs::read_to_string(&tools_b2) {
        if let Ok(val) = serde_json::from_str::<serde_json::Value>(&content) {
            if let Some(v) = val.pointer("/block_3_body/forbid_2_5_in_headers").and_then(|v| v.as_bool()) {
                config.forbid_2_5_in_headers = v;
            }
            if let Some(v) = val.pointer("/block_3_body/require_2_5_pattern_watch").and_then(|v| v.as_bool()) {
                config.require_2_5_pattern_watch = v;
            }
            if let Some(v) = val.pointer("/block_3_body/warn_non_inline_in_2_4").and_then(|v| v.as_bool()) {
                config.warn_non_inline_in_2_4 = v;
            }
            if let Some(v) = val.pointer("/block_3_body/require_tier_headers").and_then(|v| v.as_bool()) {
                config.require_tier_headers = v;
            }
            if let Some(v) = val.pointer("/block_3_body/require_extern_c_in_headers").and_then(|v| v.as_bool()) {
                config.require_extern_c_in_headers = v;
            }
            if let Some(v) = val.pointer("/block_3_body/enforce_matching_endif").and_then(|v| v.as_bool()) {
                config.enforce_matching_endif = v;
            }
            if let Some(v) = val.pointer("/block_3_body/enforce_quarantined_fallback_headers").and_then(|v| v.as_bool()) {
                config.enforce_quarantined_fallback_headers = v;
            }
            if let Some(v) = val.pointer("/block_3_body/validate_paired_c_sources").and_then(|v| v.as_bool()) {
                config.validate_paired_c_sources = v;
            }
            if let Some(v) = val.pointer("/block_4_audit/verify_monotonic_sub_assemblies").and_then(|v| v.as_bool()) {
                config.verify_monotonic_sub_assemblies = v;
            }
            if let Some(v) = val.pointer("/block_4_audit/verify_posix_macro_placement").and_then(|v| v.as_bool()) {
                config.verify_posix_macro_placement = v;
            }
        }
    }

    // Layer 2: Check engine/config/standards/blocks/block_2_opening.json
    let eng_b2 = repo_root.join("engine/config/standards/blocks/block_2_opening.json");
    if let Ok(content) = fs::read_to_string(&eng_b2) {
        if let Ok(val) = serde_json::from_str::<serde_json::Value>(&content) {
            if let Some(s) = val.pointer("/block_2_opening/pre_conditions/engine_display_name").and_then(|v| v.as_str()) {
                config.engine_name = s.to_string();
            }
            if let Some(s) = val.pointer("/block_2_opening/static_declarations/include_guard_prefix").and_then(|v| v.as_str()) {
                config.strict_include_guard_prefix = s.to_string();
            }
            if let Some(s) = val.pointer("/block_2_opening/static_declarations/legacy_include_guard_prefix").and_then(|v| v.as_str()) {
                config.include_guard_prefix = s.to_string();
            }
            if let Some(doms) = parse_str_array(&val, "/block_2_opening/static_declarations/strict_template_domains") {
                config.strict_block_2_domains = doms;
            }
            if let Some(v) = val.pointer("/block_2_opening/static_declarations/require_extern_c_in_headers").and_then(|v| v.as_bool()) {
                config.require_extern_c_in_headers = v;
            }
            if let Some(v) = val.pointer("/block_2_opening/static_declarations/require_tier_headers").and_then(|v| v.as_bool()) {
                config.require_tier_headers = v;
            }
            if let Some(v) = val.pointer("/block_2_opening/static_declarations/enforce_matching_endif_comment").and_then(|v| v.as_bool()) {
                config.enforce_matching_endif = v;
            }
            if let Some(v) = val.pointer("/block_3_body/sub_assembly_2_5_contract/forbid_in_headers").and_then(|v| v.as_bool()) {
                config.forbid_2_5_in_headers = v;
            }
            if let Some(v) = val.pointer("/block_3_body/sub_assembly_2_5_contract/require_pattern_watch").and_then(|v| v.as_bool()) {
                config.require_2_5_pattern_watch = v;
            }
            if let Some(prefixes) = parse_str_array(&val, "/block_3_body/anti_domain_bleed/forbidden_path_prefixes") {
                if !prefixes.is_empty() {
                    config.forbidden_domain_prefixes = prefixes;
                }
            }
        }
    }

    // Layer 1: Check tools/config/blocks/block_3_body.json
    let tools_b3 = repo_root.join("tools/config/blocks/block_3_body.json");
    if let Ok(content) = fs::read_to_string(&tools_b3) {
        if let Ok(val) = serde_json::from_str::<serde_json::Value>(&content) {
            if let Some(s) = val.pointer("/block_2_opening/block_marker").and_then(|v| v.as_str()) {
                config.block_3_title = s.to_string();
            }
            if let Some(fields) = parse_str_array(&val, "/block_2_opening/required_banner_fields") {
                config.block_3_required_banner_fields = fields;
            }
            if let Some(s) = val.pointer("/block_3_body/triggers_tag").and_then(|v| v.as_str()) {
                config.triggers_cross_reference_tag = s.to_string();
            }
            if let Some(v) = val.pointer("/block_3_body/ban_macros_in_block_3").and_then(|v| v.as_bool()) {
                config.ban_macros_in_block_3 = v;
            }
            if let Some(v) = val.pointer("/block_3_body/forbid_static_in_block_3").and_then(|v| v.as_bool()) {
                config.forbid_static_in_block_3 = v;
            }
            if let Some(v) = val.pointer("/block_3_body/forbid_lifecycle_and_procedures_in_block_3").and_then(|v| v.as_bool()) {
                config.forbid_lifecycle_and_procedures_in_block_3 = v;
            }
            if let Some(v) = val.pointer("/block_3_body/enforce_sub_assembly_symbol_parity").and_then(|v| v.as_bool()) {
                config.enforce_sub_assembly_symbol_parity = v;
            }
            if let Some(v) = val.pointer("/block_3_body/require_per_fn_doxygen_in_block_3").and_then(|v| v.as_bool()) {
                config.require_per_fn_doxygen_in_block_3 = v;
            }
            if let Some(v) = val.pointer("/block_3_body/require_source_reconstructible_docs").and_then(|v| v.as_bool()) {
                config.require_source_reconstructible_docs = v;
            }
        }
    }

    // Layer 2: Check engine/config/standards/blocks/block_3_body.json
    let eng_b3 = repo_root.join("engine/config/standards/blocks/block_3_body.json");
    if let Ok(content) = fs::read_to_string(&eng_b3) {
        if let Ok(val) = serde_json::from_str::<serde_json::Value>(&content) {
            if let Some(doms) = parse_str_array(&val, "/block_2_opening/static_declarations/strict_template_domains") {
                config.strict_block_3_domains = doms;
            }
            if let Some(s) = val.pointer("/block_2_opening/static_declarations/block_3_title").and_then(|v| v.as_str()) {
                config.block_3_title = s.to_string();
            }
            if let Some(fields) = parse_str_array(&val, "/block_2_opening/static_declarations/required_banner_fields") {
                config.block_3_required_banner_fields = fields;
            }
            if let Some(secs) = parse_str_array(&val, "/block_2_opening/static_declarations/dual_audience_sections") {
                config.dual_audience_sections = secs;
            }
            if let Some(s) = val.pointer("/block_3_body/pure_types_statutory_text").and_then(|v| v.as_str()) {
                config.pure_types_statutory_text = s.to_string();
            }
            if let Some(s) = val.pointer("/block_3_body/facade_umbrella_statutory_text").and_then(|v| v.as_str()) {
                config.facade_umbrella_statutory_text = s.to_string();
            }
            if let Some(s) = val.pointer("/block_3_body/triggers_cross_reference_tag").and_then(|v| v.as_str()) {
                config.triggers_cross_reference_tag = s.to_string();
            }
            if let Some(v) = val.pointer("/block_3_body/ban_macros_in_block_3").and_then(|v| v.as_bool()) {
                config.ban_macros_in_block_3 = v;
            }
            if let Some(v) = val.pointer("/block_3_body/forbid_static_in_block_3").and_then(|v| v.as_bool()) {
                config.forbid_static_in_block_3 = v;
            }
            if let Some(v) = val.pointer("/block_3_body/forbid_lifecycle_and_procedures_in_block_3").and_then(|v| v.as_bool()) {
                config.forbid_lifecycle_and_procedures_in_block_3 = v;
            }
            if let Some(v) = val.pointer("/block_3_body/enforce_sub_assembly_symbol_parity").and_then(|v| v.as_bool()) {
                config.enforce_sub_assembly_symbol_parity = v;
            }
            if let Some(v) = val.pointer("/block_3_body/require_per_fn_doxygen_in_block_3").and_then(|v| v.as_bool()) {
                config.require_per_fn_doxygen_in_block_3 = v;
            }
            if let Some(v) = val.pointer("/block_3_body/require_source_reconstructible_docs").and_then(|v| v.as_bool()) {
                config.require_source_reconstructible_docs = v;
            }
            if let Some(obj) = val.pointer("/block_3_body/archetype_subtitles").and_then(|v| v.as_object()) {
                for (k, v) in obj {
                    if let Some(s) = v.as_str() {
                        config.archetype_b3_subtitles.insert(k.clone(), s.to_string());
                    }
                }
            }
        }
    }

    // Layer 1 & 2: Check block_5_closing.json (generic -> engine specialization)
    let tools_b5 = repo_root.join("tools/config/blocks/block_5_closing.json");
    if let Ok(content) = fs::read_to_string(&tools_b5) {
        if let Ok(val) = serde_json::from_str::<serde_json::Value>(&content) {
            if let Some(boot_sfx) = val.pointer("/block_3_body/stateful_lifecycle_boot_suffix").and_then(|v| v.as_str()) {
                config.lifecycle_boot_suffix = boot_sfx.to_string();
            }
        }
    }

    let eng_b5 = repo_root.join("engine/config/standards/blocks/block_5_closing.json");
    if let Ok(content) = fs::read_to_string(&eng_b5) {
        if let Ok(val) = serde_json::from_str::<serde_json::Value>(&content) {
            if let Some(lit) = val.pointer("/block_3_body/stateless_exemption_literal").and_then(|v| v.as_str()) {
                config.stateless_exemption_literal = lit.to_string();
            }
            if let Some(dump_sfx) = val.pointer("/block_3_body/dump_card_suffix").and_then(|v| v.as_str()) {
                config.dump_card_suffix = dump_sfx.to_string();
            }
            if let Some(boot_sfx) = val.pointer("/block_3_body/lifecycle_boot_suffix").and_then(|v| v.as_str()) {
                config.lifecycle_boot_suffix = boot_sfx.to_string();
            }
        }
    }

    config
}
