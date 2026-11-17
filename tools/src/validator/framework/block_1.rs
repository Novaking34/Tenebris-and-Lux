// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/validator/framework/block_1.rs
// @id            cog.tools.validator.framework.block_1
// @layer         Layer 3: Systems Developer Tooling Core Library
// @archetype     operational | stateless
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal 5-Block Validator Suite
// @role          Language-Agnostic Block 1 Metadata & Invariant Rule Engine
// @maturity      Rev 5 Baseline | Universal Block 1 Rule Engine
// @chassis       zero-heap | Static rule verification tables; bounded issues
// @scale         O(N) tag scan over parsed Block 1 metadata items
// @seams         in: syntax::framework::block_1 | uses: validator::report | probes: self_audit -> exit
// @invariants    1. Zero panics on malformed or incomplete Block 1 AST metadata.
//                2. Emits standardized ValidationIssue records for all invariant violations.
//                3. Validates column 16 tag alignment, mandatory tags, and dual-audience commentary.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Language-agnostic validation rule engine for Block 1 Metadata. Evaluates
//   syntactically extracted Block 1 metadata tags, column 16 value alignment,
//   required identity fields, invariant counts, and dual-audience documentation.
//
//   ### For Game Designers & Non-Programmers:
//   Block 1 is the file's passport. It confirms who owns the file, what project
//   it belongs to, and explains its purpose in plain English so team members
//   understand its impact on the game and engine without reading code.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Stateless rule engine operating purely on `Block1Metadata`.
//   - Invariant Enforcement: Enforces mandatory identity anchors (@project, @file,
//     @id, @layer, @version, @interface_rev, @date, @author, @repository, @system,
//     @role, @invariants), column 16 value alignment, and dual-audience sections.
//   - Explicit Non-Goal: Does not parse raw text lines; evaluates pre-parsed AST only.
// =============================================================================

//! # OmniCode Block 1 Metadata Validator
//!
//! Enforces universal Block 1 identity, provenance, and cognitive anchor rules.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

use std::path::Path;
use crate::syntax::framework::block_1::{Block1Metadata, is_valid_tag_column};
use crate::validator::report::{IssueSeverity, ValidationIssue};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Defaults & Alignment Constants
// -----------------------------------------------------------------------------

/// Canonical tag value starting column (1-based index).
pub const CANONICAL_TAG_COLUMN: usize = 16;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types, Enums & Rule Configuration
// -----------------------------------------------------------------------------

/// Configuration settings controlling Block 1 rule evaluation.
///
/// # Examples
///
/// ```rust
/// use tl_tools::validator::framework::block_1::Block1RuleConfig;
///
/// let config = Block1RuleConfig::default();
/// assert!(config.require_dual_audience);
/// assert!(config.strict_column_alignment);
/// ```
#[derive(Debug, Clone)]
pub struct Block1RuleConfig {
    /// Expected starting column index for tag values (1-based index, canonical = 16).
    pub expected_column_alignment: usize,
    /// Whether dual-audience documentation (`### For Game Designers` and `### For Systems Programmers`) is required.
    pub require_dual_audience: bool,
    /// Whether column alignment must be strictly verified.
    pub strict_column_alignment: bool,
    /// List of mandatory tag names without the '@' prefix.
    pub required_tags: Vec<&'static str>,
}

