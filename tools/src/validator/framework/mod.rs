// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/validator/framework/mod.rs
// @id            cog.tools.validator.framework.root
// @layer         Layer 3: Systems Developer Tooling Core Library
// @archetype     facade_umbrella | stateless
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal 5-Block Validator Suite
// @role          Language-Agnostic 5-Block AST Validator Facade
// @maturity      Rev 5 Baseline | Universal 5-Block Document Validator
// @chassis       bounded-heap | Developer tooling memory chassis; bounded vectors
// @scale         O(N) single-pass validation across all 5 blocks of OmniDoc AST
// @seams         in: syntax::framework::OmniDoc | uses: validator::report | probes: self_audit -> exit
// @invariants    1. Zero panics on arbitrary OmniDoc AST instances.
//                2. Runs all 5-block rule engines in linear sequential order.
//                3. Emits standardized ValidationIssue records for all structural defects.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Language-agnostic facade orchestrating validation across the entire 5-block
//   OmniCode AST (`OmniDoc`). Coordinates Block 1 (Metadata), Block 2 (Opening),
//   Block 3 (Body), Block 4 (Audit), and Block 5 (Closing) rule engines.
//
//   ### For Game Designers & Non-Programmers:
//   This is the master quality assurance checkpoint. It reads a file through
//   the 5-block lens and makes sure every section—from its title and ingredients
//   to its actions, tests, and final wrap-up—follows the project's quality rules.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Unified composite validator over `syntax::framework::OmniDoc`.
//   - Extension Seam: Configurable via `OmniDocValidationConfig`, delegates to specialized
//     sub-assembly checkers (`block_1` through `block_5`).
//   - Explicit Non-Goal: Does not parse source files; validates ASTs only.
// =============================================================================

//! # OmniCode Universal 5-Block AST Validator Framework
//!
//! Orchestrates language-agnostic validation across the 5-block `OmniDoc` AST.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

pub mod block_1;
pub mod block_2;
pub mod block_3;
pub mod block_4;
pub mod block_5;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Re-exported Rule Functions & Configurations
// -----------------------------------------------------------------------------

pub use block_1::{validate_block_1, Block1RuleConfig};
pub use block_2::{validate_block_2, Block2RuleConfig};
pub use block_3::{validate_block_3, validate_prohibited_symbols, Block3RuleConfig};
pub use block_4::{validate_block_4, Block4RuleConfig};
pub use block_5::{validate_block_5, Block5RuleConfig};

use std::path::Path;
use crate::syntax::framework::OmniDoc;
use crate::validator::report::{IssueSeverity, ValidationIssue};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Composite Validation Configuration
// -----------------------------------------------------------------------------

/// Composite validation configuration governing all 5 blocks of an OmniDoc.
///
/// # Examples
///
/// ```rust
/// use tl_tools::validator::framework::OmniDocValidationConfig;
///
/// let config = OmniDocValidationConfig::default();
/// assert!(config.require_all_5_blocks);
/// ```
#[derive(Debug, Clone)]
pub struct OmniDocValidationConfig {
    /// Whether all 5 blocks are strictly required to be present.
    pub require_all_5_blocks: bool,
    /// Configuration for Block 1 (Metadata).
    pub b1: Block1RuleConfig,
    /// Configuration for Block 2 (Opening).
    pub b2: Block2RuleConfig,
    /// Configuration for Block 3 (Body).
    pub b3: Block3RuleConfig,
    /// Configuration for Block 4 (Audit).
    pub b4: Block4RuleConfig,
    /// Configuration for Block 5 (Closing).
    pub b5: Block5RuleConfig,
}

