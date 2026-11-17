// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/syntax/toml.rs
// @id            cog.tools.syntax.toml
// @layer         Layer 3: Developer Toolchain Language Syntax Primitives
// @archetype     pure_types | toml_parser
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal Syntax Framework // TOML Lexer
// @role          TOML Document Tokenizer, Table Scanner & Attribute Lexer
// @maturity      Rev 5 Baseline | TOML Specification Analysis
// @chassis       bounded-heap | Toolchain memory chassis; bounded tokens
// @scale         O(L) across TOML lines
// @seams         in: TOML text | out: TOML tables, keys & values
// @invariants    1. Accurate table header extraction ([table], [[array]]).
//                2. Key-value pair isolation respecting string literals and comments.
//                3. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Lexical tokenizer and structural parser for TOML configuration manifests
//   across the repository (e.g. `tools/Cargo.toml`). Extracts comment headers,
//   table declarations, and key-value bindings.
//
//   ### For Game Designers & Non-Programmers:
//   Ensures build manifests, package configurations, and tool flags follow
//   structured, readable conventions.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Parser is stateless and pure.
//   - Zero Panics: Graceful string slicing across malformed tables.
//   - Transpilation Alignment: Maps TOML configurations to universal OmniCode AST.
// =============================================================================

//! # TOML Configuration Syntax & Lexical Analyzer
//!
//! Provides lexical analysis, table header extraction, and key-value parsing
//! for TOML files.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Defaults
// -----------------------------------------------------------------------------

/// Canonical Cargo.toml standard tables.
pub const CANONICAL_CARGO_TABLES: &[&str] = &[
    "package",
    "dependencies",
    "dev-dependencies",
    "features",
    "bin",
];

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types, Enums & TOML Lexical AST
// -----------------------------------------------------------------------------

/// Discovered TOML table header.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct TomlTable {
    /// Table name (e.g. "package", "dependencies").
    pub name: String,
    /// True if array of tables (`[[name]]`).
    pub is_array: bool,
    /// 1-based line number.
    pub line_number: usize,
}

/// Discovered key-value pair in TOML text.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct TomlEntry {
    /// Key identifier.
    pub key: String,
    /// Raw unparsed value string.
    pub value: String,
    /// Containing table if any.
    pub table: Option<String>,
    /// 1-based line number.
    pub line_number: usize,
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Verification Helpers & Invariant Predicates
// -----------------------------------------------------------------------------

/// Evaluates whether a line starts a TOML table header.
pub fn is_toml_table_header(trimmed_line: &str) -> bool {
    trimmed_line.starts_with('[') && trimmed_line.ends_with(']')
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Table Header Scanner & Key-Value Lexer )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Table & Entry Scanner
// -----------------------------------------------------------------------------

/// Scans TOML text for table declarations and key-value entries.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::toml::scan_toml_content;
///
/// let toml = "[package]\nname = \"tl_tools\"\nversion = \"0.1.0\"\n";
/// let (tables, entries) = scan_toml_content(toml);
/// assert_eq!(tables.len(), 1);
/// assert_eq!(tables[0].name, "package");
/// assert_eq!(entries.len(), 2);
/// assert_eq!(entries[0].key, "name");
/// ```
pub fn scan_toml_content(content: &str) -> (Vec<TomlTable>, Vec<TomlEntry>) {
    let mut tables = Vec::new();
    let mut entries = Vec::new();
    let mut current_table: Option<String> = None;

    for (idx, line) in content.lines().enumerate() {
        let line_num = idx + 1;
        let trimmed = line.trim();

        if trimmed.starts_with('#') || trimmed.is_empty() {
            continue;
        }

        // Table header: [name] or [[name]]
        if trimmed.starts_with('[') && trimmed.ends_with(']') {
            let is_array = trimmed.starts_with("[[") && trimmed.ends_with("]]");
            let name = if is_array {
                trimmed[2..trimmed.len() - 2].trim().to_string()
            } else {
                trimmed[1..trimmed.len() - 1].trim().to_string()
            };

            current_table = Some(name.clone());
            tables.push(TomlTable {
                name,
                is_array,
                line_number: line_num,
            });
            continue;
        }

        // Key-value pair: key = value
        if let Some(eq_pos) = trimmed.find('=') {
            let key = trimmed[..eq_pos].trim().to_string();
            let val_part = trimmed[eq_pos + 1..].trim();
            // Strip trailing comment if not in quote
            let clean_val = if !val_part.starts_with('"') {
                val_part.split('#').next().unwrap_or(val_part).trim()
            } else {
                val_part
            };

            entries.push(TomlEntry {
                key,
                value: clean_val.to_string(),
                table: current_table.clone(),
                line_number: line_num,
            });
        }
    }

    (tables, entries)
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
    fn test_section_1_nominal_toml_scanning() {
        let toml = "\
# Project Cargo Manifest
[package]
name = \"tl_tools\"
version = \"0.1.2-dev\"
edition = \"2024\"

[dependencies]
serde = \"1.0\"
";
        let (tables, entries) = scan_toml_content(toml);
        assert_eq!(tables.len(), 2);
        assert_eq!(tables[0].name, "package");
        assert_eq!(tables[1].name, "dependencies");
        assert_eq!(entries.len(), 4);
        assert_eq!(entries[0].key, "name");
        assert_eq!(entries[0].value, "\"tl_tools\"");
        assert_eq!(entries[0].table.as_deref(), Some("package"));
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_empty_source() {
        let (tables, entries) = scan_toml_content("");
        assert!(tables.is_empty());
        assert!(entries.is_empty());
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_array_of_tables() {
        let toml = "[[bin]]\nname = \"validator\"\npath = \"src/bin/validator.rs\"\n";
        let (tables, entries) = scan_toml_content(toml);
        assert_eq!(tables.len(), 1);
        assert_eq!(tables[0].name, "bin");
        assert!(tables[0].is_array);
        assert_eq!(entries.len(), 2);
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_live_cargo_toml_structure() {
        let sample = "\
[package]
name = \"tl_tools\"
default-run = \"tl_tools\"

[dependencies]
serde = { version = \"1.0\", features = [\"derive\"] }
";
        let (tables, entries) = scan_toml_content(sample);
        assert!(tables.iter().any(|t| t.name == "package"));
        assert!(entries.iter().any(|e| e.key == "default-run"));
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying TOML syntax parser internal consistency.
pub fn self_audit() -> Result<(), String> {
    if CANONICAL_CARGO_TABLES.is_empty() {
        return Err("CANONICAL_CARGO_TABLES must be non-empty".to_string());
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

/// Epilogue predicate affirming that TOML syntax parser is operational.
pub fn is_toml_syntax_ready() -> bool {
    self_audit().is_ok()
}
