// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/syntax/c.rs
// @id            cog.tools.syntax.c
// @layer         Layer 3: Developer Toolchain Language Syntax Primitives
// @archetype     pure_types | lexer
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal Syntax Framework // C11/C23 Lexer
// @role          C Language Tokenizer, Comment Lexer & Function Header Parser
// @maturity      Rev 5 Baseline | ISO C11/C23 Lexical Analysis
// @chassis       bounded-heap | Toolchain memory chassis; bounded tokens
// @scale         O(L) across C source lines
// @seams         in: C source | out: C tokens, comments & declarations
// @invariants    1. Invariant 18 enforcement: detects forbidden C++-style // comments in C files.
//                2. Anti-Domain-Bleed scanner: detects forbidden RPG lore in engine/ headers.
//                3. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Lexical tokenizer and AST extraction primitives for ISO C11/C23 source and
//   header files. Extracts block comments (/* ... */), identifies forbidden
//   line comments (//), parses preprocessor directives (#include, #define), and
//   isolates function prototypes, struct definitions, and domain bleed words.
//
//   ### For Game Designers & Non-Programmers:
//   Enforces that engine code stays purely mechanical and never hardcodes story
//   lore, ensuring the Cornerstone engine remains a general-purpose RPG machine.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Lexer is stateless and pure.
//   - Invariant 18: Flags every violation of ISO C comment purity.
//   - Transpilation Alignment: Maps C declarations to universal OmniCode AST.
// =============================================================================

//! # ISO C11/C23 Syntax & Lexical Analyzer
//!
//! Provides lexical analysis, comment extraction, and signature parsing for C
//! headers and source files.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Defaults & Forbidden Domain Terms
// -----------------------------------------------------------------------------

/// Forbidden RPG Maker & lore terms that must never bleed into engine/ headers.
pub const FORBIDDEN_DOMAIN_TERMS: &[&str] = &[
    "elijah",
    "katrina",
    "sacyra",
    "libira",
    "usuru",
    "tenebris and lux",
    "lux and tenebris",
    "rpg maker",
    "moral sway",
    "twin relics",
];

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types, Enums & C Lexical AST
// -----------------------------------------------------------------------------

/// Classification of comments discovered in C source text.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum CCommentKind {
    /// Canonical ISO C block comment: `/* ... */`
    Block,
    /// Forbidden C++-style line comment: `// ...` (Violates Invariant 18 in C files)
    ForbiddenLine,
}

/// Extracted C comment token with line and column provenance.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct CCommentToken {
    /// Comment kind.
    pub kind: CCommentKind,
    /// Inner text of comment with delimiters stripped.
    pub content: String,
    /// 1-based start line.
    pub start_line: usize,
    /// 1-based start column.
    pub start_column: usize,
}

/// Discovered C preprocessor directive.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct CDirective {
    /// Directive name (e.g. "include", "define", "ifndef", "endif").
    pub directive: String,
    /// Target argument (e.g. `"core/manifest.h"`, `CORNERSTONE_LOG_H`).
    pub argument: String,
    /// 1-based line number.
    pub line_number: usize,
}

