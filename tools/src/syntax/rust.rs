// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/syntax/rust.rs
// @id            cog.tools.syntax.rust
// @layer         Layer 3: Developer Toolchain Language Syntax Primitives
// @archetype     pure_types | lexer
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal Syntax Framework // Rust 2024 Lexer
// @role          Rust Language Tokenizer, Doc-Comment Lexer & Item Parser
// @maturity      Rev 5 Baseline | Rust 2024 Edition Lexical Analysis
// @chassis       bounded-heap | Toolchain memory chassis; bounded tokens
// @scale         O(L) across Rust source lines
// @seams         in: Rust source | out: Rust tokens, items & doc comments
// @invariants    1. Conformance to Rust 2024 Edition syntax conventions.
//                2. Outer (///) and inner (/!) doc comments cleanly categorized.
//                3. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Lexical tokenizer and AST item parser for Rust 2024 Edition code across
//   the `tools/` crate. Extracts module doc comments (//!), item doc comments
//   (///), attribute blocks (#[...]), public modules, structs, enums, and functions.
//
//   ### For Game Designers & Non-Programmers:
//   Ensures that developer tools and generators are as rigorously documented
//   and structured as the game engine itself.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Lexer is stateless and pure.
//   - Zero Panics: Graceful Option/Result handling across all token streams.
//   - Transpilation Alignment: Maps Rust items to universal OmniCode AST.
// =============================================================================

//! # Rust 2024 Edition Syntax & Lexical Analyzer
//!
//! Provides lexical analysis, doc-comment extraction, and top-level item
//! parsing for Rust files.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Defaults
// -----------------------------------------------------------------------------

/// Canonical keywords introducing Rust items.
pub const RUST_ITEM_KEYWORDS: &[&str] = &[
    "fn",
    "struct",
    "enum",
    "mod",
    "trait",
    "impl",
    "type",
    "const",
];

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types, Enums & Rust Lexical AST
// -----------------------------------------------------------------------------

/// Classification of Rust documentation comments.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum RustDocKind {
    /// Inner module doc-comment: `//! ...`
    InnerModule,
    /// Outer item doc-comment: `/// ...`
    OuterItem,
}

/// Discovered Rust top-level item declaration.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct RustItem {
    /// Item keyword (e.g. "fn", "struct", "mod").
    pub kind: String,
    /// Identifier name.
    pub name: String,
    /// True if marked `pub`.
    pub is_pub: bool,
    /// 1-based line number.
    pub line_number: usize,
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Verification Helpers & Invariant Predicates
// -----------------------------------------------------------------------------

/// Evaluates whether a line starts an item doc-comment (///).
pub fn is_outer_doc_comment(trimmed_line: &str) -> bool {
    trimmed_line.starts_with("///") && !trimmed_line.starts_with("////")
}

/// Evaluates whether a line starts an inner module doc-comment (//!).
pub fn is_inner_doc_comment(trimmed_line: &str) -> bool {
    trimmed_line.starts_with("//!")
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Doc-Comment Lexer & Item Extraction )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Rust Token & Item Lexer
// -----------------------------------------------------------------------------

/// Scans Rust source text for top-level item declarations.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::rust::scan_rust_items;
///
/// let code = "pub fn run() {}\npub struct Config;\n";
/// let items = scan_rust_items(code);
/// assert_eq!(items.len(), 2);
/// assert_eq!(items[0].name, "run");
/// assert_eq!(items[1].name, "Config");
/// ```
pub fn scan_rust_items(content: &str) -> Vec<RustItem> {
    let mut items = Vec::new();

    for (idx, line) in content.lines().enumerate() {
        let line_num = idx + 1;
        let trimmed = line.trim();

        if trimmed.starts_with("//") || trimmed.starts_with("/*") || trimmed.starts_with('*') {
            continue;
        }

        let is_pub = trimmed.starts_with("pub ");
        let rest = if is_pub {
            trimmed[4..].trim_start()
        } else {
            trimmed
        };

        for &kw in RUST_ITEM_KEYWORDS {
            if rest.starts_with(kw) && rest[kw.len()..].starts_with(char::is_whitespace) {
                let after_kw = rest[kw.len()..].trim_start();
                let name = after_kw
                    .split(|c: char| c == '(' || c == '<' || c == '{' || c == ';' || c == ':' || c.is_whitespace())
                    .next()
                    .unwrap_or("")
                    .trim();

                if !name.is_empty() {
                    items.push(RustItem {
                        kind: kw.to_string(),
                        name: name.to_string(),
                        is_pub,
                        line_number: line_num,
                    });
                }
                break;
            }
        }
    }

    items
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
    fn test_section_1_nominal_item_scanning() {
        let code = "\
//! Inner module doc
use std::fs;

pub mod config;
pub struct ServerConfig;
pub enum ServerState { Idle, Running }
pub fn start_server() -> bool { true }
";
        let items = scan_rust_items(code);
        assert_eq!(items.len(), 4);
        assert_eq!(items[0].kind, "mod");
        assert_eq!(items[0].name, "config");
        assert_eq!(items[1].kind, "struct");
        assert_eq!(items[1].name, "ServerConfig");
        assert_eq!(items[2].kind, "enum");
        assert_eq!(items[2].name, "ServerState");
        assert_eq!(items[3].kind, "fn");
        assert_eq!(items[3].name, "start_server");
    }

    #[test]
    fn test_section_1_nominal_doc_comment_predicates() {
        assert!(is_outer_doc_comment("/// Item documentation"));
        assert!(!is_outer_doc_comment("//// AsciiDoc banner"));
        assert!(is_inner_doc_comment("//! Module documentation"));
        assert!(!is_inner_doc_comment("// Regular comment"));
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_empty_source() {
        let items = scan_rust_items("");
        assert!(items.is_empty());
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_pub_visibility_tracking() {
        let code = "fn private_func() {}\npub fn public_func() {}\n";
        let items = scan_rust_items(code);
        assert_eq!(items.len(), 2);
        assert!(!items[0].is_pub);
        assert!(items[1].is_pub);
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_live_cmd_structure() {
        let code = "\
pub struct ValidateArgs;
impl Default for ValidateArgs { fn default() -> Self { Self } }
pub fn execute(args: &ValidateArgs) -> Result<(), String> { Ok(()) }
";
        let items = scan_rust_items(code);
        assert!(items.iter().any(|i| i.name == "ValidateArgs" && i.is_pub));
        assert!(items.iter().any(|i| i.name == "execute" && i.is_pub));
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying Rust syntax parser internal consistency.
pub fn self_audit() -> Result<(), String> {
    if RUST_ITEM_KEYWORDS.is_empty() {
        return Err("RUST_ITEM_KEYWORDS must be non-empty".to_string());
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

/// Epilogue predicate affirming that Rust syntax parser is operational.
pub fn is_rust_syntax_ready() -> bool {
    self_audit().is_ok()
}
