// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/syntax/framework/mod.rs
// @id            cog.tools.syntax.framework.root
// @layer         Layer 3: Developer Toolchain Syntax Framework Sub-Assemblies
// @archetype     facade_umbrella | ast_orchestrator
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal Syntax Framework // Root Orchestrator
// @role          Universal 5-Block Document AST & Boundary Parser
// @maturity      Rev 5 Baseline | OmniDoc Universal Document Model
// @chassis       bounded-heap | Toolchain memory chassis; bounded block vectors
// @scale         O(L) across source document lines
// @seams         in: source text | out: OmniDoc AST | uses: block_1..5
// @invariants    1. Universal 5-Block envelope strictly segmented (Blocks 1..5).
//                2. Zero loss of non-block header or trailer content.
//                3. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Acts as the central orchestrator and public facade for the OmniCode
//   Universal 5-Block Lifecycle Framework. Splits arbitrary source text into
//   canonical 5-block envelopes and delegates to modular sub-assembly parsers,
//   yielding the language-agnostic `OmniDoc` Abstract Syntax Tree.
//
//   ### For Game Designers & Non-Programmers:
//   Provides a structured mental model where every document and code file in
//   the realm follows the identical 5-part anatomical blueprint.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Parser is stateless, reentrant, and thread-safe.
//   - AST Unification: Produces the common denominator for linting & transpiling.
//   - Transpilation Alignment: Maps directly to universal OmniCode schema.
// =============================================================================

//! # OmniCode Universal 5-Block Framework
//!
//! Provides the language-agnostic `OmniDoc` Abstract Syntax Tree and modular
//! parsers for Blocks 1 through 5.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

pub mod block_1;
pub mod block_2;
pub mod block_3;
pub mod block_4;
pub mod block_5;

pub use block_1::{
    clean_header_line, is_valid_tag_column, parse_block_1_lines, Block1Metadata,
    Block1Tag, CANONICAL_BLOCK_1_TAGS, STANDARD_TAG_VALUE_COLUMN,
};
pub use block_2::{
    extract_subassembly_markers, is_valid_subassembly_number, parse_block_2_lines,
    parse_include_directive, Block2Opening, IncludeDirective, SubAssemblyMarker,
    CANONICAL_BLOCK_2_SUBASSEMBLIES,
};
pub use block_3::{
    extract_function_name, is_function_doc_start, parse_block_3_lines,
    parse_function_docs, Block3Body, FunctionDoc, MIN_BLOCK_3_SUBASSEMBLIES,
};
pub use block_4::{
    find_self_audit, find_test_sections, is_valid_test_section_number,
    parse_block_4_lines, Block4Audit, TestSectionMarker, CANONICAL_TEST_SECTIONS,
};
pub use block_5::{
    find_header_guard_close, find_readiness_predicate, is_readiness_predicate_name,
    parse_block_5_lines, Block5Closing, MIN_BLOCK_5_SUBASSEMBLIES,
};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Defaults
// -----------------------------------------------------------------------------

/// Standard banner tokens signaling 5-block transitions.
pub const BLOCK_BANNER_TOKENS: &[&str] = &[
    "BLOCK 1: METADATA",
    "BLOCK 2: OPENING",
    "BLOCK 3: BODY",
    "BLOCK 4: AUDIT",
    "BLOCK 5: CLOSING",
];

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types, Enums & OmniDoc AST
// -----------------------------------------------------------------------------

/// Universal 5-Block Document AST representing any compliant source file.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::framework::OmniDoc;
///
/// let doc = OmniDoc::default();
/// assert_eq!(doc.line_count, 0);
/// assert!(!doc.has_all_five_blocks());
/// ```
#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct OmniDoc {
    /// Total lines in raw source text.
    pub line_count: usize,
    /// Parsed Block 1 Metadata.
    pub block_1: Block1Metadata,
    /// Parsed Block 2 Opening.
    pub block_2: Block2Opening,
    /// Parsed Block 3 Body.
    pub block_3: Block3Body,
    /// Parsed Block 4 Audit Procedures.
    pub block_4: Block4Audit,
    /// Parsed Block 5 Closing Procedures.
    pub block_5: Block5Closing,
    /// True if all 5 block sections were identified in the source.
    pub has_five_blocks: bool,
}

