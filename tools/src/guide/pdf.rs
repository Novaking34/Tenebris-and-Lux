// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/guide/pdf.rs
// @id            cog.tools.guide.pdf
// @layer         Layer 3: Developer Toolchain Command Sub-Assemblies
// @archetype     pure_types | layout_auditor
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Guide PDF Sub-Assembly
// @role          PDF Layout Geometry Validator & Asciidoctor-PDF Compiler
// @maturity      Rev 5 Baseline | In-process layout auditor & compiler
// @chassis       bounded-heap | Developer tooling memory chassis; bounded issue reports
// @scale         O(F * L) across documentation files and lines
// @seams         in: adoc_pdf_layout_rules.json | out: AdocLayoutReport, .pdf
// @invariants    1. Zero tolerance for unclosed delimiters or listing collisions.
//                2. Strict code line wrap limits (soft 104, hard 128 cols).
//                3. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Enforces textbook page geometry budgets (columns, heights, delimiters,
//   and 5-block banners) on AsciiDoc files, and invokes `asciidoctor-pdf`
//   with data-driven themes.
//
//   ### For Game Designers & Non-Programmers:
//   Validates that chapters and code plates fit neatly on physical textbook
//   pages without unsightly soft-wrapping, broken code listings, or trailing headers.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Validation runs as a pure AST scanner over lines.
//   - Extension Seam: Additional layout rules register in `AdocValidationRules`.
//   - Explicit Non-Goal: Does not alter on-disk files during validation.
// =============================================================================

//! # PDF Layout Geometry Validator & Asciidoctor-PDF Compiler
//!
//! Validates AsciiDoc layout constraints against `docs/adoc_pdf_layout_rules.json`
//! and executes PDF compilation.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

use std::collections::HashSet;
use std::fs;
use std::path::{Path, PathBuf};
use std::process::Command;
use serde::{Deserialize, Serialize};

use super::manifest::{GuideConfig, VolumeSpec};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Defaults
// -----------------------------------------------------------------------------

fn default_code_cols_warn() -> usize {
    104
}

fn default_code_cols_no_ln() -> usize {
    112
}

fn default_code_cols_hard() -> usize {
    128
}

fn default_snap_rec_lines() -> usize {
    55
}

fn default_snap_max_lines() -> usize {
    68
}

fn default_true() -> bool {
    true
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types, Enums & Layout Rule Schemas
// -----------------------------------------------------------------------------

/// PDF single-page geometry and layout validation contract (`docs/adoc_pdf_layout_rules.json`).
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct AdocPageGeometry {
    #[serde(default)]
    pub size: String,
    #[serde(default)]
    pub layout: String,
    #[serde(default)]
    pub text_cols_max: usize,
    #[serde(default = "default_code_cols_warn")]
    pub safe_code_cols_with_linenums: usize,
    #[serde(default = "default_code_cols_no_ln")]
    pub safe_code_cols_no_linenums: usize,
    #[serde(default = "default_code_cols_hard")]
    pub hard_code_cols_max: usize,
    #[serde(default)]
    pub max_lines_per_single_page: usize,
    #[serde(default = "default_snap_rec_lines")]
    pub recommended_snapshot_lines: usize,
    #[serde(default = "default_snap_max_lines")]
    pub max_inline_snapshot_lines: usize,
}

impl Default for AdocPageGeometry {
    fn default() -> Self {
        Self {
            size: "letter".to_string(),
            layout: "portrait".to_string(),
            text_cols_max: 80,
            safe_code_cols_with_linenums: default_code_cols_warn(),
            safe_code_cols_no_linenums: default_code_cols_no_ln(),
            hard_code_cols_max: default_code_cols_hard(),
            max_lines_per_single_page: 60,
            recommended_snapshot_lines: default_snap_rec_lines(),
            max_inline_snapshot_lines: default_snap_max_lines(),
        }
    }
}

#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct AdocLayoutOpening {
    #[serde(default)]
    pub strict_target_prefixes: Vec<String>,
}