impl Default for Block1RuleConfig {
    fn default() -> Self {
        Self {
            expected_column_alignment: CANONICAL_TAG_COLUMN,
            require_dual_audience: true,
            strict_column_alignment: true,
            required_tags: vec![
                "project",
                "file",
                "id",
                "layer",
                "version",
                "interface_rev",
                "date",
                "author",
                "repository",
                "system",
                "role",
                "invariants",
            ],
        }
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Verification Helpers & Identifier Invariant Predicates
// -----------------------------------------------------------------------------

/// Returns true if an OmniCode ID follows the canonical dot-separated format.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Ensures names look clean and organized like `cog.core.math`.
/// * For Systems Programmers: Validates lowercase alphanumeric words separated by dots.
///
/// # Examples
///
/// ```rust
/// use tl_tools::validator::framework::block_1::is_valid_omnicode_id;
///
/// assert!(is_valid_omnicode_id("cog.core.math"));
/// assert!(is_valid_omnicode_id("tools.validator.framework.block_1"));
/// assert!(!is_valid_omnicode_id("Invalid ID With Spaces"));
/// assert!(!is_valid_omnicode_id("..empty.parts.."));
/// ```
pub fn is_valid_omnicode_id(id: &str) -> bool {
    if id.is_empty() || id.starts_with('.') || id.ends_with('.') {
        return false;
    }
    for part in id.split('.') {
        if part.is_empty() {
            return false;
        }
        if !part.chars().all(|c| c.is_ascii_alphanumeric() || c == '_' || c == '-') {
            return false;
        }
    }
    true
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Primary Execution Engine, Rule Evaluation & Issue Generation )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Block 1 Validation Rule Engine
// -----------------------------------------------------------------------------

/// Validates a parsed `Block1Metadata` AST against universal metadata rules.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Checks that every file tells the team who wrote it,
///   what project it belongs to, and what rules it promises to follow.
/// * For Systems Programmers: Evaluates tag presence, column 16 alignment,
///   `@id` alphanumeric syntax, and dual-audience section markers.
///
/// # Examples
///
/// ```rust
/// use std::path::Path;
/// use tl_tools::syntax::framework::block_1::parse_block_1_lines;
/// use tl_tools::validator::framework::block_1::{validate_block_1, Block1RuleConfig};
///
/// let lines = [
///     "/*".to_string(),
///     "@project       Tenebris and Lux".to_string(),
///     "@file          engine/include/core/math.h".to_string(),
///     "@id            cog.core.math".to_string(),
///     "@layer         Layer 0".to_string(),
///     "@version       0.1.2-dev".to_string(),
///     "@interface_rev Rev 5".to_string(),
///     "@date          2026-11-15".to_string(),
///     "@author        CreativeWorkzStudio LLC / ProfessorSeanEX".to_string(),
///     "@repository    https://github.com/Novaking34/tenebris-lux".to_string(),
///     "@system        Core Math".to_string(),
///     "@role          Mathematical Foundation".to_string(),
///     "@invariants    1. Zero heap allocations.".to_string(),
///     "### For Game Designers & Non-Programmers:".to_string(),
///     "Math utilities.".to_string(),
///     "### For Systems Programmers & Engine Architects:".to_string(),
///     "Pure functions.".to_string(),
///     "*/".to_string(),
/// ];
/// let meta = parse_block_1_lines(&lines).unwrap();
/// let mut issues = Vec::new();
/// validate_block_1(Path::new("engine/include/core/math.h"), &meta, &Block1RuleConfig::default(), &mut issues);
/// assert!(issues.is_empty());
/// ```
pub fn validate_block_1(
    file_path: &Path,
    meta: &Block1Metadata,
    config: &Block1RuleConfig,
    issues: &mut Vec<ValidationIssue>,
) {
    // 1. Mandatory tags presence check
    for req_tag in &config.required_tags {
        if meta.tag_value(req_tag).is_none() {
            issues.push(ValidationIssue {
                file: file_path.to_path_buf(),
                block: "block_1_metadata".to_string(),
                field: format!("@{}", req_tag),
                severity: IssueSeverity::Error,
                message: format!("Missing mandatory Block 1 metadata tag '@{}'", req_tag),
            });
        }
    }

    // 2. Column alignment check for all parsed tags
    if config.strict_column_alignment {
        for tag in &meta.tags {
            if !is_valid_tag_column(tag.value_column) {
                issues.push(ValidationIssue {
                    file: file_path.to_path_buf(),
                    block: "block_1_metadata".to_string(),
                    field: format!("@{}", tag.key),
                    severity: IssueSeverity::Warning,
                    message: format!(
                        "Tag '@{}' value starts at column {}, expected column {}",
                        tag.key, tag.value_column, config.expected_column_alignment
                    ),
                });
            }
        }
    }

    // 3. Invariants count check
    if config.required_tags.contains(&"invariants") && meta.invariants.is_empty() {
        issues.push(ValidationIssue {
            file: file_path.to_path_buf(),
            block: "block_1_metadata".to_string(),
            field: "@invariants".to_string(),
            severity: IssueSeverity::Error,
            message: "Block 1 must declare at least one verifiable invariant under '@invariants'".to_string(),
        });
    }

    // 4. Dot-separated ID syntax check
    if let Some(id_val) = meta.tag_value("id") {
        if !is_valid_omnicode_id(id_val) {
            issues.push(ValidationIssue {
                file: file_path.to_path_buf(),
                block: "block_1_metadata".to_string(),
                field: "@id".to_string(),
                severity: IssueSeverity::Error,
                message: format!(
                    "Invalid @id '{}': must be dot-separated alphanumeric identifier (e.g. 'cog.core.math')",
                    id_val
                ),
            });
        }
    }

    // 5. Dual-audience documentation check
    if config.require_dual_audience {
        let has_designer = !meta.designer_text.trim().is_empty();
        let has_programmer = !meta.programmer_text.trim().is_empty();

        if !has_designer {
            issues.push(ValidationIssue {
                file: file_path.to_path_buf(),
                block: "block_1_metadata".to_string(),
                field: "documentation".to_string(),
                severity: IssueSeverity::Error,
                message: "Missing '### For Game Designers & Non-Programmers:' documentation section".to_string(),
            });
        }
        if !has_programmer {
            issues.push(ValidationIssue {
                file: file_path.to_path_buf(),
                block: "block_1_metadata".to_string(),
                field: "documentation".to_string(),
                severity: IssueSeverity::Error,
                message: "Missing '### For Systems Programmers & Engine Architects:' documentation section".to_string(),
            });
        }
    }
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
    use crate::syntax::framework::block_1::parse_block_1_lines;

    // -------------------------------------------------------------------------
    // SECTION 1: ALWAYS PASS (Nominal Golden Surface Cogs)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_1_nominal_valid_block_1_passes() {
        let lines = [
            "/*".to_string(),
            "@project       Tenebris and Lux".to_string(),
            "@file          engine/include/core/math.h".to_string(),
            "@id            cog.core.math".to_string(),
            "@layer         Layer 0: Core Engine".to_string(),
            "@version       0.1.2-dev".to_string(),
            "@interface_rev Rev 5".to_string(),
            "@date          2026-11-15".to_string(),
            "@author        CreativeWorkzStudio LLC / ProfessorSeanEX".to_string(),
            "@repository    https://github.com/Novaking34/tenebris-lux".to_string(),
            "@system        Core Math".to_string(),
            "@role          Mathematical Subsystem".to_string(),
            "@invariants    1. Zero heap allocations.".to_string(),
            "### For Game Designers & Non-Programmers:".to_string(),
            "Plain English explanation.".to_string(),
            "### For Systems Programmers & Engine Architects:".to_string(),
            "Technical details.".to_string(),
            "*/".to_string(),
        ];
        let meta = parse_block_1_lines(&lines).expect("Parsing valid block 1 failed");
        let mut issues = Vec::new();
        validate_block_1(Path::new("engine/include/core/math.h"), &meta, &Block1RuleConfig::default(), &mut issues);
        assert!(issues.is_empty(), "Expected 0 issues, got: {:?}", issues);
    }

    #[test]
    fn test_section_1_nominal_valid_id_syntax() {
        assert!(is_valid_omnicode_id("cog.core.math"));
        assert!(is_valid_omnicode_id("tools.validator.framework.block_1"));
        assert!(is_valid_omnicode_id("engine.config.viewport"));
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_missing_tags_produce_errors() {
        let lines = [
            "// @project       Tenebris and Lux".to_string(),
            "// @file          test.c".to_string(),
            "// @id            test.cog".to_string(),
        ];
        let meta = parse_block_1_lines(&lines).expect("Parsing minimal lines failed");
        let mut issues = Vec::new();
        validate_block_1(Path::new("test.c"), &meta, &Block1RuleConfig::default(), &mut issues);
        assert!(!issues.is_empty());
        assert!(issues.iter().any(|i| i.field == "@layer"));
        assert!(issues.iter().any(|i| i.field == "@version"));
        assert!(issues.iter().any(|i| i.field == "@invariants"));
    }

    #[test]
    fn test_section_2_never_fail_invalid_ids() {
        assert!(!is_valid_omnicode_id("invalid spaces"));
        assert!(!is_valid_omnicode_id(".leading.dot"));
        assert!(!is_valid_omnicode_id("trailing.dot."));
        assert!(!is_valid_omnicode_id("double..dot"));
        assert!(!is_valid_omnicode_id(""));
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_column_alignment_constant() {
        assert_eq!(CANONICAL_TAG_COLUMN, 16);
    }

    #[test]
    fn test_section_3_invariants_default_required_tags() {
        let config = Block1RuleConfig::default();
        assert!(config.required_tags.contains(&"project"));
        assert!(config.required_tags.contains(&"file"));
        assert!(config.required_tags.contains(&"id"));
        assert!(config.required_tags.contains(&"layer"));
        assert!(config.required_tags.contains(&"invariants"));
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_self_audit() {
        assert!(self_audit().is_ok());
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Diagnostic Self-Audit
// -----------------------------------------------------------------------------

/// Internal self-audit testing Block 1 validation logic against synthetic fixtures.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Tests the passport inspector to make sure it catches
///   missing fields and approves valid files.
/// * For Systems Programmers: Runs deterministic synthetic metadata structures
///   through `validate_block_1` to verify error generation and severity bounds.
///
/// # Examples
///
/// ```rust
/// use tl_tools::validator::framework::block_1::self_audit;
///
/// assert!(self_audit().is_ok());
/// ```
pub fn self_audit() -> Result<(), &'static str> {
    let config = Block1RuleConfig::default();
    let empty_meta = Block1Metadata::default();
    let mut issues = Vec::new();
    validate_block_1(Path::new("test.c"), &empty_meta, &config, &mut issues);

    // Empty metadata must produce error issues for missing mandatory tags
    if issues.is_empty() {
        return Err("Self-audit failed: empty metadata generated zero issues");
    }
    if !issues.iter().any(|i| i.field == "@project") {
        return Err("Self-audit failed: missing @project issue not detected");
    }
    Ok(())
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( State Summary Handshake, Module Epilogue & Teardown Verification )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue Predicate
// -----------------------------------------------------------------------------

/// Epilogue readiness probe for the Block 1 validator subsystem.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Confirms the metadata inspection gear is ready.
/// * For Systems Programmers: Returns true when self-audit passes.
///
/// # Examples
///
/// ```rust
/// use tl_tools::validator::framework::block_1::is_validator_block_1_ready;
///
/// assert!(is_validator_block_1_ready());
/// ```
pub fn is_validator_block_1_ready() -> bool {
    self_audit().is_ok()
}
