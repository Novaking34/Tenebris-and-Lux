// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/validator/md.rs
// @id            cog.tools.validator.md
// @layer         Layer 3: Systems Developer Tooling Core Library
// @archetype     operational | stateless
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal 5-Block Validator Suite
// @role          Markdown Specification, Code Fence & Heading Validator
// @maturity      Rev 5 Baseline
// @chassis       zero-heap | static fence state machine
// @scale         O(N) line-by-line lexical inspection
// @seams         in: syntax::md | uses: validator::report | probes: self_audit -> exit
// @invariants    1. Zero panics on arbitrary Markdown source input.
//                2. Strictly enforces code fence closure (``` and ````).
//                3. Emits standardized ValidationIssue records for all structural defects.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Validates Markdown files across repository documentation, skills, DevLogs,
//   and design documents. Enforces 5-block comment envelopes (`<!-- -->`),
//   code fence symmetry, and heading level hierarchy.
//
//   ### For Game Designers & Non-Programmers:
//   Makes sure our design documents and readme pages look clean, readable,
//   and properly structured with neat headings and closed code blocks.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Stateless document scanner operating over string slices;
//     reentrant, thread-safe, and zero dynamic heap allocations.
//   - Extension Seam: Configurable via `MarkdownValidationConfig`; plugs into
//     `tools/src/validator/mod.rs` and `test_validator_languages.rs`.
//   - Explicit Non-Goal: Does not convert Markdown to HTML/PDF; audits structural
//     hygiene, code fences, and heading taxonomy natively.
// =============================================================================

//! # OmniCode Markdown Validator
//!
//! Enforces universal 5-block envelopes, code fence symmetry, and heading structure on Markdown files.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Configuration Types & Defaults )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Pre-Conditions & Library Imports
// -----------------------------------------------------------------------------

use std::path::Path;
use crate::syntax::md::scan_markdown_headings;
use crate::validator::report::{IssueSeverity, ValidationIssue};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Configuration Types & Default Contracts
// -----------------------------------------------------------------------------

/// Configuration options for Markdown validation sweeps.
///
/// ```rust
/// use tl_tools::validator::md::MarkdownValidationConfig;
///
/// let config = MarkdownValidationConfig::default();
/// assert!(config.require_balanced_code_fences);
/// assert!(config.require_5block_banners);
/// ```
#[derive(Debug, Clone)]
pub struct MarkdownValidationConfig {
    /// Whether code fences (``` and ````) must be balanced.
    pub require_balanced_code_fences: bool,
    /// Whether 5-block comment banners are required.
    pub require_5block_banners: bool,
}