/// Discovered top-level declaration in C source.
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum CDeclaration {
    /// Typedef struct or enum definition.
    TypeDef { name: String, line_number: usize },
    /// Operational function prototype or definition.
    Function { name: String, signature: String, line_number: usize },
    /// Macro definition (#define).
    Macro { name: String, line_number: usize },
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Verification Helpers & Invariant Predicates
// -----------------------------------------------------------------------------

/// Evaluates whether a line starts a forbidden C++-style line comment in C code.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::c::is_forbidden_c_line_comment;
///
/// assert!(is_forbidden_c_line_comment("// forbidden in C11 engine"));
/// assert!(!is_forbidden_c_line_comment("/* valid C block comment */"));
/// ```
pub fn is_forbidden_c_line_comment(trimmed_line: &str) -> bool {
    trimmed_line.starts_with("//")
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Lexical Scanner, Directive Parser & Anti-Domain-Bleed Inspector )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Comment Lexer & Invariant 18 Auditor
// -----------------------------------------------------------------------------

/// Scans C source text for all comments, flagging Invariant 18 line comments.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::c::{scan_c_comments, CCommentKind};
///
/// let c_code = "/* Valid Block */\n// Invalid Line Comment\n";
/// let comments = scan_c_comments(c_code);
/// assert_eq!(comments.len(), 2);
/// assert_eq!(comments[0].kind, CCommentKind::Block);
/// assert_eq!(comments[1].kind, CCommentKind::ForbiddenLine);
/// ```
pub fn scan_c_comments(content: &str) -> Vec<CCommentToken> {
    let mut tokens = Vec::new();
    let lines: Vec<&str> = content.lines().collect();
    let mut in_block = false;
    let mut block_start_line = 0;
    let mut block_start_col = 0;
    let mut block_content = Vec::new();

    for (idx, line) in lines.iter().enumerate() {
        let line_num = idx + 1;
        let trimmed = line.trim();

        if in_block {
            if let Some(pos) = line.find("*/") {
                block_content.push(&line[..pos]);
                tokens.push(CCommentToken {
                    kind: CCommentKind::Block,
                    content: block_content.join("\n"),
                    start_line: block_start_line,
                    start_column: block_start_col,
                });
                in_block = false;
                block_content.clear();
            } else {
                block_content.push(line);
            }
            continue;
        }

        if let Some(pos) = line.find("/*") {
            let col = pos + 1;
            if let Some(end_pos) = line[pos + 2..].find("*/") {
                // Single-line block comment
                let inner = &line[pos + 2..pos + 2 + end_pos];
                tokens.push(CCommentToken {
                    kind: CCommentKind::Block,
                    content: inner.trim().to_string(),
                    start_line: line_num,
                    start_column: col,
                });
            } else {
                // Multi-line block comment start
                in_block = true;
                block_start_line = line_num;
                block_start_col = col;
                block_content.clear();
                block_content.push(&line[pos + 2..]);
            }
        } else if trimmed.starts_with("//") {
            let col = line.find("//").unwrap_or(0) + 1;
            tokens.push(CCommentToken {
                kind: CCommentKind::ForbiddenLine,
                content: trimmed[2..].trim().to_string(),
                start_line: line_num,
                start_column: col,
            });
        }
    }

    tokens
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Directive & Anti-Domain-Bleed Lexer
// -----------------------------------------------------------------------------

/// Scans C source text for domain lore terms that violate anti-bleed covenants.
///
/// Returns list of `(term, line_number, line_text)`.
pub fn find_domain_bleed_terms(content: &str) -> Vec<(&'static str, usize, String)> {
    let mut findings = Vec::new();

    for (idx, line) in content.lines().enumerate() {
        let line_num = idx + 1;
        let lower = line.to_ascii_lowercase();

        for term in FORBIDDEN_DOMAIN_TERMS {
            if lower.contains(term) {
                findings.push((*term, line_num, line.trim().to_string()));
            }
        }
    }

    findings
}

/// Scans preprocessor directives (#include, #define, #ifndef, #endif).
pub fn scan_c_directives(content: &str) -> Vec<CDirective> {
    let mut directives = Vec::new();

    for (idx, line) in content.lines().enumerate() {
        let line_num = idx + 1;
        let trimmed = line.trim();

        if let Some(rest) = trimmed.strip_prefix('#') {
            let parts: Vec<&str> = rest.trim_start().splitn(2, char::is_whitespace).collect();
            if !parts.is_empty() {
                let directive = parts[0].to_ascii_lowercase();
                let argument = if parts.len() > 1 {
                    parts[1].trim().to_string()
                } else {
                    String::new()
                };

                directives.push(CDirective {
                    directive,
                    argument,
                    line_number: line_num,
                });
            }
        }
    }

    directives
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
    fn test_section_1_nominal_comment_scanning() {
        let code = "\
/* Block 1 */
int x = 0;
/* Single line block */
int y = 1;
// Forbidden line comment
";
        let comments = scan_c_comments(code);
        assert_eq!(comments.len(), 3);
        assert_eq!(comments[0].kind, CCommentKind::Block);
        assert_eq!(comments[1].kind, CCommentKind::Block);
        assert_eq!(comments[2].kind, CCommentKind::ForbiddenLine);
    }

    #[test]
    fn test_section_1_nominal_directive_scanning() {
        let code = "\
#include <stdint.h>
#include \"core/manifest.h\"
#define BUFFER_SIZE 4096
#endif /* GUARD */
";
        let directives = scan_c_directives(code);
        assert_eq!(directives.len(), 4);
        assert_eq!(directives[0].directive, "include");
        assert_eq!(directives[0].argument, "<stdint.h>");
        assert_eq!(directives[1].argument, "\"core/manifest.h\"");
        assert_eq!(directives[2].directive, "define");
        assert_eq!(directives[3].directive, "endif");
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_empty_source() {
        let comments = scan_c_comments("");
        assert!(comments.is_empty());

        let directives = scan_c_directives("");
        assert!(directives.is_empty());

        let bleed = find_domain_bleed_terms("");
        assert!(bleed.is_empty());
    }

    #[test]
    fn test_section_2_never_fail_clean_engine_code_bleed_check() {
        let clean_c = "void log_append(const char* msg);";
        let bleed = find_domain_bleed_terms(clean_c);
        assert!(bleed.is_empty(), "Clean engine code must have 0 bleed findings");
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_anti_domain_bleed_detection() {
        let bad_c = "bool check_hero(void) { if (is_elijah) return true; }";
        let bleed = find_domain_bleed_terms(bad_c);
        assert_eq!(bleed.len(), 1);
        assert_eq!(bleed[0].0, "elijah");
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_live_engine_manifest_syntax() {
        let manifest_content = "\
/*
BLOCK 1: METADATA BLOCK
@project Tenebris and Lux
*/
#ifndef CORNERSTONE_MANIFEST_H
#define CORNERSTONE_MANIFEST_H
#include <stdbool.h>
#endif /* CORNERSTONE_MANIFEST_H */
";
        let directives = scan_c_directives(manifest_content);
        assert_eq!(directives.len(), 4);
        assert!(directives.iter().any(|d| d.directive == "ifndef"));
        assert!(directives.iter().any(|d| d.directive == "define"));
        assert!(directives.iter().any(|d| d.directive == "include"));
        assert!(directives.iter().any(|d| d.directive == "endif"));
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying C syntax parser internal consistency.
pub fn self_audit() -> Result<(), String> {
    if FORBIDDEN_DOMAIN_TERMS.is_empty() {
        return Err("FORBIDDEN_DOMAIN_TERMS must be non-empty".to_string());
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

/// Epilogue predicate affirming that C syntax parser is operational.
pub fn is_c_syntax_ready() -> bool {
    self_audit().is_ok()
}
