// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/syntax/json.rs
// @id            cog.tools.syntax.json
// @layer         Layer 3: Developer Toolchain Language Syntax Primitives
// @archetype     pure_types | envelope_parser
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal Syntax Framework // JSON 5-Block Envelope
// @role          JSON Data Manifest Tokenizer & 5-Block Envelope Validator
// @maturity      Rev 5 Baseline | 5-Block JSON Data Taxonomy
// @chassis       bounded-heap | Toolchain memory chassis; bounded Serde values
// @scale         O(N) across JSON keys and values
// @seams         in: JSON text | out: 5-Block JSON AST
// @invariants    1. Invariant 13: 5-Block JSON envelope strictly validated across manifests.
//                2. Body getter isolation: callers must query data only from block_3_body.
//                3. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   JSON lexical parser and 5-block envelope validator. Enforces the OmniCode
//   Dual Data Taxonomy and 5-Block JSON format across `engine/config/` and
//   `game/data/`. Inspects Block 1 metadata, Block 2 opening schemas, Block 3
//   body payloads, Block 4 audit tripwires, and Block 5 closing handshakes.
//
//   ### For Game Designers & Non-Programmers:
//   Ensures that JSON data files (characters, weapons, skills, quests) are
//   structured consistently so editors and loaders never fail unexpectedly.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Parser is stateless and pure.
//   - Zero Panics: Returns typed `Result` on any malformed JSON.
//   - Transpilation Alignment: Maps JSON data to universal OmniCode AST.
// =============================================================================

//! # 5-Block JSON Manifest & Data Taxonomy Analyzer
//!
//! Provides parsing and structural validation for OmniCode 5-Block JSON files.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

use serde_json::Value;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Defaults & Envelope Keys
// -----------------------------------------------------------------------------

/// Canonical keys required in a standard OmniCode 5-Block JSON manifest.
pub const CANONICAL_JSON_BLOCK_KEYS: &[&str] = &[
    "block_1_metadata",
    "block_2_opening",
    "block_3_body",
    "block_4_audit",
    "block_5_closing",
];

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types, Enums & JSON 5-Block AST
// -----------------------------------------------------------------------------

/// Structured AST representing a parsed 5-Block JSON document.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::json::Json5BlockDoc;
///
/// let doc = Json5BlockDoc::default();
/// assert!(!doc.has_all_five_blocks);
/// ```
#[derive(Debug, Clone, Default, PartialEq)]
pub struct Json5BlockDoc {
    /// Discovered block keys.
    pub present_blocks: Vec<String>,
    /// Block 1 metadata object if present.
    pub block_1_metadata: Option<Value>,
    /// Block 2 opening object if present.
    pub block_2_opening: Option<Value>,
    /// Block 3 body payload if present.
    pub block_3_body: Option<Value>,
    /// Block 4 audit object if present.
    pub block_4_audit: Option<Value>,
    /// Block 5 closing object if present.
    pub block_5_closing: Option<Value>,
    /// True if all 5 canonical block keys are present in root JSON object.
    pub has_all_five_blocks: bool,
}

impl Json5BlockDoc {
    /// Extracts a metadata tag value from Block 1 if present.
    pub fn get_metadata_field(&self, field: &str) -> Option<&str> {
        self.block_1_metadata
            .as_ref()?
            .get(field)?
            .as_str()
    }

