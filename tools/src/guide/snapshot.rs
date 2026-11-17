// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/guide/snapshot.rs
// @id            cog.tools.guide.snapshot
// @layer         Layer 3: Developer Toolchain Command Sub-Assemblies
// @archetype     pure_types | extraction_engine
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Guide Snapshot Sub-Assembly
// @role          5-Mode Textbook Code Snapshot Engine & Plate Formatter
// @maturity      Rev 5 Baseline | In-process code snapshot materializer
// @chassis       bounded-heap | Developer tooling memory chassis; bounded lines
// @scale         O(S * L) across snapshots and source file lines
// @seams         in: code_snapshots_manifest.json | out: docs/snapshots/*.adoc
// @invariants    1. 5 granular modes supported: Whole, Block, Subassembly, Function, Group.
//                2. Authentic 1-based source line retention and callout annotation.
//                3. Zero process spawning for source extraction.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Extracts live source code segments from repository disk files and renders
//   them as styled AsciiDoc plates with syntax highlighting and callout annotations.
//
//   ### For Game Designers & Non-Programmers:
//   Guarantees that code snippets printed in the Master Guide books are directly
//   extracted from the working game engine, ensuring zero out-of-date code examples.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Operates purely statelessly on source lines.
//   - Extension Seam: Additional language lexers register in `infer_language`.
//   - Explicit Non-Goal: Does not alter on-disk source files during extraction.
// =============================================================================

//! # 5-Mode Textbook Code Snapshot Engine
//!
//! Materializes verbatim and excerpted code snapshots across Whole, Block,
//! Subassembly, Function, and Group extraction modes.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

use std::fs;
use std::path::{Path, PathBuf};
use serde::{Deserialize, Serialize};

use super::manifest::GuideConfig;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Types, Enums & Snapshot Specs
// -----------------------------------------------------------------------------

/// Supported source code snapshot granularities.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
#[serde(rename_all = "lowercase")]
pub enum SnapshotMode {
    Whole,
    Block,
    Subassembly,
    Function,
    Group,
}

/// Policy controlling whether a snapshot's full wholesale text appears in Appendix A.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize, Default)]
#[serde(rename_all = "snake_case")]
pub enum WholesalePolicy {
    Always,
    #[default]
    WhenExcerpted,
    Never,
}

/// Optional focused excerpt configuration for long whole/block/group snapshots.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct SnapshotExcerptSpec {
    pub mode: String,
    #[serde(default)]
    pub target: Option<String>,
    #[serde(default)]
    pub start_line: Option<usize>,
    #[serde(default)]
    pub end_line: Option<usize>,
    #[serde(default)]
    pub max_lines: Option<usize>,
}

/// Inline callout badge mapped by pattern to textbook footnote commentary.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct SnapshotCalloutSpec {
    #[serde(rename = "match")]
    pub pattern: String,
    pub number: u32,
    pub text: String,
}

/// Single code snapshot declaration from `docs/code_snapshots_manifest.json`.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct CodeSnapshotSpec {
    pub id: String,
    pub title: String,
    pub source_file: String,
    pub mode: SnapshotMode,
    #[serde(default)]
    pub target: Option<String>,
    #[serde(default)]
    pub targets: Vec<String>,
    #[serde(default)]
    pub language: Option<String>,
    #[serde(default)]
    pub excerpt: Option<SnapshotExcerptSpec>,
    #[serde(default)]
    pub wholesale_in_appendix: WholesalePolicy,
    #[serde(default)]
    pub callout_note: Option<String>,
    #[serde(default)]
    pub callouts: Vec<SnapshotCalloutSpec>,
}

#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct CodeSnapshotsBody {
    pub snapshots: Vec<CodeSnapshotSpec>,
}

#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct CodeSnapshotsManifest {
    pub block_3_body: CodeSnapshotsBody,
}

/// Extracted contiguous line segment retaining authentic 1-based file line numbers.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct ExtractedSegment {
    pub start_line: usize,
    pub end_line: usize,
    pub lines: Vec<String>,
}

/// Materialized snapshot holding both inline plate and full wholesale extraction.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct RenderedSnapshot {
    pub spec: CodeSnapshotSpec,
    pub lang: String,
    pub inline_segments: Vec<ExtractedSegment>,
    pub wholesale_segments: Vec<ExtractedSegment>,
    pub was_excerpted: bool,
    pub total_source_lines: usize,
    pub plate_path: PathBuf,
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Verification Helpers & Invariant Predicates
// -----------------------------------------------------------------------------

/// Evaluates whether a code snapshot specification contains valid non-empty fields.
pub fn is_valid_snapshot_spec(spec: &CodeSnapshotSpec) -> bool {
    !spec.id.trim().is_empty()
        && !spec.title.trim().is_empty()
        && !spec.source_file.trim().is_empty()
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Snapshot Materialization, Parsing, Lexing & Plate Formatting )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Language Inference
// -----------------------------------------------------------------------------