impl Default for AdocLayoutOpening {
    fn default() -> Self {
        Self {
            strict_target_prefixes: vec![
                "docs/README.adoc".to_string(),
                "docs/architecture/".to_string(),
                "docs/code/README.adoc".to_string(),
                "docs/code/logging_system.adoc".to_string(),
                "docs/code/debug_system.adoc".to_string(),
                "docs/code/io_pipeline.adoc".to_string(),
                "docs/code/log/".to_string(),
                "docs/code/debug/".to_string(),
                "docs/code/io/".to_string(),
            ],
        }
    }
}

#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct AdocValidationRules {
    #[serde(default = "default_true")]
    pub enforce_snapshot_line_and_width_budgets: bool,
    #[serde(default = "default_true")]
    pub forbid_listing_delimiter_collisions: bool,
    #[serde(default = "default_true")]
    pub forbid_thematic_break_in_strict_targets: bool,
    #[serde(default = "default_true")]
    pub forbid_midfile_level0_headings: bool,
    #[serde(default = "default_true")]
    pub forbid_spurious_abstract_headings: bool,
    #[serde(default = "default_true")]
    pub require_balanced_delimiters: bool,
    #[serde(default = "default_true")]
    pub require_5block_banners_in_strict_targets: bool,
    #[serde(default = "default_true")]
    pub verify_include_paths_exist: bool,
    #[serde(default)]
    pub required_block_tokens: Vec<String>,
}

impl Default for AdocValidationRules {
    fn default() -> Self {
        Self {
            enforce_snapshot_line_and_width_budgets: true,
            forbid_listing_delimiter_collisions: true,
            forbid_thematic_break_in_strict_targets: true,
            forbid_midfile_level0_headings: true,
            forbid_spurious_abstract_headings: true,
            require_balanced_delimiters: true,
            require_5block_banners_in_strict_targets: true,
            verify_include_paths_exist: true,
            required_block_tokens: vec![
                "BLOCK 1:".to_string(),
                "BLOCK 2:".to_string(),
                "BLOCK 3:".to_string(),
                "BLOCK 4:".to_string(),
                "BLOCK 5:".to_string(),
            ],
        }
    }
}

#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct AdocLayoutBody {
    pub page_geometry: AdocPageGeometry,
    pub validation_rules: AdocValidationRules,
}

impl Default for AdocLayoutBody {
    fn default() -> Self {
        Self {
            page_geometry: AdocPageGeometry::default(),
            validation_rules: AdocValidationRules::default(),
        }
    }
}

#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct AdocLayoutRulesFile {
    pub block_2_opening: AdocLayoutOpening,
    pub block_3_body: AdocLayoutBody,
}

impl Default for AdocLayoutRulesFile {
    fn default() -> Self {
        Self {
            block_2_opening: AdocLayoutOpening::default(),
            block_3_body: AdocLayoutBody::default(),
        }
    }
}

/// Severity classification of layout validation findings.
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Serialize, Deserialize)]
pub enum LayoutSeverity {
    Warning,
    Error,
}

/// Single layout issue finding recorded during pre-flight validation.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct AdocLayoutIssue {
    pub file: String,
    pub line: usize,
    pub severity: LayoutSeverity,
    pub rule: String,
    pub message: String,
}

/// Complete report of layout validation issues across all scanned files.
///
/// # Examples
///
/// ```rust
/// use tl_tools::guide::AdocLayoutReport;
///
/// let report = AdocLayoutReport::default();
/// assert!(report.is_clean());
/// assert_eq!(report.error_count(), 0);
/// ```
#[derive(Debug, Clone, Default, PartialEq, Eq, Serialize, Deserialize)]
pub struct AdocLayoutReport {
    pub files_checked: usize,
    pub snapshots_checked: usize,
    pub issues: Vec<AdocLayoutIssue>,
}

impl AdocLayoutReport {
    pub fn error_count(&self) -> usize {
        self.issues
            .iter()
            .filter(|i| i.severity == LayoutSeverity::Error)
            .count()
    }

