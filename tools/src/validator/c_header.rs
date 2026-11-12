// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/validator/c_header.rs
// @id            tools.validator.c_header
// @layer         Layer 3: Systems Developer Tooling Core Library
// @version       0.1.0
// @interface_rev Rev 1
// @date          2026-11-09
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   C11 5-Block Header Validator. Enforces the OmniCode Universal 5-Block
//   Lifecycle Protocol, Invariant 18 (zero // comments), Anti-Domain-Bleed
//   covenants, two-sub-assembly Block 4 & Block 5 standards, and subsystem
//   archetype requirements across all headers in `engine/include/`.
// =============================================================================

//! # C11 Header Validator Engine
//!
//! Validates `engine/include/` headers against OmniCode standards, archetype
//! specifications, and invariant contracts.

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES & STRUCTURES
// Stratum: Developer Tooling | Machine: tools.validator.c_header
// =============================================================================

use std::fs;
use std::path::{Path, PathBuf};
use std::time::Instant;

use crate::runner::{RunnerContext, TaskRunner, TaskTelemetry};
use super::config::{HeaderArchetype, HeaderRulesConfig, SubsystemRecord, SubsystemRegistry};
use super::report::{IssueSeverity, ValidationIssue, ValidationSummary};

#[derive(Debug, Clone)]
struct ParsedBlock1 {
    lines: Vec<String>,
    tags: Vec<(String, String, bool)>,
    designer_text: String,
    programmer_text: String,
    sections_in_order: bool,
    sections_indented: bool,
}

impl ParsedBlock1 {
    fn tag_value(&self, name: &str) -> Option<&str> {
        self.tags
            .iter()
            .find(|(k, _, _)| k == name)
            .map(|(_, v, _)| v.as_str())
    }
}

#[derive(Debug, Clone, PartialEq, Eq)]
enum TopLevelCItem {
    FuncDef(String),
    OtherDecl(String),
}

#[derive(Debug, Clone, PartialEq, Eq)]
struct TopLevelCItemWithPos {
    start: usize,
    item: TopLevelCItem,
}

#[derive(Debug, Clone)]
struct ParsedFuncDoc {
    name: String,
    sub_assembly_idx: usize,
    designer_text: String,
    programmer_text: String,
}

#[derive(Debug, Clone)]
struct ParsedBlock3 {
    sub_assemblies: Vec<(String, String)>,
    functions: Vec<ParsedFuncDoc>,
}

fn strip_comment_decor_prefix(raw_line: &str) -> &str {
    raw_line
        .trim_start()
        .trim_start_matches('/')
        .trim_start_matches('*')
        .trim_start_matches('-')
        .trim_start_matches('=')
        .trim_start()
}

fn find_sub_assembly_pos(text: &str, num: &str) -> Option<usize> {
    let prefix = format!("Sub-Assembly {}", num);
    let mut offset = 0usize;
    for raw_line in text.split_inclusive('\n') {
        let stripped = strip_comment_decor_prefix(raw_line);
        if let Some(after) = stripped.strip_prefix(&prefix) {
            if after.is_empty()
                || after.starts_with(':')
                || after.starts_with(' ')
                || after.starts_with('(')
            {
                return Some(offset);
            }
        }
        offset += raw_line.len();
    }
    None
}

fn extract_quoted_include(trimmed_line: &str) -> Option<&str> {
    let rest = trimmed_line.strip_prefix('#')?.trim_start();
    let rest = rest.strip_prefix("include")?.trim_start();
    let after_open = rest.strip_prefix('"')?;
    let end = after_open.find('"')?;
    Some(&after_open[..end])
}

fn has_c_word(haystack: &str, word: &str) -> bool {
    haystack
        .split(|ch: char| !(ch.is_ascii_alphanumeric() || ch == '_'))
        .any(|tok| tok == word)
}

fn strip_comments_and_strings(src: &str) -> String {
    /* Mutate ASCII bytes in place so every non-comment character keeps its exact byte offset. */
    let mut buf = src.as_bytes().to_vec();
    let len = buf.len();
    let mut i = 0usize;

    while i < len {
        let b = buf[i];
        if b == b'/' && i + 1 < len && buf[i + 1] == b'*' {
            buf[i] = b' ';
            buf[i + 1] = b' ';
            i += 2;
            while i < len {
                if buf[i] == b'*' && i + 1 < len && buf[i + 1] == b'/' {
                    buf[i] = b' ';
                    buf[i + 1] = b' ';
                    i += 2;
                    break;
                }
                if buf[i] != b'\n' {
                    buf[i] = b' ';
                }
                i += 1;
            }
            continue;
        }
        if b == b'/' && i + 1 < len && buf[i + 1] == b'/' {
            while i < len && buf[i] != b'\n' {
                buf[i] = b' ';
                i += 1;
            }
            continue;
        }
        if b == b'"' || b == b'\'' {
            let quote = b;
            i += 1;
            while i < len {
                let c = buf[i];
                if c == b'\n' {
                    break;
                }
                if c == b'\\' {
                    buf[i] = b' ';
                    if i + 1 < len && buf[i + 1] != b'\n' {
                        buf[i + 1] = b' ';
                        i += 2;
                    } else {
                        i += 1;
                    }
                    continue;
                }
                if c == quote {
                    i += 1;
                    break;
                }
                buf[i] = b' ';
                i += 1;
            }
            continue;
        }
        i += 1;
    }

    String::from_utf8(buf).expect("UTF-8 invariant preserved while blanking ASCII comment/string ranges")
}

fn strip_comments_strings_and_pp(src: &str) -> String {
    let mut buf = strip_comments_and_strings(src).into_bytes();
    let len = buf.len();

    /* Phase 2: blank out preprocessor logical lines (including '\' continuations), preserving offsets. */
    let mut in_pp = false;
    let mut line_start = 0usize;
    while line_start < len {
        let mut line_end = line_start;
        while line_end < len && buf[line_end] != b'\n' {
            line_end += 1;
        }
        let line_slice = &buf[line_start..line_end];
        let trimmed_start = line_slice
            .iter()
            .position(|&c| !c.is_ascii_whitespace())
            .unwrap_or(line_slice.len());
        let trimmed_end = line_slice
            .iter()
            .rposition(|&c| !c.is_ascii_whitespace())
            .map(|p| p + 1)
            .unwrap_or(trimmed_start);
        let trimmed = if trimmed_start < trimmed_end {
            &line_slice[trimmed_start..trimmed_end]
        } else {
            &[]
        };

        let is_pp_line = if in_pp {
            in_pp = trimmed.ends_with(b"\\");
            true
        } else if trimmed.starts_with(b"#") {
            in_pp = trimmed.ends_with(b"\\");
            true
        } else {
            false
        };

        if is_pp_line {
            buf[line_start..line_end].fill(b' ');
        }

        line_start = if line_end < len { line_end + 1 } else { len };
    }

    String::from_utf8(buf).expect("UTF-8 invariant preserved while blanking ASCII comment/pp ranges")
}

fn extract_top_level_c_items_with_pos(cleaned: &str) -> Vec<TopLevelCItemWithPos> {
    let mut items = Vec::new();
    let mut depth = 0usize;
    let mut buf = String::new();
    let mut pending_header = String::new();
    let mut in_brace_init_or_type = false;
    let mut item_start: Option<usize> = None;

    for (idx, ch) in cleaned.char_indices() {
        match ch {
            '{' => {
                if depth == 0 {
                    if item_start.is_none() {
                        item_start = Some(idx);
                    }
                    let hdr = buf.trim().to_string();
                    let looks_like_type = has_c_word(&hdr, "typedef")
                        || (!hdr.contains('(')
                            && (has_c_word(&hdr, "struct")
                                || has_c_word(&hdr, "enum")
                                || has_c_word(&hdr, "union")));
                    in_brace_init_or_type = hdr.contains('=') || looks_like_type || !hdr.contains('(');
                    pending_header = hdr;
                }
                depth += 1;
            }
            '}' => {
                if depth > 0 {
                    depth -= 1;
                    if depth == 0 && !in_brace_init_or_type {
                        let start = item_start.take().unwrap_or(0);
                        let hdr = std::mem::take(&mut pending_header);
                        buf.clear();
                        if !hdr.is_empty() {
                            items.push(TopLevelCItemWithPos {
                                start,
                                item: TopLevelCItem::FuncDef(hdr),
                            });
                        }
                    }
                }
            }
            ';' if depth == 0 => {
                let start = item_start.take().unwrap_or(0);
                let decl = std::mem::take(&mut buf).trim().to_string();
                pending_header.clear();
                in_brace_init_or_type = false;
                if !decl.is_empty() {
                    items.push(TopLevelCItemWithPos {
                        start,
                        item: TopLevelCItem::OtherDecl(decl),
                    });
                }
            }
            _ if depth == 0 => {
                if item_start.is_none() && !ch.is_ascii_whitespace() {
                    item_start = Some(idx);
                }
                buf.push(ch);
            }
            _ => {}
        }
    }
    items
}

fn extract_top_level_c_items(cleaned: &str) -> Vec<TopLevelCItem> {
    extract_top_level_c_items_with_pos(cleaned)
        .into_iter()
        .map(|it| it.item)
        .collect()
}

#[derive(Debug, Clone)]
struct PrecedingDoxygen {
    has_brief: bool,
    has_dual_designer: bool,
    has_dual_programmer: bool,
    has_param_tag: bool,
    has_return_tag: bool,
    designer_text: String,
    programmer_text: String,
}

fn parse_b3_sub_assemblies(b3_slice: &str) -> Vec<(String, usize, String, usize)> {
    let mut sections = Vec::new();
    let mut offset = 0usize;
    for raw_line in b3_slice.split_inclusive('\n') {
        let stripped = strip_comment_decor_prefix(raw_line);
        if let Some(after_sa) = stripped.strip_prefix("Sub-Assembly 3.") {
            let digits_len = after_sa
                .bytes()
                .take_while(|b| b.is_ascii_digit())
                .count();
            if digits_len > 0 {
                let minor_str = &after_sa[..digits_len];
                let rest = &after_sa[digits_len..];
                if rest.is_empty() || rest.starts_with(':') || rest.starts_with(" (") {
                    let minor = minor_str.parse::<usize>().unwrap_or(0);
                    let raw_title = rest.strip_prefix(':').unwrap_or(rest).trim();
                    let title = raw_title
                        .trim_end_matches("*/")
                        .trim_end_matches(|c: char| c == '-' || c == '=' || c.is_ascii_whitespace())
                        .to_string();
                    sections.push((format!("3.{}", minor_str), minor, title, offset));
                }
            }
        }
        offset += raw_line.len();
    }
    sections
}

fn extract_c_func_sig(raw: &str, is_def: bool) -> Option<(String, bool, bool)> {
    let open_idx = raw.find('(')?;
    let before = raw[..open_idx].trim();
    if before.is_empty()
        || before.contains('=')
        || has_c_word(before, "typedef")
        || has_c_word(before, "static_assert")
        || has_c_word(before, "_Static_assert")
    {
        return None;
    }

    let mut paren_depth = 0usize;
    let mut close_idx = None;
    for (rel_i, ch) in raw[open_idx..].char_indices() {
        match ch {
            '(' => paren_depth += 1,
            ')' => {
                paren_depth = paren_depth.saturating_sub(1);
                if paren_depth == 0 {
                    close_idx = Some(open_idx + rel_i);
                    break;
                }
            }
            _ => {}
        }
    }
    let close_idx = close_idx?;
    let fn_start = before
        .rfind(|c: char| !(c.is_ascii_alphanumeric() || c == '_'))
        .map(|i| i + 1)
        .unwrap_or(0);
    let fn_name = &before[fn_start..];
    let ret_part = before[..fn_start].trim();

    if fn_name.is_empty()
        || fn_name.chars().next().is_none_or(|c| c.is_ascii_digit())
        || ret_part.is_empty()
        || matches!(fn_name, "return" | "if" | "while" | "for" | "switch" | "sizeof")
    {
        return None;
    }

    if !is_def && raw[close_idx + 1..].contains('=') {
        return None;
    }

    let params_str = raw[open_idx + 1..close_idx].trim();
    let has_params = !params_str.is_empty() && params_str != "void";
    let returns_value = if ret_part.contains('*') {
        true
    } else {
        ret_part.split_whitespace().last() != Some("void")
    };

    Some((fn_name.to_string(), has_params, returns_value))
}

