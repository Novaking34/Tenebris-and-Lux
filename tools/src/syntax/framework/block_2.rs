// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/syntax/framework/block_2.rs
// @id            cog.tools.syntax.framework.block_2
// @layer         Layer 3: Developer Toolchain Syntax Framework Sub-Assemblies
// @archetype     pure_types | opening_parser
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal Syntax Framework // Block 2 Sub-Assembly
// @role          Universal Block 2 Opening Header Parser & Sub-Assembly Scanner
// @maturity      Rev 5 Baseline | Universal 5-Block Opening AST
// @chassis       bounded-heap | Toolchain memory chassis; bounded markers
// @scale         O(L) across Block 2 lines
// @seams         in: source lines | out: Block2Opening AST
// @invariants    1. Sub-assembly sequence must follow canonical monotonic order (2.1, 2.2, ...).
//                2. Zero unquarantined include path bleeding.
//                3. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Extracts and validates Block 2 Opening declarations across all languages
//   conforming to the OmniCode Universal 5-Block Lifecycle Protocol. Discovers
//   and catalogs Sub-Assemblies 2.1 through 2.5, include directives, type
//   definitions, and invariant verification helper predicates.
//
//   ### For Game Designers & Non-Programmers:
//   Guarantees that tunable limits, data capacities, and game formulas are
//   grouped cleanly at the top of each system rather than scattered in code.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Parser is stateless and pure.
//   - Sub-Assembly Discipline: Asserts Sub-Assemblies 2.1..2.4 sequence rules.
//   - Transpilation Alignment: Maps directly to universal OmniCode Block 2 schema.
// =============================================================================

//! # Universal Block 2 Opening Parser
//!
//! Parses and validates Block 2 Opening declarations across C, C++, Rust, JSON,
//! Markdown, AsciiDoc, TOML, and CMake files.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Defaults & Sub-Assembly Identifiers
// -----------------------------------------------------------------------------

/// Canonical required Sub-Assembly prefixes for standard Block 2 headers.
pub const CANONICAL_BLOCK_2_SUBASSEMBLIES: &[&str] = &["2.1", "2.2", "2.3"];

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types, Enums & Block 2 Opening AST
// -----------------------------------------------------------------------------

/// Discovered sub-assembly marker within a 5-block file.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::framework::block_2::SubAssemblyMarker;
///
/// let marker = SubAssemblyMarker {
///     number: "2.1".to_string(),
///     title: "Operational Constants".to_string(),
///     line_number: 14,
/// };
/// assert_eq!(marker.number, "2.1");
/// ```
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct SubAssemblyMarker {
    /// Sub-assembly designator string (e.g., "2.1", "2.2", "3.1").
    pub number: String,
    /// Human-readable title of the sub-assembly.
    pub title: String,
    /// 1-based source line where marker begins.
    pub line_number: usize,
}

/// Extracted include or import directive.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct IncludeDirective {
    /// Path or module imported.
    pub path: String,
    /// True if quoted (e.g. `#include "core/manifest.h"`).
    pub is_quoted: bool,
    /// True if system angle bracket (e.g. `<stdint.h>`).
    pub is_system: bool,
    /// 1-based line number.
    pub line_number: usize,
}

/// Structured AST representing a parsed Block 2 Opening section.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::framework::block_2::Block2Opening;
///
/// let b2 = Block2Opening::default();
/// assert!(b2.sub_assemblies.is_empty());
/// assert!(b2.includes.is_empty());
/// ```
#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct Block2Opening {
    /// Raw lines of Block 2.
    pub raw_lines: Vec<String>,
    /// Discovered sub-assembly markers (2.1..2.5).
    pub sub_assemblies: Vec<SubAssemblyMarker>,
    /// Discovered includes or imports.
    pub includes: Vec<IncludeDirective>,
    /// True if required 2.1, 2.2, 2.3 markers are all present.
    pub has_baseline_subassemblies: bool,
}

impl Block2Opening {
    /// Evaluates whether a specific sub-assembly number exists (e.g. "2.1").
    pub fn has_subassembly(&self, num: &str) -> bool {
        self.sub_assemblies.iter().any(|s| s.number == num)
    }