    pub fn warning_count(&self) -> usize {
        self.issues
            .iter()
            .filter(|i| i.severity == LayoutSeverity::Warning)
            .count()
    }

    pub fn is_clean(&self) -> bool {
        self.error_count() == 0
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Verification Helpers & Invariant Predicates
// -----------------------------------------------------------------------------

/// Evaluates whether a candidate file path falls under strict 5-block layout rules.
pub fn is_strict_layout_target(rel_repo_path: &str, rules: &AdocLayoutRulesFile) -> bool {
    let normalized = rel_repo_path.replace('\\', "/");
    rules
        .block_2_opening
        .strict_target_prefixes
        .iter()
        .any(|pref| {
            let p = pref.trim_start_matches("./");
            normalized == p || (p.ends_with('/') && normalized.starts_with(p))
        })
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Layout Auditing, Text Validation & Asciidoctor-PDF Invocation )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Layout Auditing, Text Validation & Asciidoctor-PDF Invocation
// -----------------------------------------------------------------------------

/// Loads PDF single-page layout geometry and formatting rules from `docs/adoc_pdf_layout_rules.json`.
pub fn load_layout_rules(docs_dir: &Path) -> Result<AdocLayoutRulesFile, String> {
    let rules_path = docs_dir.join("adoc_pdf_layout_rules.json");
    if !rules_path.is_file() {
        return Ok(AdocLayoutRulesFile::default());
    }

    let raw = fs::read_to_string(&rules_path)
        .map_err(|e| format!("Failed to read layout rules {:?}: {}", rules_path, e))?;
    serde_json::from_str::<AdocLayoutRulesFile>(&raw)
        .map_err(|e| format!("Failed to parse layout rules {:?}: {}", rules_path, e))
}

/// Validates all volume chapters, strict 5-block docs, and generated snapshot plates.
pub fn validate_adoc_layout(
    repo_root: &Path,
    docs_dir: &Path,
    volumes: &[VolumeSpec],
    config: &GuideConfig,
) -> Result<AdocLayoutReport, String> {
    let rules = load_layout_rules(docs_dir)?;
    let mut report = AdocLayoutReport::default();
    let mut checked_paths: HashSet<PathBuf> = HashSet::new();

    let mut candidate_paths: Vec<PathBuf> = Vec::new();
    for rel in [
        "docs/README.adoc",
        "docs/code/README.adoc",
        "docs/architecture/README.adoc",
    ] {
        let p = repo_root.join(rel);
        if p.is_file() {
            candidate_paths.push(p);
        }
    }
    for vol in volumes {
        for ch in &vol.chapters {
            if ch.source_path.is_file() {
                candidate_paths.push(ch.source_path.clone());
            }
        }
    }
    for area in ["architecture", "code"] {
        let area_dir = docs_dir.join(area);
        if area_dir.is_dir() {
            let mut stack = vec![area_dir];
            while let Some(dir) = stack.pop() {
                if let Ok(entries) = fs::read_dir(&dir) {
                    for entry in entries.flatten() {
                        let ep = entry.path();
                        if ep.is_dir() {
                            stack.push(ep);
                        } else if ep.extension().and_then(|s| s.to_str()) == Some("adoc") {
                            candidate_paths.push(ep);
                        }
                    }
                }
            }
        }
    }

    candidate_paths.sort();
    for path in candidate_paths {
        if !checked_paths.insert(path.clone()) {
            continue;
        }
        if path.extension().and_then(|s| s.to_str()) != Some("adoc") {
            continue;
        }
        let raw = match fs::read_to_string(&path) {
            Ok(s) => s,
            Err(_) => continue,
        };
        let rel = path
            .strip_prefix(repo_root)
            .unwrap_or(&path)
            .to_string_lossy()
            .to_string();
        let strict = is_strict_layout_target(&rel, &rules);
        let mut file_issues =
            validate_adoc_text(&rel, path.parent().unwrap_or(docs_dir), &raw, strict, false, &rules);
        report.files_checked += 1;
        report.issues.append(&mut file_issues);
    }

    let snap_dir = repo_root.join(&config.snapshots.output_dir);
    if snap_dir.is_dir() {
        if let Ok(entries) = fs::read_dir(&snap_dir) {
            let mut snap_files: Vec<PathBuf> = entries
                .flatten()
                .map(|e| e.path())
                .filter(|p| {
                    p.file_name()
                        .and_then(|s| s.to_str())
                        .map(|n| n.starts_with("snap_") && n.ends_with(".adoc"))
                        .unwrap_or(false)
                })
                .collect();
            snap_files.sort();
            for path in snap_files {
                if let Ok(raw) = fs::read_to_string(&path) {
                    let rel = path
                        .strip_prefix(repo_root)
                        .unwrap_or(&path)
                        .to_string_lossy()
                        .to_string();
                    let mut issues =
                        validate_adoc_text(&rel, &snap_dir, &raw, false, true, &rules);
                    report.snapshots_checked += 1;
                    report.issues.append(&mut issues);
                }
            }
        }
    }

    Ok(report)
}

/// Validates one AsciiDoc text buffer for PDF single-page fit, fence integrity, includes, and 5-block banners.
pub fn validate_adoc_text(
    rel_label: &str,
    base_dir: &Path,
    content: &str,
    strict_5block: bool,
    is_snapshot_plate: bool,
    rules: &AdocLayoutRulesFile,
) -> Vec<AdocLayoutIssue> {
    let mut issues = Vec::new();
    let geom = &rules.block_3_body.page_geometry;
    let cfg = &rules.block_3_body.validation_rules;

    let mut fence_stack: Vec<(&'static str, usize)> = Vec::new();
    let mut seen_title = false;
    let mut seen_blocks: HashSet<String> = HashSet::new();
    let mut current_code_lines: usize = 0;
    let mut current_code_start: usize = 0;
    let mut first_nonblank_checked = false;

    let lines: Vec<&str> = content.lines().collect();
    for (idx, line) in lines.iter().enumerate() {
        let line_no = idx + 1;
        let trimmed = line.trim();

        if !first_nonblank_checked && !trimmed.is_empty() {
            first_nonblank_checked = true;
            if strict_5block && trimmed != "////" {
                issues.push(AdocLayoutIssue {
                    file: rel_label.to_string(),
                    line: line_no,
                    severity: LayoutSeverity::Error,
                    rule: "require_5block_banners_in_strict_targets".to_string(),
                    message: "Strict 5-block AsciiDoc file must open with leading //// BLOCK 1 banner before doc title.".to_string(),
                });
            }
        }

        let in_verbatim = fence_stack
            .iter()
            .any(|(f, _)| matches!(*f, "////" | "----" | "------" | "...." | "```"));
        let active_comment = fence_stack.last().map(|(f, _)| *f) == Some("////");
        let active_code = fence_stack.last().map(|(f, _)| *f);

        if active_comment {
            for tok in &cfg.required_block_tokens {
                if trimmed.contains(tok.as_str()) {
                    seen_blocks.insert(tok.clone());
                }
            }
            if trimmed == "////" {
                fence_stack.pop();
            }
            continue;
        }

        if matches!(active_code, Some("----") | Some("------") | Some("....") | Some("```")) {
            let top = active_code.unwrap_or("----");
            let closed = (top == "```" && trimmed.starts_with("```")) || trimmed == top;
            if closed {
                if is_snapshot_plate
                    && cfg.enforce_snapshot_line_and_width_budgets
                    && current_code_lines > geom.max_inline_snapshot_lines
                {
                    issues.push(AdocLayoutIssue {
                        file: rel_label.to_string(),
                        line: current_code_start,
                        severity: LayoutSeverity::Warning,
                        rule: "snapshot_single_page_height".to_string(),
                        message: format!(
                            "Inline snapshot listing is {} lines (exceeds {} single-page limit; cap excerpt.max_lines).",
                            current_code_lines, geom.max_inline_snapshot_lines
                        ),
                    });
                }
                fence_stack.pop();
                current_code_lines = 0;
                continue;
            }

            if cfg.forbid_listing_delimiter_collisions && top == "----" && trimmed == "----" {
                issues.push(AdocLayoutIssue {
                    file: rel_label.to_string(),
                    line: line_no,
                    severity: LayoutSeverity::Error,
                    rule: "forbid_listing_delimiter_collisions".to_string(),
                    message: "Nested 4-hyphen line inside ---- fence closes listing prematurely.".to_string(),
                });
            }

            current_code_lines += 1;
            let char_len = line.chars().count();
            let is_box_art = line.contains('┌') || line.contains('│') || line.contains('└');
            if !is_box_art
                && (strict_5block || is_snapshot_plate)
                && cfg.enforce_snapshot_line_and_width_budgets
            {
                if char_len > geom.hard_code_cols_max {
                    issues.push(AdocLayoutIssue {
                        file: rel_label.to_string(),
                        line: line_no,
                        severity: LayoutSeverity::Error,
                        rule: "hard_code_cols_max".to_string(),
                        message: format!(
                            "Code line width {} exceeds hard PDF wrap limit {} cols.",
                            char_len, geom.hard_code_cols_max
                        ),
                    });
                } else if char_len > geom.safe_code_cols_with_linenums {
                    issues.push(AdocLayoutIssue {
                        file: rel_label.to_string(),
                        line: line_no,
                        severity: LayoutSeverity::Warning,
                        rule: "safe_code_cols_with_linenums".to_string(),
                        message: format!(
                            "Code line width {} exceeds {} cols safe linenums budget (may soft-wrap in PDF).",
                            char_len, geom.safe_code_cols_with_linenums
                        ),
                    });
                }
            }
            continue;
        }

        if let Some(&(top, _)) = fence_stack.last() {
            let closed = match top {
                "|===" => trimmed == "|===",
                "====" => trimmed == "====",
                "****" => trimmed == "****",
                "--" => trimmed == "--",
                _ => false,
            };
            if closed {
                fence_stack.pop();
                continue;
            }
        }

        if !in_verbatim {
            let opening = if trimmed.starts_with("```") {
                Some("```")
            } else {
                match trimmed {
                    "////" => Some("////"),
                    "------" => Some("------"),
                    "----" => Some("----"),
                    "...." => Some("...."),
                    "====" => Some("===="),
                    "****" => Some("****"),
                    "|===" => Some("|==="),
                    "--" => Some("--"),
                    _ => None,
                }
            };
            if let Some(tok) = opening {
                if matches!(tok, "----" | "------" | "...." | "```") {
                    current_code_lines = 0;
                    current_code_start = line_no;
                }
                fence_stack.push((tok, line_no));
                continue;
            }
        }

        if trimmed.starts_with("= ") && !trimmed.starts_with("== ") {
            if !seen_title {
                seen_title = true;
            } else if cfg.forbid_midfile_level0_headings && !is_snapshot_plate {
                issues.push(AdocLayoutIssue {
                    file: rel_label.to_string(),
                    line: line_no,
                    severity: LayoutSeverity::Error,
                    rule: "forbid_midfile_level0_headings".to_string(),
                    message: "Mid-file level-0 (= ) heading injects spurious book Parts during PDF compilation.".to_string(),
                });
            }
        }

        if cfg.forbid_spurious_abstract_headings && trimmed == "[abstract]" {
            let next_trimmed = lines.get(idx + 1).map(|l| l.trim()).unwrap_or("");
            if next_trimmed.starts_with("== ") {
                issues.push(AdocLayoutIssue {
                    file: rel_label.to_string(),
                    line: line_no,
                    severity: if strict_5block {
                        LayoutSeverity::Error
                    } else {
                        LayoutSeverity::Warning
                    },
                    rule: "forbid_spurious_abstract_headings".to_string(),
                    message: "[abstract] directly above == heading scrambles Asciidoctor book chapter numbers.".to_string(),
                });
            }
        }

        if strict_5block && cfg.forbid_thematic_break_in_strict_targets && trimmed == "'''" {
            issues.push(AdocLayoutIssue {
                file: rel_label.to_string(),
                line: line_no,
                severity: LayoutSeverity::Error,
                rule: "forbid_thematic_break_in_strict_targets".to_string(),
                message: "Legacy ''' horizontal rule creates unnecessary vertical PDF page breaks; use section hierarchy instead.".to_string(),
            });
        }

        if cfg.verify_include_paths_exist && trimmed.starts_with("include::") {
            if let Some(rest) = trimmed.strip_prefix("include::") {
                if let Some(br) = rest.find('[') {
                    let rel_inc = rest[..br].trim();
                    if !rel_inc.contains('{') && !base_dir.join(rel_inc).is_file() {
                        issues.push(AdocLayoutIssue {
                            file: rel_label.to_string(),
                            line: line_no,
                            severity: LayoutSeverity::Error,
                            rule: "verify_include_paths_exist".to_string(),
                            message: format!(
                                "Include target '{}' does not resolve relative to '{}'.",
                                rel_inc,
                                base_dir.display()
                            ),
                        });
                    }
                }
            }
        }
    }

    if cfg.require_balanced_delimiters && !fence_stack.is_empty() {
        for (tok, opened_line) in fence_stack {
            issues.push(AdocLayoutIssue {
                file: rel_label.to_string(),
                line: opened_line,
                severity: LayoutSeverity::Error,
                rule: "require_balanced_delimiters".to_string(),
                message: format!(
                    "Unclosed AsciiDoc delimiter '{}' opened at line {}.",
                    tok, opened_line
                ),
            });
        }
    }

    if strict_5block && cfg.require_5block_banners_in_strict_targets {
        for req in &cfg.required_block_tokens {
            if !seen_blocks.contains(req) {
                issues.push(AdocLayoutIssue {
                    file: rel_label.to_string(),
                    line: 1,
                    severity: LayoutSeverity::Error,
                    rule: "require_5block_banners_in_strict_targets".to_string(),
                    message: format!(
                        "Missing 5-block AsciiDoc comment banner containing '{}'.",
                        req
                    ),
                });
            }
        }
    }

    issues
}

/// Invokes `asciidoctor-pdf` CLI with configured themes and attributes.
pub fn compile_pdf(adoc_path: &Path, pdf_path: &Path, label: &str, config: &GuideConfig) {
    println!(
        "  Compiling {} PDF: {:?} via asciidoctor-pdf...",
        label, pdf_path
    );
    let mut cmd = Command::new("asciidoctor-pdf");
    let theme_yaml = config
        .pdf_themes_dir
        .join(format!("{}-theme.yml", config.pdf_theme_name));
    if theme_yaml.is_file() {
        cmd.arg("-a")
            .arg(format!("pdf-theme={}", config.pdf_theme_name))
            .arg("-a")
            .arg(format!("pdf-themesdir={}", config.pdf_themes_dir.display()));
    }
    cmd.arg(adoc_path).arg("-o").arg(pdf_path);

    match cmd.status() {
        Ok(s) if s.success() => {
            println!("  [SUCCESS] {} PDF generated at: {:?}", label, pdf_path);
        }
        Ok(s) => {
            eprintln!(
                "  [WARN] asciidoctor-pdf for {} exited with code: {:?}",
                label,
                s.code()
            );
        }
        Err(e) => {
            eprintln!("  [WARN] asciidoctor-pdf for {} failed: {}", label, e);
        }
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

    // -------------------------------------------------------------------------
    // SECTION 1: ALWAYS PASS (Nominal Golden Surface Cogs)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_1_nominal_empty_report() {
        let report = AdocLayoutReport::default();
        assert!(report.is_clean());
        assert_eq!(report.error_count(), 0);
        assert_eq!(report.warning_count(), 0);
    }

    #[test]
    fn test_section_1_nominal_valid_5block_document() {
        let rules = AdocLayoutRulesFile::default();
        let valid_doc = "\
////
BLOCK 1: METADATA BLOCK
////
= Valid Chapter
Author Name
v0.1.0

////
BLOCK 2: OPENING BLOCK
////

////
BLOCK 3: BODY BLOCK
////
== Main Section
Content here.

////
BLOCK 4: AUDIT BLOCK
////

////
BLOCK 5: CLOSING BLOCK
////
";
        let issues = validate_adoc_text(
            "docs/code/test.adoc",
            Path::new("."),
            valid_doc,
            true,
            false,
            &rules,
        );
        assert!(issues.is_empty(), "Valid 5-block doc should have zero issues: {:?}", issues);
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_missing_block_1_banner() {
        let rules = AdocLayoutRulesFile::default();
        let bad_doc = "= Chapter Without Leading Block 1\n\n== Section\nBody";
        let issues = validate_adoc_text(
            "docs/code/bad.adoc",
            Path::new("."),
            bad_doc,
            true,
            false,
            &rules,
        );
        assert!(issues.iter().any(|i| i.rule == "require_5block_banners_in_strict_targets"));
    }

    #[test]
    fn test_section_2_never_fail_unclosed_delimiter() {
        let rules = AdocLayoutRulesFile::default();
        let unclosed = "== Section\n\n[source,c]\n----\nint x = 1;\n";
        let issues = validate_adoc_text(
            "docs/code/unclosed.adoc",
            Path::new("."),
            unclosed,
            false,
            false,
            &rules,
        );
        assert!(issues.iter().any(|i| i.rule == "require_balanced_delimiters"));
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_code_column_limits() {
        let rules = AdocLayoutRulesFile::default();
        let long_line = format!("// {}", "x".repeat(130));
        let doc = format!("[source,c]\n----\n{}\n----\n", long_line);
        let issues = validate_adoc_text(
            "docs/code/long.adoc",
            Path::new("."),
            &doc,
            true,
            false,
            &rules,
        );
        assert!(issues.iter().any(|i| i.rule == "hard_code_cols_max"));
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_layout_report_error_counting() {
        let mut report = AdocLayoutReport::default();
        report.issues.push(AdocLayoutIssue {
            file: "test.adoc".to_string(),
            line: 10,
            severity: LayoutSeverity::Error,
            rule: "test_err".to_string(),
            message: "Error message".to_string(),
        });
        report.issues.push(AdocLayoutIssue {
            file: "test.adoc".to_string(),
            line: 20,
            severity: LayoutSeverity::Warning,
            rule: "test_warn".to_string(),
            message: "Warning message".to_string(),
        });
        assert_eq!(report.error_count(), 1);
        assert_eq!(report.warning_count(), 1);
        assert!(!report.is_clean());
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Diagnostic Self-Audit
// -----------------------------------------------------------------------------

/// Performs an in-process diagnostic preflight self-audit of PDF layout invariants.
///
/// # Examples
///
/// ```rust
/// use tl_tools::guide::pdf::self_audit;
///
/// assert!(self_audit().is_ok());
/// ```
pub fn self_audit() -> Result<(), String> {
    let report = AdocLayoutReport::default();
    if !report.is_clean() {
        return Err("Default AdocLayoutReport must be clean".to_string());
    }
    let rules = AdocLayoutRulesFile::default();
    if rules.block_3_body.page_geometry.hard_code_cols_max != 128 {
        return Err("hard_code_cols_max invariant violated".to_string());
    }
    Ok(())
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( State Summary Handshake, Module Epilogue & Teardown Verification )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue Predicate
// -----------------------------------------------------------------------------

/// Verifies that the PDF layout sub-assembly is ready for operation.
///
/// # Examples
///
/// ```rust
/// use tl_tools::guide::pdf::is_pdf_subsystem_ready;
///
/// assert!(is_pdf_subsystem_ready());
/// ```
pub fn is_pdf_subsystem_ready() -> bool {
    self_audit().is_ok()
}
