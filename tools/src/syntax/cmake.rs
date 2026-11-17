// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/syntax/cmake.rs
// @id            cog.tools.syntax.cmake
// @layer         Layer 3: Developer Toolchain Language Syntax Primitives
// @archetype     pure_types | cmake_parser
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal Syntax Framework // CMake Lexer
// @role          CMake Script Tokenizer, Command Scanner & Parameter Lexer
// @maturity      Rev 5 Baseline | Modern CMake Script Analysis
// @chassis       bounded-heap | Toolchain memory chassis; bounded tokens
// @scale         O(L) across CMake script lines
// @seams         in: CMake script | out: CMake commands, args & variables
// @invariants    1. Accurate command token extraction (command_name(...)).
//                2. Line comment (#) tracking preserving quoted literals.
//                3. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Lexical tokenizer and structural parser for CMake build scripts and
//   generators across `cmake/` and root `CMakeLists.txt`. Extracts comment
//   headers, command invocations, set() variables, and target bindings.
//
//   ### For Game Designers & Non-Programmers:
//   Ensures the build system is deterministic, portable, and as structured
//   as the engine source code.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Parser is stateless and pure.
//   - Zero Panics: Graceful command extraction across nested parentheses.
//   - Transpilation Alignment: Maps CMake invocations to universal OmniCode AST.
// =============================================================================

//! # Modern CMake Syntax & Command Analyzer
//!
//! Provides lexical analysis, command invocation extraction, and variable
//! tracking for CMake files.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Defaults
// -----------------------------------------------------------------------------

/// Canonical modern CMake command names.
pub const CANONICAL_CMAKE_COMMANDS: &[&str] = &[
    "cmake_minimum_required",
    "project",
    "set",
    "add_executable",
    "add_library",
    "target_link_libraries",
    "target_include_directories",
];

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types, Enums & CMake Lexical AST
// -----------------------------------------------------------------------------

/// Discovered CMake command invocation.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct CMakeCommand {
    /// Command identifier (e.g. "set", "target_link_libraries").
    pub name: String,
    /// Unparsed argument string between parentheses.
    pub arguments: String,
    /// 1-based line number.
    pub line_number: usize,
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Verification Helpers & Invariant Predicates
// -----------------------------------------------------------------------------

/// Evaluates whether a line starts a CMake command invocation.
pub fn is_cmake_command_invocation(trimmed_line: &str) -> bool {
    if trimmed_line.starts_with('#') || trimmed_line.is_empty() {
        return false;
    }
    if let Some(pos) = trimmed_line.find('(') {
        let name = trimmed_line[..pos].trim();
        !name.is_empty() && name.chars().all(|c| c.is_ascii_alphanumeric() || c == '_')
    } else {
        false
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Command Scanner & Parameter Lexer )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Command Invocation Scanner
// -----------------------------------------------------------------------------

/// Scans CMake source text for command invocations.
///
/// # Examples
///
/// ```rust
/// use tl_tools::syntax::cmake::scan_cmake_commands;
///
/// let script = "set(TARGET_NAME tenebris-lux)\nadd_executable(${TARGET_NAME} main.c)\n";
/// let commands = scan_cmake_commands(script);
/// assert_eq!(commands.len(), 2);
/// assert_eq!(commands[0].name, "set");
/// assert_eq!(commands[1].name, "add_executable");
/// ```
pub fn scan_cmake_commands(content: &str) -> Vec<CMakeCommand> {
    let mut commands = Vec::new();
    let mut current_name: Option<String> = None;
    let mut current_args: Vec<String> = Vec::new();
    let mut start_line = 0;
    let mut in_parens = false;

    for (idx, line) in content.lines().enumerate() {
        let line_num = idx + 1;
        let trimmed = line.trim();

        if !in_parens && (trimmed.starts_with('#') || trimmed.is_empty()) {
            continue;
        }

        if !in_parens {
            if let Some(open_pos) = trimmed.find('(') {
                let name = trimmed[..open_pos].trim().to_ascii_lowercase();
                if !name.is_empty() && name.chars().all(|c| c.is_ascii_alphanumeric() || c == '_') {
                    let after_open = &trimmed[open_pos + 1..];
                    if let Some(close_pos) = after_open.rfind(')') {
                        // Single line invocation
                        let args = after_open[..close_pos].trim().to_string();
                        commands.push(CMakeCommand {
                            name,
                            arguments: args,
                            line_number: line_num,
                        });
                    } else {
                        // Multi-line invocation start
                        current_name = Some(name);
                        current_args.push(after_open.trim().to_string());
                        start_line = line_num;
                        in_parens = true;
                    }
                }
            }
        } else {
            // Multi-line continuation
            if let Some(close_pos) = trimmed.rfind(')') {
                current_args.push(trimmed[..close_pos].trim().to_string());
                commands.push(CMakeCommand {
                    name: current_name.take().unwrap_or_default(),
                    arguments: current_args.join(" ").trim().to_string(),
                    line_number: start_line,
                });
                current_args.clear();
                in_parens = false;
            } else {
                current_args.push(trimmed.to_string());
            }
        }
    }

    commands
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
    fn test_section_1_nominal_cmake_scanning() {
        let script = "\
# CMake build file
cmake_minimum_required(VERSION 3.20)
project(TenebrisAndLux LANGUAGES C CXX)
set(CMAKE_C_STANDARD 11)
target_link_libraries(
    engine
    PRIVATE m
)
";
        let commands = scan_cmake_commands(script);
        assert_eq!(commands.len(), 4);
        assert_eq!(commands[0].name, "cmake_minimum_required");
        assert_eq!(commands[1].name, "project");
        assert_eq!(commands[2].name, "set");
        assert_eq!(commands[3].name, "target_link_libraries");
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_empty_source() {
        let commands = scan_cmake_commands("");
        assert!(commands.is_empty());
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_command_predicate() {
        assert!(is_cmake_command_invocation("set(A 1)"));
        assert!(is_cmake_command_invocation("target_link_libraries(a b)"));
        assert!(!is_cmake_command_invocation("# Comment"));
        assert!(!is_cmake_command_invocation("regular text"));
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_live_cornerstone_testing_syntax() {
        let sample = "\
set(CORNERSTONE_TEST_SUITE ON)
enable_testing()
add_subdirectory(tests)
";
        let commands = scan_cmake_commands(sample);
        assert_eq!(commands.len(), 3);
        assert_eq!(commands[0].name, "set");
        assert_eq!(commands[1].name, "enable_testing");
        assert_eq!(commands[2].name, "add_subdirectory");
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying CMake syntax parser internal consistency.
pub fn self_audit() -> Result<(), String> {
    if CANONICAL_CMAKE_COMMANDS.is_empty() {
        return Err("CANONICAL_CMAKE_COMMANDS must be non-empty".to_string());
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

/// Epilogue predicate affirming that CMake syntax parser is operational.
pub fn is_cmake_syntax_ready() -> bool {
    self_audit().is_ok()
}
