// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/guide/compiler.rs
// @id            cog.tools.guide.compiler
// @layer         Layer 3: Developer Toolchain Command Sub-Assemblies
// @archetype     pure_types | transformation_engine
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Guide Compiler Sub-Assembly
// @role          AsciiDoc Preprocessor, Markdown Translator & Book Assembler
// @maturity      Rev 5 Baseline | In-process document transformation pipeline
// @chassis       bounded-heap | Developer tooling memory chassis; bounded text
// @scale         O(C * N) across chapters and document lines
// @seams         in: docs/**/*.adoc | out: master & volume .adoc / .md
// @invariants    1. Fence-aware heading shifts (never shift inside delimited blocks).
//                2. Clean GFM Markdown translation preserving tables and callouts.
//                3. Zero process spawning for document assembly.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Preprocesses AsciiDoc chapter sources, expands includes and custom attributes,
//   shifts headings hierarchically, converts AsciiDoc to clean GitHub-Flavored
//   Markdown, and assembles the unified Master Guide compendium.
//
//   ### For Game Designers & Non-Programmers:
//   Automatically translates your AsciiDoc design and lore documents into
//   single-file Markdown compendiums for Google Gemini and NotebookLM research.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Operates purely on immutable string slices and
//     file paths without mutating source documents on disk.
//   - Extension Seam: Additional inline AsciiDoc macros translate in `convert_inline_xrefs`.
//   - Explicit Non-Goal: Does not spawn external processes during document assembly.
// =============================================================================

//! # AsciiDoc Preprocessor, Markdown Translator & Book Assembler
//!
//! Handles include expansion, heading level shifting, AsciiDoc-to-Markdown
//! translation, and whole-book assembly.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

use std::collections::{HashMap, HashSet};
use std::fs;
use std::path::{Path, PathBuf};

use super::manifest::{ChapterSpec, GuideConfig, VolumeSpec};
use super::snapshot::{
    format_wholesale_appendix_entry, snapshot_goes_in_appendix, RenderedSnapshot,
};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Constants & Target Defaults
// -----------------------------------------------------------------------------

/// Maximum recursion depth for nested `include::` expansion.
pub const MAX_INCLUDE_DEPTH: usize = 6;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Verification Helpers & Invariant Predicates
// -----------------------------------------------------------------------------

/// Evaluates whether a line starts a delimited verbatim code or comment fence.
pub fn is_fence_delimiter(trimmed: &str) -> bool {
    matches!(
        trimmed,
        "----" | "------" | "...." | "////" | "====" | "****" | "|===" | "--"
    ) || trimmed.starts_with("```")
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Document Preprocessing, Translation & Whole-Book Assembly )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Directory Scanning & Title Extraction
// -----------------------------------------------------------------------------

/// Recursively scans an area directory relative to docs/ for .adoc and .md chapters.
pub fn scan_area_for_chapters(docs_dir: &Path, area_rel: &str) -> Vec<(String, PathBuf)> {
    let excludes = vec![
        "snapshots".to_string(),
        "themes".to_string(),
        "volumes".to_string(),
        "offline_work".to_string(),
    ];
    scan_area_for_chapters_with_excludes(docs_dir, area_rel, &excludes)
}

/// Recursively scans an area directory with explicit exclude filters.
pub fn scan_area_for_chapters_with_excludes(
    docs_dir: &Path,
    area_rel: &str,
    excludes: &[String],
) -> Vec<(String, PathBuf)> {
    let target_dir = docs_dir.join(area_rel);
    let mut results = Vec::new();
    if !target_dir.is_dir() {
        return results;
    }

    scan_dir_recursive(&target_dir, excludes, &mut results);
    results.sort_by(|a, b| a.1.cmp(&b.1));
    results
}

fn scan_dir_recursive(dir: &Path, excludes: &[String], out: &mut Vec<(String, PathBuf)>) {
    if let Ok(entries) = fs::read_dir(dir) {
        for entry in entries.flatten() {
            let path = entry.path();
            let name = path.file_name().and_then(|s| s.to_str()).unwrap_or("");
            if path.is_dir() {
                if excludes.iter().any(|ex| ex.eq_ignore_ascii_case(name)) {
                    continue;
                }
                scan_dir_recursive(&path, excludes, out);
            } else if path.is_file() {
                let ext = path.extension().and_then(|s| s.to_str()).unwrap_or("");
                if ext == "adoc" || ext == "md" {
                    if name.eq_ignore_ascii_case("readme.adoc")
                        || name.eq_ignore_ascii_case("readme.md")
                    {
                        continue;
                    }
                    let title = extract_title_from_file(&path);
                    out.push((title, path));
                }
            }
        }
    }
}

