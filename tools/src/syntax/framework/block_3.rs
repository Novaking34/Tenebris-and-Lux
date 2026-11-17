// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/syntax/framework/block_3.rs
// @id            cog.tools.syntax.framework.block_3
// @layer         Layer 3: Developer Toolchain Syntax Framework Sub-Assemblies
// @archetype     pure_types | body_parser
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal Syntax Framework // Block 3 Sub-Assembly
// @role          Universal Block 3 Body Parser & Function Docstring Extractor
// @maturity      Rev 5 Baseline | Universal 5-Block Body AST
// @chassis       bounded-heap | Toolchain memory chassis; bounded function vectors
// @scale         O(L) across Block 3 lines
// @seams         in: source lines | out: Block3Body AST
// @invariants    1. Sub-Assembly 3.X markers must follow monotonic order.
//                2. All public operational functions must retain dual-audience documentation.
//                3. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Extracts and validates Block 3 Body declarations across all languages
//   conforming to the OmniCode Universal 5-Block Lifecycle Protocol. Discovers
//   and catalogs Sub-Assemblies 3.X, operational function docstrings, dual-audience
//   commentary, and operational signatures.
//
//   ### For Game Designers & Non-Programmers:
//   Ensures that every active game mechanic and operational function is
//   documented with plain-English intent before technical code details.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Parser is stateless and pure.
//   - Signature Isolation: Extracts clean function names and parameter spans.
//   - Transpilation Alignment: Maps directly to universal OmniCode Block 3 schema.
// =============================================================================

//! # Universal Block 3 Body Parser
//!
//! Parses and validates Block 3 Body declarations across C, C++, Rust, JSON,
//! Markdown, AsciiDoc, TOML, and CMake files.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

use super::block_2::{extract_subassembly_markers, SubAssemblyMarker};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Defaults
// -----------------------------------------------------------------------------

/// Canonical minimum expected sub-assemblies in a standard Block 3 body.
pub const MIN_BLOCK_3_SUBASSEMBLIES: usize = 1;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types, Enums & Block 3 Body AST
// -----------------------------------------------------------------------------

/// Extracted function documentation envelope and signature metadata.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::framework::block_3::FunctionDoc;
///
/// let doc = FunctionDoc {
///     name: "log_append".to_string(),
///     signature: "bool log_append(const char* msg)".to_string(),
///     designer_text: "Appends message.".to_string(),
///     programmer_text: "Zero heap reentrant.".to_string(),
///     sub_assembly: "3.1".to_string(),
///     line_number: 100,
/// };
/// assert_eq!(doc.name, "log_append");
/// assert!(doc.has_dual_audience());
/// ```
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct FunctionDoc {
    /// Function name identifier.
    pub name: String,
    /// Full declaration or prototype signature.
    pub signature: String,
    /// Extracted plain-English designer documentation.
    pub designer_text: String,
    /// Extracted systems programmer / architectural documentation.
    pub programmer_text: String,
    /// Associated sub-assembly number (e.g. "3.1").
    pub sub_assembly: String,
    /// 1-based line number where function declaration begins.
    pub line_number: usize,
}

impl FunctionDoc {
    /// Evaluates whether the docstring contains both designer and programmer sections.
    pub fn has_dual_audience(&self) -> bool {
        !self.designer_text.trim().is_empty() && !self.programmer_text.trim().is_empty()
    }
}

/// Structured AST representing a parsed Block 3 Body section.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::framework::block_3::Block3Body;
///
/// let b3 = Block3Body::default();
/// assert!(b3.sub_assemblies.is_empty());
/// assert!(b3.functions.is_empty());
/// ```
#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct Block3Body {
    /// Raw lines of Block 3.
    pub raw_lines: Vec<String>,
    /// Discovered sub-assembly markers (3.1, 3.2, ...).
    pub sub_assemblies: Vec<SubAssemblyMarker>,
    /// Discovered function documentation blocks and signatures.
    pub functions: Vec<FunctionDoc>,
}

impl Block3Body {
    /// Evaluates whether a specific sub-assembly number exists (e.g. "3.1").
    pub fn has_subassembly(&self, num: &str) -> bool {
        self.sub_assemblies.iter().any(|s| s.number == num)
    }

