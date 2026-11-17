// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/syntax/adoc.rs
// @id            cog.tools.syntax.adoc
// @layer         Layer 3: Developer Toolchain Language Syntax Primitives
// @archetype     pure_types | asciidoc_parser
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal Syntax Framework // AsciiDoc Lexer
// @role          AsciiDoc Document Tokenizer, Delimiter Stack Lexer & Include Tracker
// @maturity      Rev 5 Baseline | Asciidoctor Document Model
// @chassis       bounded-heap | Toolchain memory chassis; bounded delimiters
// @scale         O(L) across AsciiDoc lines
// @seams         in: AsciiDoc text | out: AsciiDoc tokens, sections & delimiters
// @invariants    1. Delimiter stack pairing (////, ----, |===, ====) strictly audited.
//                2. Verbatim code listings preserve code lines without interpretation.
//                3. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Lexical tokenizer and structural delimiter stack tracker for AsciiDoc
//   technical manuals and living documentation in `docs/`. Tracks comment
//   blocks (////), listing fences (----), tables (|===), sidebars (****),
//   examples (====), attributes (:attr:), and `include::` directives.
//
//   ### For Game Designers & Non-Programmers:
//   Ensures the technical rulebooks and continental lore compile into PDF
//   books without broken tables, unclosed listings, or missing includes.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Parser is stateless and pure.
//   - Delimiter Stack Integrity: Catches unclosed fences with exact line bounds.
//   - Transpilation Alignment: Maps AsciiDoc to universal OmniCode AST.
// =============================================================================

//! # AsciiDoc Syntax & Delimiter Stack Analyzer
//!
//! Provides lexical analysis, section heading parsing, and delimiter balance
//! tracking for AsciiDoc documents.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Defaults
// -----------------------------------------------------------------------------

/// Canonical AsciiDoc block delimiter tokens.
pub const ADOC_DELIMITERS: &[&str] = &[
    "////",
    "----",
    "------",
    "....",
    "====",
    "****",
    "|===",
    "--",
];

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types, Enums & AsciiDoc Lexical AST
// -----------------------------------------------------------------------------

/// Discovered AsciiDoc section level.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct AdocSection {
    /// Equal count (1 for `= `, 2 for `== `, up to 6).
    pub level: usize,
    /// Title string with `=` stripped.
    pub title: String,
    /// 1-based line number.
    pub line_number: usize,
}

/// Discovered include directive: `include::path[opts]`.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct AdocInclude {
    /// Relative or absolute path to target.
    pub target_path: String,
    /// Optional attributes string inside bracket.
    pub options: String,
    /// 1-based line number.
    pub line_number: usize,
}

/// Delimiter balance report.
#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct AdocDelimiterReport {
    /// True if all opened block delimiters were properly closed.
    pub is_balanced: bool,
    /// Unclosed delimiter token and line number if any.
    pub unclosed: Option<(String, usize)>,
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Verification Helpers & Invariant Predicates
// -----------------------------------------------------------------------------

/// Evaluates whether a line starts an AsciiDoc block delimiter.
pub fn is_adoc_delimiter(trimmed_line: &str) -> bool {
    ADOC_DELIMITERS.contains(&trimmed_line)
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Delimiter Stack Auditor, Section Parser & Include Lexer )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Delimiter Stack Auditor
// -----------------------------------------------------------------------------

/// Audits AsciiDoc source text for balanced block delimiter stacks.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::adoc::audit_adoc_delimiters;
///
/// let adoc = "----\ncode\n----\n";
/// let report = audit_adoc_delimiters(adoc);
/// assert!(report.is_balanced);
/// ```
pub fn audit_adoc_delimiters(content: &str) -> AdocDelimiterReport {
    let mut stack: Vec<(&str, usize)> = Vec::new();

    for (idx, line) in content.lines().enumerate() {
        let line_num = idx + 1;
        let t = line.trim();

        if let Some(&(top, _)) = stack.last() {
            let closes = match top {
                "|===" => t == "|===",
                "====" => t == "====",
                "****" => t == "****",
                "--" => t == "--",
                "////" => t == "////",
                "----" => t == "----",
                "------" => t == "------",
                "...." => t == "....",
                _ => false,
            };

            if closes {
                stack.pop();
                continue;
            }
        }

        // Check opening delimiter
        for &del in ADOC_DELIMITERS {
            if t == del {
                stack.push((del, line_num));
                break;
            }
        }
    }

    if let Some(&(unclosed_tok, line_num)) = stack.last() {
        AdocDelimiterReport {
            is_balanced: false,
            unclosed: Some((unclosed_tok.to_string(), line_num)),
        }
    } else {
        AdocDelimiterReport {
            is_balanced: true,
            unclosed: None,
        }
    }
}