fn extract_preceding_doxygen(
    full_src: &str,
    abs_item_start: usize,
    dual_sections: &[String],
) -> Option<PrecedingDoxygen> {
    let before = full_src[..abs_item_start].trim_end();
    if !before.ends_with("*/") {
        return None;
    }
    let open_idx = before.rfind("/*")?;
    let raw_doc = &before[open_idx..];
    if !raw_doc.starts_with("/**")
        || raw_doc.contains("BLOCK 3: BODY BLOCK")
        || raw_doc
            .lines()
            .any(|l| strip_comment_decor_prefix(l).starts_with("Sub-Assembly 3."))
    {
        return None;
    }

    let has_brief = raw_doc.contains("@brief");
    let has_param_tag = raw_doc.contains("@param");
    let has_return_tag = raw_doc.contains("@return");

    let cleaned_lines: Vec<String> = raw_doc
        .lines()
        .map(|line| {
            let mut s = line.trim();
            if let Some(after_open) = s.strip_prefix("/**") {
                s = after_open.trim();
            } else if let Some(after_open) = s.strip_prefix("/*") {
                s = after_open.trim();
            }
            if let Some(before_close) = s.strip_suffix("*/") {
                s = before_close.trim();
            }
            if let Some(after_star) = s.strip_prefix('*') {
                s = after_star.trim();
            }
            s.to_string()
        })
        .collect();

    let d_hdr = dual_sections
        .first()
        .map(|s| s.as_str())
        .unwrap_or("### For Game Designers & Non-Programmers:");
    let p_hdr = dual_sections
        .get(1)
        .map(|s| s.as_str())
        .unwrap_or("### For Systems Programmers & Engine Architects:");

    let d_pos = cleaned_lines.iter().position(|l| l == d_hdr);
    let p_pos = cleaned_lines.iter().position(|l| l == p_hdr);

    let (designer_text, programmer_text) = match (d_pos, p_pos) {
        (Some(dp), Some(pp)) if dp < pp => {
            let d_txt = cleaned_lines[dp + 1..pp]
                .iter()
                .map(String::as_str)
                .take_while(|l| !l.starts_with('@'))
                .filter(|l| !l.is_empty())
                .collect::<Vec<_>>()
                .join(" ");
            let p_txt = cleaned_lines[pp + 1..]
                .iter()
                .map(String::as_str)
                .take_while(|l| !l.starts_with("@param") && !l.starts_with("@return"))
                .filter(|l| !l.is_empty())
                .collect::<Vec<_>>()
                .join(" ");
            (d_txt, p_txt)
        }
        _ => (String::new(), String::new()),
    };

    Some(PrecedingDoxygen {
        has_brief,
        has_dual_designer: d_pos.is_some(),
        has_dual_programmer: p_pos.is_some(),
        has_param_tag,
        has_return_tag,
        designer_text,
        programmer_text,
    })
}

fn is_valid_cog_id(id: &str) -> bool {
    let Some(rest) = id.strip_prefix("cog.") else {
        return false;
    };
    let parts: Vec<&str> = rest.split('.').collect();
    if parts.is_empty() || parts.len() > 2 {
        return false;
    }
    parts.iter().all(|seg| {
        !seg.is_empty()
            && seg
                .bytes()
                .all(|b| b.is_ascii_lowercase() || b.is_ascii_digit() || b == b'_')
    })
}

fn archetype_prefix(archetype: HeaderArchetype, stateful: bool) -> String {
    let arch_str = match archetype {
        HeaderArchetype::Operational => "operational",
        HeaderArchetype::QuarantinedFallback => "quarantined_fallback",
        HeaderArchetype::PureTypes => "pure_types",
        HeaderArchetype::FacadeUmbrella => "facade_umbrella",
    };
    let state_str = if stateful { "stateful" } else { "stateless" };
    format!("{} | {}", arch_str, state_str)
}

fn parse_block_1_comment(content: &str, col_width: usize) -> Result<ParsedBlock1, String> {
    let trimmed = content.trim_start_matches('\u{feff}').trim_start();
    if !trimmed.starts_with("/*") {
        return Err("File must open with a C-style /* Block 1 comment".to_string());
    }
    let Some(close_idx) = trimmed.find("*/") else {
        return Err("Unclosed Block 1 /* ... */ comment".to_string());
    };
    let raw_block = &trimmed[..close_idx + 2];
    let lines: Vec<String> = raw_block
        .lines()
        .map(|l| l.trim_end_matches('\r').to_string())
        .collect();

    if lines.len() < 8 {
        return Err("Block 1 comment is unexpectedly short".to_string());
    }

    let purpose_idx = lines
        .iter()
        .position(|l| l.trim() == "PURPOSE & ARCHITECTURAL ROLE:");
    let tag_end_idx = purpose_idx.unwrap_or(lines.len().saturating_sub(2));

    let mut tags: Vec<(String, String, bool)> = Vec::new();
    for line in &lines[5..tag_end_idx] {
        let l = line.as_str();
        if l.trim().is_empty() {
            continue;
        }
        if l.starts_with('@') {
            let tag_end = l.find(|c: char| c.is_ascii_whitespace()).unwrap_or(l.len());
            let tag_name = l[..tag_end].to_string();
            let after_tag = &l[tag_end..];
            let spaces = after_tag.chars().take_while(|&c| c == ' ').count();
            let expected_spaces = if col_width > tag_end {
                col_width - tag_end
            } else {
                1
            };
            let aligned = spaces == expected_spaces && !after_tag[spaces..].trim().is_empty();
            let val = l[tag_end..].trim().to_string();
            tags.push((tag_name, val, aligned));
        } else if let Some((_, last_val, last_aligned)) = tags.last_mut() {
            let indent = l.chars().take_while(|&c| c == ' ').count();
            if indent != col_width {
                *last_aligned = false;
            }
            if !last_val.is_empty() {
                last_val.push('\n');
            }
            last_val.push_str(l.trim());
        } else {
            return Err(format!("Unexpected non-tag line in Block 1 tag section: '{}'", l));
        }
    }

    let mut designer_text = String::new();
    let mut programmer_text = String::new();
    let mut sections_in_order = false;
    let mut sections_indented = false;

    if let Some(p_idx) = purpose_idx {
        let body_lines = &lines[p_idx + 1..lines.len().saturating_sub(2)];
        let d_pos = body_lines
            .iter()
            .position(|l| l.trim() == "### For Game Designers & Non-Programmers:");
        let prog_pos = body_lines
            .iter()
            .position(|l| l.trim() == "### For Systems Programmers & Engine Architects:");

        if let (Some(dp), Some(pp)) = (d_pos, prog_pos) {
            if dp < pp {
                sections_in_order = true;
                sections_indented = body_lines[dp].starts_with("  ### ")
                    && body_lines[pp].starts_with("  ### ");
                designer_text = body_lines[dp + 1..pp]
                    .iter()
                    .map(|l| l.trim())
                    .filter(|l| !l.is_empty())
                    .collect::<Vec<_>>()
                    .join(" ");
                programmer_text = body_lines[pp + 1..]
                    .iter()
                    .map(|l| l.trim())
                    .filter(|l| !l.is_empty())
                    .collect::<Vec<_>>()
                    .join(" ");
            }
        }
    }

    Ok(ParsedBlock1 {
        lines,
        tags,
        designer_text,
        programmer_text,
        sections_in_order,
        sections_indented,
    })
}

/// Validator auditing C11 header files against 5-block architecture.
pub struct HeaderValidator {
    repo_root: PathBuf,
    registry: SubsystemRegistry,
    rules: HeaderRulesConfig,
    verbose: bool,
}

// =============================================================================
// BLOCK 3: BODY BLOCK / CORE VALIDATION IMPLEMENTATION
// Stratum: Developer Tooling | Machine: tools.validator.c_header
// =============================================================================

impl HeaderValidator {
    /// Creates a new HeaderValidator with loaded registry and rules.
    pub fn new(repo_root: PathBuf, registry: SubsystemRegistry, rules: HeaderRulesConfig, verbose: bool) -> Self {
        Self {
            repo_root,
            registry,
            rules,
            verbose,
        }
    }

    /// Recursively discovers all `*.h` files in `engine/include/`.
    pub fn discover_headers(include_dir: &Path) -> Vec<PathBuf> {
        let mut headers = Vec::new();
        if !include_dir.exists() || !include_dir.is_dir() {
            return headers;
        }

        let mut stack = vec![include_dir.to_path_buf()];
        while let Some(current) = stack.pop() {
            if let Ok(entries) = fs::read_dir(&current) {
                for entry in entries.flatten() {
                    let path = entry.path();
                    if path.is_dir() {
                        stack.push(path);
                    } else if path.is_file() {
                        if let Some(ext) = path.extension() {
                            if ext == "h" {
                                headers.push(path);
                            }
                        }
                    }
                }
            }
        }
        headers.sort();
        headers
    }

    /// Executes validation sweep across all registered headers.
    pub fn run_audit(&self) -> Result<ValidationSummary, String> {
        let start = Instant::now();
        let include_dir = self.repo_root.join("engine/include");

        if !include_dir.exists() {
            return Err(format!("Engine include directory not found: {:?}", include_dir));
        }

        let header_files = Self::discover_headers(&include_dir);
        let mut summary = ValidationSummary {
            total_files: header_files.len(),
            ..Default::default()
        };

        for file_path in &header_files {
            let mut file_issues = Vec::new();
            self.validate_header(file_path, &mut file_issues);

            let has_errors = file_issues.iter().any(|i| i.severity == IssueSeverity::Error);
            if has_errors {
                summary.failed_files += 1;
            } else {
                summary.passed_files += 1;
            }

            if self.verbose {
                let status_badge = if has_errors { "[FAIL]" } else { "[PASS]" };
                let rel_path = file_path
                    .strip_prefix(&self.repo_root)
                    .unwrap_or(file_path);
                println!("{} {}", status_badge, rel_path.display());
            }

            summary.issues.extend(file_issues);
        }

        summary.duration_ms = start.elapsed().as_millis() as u64;
        Ok(summary)
    }