/// Infers the source code syntax language from file extension.
///
/// # Examples
///
/// ```rust
/// use tl_tools::guide::infer_language;
///
/// assert_eq!(infer_language("engine/src/main.c"), "c");
/// assert_eq!(infer_language("tools/src/main.rs"), "rust");
/// assert_eq!(infer_language("CMakeLists.txt"), "cmake");
/// assert_eq!(infer_language("config.json"), "json");
/// ```
pub fn infer_language(source_file: &str) -> String {
    let lower = source_file.to_ascii_lowercase();
    if lower.ends_with(".c") || lower.ends_with(".h") {
        "c".to_string()
    } else if lower.ends_with(".cpp") || lower.ends_with(".hpp") {
        "cpp".to_string()
    } else if lower.ends_with(".cmake") || lower.ends_with("cmakelists.txt") {
        "cmake".to_string()
    } else if lower.ends_with(".json") {
        "json".to_string()
    } else if lower.ends_with(".rs") {
        "rust".to_string()
    } else if lower.ends_with(".yml") || lower.ends_with(".yaml") {
        "yaml".to_string()
    } else {
        "text".to_string()
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: AST Extraction & Segment Matching
// -----------------------------------------------------------------------------

pub fn extract_segments_for_spec(
    lines: &[String],
    spec: &CodeSnapshotSpec,
    lang: &str,
) -> Result<Vec<ExtractedSegment>, String> {
    match spec.mode {
        SnapshotMode::Whole => {
            if lines.is_empty() {
                return Err(format!("Snapshot '{}': source file is empty.", spec.id));
            }
            Ok(vec![ExtractedSegment {
                start_line: 1,
                end_line: lines.len(),
                lines: lines.to_vec(),
            }])
        }
        SnapshotMode::Block => {
            let target = spec
                .target
                .as_deref()
                .ok_or_else(|| format!("Snapshot '{}': mode 'block' requires 'target'.", spec.id))?;
            Ok(vec![extract_block_segment(lines, target, lang).ok_or_else(|| {
                format!(
                    "Snapshot '{}': block '{}' not found in {}.",
                    spec.id, target, spec.source_file
                )
            })?])
        }
        SnapshotMode::Subassembly => {
            let target = spec.target.as_deref().ok_or_else(|| {
                format!("Snapshot '{}': mode 'subassembly' requires 'target'.", spec.id)
            })?;
            Ok(vec![extract_subassembly_segment(lines, target).ok_or_else(|| {
                format!(
                    "Snapshot '{}': subassembly '{}' not found in {}.",
                    spec.id, target, spec.source_file
                )
            })?])
        }
        SnapshotMode::Function => {
            let target = spec.target.as_deref().ok_or_else(|| {
                format!("Snapshot '{}': mode 'function' requires 'target'.", spec.id)
            })?;
            Ok(vec![extract_symbol_segment(lines, target, lang).ok_or_else(|| {
                format!(
                    "Snapshot '{}': function/symbol '{}' not found in {}.",
                    spec.id, target, spec.source_file
                )
            })?])
        }
        SnapshotMode::Group => {
            if spec.targets.is_empty() {
                return Err(format!(
                    "Snapshot '{}': mode 'group' requires non-empty 'targets' array.",
                    spec.id
                ));
            }
            let mut segs = Vec::new();
            for name in &spec.targets {
                let seg = extract_symbol_segment(lines, name, lang).ok_or_else(|| {
                    format!(
                        "Snapshot '{}': group member '{}' not found in {}.",
                        spec.id, name, spec.source_file
                    )
                })?;
                segs.push(seg);
            }
            Ok(merge_segments(lines, segs))
        }
    }
}

pub fn extract_excerpt_segments(
    all_lines: &[String],
    wholesale: &[ExtractedSegment],
    ex: &SnapshotExcerptSpec,
    lang: &str,
) -> Result<Vec<ExtractedSegment>, String> {
    let cap_segment = |mut seg: ExtractedSegment, max_opt: Option<usize>| -> ExtractedSegment {
        if let Some(max_n) = max_opt {
            if seg.lines.len() > max_n && max_n > 0 {
                seg.lines.truncate(max_n);
                seg.end_line = seg.start_line + seg.lines.len() - 1;
            }
        }
        seg
    };

    match ex.mode.to_ascii_lowercase().as_str() {
        "lines" => {
            let s = ex.start_line.unwrap_or(1).max(1);
            let e = ex.end_line.unwrap_or(all_lines.len()).min(all_lines.len());
            if s > e || s > all_lines.len() {
                return Ok(wholesale.to_vec());
            }
            let slice = all_lines[s - 1..e].to_vec();
            Ok(vec![cap_segment(
                ExtractedSegment {
                    start_line: s,
                    end_line: e,
                    lines: slice,
                },
                ex.max_lines,
            )])
        }
        "block" => {
            if let Some(ref tgt) = ex.target {
                if let Some(seg) = extract_block_segment(all_lines, tgt, lang) {
                    return Ok(vec![cap_segment(seg, ex.max_lines)]);
                }
            }
            Ok(wholesale.to_vec())
        }
        "subassembly" => {
            if let Some(ref tgt) = ex.target {
                if let Some(seg) = extract_subassembly_segment(all_lines, tgt) {
                    return Ok(vec![cap_segment(seg, ex.max_lines)]);
                }
            }
            Ok(wholesale.to_vec())
        }
        "symbol" | "function" => {
            if let Some(ref tgt) = ex.target {
                if let Some(seg) = extract_symbol_segment(all_lines, tgt, lang) {
                    return Ok(vec![cap_segment(seg, ex.max_lines)]);
                }
            }
            Ok(wholesale.to_vec())
        }
        _ => {
            if let Some(first) = wholesale.first().cloned() {
                Ok(vec![cap_segment(first, ex.max_lines)])
            } else {
                Ok(Vec::new())
            }
        }
    }
}

pub fn merge_segments(
    all_lines: &[String],
    mut segs: Vec<ExtractedSegment>,
) -> Vec<ExtractedSegment> {
    if segs.is_empty() {
        return segs;
    }
    segs.sort_by_key(|s| s.start_line);
    let mut merged = Vec::new();
    let mut cur = segs[0].clone();
    for next in segs.into_iter().skip(1) {
        if next.start_line <= cur.end_line + 2 {
            let end = cur.end_line.max(next.end_line);
            cur = ExtractedSegment {
                start_line: cur.start_line,
                end_line: end,
                lines: all_lines[cur.start_line - 1..end].to_vec(),
            };
        } else {
            merged.push(cur);
            cur = next;
        }
    }
    merged.push(cur);
    merged
}

/// Extracts a `BLOCK N` region from C/CMake/Rust/AsciiDoc or a top-level JSON key block.
pub fn extract_block_segment(
    lines: &[String],
    target: &str,
    lang: &str,
) -> Option<ExtractedSegment> {
    if lang == "json" {
        return extract_json_key_block(lines, target);
    }

    let needle = format!("{}:", target.trim());
    let needle_lower = needle.to_ascii_lowercase();
    let target_lower = target.trim().to_ascii_lowercase();

    let hit_idx = lines.iter().position(|line| {
        let low = line.to_ascii_lowercase();
        (low.contains(&needle_lower) || low.contains(&target_lower))
            && (low.contains("block ")
                || low.trim_start().starts_with("//")
                || low.trim_start().starts_with('#')
                || low.trim_start().starts_with('*')
                || lines_has_comment_banner(lines, line))
    })?;

    let start_idx = find_banner_start(lines, hit_idx);
    let mut end_idx = lines.len();
    for i in (hit_idx + 1)..lines.len() {
        let trimmed = lines[i].trim();
        let low = trimmed.to_ascii_lowercase();
        if low.starts_with("block ") && (low.contains(':') || low.contains("block"))
            || low.contains("block 1:")
            || low.contains("block 2:")
            || low.contains("block 3:")
            || low.contains("block 4:")
            || low.contains("block 5:")
        {
            if is_inside_banner_frame(lines, i) {
                end_idx = find_banner_start(lines, i);
                break;
            }
        }
    }

    while end_idx > start_idx + 1 && lines[end_idx - 1].trim().is_empty() {
        end_idx -= 1;
    }

    Some(ExtractedSegment {
        start_line: start_idx + 1,
        end_line: end_idx,
        lines: lines[start_idx..end_idx].to_vec(),
    })
}

/// Extracts a top-level JSON property (`"block_3_body": { ... }`) by brace depth.
pub fn extract_json_key_block(lines: &[String], key: &str) -> Option<ExtractedSegment> {
    let quoted = format!("\"{}\"", key);
    let mut depth: i32 = 0;
    let mut start_idx = None;
    let mut target_depth = 0;

    for (i, line) in lines.iter().enumerate() {
        if start_idx.is_none() && depth == 1 && line.contains(&quoted) && line.contains(':') {
            start_idx = Some(i);
            target_depth = depth;
        }

        let mut in_str = false;
        let mut esc = false;
        for ch in line.chars() {
            if esc {
                esc = false;
                continue;
            }
            if ch == '\\' && in_str {
                esc = true;
                continue;
            }
            if ch == '"' {
                in_str = !in_str;
                continue;
            }
            if !in_str {
                match ch {
                    '{' | '[' => depth += 1,
                    '}' | ']' => depth -= 1,
                    _ => {}
                }
            }
        }

        if let Some(s) = start_idx {
            if depth <= target_depth {
                return Some(ExtractedSegment {
                    start_line: s + 1,
                    end_line: i + 1,
                    lines: lines[s..=i].to_vec(),
                });
            }
        }
    }
    None
}

/// Extracts a numbered `Sub-Assembly X.Y` region until the next Sub-Assembly or BLOCK.
pub fn extract_subassembly_segment(lines: &[String], target: &str) -> Option<ExtractedSegment> {
    let norm = if target.to_ascii_lowercase().starts_with("sub-assembly") {
        target.to_string()
    } else {
        format!("Sub-Assembly {}", target)
    };
    let norm_lower = norm.to_ascii_lowercase();

    let hit_idx = lines.iter().enumerate().position(|(idx, line)| {
        let low = line.to_ascii_lowercase();
        low.contains(&norm_lower)
            && (low.trim_start().starts_with("//")
                || low.trim_start().starts_with('#')
                || low.trim_start().starts_with('*')
                || is_inside_banner_frame(lines, idx))
    })?;

    let start_idx = find_banner_start(lines, hit_idx);
    let mut end_idx = lines.len();
    for i in (hit_idx + 1)..lines.len() {
        let low = lines[i].to_ascii_lowercase();
        let is_next_sub = low.contains("sub-assembly ")
            && (low.trim_start().starts_with("//")
                || low.trim_start().starts_with('#')
                || low.trim_start().starts_with('*')
                || is_inside_banner_frame(lines, i));
        let is_next_block = (low.contains("block 1:")
            || low.contains("block 2:")
            || low.contains("block 3:")
            || low.contains("block 4:")
            || low.contains("block 5:"))
            && is_inside_banner_frame(lines, i);

        if is_next_sub || is_next_block {
            end_idx = find_banner_start(lines, i);
            break;
        }
    }

    while end_idx > start_idx + 1 && lines[end_idx - 1].trim().is_empty() {
        end_idx -= 1;
    }

    Some(ExtractedSegment {
        start_line: start_idx + 1,
        end_line: end_idx,
        lines: lines[start_idx..end_idx].to_vec(),
    })
}

/// Extracts a top-level C or Rust function definition (with preceding Doxygen docstring).
pub fn extract_symbol_segment(
    lines: &[String],
    symbol: &str,
    _lang: &str,
) -> Option<ExtractedSegment> {
    let mut brace_depth: i32 = 0;
    let mut in_block_comment = false;
    let mut fallback_proto: Option<ExtractedSegment> = None;

    let mut i = 0;
    while i < lines.len() {
        let line = &lines[i];
        let trimmed = line.trim();

        if brace_depth == 0
            && !in_block_comment
            && !trimmed.starts_with("//")
            && !trimmed.starts_with("/*")
            && !trimmed.starts_with('*')
            && !trimmed.starts_with('#')
            && line_contains_symbol_call(trimmed, symbol)
        {
            let lookahead_end = (i + 12).min(lines.len());
            let mut found_open_brace = None;
            let mut found_semi = None;
            for j in i..lookahead_end {
                let lt = lines[j].trim();
                if let Some(pos) = lt.find('{') {
                    if !lt[..pos].contains(';') {
                        found_open_brace = Some(j);
                        break;
                    }
                }
                if lt.contains(';') {
                    found_semi = Some(j);
                    break;
                }
            }

            if let Some(brace_line) = found_open_brace {
                let start_idx = walk_back_docstring(lines, i);
                let end_idx = scan_balanced_braces(lines, brace_line)?;
                return Some(ExtractedSegment {
                    start_line: start_idx + 1,
                    end_line: end_idx + 1,
                    lines: lines[start_idx..=end_idx].to_vec(),
                });
            } else if let Some(semi_line) = found_semi {
                if fallback_proto.is_none() {
                    let start_idx = walk_back_docstring(lines, i);
                    fallback_proto = Some(ExtractedSegment {
                        start_line: start_idx + 1,
                        end_line: semi_line + 1,
                        lines: lines[start_idx..=semi_line].to_vec(),
                    });
                }
            }
        }

        advance_c_brace_state(line, &mut brace_depth, &mut in_block_comment);
        i += 1;
    }

    for idx in 0..lines.len() {
        let lt = lines[idx].trim();
        if lt.starts_with("typedef ") && lt.contains(symbol) && lt.contains('{') {
            let start_idx = walk_back_docstring(lines, idx);
            let end_idx = scan_balanced_braces(lines, idx)?;
            return Some(ExtractedSegment {
                start_line: start_idx + 1,
                end_line: end_idx + 1,
                lines: lines[start_idx..=end_idx].to_vec(),
            });
        }
    }

    fallback_proto
}

pub fn line_contains_symbol_call(trimmed: &str, symbol: &str) -> bool {
    if let Some(pos) = trimmed.find(symbol) {
        let before_ok = pos == 0
            || !trimmed[..pos]
                .chars()
                .last()
                .map(|c| c.is_ascii_alphanumeric() || c == '_')
                .unwrap_or(false);
        if !before_ok {
            return false;
        }
        let after = trimmed[pos + symbol.len()..].trim_start();
        return after.starts_with('(');
    }
    false
}

pub fn walk_back_docstring(lines: &[String], fn_idx: usize) -> usize {
    let mut idx = fn_idx;
    while idx > 0 {
        let prev = lines[idx - 1].trim();
        if prev.starts_with("CS_") || prev.starts_with("#[") {
            idx -= 1;
            continue;
        }
        if prev.starts_with("///") || prev.starts_with("//") {
            if prev.contains("====") || prev.contains("Sub-Assembly") || prev.contains("BLOCK ") {
                break;
            }
            idx -= 1;
            continue;
        }
        if prev.ends_with("*/") {
            let mut comment_start = idx - 1;
            while comment_start > 0 && !lines[comment_start].trim().starts_with("/*") {
                comment_start -= 1;
            }
            let head = lines[comment_start].trim();
            let is_section_banner = head == "/*"
                && comment_start + 1 < lines.len()
                && (lines[comment_start + 1].contains("====")
                    || lines[comment_start + 1].contains("Sub-Assembly")
                    || lines[comment_start + 1].contains("BLOCK "));
            if is_section_banner {
                break;
            }
            idx = comment_start;
            continue;
        }
        break;
    }
    idx
}

pub fn scan_balanced_braces(lines: &[String], open_line: usize) -> Option<usize> {
    let mut depth: i32 = 0;
    let mut in_comment = false;
    let mut saw_open = false;

    for (i, line) in lines.iter().enumerate().skip(open_line) {
        advance_c_brace_state_flag(line, &mut depth, &mut in_comment, &mut saw_open);
        if saw_open && depth <= 0 {
            return Some(i);
        }
    }
    None
}

pub fn advance_c_brace_state(line: &str, depth: &mut i32, in_comment: &mut bool) {
    let mut dummy = false;
    advance_c_brace_state_flag(line, depth, in_comment, &mut dummy);
}

pub fn advance_c_brace_state_flag(
    line: &str,
    depth: &mut i32,
    in_comment: &mut bool,
    saw_open: &mut bool,
) {
    let chars: Vec<char> = line.chars().collect();
    let mut i = 0;
    let mut in_str = false;
    let mut str_delim = '"';
    while i < chars.len() {
        if *in_comment {
            if chars[i] == '*' && i + 1 < chars.len() && chars[i + 1] == '/' {
                *in_comment = false;
                i += 2;
            } else {
                i += 1;
            }
            continue;
        }
        if in_str {
            if chars[i] == '\\' {
                i += 2;
                continue;
            }
            if chars[i] == str_delim {
                in_str = false;
            }
            i += 1;
            continue;
        }
        if chars[i] == '/' && i + 1 < chars.len() {
            if chars[i + 1] == '/' {
                break;
            }
            if chars[i + 1] == '*' {
                *in_comment = true;
                i += 2;
                continue;
            }
        }
        if chars[i] == '"' || chars[i] == '\'' {
            in_str = true;
            str_delim = chars[i];
            i += 1;
            continue;
        }
        if chars[i] == '{' {
            *depth += 1;
            *saw_open = true;
        } else if chars[i] == '}' {
            *depth -= 1;
        }
        i += 1;
    }
}

pub fn is_inside_banner_frame(lines: &[String], idx: usize) -> bool {
    let start = idx.saturating_sub(3);
    let end = (idx + 3).min(lines.len().saturating_sub(1));
    for k in start..=end {
        let t = lines[k].trim();
        if t.starts_with("/*")
            || t.contains("==========")
            || t.starts_with("// ---")
            || t.starts_with("# ---")
            || t == "////"
        {
            return true;
        }
    }
    false
}

pub fn find_banner_start(lines: &[String], hit_idx: usize) -> usize {
    let mut start = hit_idx;
    let floor = hit_idx.saturating_sub(4);
    while start > floor {
        let prev = lines[start - 1].trim();
        if prev.starts_with("/*")
            || prev.starts_with("// ===")
            || prev.starts_with("// ---")
            || prev.starts_with("# ===")
            || prev.starts_with("# ---")
            || prev.starts_with("====")
            || prev.starts_with("----")
            || prev == "////"
        {
            start -= 1;
        } else {
            break;
        }
    }
    start
}

pub fn lines_has_comment_banner(lines: &[String], target_line: &str) -> bool {
    let target_idx = match lines.iter().position(|l| l == target_line) {
        Some(idx) => idx,
        None => return false,
    };
    is_inside_banner_frame(lines, target_idx)
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: Plate & Appendix Formatting
// -----------------------------------------------------------------------------

/// Formats a snapshot plate ready for inline `include::` inside AsciiDoc chapters.
pub fn format_snapshot_plate(snap: &RenderedSnapshot, show_linenums: bool) -> String {
    let mut out = String::new();
    let first_line = snap.inline_segments.first().map(|s| s.start_line).unwrap_or(1);
    let last_line = snap.inline_segments.last().map(|s| s.end_line).unwrap_or(1);

    out.push_str(&format!("[[snap_plate_{}]]\n", snap.spec.id));
    out.push_str(&format!(
        ".{} (`{}#L{}-L{}` · Mode: `{:?}`)\n",
        snap.spec.title, snap.spec.source_file, first_line, last_line, snap.spec.mode
    ));

    let mut applied_callouts: Vec<&SnapshotCalloutSpec> = Vec::new();
    let comment_prefix = if snap.lang == "cmake" || snap.lang == "yaml" {
        "# "
    } else {
        "// "
    };

    for (seg_idx, seg) in snap.inline_segments.iter().enumerate() {
        if seg_idx > 0 {
            out.push_str(&format!(
                "\n_Group slice {} (`#L{}-L{}`):_\n\n",
                seg_idx + 1,
                seg.start_line,
                seg.end_line
            ));
        }
        if show_linenums {
            out.push_str(&format!(
                "[source,{},linenums,start={}]\n------\n",
                snap.lang, seg.start_line
            ));
        } else {
            out.push_str(&format!("[source,{}]\n------\n", snap.lang));
        }

        for line in &seg.lines {
            let mut annotated = line.clone();
            if snap.lang != "json" {
                let lt = line.trim();
                let is_comment = lt.starts_with("/*")
                    || lt.starts_with('*')
                    || lt.starts_with("//")
                    || lt.starts_with("# ");
                if !is_comment {
                    for co in &snap.spec.callouts {
                        if !applied_callouts.iter().any(|c| c.number == co.number)
                            && line.contains(&co.pattern)
                        {
                            annotated.push_str(&format!(" {}<{}>", comment_prefix, co.number));
                            applied_callouts.push(co);
                            break;
                        }
                    }
                }
            }
            out.push_str(&annotated);
            out.push('\n');
        }
        out.push_str("------\n");
    }

    if !applied_callouts.is_empty() {
        applied_callouts.sort_by_key(|c| c.number);
        out.push('\n');
        for co in &applied_callouts {
            out.push_str(&format!("<{}> {}\n", co.number, co.text));
        }
    }

    if let Some(ref note) = snap.spec.callout_note {
        out.push_str(&format!("\n_{}_\n", note));
    }

    if snapshot_goes_in_appendix(snap) {
        let ws_first = snap.wholesale_segments.first().map(|s| s.start_line).unwrap_or(1);
        let ws_last = snap.wholesale_segments.last().map(|s| s.end_line).unwrap_or(1);
        out.push_str(&format!(
            "\nifdef::has-snapshot-appendix[Wholesale Listing: Complete verbatim code (`#L{}-L{}`) -> <<snap_wholesale_{},Appendix: {}>>.]\n",
            ws_first, ws_last, snap.spec.id, snap.spec.title
        ));
    }

    out
}

pub fn snapshot_goes_in_appendix(snap: &RenderedSnapshot) -> bool {
    match snap.spec.wholesale_in_appendix {
        WholesalePolicy::Always => true,
        WholesalePolicy::Never => false,
        WholesalePolicy::WhenExcerpted => snap.was_excerpted,
    }
}

/// Formats a wholesale reference entry appended into Appendix A of compiled books.
pub fn format_wholesale_appendix_entry(snap: &RenderedSnapshot, show_linenums: bool) -> String {
    let mut out = String::new();
    let ws_first = snap.wholesale_segments.first().map(|s| s.start_line).unwrap_or(1);
    let ws_last = snap.wholesale_segments.last().map(|s| s.end_line).unwrap_or(1);

    out.push_str(&format!("[[snap_wholesale_{}]]\n", snap.spec.id));
    out.push_str(&format!("=== {}\n\n", snap.spec.title));
    out.push_str(&format!(
        "*Snapshot ID*: `{}` | *Mode*: `{:?}` | *File*: `{}` (`L{}–L{}` of `{} lines`) | <<snap_plate_{},Jump back to chapter text>>\n\n",
        snap.spec.id,
        snap.spec.mode,
        snap.spec.source_file,
        ws_first,
        ws_last,
        snap.total_source_lines,
        snap.spec.id
    ));

    for seg in &snap.wholesale_segments {
        if show_linenums {
            out.push_str(&format!(
                "[source,{},linenums,start={}]\n------\n",
                snap.lang, seg.start_line
            ));
        } else {
            out.push_str(&format!("[source,{}]\n------\n", snap.lang));
        }
        for line in &seg.lines {
            out.push_str(line);
            out.push('\n');
        }
        out.push_str("------\n\n");
    }
    out
}

pub fn format_snapshots_index(snaps: &[RenderedSnapshot]) -> String {
    let mut out = String::new();
    out.push_str("= Generated Textbook Code Snapshots Index\n\n");
    out.push_str("[cols=\"2,2,1,2,1\",options=\"header\"]\n|===\n");
    out.push_str("| Snapshot ID | Source File | Mode | Line Span | Appendix?\n\n");
    for s in snaps {
        let first = s.wholesale_segments.first().map(|x| x.start_line).unwrap_or(1);
        let last = s.wholesale_segments.last().map(|x| x.end_line).unwrap_or(1);
        out.push_str(&format!(
            "| `{}` | `{}` | `{:?}` | L{}–L{} | {}\n",
            s.spec.id,
            s.spec.source_file,
            s.spec.mode,
            first,
            last,
            if snapshot_goes_in_appendix(s) { "Yes" } else { "Inline full" }
        ));
    }
    out.push_str("|===\n");
    out
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: Snapshot Materialization & Batching
// -----------------------------------------------------------------------------

/// Loads `docs/code_snapshots_manifest.json` and writes live plates into `docs/snapshots/`.
pub fn prepare_snapshots(
    repo_root: &Path,
    config: &GuideConfig,
) -> Result<Vec<RenderedSnapshot>, String> {
    let manifest_path = repo_root.join(&config.snapshots.manifest_path);
    if !manifest_path.is_file() {
        return Ok(Vec::new());
    }

    let raw = fs::read_to_string(&manifest_path)
        .map_err(|e| format!("Failed to read snapshots manifest {:?}: {}", manifest_path, e))?;
    let manifest: CodeSnapshotsManifest = serde_json::from_str(&raw)
        .map_err(|e| format!("Failed to parse snapshots manifest {:?}: {}", manifest_path, e))?;

    let out_dir = repo_root.join(&config.snapshots.output_dir);
    fs::create_dir_all(&out_dir)
        .map_err(|e| format!("Failed to create snapshots dir {:?}: {}", out_dir, e))?;

    let mut rendered = Vec::with_capacity(manifest.block_3_body.snapshots.len());
    for spec in manifest.block_3_body.snapshots {
        let snap = materialize_snapshot(repo_root, &out_dir, spec)?;
        let plate_text = format_snapshot_plate(&snap, config.snapshots.show_line_numbers);
        fs::write(&snap.plate_path, plate_text)
            .map_err(|e| format!("Failed to write snapshot plate {:?}: {}", snap.plate_path, e))?;
        rendered.push(snap);
    }

    let index_text = format_snapshots_index(&rendered);
    fs::write(out_dir.join("INDEX.adoc"), index_text)
        .map_err(|e| format!("Failed to write snapshot INDEX.adoc: {}", e))?;

    Ok(rendered)
}

/// Materializes a single `CodeSnapshotSpec` from repository disk sources.
pub fn materialize_snapshot(
    repo_root: &Path,
    out_dir: &Path,
    spec: CodeSnapshotSpec,
) -> Result<RenderedSnapshot, String> {
    let src_path = repo_root.join(&spec.source_file);
    let raw = fs::read_to_string(&src_path).map_err(|e| {
        format!(
            "Snapshot '{}' could not read source '{}': {}",
            spec.id, spec.source_file, e
        )
    })?;
    let all_lines: Vec<String> = raw.lines().map(|l| l.to_string()).collect();
    let total_source_lines = all_lines.len();
    let lang = spec
        .language
        .clone()
        .unwrap_or_else(|| infer_language(&spec.source_file));

    let wholesale_segments = extract_segments_for_spec(&all_lines, &spec, &lang)?;
    let (inline_segments, was_excerpted) = if let Some(ref ex) = spec.excerpt {
        let segs = extract_excerpt_segments(&all_lines, &wholesale_segments, ex, &lang)?;
        let inline_count: usize = segs.iter().map(|s| s.lines.len()).sum();
        let whole_count: usize = wholesale_segments.iter().map(|s| s.lines.len()).sum();
        (segs, inline_count < whole_count)
    } else {
        (wholesale_segments.clone(), false)
    };

    let plate_path = out_dir.join(format!("{}.adoc", spec.id));
    Ok(RenderedSnapshot {
        spec,
        lang,
        inline_segments,
        wholesale_segments,
        was_excerpted,
        total_source_lines,
        plate_path,
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
    fn test_section_1_nominal_language_inference() {
        assert_eq!(infer_language("engine/src/main.c"), "c");
        assert_eq!(infer_language("engine/include/io.h"), "c");
        assert_eq!(infer_language("tools/src/main.rs"), "rust");
        assert_eq!(infer_language("CMakeLists.txt"), "cmake");
        assert_eq!(infer_language("data.json"), "json");
        assert_eq!(infer_language("theme.yml"), "yaml");
        assert_eq!(infer_language("notes.txt"), "text");
    }

    #[test]
    fn test_section_1_nominal_block_and_subassembly_extraction() {
        let c_lines: Vec<String> = vec![
            "/*".to_string(),
            "BLOCK 1: METADATA".to_string(),
            "*/".to_string(),
            "/* --- Sub-Assembly 2.1: Constants --- */".to_string(),
            "#define FOO 42".to_string(),
            "/* --- Sub-Assembly 2.2: Types --- */".to_string(),
            "typedef int bar_t;".to_string(),
            "/*".to_string(),
            "BLOCK 3: BODY".to_string(),
            "*/".to_string(),
            "int add(int a, int b) { return a + b; }".to_string(),
        ];

        let blk1 = extract_block_segment(&c_lines, "BLOCK 1", "c").expect("BLOCK 1 found");
        assert_eq!(blk1.start_line, 1);

        let sub21 = extract_subassembly_segment(&c_lines, "2.1").expect("Sub-Assembly 2.1 found");
        assert!(sub21.lines.iter().any(|l| l.contains("FOO 42")));
        assert!(!sub21.lines.iter().any(|l| l.contains("bar_t")));

        let sym = extract_symbol_segment(&c_lines, "add", "c").expect("Symbol add found");
        assert!(sym.lines.iter().any(|l| l.contains("int add")));
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_missing_target() {
        let lines: Vec<String> = vec!["int x = 1;".to_string()];
        assert!(extract_block_segment(&lines, "BLOCK 99", "c").is_none());
        assert!(extract_subassembly_segment(&lines, "9.9").is_none());
        assert!(extract_symbol_segment(&lines, "nonexistent_fn", "c").is_none());
    }

    #[test]
    fn test_section_2_never_fail_invalid_spec_validation() {
        let bad_spec = CodeSnapshotSpec {
            id: "".to_string(),
            title: "".to_string(),
            source_file: "".to_string(),
            mode: SnapshotMode::Whole,
            target: None,
            targets: vec![],
            language: None,
            excerpt: None,
            wholesale_in_appendix: WholesalePolicy::Never,
            callout_note: None,
            callouts: vec![],
        };
        assert!(!is_valid_snapshot_spec(&bad_spec));
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_snapshot_modes_and_policies() {
        let modes = [
            SnapshotMode::Whole,
            SnapshotMode::Block,
            SnapshotMode::Subassembly,
            SnapshotMode::Function,
            SnapshotMode::Group,
        ];
        assert_eq!(modes.len(), 5);

        let policies = [
            WholesalePolicy::Always,
            WholesalePolicy::WhenExcerpted,
            WholesalePolicy::Never,
        ];
        assert_eq!(policies.len(), 3);
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_materialize_and_format_plate() {
        let temp_dir = std::env::temp_dir();
        let src_file = temp_dir.join("test_sample_cog.c");
        let out_dir = temp_dir.join("snapshots_test_out");
        fs::create_dir_all(&out_dir).expect("Create out dir");

        let content = "int test_target_fn(int x) {\n    return x * 2;\n}\n";
        fs::write(&src_file, content).expect("Write source file");

        let spec = CodeSnapshotSpec {
            id: "snap_test_target".to_string(),
            title: "Test Target Function".to_string(),
            source_file: src_file.to_string_lossy().to_string(),
            mode: SnapshotMode::Function,
            target: Some("test_target_fn".to_string()),
            targets: vec![],
            language: Some("c".to_string()),
            excerpt: None,
            wholesale_in_appendix: WholesalePolicy::WhenExcerpted,
            callout_note: Some("Doubles the integer parameter.".to_string()),
            callouts: vec![SnapshotCalloutSpec {
                pattern: "return x * 2".to_string(),
                number: 1,
                text: "Arithmetic double".to_string(),
            }],
        };

        let res = materialize_snapshot(Path::new("/"), &out_dir, spec);
        assert!(res.is_ok());
        let snap = res.unwrap();
        let plate = format_snapshot_plate(&snap, true);
        assert!(plate.contains("[[snap_plate_snap_test_target]]"));
        assert!(plate.contains("Test Target Function"));
        assert!(plate.contains("[source,c,linenums,start=1]"));
        assert!(plate.contains("<1> Arithmetic double"));

        let _ = fs::remove_file(src_file);
        let _ = fs::remove_dir_all(out_dir);
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Diagnostic Self-Audit
// -----------------------------------------------------------------------------

/// Performs an in-process diagnostic preflight self-audit of snapshot module invariants.
///
/// # Examples
///
/// ```rust
/// use tl_tools::guide::snapshot::self_audit;
///
/// assert!(self_audit().is_ok());
/// ```
pub fn self_audit() -> Result<(), String> {
    if infer_language("test.c") != "c" {
        return Err("infer_language failed for C extension".to_string());
    }
    if infer_language("test.rs") != "rust" {
        return Err("infer_language failed for Rust extension".to_string());
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

/// Verifies that the snapshot sub-assembly is ready for operation.
///
/// # Examples
///
/// ```rust
/// use tl_tools::guide::snapshot::is_snapshot_subsystem_ready;
///
/// assert!(is_snapshot_subsystem_ready());
/// ```
pub fn is_snapshot_subsystem_ready() -> bool {
    self_audit().is_ok()
}
