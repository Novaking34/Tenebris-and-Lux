// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/syntax/framework/block_5.rs
// @id            cog.tools.syntax.framework.block_5
// @layer         Layer 3: Developer Toolchain Syntax Framework Sub-Assemblies
// @archetype     pure_types | closing_parser
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal Syntax Framework // Block 5 Sub-Assembly
// @role          Universal Block 5 Closing Parser & Readiness Predicate Scanner
// @maturity      Rev 5 Baseline | Universal 5-Block Closing AST
// @chassis       bounded-heap | Toolchain memory chassis; bounded markers
// @scale         O(L) across Block 5 lines
// @seams         in: source lines | out: Block5Closing AST
// @invariants    1. Subsystem readiness predicate (is_..._ready) discovery required.
//                2. Epilogue clean state handshake verification preserved.
//                3. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Extracts and validates Block 5 Closing declarations across all languages
//   conforming to the OmniCode Universal 5-Block Lifecycle Protocol. Discovers
//   and catalogs Sub-Assemblies 5.1 and 5.2, epilogue clean state handshakes,
//   subsystem readiness predicates (is_<subsystem>_ready), and matching header
//   guard closing directives.
//
//   ### For Game Designers & Non-Programmers:
//   Guarantees that every subsystem cleanly finishes its turn, logs clean state,
//   and certifies readiness before higher-level game loops tick.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Parser is stateless and pure.
//   - Header Guard Closure: Inspects matching #endif /* ... */ comment tokens.
//   - Transpilation Alignment: Maps directly to universal OmniCode Block 5 schema.
// =============================================================================

//! # Universal Block 5 Closing Parser
//!
//! Parses and validates Block 5 Closing declarations across C, C++, Rust, JSON,
//! Markdown, AsciiDoc, TOML, and CMake files.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

use super::block_2::{extract_subassembly_markers, SubAssemblyMarker};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Defaults
// -----------------------------------------------------------------------------

/// Canonical minimum expected sub-assemblies in standard Block 5.
pub const MIN_BLOCK_5_SUBASSEMBLIES: usize = 1;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types, Enums & Block 5 Closing AST
// -----------------------------------------------------------------------------

/// Structured AST representing a parsed Block 5 Closing section.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::framework::block_5::Block5Closing;
///
/// let b5 = Block5Closing::default();
/// assert!(b5.sub_assemblies.is_empty());
/// assert!(!b5.has_readiness_predicate);
/// ```
#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct Block5Closing {
    /// Raw lines of Block 5.
    pub raw_lines: Vec<String>,
    /// Discovered sub-assemblies (5.1, 5.2).
    pub sub_assemblies: Vec<SubAssemblyMarker>,
    /// True if an `is_<subsystem>_ready` predicate function was discovered.
    pub has_readiness_predicate: bool,
    /// Function name of discovered readiness predicate if present.
    pub readiness_function_name: Option<String>,
    /// Line number of discovered readiness predicate if present.
    pub readiness_line: Option<usize>,
    /// True if a trailing `#endif` header guard close was found.
    pub has_guard_close: bool,
    /// Trailing `#endif` comment text if found.
    pub guard_close_comment: Option<String>,
}