impl Default for OmniDocValidationConfig {
    fn default() -> Self {
        Self {
            require_all_5_blocks: true,
            b1: Block1RuleConfig::default(),
            b2: Block2RuleConfig::default(),
            b3: Block3RuleConfig::default(),
            b4: Block4RuleConfig::default(),
            b5: Block5RuleConfig::default(),
        }
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Configuration Presets
// -----------------------------------------------------------------------------

impl OmniDocValidationConfig {
    /// Constructs a strict 5-block validation configuration.
    pub fn strict() -> Self {
        Self::default()
    }

    /// Constructs a permissive configuration with all block checks relaxed.
    pub fn permissive() -> Self {
        Self {
            require_all_5_blocks: false,
            b1: Block1RuleConfig {
                expected_column_alignment: 16,
                require_dual_audience: false,
                strict_column_alignment: false,
                required_tags: Vec::new(),
            },
            b2: Block2RuleConfig {
                require_sub_assemblies: false,
                enforce_ascending_sub_assemblies: false,
                forbidden_include_patterns: Vec::new(),
            },
            b3: Block3RuleConfig {
                require_dual_audience_docstrings: false,
                enforce_ascending_sub_assemblies: false,
                prohibited_symbols: Vec::new(),
            },
            b4: Block4RuleConfig {
                require_self_audit: false,
                require_all_four_test_sections: false,
                require_tests_present: false,
            },
            b5: Block5RuleConfig {
                require_readiness_predicate: false,
                require_guard_close: false,
                expected_guard_comment: None,
            },
        }
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Composite Validator Orchestrator & Dispatch Pipeline )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Composite Validator Dispatcher
// -----------------------------------------------------------------------------

/// Validates an entire parsed `OmniDoc` AST across all five blocks.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Reviews the entire document from top to bottom, checking
///   every block against project standards.
/// * For Systems Programmers: Dispatches to `validate_block_1` through `validate_block_5`,
///   recording any missing block banners or structural breaches.
///
/// # Examples
///
/// ```rust
/// use std::path::Path;
/// use tl_tools::syntax::framework::OmniDoc;
/// use tl_tools::validator::framework::{validate_omni_doc, OmniDocValidationConfig};
///
/// let doc = OmniDoc::default();
/// let mut issues = Vec::new();
/// validate_omni_doc(Path::new("test.c"), &doc, &OmniDocValidationConfig::strict(), &mut issues);
/// assert!(issues.iter().any(|i| i.field == "has_five_blocks"));
/// ```
pub fn validate_omni_doc(
    file_path: &Path,
    doc: &OmniDoc,
    config: &OmniDocValidationConfig,
    issues: &mut Vec<ValidationIssue>,
) {
    // 0. Whole-document 5-block banner discovery check
    if config.require_all_5_blocks && !doc.has_five_blocks {
        issues.push(ValidationIssue {
            file: file_path.to_path_buf(),
            block: "document_structure".to_string(),
            field: "has_five_blocks".to_string(),
            severity: IssueSeverity::Error,
            message: "Document is missing one or more of the 5 canonical block banners".to_string(),
        });
    }

    // 1. Block 1 Validation
    if config.require_all_5_blocks || !doc.block_1.raw_lines.is_empty() {
        validate_block_1(file_path, &doc.block_1, &config.b1, issues);
    }

    // 2. Block 2 Validation
    if config.require_all_5_blocks || !doc.block_2.sub_assemblies.is_empty() {
        validate_block_2(file_path, &doc.block_2, &config.b2, issues);
    }

    // 3. Block 3 Validation
    if config.require_all_5_blocks || !doc.block_3.functions.is_empty() {
        validate_block_3(file_path, &doc.block_3, &config.b3, issues);
    }

    // 4. Block 4 Validation
    if config.require_all_5_blocks || doc.block_4.has_self_audit || !doc.block_4.test_sections.is_empty() {
        validate_block_4(file_path, &doc.block_4, &config.b4, issues);
    }

    // 5. Block 5 Validation
    if config.require_all_5_blocks || doc.block_5.has_readiness_predicate {
        validate_block_5(file_path, &doc.block_5, &config.b5, issues);
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
    use std::path::Path;

    // -------------------------------------------------------------------------
    // SECTION 1: ALWAYS PASS (Nominal Golden Surface Cogs)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_1_nominal_empty_non_strict_passes() {
        let doc = OmniDoc::default();
        let mut issues = Vec::new();
        let config = OmniDocValidationConfig::permissive();
        validate_omni_doc(Path::new("test.c"), &doc, &config, &mut issues);
        assert!(issues.is_empty(), "Unexpected issues: {:?}", issues);
    }

    #[test]
    fn test_section_1_nominal_strict_config_construction() {
        let config = OmniDocValidationConfig::strict();
        assert!(config.require_all_5_blocks);
        assert!(config.b1.require_dual_audience);
        assert!(config.b4.require_self_audit);
        assert!(config.b5.require_readiness_predicate);
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_missing_blocks_in_strict_mode() {
        let doc = OmniDoc::default();
        let mut issues = Vec::new();
        validate_omni_doc(Path::new("test.c"), &doc, &OmniDocValidationConfig::strict(), &mut issues);
        assert!(!issues.is_empty());
        assert!(issues.iter().any(|i| i.field == "has_five_blocks"));
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_subsystems_readiness() {
        assert!(block_1::is_validator_block_1_ready());
        assert!(block_2::is_validator_block_2_ready());
        assert!(block_3::is_validator_block_3_ready());
        assert!(block_4::is_validator_block_4_ready());
        assert!(block_5::is_validator_block_5_ready());
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

/// Internal self-audit confirming framework validator wiring and sub-assembly readiness.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Tests all 5 block inspectors in one combined check.
/// * For Systems Programmers: Runs self_audit across blocks 1-5 and asserts composite dispatch.
///
/// # Examples
///
/// ```rust
/// use tl_tools::validator::framework::self_audit;
///
/// assert!(self_audit().is_ok());
/// ```
pub fn self_audit() -> Result<(), &'static str> {
    block_1::self_audit()?;
    block_2::self_audit()?;
    block_3::self_audit()?;
    block_4::self_audit()?;
    block_5::self_audit()?;
    Ok(())
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( State Summary Handshake, Module Epilogue & Teardown Verification )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue Predicate
// -----------------------------------------------------------------------------

/// Epilogue readiness probe for the entire validator framework.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Confirms that all 5 inspection gears are linked and ready.
/// * For Systems Programmers: Returns true when composite self-audit passes.
///
/// # Examples
///
/// ```rust
/// use tl_tools::validator::framework::is_validator_framework_ready;
///
/// assert!(is_validator_framework_ready());
/// ```
pub fn is_validator_framework_ready() -> bool {
    self_audit().is_ok()
}