/// Extracts clean human-readable chapter titles, skipping leading 5-block //// comments.
///
/// # Examples
///
/// ```rust
/// use std::path::Path;
/// use tl_tools::guide::extract_title_from_file;
///
/// let title = extract_title_from_file(Path::new("nonexistent.adoc"));
/// assert!(!title.is_empty());
/// ```
pub fn extract_title_from_file(path: &Path) -> String {
    if let Ok(content) = fs::read_to_string(path) {
        let mut in_comment_block = false;
        for line in content.lines().take(120) {
            let trimmed = line.trim();
            if trimmed == "////" {
                in_comment_block = !in_comment_block;
                continue;
            }
            if in_comment_block || trimmed.starts_with("//") || trimmed.is_empty() {
                continue;
            }
            if trimmed.starts_with("= ") && !trimmed.starts_with("== ") {
                return trimmed[2..].trim().to_string();
            } else if trimmed.starts_with("# ") && !trimmed.starts_with("## ") {
                return trimmed[2..].trim().to_string();
            }
        }
    }

    let stem = path.file_stem().and_then(|s| s.to_str()).unwrap_or("Chapter");
    stem.replace('_', " ").replace('-', " ")
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: AsciiDoc Text Manipulation & Attribute Expansion
// -----------------------------------------------------------------------------

/// Strips leading 5-block `////` metadata block and standalone AsciiDoc document header.
///
/// # Examples
///
/// ```rust
/// use tl_tools::guide::strip_adoc_header;
///
/// let raw = "= Title\nAuthor\nv1.0\n\n== Section 1\nBody text";
/// let stripped = strip_adoc_header(raw);
/// assert!(stripped.starts_with("== Section 1"));
/// ```
pub fn strip_adoc_header(content: &str) -> String {
    let all_lines: Vec<&str> = content.lines().collect();
    let mut idx = 0;

    // Pass 1: Skip leading whitespace and leading `//// ... ////` comment blocks
    while idx < all_lines.len() {
        let t = all_lines[idx].trim();
        if t.is_empty() || (t.starts_with("//") && t != "////") {
            idx += 1;
        } else if t == "////" {
            idx += 1;
            while idx < all_lines.len() && all_lines[idx].trim() != "////" {
                idx += 1;
            }
            if idx < all_lines.len() {
                idx += 1;
            }
        } else {
            break;
        }
    }

    // Pass 2: Consume standalone `= Document Title`, author/revision, and `:attr:` lines
    if idx < all_lines.len() {
        let first = all_lines[idx].trim();
        if first.starts_with("= ") && !first.starts_with("== ") {
            idx += 1;
            let mut seen_attr = false;
            let mut meta_lines = 0;
            while idx < all_lines.len() {
                let t = all_lines[idx].trim();
                if t.is_empty() {
                    idx += 1;
                    break;
                }
                if t.starts_with(':') {
                    seen_attr = true;
                    idx += 1;
                } else if !seen_attr && meta_lines < 2 && !t.starts_with("==") && t != "////" {
                    meta_lines += 1;
                    idx += 1;
                } else {
                    break;
                }
            }
            while idx < all_lines.len() {
                let t = all_lines[idx].trim();
                if t.is_empty() || t.starts_with(':') {
                    idx += 1;
                } else {
                    break;
                }
            }
        }
    }

    let remaining = &all_lines[idx..];
    if remaining.is_empty() {
        content.to_string()
    } else {
        remaining.join("\n")
    }
}

/// Recursively inlines `include::path[]` relative to `base_dir` (capped depth = 6).
pub fn expand_includes(base_dir: &Path, content: &str, depth: usize) -> String {
    if depth >= MAX_INCLUDE_DEPTH {
        return content.to_string();
    }
    let mut out = String::with_capacity(content.len());
    let mut active_fence: Option<&'static str> = None;

    for line in content.lines() {
        let t = line.trim();
        if let Some(fence) = active_fence {
            if t == fence {
                active_fence = None;
            }
            out.push_str(line);
            out.push('\n');
            continue;
        }
        if t == "----" || t == "------" || t == "...." || t == "////" {
            active_fence = Some(if t == "------" {
                "------"
            } else if t == "----" {
                "----"
            } else if t == "...." {
                "...."
            } else {
                "////"
            });
            out.push_str(line);
            out.push('\n');
            continue;
        }

        if let Some(rest) = t.strip_prefix("include::") {
            if let Some(bracket_idx) = rest.find('[') {
                let rel = rest[..bracket_idx].trim();
                let target = base_dir.join(rel);
                if target.is_file() {
                    if let Ok(inc_raw) = fs::read_to_string(&target) {
                        let child_dir = target.parent().unwrap_or(base_dir);
                        let expanded = expand_includes(child_dir, &inc_raw, depth + 1);
                        out.push_str(&expanded);
                        if !expanded.ends_with('\n') {
                            out.push('\n');
                        }
                        continue;
                    }
                }
            }
        }
        out.push_str(line);
        out.push('\n');
    }
    out
}

/// Resolves chapter-scoped custom `:key: val` definitions outside code fences.
pub fn resolve_and_strip_custom_attributes(content: &str) -> String {
    let builtins: HashSet<&'static str> = [
        "toc",
        "toclevels",
        "sectnums",
        "sectnumlevels",
        "icons",
        "source-highlighter",
        "doctype",
        "pdf-theme",
        "pdf-themesdir",
        "has-snapshot-appendix",
        "chapter-signifier",
        "appendix-caption",
    ]
    .into_iter()
    .collect();

    let mut attrs: HashMap<String, String> = HashMap::new();
    let mut kept_lines = Vec::new();
    let mut in_fence: Option<&'static str> = None;

    for line in content.lines() {
        let t = line.trim();
        if let Some(f) = in_fence {
            if t == f {
                in_fence = None;
            }
            kept_lines.push(line.to_string());
            continue;
        }
        if matches!(t, "----" | "------" | "...." | "////") {
            in_fence = Some(match t {
                "------" => "------",
                "----" => "----",
                "...." => "....",
                _ => "////",
            });
            kept_lines.push(line.to_string());
            continue;
        }

        if t.starts_with(':') {
            if let Some(end_col) = t[1..].find(':') {
                let key = &t[1..1 + end_col];
                let val = t[2 + end_col..].trim();
                let valid_key = !key.is_empty()
                    && !key.starts_with('!')
                    && !key.ends_with('!')
                    && key.chars().all(|c| c.is_ascii_alphanumeric() || c == '-' || c == '_');
                if valid_key && !builtins.contains(key) && !val.is_empty() {
                    attrs.insert(key.to_string(), val.to_string());
                    continue;
                }
            }
        }
        kept_lines.push(line.to_string());
    }

    let joined = kept_lines.join("\n");
    if attrs.is_empty() {
        return joined;
    }
    let mut res = joined;
    for (k, v) in &attrs {
        res = res.replace(&format!("{{{}}}", k), v);
    }
    res
}