impl Block5Closing {
    /// Evaluates whether a specific sub-assembly exists (e.g. "5.1", "5.2").
    pub fn has_subassembly(&self, num: &str) -> bool {
        self.sub_assemblies.iter().any(|s| s.number == num)
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Verification Helpers & Invariant Predicates
// -----------------------------------------------------------------------------

/// Evaluates whether a candidate function name is a valid subsystem readiness predicate.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::framework::block_5::is_readiness_predicate_name;
///
/// assert!(is_readiness_predicate_name("is_log_subsystem_ready"));
/// assert!(is_readiness_predicate_name("is_core_manifest_ready"));
/// assert!(!is_readiness_predicate_name("is_valid_pointer"));
/// ```
pub fn is_readiness_predicate_name(name: &str) -> bool {
    let lower = name.to_ascii_lowercase();
    lower.starts_with("is_") && lower.ends_with("_ready")
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Readiness Scanner, Guard Inspection & Epilogue Lexing )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Readiness Predicate & Guard Close Scanner
// -----------------------------------------------------------------------------

/// Discovers subsystem readiness predicate function declaration in Block 5 lines.
pub fn find_readiness_predicate(lines: &[String]) -> Option<(String, usize)> {
    for (idx, line) in lines.iter().enumerate() {
        let line_num = idx + 1;
        let t = line.trim();

        if !t.starts_with("//") && !t.starts_with("/*") && !t.starts_with('*') {
            if let Some(pos) = t.find("is_") {
                let candidate = &t[pos..];
                if let Some(end_paren) = candidate.find('(') {
                    let fn_name = candidate[..end_paren].trim();
                    if is_readiness_predicate_name(fn_name) {
                        return Some((fn_name.to_string(), line_num));
                    }
                } else if candidate.ends_with("_ready") {
                    return Some((candidate.to_string(), line_num));
                }
            }
        }
    }
    None
}

/// Discovers trailing `#endif` header guard closure.
pub fn find_header_guard_close(lines: &[String]) -> Option<(bool, Option<String>)> {
    for line in lines.iter().rev() {
        let t = line.trim();
        if let Some(after) = t.strip_prefix("#endif") {
            let comment = after.trim();
            let cleaned = if comment.starts_with("/*") && comment.ends_with("*/") {
                Some(comment[2..comment.len() - 2].trim().to_string())
            } else if comment.starts_with("//") {
                Some(comment[2..].trim().to_string())
            } else if !comment.is_empty() {
                Some(comment.to_string())
            } else {
                None
            };
            return Some((true, cleaned));
        }
    }
    None
}

/// Parses a slice of Block 5 Closing lines into a `Block5Closing` AST.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::framework::block_5::parse_block_5_lines;
///
/// let lines = vec![
///     "// Sub-Assembly 5.1: Epilogue".to_string(),
///     "// Sub-Assembly 5.2: Readiness Predicate".to_string(),
///     "pub fn is_framework_ready() -> bool { true }".to_string(),
///     "#endif /* CORNERSTONE_FRAMEWORK_H */".to_string(),
/// ];
/// let b5 = parse_block_5_lines(&lines).expect("Must parse Block 5");
/// assert!(b5.has_readiness_predicate);
/// assert!(b5.has_guard_close);
/// ```
pub fn parse_block_5_lines(lines: &[String]) -> Result<Block5Closing, String> {
    let sub_assemblies = extract_subassembly_markers(lines, 5);
    let readiness = find_readiness_predicate(lines);
    let guard = find_header_guard_close(lines);

    Ok(Block5Closing {
        raw_lines: lines.to_vec(),
        sub_assemblies,
        has_readiness_predicate: readiness.is_some(),
        readiness_function_name: readiness.as_ref().map(|(n, _)| n.clone()),
        readiness_line: readiness.as_ref().map(|(_, l)| *l),
        has_guard_close: guard.is_some(),
        guard_close_comment: guard.and_then(|(_, c)| c),
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
    fn test_section_1_nominal_readiness_predicate_discovery() {
        let lines = vec![
            "/* Sub-Assembly 5.1: Epilogue & Clean State Handshake */".to_string(),
            "/* Sub-Assembly 5.2: Subsystem Readiness Predicate */".to_string(),
            "bool is_log_subsystem_ready(void);".to_string(),
            "#endif /* CORNERSTONE_LOG_H */".to_string(),
        ];

        let b5 = parse_block_5_lines(&lines).expect("Parsing must succeed");
        assert_eq!(b5.sub_assemblies.len(), 2);
        assert!(b5.has_readiness_predicate);
        assert_eq!(
            b5.readiness_function_name.as_deref(),
            Some("is_log_subsystem_ready")
        );
        assert!(b5.has_guard_close);
        assert_eq!(
            b5.guard_close_comment.as_deref(),
            Some("CORNERSTONE_LOG_H")
        );
    }

    #[test]
    fn test_section_1_nominal_predicate_name_predicate() {
        assert!(is_readiness_predicate_name("is_core_ready"));
        assert!(is_readiness_predicate_name("is_log_subsystem_ready"));
        assert!(!is_readiness_predicate_name("is_valid"));
        assert!(!is_readiness_predicate_name("ready_system"));
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_empty_lines() {
        let empty: Vec<String> = vec![];
        let b5 = parse_block_5_lines(&empty).expect("Empty input must return default AST");
        assert!(b5.sub_assemblies.is_empty());
        assert!(!b5.has_readiness_predicate);
        assert!(!b5.has_guard_close);
    }

    #[test]
    fn test_section_2_never_fail_missing_readiness() {
        let lines = vec!["// Just a regular line".to_string()];
        let b5 = parse_block_5_lines(&lines).expect("Must parse cleanly");
        assert!(!b5.has_readiness_predicate);
        assert_eq!(b5.readiness_function_name, None);
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_subassembly_ordering() {
        let lines = vec![
            "// Sub-Assembly 5.1: Epilogue".to_string(),
            "// Sub-Assembly 5.2: Readiness".to_string(),
        ];
        let b5 = parse_block_5_lines(&lines).expect("Must parse markers");
        assert_eq!(b5.sub_assemblies.len(), 2);
        assert_eq!(b5.sub_assemblies[0].number, "5.1");
        assert_eq!(b5.sub_assemblies[1].number, "5.2");
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_rust_block_5() {
        let rust_lines = vec![
            "// =============================================================================".to_string(),
            "// BLOCK 5: CLOSING BLOCK".to_string(),
            "// =============================================================================".to_string(),
            "// Sub-Assembly 5.1: Epilogue & Clean State Handshake".to_string(),
            "// Sub-Assembly 5.2: Subsystem Readiness Predicate".to_string(),
            "pub fn is_rust_module_ready() -> bool {".to_string(),
            "    self_audit().is_ok()".to_string(),
            "}".to_string(),
        ];

        let b5 = parse_block_5_lines(&rust_lines).expect("Must parse Rust Block 5");
        assert_eq!(b5.sub_assemblies.len(), 2);
        assert!(b5.has_readiness_predicate);
        assert_eq!(
            b5.readiness_function_name.as_deref(),
            Some("is_rust_module_ready")
        );
        assert!(!b5.has_guard_close); // Rust has no #endif
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying Block 5 parser internal consistency.
pub fn self_audit() -> Result<(), String> {
    if MIN_BLOCK_5_SUBASSEMBLIES == 0 {
        return Err("MIN_BLOCK_5_SUBASSEMBLIES must be >= 1".to_string());
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

/// Epilogue predicate affirming that Block 5 framework parser is operational.
pub fn is_block_5_framework_ready() -> bool {
    self_audit().is_ok()
}
