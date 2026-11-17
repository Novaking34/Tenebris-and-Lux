// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/syntax/mod.rs
// @id            cog.tools.syntax.root
// @layer         Layer 3: Developer Toolchain Language Syntax Primitives
// @archetype     facade_umbrella | syntax_orchestrator
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal Syntax Framework // Root Facade
// @role          Universal Language Syntax Registry & OmniDoc Dispatcher
// @maturity      Rev 5 Baseline | Cross-Language Syntax Primitives
// @chassis       bounded-heap | Toolchain memory chassis; bounded dispatch
// @scale         O(1) language resolution, O(L) document parsing
// @seams         in: raw files | out: OmniDoc AST | uses: framework, c, cpp, rust, json, md, adoc, toml, cmake
// @invariants    1. Universal 5-Block framework accessible across all 8 supported languages.
//                2. Zero unhandled language extensions in detection dispatcher.
//                3. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Central orchestrator and public facade for the OmniCode Universal Syntax
//   Framework. Unifies language-agnostic 5-block meta-architecture with 8
//   concrete language syntax parsers: C, C++, Rust, JSON, Markdown, AsciiDoc,
//   TOML, and CMake.
//
//   ### For Game Designers & Non-Programmers:
//   Provides a single unified standard where all game lore, data configs,
//   scripts, and engine code are parsed and audited under the same rules.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: All syntax parsers are stateless and reentrant.
//   - Language Parity: Implements the AST backbone for cross-language transpiling.
//   - Transpilation Alignment: Maps all repo files into the universal OmniDoc AST.
// =============================================================================

//! # OmniCode Universal Syntax Framework
//!
//! Unifies the language-agnostic 5-Block Lifecycle Protocol with concrete
//! lexical analyzers across C, C++, Rust, JSON, Markdown, AsciiDoc, TOML, and
//! CMake.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

pub mod adoc;
pub mod c;
pub mod cmake;
pub mod cpp;
pub mod framework;
pub mod json;
pub mod md;
pub mod rust;
pub mod toml;

use std::path::Path;

pub use framework::{
    detect_block_banner, parse_omni_doc, segment_source_lines, Block1Metadata,
    Block1Tag, Block2Opening, Block3Body, Block4Audit, Block5Closing,
    FunctionDoc, IncludeDirective, OmniDoc, SubAssemblyMarker,
    TestSectionMarker, BLOCK_BANNER_TOKENS, CANONICAL_BLOCK_1_TAGS,
    CANONICAL_BLOCK_2_SUBASSEMBLIES, CANONICAL_TEST_SECTIONS,
    STANDARD_TAG_VALUE_COLUMN,
};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Defaults & Language Identifiers
// -----------------------------------------------------------------------------

/// Canonical languages supported across the OmniCode syntax framework.
pub const SUPPORTED_LANGUAGES: &[&str] = &[
    "c",
    "cpp",
    "rust",
    "json",
    "markdown",
    "asciidoc",
    "toml",
    "cmake",
];

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types, Enums & Language Enumeration
// -----------------------------------------------------------------------------

/// Enumeration of distinct programming and markup languages within the repository.
///
/// # Examples
///
/// ```rust
/// use std::path::Path;
/// use tl_tools::syntax::{detect_language_from_path, Language};
///
/// assert_eq!(detect_language_from_path(Path::new("engine/src/main.c")), Language::C);
/// assert_eq!(detect_language_from_path(Path::new("tools/src/main.rs")), Language::Rust);
/// ```
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum Language {
    C,
    Cpp,
    Rust,
    Json,
    Markdown,
    AsciiDoc,
    Toml,
    CMake,
    Unknown,
}