pub fn strip_standalone_macros(content: &str) -> String {
    let mut out = String::with_capacity(content.len());
    let mut in_fence: Option<&'static str> = None;

    for line in content.lines() {
        let t = line.trim();
        if let Some(f) = in_fence {
            if (f == "```" && t.starts_with("```")) || t == f {
                in_fence = None;
            }
            out.push_str(line);
            out.push('\n');
            continue;
        }
        if t.starts_with("```") {
            in_fence = Some("```");
            out.push_str(line);
            out.push('\n');
            continue;
        }
        if matches!(t, "----" | "------" | "...." | "////") {
            in_fence = Some(match t {
                "------" => "------",
                "----" => "----",
                "...." => "....",
                _ => "////",
            });
            out.push_str(line);
            out.push('\n');
            continue;
        }

        if t == "toc::[]" || t == "[abstract]" {
            continue;
        }
        out.push_str(line);
        out.push('\n');
    }
    out
}

/// Shifts AsciiDoc headings by `+shift` ONLY outside delimited blocks.
///
/// # Examples
///
/// ```rust
/// use tl_tools::guide::shift_section_levels;
///
/// let adoc = "== Chapter\n=== Subtopic\n";
/// let shifted = shift_section_levels(adoc, 1, false);
/// assert!(shifted.contains("=== Chapter"));
/// assert!(shifted.contains("==== Subtopic"));
/// ```
pub fn shift_section_levels(content: &str, shift: usize, strip_manual_nums: bool) -> String {
    let mut out = String::with_capacity(content.len() + 256);
    let mut fence_stack: Vec<&'static str> = Vec::new();

    for line in content.lines() {
        let t = line.trim();
        let in_verbatim = fence_stack.iter().any(|f| {
            *f == "----" || *f == "------" || *f == "...." || *f == "////" || *f == "```"
        });

        if let Some(&top) = fence_stack.last() {
            if (top == "|===" && t == "|===")
                || (top == "```" && t.starts_with("```"))
                || (top != "|===" && top != "```" && t == top)
            {
                fence_stack.pop();
                out.push_str(line);
                out.push('\n');
                continue;
            }
        }

        if !in_verbatim {
            let maybe_fence = if t.starts_with("```") {
                Some("```")
            } else {
                match t {
                    "------" => Some("------"),
                    "----" => Some("----"),
                    "...." => Some("...."),
                    "////" => Some("////"),
                    "====" => Some("===="),
                    "****" => Some("****"),
                    "|===" => Some("|==="),
                    "--" => Some("--"),
                    _ => None,
                }
            };
            if let Some(f) = maybe_fence {
                fence_stack.push(f);
                out.push_str(line);
                out.push('\n');
                continue;
            }
        }

        let allow_heading = !in_verbatim && !fence_stack.contains(&"|===");
        if allow_heading {
            let eq_count = line.chars().take_while(|&c| c == '=').count();
            if (2..=5).contains(&eq_count) && line.as_bytes().get(eq_count) == Some(&b' ') {
                let new_level = (eq_count + shift).min(6);
                let rest = line[eq_count + 1..].trim();
                let cleaned = if strip_manual_nums {
                    strip_manual_section_prefix(rest)
                } else {
                    rest
                };
                for _ in 0..new_level {
                    out.push('=');
                }
                out.push(' ');
                out.push_str(cleaned);
                out.push('\n');
                continue;
            }
        }

        out.push_str(line);
        out.push('\n');
    }
    out
}