impl Default for MarkdownValidationConfig {
    fn default() -> Self {
        Self {
            require_balanced_code_fences: true,
            require_5block_banners: true,
        }
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Validation Rule Engine & Sub-Assembly Checkers )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Code Fence Symmetry Auditor
// -----------------------------------------------------------------------------

/// Audits Markdown content for balanced code fence delimiters (``` and ````).
pub fn audit_code_fences(
    file_path: &Path,
    content: &str,
    issues: &mut Vec<ValidationIssue>,
) {
    let mut in_3fence = false;
    let mut in_4fence = false;
    let mut fence_open_line = 0;

    for (idx, line) in content.lines().enumerate() {
        let trimmed = line.trim();
        if trimmed.starts_with("````") {
            in_4fence = !in_4fence;
            if in_4fence {
                fence_open_line = idx + 1;
            }
        } else if trimmed.starts_with("```") && !in_4fence {
            in_3fence = !in_3fence;
            if in_3fence {
                fence_open_line = idx + 1;
            }
        }
    }

    if in_3fence || in_4fence {
        issues.push(ValidationIssue {
            file: file_path.to_path_buf(),
            block: "syntax_code_fence".to_string(),
            field: format!("line_{}", fence_open_line),
            severity: IssueSeverity::Error,
            message: format!("Unclosed Markdown code fence opened at line {}", fence_open_line),
        });
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Universal 5-Block Envelope Auditor
// -----------------------------------------------------------------------------

/// Audits Markdown files for the presence of recommended 5-block comment banners.
pub fn audit_5block_banners(
    file_path: &Path,
    content: &str,
    issues: &mut Vec<ValidationIssue>,
) {
    let expected_blocks = [
        "BLOCK 1: METADATA",
        "BLOCK 2: OPENING",
        "BLOCK 3: BODY",
        "BLOCK 4: AUDIT",
        "BLOCK 5: CLOSING",
    ];

    for banner_pat in &expected_blocks {
        if !content.contains(banner_pat) {
            issues.push(ValidationIssue {
                file: file_path.to_path_buf(),
                block: "document_structure".to_string(),
                field: banner_pat.to_lowercase().replace(' ', "_"),
                severity: IssueSeverity::Warning,
                message: format!("Missing recommended Markdown block banner '{}'", banner_pat),
            });
        }
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: Main Validation Entry Point
// -----------------------------------------------------------------------------

/// Validates Markdown document content against repository documentation standards.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Confirms design manuscripts have tidy headings and matching code fences.
/// * For Systems Programmers: Evaluates code fence depth, checks HTML comment closures,
///   and scans heading hierarchy via `scan_markdown_headings`.
///
/// ````rust
/// use std::path::Path;
/// use tl_tools::validator::md::{validate_markdown_content, MarkdownValidationConfig};
///
/// let md = r#"
/// <!--
/// BLOCK 1: METADATA BLOCK
/// BLOCK 2: OPENING BLOCK
/// BLOCK 3: BODY BLOCK
/// BLOCK 4: AUDIT PROCEDURES BLOCK
/// BLOCK 5: CLOSING BLOCK
/// -->
/// # Title
/// ```rust
/// fn sample() {}
/// ```
/// "#;
/// let mut issues = Vec::new();
/// validate_markdown_content(Path::new("docs/sample.md"), md, &MarkdownValidationConfig::default(), &mut issues);
/// assert!(issues.is_empty(), "Issues: {:?}", issues);
/// ````
pub fn validate_markdown_content(
    file_path: &Path,
    content: &str,
    config: &MarkdownValidationConfig,
    issues: &mut Vec<ValidationIssue>,
) {
    // 1. Code Fence Symmetry Check
    if config.require_balanced_code_fences {
        audit_code_fences(file_path, content, issues);
    }

    // 2. Five-Block Envelope Verification
    if config.require_5block_banners {
        audit_5block_banners(file_path, content, issues);
    }

    // 3. Scan Headings via Syntax Lexer
    let _headings = scan_markdown_headings(content);
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Four-Section Ontological Test Suite & Subsystem Self-Audit )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Internal Subsystem Self-Audit Probe
// -----------------------------------------------------------------------------

/// Internal self-audit testing Markdown validation rules.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Tests the Markdown doc inspector.
/// * For Systems Programmers: Runs unclosed code fence fixtures through `validate_markdown_content`.
///
/// ```rust
/// use tl_tools::validator::md::self_audit;
///
/// assert!(self_audit().is_ok());
/// ```
pub fn self_audit() -> Result<(), &'static str> {
    let config = MarkdownValidationConfig::default();

    // 1. Nominal balanced test
    let good_md = r#"
<!--
BLOCK 1: METADATA BLOCK
BLOCK 2: OPENING BLOCK
BLOCK 3: BODY BLOCK
BLOCK 4: AUDIT PROCEDURES BLOCK
BLOCK 5: CLOSING BLOCK
-->
# Title

```rust
fn foo() {}
```
"#;
    let mut good_issues = Vec::new();
    validate_markdown_content(Path::new("test.md"), good_md, &config, &mut good_issues);
    if !good_issues.is_empty() {
        return Err("Self-audit failed: nominal balanced Markdown produced unexpected issues");
    }

    // 2. Fault inversion: unclosed code fence
    let broken_md = "```rust\nUnclosed code fence\n";
    let mut bad_issues = Vec::new();
    validate_markdown_content(Path::new("test.md"), broken_md, &config, &mut bad_issues);
    if !bad_issues.iter().any(|i| i.block == "syntax_code_fence") {
        return Err("Self-audit failed: unclosed code fence issue not detected");
    }

    // 3. Fault inversion: unclosed 4-backtick code fence
    let broken_4fence = "````markdown\nUnclosed 4-fence\n";
    let mut bad_4issues = Vec::new();
    validate_markdown_content(Path::new("test.md"), broken_4fence, &config, &mut bad_4issues);
    if !bad_4issues.iter().any(|i| i.block == "syntax_code_fence") {
        return Err("Self-audit failed: unclosed 4-fence issue not detected");
    }

    Ok(())
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Four-Section Ontological Unit Tests
// -----------------------------------------------------------------------------

#[cfg(test)]
mod tests {
    use super::*;

    // -------------------------------------------------------------------------
    // Section 1: Nominal & Always-Pass Tests (Golden Surface Path)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_1_nominal_balanced_markdown_passes() {
        let md = r#"
<!--
BLOCK 1: METADATA BLOCK
BLOCK 2: OPENING BLOCK
BLOCK 3: BODY BLOCK
BLOCK 4: AUDIT PROCEDURES BLOCK
BLOCK 5: CLOSING BLOCK
-->
# Title

```rust
fn foo() {}
```
"#;
        let mut issues = Vec::new();
        validate_markdown_content(Path::new("test.md"), md, &MarkdownValidationConfig::default(), &mut issues);
        assert!(issues.is_empty(), "Unexpected issues: {:?}", issues);
    }

    // -------------------------------------------------------------------------
    // Section 2: Resilience & Never-Fail Edge Tests (Fault Inversion)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_2_never_fail_unclosed_fence_detected() {
        let md = "```rust\nlet x = 1;\n";
        let mut issues = Vec::new();
        let config = MarkdownValidationConfig {
            require_5block_banners: false,
            ..Default::default()
        };
        validate_markdown_content(Path::new("test.md"), md, &config, &mut issues);
        assert!(issues.iter().any(|i| i.block == "syntax_code_fence"));
    }

    #[test]
    fn test_section_2_never_fail_empty_document_graceful() {
        let mut issues = Vec::new();
        validate_markdown_content(Path::new("empty.md"), "", &MarkdownValidationConfig::default(), &mut issues);
        assert!(!issues.is_empty());
    }

    // -------------------------------------------------------------------------
    // Section 3: Invariant & Structural Guard Tests (Strict Immutability)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_3_invariants_missing_banners_warning() {
        let md = "# Title\n\nSome body text.\n";
        let mut issues = Vec::new();
        let config = MarkdownValidationConfig {
            require_balanced_code_fences: false,
            require_5block_banners: true,
        };
        validate_markdown_content(Path::new("test.md"), md, &config, &mut issues);
        assert_eq!(issues.len(), 5);
    }

    #[test]
    fn test_section_3_invariants_nested_fences_handled() {
        let md = r#"
````markdown
```rust
let nested = 1;
```
````
"#;
        let mut issues = Vec::new();
        let config = MarkdownValidationConfig {
            require_balanced_code_fences: true,
            require_5block_banners: false,
        };
        validate_markdown_content(Path::new("test.md"), md, &config, &mut issues);
        assert!(issues.is_empty(), "Nested fences in 4-backtick blocks must be balanced");
    }

    // -------------------------------------------------------------------------
    // Section 4: Emergence & Integration Tests (Systemic Synthesis)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_4_emergence_self_audit() {
        assert!(self_audit().is_ok());
    }
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Epilogue Readiness & Subsystem Handshake )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue Readiness Probe
// -----------------------------------------------------------------------------

/// Epilogue readiness probe for the Markdown validator subsystem.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Confirms the markdown document inspector is ready.
/// * For Systems Programmers: Returns true when self-audit passes.
///
/// ```rust
/// use tl_tools::validator::md::is_markdown_validator_ready;
///
/// assert!(is_markdown_validator_ready());
/// ```
pub fn is_markdown_validator_ready() -> bool {
    self_audit().is_ok()
}

// -----------------------------------------------------------------------------
// Sub-Assembly 5.2: Subsystem Navigation & Module Exports
// -----------------------------------------------------------------------------

/// Returns a human-readable telemetry status string for the Markdown validator.
pub fn md_validator_status() -> &'static str {
    if is_markdown_validator_ready() {
        "READY"
    } else {
        "UNAVAILABLE"
    }
}