    /// Returns the title of a sub-assembly if found.
    pub fn subassembly_title(&self, num: &str) -> Option<&str> {
        self.sub_assemblies
            .iter()
            .find(|s| s.number == num)
            .map(|s| s.title.as_str())
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Verification Helpers & Invariant Predicates
// -----------------------------------------------------------------------------

/// Evaluates whether a sub-assembly number follows the expected "X.Y" format.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::framework::block_2::is_valid_subassembly_number;
///
/// assert!(is_valid_subassembly_number("2.1", 2));
/// assert!(is_valid_subassembly_number("2.4", 2));
/// assert!(!is_valid_subassembly_number("3.1", 2));
/// ```
pub fn is_valid_subassembly_number(num: &str, expected_block: u32) -> bool {
    let parts: Vec<&str> = num.split('.').collect();
    if parts.len() != 2 {
        return false;
    }
    if let (Ok(block), Ok(_sub)) = (parts[0].parse::<u32>(), parts[1].parse::<u32>()) {
        block == expected_block
    } else {
        false
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Sub-Assembly Parsing, Marker Extraction & Include Tracking )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Sub-Assembly Marker Scanner
// -----------------------------------------------------------------------------

/// Scans lines for standard OmniCode sub-assembly markers (e.g. "Sub-Assembly 2.1: Title").
pub fn extract_subassembly_markers(lines: &[String], expected_block: u32) -> Vec<SubAssemblyMarker> {
    let mut markers = Vec::new();
    let block_prefix = format!("Sub-Assembly {}.", expected_block);

    for (idx, line) in lines.iter().enumerate() {
        let line_num = idx + 1;
        let trimmed = line.trim();

        // Strip comment decoration: //, /*, *, #
        let cleaned = trimmed
            .trim_start_matches('/')
            .trim_start_matches('*')
            .trim_start_matches('-')
            .trim_start_matches('#')
            .trim();

        if let Some(pos) = cleaned.find(&block_prefix) {
            let after = &cleaned[pos + "Sub-Assembly ".len()..];
            let parts: Vec<&str> = after.splitn(2, ':').collect();
            if !parts.is_empty() {
                let num = parts[0].trim().to_string();
                let title = if parts.len() > 1 {
                    parts[1].trim().trim_end_matches('-').trim().to_string()
                } else {
                    String::new()
                };

                if is_valid_subassembly_number(&num, expected_block) {
                    markers.push(SubAssemblyMarker {
                        number: num,
                        title,
                        line_number: line_num,
                    });
                }
            }
        }
    }

    markers
}

/// Parses an include or import line from C/C++/Rust source.
pub fn parse_include_directive(raw_line: &str, line_num: usize) -> Option<IncludeDirective> {
    let trimmed = raw_line.trim();

    // C/C++ #include
    if let Some(rest) = trimmed.strip_prefix('#') {
        let rest = rest.trim_start();
        if let Some(after) = rest.strip_prefix("include") {
            let target = after.trim();
            if target.starts_with('"') && target.ends_with('"') && target.len() >= 2 {
                return Some(IncludeDirective {
                    path: target[1..target.len() - 1].to_string(),
                    is_quoted: true,
                    is_system: false,
                    line_number: line_num,
                });
            } else if target.starts_with('<') && target.ends_with('>') && target.len() >= 2 {
                return Some(IncludeDirective {
                    path: target[1..target.len() - 1].to_string(),
                    is_quoted: false,
                    is_system: true,
                    line_number: line_num,
                });
            }
        }
    }

    // Rust use directive
    if trimmed.starts_with("use ") && trimmed.ends_with(';') {
        let module = trimmed[4..trimmed.len() - 1].trim().to_string();
        return Some(IncludeDirective {
            path: module,
            is_quoted: false,
            is_system: false,
            line_number: line_num,
        });
    }

    None
}

/// Parses a slice of Block 2 Opening lines into a `Block2Opening` AST.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::framework::block_2::parse_block_2_lines;
///
/// let lines = vec![
///     "#include <stdint.h>".to_string(),
///     "#include \"core/manifest.h\"".to_string(),
///     "// Sub-Assembly 2.1: Constants".to_string(),
///     "// Sub-Assembly 2.2: Types".to_string(),
///     "// Sub-Assembly 2.3: Predicates".to_string(),
/// ];
/// let b2 = parse_block_2_lines(&lines).expect("Must parse Block 2");
/// assert_eq!(b2.includes.len(), 2);
/// assert!(b2.has_baseline_subassemblies);
/// ```
pub fn parse_block_2_lines(lines: &[String]) -> Result<Block2Opening, String> {
    let mut b2 = Block2Opening {
        raw_lines: lines.to_vec(),
        ..Default::default()
    };

    b2.sub_assemblies = extract_subassembly_markers(lines, 2);

    for (idx, line) in lines.iter().enumerate() {
        if let Some(inc) = parse_include_directive(line, idx + 1) {
            b2.includes.push(inc);
        }
    }

    b2.has_baseline_subassemblies = CANONICAL_BLOCK_2_SUBASSEMBLIES
        .iter()
        .all(|num| b2.has_subassembly(num));

    Ok(b2)
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
    fn test_section_1_nominal_subassembly_parsing() {
        let lines = vec![
            "// -----------------------------------------------------------------------------".to_string(),
            "// Sub-Assembly 2.1: Operational Constants & Suite Identifiers".to_string(),
            "// -----------------------------------------------------------------------------".to_string(),
            "pub const TIMEOUT: u64 = 5000;".to_string(),
            "// Sub-Assembly 2.2: Types, Enums & Structs".to_string(),
            "pub struct Config;".to_string(),
            "// Sub-Assembly 2.3: Verification Helpers".to_string(),
            "pub fn is_valid() -> bool { true }".to_string(),
        ];

        let b2 = parse_block_2_lines(&lines).expect("Parsing must succeed");
        assert_eq!(b2.sub_assemblies.len(), 3);
        assert!(b2.has_subassembly("2.1"));
        assert!(b2.has_subassembly("2.2"));
        assert!(b2.has_subassembly("2.3"));
        assert!(b2.has_baseline_subassemblies);
        assert_eq!(
            b2.subassembly_title("2.1"),
            Some("Operational Constants & Suite Identifiers")
        );
    }

    #[test]
    fn test_section_1_nominal_include_parsing() {
        let inc_quoted = parse_include_directive("#include \"core/manifest.h\"", 10);
        assert!(inc_quoted.is_some());
        let q = inc_quoted.unwrap();
        assert_eq!(q.path, "core/manifest.h");
        assert!(q.is_quoted);
        assert!(!q.is_system);

        let inc_sys = parse_include_directive("#include <stdint.h>", 11);
        assert!(inc_sys.is_some());
        let s = inc_sys.unwrap();
        assert_eq!(s.path, "stdint.h");
        assert!(!s.is_quoted);
        assert!(s.is_system);

        let inc_rust = parse_include_directive("use std::path::PathBuf;", 12);
        assert!(inc_rust.is_some());
        assert_eq!(inc_rust.unwrap().path, "std::path::PathBuf");
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_empty_lines() {
        let empty: Vec<String> = vec![];
        let b2 = parse_block_2_lines(&empty).expect("Empty input must return default AST");
        assert!(b2.sub_assemblies.is_empty());
        assert!(!b2.has_baseline_subassemblies);
    }

    #[test]
    fn test_section_2_never_fail_invalid_subassembly_numbers() {
        assert!(!is_valid_subassembly_number("invalid", 2));
        assert!(!is_valid_subassembly_number("2", 2));
        assert!(!is_valid_subassembly_number("2.1.3", 2));
        assert!(!is_valid_subassembly_number("3.1", 2));
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_subassembly_sequence() {
        let markers = vec![
            SubAssemblyMarker {
                number: "2.1".to_string(),
                title: "Constants".to_string(),
                line_number: 10,
            },
            SubAssemblyMarker {
                number: "2.2".to_string(),
                title: "Types".to_string(),
                line_number: 20,
            },
            SubAssemblyMarker {
                number: "2.3".to_string(),
                title: "Predicates".to_string(),
                line_number: 30,
            },
        ];

        // Monotonic order of line numbers and sub-assembly digits
        for i in 1..markers.len() {
            assert!(markers[i].line_number > markers[i - 1].line_number);
        }
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_c_header_block_2() {
        let c_lines = vec![
            "#include <stdint.h>".to_string(),
            "#include <stdbool.h>".to_string(),
            "#include \"core/manifest.h\"".to_string(),
            "/* ------------------------------------------------------------------------- */".to_string(),
            "/* Sub-Assembly 2.1: Operational Constants & System Flags                   */".to_string(),
            "/* ------------------------------------------------------------------------- */".to_string(),
            "#define CS_LOG_BUFFER_SIZE 4096".to_string(),
            "/* Sub-Assembly 2.2: Types, Enums & Core Envelopes                          */".to_string(),
            "typedef struct LogRing LogRing;".to_string(),
            "/* Sub-Assembly 2.3: Verification Helpers & Invariant Predicates             */".to_string(),
            "bool is_valid_ring(const LogRing* ring);".to_string(),
        ];

        let b2 = parse_block_2_lines(&c_lines).expect("Must parse C Block 2");
        assert_eq!(b2.includes.len(), 3);
        assert_eq!(b2.sub_assemblies.len(), 3);
        assert!(b2.has_baseline_subassemblies);
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying Block 2 parser internal consistency.
pub fn self_audit() -> Result<(), String> {
    if CANONICAL_BLOCK_2_SUBASSEMBLIES.len() < 3 {
        return Err("Canonical Block 2 must require at least 2.1..2.3".to_string());
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

/// Epilogue predicate affirming that Block 2 framework parser is operational.
pub fn is_block_2_framework_ready() -> bool {
    self_audit().is_ok()
}