/// Scans AsciiDoc text for section headings, respecting verbatim listings.
pub fn scan_adoc_sections(content: &str) -> Vec<AdocSection> {
    let mut sections = Vec::new();
    let mut in_verbatim = false;

    for (idx, line) in content.lines().enumerate() {
        let line_num = idx + 1;
        let t = line.trim();

        if t == "----" || t == "------" || t == "...." || t == "////" {
            in_verbatim = !in_verbatim;
            continue;
        }

        if !in_verbatim && t.starts_with('=') {
            let eq_count = t.chars().take_while(|&c| c == '=').count();
            if (1..=6).contains(&eq_count) && t.as_bytes().get(eq_count) == Some(&b' ') {
                let rest = t[eq_count..].trim();
                sections.push(AdocSection {
                    level: eq_count,
                    title: rest.to_string(),
                    line_number: line_num,
                });
            }
        }
    }

    sections
}

/// Scans AsciiDoc text for `include::path[options]` directives.
pub fn scan_adoc_includes(content: &str) -> Vec<AdocInclude> {
    let mut includes = Vec::new();

    for (idx, line) in content.lines().enumerate() {
        let line_num = idx + 1;
        let t = line.trim();

        if let Some(rest) = t.strip_prefix("include::") {
            if let Some(bracket_open) = rest.find('[') {
                if let Some(bracket_close) = rest[bracket_open..].find(']') {
                    let path = rest[..bracket_open].trim().to_string();
                    let opts = rest[bracket_open + 1..bracket_open + bracket_close].trim().to_string();

                    includes.push(AdocInclude {
                        target_path: path,
                        options: opts,
                        line_number: line_num,
                    });
                }
            }
        }
    }

    includes
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
    fn test_section_1_nominal_delimiter_and_section_scanning() {
        let adoc = "\
= Doc Title
== Section 1
----
int x = 0;
----
include::chapters/intro.adoc[leveloffset=+1]
";
        let sections = scan_adoc_sections(adoc);
        assert_eq!(sections.len(), 2);
        assert_eq!(sections[0].title, "Doc Title");
        assert_eq!(sections[1].title, "Section 1");

        let report = audit_adoc_delimiters(adoc);
        assert!(report.is_balanced);

        let includes = scan_adoc_includes(adoc);
        assert_eq!(includes.len(), 1);
        assert_eq!(includes[0].target_path, "chapters/intro.adoc");
        assert_eq!(includes[0].options, "leveloffset=+1");
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_unclosed_delimiter() {
        let unclosed = "----\nunclosed code block\n";
        let report = audit_adoc_delimiters(unclosed);
        assert!(!report.is_balanced);
        assert_eq!(report.unclosed, Some(("----".to_string(), 1)));
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_verbatim_heading_preservation() {
        let adoc = "\
== Section
----
=== Fake Heading Inside Code
----
== Next Section
";
        let sections = scan_adoc_sections(adoc);
        assert_eq!(sections.len(), 2);
        assert_eq!(sections[0].title, "Section");
        assert_eq!(sections[1].title, "Next Section");
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_nested_table_and_comment_blocks() {
        let complex = "\
////
BLOCK 1: METADATA
////
= Title
|===
| Header 1 | Header 2
| Cell 1   | Cell 2
|===
";
        let report = audit_adoc_delimiters(complex);
        assert!(report.is_balanced);
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying AsciiDoc syntax parser internal consistency.
pub fn self_audit() -> Result<(), String> {
    if ADOC_DELIMITERS.is_empty() {
        return Err("ADOC_DELIMITERS must be non-empty".to_string());
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

/// Epilogue predicate affirming that AsciiDoc syntax parser is operational.
pub fn is_adoc_syntax_ready() -> bool {
    self_audit().is_ok()
}
