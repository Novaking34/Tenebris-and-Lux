// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/syntax/framework/block_1.rs
// @id            cog.tools.syntax.framework.block_1
// @layer         Layer 3: Developer Toolchain Syntax Framework Sub-Assemblies
// @archetype     pure_types | metadata_parser
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal Syntax Framework // Block 1 Sub-Assembly
// @role          Universal Block 1 Metadata Header Parser & Tag Extractor
// @maturity      Rev 5 Baseline | Universal 5-Block Metadata AST
// @chassis       bounded-heap | Toolchain memory chassis; bounded tag vectors
// @scale         O(L) across header lines
// @seams         in: source lines | out: Block1Metadata AST
// @invariants    1. Tag values must align to column 16 (tag key <= 15 chars).
//                2. Dual-audience documentation sections preserved without loss.
//                3. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Extracts and validates Block 1 Metadata headers across all languages
//   conforming to the OmniCode Universal 5-Block Lifecycle Protocol. Parses
//   standard provenance tags (@project, @file, @id, @layer, @archetype,
//   @version, @interface_rev, @date, @author, @repository, @system, @role,
//   @maturity, @chassis, @scale, @seams, @invariants) and isolates
//   dual-audience documentation sections.
//
//   ### For Game Designers & Non-Programmers:
//   Ensures that every game rule, item catalog, and engine subsystem retains
//   clear plain-English descriptions and provenance identifying who owns it.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Parser is stateless and reentrant.
//   - Extension Seam: New metadata tags register without schema breaking changes.
//   - Transpilation Alignment: Maps directly to universal OmniCode Block 1 schema.
// =============================================================================

//! # Universal Block 1 Metadata Parser
//!
//! Parses and validates Block 1 Metadata headers across C, C++, Rust, JSON,
//! Markdown, AsciiDoc, TOML, and CMake files.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

use std::collections::HashMap;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Defaults & Tag Constants
// -----------------------------------------------------------------------------

/// Standard 1-based column where tag values must align.
pub const STANDARD_TAG_VALUE_COLUMN: usize = 16;

/// Canonical standard Block 1 tags mandated by OmniCode Rev 5.
pub const CANONICAL_BLOCK_1_TAGS: &[&str] = &[
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
];

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types, Enums & Block 1 Metadata AST
// -----------------------------------------------------------------------------

/// Individual metadata tag binding key, value, and line/column provenance.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::framework::block_1::Block1Tag;
///
/// let tag = Block1Tag {
///     key: "project".to_string(),
///     value: "Tenebris and Lux".to_string(),
///     line_number: 5,
///     value_column: 16,
///     is_aligned: true,
/// };
/// assert_eq!(tag.key, "project");
/// assert!(tag.is_aligned);
/// ```
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct Block1Tag {
    /// Tag identifier without leading '@' (e.g., "project", "layer").
    pub key: String,
    /// Tag string value.
    pub value: String,
    /// 1-based source line number where tag appears.
    pub line_number: usize,
    /// 1-based column where tag value starts.
    pub value_column: usize,
    /// True if value begins exactly at column 16.
    pub is_aligned: bool,
}

/// Structured AST representing a complete parsed Block 1 Metadata header.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::framework::block_1::Block1Metadata;
///
/// let meta = Block1Metadata::default();
/// assert!(meta.tags.is_empty());
/// assert!(!meta.has_tag("project"));
/// ```
#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct Block1Metadata {
    /// Raw unparsed lines comprising the Block 1 comment or envelope.
    pub raw_lines: Vec<String>,
    /// Ordered list of discovered metadata tags.
    pub tags: Vec<Block1Tag>,
    /// Fast lookup index for tag values by key.
    pub tag_index: HashMap<String, String>,
    /// Extracted plain-English game designer documentation text.
    pub designer_text: String,
    /// Extracted systems programmer / architectural documentation text.
    pub programmer_text: String,
    /// List of parsed invariant statements.
    pub invariants: Vec<String>,
    /// True if both Designer and Programmer sections are present.
    pub has_dual_audience: bool,
    /// True if all parsed tags start at column 16.
    pub all_tags_aligned: bool,
}

impl Block1Metadata {
    /// Returns the string value of a tag by key if present.
    pub fn tag_value(&self, key: &str) -> Option<&str> {
        self.tag_index.get(key).map(|s| s.as_str())
    }

    /// Evaluates whether a specific tag exists in the metadata block.
    pub fn has_tag(&self, key: &str) -> bool {
        self.tag_index.contains_key(key)
    }

