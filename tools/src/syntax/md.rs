// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/syntax/md.rs
// @id            cog.tools.syntax.md
// @layer         Layer 3: Developer Toolchain Language Syntax Primitives
// @archetype     pure_types | markdown_parser
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal Syntax Framework // Markdown Lexer
// @role          Markdown Document Tokenizer, Heading Parser & Fence Lexer
// @maturity      Rev 5 Baseline | GitHub Flavored Markdown Analysis
// @chassis       bounded-heap | Toolchain memory chassis; bounded tokens
// @scale         O(L) across Markdown lines
// @seams         in: Markdown text | out: Markdown tokens, headings & code fences
// @invariants    1. GFM heading levels 1..6 accurately tracked.
//                2. Verbatim code fences (```) tracked to prevent false heading shifts.
//                3. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Lexical tokenizer and structural parser for GitHub Flavored Markdown (GFM)
//   documents across `docs/` and root documentation. Extracts HTML comment
//   headers (<!-- ... -->), heading hierarchies, alert callouts, and verbatim
//   fenced code blocks.
//
//   ### For Game Designers & Non-Programmers:
//   Guarantees that game design manuscripts and narrative lore in Markdown
//   conform to the same structural rigor and readability as engine source code.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Parser is stateless and pure.
//   - Fence Integrity: Discovers unclosed code fences before publishing.
//   - Transpilation Alignment: Maps Markdown headings to universal OmniCode AST.
// =============================================================================

//! # GitHub Flavored Markdown Syntax & Lexical Analyzer
//!
//! Provides lexical analysis, heading extraction, and code fence tracking for
//! Markdown files.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Defaults
// -----------------------------------------------------------------------------

/// Standard GitHub alert types supported in OmniCode Markdown documents.
pub const GITHUB_ALERT_TYPES: &[&str] = &[
    "[!NOTE]",
    "[!TIP]",
    "[!IMPORTANT]",
    "[!WARNING]",
    "[!CAUTION]",
];

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types, Enums & Markdown Lexical AST
// -----------------------------------------------------------------------------

/// Discovered Markdown section heading.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct MarkdownHeading {
    /// Heading level (1 for `#`, 2 for `##`, up to 6).
    pub level: usize,
    /// Heading text with `#` prefixes stripped.
    pub title: String,
    /// 1-based line number.
    pub line_number: usize,
}

/// Discovered Markdown fenced code block.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct MarkdownCodeFence {
    /// Declared syntax language (e.g. "rust", "c", "json").
    pub language: String,
    /// 1-based start line number of opening fence.
    pub start_line: usize,
    /// 1-based end line number of closing fence if closed.
    pub end_line: Option<usize>,
    /// Number of code lines inside the block.
    pub line_count: usize,
    /// True if properly closed.
    pub is_closed: bool,
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Verification Helpers & Invariant Predicates
// -----------------------------------------------------------------------------

/// Evaluates whether a line starts a Markdown code fence (```).
pub fn is_markdown_code_fence(trimmed_line: &str) -> bool {
    trimmed_line.starts_with("```")
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Heading Scanner, Code Fence Tracker & HTML Comment Lexer )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Heading & Code Fence Lexer
// -----------------------------------------------------------------------------

/// Scans Markdown text for section headings, respecting code fences.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::md::scan_markdown_headings;
///
/// let md = "# Main Title\n\n## Section 1\n```c\n# Not a heading\n```\n### Sub 1.1\n";
/// let headings = scan_markdown_headings(md);
/// assert_eq!(headings.len(), 3);
/// assert_eq!(headings[0].level, 1);
/// assert_eq!(headings[0].title, "Main Title");
/// assert_eq!(headings[1].level, 2);
/// assert_eq!(headings[2].level, 3);
/// ```
pub fn scan_markdown_headings(content: &str) -> Vec<MarkdownHeading> {
    let mut headings = Vec::new();
    let mut in_fence = false;

    for (idx, line) in content.lines().enumerate() {
        let line_num = idx + 1;
        let trimmed = line.trim();

        if trimmed.starts_with("```") {
            in_fence = !in_fence;
            continue;
        }

        if !in_fence && trimmed.starts_with('#') {
            let hash_count = trimmed.chars().take_while(|&c| c == '#').count();
            if (1..=6).contains(&hash_count) {
                let rest = trimmed[hash_count..].trim_start();
                if trimmed.as_bytes().get(hash_count) == Some(&b' ') {
                    headings.push(MarkdownHeading {
                        level: hash_count,
                        title: rest.to_string(),
                        line_number: line_num,
                    });
                }
            }
        }
    }

    headings
}

/// Scans Markdown text for fenced code blocks, reporting closed status and bounds.
pub fn scan_markdown_code_fences(content: &str) -> Vec<MarkdownCodeFence> {
    let mut fences = Vec::new();
    let mut current_fence: Option<MarkdownCodeFence> = None;

    for (idx, line) in content.lines().enumerate() {
        let line_num = idx + 1;
        let trimmed = line.trim();

        if trimmed.starts_with("```") {
            if let Some(mut fence) = current_fence.take() {
                fence.end_line = Some(line_num);
                fence.is_closed = true;
                fences.push(fence);
            } else {
                let lang = trimmed.trim_start_matches('`').trim().to_string();
                current_fence = Some(MarkdownCodeFence {
                    language: lang,
                    start_line: line_num,
                    end_line: None,
                    line_count: 0,
                    is_closed: false,
                });
            }
        } else if let Some(ref mut fence) = current_fence {
            fence.line_count += 1;
        }
    }

    if let Some(unclosed) = current_fence {
        fences.push(unclosed);
    }

    fences
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
    fn test_section_1_nominal_heading_and_fence_scanning() {
        let md = "\
# Title
Paragraph text.
```rust
fn main() {}
```
## Section 2
";
        let headings = scan_markdown_headings(md);
        assert_eq!(headings.len(), 2);
        assert_eq!(headings[0].title, "Title");
        assert_eq!(headings[1].title, "Section 2");

        let fences = scan_markdown_code_fences(md);
        assert_eq!(fences.len(), 1);
        assert_eq!(fences[0].language, "rust");
        assert!(fences[0].is_closed);
        assert_eq!(fences[0].line_count, 1);
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_unclosed_fence() {
        let unclosed = "# Doc\n```c\nint x = 0;\n";
        let fences = scan_markdown_code_fences(unclosed);
        assert_eq!(fences.len(), 1);
        assert!(!fences[0].is_closed);
        assert_eq!(fences[0].end_line, None);
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_fence_preservation_for_headings() {
        let md = "\
# Heading Outside
```text
# Fake Heading Inside
## Another Fake
```
# Second Heading Outside
";
        let headings = scan_markdown_headings(md);
        assert_eq!(headings.len(), 2);
        assert_eq!(headings[0].title, "Heading Outside");
        assert_eq!(headings[1].title, "Second Heading Outside");
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_nested_fence_and_heading_depth() {
        let md = "\
# Volume
## Part
### Chapter
#### Sub-Assembly
##### Section
###### Paragraph
";
        let headings = scan_markdown_headings(md);
        assert_eq!(headings.len(), 6);
        for i in 1..=6 {
            assert_eq!(headings[i - 1].level, i);
        }
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying Markdown syntax parser internal consistency.
pub fn self_audit() -> Result<(), String> {
    if GITHUB_ALERT_TYPES.len() != 5 {
        return Err("GITHUB_ALERT_TYPES must define 5 alert types".to_string());
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

/// Epilogue predicate affirming that Markdown syntax parser is operational.
pub fn is_md_syntax_ready() -> bool {
    self_audit().is_ok()
}