impl Language {
    /// Returns the canonical lowercase string identifier of the language.
    pub fn as_str(&self) -> &'static str {
        match self {
            Self::C => "c",
            Self::Cpp => "cpp",
            Self::Rust => "rust",
            Self::Json => "json",
            Self::Markdown => "markdown",
            Self::AsciiDoc => "asciidoc",
            Self::Toml => "toml",
            Self::CMake => "cmake",
            Self::Unknown => "unknown",
        }
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Verification Helpers & Invariant Predicates
// -----------------------------------------------------------------------------

/// Resolves language identity from a filesystem path based on extension and filename.
///
/// # Examples
///
/// ```rust
/// use std::path::Path;
/// use tl_tools::syntax::{detect_language_from_path, Language};
///
/// assert_eq!(detect_language_from_path(Path::new("CMakeLists.txt")), Language::CMake);
/// assert_eq!(detect_language_from_path(Path::new("Cargo.toml")), Language::Toml);
/// assert_eq!(detect_language_from_path(Path::new("README.md")), Language::Markdown);
/// assert_eq!(detect_language_from_path(Path::new("README.adoc")), Language::AsciiDoc);
/// ```
pub fn detect_language_from_path(path: &Path) -> Language {
    let file_name = path
        .file_name()
        .and_then(|s| s.to_str())
        .unwrap_or("")
        .to_ascii_lowercase();

    if file_name == "cmakelists.txt" || file_name.ends_with(".cmake") {
        return Language::CMake;
    }
    if file_name.ends_with(".toml") {
        return Language::Toml;
    }

    let ext = path
        .extension()
        .and_then(|s| s.to_str())
        .unwrap_or("")
        .to_ascii_lowercase();

    match ext.as_str() {
        "c" | "h" => Language::C,
        "cpp" | "hpp" | "cc" | "cxx" => Language::Cpp,
        "rs" => Language::Rust,
        "json" => Language::Json,
        "md" | "markdown" => Language::Markdown,
        "adoc" | "asciidoc" => Language::AsciiDoc,
        "toml" => Language::Toml,
        "cmake" => Language::CMake,
        _ => Language::Unknown,
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Universal Document Parser & Language Dispatcher )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Universal Parsing Dispatcher
// -----------------------------------------------------------------------------

/// Parses any repository source file into an `OmniDoc` AST regardless of language.
///
/// # Examples
///
/// ```rust
/// use std::path::Path;
/// use tl_tools::syntax::parse_document;
///
/// let c_code = "/* BLOCK 1: METADATA BLOCK\n@project Tenebris and Lux\n*/\n";
/// let doc = parse_document(Path::new("main.c"), c_code).expect("Must parse document");
/// assert_eq!(doc.block_1.tag_value("project"), Some("Tenebris and Lux"));
/// ```
pub fn parse_document(_path: &Path, content: &str) -> Result<OmniDoc, String> {
    parse_omni_doc(content)
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
    fn test_section_1_nominal_language_detection() {
        assert_eq!(detect_language_from_path(Path::new("engine/src/main.c")), Language::C);
        assert_eq!(detect_language_from_path(Path::new("engine/include/core/manifest.h")), Language::C);
        assert_eq!(detect_language_from_path(Path::new("engine/src/render.cpp")), Language::Cpp);
        assert_eq!(detect_language_from_path(Path::new("engine/include/render.hpp")), Language::Cpp);
        assert_eq!(detect_language_from_path(Path::new("tools/src/main.rs")), Language::Rust);
        assert_eq!(detect_language_from_path(Path::new("engine/config/manifest.json")), Language::Json);
        assert_eq!(detect_language_from_path(Path::new("README.md")), Language::Markdown);
        assert_eq!(detect_language_from_path(Path::new("docs/README.adoc")), Language::AsciiDoc);
        assert_eq!(detect_language_from_path(Path::new("tools/Cargo.toml")), Language::Toml);
        assert_eq!(detect_language_from_path(Path::new("CMakeLists.txt")), Language::CMake);
        assert_eq!(detect_language_from_path(Path::new("cmake/Testing.cmake")), Language::CMake);
    }

    #[test]
    fn test_section_1_nominal_language_as_str() {
        assert_eq!(Language::C.as_str(), "c");
        assert_eq!(Language::Cpp.as_str(), "cpp");
        assert_eq!(Language::Rust.as_str(), "rust");
        assert_eq!(Language::Json.as_str(), "json");
        assert_eq!(Language::Markdown.as_str(), "markdown");
        assert_eq!(Language::AsciiDoc.as_str(), "asciidoc");
        assert_eq!(Language::Toml.as_str(), "toml");
        assert_eq!(Language::CMake.as_str(), "cmake");
        assert_eq!(Language::Unknown.as_str(), "unknown");
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_unknown_extensions() {
        assert_eq!(detect_language_from_path(Path::new("untracked.xyz")), Language::Unknown);
        assert_eq!(detect_language_from_path(Path::new("no_extension")), Language::Unknown);
        assert_eq!(detect_language_from_path(Path::new("")), Language::Unknown);
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_supported_languages_count() {
        assert_eq!(SUPPORTED_LANGUAGES.len(), 8);
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_all_subsystem_readiness() {
        assert!(framework::is_framework_ready());
        assert!(c::is_c_syntax_ready());
        assert!(cpp::is_cpp_syntax_ready());
        assert!(rust::is_rust_syntax_ready());
        assert!(json::is_json_syntax_ready());
        assert!(md::is_md_syntax_ready());
        assert!(adoc::is_adoc_syntax_ready());
        assert!(toml::is_toml_syntax_ready());
        assert!(cmake::is_cmake_syntax_ready());
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying syntax orchestrator internal consistency.
pub fn self_audit() -> Result<(), String> {
    framework::self_audit()?;
    c::self_audit()?;
    cpp::self_audit()?;
    rust::self_audit()?;
    json::self_audit()?;
    md::self_audit()?;
    adoc::self_audit()?;
    toml::self_audit()?;
    cmake::self_audit()?;
    if SUPPORTED_LANGUAGES.is_empty() {
        return Err("SUPPORTED_LANGUAGES must be non-empty".to_string());
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

/// Epilogue predicate affirming that syntax orchestrator is operational.
pub fn is_syntax_subsystem_ready() -> bool {
    self_audit().is_ok()
}