    /// Audits an individual C11 header file.
    pub fn validate_header(&self, path: &Path, issues: &mut Vec<ValidationIssue>) {
        let rel_to_include = match path.strip_prefix(self.repo_root.join("engine/include")) {
            Ok(p) => p.to_string_lossy().to_string(),
            Err(_) => path.to_string_lossy().to_string(),
        };

        let record = self.registry.headers.get(&rel_to_include);
        if record.is_none() {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "REGISTRY".to_string(),
                field: "registration".to_string(),
                severity: IssueSeverity::Warning,
                message: format!("Header '{}' is not registered in subsystems.json", rel_to_include),
            });
        }

        let archetype = record.map(|r| r.archetype).unwrap_or(HeaderArchetype::Operational);
        let stateful = record.map(|r| r.stateful).unwrap_or(false);

        let content = match fs::read_to_string(path) {
            Ok(c) => c,
            Err(e) => {
                issues.push(ValidationIssue {
                    file: path.to_path_buf(),
                    block: "I/O".to_string(),
                    field: "read".to_string(),
                    severity: IssueSeverity::Error,
                    message: format!("Failed to read header file: {}", e),
                });
                return;
            }
        };

        // 1. Invariant 18: Strict ISO C comments only (zero // comments)
        self.validate_invariant_18(path, &content, issues);

        // 2. Block 1: Metadata Block (baseline + strict template & paired .c parity)
        self.validate_block_1_metadata(path, &content, issues);
        let run_strict_pairing =
            self.is_strict_domain(&rel_to_include) && self.rules.enforce_template_alignment;
        let header_b1 = if run_strict_pairing {
            let expected_h_rel = format!("engine/include/{}", rel_to_include);
            self.validate_strict_block_1_file(
                path,
                &expected_h_rel,
                record,
                false,
                &content,
                issues,
            )
        } else {
            None
        };

        // 3. Block 2: Opening Block & Include Guards
        self.validate_block_2_opening(path, &rel_to_include, &content, archetype, issues);

        // 4. Block 3: Body Block
        let header_b3 =
            self.validate_block_3_body(path, &rel_to_include, &content, archetype, issues);

        // 4b. Paired .c Source Verification (Blocks 1, 2, and 3 parity)
        if run_strict_pairing && self.rules.validate_paired_c_sources {
            self.validate_paired_source(
                path,
                &rel_to_include,
                record,
                header_b1.as_ref(),
                header_b3.as_ref(),
                issues,
            );
        }

        // 5. Block 4: Audit Block & Verification Cogs
        self.validate_block_4_audit(path, &content, archetype, issues);

        // 6. Block 5: Closing Block, Dump Cards & Lifecycle
        self.validate_block_5_closing(path, &rel_to_include, &content, archetype, stateful, issues);
    }

    fn validate_invariant_18(&self, path: &Path, content: &str, issues: &mut Vec<ValidationIssue>) {
        for (line_num, line) in content.lines().enumerate() {
            if let Some(pos) = line.find("//") {
                // Check if the double-slash is preceded by "http:", "https:", or "rom:"
                let before = &line[..pos];
                if before.ends_with("http:") || before.ends_with("https:") || before.ends_with("rom:") {
                    continue;
                }

                // Check whitelisted URLs
                let is_whitelisted = self.rules.url_whitelists.iter().any(|u| line.contains(u));
                if !is_whitelisted {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "INVARIANT_18".to_string(),
                        field: format!("line_{}", line_num + 1),
                        severity: IssueSeverity::Error,
                        message: format!("Forbidden '//' comment found at line {}: {}", line_num + 1, line.trim()),
                    });
                }
            }
        }
    }

    fn validate_block_1_metadata(&self, path: &Path, content: &str, issues: &mut Vec<ValidationIssue>) {
        if !content.contains("BLOCK 1: METADATA BLOCK") {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_1_metadata".to_string(),
                field: "header".to_string(),
                severity: IssueSeverity::Error,
                message: "Missing 'BLOCK 1: METADATA BLOCK' marker".to_string(),
            });
            return;
        }

        // Check mandatory tags
        for tag in &self.rules.mandatory_metadata_tags {
            if !content.contains(tag) {
                issues.push(ValidationIssue {
                    file: path.to_path_buf(),
                    block: "block_1_metadata".to_string(),
                    field: tag.clone(),
                    severity: IssueSeverity::Error,
                    message: format!("Missing required metadata tag '{}'", tag),
                });
            }
        }

        // Check mandatory sections
        for sec in &self.rules.mandatory_metadata_sections {
            if !content.contains(sec) {
                issues.push(ValidationIssue {
                    file: path.to_path_buf(),
                    block: "block_1_metadata".to_string(),
                    field: sec.clone(),
                    severity: IssueSeverity::Error,
                    message: format!("Missing required metadata section '{}'", sec),
                });
            }
        }

        // Check project canonical anchor
        if !content.contains(&self.rules.canonical_project) {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_1_metadata".to_string(),
                field: "@project".to_string(),
                severity: IssueSeverity::Error,
                message: format!("Missing canonical project '{}'", self.rules.canonical_project),
            });
        }

        // Check canonical author
        if !content.contains(&self.rules.canonical_author) {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_1_metadata".to_string(),
                field: "@author".to_string(),
                severity: IssueSeverity::Error,
                message: format!("Missing canonical author '{}'", self.rules.canonical_author),
            });
        }

        // Check canonical repository
        if !content.contains(&self.rules.canonical_repository) {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_1_metadata".to_string(),
                field: "@repository".to_string(),
                severity: IssueSeverity::Error,
                message: format!("Missing canonical repository '{}'", self.rules.canonical_repository),
            });
        }
    }

    fn is_strict_domain(&self, rel_to_include: &str) -> bool {
        self.rules
            .strict_template_domains
            .iter()
            .any(|dom| rel_to_include.starts_with(&format!("{}/", dom)))
    }

    fn is_strict_domain_in(domains: &[String], rel_to_include: &str) -> bool {
        domains
            .iter()
            .any(|dom| rel_to_include.starts_with(&format!("{}/", dom)))
    }

    fn is_strict_b2_domain(&self, rel_to_include: &str) -> bool {
        Self::is_strict_domain_in(&self.rules.strict_block_2_domains, rel_to_include)
    }

    fn is_strict_b3_domain(&self, rel_to_include: &str) -> bool {
        Self::is_strict_domain_in(&self.rules.strict_block_3_domains, rel_to_include)
    }

    fn expected_cornerstone_guard(&self, rel_to_include: &str) -> String {
        let sanitized: String = rel_to_include
            .chars()
            .map(|ch| {
                if ch.is_ascii_alphanumeric() {
                    ch.to_ascii_uppercase()
                } else {
                    '_'
                }
            })
            .collect();
        format!("{}{}", self.rules.strict_include_guard_prefix, sanitized)
    }

    fn push_block_issue(
        issues: &mut Vec<ValidationIssue>,
        path: &Path,
        block: &str,
        field: &str,
        severity: IssueSeverity,
        message: String,
    ) {
        issues.push(ValidationIssue {
            file: path.to_path_buf(),
            block: block.to_string(),
            field: field.to_string(),
            severity,
            message,
        });
    }

    fn push_b2_issue(
        issues: &mut Vec<ValidationIssue>,
        path: &Path,
        field: &str,
        severity: IssueSeverity,
        message: String,
    ) {
        Self::push_block_issue(issues, path, "block_2_opening", field, severity, message);
    }

    fn push_b3_issue(
        issues: &mut Vec<ValidationIssue>,
        path: &Path,
        field: &str,
        severity: IssueSeverity,
        message: String,
    ) {
        Self::push_block_issue(issues, path, "block_3_body", field, severity, message);
    }

    fn validate_anti_domain_bleed(
        &self,
        path: &Path,
        content: &str,
        kind: &str,
        issues: &mut Vec<ValidationIssue>,
    ) {
        for forbidden in &self.rules.forbidden_domain_prefixes {
            let pattern = format!("#include \"{}", forbidden);
            if content.contains(&pattern) {
                Self::push_b2_issue(
                    issues,
                    path,
                    "anti_domain_bleed",
                    IssueSeverity::Error,
                    format!(
                        "Domain bleed detected: engine {} includes game domain path '{}'",
                        kind, forbidden
                    ),
                );
            }
        }
    }

    fn validate_cornerstone_banner_fields(
        &self,
        path: &Path,
        block: &str,
        block_label: &str,
        banner_slice: &str,
        required_fields: &[String],
        issues: &mut Vec<ValidationIssue>,
    ) {
        if !self.rules.engine_name.is_empty() && !banner_slice.contains(&self.rules.engine_name) {
            Self::push_block_issue(
                issues,
                path,
                block,
                "engine_name",
                IssueSeverity::Error,
                format!(
                    "{} banner missing canonical engine name '{}'",
                    block_label, self.rules.engine_name
                ),
            );
        }
        for label in required_fields {
            if !banner_slice.contains(label.as_str()) {
                Self::push_block_issue(
                    issues,
                    path,
                    block,
                    "banner",
                    IssueSeverity::Error,
                    format!(
                        "{} Cornerstone banner missing required field '{}'",
                        block_label, label
                    ),
                );
            }
        }
    }

    fn validate_cornerstone_b2_banner(
        &self,
        path: &Path,
        b2_slice: &str,
        issues: &mut Vec<ValidationIssue>,
    ) {
        let fields = [
            "Stratum:".to_string(),
            "Module:".to_string(),
            "Mission:".to_string(),
        ];
        self.validate_cornerstone_banner_fields(
            path,
            "block_2_opening",
            "Block 2",
            b2_slice,
            &fields,
            issues,
        );
    }

    fn validate_cornerstone_b3_banner(
        &self,
        path: &Path,
        b3_slice: &str,
        archetype: HeaderArchetype,
        is_source: bool,
        issues: &mut Vec<ValidationIssue>,
    ) {
        let banner_end = b3_slice.find("*/").unwrap_or_else(|| b3_slice.len().min(600));
        let banner_slice = &b3_slice[..banner_end];

        self.validate_cornerstone_banner_fields(
            path,
            "block_3_body",
            "Block 3",
            banner_slice,
            &self.rules.block_3_required_banner_fields,
            issues,
        );

        let has_subtitle = banner_slice
            .lines()
            .take(6)
            .map(str::trim)
            .any(|l| l.starts_with('(') && l.ends_with(')'));
        if !has_subtitle {
            Self::push_b3_issue(
                issues,
                path,
                "b3_subtitle",
                IssueSeverity::Error,
                "Block 3 Cornerstone banner missing parenthetical '( ... )' archetype subtitle"
                    .to_string(),
            );
        }

        let subtitle_key = match (archetype, is_source) {
            (HeaderArchetype::Operational, false) => "operational_header",
            (HeaderArchetype::Operational, true) => "operational_source",
            (HeaderArchetype::QuarantinedFallback, false) => "quarantined_fallback_header",
            (HeaderArchetype::QuarantinedFallback, true) => "quarantined_fallback_source",
            (HeaderArchetype::FacadeUmbrella, false) => "facade_umbrella_header",
            (HeaderArchetype::PureTypes, false) => "pure_types_header",
            _ if is_source => "universal_fallback_source",
            _ => "universal_fallback_header",
        };
        if let Some(expected_sub) = self.rules.archetype_b3_subtitles.get(subtitle_key) {
            if !banner_slice.contains(expected_sub) {
                Self::push_b3_issue(
                    issues,
                    path,
                    "b3_archetype_subtitle",
                    IssueSeverity::Warning,
                    format!(
                        "Block 3 subtitle differs from archetype template '{}' ('{}')",
                        subtitle_key, expected_sub
                    ),
                );
            }
        }
    }

    fn validate_tier_dividers(
        &self,
        path: &Path,
        b2_slice: &str,
        issues: &mut Vec<ValidationIssue>,
    ) -> Option<usize> {
        let t1 = b2_slice.find("Tier 1:");
        let t2 = b2_slice.find("Tier 2:");
        let t3 = b2_slice.find("Tier 3:");

        for (label, pos) in [("Tier 1:", t1), ("Tier 2:", t2), ("Tier 3:", t3)] {
            if pos.is_none() {
                Self::push_b2_issue(
                    issues,
                    path,
                    "tier_headers",
                    IssueSeverity::Error,
                    format!("Block 2 missing include divider '{}'", label),
                );
            }
        }
        if let (Some(p1), Some(p2), Some(p3)) = (t1, t2, t3) {
            if !(p1 < p2 && p2 < p3) {
                Self::push_b2_issue(
                    issues,
                    path,
                    "tier_order",
                    IssueSeverity::Error,
                    "Block 2 include dividers must appear in monotonic Tier 1 -> Tier 2 -> Tier 3 order".to_string(),
                );
            }
        }
        t3
    }

    fn validate_strict_b2_sub_assemblies(
        &self,
        path: &Path,
        content: &str,
        b2_start: usize,
        b2_end: usize,
        is_source: bool,
        issues: &mut Vec<ValidationIssue>,
    ) {
        let b2_slice = &content[b2_start..b2_end];
        let p21 = find_sub_assembly_pos(b2_slice, "2.1");
        let p22 = find_sub_assembly_pos(b2_slice, "2.2");
        let p23 = find_sub_assembly_pos(b2_slice, "2.3");
        let p24 = find_sub_assembly_pos(b2_slice, "2.4");
        let p25 = find_sub_assembly_pos(b2_slice, "2.5");

        if !is_source && self.rules.forbid_2_5_in_headers && find_sub_assembly_pos(content, "2.5").is_some() {
            Self::push_b2_issue(
                issues,
                path,
                "sub_assembly_2_5",
                IssueSeverity::Error,
                "Sub-Assembly 2.5 is forbidden in .h headers; move internal non-inline helpers to the paired .c source".to_string(),
            );
        }

        for (num, pos) in [("2.1", p21), ("2.2", p22), ("2.3", p23), ("2.4", p24)] {
            if pos.is_none() {
                Self::push_b2_issue(
                    issues,
                    path,
                    "sub_assembly",
                    IssueSeverity::Error,
                    format!("Missing required 'Sub-Assembly {}' section in Block 2", num),
                );
            }
        }

        if self.rules.verify_monotonic_sub_assemblies {
            if let (Some(a), Some(b), Some(c), Some(d)) = (p21, p22, p23, p24) {
                if !(a < b && b < c && c < d) {
                    Self::push_b2_issue(
                        issues,
                        path,
                        "sub_assembly_order",
                        IssueSeverity::Error,
                        "Sub-Assemblies 2.1..2.4 must appear in strictly increasing order".to_string(),
                    );
                }
                if is_source {
                    if let Some(e) = p25 {
                        if e <= d {
                            Self::push_b2_issue(
                                issues,
                                path,
                                "sub_assembly_order",
                                IssueSeverity::Error,
                                "Sub-Assembly 2.5 must appear after Sub-Assembly 2.4 and before BLOCK 3".to_string(),
                            );
                        }
                    }
                }
            }
        }

        if !is_source {
            return;
        }

        /* Strip full-file comments/strings/pp so multi-line comment openings above markers stay neutralized. */
        let cleaned_full = strip_comments_strings_and_pp(content);

        if let Some(start24) = p24 {
            if self.rules.warn_non_inline_in_2_4 {
                let end24 = p25.filter(|&p| p > start24).unwrap_or(b2_slice.len());
                let cleaned24 = &cleaned_full[b2_start + start24..b2_start + end24];
                for item in extract_top_level_c_items(cleaned24) {
                    if let TopLevelCItem::FuncDef(hdr) = item {
                        if !has_c_word(&hdr, "inline") {
                            Self::push_b2_issue(
                                issues,
                                path,
                                "sub_assembly_2_4",
                                IssueSeverity::Warning,
                                format!(
                                    "Non-inline function definition '{}' in Sub-Assembly 2.4; promote non-inline helpers to monitored Sub-Assembly 2.5",
                                    hdr.lines().next().unwrap_or(&hdr).trim()
                                ),
                            );
                        }
                    }
                }
            }
        }

        if let Some(start25) = p25 {
            let s25 = &b2_slice[start25..];
            if self.rules.require_2_5_pattern_watch {
                let banner_end = s25.find("*/").map(|i| i + 2).unwrap_or(s25.len());
                let banner = &s25[..banner_end];
                for label in ["Support Role:", "Pattern Watch:", "Linkage Rule:"] {
                    if !banner.contains(label) {
                        Self::push_b2_issue(
                            issues,
                            path,
                            "sub_assembly_2_5_banner",
                            IssueSeverity::Error,
                            format!(
                                "Monitored Sub-Assembly 2.5 banner missing required discipline field '{}'",
                                label
                            ),
                        );
                    }
                }
            }

            let cleaned25 = &cleaned_full[b2_start + start25..b2_end];
            for item in extract_top_level_c_items(cleaned25) {
                match item {
                    TopLevelCItem::FuncDef(hdr) => {
                        if !has_c_word(&hdr, "static") {
                            Self::push_b2_issue(
                                issues,
                                path,
                                "sub_assembly_2_5_linkage",
                                IssueSeverity::Error,
                                format!(
                                    "Non-static function '{}' in Sub-Assembly 2.5 violates internal static linkage rule",
                                    hdr.lines().next().unwrap_or(&hdr).trim()
                                ),
                            );
                        }
                    }
                    TopLevelCItem::OtherDecl(decl) => {
                        let is_forward_proto = decl.contains('(') && !decl.contains('=');
                        if !is_forward_proto {
                            Self::push_b2_issue(
                                issues,
                                path,
                                "sub_assembly_2_5_storage",
                                IssueSeverity::Error,
                                format!(
                                    "Module-level storage declaration '{}' is forbidden in Sub-Assembly 2.5; move arrays/state to Sub-Assembly 2.3",
                                    decl.lines().next().unwrap_or(&decl).trim()
                                ),
                            );
                        }
                    }
                }
            }
        }
    }

    fn validate_source_block_2_opening(
        &self,
        c_path: &Path,
        content: &str,
        issues: &mut Vec<ValidationIssue>,
    ) {
        self.validate_anti_domain_bleed(c_path, content, "source", issues);

        let b2_pos = match content.find("BLOCK 2: OPENING BLOCK") {
            Some(p) => p,
            None => {
                Self::push_b2_issue(
                    issues,
                    c_path,
                    "header",
                    IssueSeverity::Error,
                    "Missing 'BLOCK 2: OPENING BLOCK' marker in paired .c source".to_string(),
                );
                return;
            }
        };

        if self.rules.verify_posix_macro_placement {
            let mut offset = 0usize;
            for raw_line in content.split_inclusive('\n') {
                let trimmed = raw_line.trim_start();
                if let Some(after_hash) = trimmed.strip_prefix('#') {
                    let after_hash = after_hash.trim_start();
                    if let Some(after_def) = after_hash.strip_prefix("define") {
                        if after_def
                            .chars()
                            .next()
                            .is_some_and(|c| c.is_ascii_whitespace())
                            && after_def.trim_start().starts_with("_POSIX_C_SOURCE")
                            && offset >= b2_pos
                        {
                            Self::push_b2_issue(
                                issues,
                                c_path,
                                "posix_macro_placement",
                                IssueSeverity::Error,
                                "#define _POSIX_C_SOURCE must appear strictly before BLOCK 2: OPENING BLOCK".to_string(),
                            );
                        }
                    }
                }
                offset += raw_line.len();
            }
        }

        let b3_pos = content.find("BLOCK 3: BODY BLOCK");
        let b2_end = match b3_pos {
            Some(p) if p > b2_pos => p,
            _ => {
                Self::push_b2_issue(
                    issues,
                    c_path,
                    "block_order",
                    IssueSeverity::Error,
                    "Paired .c source missing 'BLOCK 3: BODY BLOCK' after BLOCK 2: OPENING BLOCK".to_string(),
                );
                content.len()
            }
        };
        let b2_slice = &content[b2_pos..b2_end];

        self.validate_cornerstone_b2_banner(c_path, b2_slice, issues);
        if self.rules.require_tier_headers {
            self.validate_tier_dividers(c_path, b2_slice, issues);
        }
        self.validate_strict_b2_sub_assemblies(c_path, content, b2_pos, b2_end, true, issues);
    }

    fn push_b1_err(
        issues: &mut Vec<ValidationIssue>,
        path: &Path,
        field: &str,
        message: String,
    ) {
        issues.push(ValidationIssue {
            file: path.to_path_buf(),
            block: "block_1_metadata".to_string(),
            field: field.to_string(),
            severity: IssueSeverity::Error,
            message,
        });
    }

    fn validate_strict_block_1_file(
        &self,
        path: &Path,
        expected_rel_path: &str,
        record: Option<&SubsystemRecord>,
        is_source: bool,
        content: &str,
        issues: &mut Vec<ValidationIssue>,
    ) -> Option<ParsedBlock1> {
        let parsed = match parse_block_1_comment(content, self.rules.tag_column_width) {
            Ok(p) => p,
            Err(msg) => {
                Self::push_b1_err(issues, path, "template", msg);
                return None;
            }
        };

        let tpl = if is_source && !self.rules.source_template_lines.is_empty() {
            &self.rules.source_template_lines
        } else {
            &self.rules.header_template_lines
        };

        // 1. Validate banner lines against c_block_1_template from engine/config/standards/blocks/block_1_metadata.json
        let expected_banner = [
            "/*".to_string(),
            self.rules.banner_rule.clone(),
            self.rules.block_1_title.clone(),
            self.rules.block_1_subtitle.clone(),
            self.rules.banner_rule.clone(),
        ];
        for idx in 0..5 {
            let expected_line = tpl.get(idx).unwrap_or(&expected_banner[idx]);
            if parsed.lines.get(idx) != Some(expected_line) {
                Self::push_b1_err(
                    issues,
                    path,
                    "template_banner",
                    format!(
                        "Block 1 line {} does not match c_block_1_template ('{}')",
                        idx + 1,
                        expected_line
                    ),
                );
            }
        }
        let n = parsed.lines.len();
        let close_rule = tpl
            .len()
            .checked_sub(2)
            .and_then(|i| tpl.get(i))
            .unwrap_or(&self.rules.banner_rule);
        if n < 7 || &parsed.lines[n - 2] != close_rule || parsed.lines[n - 1] != "*/" {
            Self::push_b1_err(
                issues,
                path,
                "template_footer",
                "Block 1 closing banner must match 80-'=' rule followed by '*/'".to_string(),
            );
        }

        // 2. Validate ordered 17 tags and column-width alignment
        if self.rules.enforce_tag_order {
            let actual_tags: Vec<&str> = parsed.tags.iter().map(|(k, _, _)| k.as_str()).collect();
            let expected_tags: Vec<&str> = self
                .rules
                .ordered_metadata_tags
                .iter()
                .map(|s| s.as_str())
                .collect();
            if actual_tags != expected_tags {
                Self::push_b1_err(
                    issues,
                    path,
                    "ordered_tags",
                    format!(
                        "Block 1 tags must match exact canonical order {:?}, found {:?}",
                        expected_tags, actual_tags
                    ),
                );
            }

            for (tag, val, aligned) in &parsed.tags {
                if !*aligned {
                    Self::push_b1_err(
                        issues,
                        path,
                        tag,
                        format!(
                            "Tag '{}' must align value at column {} (1-indexed column {})",
                            tag,
                            self.rules.tag_column_width,
                            self.rules.tag_column_width + 1
                        ),
                    );
                }
                if val.trim().is_empty() {
                    Self::push_b1_err(
                        issues,
                        path,
                        tag,
                        format!("Tag '{}' value must not be empty", tag),
                    );
                }
            }
        }

        // 3. Movement-resilient @file & @id contracts
        if let Some(file_val) = parsed.tag_value("@file") {
            if file_val != expected_rel_path {
                Self::push_b1_err(
                    issues,
                    path,
                    "@file",
                    format!(
                        "@file '{}' does not match current repo path '{}'",
                        file_val, expected_rel_path
                    ),
                );
            }
        }

        if let Some(id_val) = parsed.tag_value("@id") {
            if !is_valid_cog_id(id_val) {
                Self::push_b1_err(
                    issues,
                    path,
                    "@id",
                    format!(
                        "@id '{}' must follow movement-resilient format 'cog.<module>[.<variant>]' without baked directory paths",
                        id_val
                    ),
                );
            }
            if let Some(rec) = record {
                if let Some(expected_id) = rec.id.as_deref() {
                    if id_val != expected_id {
                        Self::push_b1_err(
                            issues,
                            path,
                            "@id",
                            format!(
                                "@id '{}' does not match subsystems.json id '{}'",
                                id_val, expected_id
                            ),
                        );
                    }
                }
            }
        }

        // 4. Growth tags (@archetype, @maturity, @chassis, @scale, @seams, @invariants)
        if let (Some(arch_val), Some(rec)) = (parsed.tag_value("@archetype"), record) {
            let expected_prefix = archetype_prefix(rec.archetype, rec.stateful);
            if arch_val != expected_prefix {
                Self::push_b1_err(
                    issues,
                    path,
                    "@archetype",
                    format!(
                        "@archetype '{}' must match subsystems.json '{}'",
                        arch_val, expected_prefix
                    ),
                );
            }
        }

        if let Some(mat_val) = parsed.tag_value("@maturity") {
            let valid_mat = self.rules.maturity_taxonomy.iter().any(|stage| {
                mat_val
                    .strip_prefix(stage)
                    .map(|rest| {
                        let r = rest.trim_start();
                        r.starts_with('|') && !r[1..].trim().is_empty()
                    })
                    .unwrap_or(false)
            });
            if !valid_mat {
                Self::push_b1_err(
                    issues,
                    path,
                    "@maturity",
                    format!(
                        "@maturity '{}' must begin with one of {:?} followed by ' | <continuity detail>'",
                        mat_val, self.rules.maturity_taxonomy
                    ),
                );
            }
        }

        if let Some(chassis_val) = parsed.tag_value("@chassis") {
            if !chassis_val.starts_with("zero-heap |") || chassis_val["zero-heap |".len()..].trim().is_empty() {
                Self::push_b1_err(
                    issues,
                    path,
                    "@chassis",
                    "@chassis in Layer 0 must begin with 'zero-heap | <measured footprint/bounds>'".to_string(),
                );
            }
        }

        if let Some(inv_val) = parsed.tag_value("@invariants") {
            if !inv_val.starts_with("1. Stratum Baseline:") {
                Self::push_b1_err(
                    issues,
                    path,
                    "@invariants",
                    "@invariants item 1 must use '1. Stratum Baseline: ...' instead of duplicated stratum boilerplate".to_string(),
                );
            }
            if !inv_val.contains("\n2.") && !inv_val.contains("2.") {
                Self::push_b1_err(
                    issues,
                    path,
                    "@invariants",
                    "@invariants must include at least one file-specific invariant ('2. ...')".to_string(),
                );
            }
        }

        // 5. Dual-audience sections in exact template order and indentation
        if !parsed.sections_in_order || !parsed.sections_indented {
            Self::push_b1_err(
                issues,
                path,
                "dual_audience_sections",
                "PURPOSE & ARCHITECTURAL ROLE must contain '  ### For Game Designers & Non-Programmers:' followed by '  ### For Systems Programmers & Engine Architects:'".to_string(),
            );
        }
        if parsed.designer_text.is_empty() || parsed.programmer_text.is_empty() {
            Self::push_b1_err(
                issues,
                path,
                "dual_audience_sections",
                "Both Dual-Audience overview sections must contain non-empty documentation".to_string(),
            );
        }

        Some(parsed)
    }

    fn validate_paired_source(
        &self,
        _header_path: &Path,
        rel_to_include: &str,
        record: Option<&SubsystemRecord>,
        header_b1: Option<&ParsedBlock1>,
        header_b3: Option<&ParsedBlock3>,
        issues: &mut Vec<ValidationIssue>,
    ) {
        let Some(stem_rel) = rel_to_include.strip_suffix(".h") else {
            return;
        };
        let rel_c = format!("{}.c", stem_rel);
        let expected_c_rel = format!("engine/src/{}", rel_c);
        let c_path = self.repo_root.join(&expected_c_rel);
        let archetype = record
            .map(|r| r.archetype)
            .unwrap_or(HeaderArchetype::Operational);

        if !c_path.exists() {
            if matches!(
                archetype,
                HeaderArchetype::FacadeUmbrella | HeaderArchetype::PureTypes
            ) {
                return;
            }
            Self::push_b1_err(
                issues,
                &c_path,
                "paired_source",
                format!("Missing paired C source file '{}'", expected_c_rel),
            );
            return;
        }

        let content = match fs::read_to_string(&c_path) {
            Ok(c) => c,
            Err(e) => {
                Self::push_b1_err(
                    issues,
                    &c_path,
                    "read",
                    format!("Failed to read paired C source: {}", e),
                );
                return;
            }
        };

        let before_err_count = issues
            .iter()
            .filter(|i| i.severity == IssueSeverity::Error)
            .count();

        self.validate_invariant_18(&c_path, &content, issues);
        self.validate_block_1_metadata(&c_path, &content, issues);
        let src_b1 = self.validate_strict_block_1_file(
            &c_path,
            &expected_c_rel,
            record,
            true,
            &content,
            issues,
        );

        if let (Some(hb), Some(sb)) = (header_b1, src_b1.as_ref()) {
            for shared_tag in [
                "@id",
                "@layer",
                "@archetype",
                "@version",
                "@interface_rev",
                "@date",
                "@system",
            ] {
                if hb.tag_value(shared_tag) != sb.tag_value(shared_tag) {
                    Self::push_b1_err(
                        issues,
                        &c_path,
                        shared_tag,
                        format!(
                            "Header/Source parity mismatch on '{}': header has {:?}, source has {:?}",
                            shared_tag,
                            hb.tag_value(shared_tag),
                            sb.tag_value(shared_tag)
                        ),
                    );
                }
            }
            if hb.designer_text == sb.designer_text && !hb.designer_text.is_empty() {
                Self::push_b1_err(
                    issues,
                    &c_path,
                    "designer_prose_anti_duplication",
                    "Source Designer section must complement the header rather than copying it verbatim".to_string(),
                );
            }
            if hb.programmer_text == sb.programmer_text && !hb.programmer_text.is_empty() {
                Self::push_b1_err(
                    issues,
                    &c_path,
                    "programmer_prose_anti_duplication",
                    "Source Programmer section must detail runtime chassis/probes rather than copying header prose verbatim".to_string(),
                );
            }
        }

        if self.is_strict_b2_domain(rel_to_include) {
            self.validate_source_block_2_opening(&c_path, &content, issues);
        }

        if self.is_strict_b3_domain(rel_to_include) {
            let src_b3 = self.validate_b3_structure(
                &c_path,
                rel_to_include,
                &content,
                archetype,
                true,
                issues,
            );
            if let (Some(hb), Some(sb)) = (header_b3, src_b3.as_ref()) {
                self.validate_b3_pair_parity(&c_path, hb, sb, issues);
            }
        }

        let after_err_count = issues
            .iter()
            .filter(|i| i.severity == IssueSeverity::Error)
            .count();
        if self.verbose {
            let badge = if after_err_count > before_err_count {
                "[FAIL]"
            } else {
                "[PASS]"
            };
            println!("{} {}", badge, expected_c_rel);
        }
    }

    fn validate_block_2_opening(
        &self,
        path: &Path,
        rel_to_include: &str,
        content: &str,
        archetype: HeaderArchetype,
        issues: &mut Vec<ValidationIssue>,
    ) {
        let strict_b2 = self.is_strict_b2_domain(rel_to_include);
        let b2_pos = content.find("BLOCK 2: OPENING BLOCK");
        if b2_pos.is_none() {
            Self::push_b2_issue(
                issues,
                path,
                "header",
                IssueSeverity::Error,
                "Missing 'BLOCK 2: OPENING BLOCK' marker".to_string(),
            );
        }

        // Include guard checks (Cornerstone dir-qualified strict guard vs legacy Tenebris guard)
        if strict_b2 {
            let expected_guard = self.expected_cornerstone_guard(rel_to_include);
            let expected_ifndef = format!("#ifndef {}", expected_guard);
            let expected_define = format!("#define {}", expected_guard);
            let has_ifndef = content.lines().any(|l| l.trim() == expected_ifndef);
            let has_define = content.lines().any(|l| l.trim() == expected_define);
            if !has_ifndef || !has_define {
                Self::push_b2_issue(
                    issues,
                    path,
                    "include_guard",
                    IssueSeverity::Error,
                    format!(
                        "Expected Cornerstone dir-qualified include guard '{}' (#ifndef / #define)",
                        expected_guard
                    ),
                );
            }
        } else {
            let guard_prefix = &self.rules.include_guard_prefix;
            let has_ifndef = content
                .lines()
                .any(|l| l.starts_with("#ifndef") && l.contains(guard_prefix));
            let has_define = content
                .lines()
                .any(|l| l.starts_with("#define") && l.contains(guard_prefix));

            if !has_ifndef || !has_define {
                Self::push_b2_issue(
                    issues,
                    path,
                    "include_guard",
                    IssueSeverity::Error,
                    format!("Missing or invalid include guard with prefix '{}'", guard_prefix),
                );
            }
        }

        // Anti-Domain-Bleed Covenant
        self.validate_anti_domain_bleed(path, content, "header", issues);

        if strict_b2 {
            let Some(b2_start) = b2_pos else {
                return;
            };
            let b3_pos = content.find("BLOCK 3: BODY BLOCK");
            let b2_end = match b3_pos {
                Some(p) if p > b2_start => p,
                _ => {
                    Self::push_b2_issue(
                        issues,
                        path,
                        "block_order",
                        IssueSeverity::Error,
                        "Header missing 'BLOCK 3: BODY BLOCK' after BLOCK 2: OPENING BLOCK".to_string(),
                    );
                    content.len()
                }
            };
            let b2_slice = &content[b2_start..b2_end];

            self.validate_cornerstone_b2_banner(path, b2_slice, issues);

            let t3_pos = if self.rules.require_tier_headers {
                self.validate_tier_dividers(path, b2_slice, issues)
            } else {
                b2_slice.find("Tier 3:")
            };

            if self.rules.require_extern_c_in_headers {
                let search_from = t3_pos.unwrap_or(0);
                let tail = &b2_slice[search_from..];
                let lines: Vec<&str> = tail
                    .lines()
                    .map(str::trim)
                    .filter(|l| !l.is_empty())
                    .collect();
                let has_extern_c_open = lines.windows(3).any(|w| {
                    w == ["#ifdef __cplusplus", "extern \"C\" {", "#endif"]
                });
                if !has_extern_c_open {
                    Self::push_b2_issue(
                        issues,
                        path,
                        "extern_c_open",
                        IssueSeverity::Error,
                        "Strict Block 2 header missing '#ifdef __cplusplus / extern \"C\" { / #endif' after Tier 3 includes".to_string(),
                    );
                }
            }

            if self.rules.enforce_quarantined_fallback_headers
                && archetype == HeaderArchetype::QuarantinedFallback
            {
                for raw_line in content.lines() {
                    if let Some(inc_path) = extract_quoted_include(raw_line.trim_start()) {
                        let allowed = inc_path == "core/manifest.h"
                            || inc_path.ends_with("_types.h")
                            || inc_path.ends_with("_fallback.h");
                        if !allowed {
                            Self::push_b2_issue(
                                issues,
                                path,
                                "quarantined_fallback_isolation",
                                IssueSeverity::Error,
                                format!(
                                    "QuarantinedFallback header cannot include non-isolated local header '{}'",
                                    inc_path
                                ),
                            );
                        }
                    }
                }
            }

            self.validate_strict_b2_sub_assemblies(path, content, b2_start, b2_end, false, issues);
            return;
        }

        // Archetype Sub-Assemblies (legacy baseline headers)
        if archetype == HeaderArchetype::Operational || archetype == HeaderArchetype::QuarantinedFallback {
            if !content.contains("Sub-Assembly 2.1") {
                issues.push(ValidationIssue {
                    file: path.to_path_buf(),
                    block: "block_2_opening".to_string(),
                    field: "Sub-Assembly 2.1".to_string(),
                    severity: IssueSeverity::Warning,
                    message: "Missing 'Sub-Assembly 2.1' header in opening block".to_string(),
                });
            }
            if !content.contains("Sub-Assembly 2.2") {
                issues.push(ValidationIssue {
                    file: path.to_path_buf(),
                    block: "block_2_opening".to_string(),
                    field: "Sub-Assembly 2.2".to_string(),
                    severity: IssueSeverity::Warning,
                    message: "Missing 'Sub-Assembly 2.2' header in opening block".to_string(),
                });
            }
            if !content.contains("Sub-Assembly 2.3") {
                issues.push(ValidationIssue {
                    file: path.to_path_buf(),
                    block: "block_2_opening".to_string(),
                    field: "Sub-Assembly 2.3".to_string(),
                    severity: IssueSeverity::Warning,
                    message: "Missing 'Sub-Assembly 2.3' header in opening block".to_string(),
                });
            }
            if !content.contains("Sub-Assembly 2.4") {
                issues.push(ValidationIssue {
                    file: path.to_path_buf(),
                    block: "block_2_opening".to_string(),
                    field: "Sub-Assembly 2.4".to_string(),
                    severity: IssueSeverity::Warning,
                    message: "Missing 'Sub-Assembly 2.4' header in opening block".to_string(),
                });
            }
        }
    }

    fn find_next_block_marker(content: &str, after_pos: usize) -> usize {
        let mut rel_off = 0usize;
        for raw_line in content[after_pos..].split_inclusive('\n') {
            let stripped = strip_comment_decor_prefix(raw_line);
            if stripped.starts_with("BLOCK 4:") || stripped.starts_with("BLOCK 5:") {
                return after_pos + rel_off;
            }
            rel_off += raw_line.len();
        }
        content.len()
    }

    fn is_forbidden_b3_fn(&self, fn_name: &str) -> bool {
        fn_name.ends_with(&self.rules.audit_pattern)
            || fn_name.contains("_audit_")
            || fn_name.ends_with(&self.rules.preflight_pattern)
            || fn_name.contains("_preflight_")
            || fn_name.ends_with(&self.rules.dump_card_suffix)
            || fn_name == "debug_dump_state"
            || fn_name == "debug_dump_snapshot_history"
            || fn_name.ends_with(&self.rules.lifecycle_boot_suffix)
            || fn_name.ends_with("_init")
            || fn_name.ends_with("_init_default")
            || fn_name.ends_with("_shutdown")
            || fn_name.ends_with("_teardown")
    }

    fn check_b3_macros(&self, path: &Path, b3_slice: &str, issues: &mut Vec<ValidationIssue>) {
        if !self.rules.ban_macros_in_block_3 {
            return;
        }
        let stripped = strip_comments_and_strings(b3_slice);
        for line in stripped.lines() {
            let trimmed = line.trim_start();
            if let Some(after_hash) = trimmed.strip_prefix('#') {
                let kw = after_hash.trim_start();
                if let Some(after_def) = kw.strip_prefix("define") {
                    if after_def.is_empty()
                        || after_def
                            .chars()
                            .next()
                            .is_some_and(|c| c.is_ascii_whitespace())
                    {
                        Self::push_b3_issue(
                            issues,
                            path,
                            "macros_in_block_3",
                            IssueSeverity::Error,
                            format!(
                                "Preprocessor macro '{}' is forbidden in Block 3; relocate to Block 2 (2.1/2.4)",
                                trimmed.lines().next().unwrap_or(trimmed)
                            ),
                        );
                    }
                }
            }
        }
    }

    fn validate_b3_statutory_stub(
        &self,
        path: &Path,
        b3_slice: &str,
        archetype: HeaderArchetype,
        items: &[TopLevelCItemWithPos],
        issues: &mut Vec<ValidationIssue>,
    ) {
        let b3_norm = b3_slice.split_whitespace().collect::<Vec<_>>().join(" ");
        match archetype {
            HeaderArchetype::PureTypes => {
                let has_any_note = b3_norm.contains(&self.rules.pure_types_statutory_text)
                    || b3_norm.contains("purely static structures")
                    || b3_norm.contains("pure type")
                    || b3_norm.contains("Pure Structural Definitions");
                if !has_any_note {
                    Self::push_b3_issue(
                        issues,
                        path,
                        "statutory_text",
                        IssueSeverity::Error,
                        "PureTypes header Block 3 missing statutory boundary notice".to_string(),
                    );
                } else if !b3_norm.contains(&self.rules.pure_types_statutory_text) {
                    Self::push_b3_issue(
                        issues,
                        path,
                        "statutory_text",
                        IssueSeverity::Warning,
                        format!(
                            "PureTypes Block 3 notice differs from canonical phrasing '{}'",
                            self.rules.pure_types_statutory_text
                        ),
                    );
                }
            }
            HeaderArchetype::FacadeUmbrella => {
                if !b3_norm.contains(&self.rules.facade_umbrella_statutory_text) {
                    Self::push_b3_issue(
                        issues,
                        path,
                        "facade_text",
                        IssueSeverity::Error,
                        format!(
                            "FacadeUmbrella Block 3 missing canonical delegation notice '{}'",
                            self.rules.facade_umbrella_statutory_text
                        ),
                    );
                }
            }
            _ => {}
        }

        for entry in items {
            let (raw, is_def) = match &entry.item {
                TopLevelCItem::FuncDef(s) => (s.as_str(), true),
                TopLevelCItem::OtherDecl(s) => (s.as_str(), false),
            };
            if let Some((fn_name, _, _)) = extract_c_func_sig(raw, is_def) {
                Self::push_b3_issue(
                    issues,
                    path,
                    "functions_in_statutory_stub",
                    IssueSeverity::Error,
                    format!(
                        "Statutory stub ({:?}) forbids operational function '{}' in Block 3",
                        archetype, fn_name
                    ),
                );
            }
        }
    }

    fn inspect_b3_items(
        &self,
        path: &Path,
        content: &str,
        b3_start: usize,
        items: &[TopLevelCItemWithPos],
        sub_assemblies: &[(String, usize, String, usize)],
        is_source: bool,
        issues: &mut Vec<ValidationIssue>,
    ) -> Vec<ParsedFuncDoc> {
        let mut parsed_fns = Vec::new();
        for entry in items {
            let abs_start = b3_start + entry.start;
            let (raw, is_def) = match &entry.item {
                TopLevelCItem::FuncDef(s) => (s.as_str(), true),
                TopLevelCItem::OtherDecl(s) => (s.as_str(), false),
            };

            if is_source && self.rules.forbid_static_in_block_3 && has_c_word(raw, "static") {
                Self::push_b3_issue(
                    issues,
                    path,
                    "forbid_static_in_block_3",
                    IssueSeverity::Error,
                    format!(
                        "Static linkage/storage '{}' is forbidden in Block 3; move storage to 2.3 and internal helpers to 2.4/2.5",
                        raw.lines().next().unwrap_or(raw).trim()
                    ),
                );
            }

            let Some((fn_name, has_params, returns_value)) = extract_c_func_sig(raw, is_def) else {
                continue;
            };

            let sub_idx = match sub_assemblies
                .iter()
                .rposition(|(_, _, _, sa_off)| *sa_off <= entry.start)
            {
                Some(idx) => idx,
                None => {
                    Self::push_b3_issue(
                        issues,
                        path,
                        "fn_before_sub_assembly",
                        IssueSeverity::Error,
                        format!(
                            "Function '{}' appears in Block 3 before Sub-Assembly 3.1",
                            fn_name
                        ),
                    );
                    0
                }
            };

            if self.rules.forbid_lifecycle_and_procedures_in_block_3
                && self.is_forbidden_b3_fn(&fn_name)
            {
                Self::push_b3_issue(
                    issues,
                    path,
                    "forbid_lifecycle_or_audit_in_block_3",
                    IssueSeverity::Error,
                    format!(
                        "Audit/preflight/dump-card/lifecycle symbol '{}' belongs in Block 4 or Block 5, not Block 3",
                        fn_name
                    ),
                );
            }

            let mut doc_entry = ParsedFuncDoc {
                name: fn_name.clone(),
                sub_assembly_idx: sub_idx,
                designer_text: String::new(),
                programmer_text: String::new(),
            };

            if self.rules.require_per_fn_doxygen_in_block_3 {
                match extract_preceding_doxygen(
                    content,
                    abs_start,
                    &self.rules.dual_audience_sections,
                ) {
                    None => {
                        Self::push_b3_issue(
                            issues,
                            path,
                            "per_fn_doxygen",
                            IssueSeverity::Error,
                            format!(
                                "Function '{}' in Block 3 is missing a preceding Doxygen /** ... */ comment",
                                fn_name
                            ),
                        );
                    }
                    Some(doc) => {
                        if !doc.has_brief {
                            Self::push_b3_issue(
                                issues,
                                path,
                                "fn_brief",
                                IssueSeverity::Error,
                                format!("Function '{}' Doxygen missing '@brief'", fn_name),
                            );
                        }
                        if !doc.has_dual_designer || !doc.has_dual_programmer {
                            Self::push_b3_issue(
                                issues,
                                path,
                                "fn_dual_audience",
                                IssueSeverity::Error,
                                format!(
                                    "Function '{}' Doxygen missing required Dual-Audience headings (Designer & Programmer)",
                                    fn_name
                                ),
                            );
                        }
                        if has_params && !doc.has_param_tag {
                            Self::push_b3_issue(
                                issues,
                                path,
                                "fn_param_doc",
                                IssueSeverity::Error,
                                format!("Function '{}' Doxygen missing '@param' documentation", fn_name),
                            );
                        }
                        if returns_value && !doc.has_return_tag {
                            Self::push_b3_issue(
                                issues,
                                path,
                                "fn_return_doc",
                                IssueSeverity::Error,
                                format!("Function '{}' Doxygen missing '@return' documentation", fn_name),
                            );
                        }
                        doc_entry.designer_text = doc.designer_text;
                        doc_entry.programmer_text = doc.programmer_text;
                    }
                }
            }

            parsed_fns.push(doc_entry);
        }
        parsed_fns
    }

    fn validate_b3_pair_parity(
        &self,
        c_path: &Path,
        hb: &ParsedBlock3,
        sb: &ParsedBlock3,
        issues: &mut Vec<ValidationIssue>,
    ) {
        if hb.sub_assemblies != sb.sub_assemblies {
            Self::push_b3_issue(
                issues,
                c_path,
                "b3_sub_assembly_parity",
                IssueSeverity::Error,
                format!(
                    "Block 3 Sub-Assembly 3.X parity mismatch between .h ({:?}) and .c ({:?})",
                    hb.sub_assemblies, sb.sub_assemblies
                ),
            );
        }

        if self.rules.enforce_sub_assembly_symbol_parity {
            let h_syms: Vec<(&str, &str)> = hb
                .functions
                .iter()
                .map(|f| {
                    let sa = hb
                        .sub_assemblies
                        .get(f.sub_assembly_idx)
                        .map(|(n, _)| n.as_str())
                        .unwrap_or("?");
                    (sa, f.name.as_str())
                })
                .collect();
            let s_syms: Vec<(&str, &str)> = sb
                .functions
                .iter()
                .map(|f| {
                    let sa = sb
                        .sub_assemblies
                        .get(f.sub_assembly_idx)
                        .map(|(n, _)| n.as_str())
                        .unwrap_or("?");
                    (sa, f.name.as_str())
                })
                .collect();
            if h_syms != s_syms {
                Self::push_b3_issue(
                    issues,
                    c_path,
                    "b3_symbol_parity",
                    IssueSeverity::Error,
                    format!(
                        "Block 3 Sub-Assembly function order/parity mismatch between .h ({} funcs) and .c ({} funcs)",
                        h_syms.len(),
                        s_syms.len()
                    ),
                );
            }
        }

        if self.rules.require_source_reconstructible_docs {
            for h_fn in &hb.functions {
                if let Some(c_fn) = sb.functions.iter().find(|f| f.name == h_fn.name) {
                    if !h_fn.designer_text.is_empty() && h_fn.designer_text == c_fn.designer_text {
                        Self::push_b3_issue(
                            issues,
                            c_path,
                            "b3_designer_prose_anti_duplication",
                            IssueSeverity::Error,
                            format!(
                                "Function '{}' source Designer prose must complement .h instead of copying it verbatim",
                                h_fn.name
                            ),
                        );
                    }
                    if !h_fn.programmer_text.is_empty()
                        && h_fn.programmer_text == c_fn.programmer_text
                    {
                        Self::push_b3_issue(
                            issues,
                            c_path,
                            "b3_programmer_prose_anti_duplication",
                            IssueSeverity::Error,
                            format!(
                                "Function '{}' source Programmer prose must detail implementation/probes instead of copying .h verbatim",
                                h_fn.name
                            ),
                        );
                    }
                }
            }
        }
    }

    fn validate_legacy_block_3_body(
        &self,
        path: &Path,
        content: &str,
        archetype: HeaderArchetype,
        issues: &mut Vec<ValidationIssue>,
    ) {
        match archetype {
            HeaderArchetype::Operational => {
                if !content.contains("Sub-Assembly 3.1") {
                    Self::push_b3_issue(
                        issues,
                        path,
                        "Sub-Assembly 3.1",
                        IssueSeverity::Warning,
                        "Missing 'Sub-Assembly 3.1' in body block".to_string(),
                    );
                }
                if !content.contains("Sub-Assembly 3.2") {
                    Self::push_b3_issue(
                        issues,
                        path,
                        "Sub-Assembly 3.2",
                        IssueSeverity::Warning,
                        "Missing 'Sub-Assembly 3.2' in body block".to_string(),
                    );
                }
                for sec in &self.rules.dual_audience_sections {
                    if !content.contains(sec) {
                        Self::push_b3_issue(
                            issues,
                            path,
                            "dual_audience_documentation",
                            IssueSeverity::Warning,
                            format!("Missing dual-audience section '{}'", sec),
                        );
                    }
                }
            }
            HeaderArchetype::QuarantinedFallback => {
                if !content.contains("Sub-Assembly 3.1") {
                    Self::push_b3_issue(
                        issues,
                        path,
                        "Sub-Assembly 3.1",
                        IssueSeverity::Warning,
                        "Missing 'Sub-Assembly 3.1' in fallback body block".to_string(),
                    );
                }
                for sec in &self.rules.dual_audience_sections {
                    if !content.contains(sec) {
                        Self::push_b3_issue(
                            issues,
                            path,
                            "dual_audience_documentation",
                            IssueSeverity::Warning,
                            format!("Missing dual-audience section '{}'", sec),
                        );
                    }
                }
            }
            HeaderArchetype::PureTypes => {
                if !content.contains("purely static structures") && !content.contains("pure type") {
                    Self::push_b3_issue(
                        issues,
                        path,
                        "statutory_text",
                        IssueSeverity::Warning,
                        "Pure type header missing statutory body note".to_string(),
                    );
                }
            }
            HeaderArchetype::FacadeUmbrella => {
                let has_facade_note = content.contains(
                    "all functional APIs are declared in their respective domain headers",
                ) || (content.contains("functional")
                    && content.contains("declared in their respective domain headers"));
                if !has_facade_note {
                    Self::push_b3_issue(
                        issues,
                        path,
                        "facade_text",
                        IssueSeverity::Warning,
                        "Facade umbrella missing statutory body cross-reference note".to_string(),
                    );
                }
            }
        }
    }

    fn validate_b3_structure(
        &self,
        path: &Path,
        rel_to_include: &str,
        content: &str,
        archetype: HeaderArchetype,
        is_source: bool,
        issues: &mut Vec<ValidationIssue>,
    ) -> Option<ParsedBlock3> {
        let Some(b3_start) = content.find(&self.rules.block_3_title) else {
            Self::push_b3_issue(
                issues,
                path,
                "header",
                IssueSeverity::Error,
                format!("Missing '{}' marker", self.rules.block_3_title),
            );
            return None;
        };

        let b3_end =
            Self::find_next_block_marker(content, b3_start + self.rules.block_3_title.len());
        let b3_slice = &content[b3_start..b3_end];

        if !self.is_strict_b3_domain(rel_to_include) {
            self.validate_legacy_block_3_body(path, content, archetype, issues);
            return None;
        }

        self.validate_cornerstone_b3_banner(path, b3_slice, archetype, is_source, issues);
        self.check_b3_macros(path, b3_slice, issues);

        let cleaned_full = strip_comments_strings_and_pp(content);
        let items = extract_top_level_c_items_with_pos(&cleaned_full[b3_start..b3_end]);

        if matches!(
            archetype,
            HeaderArchetype::PureTypes | HeaderArchetype::FacadeUmbrella
        ) {
            self.validate_b3_statutory_stub(path, b3_slice, archetype, &items, issues);
            return Some(ParsedBlock3 {
                sub_assemblies: Vec::new(),
                functions: Vec::new(),
            });
        }

        let sub_assemblies = parse_b3_sub_assemblies(b3_slice);
        if !is_source
            && !self.rules.triggers_cross_reference_tag.is_empty()
            && !b3_slice.contains(&self.rules.triggers_cross_reference_tag)
        {
            Self::push_b3_issue(
                issues,
                path,
                "triggers_cross_reference",
                IssueSeverity::Error,
                format!(
                    "Block 3 operational/fallback header missing '{}' cross-reference wiring",
                    self.rules.triggers_cross_reference_tag
                ),
            );
        }
        if sub_assemblies.is_empty() || sub_assemblies[0].0 != "3.1" {
            Self::push_b3_issue(
                issues,
                path,
                "b3_sub_assembly",
                IssueSeverity::Error,
                "Block 3 must begin functional layout with 'Sub-Assembly 3.1:'".to_string(),
            );
        }
        for pair in sub_assemblies.windows(2) {
            if pair[1].1 <= pair[0].1 {
                Self::push_b3_issue(
                    issues,
                    path,
                    "b3_sub_assembly_order",
                    IssueSeverity::Error,
                    format!(
                        "Block 3 Sub-Assemblies must be strictly increasing ({} followed by {})",
                        pair[0].0, pair[1].0
                    ),
                );
            }
        }

        let functions = self.inspect_b3_items(
            path,
            content,
            b3_start,
            &items,
            &sub_assemblies,
            is_source,
            issues,
        );

        Some(ParsedBlock3 {
            sub_assemblies: sub_assemblies
                .into_iter()
                .map(|(num, _, title, _)| (num, title))
                .collect(),
            functions,
        })
    }

    fn validate_block_3_body(
        &self,
        path: &Path,
        rel_to_include: &str,
        content: &str,
        archetype: HeaderArchetype,
        issues: &mut Vec<ValidationIssue>,
    ) -> Option<ParsedBlock3> {
        self.validate_b3_structure(path, rel_to_include, content, archetype, false, issues)
    }

    fn validate_block_4_audit(
        &self,
        path: &Path,
        content: &str,
        archetype: HeaderArchetype,
        issues: &mut Vec<ValidationIssue>,
    ) {
        if !content.contains("BLOCK 4: AUDIT PROCEDURES BLOCK") && !content.contains("BLOCK 4: AUDIT BLOCK") {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_4_audit".to_string(),
                field: "header".to_string(),
                severity: IssueSeverity::Error,
                message: "Missing 'BLOCK 4: AUDIT PROCEDURES BLOCK' marker".to_string(),
            });
        }

        match archetype {
            HeaderArchetype::Operational | HeaderArchetype::QuarantinedFallback => {
                if !content.contains("Sub-Assembly 4.1") {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_4_audit".to_string(),
                        field: "Sub-Assembly 4.1".to_string(),
                        severity: IssueSeverity::Error,
                        message: "Missing 'Sub-Assembly 4.1' in audit block".to_string(),
                    });
                }
                if !content.contains("Sub-Assembly 4.2") {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_4_audit".to_string(),
                        field: "Sub-Assembly 4.2".to_string(),
                        severity: IssueSeverity::Error,
                        message: "Missing 'Sub-Assembly 4.2' in audit block".to_string(),
                    });
                }

                // Verify audit and preflight cog declarations
                let has_audit = content.contains(&self.rules.audit_pattern);
                let has_preflight = content.contains(&self.rules.preflight_pattern);

                if !has_audit {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_4_audit".to_string(),
                        field: "audit_cog".to_string(),
                        severity: IssueSeverity::Error,
                        message: format!("Missing operational audit function matching '{}'", self.rules.audit_pattern),
                    });
                }
                if !has_preflight {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_4_audit".to_string(),
                        field: "preflight_cog".to_string(),
                        severity: IssueSeverity::Error,
                        message: format!("Missing operational preflight function matching '{}'", self.rules.preflight_pattern),
                    });
                }
            }
            HeaderArchetype::PureTypes => {
                if !content.contains("Functional audits") && !content.contains("declared in") {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_4_audit".to_string(),
                        field: "statutory_text".to_string(),
                        severity: IssueSeverity::Warning,
                        message: "Pure type header missing statutory audit cross-reference note".to_string(),
                    });
                }
            }
            HeaderArchetype::FacadeUmbrella => {
                if !content.contains("Sub-Assembly 4.1") || !content.contains("Sub-Assembly 4.2") {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_4_audit".to_string(),
                        field: "cross_references".to_string(),
                        severity: IssueSeverity::Warning,
                        message: "Facade umbrella missing Sub-Assembly 4.1 or 4.2 cross-references".to_string(),
                    });
                }
            }
        }
    }

    fn validate_block_5_closing(
        &self,
        path: &Path,
        rel_to_include: &str,
        content: &str,
        archetype: HeaderArchetype,
        stateful: bool,
        issues: &mut Vec<ValidationIssue>,
    ) {
        if !content.contains("BLOCK 5: CLOSING BLOCK") && !content.contains("BLOCK 5: CLOSING & LIFECYCLE BLOCK") {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_5_closing".to_string(),
                field: "header".to_string(),
                severity: IssueSeverity::Error,
                message: "Missing 'BLOCK 5: CLOSING BLOCK' marker".to_string(),
            });
        }

        match archetype {
            HeaderArchetype::Operational | HeaderArchetype::QuarantinedFallback => {
                if !content.contains("Sub-Assembly 5.1") {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_5_closing".to_string(),
                        field: "Sub-Assembly 5.1".to_string(),
                        severity: IssueSeverity::Error,
                        message: "Missing 'Sub-Assembly 5.1' in closing block".to_string(),
                    });
                }
                if !content.contains("Sub-Assembly 5.2") {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_5_closing".to_string(),
                        field: "Sub-Assembly 5.2".to_string(),
                        severity: IssueSeverity::Error,
                        message: "Missing 'Sub-Assembly 5.2' in closing block".to_string(),
                    });
                }

                // Verify dump card declaration
                if !content.contains(&self.rules.dump_card_suffix) {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_5_closing".to_string(),
                        field: "dump_card".to_string(),
                        severity: IssueSeverity::Error,
                        message: format!("Missing diagnostic dump card declaration matching '{}'", self.rules.dump_card_suffix),
                    });
                }

                // Verify Sub-Assembly 5.2 lifecycle or statutory stateless exemption
                if stateful {
                    let has_init = content.contains("_init")
                        || (archetype == HeaderArchetype::QuarantinedFallback
                            && content.contains(&self.rules.lifecycle_boot_suffix));
                    let has_shutdown = content.contains("_shutdown") || content.contains("_teardown");
                    if !has_init || !has_shutdown {
                        issues.push(ValidationIssue {
                            file: path.to_path_buf(),
                            block: "block_5_closing".to_string(),
                            field: "lifecycle".to_string(),
                            severity: IssueSeverity::Error,
                            message: "Stateful subsystem must declare _init (or _boot for BIOS fallback) and _shutdown/_teardown procedures".to_string(),
                        });
                    }
                } else {
                    let has_exemption = content.contains("Stateless module")
                        || content.contains("Stateless foundation")
                        || content.contains("pure stack-allocated execution")
                        || content.contains("Pure procedural loop")
                        || content.contains(&self.rules.stateless_exemption_literal);
                    if !has_exemption {
                        issues.push(ValidationIssue {
                            file: path.to_path_buf(),
                            block: "block_5_closing".to_string(),
                            field: "stateless_exemption".to_string(),
                            severity: IssueSeverity::Error,
                            message: "Stateless subsystem must declare statutory exemption note in Sub-Assembly 5.2".to_string(),
                        });
                    }
                }
            }
            HeaderArchetype::PureTypes => {
                if !content.contains("Pure type definition header") && !content.contains("declared in") {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_5_closing".to_string(),
                        field: "statutory_text".to_string(),
                        severity: IssueSeverity::Warning,
                        message: "Pure type header missing statutory closing cross-reference note".to_string(),
                    });
                }
            }
            HeaderArchetype::FacadeUmbrella => {
                if !content.contains("Sub-Assembly 5.1") || !content.contains("Sub-Assembly 5.2") {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_5_closing".to_string(),
                        field: "cross_references".to_string(),
                        severity: IssueSeverity::Warning,
                        message: "Facade umbrella missing Sub-Assembly 5.1 or 5.2 cross-references".to_string(),
                    });
                }
            }
        }

        if self.is_strict_b2_domain(rel_to_include) {
            let non_empty: Vec<&str> = content
                .lines()
                .map(str::trim)
                .filter(|l| !l.is_empty())
                .collect();

            if self.rules.enforce_matching_endif {
                let expected_endif =
                    format!("#endif /* {} */", self.expected_cornerstone_guard(rel_to_include));
                if non_empty.last().copied() != Some(expected_endif.as_str()) {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_5_closing".to_string(),
                        field: "endif".to_string(),
                        severity: IssueSeverity::Error,
                        message: format!(
                            "Final non-empty line must match exact Cornerstone guard '{}'",
                            expected_endif
                        ),
                    });
                }
            }

            if self.rules.require_extern_c_in_headers {
                let before_guard = if non_empty.last().is_some_and(|l| l.starts_with("#endif")) {
                    &non_empty[..non_empty.len().saturating_sub(1)]
                } else {
                    &non_empty[..]
                };
                let tail_start = before_guard.len().saturating_sub(10);
                let has_extern_c_close = before_guard[tail_start..]
                    .windows(3)
                    .any(|w| w == ["#ifdef __cplusplus", "}", "#endif"]);
                if !has_extern_c_close {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_5_closing".to_string(),
                        field: "extern_c_close".to_string(),
                        severity: IssueSeverity::Error,
                        message: "Strict Block 2 header missing closing '#ifdef __cplusplus / } / #endif' before final #endif guard".to_string(),
                    });
                }
            }
            return;
        }

        // Validate final #endif comment matches include guard (legacy baseline)
        let guard_prefix = &self.rules.include_guard_prefix;
        let last_lines: Vec<&str> = content.lines().rev().take(5).collect();
        let has_matching_endif = last_lines.iter().any(|l| l.starts_with("#endif") && l.contains(guard_prefix));

        if !has_matching_endif {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_5_closing".to_string(),
                field: "endif".to_string(),
                severity: IssueSeverity::Error,
                message: format!("Missing matching #endif comment with prefix '{}'", guard_prefix),
            });
        }
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK / TASK RUNNER IMPLEMENTATION
// Stratum: Developer Tooling | Machine: tools.validator.c_header
// =============================================================================