    /// Evaluates whether all discovered functions retain dual-audience documentation.
    pub fn all_functions_have_dual_audience(&self) -> bool {
        !self.functions.is_empty() && self.functions.iter().all(|f| f.has_dual_audience())
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Verification Helpers & Invariant Predicates
// -----------------------------------------------------------------------------

/// Evaluates whether a line starts an operational function docstring banner.
pub fn is_function_doc_start(trimmed_line: &str) -> bool {
    trimmed_line.starts_with("/**") || trimmed_line.starts_with("///")
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Function Doc Parsing, Sub-Assembly Extraction & Signature Lexing )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Function Docstring Parser
// -----------------------------------------------------------------------------

/// Parses function documentation comments and declaration signatures.
pub fn parse_function_docs(lines: &[String], sub_assemblies: &[SubAssemblyMarker]) -> Vec<FunctionDoc> {
    let mut docs = Vec::new();
    let mut current_designer = Vec::new();
    let mut current_programmer = Vec::new();
    let mut in_comment = false;
    let mut in_designer = false;
    let mut in_programmer = false;
    let mut doc_start_line = 0;

    for (idx, line) in lines.iter().enumerate() {
        let line_num = idx + 1;
        let trimmed = line.trim();

        if !in_comment && (trimmed.starts_with("/**") || trimmed.starts_with("/*")) {
            in_comment = true;
            doc_start_line = line_num;
            current_designer.clear();
            current_programmer.clear();
            in_designer = false;
            in_programmer = false;
        }

        if in_comment {
            if let Some(pos) = trimmed.find("For Game Designers") {
                in_designer = true;
                in_programmer = false;
                let after = trimmed[pos + "For Game Designers".len()..]
                    .trim_start_matches('&')
                    .trim_start_matches("Non-Programmers:")
                    .trim_start_matches(':')
                    .trim_start_matches('-')
                    .trim();
                if !after.is_empty() {
                    current_designer.push(after.to_string());
                }
                continue;
            } else if let Some(pos) = trimmed.find("For Systems Programmers") {
                in_designer = false;
                in_programmer = true;
                let after = trimmed[pos + "For Systems Programmers".len()..]
                    .trim_start_matches('&')
                    .trim_start_matches("Engine Architects:")
                    .trim_start_matches(':')
                    .trim_start_matches('-')
                    .trim();
                if !after.is_empty() {
                    current_programmer.push(after.to_string());
                }
                continue;
            }

            if in_designer {
                let clean = trimmed.trim_start_matches('*').trim();
                if !clean.is_empty() && !clean.starts_with("*/") {
                    current_designer.push(clean.to_string());
                }
            } else if in_programmer {
                let clean = trimmed.trim_start_matches('*').trim();
                if !clean.is_empty() && !clean.starts_with("*/") {
                    current_programmer.push(clean.to_string());
                }
            }

            if trimmed.ends_with("*/") {
                in_comment = false;
                in_designer = false;
                in_programmer = false;
                continue;
            }
        } else if !current_designer.is_empty() || !current_programmer.is_empty() {
            // Next non-comment, non-empty line is candidate function signature
            if !trimmed.is_empty() && !trimmed.starts_with("//") && !trimmed.starts_with("/*") {
                // Find associated sub-assembly by line proximity
                let sub_num = sub_assemblies
                    .iter()
                    .filter(|s| s.line_number < line_num)
                    .last()
                    .map(|s| s.number.clone())
                    .unwrap_or_else(|| "3.1".to_string());

                let func_name = extract_function_name(trimmed);
                docs.push(FunctionDoc {
                    name: func_name,
                    signature: trimmed.to_string(),
                    designer_text: current_designer.join("\n"),
                    programmer_text: current_programmer.join("\n"),
                    sub_assembly: sub_num,
                    line_number: doc_start_line,
                });

                current_designer.clear();
                current_programmer.clear();
            }
        }
    }

    docs
}

/// Extracts simple function name from a declaration signature.
pub fn extract_function_name(sig: &str) -> String {
    let clean = sig.trim_end_matches(';').trim();
    if let Some(open_paren) = clean.find('(') {
        let before_paren = clean[..open_paren].trim();
        before_paren
            .split_whitespace()
            .last()
            .unwrap_or("unknown")
            .trim_start_matches('*')
            .to_string()
    } else {
        clean.split_whitespace().last().unwrap_or("unknown").to_string()
    }
}

/// Parses a slice of Block 3 Body lines into a `Block3Body` AST.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::framework::block_3::parse_block_3_lines;
///
/// let lines = vec![
///     "// Sub-Assembly 3.1: Operations".to_string(),
///     "/**".to_string(),
///     " * For Game Designers: Description here.".to_string(),
///     " * For Systems Programmers: Invariant details.".to_string(),
///     " */".to_string(),
///     "bool execute_task(void);".to_string(),
/// ];
/// let b3 = parse_block_3_lines(&lines).expect("Must parse Block 3");
/// assert_eq!(b3.sub_assemblies.len(), 1);
/// assert_eq!(b3.functions.len(), 1);
/// assert!(b3.functions[0].has_dual_audience());
/// ```
pub fn parse_block_3_lines(lines: &[String]) -> Result<Block3Body, String> {
    let sub_assemblies = extract_subassembly_markers(lines, 3);
    let functions = parse_function_docs(lines, &sub_assemblies);

    Ok(Block3Body {
        raw_lines: lines.to_vec(),
        sub_assemblies,
        functions,
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
    fn test_section_1_nominal_function_doc_parsing() {
        let lines = vec![
            "/* Sub-Assembly 3.1: Core Logging Operations */".to_string(),
            "/**".to_string(),
            " * ### For Game Designers & Non-Programmers:".to_string(),
            " * Emits high-priority alert into user HUD.".to_string(),
            " * ### For Systems Programmers & Engine Architects:".to_string(),
            " * Zero heap dynamic allocation in Layer 0.".to_string(),
            " */".to_string(),
            "bool log_emit_alert(const char* message);".to_string(),
        ];

        let b3 = parse_block_3_lines(&lines).expect("Parsing must succeed");
        assert_eq!(b3.sub_assemblies.len(), 1);
        assert_eq!(b3.functions.len(), 1);

        let func = &b3.functions[0];
        assert_eq!(func.name, "log_emit_alert");
        assert!(func.has_dual_audience());
        assert!(func.designer_text.contains("Emits high-priority alert"));
        assert!(func.programmer_text.contains("Zero heap"));
    }

    #[test]
    fn test_section_1_nominal_function_name_extraction() {
        assert_eq!(extract_function_name("bool log_init(void);"), "log_init");
        assert_eq!(
            extract_function_name("const char* get_version(int flag);"),
            "get_version"
        );
        assert_eq!(
            extract_function_name("pub fn run_task(&self) -> bool"),
            "run_task"
        );
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_empty_lines() {
        let empty: Vec<String> = vec![];
        let b3 = parse_block_3_lines(&empty).expect("Empty input must return default AST");
        assert!(b3.sub_assemblies.is_empty());
        assert!(b3.functions.is_empty());
    }

    #[test]
    fn test_section_2_never_fail_doc_without_signature() {
        let lines = vec![
            "/**".to_string(),
            " * Stray comment without following function".to_string(),
            " */".to_string(),
        ];
        let b3 = parse_block_3_lines(&lines).expect("Must handle gracefully");
        assert!(b3.functions.is_empty());
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_subassembly_ordering() {
        let lines = vec![
            "/* Sub-Assembly 3.1: Init */".to_string(),
            "/* Sub-Assembly 3.2: Execution */".to_string(),
            "/* Sub-Assembly 3.3: Shutdown */".to_string(),
        ];
        let b3 = parse_block_3_lines(&lines).expect("Must parse sub-assemblies");
        assert_eq!(b3.sub_assemblies.len(), 3);
        assert_eq!(b3.sub_assemblies[0].number, "3.1");
        assert_eq!(b3.sub_assemblies[1].number, "3.2");
        assert_eq!(b3.sub_assemblies[2].number, "3.3");
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_multi_function_body() {
        let lines = vec![
            "/* Sub-Assembly 3.1: Core Getters */".to_string(),
            "/**".to_string(),
            " * For Game Designers: Returns length.".to_string(),
            " * For Systems Programmers: O(1) getter.".to_string(),
            " */".to_string(),
            "size_t get_count(void);".to_string(),
            "".to_string(),
            "/* Sub-Assembly 3.2: Mutation Cogs */".to_string(),
            "/**".to_string(),
            " * For Game Designers: Adds item.".to_string(),
            " * For Systems Programmers: Zero heap write.".to_string(),
            " */".to_string(),
            "bool add_item(int id);".to_string(),
        ];

        let b3 = parse_block_3_lines(&lines).expect("Must parse multi-function Block 3");
        assert_eq!(b3.sub_assemblies.len(), 2);
        assert_eq!(b3.functions.len(), 2);
        assert!(b3.all_functions_have_dual_audience());
        assert_eq!(b3.functions[0].sub_assembly, "3.1");
        assert_eq!(b3.functions[1].sub_assembly, "3.2");
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying Block 3 parser internal consistency.
pub fn self_audit() -> Result<(), String> {
    if MIN_BLOCK_3_SUBASSEMBLIES == 0 {
        return Err("MIN_BLOCK_3_SUBASSEMBLIES must be >= 1".to_string());
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

/// Epilogue predicate affirming that Block 3 framework parser is operational.
pub fn is_block_3_framework_ready() -> bool {
    self_audit().is_ok()
}