pub fn strip_manual_section_prefix(heading: &str) -> &str {
    let bytes = heading.as_bytes();
    let mut i = 0;
    let mut saw_dot = false;
    let mut digits_before_dot = 0;

    while i < bytes.len() && bytes[i].is_ascii_digit() {
        digits_before_dot += 1;
        i += 1;
    }
    if digits_before_dot == 0 || digits_before_dot > 2 {
        return heading;
    }
    while i < bytes.len() {
        if bytes[i] == b'.' {
            saw_dot = true;
            i += 1;
            while i < bytes.len() && bytes[i].is_ascii_digit() {
                i += 1;
            }
        } else {
            break;
        }
    }
    if !saw_dot {
        return heading;
    }
    while i < bytes.len() && bytes[i] == b' ' {
        i += 1;
    }
    if i < bytes.len() && bytes[i].is_ascii_alphabetic() {
        &heading[i..]
    } else {
        heading
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: AsciiDoc to Markdown Translation
// -----------------------------------------------------------------------------

/// Converts full AsciiDoc (fences, admonitions, sidebars, comments, tables, callouts) to GFM.
///
/// # Examples
///
/// ```rust
/// use tl_tools::guide::adoc_to_markdown;
///
/// let adoc = "== Architecture\n\n[NOTE]\n====\nImportant note\n====\n";
/// let md = adoc_to_markdown(adoc);
/// assert!(md.contains("## Architecture"));
/// assert!(md.contains("> **[NOTE]**"));
/// ```
pub fn adoc_to_markdown(adoc: &str) -> String {
    let lines: Vec<&str> = adoc.lines().collect();
    let mut md = String::with_capacity(adoc.len());

    let mut in_comment_block = false;
    let mut in_code_block = false;
    let mut code_delim = "";
    let mut in_admonition: Option<String> = None;
    let mut in_sidebar = false;
    let mut in_table = false;
    let mut table_cols_expected: Option<usize> = None;
    let mut table_cells: Vec<String> = Vec::new();
    let mut inferred_cols_from_first_group: Option<usize> = None;

    let mut pending_attr: Option<String> = None;
    let mut pending_caption: Option<String> = None;

    let mut i = 0;
    while i < lines.len() {
        let line = lines[i];
        let trimmed = line.trim();

        if in_comment_block {
            if trimmed == "////" {
                in_comment_block = false;
            }
            i += 1;
            continue;
        }

        if in_code_block {
            if (code_delim == "```" && trimmed.starts_with("```")) || trimmed == code_delim {
                in_code_block = false;
                code_delim = "";
                md.push_str(if in_admonition.is_some() || in_sidebar {
                    "> ```\n"
                } else {
                    "```\n"
                });
            } else {
                if in_admonition.is_some() || in_sidebar {
                    md.push_str("> ");
                }
                md.push_str(line);
                md.push('\n');
            }
            i += 1;
            continue;
        }

        if trimmed.starts_with("```") {
            in_code_block = true;
            code_delim = "```";
            pending_attr = None;
            pending_caption = None;
            if in_admonition.is_some() || in_sidebar {
                md.push_str(&format!("> {}\n", trimmed));
            } else {
                md.push_str(&format!("{}\n", trimmed));
            }
            i += 1;
            continue;
        }

        if trimmed == "////" {
            in_comment_block = true;
            pending_attr = None;
            pending_caption = None;
            i += 1;
            continue;
        }

        if trimmed == "//" || trimmed.starts_with("// ") || trimmed.starts_with("///") {
            i += 1;
            continue;
        }

        if trimmed == "----" || trimmed == "------" || trimmed == "...." {
            if let Some(cap) = pending_caption.take() {
                md.push_str(&format!("**{}**\n\n", cap));
            }
            let lang = pending_attr
                .take()
                .and_then(|attr| extract_source_lang(&attr))
                .unwrap_or_default();
            in_code_block = true;
            code_delim = if trimmed == "------" {
                "------"
            } else if trimmed == "----" {
                "----"
            } else {
                "...."
            };
            if in_admonition.is_some() || in_sidebar {
                md.push_str(&format!("> ```{}\n", lang));
            } else {
                md.push_str(&format!("```{}\n", lang));
            }
            i += 1;
            continue;
        }

        if trimmed == "====" {
            if in_admonition.is_some() {
                in_admonition = None;
                md.push('\n');
            } else {
                let kind = pending_attr
                    .take()
                    .and_then(|a| extract_admonition_kind(&a))
                    .unwrap_or_else(|| "NOTE".to_string());
                let title_suffix = pending_caption
                    .take()
                    .map(|c| format!(": {}", c))
                    .unwrap_or_default();
                md.push_str(&format!("> **[{}]**{}\n> \n", kind, title_suffix));
                in_admonition = Some(kind);
            }
            i += 1;
            continue;
        }

        if trimmed == "****" {
            if in_sidebar {
                in_sidebar = false;
                md.push('\n');
            } else {
                in_sidebar = true;
                let title = pending_caption
                    .take()
                    .unwrap_or_else(|| "Sidebar".to_string());
                pending_attr = None;
                md.push_str(&format!("> **{}**\n> \n", title));
            }
            i += 1;
            continue;
        }

        if trimmed == "|===" {
            if in_table {
                let cols = table_cols_expected
                    .or(inferred_cols_from_first_group)
                    .unwrap_or(1)
                    .max(1);
                flush_markdown_table(&mut md, &table_cells, cols);
                md.push('\n');
                in_table = false;
                table_cols_expected = None;
                inferred_cols_from_first_group = None;
                table_cells.clear();
            } else {
                if let Some(cap) = pending_caption.take() {
                    md.push_str(&format!("**{}**\n\n", cap));
                }
                let attr = pending_attr.take().unwrap_or_default();
                table_cols_expected = extract_table_col_count(&attr);
                inferred_cols_from_first_group = None;
                table_cells.clear();
                in_table = true;
            }
            i += 1;
            continue;
        }

        if in_table {
            if trimmed.is_empty() {
                if inferred_cols_from_first_group.is_none() && !table_cells.is_empty() {
                    inferred_cols_from_first_group = Some(table_cells.len());
                }
                i += 1;
                continue;
            }
            if trimmed == "a|" || trimmed.starts_with("a| ") {
                let text = trimmed[2..].trim().to_string();
                table_cells.push(text);
            } else if let Some(rest) = trimmed.strip_prefix('|') {
                let normalized = rest.replace(" a|", "|").replace("|a|", "|");
                let parts: Vec<&str> = normalized.split('|').collect();
                let push_count = if parts.len() > 1 && normalized.trim_end().ends_with('|') {
                    parts.len() - 1
                } else {
                    parts.len()
                };
                for cell in parts.into_iter().take(push_count) {
                    table_cells.push(cell.trim().to_string());
                }
                if push_count > 1 && inferred_cols_from_first_group.is_none() {
                    inferred_cols_from_first_group = Some(push_count);
                }
            } else if let Some(last) = table_cells.last_mut() {
                let piece = if let Some(bullet) = trimmed.strip_prefix("* ") {
                    format!("• {}", bullet.trim())
                } else {
                    trimmed.to_string()
                };
                if last.is_empty() {
                    *last = piece;
                } else {
                    last.push_str("<br>");
                    last.push_str(&piece);
                }
            }
            i += 1;
            continue;
        }

        if trimmed == "--"
            || trimmed == "toc::[]"
            || trimmed.starts_with("[[") && trimmed.ends_with("]]")
        {
            i += 1;
            continue;
        }
        if trimmed.starts_with(':') && !trimmed.starts_with("::") {
            i += 1;
            continue;
        }
        if trimmed.starts_with('[') && trimmed.ends_with(']') && !trimmed.contains("](") {
            pending_attr = Some(trimmed.to_string());
            i += 1;
            continue;
        }
        if trimmed.starts_with('.') && trimmed.len() > 1 && !trimmed.starts_with("..") {
            let next_is_block = lines
                .get(i + 1)
                .map(|nl| {
                    let nt = nl.trim();
                    nt.starts_with('[')
                        || matches!(nt, "----" | "...." | "====" | "****" | "|===")
                })
                .unwrap_or(false);
            if next_is_block {
                pending_caption = Some(trimmed[1..].trim().to_string());
                i += 1;
                continue;
            }
        }

        let processed_line = unwrap_snapshot_ifdef(line);
        let pt = processed_line.trim();

        if in_admonition.is_some() || in_sidebar {
            if pt.is_empty() {
                md.push_str(">\n");
            } else {
                md.push_str(&format!("> {}\n", convert_inline_xrefs(&processed_line)));
            }
            i += 1;
            continue;
        }

        if let Some((tag, rest)) = split_inline_admonition(pt) {
            md.push_str(&format!("> **{}:** {}\n", tag, convert_inline_xrefs(rest)));
            i += 1;
            continue;
        }
        if pt.ends_with("::") && pt.len() > 2 && !pt.starts_with(':') {
            let term = pt[..pt.len() - 2].trim();
            md.push_str(&format!("**{}**  \n", convert_inline_xrefs(term)));
            i += 1;
            continue;
        }

        pending_attr = None;
        pending_caption = None;
        let converted = if let Some(h) = pt.strip_prefix("====== ") {
            format!("###### {}", h.trim())
        } else if let Some(h) = pt.strip_prefix("===== ") {
            format!("##### {}", h.trim())
        } else if let Some(h) = pt.strip_prefix("==== ") {
            format!("#### {}", h.trim())
        } else if let Some(h) = pt.strip_prefix("=== ") {
            format!("### {}", h.trim())
        } else if let Some(h) = pt.strip_prefix("== ") {
            format!("## {}", h.trim())
        } else if let Some(h) = pt.strip_prefix("= ") {
            format!("# {}", h.trim())
        } else {
            convert_inline_xrefs(&processed_line)
        };

        md.push_str(&converted);
        md.push('\n');
        i += 1;
    }

    md
}

fn extract_source_lang(attr: &str) -> Option<String> {
    let inner = attr.trim_start_matches('[').trim_end_matches(']');
    let parts: Vec<&str> = inner.split(',').collect();
    if parts.first().map(|s| s.trim()) == Some("source") {
        let lang = parts.get(1).map(|s| s.trim()).unwrap_or("");
        if !lang.is_empty() && !lang.starts_with('%') && !lang.contains('=') {
            return Some(lang.to_string());
        }
    }
    None
}

fn extract_admonition_kind(attr: &str) -> Option<String> {
    let inner = attr.trim_start_matches('[').trim_end_matches(']').trim();
    let upper = inner.to_ascii_uppercase();
    for kind in ["NOTE", "TIP", "IMPORTANT", "WARNING", "CAUTION"] {
        if upper == kind || upper.starts_with(&format!("{},", kind)) {
            return Some(kind.to_string());
        }
    }
    None
}

fn flush_markdown_table(md: &mut String, cells: &[String], cols: usize) {
    if cells.is_empty() {
        return;
    }
    for (row_idx, chunk) in cells.chunks(cols).enumerate() {
        let mut row: Vec<String> = chunk.to_vec();
        while row.len() < cols {
            row.push(String::new());
        }
        md.push_str(&format!("| {} |\n", row.join(" | ")));
        if row_idx == 0 {
            let sep: Vec<&str> = std::iter::repeat("---").take(cols).collect();
            md.push_str(&format!("| {} |\n", sep.join(" | ")));
        }
    }
}

fn extract_table_col_count(attr: &str) -> Option<usize> {
    let pos = attr.find("cols=")?;
    let rest = &attr[pos + 5..];
    let quoted = rest.trim_start_matches('"');
    let spec = quoted.split('"').next()?;
    if let Some(star) = spec.strip_suffix('*') {
        if let Ok(n) = star.trim().parse::<usize>() {
            if n > 0 {
                return Some(n);
            }
        }
    }
    let count = spec.split(',').filter(|s| !s.trim().is_empty()).count();
    if count > 0 {
        Some(count)
    } else {
        None
    }
}

fn split_inline_admonition(line: &str) -> Option<(&'static str, &str)> {
    for tag in ["NOTE", "TIP", "IMPORTANT", "WARNING", "CAUTION"] {
        let prefix = format!("{}: ", tag);
        if let Some(rest) = line.strip_prefix(&prefix) {
            return Some((tag, rest));
        }
    }
    None
}

fn unwrap_snapshot_ifdef(line: &str) -> String {
    let t = line.trim();
    if let Some(rest) = t.strip_prefix("ifdef::has-snapshot-appendix[") {
        if let Some(inner) = rest.strip_suffix(']') {
            return inner.to_string();
        }
    }
    line.to_string()
}

fn convert_inline_xrefs(line: &str) -> String {
    let mut out = String::with_capacity(line.len());
    let mut rest = line;
    while let Some(start) = rest.find("<<") {
        out.push_str(&rest[..start]);
        let after_open = &rest[start + 2..];
        if let Some(end) = after_open.find(">>") {
            let inner = &after_open[..end];
            if let Some((anchor, label)) = inner.split_once(',') {
                out.push_str(&format!("[{}](#{})", label.trim(), anchor.trim()));
            } else {
                out.push_str(&format!("[{}](#{})", inner.trim(), inner.trim()));
            }
            rest = &after_open[end + 2..];
        } else {
            out.push_str("<<");
            rest = after_open;
        }
    }
    out.push_str(rest);
    out
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: Whole-Book Assembly Engine
// -----------------------------------------------------------------------------

pub fn push_book_header(config: &GuideConfig, adoc: &mut String, title: &str) {
    let p = &config.publication;
    adoc.push_str(&format!("= {}\n", title));
    adoc.push_str(&format!("{}\n", p.author));
    adoc.push_str(&format!("v{}, {}\n", p.revnumber, p.revdate));
    adoc.push_str(&format!(":doctype: {}\n", p.doctype));
    adoc.push_str(&format!(":toc: {}\n", p.toc));
    adoc.push_str(&format!(":toclevels: {}\n", config.toc_levels));
    adoc.push_str(&format!(":sectnumlevels: {}\n", config.sectnum_levels));
    adoc.push_str(&format!(":icons: {}\n", p.icons));
    adoc.push_str(&format!(":source-highlighter: {}\n", p.source_highlighter));
    adoc.push_str(":has-snapshot-appendix:\n\n");
}

pub fn prepare_chapter_body(
    docs_dir: &Path,
    config: &GuideConfig,
    ch: &ChapterSpec,
    snapshots: &[RenderedSnapshot],
    referenced_snap_ids: &mut HashSet<String>,
) -> String {
    let raw_content = if ch.source_path.exists() {
        fs::read_to_string(&ch.source_path)
            .unwrap_or_else(|_| format!("Chapter content unavailable from {:?}", ch.source_path))
    } else {
        format!("_File not found: {:?}_\n", ch.source_path)
    };

    let mut body = strip_adoc_header(&raw_content);

    if config.publication.expand_custom_attributes {
        body = resolve_and_strip_custom_attributes(&body);
    }
    if config.publication.expand_includes {
        let base_dir = ch.source_path.parent().unwrap_or(docs_dir);
        body = expand_includes(base_dir, &body, 0);
    }

    for snap in snapshots {
        let inc_token = format!("{}.adoc", snap.spec.id);
        let plate_token = format!("snap_plate_{}", snap.spec.id);
        let ws_token = format!("snap_wholesale_{}", snap.spec.id);
        if raw_content.contains(&inc_token)
            || raw_content.contains(&ws_token)
            || body.contains(&plate_token)
            || body.contains(&ws_token)
        {
            referenced_snap_ids.insert(snap.spec.id.clone());
        }
    }

    if config.publication.shift_chapter_headings {
        body = shift_section_levels(
            &body,
            1,
            config.publication.strip_manual_heading_numbers,
        );
    }
    strip_standalone_macros(&body)
}

pub fn append_wholesale_appendix(
    config: &GuideConfig,
    adoc: &mut String,
    md: &mut String,
    snapshots: &[RenderedSnapshot],
    referenced_ids: &HashSet<String>,
    vol_override: Option<bool>,
) {
    if !config.snapshots.include_wholesale_appendix || vol_override == Some(false) {
        return;
    }
    let entries: Vec<&RenderedSnapshot> = snapshots
        .iter()
        .filter(|s| {
            referenced_ids.contains(&s.spec.id)
                && (snapshot_goes_in_appendix(s) || vol_override == Some(true))
        })
        .collect();

    if entries.is_empty() {
        return;
    }

    let title = &config.snapshots.appendix_title;
    adoc.push_str(&format!(
        "[appendix]\n[[appendix_code_snapshots]]\n== {}\n\n",
        title
    ));
    adoc.push_str(
        "Complete verbatim reference plates for source files, blocks, sub-assemblies, \
         and function groups excerpted in preceding chapters.\n\n",
    );

    let mut app_md_adoc = String::new();
    for snap in entries {
        let plate = format_wholesale_appendix_entry(snap, config.snapshots.show_line_numbers);
        adoc.push_str(&plate);
        app_md_adoc.push_str(&plate);
    }

    md.push_str(&format!("## Appendix: {}\n\n", title));
    md.push_str(&adoc_to_markdown(&app_md_adoc));
    md.push_str("\n---\n\n");
}

pub fn assemble_content_with_snapshots(
    repo_root: &Path,
    docs_dir: &Path,
    volumes: &[VolumeSpec],
    config: &GuideConfig,
    snapshots: &[RenderedSnapshot],
) -> Result<(String, String), String> {
    let mut adoc = String::with_capacity(400_000);
    let mut md = String::with_capacity(400_000);

    push_book_header(config, &mut adoc, &config.publication.master_title);
    adoc.push_str(":sectnums!:\n\n");
    adoc.push_str("[preface]\n== Executive Summary\n\n");
    adoc.push_str(
        "This Master Guide serves as the definitive single source of truth for *Tenebris and Lux*.\n\
         It unites world canon, character dossiers, ludology, the 2017–2018 demo baseline,\n\
         software architecture, and engineering dev logs into a single textbook compendium.\n\n",
    );
    adoc.push_str("[cols=\"1,3,3\",options=\"header\"]\n|===\n| Volume | Title | Scope\n");
    for vol in volumes {
        adoc.push_str(&format!(
            "| Volume {} | {} ({} ch.) | {}\n",
            vol.roman,
            vol.title,
            vol.chapters.len(),
            vol.description
        ));
    }
    adoc.push_str("|===\n\n");
    adoc.push_str(":sectnums:\n\n");

    md.push_str(&format!("# {}\n\n", config.publication.master_title));
    md.push_str(&format!("> **Author**: {}  \n", config.publication.author));
    md.push_str(&format!(
        "> **Version**: {} ({})  \n",
        config.publication.revnumber, config.publication.revdate
    ));
    md.push_str(
        "> **Standard**: OmniCode 5-Block Ontological Architecture & Layered Invariants  \n",
    );
    md.push_str(
        "> **Purpose**: Unified Single-File Master Compendium for Google Gemini and NotebookLM\n\n",
    );
    md.push_str("## Executive Summary\n\n");
    md.push_str(
        "This Master Guide compiles the entire architectural, ludological, and narrative universe of **Tenebris and Lux**.\n\n",
    );

    md.push_str("## Table of Contents\n\n");
    for vol in volumes {
        md.push_str(&format!("- **Volume {}: {}**\n", vol.roman, vol.title));
        for ch in &vol.chapters {
            md.push_str(&format!("  - Chapter {}: {}\n", ch.number, ch.title));
        }
    }
    md.push_str("\n---\n\n");

    let mut referenced_ids = HashSet::new();

    for vol in volumes {
        adoc.push_str(&format!("= Volume {}: {}\n\n", vol.roman, vol.title));
        adoc.push_str(&format!("{}\n\n", vol.description));

        md.push_str(&format!("# Volume {}: {}\n\n", vol.roman, vol.title));
        md.push_str(&format!("_{}_\n\n", vol.description));

        for ch in &vol.chapters {
            let processed =
                prepare_chapter_body(docs_dir, config, ch, snapshots, &mut referenced_ids);
            let rel_src = ch.source_path.strip_prefix(repo_root).unwrap_or(&ch.source_path);

            adoc.push_str(&format!("== {}\n\n", ch.title));
            adoc.push_str(&format!("*Source File*: `{}`\n\n", rel_src.display()));
            adoc.push_str(&processed);
            adoc.push_str("\n\n");

            md.push_str(&format!("## Chapter {}: {}\n\n", ch.number, ch.title));
            md.push_str(&format!("> **Source**: `{}`\n\n", rel_src.display()));
            md.push_str(&adoc_to_markdown(&processed));
            md.push_str("\n\n---\n\n");
        }
    }

    append_wholesale_appendix(config, &mut adoc, &mut md, snapshots, &referenced_ids, None);
    Ok((deduplicate_snapshot_plate_anchors(&adoc), md))
}

pub fn assemble_volume_with_snapshots(
    repo_root: &Path,
    docs_dir: &Path,
    vol: &VolumeSpec,
    config: &GuideConfig,
    snapshots: &[RenderedSnapshot],
) -> Result<(String, String), String> {
    let mut adoc = String::with_capacity(100_000);
    let mut md = String::with_capacity(100_000);

    push_book_header(
        config,
        &mut adoc,
        &format!("Volume {}: {}", vol.roman, vol.title),
    );
    adoc.push_str(":sectnums:\n\n");
    adoc.push_str(&format!(
        "[NOTE]\n.Volume {} Synopsis\n====\n{}\n====\n\n",
        vol.roman, vol.description
    ));

    md.push_str(&format!("# Volume {}: {}\n\n", vol.roman, vol.title));
    md.push_str(&format!("> **Author**: {}  \n", config.publication.author));
    md.push_str(&format!(
        "> **Compendium**: Tenebris and Lux Master Guide — Volume {} ({})  \n",
        vol.roman, config.publication.revnumber
    ));
    md.push_str(
        "> **Purpose**: Context Document for Google Gemini & NotebookLM Workflows\n\n",
    );
    md.push_str("## Volume Synopsis\n\n");
    md.push_str(&format!("_{}_\n\n", vol.description));

    md.push_str("## Table of Contents\n\n");
    for (idx, ch) in vol.chapters.iter().enumerate() {
        md.push_str(&format!("{}. {}\n", idx + 1, ch.title));
    }
    md.push_str("\n---\n\n");

    let mut referenced_ids = HashSet::new();

    for (idx, ch) in vol.chapters.iter().enumerate() {
        let processed =
            prepare_chapter_body(docs_dir, config, ch, snapshots, &mut referenced_ids);
        let rel_src = ch.source_path.strip_prefix(repo_root).unwrap_or(&ch.source_path);

        adoc.push_str(&format!("== {}\n\n", ch.title));
        adoc.push_str(&format!("*Source File*: `{}`\n\n", rel_src.display()));
        adoc.push_str(&processed);
        adoc.push_str("\n\n");

        md.push_str(&format!("## Chapter {}: {}\n\n", idx + 1, ch.title));
        md.push_str(&format!("> **Source**: `{}`\n\n", rel_src.display()));
        md.push_str(&adoc_to_markdown(&processed));
        md.push_str("\n\n---\n\n");
    }

    let vol_override = config.volume_appendix_overrides.get(&vol.roman).copied();
    append_wholesale_appendix(
        config,
        &mut adoc,
        &mut md,
        snapshots,
        &referenced_ids,
        vol_override,
    );
    Ok((deduplicate_snapshot_plate_anchors(&adoc), md))
}

pub fn deduplicate_snapshot_plate_anchors(adoc: &str) -> String {
    let mut out = String::with_capacity(adoc.len());
    let mut counts: HashMap<String, usize> = HashMap::new();
    let mut active_fence: Option<&'static str> = None;

    for line in adoc.lines() {
        let t = line.trim();
        if let Some(fence) = active_fence {
            if t == fence {
                active_fence = None;
            }
            out.push_str(line);
            out.push('\n');
            continue;
        }
        if matches!(t, "----" | "------" | "...." | "////") {
            active_fence = Some(match t {
                "------" => "------",
                "----" => "----",
                "...." => "....",
                _ => "////",
            });
            out.push_str(line);
            out.push('\n');
            continue;
        }

        if t.starts_with("[[snap_plate_") && t.ends_with("]]") {
            let id = &t[2..t.len() - 2];
            let entry = counts.entry(id.to_string()).or_insert(0);
            *entry += 1;
            if *entry == 1 {
                out.push_str(line);
            } else {
                out.push_str(&format!("[[{}_ref{}]]", id, *entry));
            }
        } else {
            out.push_str(line);
        }
        out.push('\n');
    }
    out
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
    fn test_section_1_nominal_strip_adoc_header() {
        let legacy = "= Document Title\nAuthor\nv1.0\n:toc: left\n\n== Section 1\nContent";
        let stripped = strip_adoc_header(legacy);
        assert!(stripped.starts_with("== Section 1"));
    }

    #[test]
    fn test_section_1_nominal_shift_section_levels() {
        let adoc = "== 1. Intro\n=== 1.1 Subtopic\n";
        let shifted = shift_section_levels(adoc, 1, true);
        assert!(shifted.contains("=== Intro\n"));
        assert!(shifted.contains("==== Subtopic\n"));
    }

    #[test]
    fn test_section_1_nominal_adoc_to_markdown_translation() {
        let adoc = "== Architecture\n\n[NOTE]\n====\nZero heap in layers 0-2!\n====\n";
        let md = adoc_to_markdown(adoc);
        assert!(md.contains("## Architecture"));
        assert!(md.contains("> **[NOTE]**"));
        assert!(md.contains("Zero heap in layers 0-2!"));
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_empty_input_handling() {
        assert_eq!(strip_adoc_header(""), "");
        assert_eq!(shift_section_levels("", 1, false), "");
        assert_eq!(adoc_to_markdown(""), "");
    }

    #[test]
    fn test_section_2_never_fail_fence_preservation() {
        let adoc = "[source,c]\n----\n== Not a real heading\n----\n";
        let shifted = shift_section_levels(adoc, 1, false);
        assert!(shifted.contains("== Not a real heading\n"));
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_max_include_depth() {
        assert_eq!(MAX_INCLUDE_DEPTH, 6);
    }

    #[test]
    fn test_section_3_invariants_fence_delimiter_predicate() {
        assert!(is_fence_delimiter("----"));
        assert!(is_fence_delimiter("------"));
        assert!(is_fence_delimiter("===="));
        assert!(is_fence_delimiter("|==="));
        assert!(is_fence_delimiter("```rust"));
        assert!(!is_fence_delimiter("== Section 1"));
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_whole_book_assembly() {
        let temp_dir = std::env::temp_dir();
        let docs_dir = temp_dir.join("test_docs_assembly");
        fs::create_dir_all(&docs_dir).expect("Create docs dir");

        let ch1_path = docs_dir.join("ch1.adoc");
        fs::write(&ch1_path, "== First Chapter\n\nBody content of chapter 1.").expect("Write ch1");

        let volumes = vec![VolumeSpec {
            roman: "I".to_string(),
            slug: "test_vol".to_string(),
            title: "Test Volume".to_string(),
            description: "Testing whole book assembly".to_string(),
            scan_areas: vec![],
            chapters: vec![ChapterSpec {
                number: 1,
                title: "First Chapter".to_string(),
                source_path: ch1_path,
            }],
        }];

        let config = crate::guide::default_config(&temp_dir);
        let snapshots = vec![];

        let res = assemble_content_with_snapshots(
            &temp_dir,
            &docs_dir,
            &volumes,
            &config,
            &snapshots,
        );
        assert!(res.is_ok());
        let (adoc, md) = res.unwrap();
        assert!(adoc.contains("= Tenebris and Lux: Comprehensive Master Guide"));
        assert!(adoc.contains("== Executive Summary"));
        assert!(md.contains("# Tenebris and Lux: Comprehensive Master Guide"));
        assert!(md.contains("## Chapter 1: First Chapter"));

        let _ = fs::remove_dir_all(docs_dir);
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Diagnostic Self-Audit
// -----------------------------------------------------------------------------

/// Performs an in-process diagnostic preflight self-audit of compiler invariants.
///
/// # Examples
///
/// ```rust
/// use tl_tools::guide::compiler::self_audit;
///
/// assert!(self_audit().is_ok());
/// ```
pub fn self_audit() -> Result<(), String> {
    let test_input = "= Title\n\n== Section\nBody";
    let stripped = strip_adoc_header(test_input);
    if !stripped.starts_with("== Section") {
        return Err("strip_adoc_header self-audit failed".to_string());
    }
    let shifted = shift_section_levels("== Title\n", 1, false);
    if !shifted.contains("=== Title") {
        return Err("shift_section_levels self-audit failed".to_string());
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

/// Verifies that the compiler sub-assembly is ready for operation.
///
/// # Examples
///
/// ```rust
/// use tl_tools::guide::compiler::is_compiler_subsystem_ready;
///
/// assert!(is_compiler_subsystem_ready());
/// ```
pub fn is_compiler_subsystem_ready() -> bool {
    self_audit().is_ok()
}