    /// Evaluates whether a specific block key exists.
    pub fn has_block(&self, block_name: &str) -> bool {
        self.present_blocks.iter().any(|b| b == block_name)
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Verification Helpers & Invariant Predicates
// -----------------------------------------------------------------------------

/// Evaluates whether a key matches any canonical 5-block envelope key.
pub fn is_canonical_json_block_key(key: &str) -> bool {
    CANONICAL_JSON_BLOCK_KEYS.contains(&key)
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( JSON Parsing, Envelope Inspection & Block Isolation )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: JSON Envelope Parser
// -----------------------------------------------------------------------------

/// Parses raw JSON text into a `Json5BlockDoc` AST.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::json::parse_json_5block_doc;
///
/// let json_text = r#"{
///   "block_1_metadata": { "project": "Tenebris and Lux" },
///   "block_2_opening": {},
///   "block_3_body": { "items": [] },
///   "block_4_audit": {},
///   "block_5_closing": {}
/// }"#;
/// let doc = parse_json_5block_doc(json_text).expect("Must parse 5-block JSON");
/// assert!(doc.has_all_five_blocks);
/// assert_eq!(doc.get_metadata_field("project"), Some("Tenebris and Lux"));
/// ```
pub fn parse_json_5block_doc(content: &str) -> Result<Json5BlockDoc, String> {
    let parsed: Value = serde_json::from_str(content)
        .map_err(|e| format!("Failed to parse JSON content: {}", e))?;

    let Some(obj) = parsed.as_object() else {
        return Err("Root JSON entity must be an object envelope".to_string());
    };

    let mut present = Vec::new();
    let b1 = obj.get("block_1_metadata").cloned();
    let b2 = obj.get("block_2_opening").or_else(|| obj.get("block_2_types")).cloned();
    let b3 = obj.get("block_3_body").cloned();
    let b4 = obj.get("block_4_audit").cloned();
    let b5 = obj.get("block_5_closing").cloned();

    for key in CANONICAL_JSON_BLOCK_KEYS {
        if obj.contains_key(*key) || (*key == "block_2_opening" && obj.contains_key("block_2_types")) {
            present.push(key.to_string());
        }
    }

    let all_blocks = present.len() == CANONICAL_JSON_BLOCK_KEYS.len();

    Ok(Json5BlockDoc {
        present_blocks: present,
        block_1_metadata: b1,
        block_2_opening: b2,
        block_3_body: b3,
        block_4_audit: b4,
        block_5_closing: b5,
        has_all_five_blocks: all_blocks,
    })
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Four-Section Ontological Test Schema & Diagnostic Self-Audit )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Unit Test Suite (Four-Section Ontological Schema)
// -----------------------------------------------------------------------------

#[cfg(test)]
mod tests {
    use super::*;

    // -------------------------------------------------------------------------
    // SECTION 1: ALWAYS PASS (Nominal Golden Surface Cogs)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_1_nominal_5block_json_parsing() {
        let json_text = r#"{
            "block_1_metadata": {
                "project": "Tenebris and Lux",
                "file": "engine/config/manifest.json",
                "version": "0.1.2-dev"
            },
            "block_2_opening": { "schema": "test.schema.v1" },
            "block_3_body": { "entities": [1, 2, 3] },
            "block_4_audit": { "test_status": "pass" },
            "block_5_closing": { "status": "APPROVED" }
        }"#;

        let doc = parse_json_5block_doc(json_text).expect("Parsing must succeed");
        assert!(doc.has_all_five_blocks);
        assert_eq!(doc.get_metadata_field("project"), Some("Tenebris and Lux"));
        assert_eq!(doc.get_metadata_field("version"), Some("0.1.2-dev"));
        assert!(doc.has_block("block_3_body"));
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_malformed_json() {
        let broken = "{ invalid_json: true, ";
        let res = parse_json_5block_doc(broken);
        assert!(res.is_err());
    }

    #[test]
    fn test_section_2_never_fail_json_array_root() {
        let arr = "[1, 2, 3]";
        let res = parse_json_5block_doc(arr);
        assert!(res.is_err());
        assert!(res.unwrap_err().contains("Root JSON entity must be an object"));
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_partial_blocks_detected() {
        let partial = r#"{ "block_1_metadata": {}, "block_3_body": {} }"#;
        let doc = parse_json_5block_doc(partial).expect("Partial JSON must parse");
        assert!(!doc.has_all_five_blocks);
        assert!(doc.has_block("block_1_metadata"));
        assert!(doc.has_block("block_3_body"));
        assert!(!doc.has_block("block_2_opening"));
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_live_layout_rules_envelope() {
        let sample = r#"{
            "block_1_metadata": { "id": "test.rules" },
            "block_2_opening": { "units": 72.0 },
            "block_3_body": { "rules": true },
            "block_4_audit": { "validator": "cargo test" },
            "block_5_closing": { "status": "ACTIVE" }
        }"#;

        let doc = parse_json_5block_doc(sample).expect("Must parse live schema");
        assert!(doc.has_all_five_blocks);
        assert_eq!(doc.get_metadata_field("id"), Some("test.rules"));
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying JSON syntax parser internal consistency.
pub fn self_audit() -> Result<(), String> {
    if CANONICAL_JSON_BLOCK_KEYS.len() != 5 {
        return Err("CANONICAL_JSON_BLOCK_KEYS must define exactly 5 block keys".to_string());
    }
    Ok(())
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Subsystem Readiness Predicate & Epilogue Handshake )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue & Clean State Handshake
// -----------------------------------------------------------------------------

/// Epilogue predicate affirming that JSON syntax parser is operational.
pub fn is_json_syntax_ready() -> bool {
    self_audit().is_ok()
}