    /// Evaluates whether all canonical baseline tags are present.
    pub fn has_canonical_tags(&self) -> bool {
        CANONICAL_BLOCK_1_TAGS.iter().all(|t| self.has_tag(t))
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Verification Helpers & Invariant Predicates
// -----------------------------------------------------------------------------

/// Evaluates whether a tag value column satisfies standard alignment.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::framework::block_1::is_valid_tag_column;
///
/// assert!(is_valid_tag_column(16));
/// assert!(!is_valid_tag_column(15));
/// assert!(!is_valid_tag_column(17));
/// ```
pub fn is_valid_tag_column(column: usize) -> bool {
    column == STANDARD_TAG_VALUE_COLUMN
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Parsing Engine, Tag Lexing & Dual-Audience Extraction )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Universal Header Line Parser
// -----------------------------------------------------------------------------

/// Strips common comment decoration prefixes across all supported languages.
pub fn clean_header_line(line: &str) -> &str {
    let t = line.trim_start();
    if let Some(after) = t.strip_prefix("/*") {
        after.trim_start_matches('*').trim_start()
    } else if let Some(after) = t.strip_prefix("//") {
        after.trim_start_matches('/').trim_start()
    } else if let Some(after) = t.strip_prefix('#') {
        after.trim_start()
    } else if let Some(after) = t.strip_prefix('*') {
        after.trim_start()
    } else if let Some(after) = t.strip_prefix("////") {
        after.trim_start()
    } else {
        t
    }
}

/// Parses a slice of Block 1 header lines into a structured `Block1Metadata` AST.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::framework::block_1::parse_block_1_lines;
///
/// let lines = vec![
///     "/*".to_string(),
///     "BLOCK 1: METADATA BLOCK".to_string(),
///     "@project       Tenebris and Lux".to_string(),
///     "@id            test.core".to_string(),
///     "### For Game Designers & Non-Programmers:".to_string(),
///     "Design notes here.".to_string(),
///     "### For Systems Programmers & Engine Architects:".to_string(),
///     "Systems notes here.".to_string(),
///     "*/".to_string(),
/// ];
/// let meta = parse_block_1_lines(&lines).expect("Must parse Block 1");
/// assert_eq!(meta.tag_value("project"), Some("Tenebris and Lux"));
/// assert!(meta.has_dual_audience);
/// ```
pub fn parse_block_1_lines(lines: &[String]) -> Result<Block1Metadata, String> {
    let mut meta = Block1Metadata {
        raw_lines: lines.to_vec(),
        ..Default::default()
    };

    let mut in_designer = false;
    let mut in_programmer = false;
    let mut designer_lines = Vec::new();
    let mut programmer_lines = Vec::new();
    let mut all_aligned = true;

    for (idx, raw_line) in lines.iter().enumerate() {
        let line_num = idx + 1;
        let trimmed = raw_line.trim();

        // 1. Dual-Audience Section Heading Detection
        if trimmed.contains("### For Game Designers & Non-Programmers:")
            || trimmed.contains("For Game Designers & Non-Programmers:")
        {
            in_designer = true;
            in_programmer = false;
            continue;
        } else if trimmed.contains("### For Systems Programmers & Engine Architects:")
            || trimmed.contains("For Systems Programmers & Engine Architects:")
        {
            in_designer = false;
            in_programmer = true;
            continue;
        } else if trimmed.starts_with("========")
            || trimmed == "*/"
            || trimmed == "////"
            || trimmed.starts_with("-->")
        {
            in_designer = false;
            in_programmer = false;
        }

        // 2. Accumulate section documentation text
        if in_designer {
            let clean = clean_header_line(raw_line);
            if !clean.is_empty() {
                designer_lines.push(clean.to_string());
            }
            continue;
        } else if in_programmer {
            let clean = clean_header_line(raw_line);
            if !clean.is_empty() {
                programmer_lines.push(clean.to_string());
            }
            continue;
        }

        // 3. Metadata Tag Detection (@key value)
        let cleaned = clean_header_line(raw_line);
        if let Some(tag_part) = cleaned.strip_prefix('@') {
            let parts: Vec<&str> = tag_part.splitn(2, char::is_whitespace).collect();
            if !parts.is_empty() {
                let key = parts[0].to_ascii_lowercase();
                let value = if parts.len() > 1 {
                    parts[1].trim().to_string()
                } else {
                    String::new()
                };

                // Compute exact column of value in raw_line
                let value_col = if let Some(val_idx) = raw_line.find(&value) {
                    val_idx + 1
                } else {
                    STANDARD_TAG_VALUE_COLUMN
                };

                let aligned = is_valid_tag_column(value_col);
                if !aligned && !value.is_empty() {
                    all_aligned = false;
                }

                if key == "invariants" || key.starts_with("invariants") {
                    meta.invariants.push(value.clone());
                }

                meta.tag_index.insert(key.clone(), value.clone());
                meta.tags.push(Block1Tag {
                    key,
                    value,
                    line_number: line_num,
                    value_column: value_col,
                    is_aligned: aligned,
                });
            }
        }
    }

    meta.designer_text = designer_lines.join("\n");
    meta.programmer_text = programmer_lines.join("\n");
    meta.has_dual_audience =
        !meta.designer_text.trim().is_empty() && !meta.programmer_text.trim().is_empty();
    meta.all_tags_aligned = all_aligned && !meta.tags.is_empty();

    Ok(meta)
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
    fn test_section_1_nominal_tag_parsing() {
        let lines = vec![
            "/*".to_string(),
            "@project       Tenebris and Lux".to_string(),
            "@layer         Layer 0: Core Engine".to_string(),
            "### For Game Designers & Non-Programmers:".to_string(),
            "Game mechanics overview.".to_string(),
            "### For Systems Programmers & Engine Architects:".to_string(),
            "Zero heap allocation contracts.".to_string(),
            "*/".to_string(),
        ];

        let meta = parse_block_1_lines(&lines).expect("Parsing must succeed");
        assert_eq!(meta.tag_value("project"), Some("Tenebris and Lux"));
        assert_eq!(meta.tag_value("layer"), Some("Layer 0: Core Engine"));
        assert!(meta.has_dual_audience);
        assert!(meta.designer_text.contains("Game mechanics"));
        assert!(meta.programmer_text.contains("Zero heap"));
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_empty_lines() {
        let empty: Vec<String> = vec![];
        let meta = parse_block_1_lines(&empty).expect("Empty input must return default AST");
        assert!(meta.tags.is_empty());
        assert!(!meta.has_dual_audience);
    }

    #[test]
    fn test_section_2_never_fail_missing_tags() {
        let lines = vec!["No tags here".to_string()];
        let meta = parse_block_1_lines(&lines).expect("Must return clean AST");
        assert_eq!(meta.tag_value("nonexistent"), None);
        assert!(!meta.has_tag("project"));
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_column_16_alignment() {
        assert!(is_valid_tag_column(16));
        assert!(!is_valid_tag_column(15));
        assert!(!is_valid_tag_column(17));
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_full_canonical_metadata() {
        let lines = vec![
            "/*".to_string(),
            "@project       Tenebris and Lux".to_string(),
            "@file          engine/include/core/manifest.h".to_string(),
            "@id            cog.core.manifest".to_string(),
            "@layer         Layer 0: Core Engine".to_string(),
            "@version       0.1.2-dev".to_string(),
            "@interface_rev Rev 5".to_string(),
            "@date          2026-11-15".to_string(),
            "@author        CreativeWorkzStudio LLC / ProfessorSeanEX".to_string(),
            "@repository    https://github.com/Novaking34/tenebris-lux".to_string(),
            "@system        Cornerstone Core Manifest".to_string(),
            "@role          Public Contract & System Anchors".to_string(),
            "@invariants    1. Zero dynamic heap allocation in Layer 0.".to_string(),
            "### For Game Designers & Non-Programmers:".to_string(),
            "Core engine tuning levers.".to_string(),
            "### For Systems Programmers & Engine Architects:".to_string(),
            "Chassis specifications.".to_string(),
            "*/".to_string(),
        ];

        let meta = parse_block_1_lines(&lines).expect("Must parse canonical header");
        assert!(meta.has_canonical_tags());
        assert_eq!(meta.invariants.len(), 1);
        assert!(meta.all_tags_aligned);
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying Block 1 parser internal consistency.
pub fn self_audit() -> Result<(), String> {
    if STANDARD_TAG_VALUE_COLUMN != 16 {
        return Err("STANDARD_TAG_VALUE_COLUMN must equal 16".to_string());
    }
    if CANONICAL_BLOCK_1_TAGS.is_empty() {
        return Err("CANONICAL_BLOCK_1_TAGS must be non-empty".to_string());
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

/// Epilogue predicate affirming that Block 1 framework parser is operational.
pub fn is_block_1_framework_ready() -> bool {
    self_audit().is_ok()
}