/// Task runner adapter integrating C11 header validation.
pub struct HeaderValidationTask;

impl TaskRunner for HeaderValidationTask {
    fn name(&self) -> &'static str {
        "header_validator"
    }

    fn description(&self) -> &'static str {
        "Audits C11 engine headers across engine/include/ for OmniCode 5-block compliance"
    }

    fn preflight(&self, ctx: &RunnerContext) -> Result<(), String> {
        let include_dir = ctx.repo_root.join("engine/include");
        if !include_dir.exists() {
            return Err(format!("Engine include directory not found at {:?}", include_dir));
        }
        Ok(())
    }

    fn run(&self, ctx: &RunnerContext) -> Result<TaskTelemetry, String> {
        let registry = super::config::load_subsystems_registry(&ctx.repo_root)?;
        let rules = super::config::load_header_rules_config(&ctx.repo_root);

        let validator = HeaderValidator::new(ctx.repo_root.clone(), registry, rules, ctx.verbose);
        let summary = validator.run_audit()?;

        if summary.is_success(false) {
            Ok(TaskTelemetry::success(
                self.name(),
                summary.duration_ms,
                &format!(
                    "Audited {} C11 headers cleanly (0 errors, {} warnings)",
                    summary.total_files,
                    summary.warning_count()
                ),
            ))
        } else {
            Ok(TaskTelemetry::failure(
                self.name(),
                summary.duration_ms,
                &format!(
                    "C11 header validation failed: {} errors across {} files",
                    summary.error_count(),
                    summary.failed_files
                ),
            ))
        }
    }
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK / TESTS
// Stratum: Developer Tooling | Machine: tools.validator.c_header
// =============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_all_engine_headers_pass_validation() {
        let candidate_roots = [Path::new("."), Path::new("..")];
        let repo_root = candidate_roots
            .iter()
            .find(|p| p.join("engine/include").exists())
            .map(|p| p.canonicalize().unwrap_or_else(|_| p.to_path_buf()))
            .unwrap_or_else(|| PathBuf::from("."));

        if !repo_root.join("engine/include").exists() {
            return;
        }

        let registry = crate::validator::config::load_subsystems_registry(&repo_root)
            .expect("Failed to load subsystems registry");
        let expected_headers = registry.headers.len();
        let rules = crate::validator::config::load_header_rules_config(&repo_root);

        let validator = HeaderValidator::new(repo_root, registry, rules, false);
        let summary = validator.run_audit().expect("Audit run failed");

        assert!(
            summary.is_success(false),
            "C11 Header validation failed with {} errors across {} files",
            summary.error_count(),
            summary.failed_files
        );
        assert_eq!(
            summary.total_files, expected_headers,
            "Audited engine header count must match subsystems.json registry ({})",
            expected_headers
        );
    }

    #[test]
    fn test_block_1_template_schema_self_consistent() {
        let candidate_roots = [Path::new("."), Path::new("..")];
        let repo_root = candidate_roots
            .iter()
            .find(|p| p.join("engine/config/standards/blocks/block_1_metadata.json").exists())
            .map(|p| p.canonicalize().unwrap_or_else(|_| p.to_path_buf()))
            .unwrap_or_else(|| PathBuf::from("."));

        if !repo_root.join("engine/config/standards/blocks/block_1_metadata.json").exists() {
            return;
        }

        let rules = crate::validator::config::load_header_rules_config(&repo_root);
        assert_eq!(rules.ordered_metadata_tags.len(), 17);
        assert_eq!(rules.growth_tags.len(), 6);
        assert!(!rules.header_template_lines.is_empty());
        assert!(!rules.source_template_lines.is_empty());
        assert!(rules.strict_template_domains.contains(&"log".to_string()));
        assert!(rules.strict_template_domains.contains(&"debug".to_string()));
        assert!(rules.strict_template_domains.contains(&"io".to_string()));

        for tpl_lines in [&rules.header_template_lines, &rules.source_template_lines] {
            let joined = tpl_lines.join("\n");
            let parsed = parse_block_1_comment(&joined, rules.tag_column_width)
                .expect("Embedded c_block_1_template must parse cleanly");
            let tpl_tags: Vec<&str> = parsed.tags.iter().map(|(k, _, _)| k.as_str()).collect();
            let expected_tags: Vec<&str> = rules.ordered_metadata_tags.iter().map(|s| s.as_str()).collect();
            assert_eq!(tpl_tags, expected_tags, "Template tags must match ordered_tags");
            assert!(parsed.tags.iter().all(|(_, _, aligned)| *aligned), "All template tags must align to tag_column_width");
            assert!(parsed.sections_in_order && parsed.sections_indented, "Template dual-audience sections must be ordered & indented");
        }
    }

    #[test]
    fn test_block_2_config_and_guard_derivation() {
        let candidate_roots = [Path::new("."), Path::new("..")];
        let repo_root = candidate_roots
            .iter()
            .find(|p| p.join("engine/config/standards/blocks/block_2_opening.json").exists())
            .map(|p| p.canonicalize().unwrap_or_else(|_| p.to_path_buf()))
            .unwrap_or_else(|| PathBuf::from("."));

        if !repo_root.join("engine/config/standards/blocks/block_2_opening.json").exists() {
            return;
        }

        let rules = crate::validator::config::load_header_rules_config(&repo_root);
        assert_eq!(rules.engine_name, "Cornerstone Game Engine");
        assert_eq!(rules.strict_include_guard_prefix, "CORNERSTONE_");
        assert!(rules.forbid_2_5_in_headers);
        assert!(rules.require_extern_c_in_headers);
        assert!(rules.strict_block_2_domains.contains(&"log".to_string()));
        assert!(rules.strict_block_2_domains.contains(&"debug".to_string()));
        assert!(rules.strict_block_2_domains.contains(&"io".to_string()));

        let validator = HeaderValidator::new(
            repo_root,
            SubsystemRegistry::default(),
            rules,
            false,
        );
        assert_eq!(validator.expected_cornerstone_guard("log/log.h"), "CORNERSTONE_LOG_LOG_H");
        assert_eq!(
            validator.expected_cornerstone_guard("debug/debug_types.h"),
            "CORNERSTONE_DEBUG_DEBUG_TYPES_H"
        );
    }

    #[test]
    fn test_block_2_header_strict_rules_and_fallback_isolation() {
        let rules = HeaderRulesConfig::default();
        let validator = HeaderValidator::new(
            PathBuf::from("."),
            SubsystemRegistry::default(),
            rules,
            false,
        );

        let valid_fb = r#"
/* BLOCK 1: METADATA BLOCK */
#ifndef CORNERSTONE_LOG_LOG_FALLBACK_H
#define CORNERSTONE_LOG_LOG_FALLBACK_H
/*
BLOCK 2: OPENING BLOCK
Engine:   Cornerstone Game Engine
Stratum:  Layer 0
Module:   log.fallback
Mission:  Safe boot
*/
/* --- Tier 1: ISO C11 --- */
#include <stdbool.h>
/* --- Tier 2: External Libraries --- */
/* None */
/* --- Tier 3: Engine Headers --- */
#include "log/log_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Sub-Assembly 2.1: Limits */
/* Sub-Assembly 2.2: Manifest */
/* Sub-Assembly 2.3: Types */
/* Sub-Assembly 2.4: Inlines */
/* BLOCK 3: BODY BLOCK */
/* BLOCK 5: CLOSING BLOCK */
/* Sub-Assembly 5.1: Diagnostic Dump Cards */
void log_fallback_execute_dump_card(void);
/* Sub-Assembly 5.2: Lifecycle Boot & Teardown */
int log_fallback_boot(void);
void log_fallback_shutdown(void);
#ifdef __cplusplus
}
#endif
#endif /* CORNERSTONE_LOG_LOG_FALLBACK_H */
"#;
        let mut ok_issues = Vec::new();
        validator.validate_block_2_opening(
            Path::new("log_fallback.h"),
            "log/log_fallback.h",
            valid_fb,
            HeaderArchetype::QuarantinedFallback,
            &mut ok_issues,
        );
        validator.validate_block_5_closing(
            Path::new("log_fallback.h"),
            "log/log_fallback.h",
            valid_fb,
            HeaderArchetype::QuarantinedFallback,
            true,
            &mut ok_issues,
        );
        assert!(
            ok_issues.is_empty(),
            "Valid Cornerstone QuarantinedFallback header produced issues: {:?}",
            ok_issues
        );

        let bad_fb = r#"