impl OmniDoc {
    /// Parses arbitrary raw source text into an `OmniDoc` AST.
    pub fn parse(content: &str) -> Result<Self, String> {
        parse_omni_doc(content)
    }

    /// Evaluates whether all 5 canonical blocks were discovered.
    pub fn has_all_five_blocks(&self) -> bool {
        self.has_five_blocks
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Verification Helpers & Invariant Predicates
// -----------------------------------------------------------------------------

/// Identifies whether a trimmed line contains a standard Block N banner.
///
/// Returns 1-based block index (1..=5) if matched.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::framework::detect_block_banner;
///
/// assert_eq!(detect_block_banner("// BLOCK 1: METADATA BLOCK"), Some(1));
/// assert_eq!(detect_block_banner("/* BLOCK 3: BODY BLOCK */"), Some(3));
/// assert_eq!(detect_block_banner("regular text"), None);
/// ```
pub fn detect_block_banner(trimmed: &str) -> Option<usize> {
    let upper = trimmed.to_ascii_uppercase();
    if upper.contains("BLOCK 1:") || upper.contains("BLOCK 1_METADATA") {
        Some(1)
    } else if upper.contains("BLOCK 2:") || upper.contains("BLOCK 2_OPENING") {
        Some(2)
    } else if upper.contains("BLOCK 3:") || upper.contains("BLOCK 3_BODY") {
        Some(3)
    } else if upper.contains("BLOCK 4:") || upper.contains("BLOCK 4_AUDIT") {
        Some(4)
    } else if upper.contains("BLOCK 5:") || upper.contains("BLOCK 5_CLOSING") {
        Some(5)
    } else {
        None
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( OmniDoc Document Parser & Block Segmentation Engine )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Document Segmentation Engine
// -----------------------------------------------------------------------------

/// Splits raw source text into 5 line partitions based on block banner delimiters.
pub fn segment_source_lines(content: &str) -> [Vec<String>; 5] {
    let mut partitions: [Vec<String>; 5] = [
        Vec::new(),
        Vec::new(),
        Vec::new(),
        Vec::new(),
        Vec::new(),
    ];

    let mut current_block: Option<usize> = None;

    for line in content.lines() {
        let trimmed = line.trim();

        if let Some(block_idx) = detect_block_banner(trimmed) {
            current_block = Some(block_idx);
        }

        if let Some(blk) = current_block {
            if blk >= 1 && blk <= 5 {
                partitions[blk - 1].push(line.to_string());
            }
        }
    }

    partitions
}

/// Parses raw document text into the universal `OmniDoc` AST.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::framework::parse_omni_doc;
///
/// let raw = "\
/// /*
/// BLOCK 1: METADATA BLOCK
/// @project Tenebris and Lux
/// */
/// // BLOCK 2: OPENING BLOCK
/// // Sub-Assembly 2.1: Constants
/// // Sub-Assembly 2.2: Types
/// // Sub-Assembly 2.3: Predicates
/// // BLOCK 3: BODY BLOCK
/// // Sub-Assembly 3.1: Ops
/// // BLOCK 4: AUDIT PROCEDURES BLOCK
/// // Sub-Assembly 4.1: Tests
/// // BLOCK 5: CLOSING BLOCK
/// // Sub-Assembly 5.1: Epilogue
/// ";
/// let doc = parse_omni_doc(raw).expect("Must parse OmniDoc");
/// assert_eq!(doc.block_1.tag_value("project"), Some("Tenebris and Lux"));
/// assert!(doc.has_all_five_blocks());
/// ```
pub fn parse_omni_doc(content: &str) -> Result<OmniDoc, String> {
    let lines_count = content.lines().count();
    let [b1_lines, b2_lines, b3_lines, b4_lines, b5_lines] = segment_source_lines(content);

    let has_b1 = !b1_lines.is_empty();
    let has_b2 = !b2_lines.is_empty();
    let has_b3 = !b3_lines.is_empty();
    let has_b4 = !b4_lines.is_empty();
    let has_b5 = !b5_lines.is_empty();

    let block_1 = parse_block_1_lines(&b1_lines)?;
    let block_2 = parse_block_2_lines(&b2_lines)?;
    let block_3 = parse_block_3_lines(&b3_lines)?;
    let block_4 = parse_block_4_lines(&b4_lines)?;
    let block_5 = parse_block_5_lines(&b5_lines)?;

    Ok(OmniDoc {
        line_count: lines_count,
        block_1,
        block_2,
        block_3,
        block_4,
        block_5,
        has_five_blocks: has_b1 && has_b2 && has_b3 && has_b4 && has_b5,
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
    fn test_section_1_nominal_omni_doc_parsing() {
        let text = "\
/*
BLOCK 1: METADATA BLOCK
@project       Tenebris and Lux
@id            test.doc
### For Game Designers & Non-Programmers:
Design summary.
### For Systems Programmers & Engine Architects:
Technical summary.
*/

// BLOCK 2: OPENING BLOCK
// Sub-Assembly 2.1: Constants
// Sub-Assembly 2.2: Types
// Sub-Assembly 2.3: Predicates

// BLOCK 3: BODY BLOCK
// Sub-Assembly 3.1: Core Functions
/**
 * For Game Designers: Runs action.
 * For Systems Programmers: Reentrant.
 */
void run_action(void);

// BLOCK 4: AUDIT PROCEDURES BLOCK
// Sub-Assembly 4.1: Unit Tests
// SECTION 1: ALWAYS PASS
// SECTION 2: NEVER FAIL
// SECTION 3: INVARIANTS
// SECTION 4: EMERGENCE
// Sub-Assembly 4.2: Preflight
bool self_audit(void) { return true; }

// BLOCK 5: CLOSING BLOCK
// Sub-Assembly 5.1: Epilogue
// Sub-Assembly 5.2: Readiness
bool is_test_doc_ready(void);
";

        let doc = parse_omni_doc(text).expect("Must parse nominal OmniDoc");
        assert!(doc.has_all_five_blocks());
        assert_eq!(doc.block_1.tag_value("project"), Some("Tenebris and Lux"));
        assert!(doc.block_1.has_dual_audience);
        assert!(doc.block_2.has_baseline_subassemblies);
        assert_eq!(doc.block_3.functions.len(), 1);
        assert!(doc.block_4.has_all_four_sections);
        assert!(doc.block_4.has_self_audit);
        assert!(doc.block_5.has_readiness_predicate);
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_empty_document() {
        let doc = parse_omni_doc("").expect("Empty document must return default AST");
        assert_eq!(doc.line_count, 0);
        assert!(!doc.has_five_blocks);
    }

    #[test]
    fn test_section_2_never_fail_document_without_blocks() {
        let doc = parse_omni_doc("hello world\njust arbitrary lines\n")
            .expect("Unstructured doc must return clean AST");
        assert_eq!(doc.line_count, 2);
        assert!(!doc.has_five_blocks);
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_banner_detection() {
        assert_eq!(detect_block_banner("// BLOCK 1: METADATA BLOCK"), Some(1));
        assert_eq!(detect_block_banner("/* BLOCK 2: OPENING BLOCK */"), Some(2));
        assert_eq!(detect_block_banner("## BLOCK 3: BODY BLOCK"), Some(3));
        assert_eq!(detect_block_banner("//// BLOCK 4: AUDIT PROCEDURES ////"), Some(4));
        assert_eq!(detect_block_banner("# BLOCK 5: CLOSING BLOCK"), Some(5));
        assert_eq!(detect_block_banner("arbitrary content"), None);
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_subassembly_readiness() {
        assert!(block_1::is_block_1_framework_ready());
        assert!(block_2::is_block_2_framework_ready());
        assert!(block_3::is_block_3_framework_ready());
        assert!(block_4::is_block_4_framework_ready());
        assert!(block_5::is_block_5_framework_ready());
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying OmniCode framework parser internal consistency.
pub fn self_audit() -> Result<(), String> {
    block_1::self_audit()?;
    block_2::self_audit()?;
    block_3::self_audit()?;
    block_4::self_audit()?;
    block_5::self_audit()?;
    if BLOCK_BANNER_TOKENS.len() != 5 {
        return Err("BLOCK_BANNER_TOKENS must define 5 block tokens".to_string());
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

/// Epilogue predicate affirming that OmniCode Universal Framework parser is operational.
pub fn is_framework_ready() -> bool {
    self_audit().is_ok()
}