#ifndef TENEBRIS_LUX_LOG_FALLBACK_H
#define TENEBRIS_LUX_LOG_FALLBACK_H
/*
BLOCK 2: OPENING BLOCK
Engine:   Cornerstone Game Engine
Stratum:  Layer 0
Module:   log.fallback
Mission:  Safe boot
*/
/* --- Tier 1: ISO C11 --- */
/* --- Tier 2: External Libraries --- */
/* --- Tier 3: Engine Headers --- */
#include "io/fs_utils.h"
/* Sub-Assembly 2.1: Limits */
/* Sub-Assembly 2.2: Manifest */
/* Sub-Assembly 2.3: Types */
/* Sub-Assembly 2.4: Inlines */
/* Sub-Assembly 2.5: Illegal in header */
/* BLOCK 3: BODY BLOCK */
#endif
"#;
        let mut bad_issues = Vec::new();
        validator.validate_block_2_opening(
            Path::new("log_fallback.h"),
            "log/log_fallback.h",
            bad_fb,
            HeaderArchetype::QuarantinedFallback,
            &mut bad_issues,
        );
        validator.validate_block_5_closing(
            Path::new("log_fallback.h"),
            "log/log_fallback.h",
            bad_fb,
            HeaderArchetype::QuarantinedFallback,
            true,
            &mut bad_issues,
        );
        for expected_field in [
            "include_guard",
            "extern_c_open",
            "quarantined_fallback_isolation",
            "sub_assembly_2_5",
            "endif",
            "extern_c_close",
        ] {
            assert!(
                bad_issues.iter().any(|i| i.field == expected_field && i.severity == IssueSeverity::Error),
                "Expected Error on field '{}', got {:?}",
                expected_field,
                bad_issues
            );
        }
    }

    #[test]
    fn test_block_2_source_posix_order_2_4_warnings_and_monitored_2_5() {
        let rules = HeaderRulesConfig::default();
        let validator = HeaderValidator::new(
            PathBuf::from("."),
            SubsystemRegistry::default(),
            rules,
            false,
        );

        let valid_c = r#"
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
/*
BLOCK 2: OPENING BLOCK
Engine:   Cornerstone Game Engine
Stratum:  Layer 0
Module:   log.core
Mission:  Flight ring
*/
/* --- Tier 1: ISO C11 --- */
/* --- Tier 2: External Libraries --- */
/* --- Tier 3: Engine Headers --- */
/* Sub-Assembly 2.1: Limits */
/* Sub-Assembly 2.2: Private Types */
/* Sub-Assembly 2.3: Static Storage */
static int s_ring_buf[8];
/* Sub-Assembly 2.4: Inline Predicates */
#define LOG_MACRO(x) \
    do { if ((x) == 0) { break; } } while (0)
static int log_helper_fwd(int x);
static inline bool log_is_valid(int x) { return x > 0; }
/* Sub-Assembly 2.5: Monitored Support Helpers
 * Support Role: Internal formatting helper
 * Pattern Watch: Split if another module needs formatting
 * Linkage Rule: Strictly static internal linkage only
 */
static int log_helper_fwd(int x) {
    static const int k_mult = 2;
    return x * k_mult;
}
/* BLOCK 3: BODY BLOCK */
"#;
        let mut ok_issues = Vec::new();
        validator.validate_source_block_2_opening(Path::new("log.c"), valid_c, &mut ok_issues);
        assert!(
            ok_issues.is_empty(),
            "Valid Cornerstone source produced unexpected issues: {:?}",
            ok_issues
        );

        let bad_c = r#"
/*
BLOCK 2: OPENING BLOCK
Engine:   Cornerstone Game Engine
Stratum:  Layer 0
Module:   log.core
Mission:  Flight ring
*/
#define _POSIX_C_SOURCE 200809L
/* --- Tier 1: ISO C11 --- */
/* --- Tier 2: External Libraries --- */
/* --- Tier 3: Engine Headers --- */
/* Sub-Assembly 2.1: Limits */
/* Sub-Assembly 2.2: Private Types */
/* Sub-Assembly 2.3: Static Storage */
/* Sub-Assembly 2.4: Inline Predicates */
static int non_inline_in_2_4(int x) { return x + 1; }
/* Sub-Assembly 2.5: Internal Support Helpers
 * Support Role: Unmonitored
 */
static int s_forbidden_arr[4] = {1, 2, 3, 4};
int non_static_helper(void) { return 0; }
/* BLOCK 3: BODY BLOCK */
"#;
        let mut bad_issues = Vec::new();
        validator.validate_source_block_2_opening(Path::new("log.c"), bad_c, &mut bad_issues);
        assert!(bad_issues.iter().any(|i| i.field == "posix_macro_placement" && i.severity == IssueSeverity::Error));
        assert!(bad_issues.iter().any(|i| i.field == "sub_assembly_2_4" && i.severity == IssueSeverity::Warning));
        assert!(bad_issues.iter().any(|i| i.field == "sub_assembly_2_5_banner" && i.severity == IssueSeverity::Error));
        assert!(bad_issues.iter().any(|i| i.field == "sub_assembly_2_5_linkage" && i.severity == IssueSeverity::Error));
        assert!(bad_issues.iter().any(|i| i.field == "sub_assembly_2_5_storage" && i.severity == IssueSeverity::Error));
    }

    #[test]
    fn test_block_3_config_loading_and_strict_rules() {
        let candidate_roots = [Path::new("."), Path::new("..")];
        let repo_root = candidate_roots
            .iter()
            .find(|p| p.join("engine/config/standards/blocks/block_3_body.json").exists())
            .map(|p| p.canonicalize().unwrap_or_else(|_| p.to_path_buf()))
            .unwrap_or_else(|| PathBuf::from("."));

        if repo_root
            .join("engine/config/standards/blocks/block_3_body.json")
            .exists()
        {
            let loaded = crate::validator::config::load_header_rules_config(&repo_root);
            assert!(loaded.strict_block_3_domains.contains(&"log".to_string()));
            assert!(loaded.strict_block_3_domains.contains(&"debug".to_string()));
            assert!(loaded.strict_block_3_domains.contains(&"io".to_string()));
            assert_eq!(loaded.block_3_title, "BLOCK 3: BODY BLOCK");
            assert_eq!(loaded.triggers_cross_reference_tag, "@triggers");
            assert!(loaded.ban_macros_in_block_3);
            assert!(loaded.forbid_static_in_block_3);
            assert!(loaded.forbid_lifecycle_and_procedures_in_block_3);
            assert!(loaded.enforce_sub_assembly_symbol_parity);
            assert!(loaded.require_per_fn_doxygen_in_block_3);
            assert!(loaded.require_source_reconstructible_docs);
        }

        let rules = HeaderRulesConfig::default();
        let validator = HeaderValidator::new(
            PathBuf::from("."),
            SubsystemRegistry::default(),
            rules,
            false,
        );

        let valid_h = r#"
/*
BLOCK 3: BODY BLOCK
( Public Operational Cogs, State Accessors & Domain APIs )
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.debug
Mission:       Diagnostic query and tripwire reporting APIs.
*/
/* Sub-Assembly 3.1: Core Operations
 * @triggers Block 4 probes verify contract integrity.
 */
/**
 * @brief Clears active state without returning a value.
 *
 * ### For Game Designers & Non-Programmers:
 * Resets runtime flags before a new scenario begins.
 *
 * ### For Systems Programmers & Engine Architects:
 * Public interface contract mutating module state atomically with zero heap use.
 */
void debug_clear_scenario(void);

/**
 * @brief Looks up a raw static descriptor handle.
 *
 * ### For Game Designers & Non-Programmers:
 * Retrieves an internal reference handle for diagnostic menus.
 *
 * ### For Systems Programmers & Engine Architects:
 * Contract: returns read-only pointer or NULL on invalid key.
 *
 * @param[in] key Registry key string.
 * @return Immutable pointer into static ROM table, or NULL.
 */
const void *debug_find_handle(const char *key);

/**
 * @brief Emits the tripwire diagnostic report.
 *
 * ### For Game Designers & Non-Programmers:
 * Prints a human-readable summary of recent safety checks.
 *
 * ### For Systems Programmers & Engine Architects:
 * Public operational reporting seam allowed cleanly inside Block 3 Sub-Assembly 3.5.
 */
void debug_dump_tripwire_report(void);
/* BLOCK 4: AUDIT BLOCK */
"#;

        let valid_c = r#"
/*
BLOCK 3: BODY BLOCK
( Operational Business Logic, Span Scopes & Runtime Hydration )
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.debug
Mission:       Diagnostic runtime engine.
*/
/* Sub-Assembly 3.1: Core Operations */
/**
 * @brief Clears active state counters in static .bss without freeing memory.
 *
 * ### For Game Designers & Non-Programmers:
 * Rewinds counters immediately between test runs without affecting ROM settings.
 *
 * ### For Systems Programmers & Engine Architects:
 * Sets `s_state.count = 0U` directly in static storage in O(1) without heap allocation.
 */
void debug_clear_scenario(void) {
    /* Step 1: reset static counter */
}

/**
 * @brief Scans static .rodata descriptor table for matching key.
 *
 * ### For Game Designers & Non-Programmers:
 * Searches built-in engine ROM entries for the requested label.
 *
 * ### For Systems Programmers & Engine Architects:
 * Step 1: guard NULL key; Step 2: O(N) linear scan across `.rodata` descriptors.
 *
 * @param[in] key Non-empty identifier token.
 * @return Pointer into `.rodata` descriptor or NULL if unmatched.
 */
const void *debug_find_handle(const char *key) {
    (void)key;
    return 0;
}

/**
 * @brief Formats latched tripwire records into stderr/telemetry stream.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays tripwire fault counts and last reason clearly on the console.
 *
 * ### For Systems Programmers & Engine Architects:
 * Walks fixed ring buffer in O(N) without heap allocations; emits LOG_WARN when tripped.
 */
void debug_dump_tripwire_report(void) {
}
/* BLOCK 4: AUDIT BLOCK */
"#;

        let mut ok_issues = Vec::new();
        let hb = validator
            .validate_block_3_body(
                Path::new("debug/debug.h"),
                "debug/debug.h",
                valid_h,
                HeaderArchetype::Operational,
                &mut ok_issues,
            )
            .expect("Valid strict Block 3 header must parse");
        let sb = validator
            .validate_b3_structure(
                Path::new("debug/debug.c"),
                "debug/debug.h",
                valid_c,
                HeaderArchetype::Operational,
                true,
                &mut ok_issues,
            )
            .expect("Valid strict Block 3 source must parse");
        validator.validate_b3_pair_parity(Path::new("debug/debug.c"), &hb, &sb, &mut ok_issues);
        assert!(
            ok_issues.is_empty(),
            "Valid Cornerstone Block 3 pair produced unexpected issues: {:?}",
            ok_issues
        );

        let bad_c = r#"
/*
BLOCK 3: BODY BLOCK
( Custom Wrong Subtitle )
Engine:        Cornerstone Game Engine
Stratum:       Layer 0
Module:        cog.log
Mission:       Leaky implementation.
*/
#define BAD_B3_MACRO 1
void fn_too_early(void) {}
/* Sub-Assembly 3.2: Second */
/* Sub-Assembly 3.1: First */
static int s_bad_b3_var = 0;

void log_missing_doc(int x) { (void)x; }

/**
 * @brief Bad lifecycle leak.
 *
 * ### For Game Designers & Non-Programmers:
 * Resets runtime flags before a new scenario begins.
 *
 * ### For Systems Programmers & Engine Architects:
 * Public interface contract mutating module state atomically with zero heap use.
 */
void *log_bad_init(int bad_param) { return 0; }
/* BLOCK 4: AUDIT BLOCK */
"#;
        let mut bad_issues = Vec::new();
        let bad_sb = validator
            .validate_b3_structure(
                Path::new("debug/debug.c"),
                "debug/debug.h",
                bad_c,
                HeaderArchetype::Operational,
                true,
                &mut bad_issues,
            )
            .expect("Invalid Block 3 source still returns ParsedBlock3");
        validator.validate_b3_pair_parity(
            Path::new("debug/debug.c"),
            &hb,
            &bad_sb,
            &mut bad_issues,
        );

        for expected_field in [
            "b3_archetype_subtitle",
            "macros_in_block_3",
            "b3_sub_assembly",
            "b3_sub_assembly_order",
            "forbid_static_in_block_3",
            "fn_before_sub_assembly",
            "per_fn_doxygen",
            "forbid_lifecycle_or_audit_in_block_3",
            "fn_param_doc",
            "fn_return_doc",
            "b3_sub_assembly_parity",
            "b3_symbol_parity",
        ] {
            assert!(
                bad_issues.iter().any(|i| i.field == expected_field),
                "Expected issue field '{}' not found in {:?}",
                expected_field,
                bad_issues
            );
        }

        // Check verbatim Designer/Programmer anti-duplication on matching function name
        let verbatim_c = valid_c.replace(
            "Rewinds counters immediately between test runs without affecting ROM settings.",
            "Resets runtime flags before a new scenario begins.",
        );
        let mut verb_issues = Vec::new();
        let vsb = validator
            .validate_b3_structure(
                Path::new("debug/debug.c"),
                "debug/debug.h",
                &verbatim_c,
                HeaderArchetype::Operational,
                true,
                &mut verb_issues,
            )
            .unwrap();
        validator.validate_b3_pair_parity(Path::new("debug/debug.c"), &hb, &vsb, &mut verb_issues);
        assert!(verb_issues
            .iter()
            .any(|i| i.field == "b3_designer_prose_anti_duplication" && i.severity == IssueSeverity::Error));

        // Check PureTypes and FacadeUmbrella reject operational functions and enforce statutory text
        let bad_stub = r#"
/*
BLOCK 3: BODY BLOCK
( Statutory Pure-Types Notice & Structural Boundary )
Engine:        Cornerstone Game Engine
Stratum:       Layer 0
Module:        cog.log.types
Mission:       Stub.
*/
int illegal_stub_fn(void);
/* BLOCK 4: AUDIT BLOCK */
"#;
        let mut stub_issues = Vec::new();
        validator.validate_block_3_body(
            Path::new("log/log_types.h"),
            "log/log_types.h",
            bad_stub,
            HeaderArchetype::PureTypes,
            &mut stub_issues,
        );
        assert!(stub_issues
            .iter()
            .any(|i| i.field == "statutory_text" && i.severity == IssueSeverity::Error));
        assert!(stub_issues
            .iter()
            .any(|i| i.field == "functions_in_statutory_stub" && i.severity == IssueSeverity::Error));

        let mut facade_issues = Vec::new();
        validator.validate_block_3_body(
            Path::new("io/io.h"),
            "io/io.h",
            bad_stub,
            HeaderArchetype::FacadeUmbrella,
            &mut facade_issues,
        );
        assert!(facade_issues
            .iter()
            .any(|i| i.field == "facade_text" && i.severity == IssueSeverity::Error));
        assert!(facade_issues
            .iter()
            .any(|i| i.field == "functions_in_statutory_stub" && i.severity == IssueSeverity::Error));
    }
}
