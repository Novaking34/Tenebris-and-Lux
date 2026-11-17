// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/guide/theme.rs
// @id            cog.tools.guide.theme
// @layer         Layer 3: Developer Toolchain Command Sub-Assemblies
// @archetype     pure_types | theme_engine
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Guide Theme Sub-Assembly
// @role          PDF Textbook Theme Specification & YAML Generator
// @maturity      Rev 5 Baseline | Asciidoctor-PDF theme compiler
// @chassis       bounded-heap | Developer tooling memory chassis; bounded formatting
// @scale         O(1) theme specification serialization
// @seams         in: guide_theme_config.json | out: docs/themes/*-theme.yml
// @invariants    1. Valid Asciidoctor-PDF 2.3+ theme schema adherence.
//                2. Deterministic CSS/YAML property emission matching JSON.
//                3. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Translates high-level 5-block theme design manifests (`guide_theme_config.json`)
//   into strictly-formatted Asciidoctor-PDF YAML theme files.
//
//   ### For Game Designers & Non-Programmers:
//   Allows tuning page geometry, margins, typography sizes, code block colors,
//   and header/footer chrome without writing manual Asciidoctor YAML.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Theme rendering is pure string transformation.
//   - Extension Seam: Additional theme properties map directly to YAML tokens.
//   - Explicit Non-Goal: Does not spawn external processes during theme sync.
// =============================================================================

//! # Guide Theme Specification & Compiler
//!
//! Generates Asciidoctor-PDF theme YAML from 5-block JSON specifications.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

use std::fs;
use std::path::{Path, PathBuf};
use serde::{Deserialize, Serialize};

use super::manifest::{
    default_theme_extends, default_theme_name, default_theme_yaml_path,
    GuideConfig,
};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Types, Enums & Theme Envelopes
// -----------------------------------------------------------------------------

/// Data-driven PDF textbook theme specification (`docs/guide_theme_config.json`).
#[derive(Debug, Clone, PartialEq, Serialize, Deserialize)]
pub struct GuideThemeManifest {
    #[serde(default, alias = "block_2_opening")]
    pub block_2_types: Option<GuideThemeMeta>,
    pub block_3_body: GuideThemeBody,
}

/// Metadata describing theme name, base theme, and target YAML output path.
///
/// # Examples
///
/// ```rust
/// use tl_tools::guide::GuideThemeMeta;
///
/// let meta = GuideThemeMeta::default();
/// assert_eq!(meta.theme_name, "cornerstone-book");
/// assert_eq!(meta.extends, "default");
/// ```
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct GuideThemeMeta {
    #[serde(default = "default_theme_name")]
    pub theme_name: String,
    #[serde(default = "default_theme_extends")]
    pub extends: String,
    #[serde(default = "default_theme_yaml_path")]
    pub generated_yaml_path: String,
}

impl Default for GuideThemeMeta {
    fn default() -> Self {
        Self {
            theme_name: default_theme_name(),
            extends: default_theme_extends(),
            generated_yaml_path: default_theme_yaml_path(),
        }
    }
}

/// Complete theme body containing page, typography, spacing, and chrome parameters.
#[derive(Debug, Clone, PartialEq, Serialize, Deserialize)]
pub struct GuideThemeBody {
    pub page: ThemePage,
    pub typography: ThemeTypography,
    pub spacing: ThemeSpacing,
    pub breaks: ThemeBreaks,
    pub blocks: ThemeBlocks,
    pub running_chrome: ThemeRunningChrome,
}

impl Default for GuideThemeBody {
    fn default() -> Self {
        Self {
            page: ThemePage::default(),
            typography: ThemeTypography::default(),
            spacing: ThemeSpacing::default(),
            breaks: ThemeBreaks::default(),
            blocks: ThemeBlocks::default(),
            running_chrome: ThemeRunningChrome::default(),
        }
    }
}

/// Physical page dimensions and margins.
#[derive(Debug, Clone, PartialEq, Serialize, Deserialize)]
pub struct ThemePage {
    pub size: String,
    pub layout: String,
    pub margin_top_in: f64,
    pub margin_right_in: f64,
    pub margin_bottom_in: f64,
    pub margin_left_in: f64,
    pub background_color: String,
}

impl Default for ThemePage {
    fn default() -> Self {
        Self {
            size: "letter".to_string(),
            layout: "portrait".to_string(),
            margin_top_in: 0.75,
            margin_right_in: 0.75,
            margin_bottom_in: 0.75,
            margin_left_in: 0.75,
            background_color: "#ffffff".to_string(),
        }
    }
}

/// Base typography styles, fonts, and colors.
#[derive(Debug, Clone, PartialEq, Serialize, Deserialize)]
pub struct ThemeTypography {
    pub font_color: String,
    pub base_font_size: f64,
    pub line_height_length: f64,
    pub text_align: String,
    pub link_color: String,
    pub codespan_color: String,
    pub code_font_size: f64,
    pub code_line_height: f64,
    pub code_padding: f64,
    pub code_line_gap: f64,
    pub code_background_color: String,
    pub code_border_color: String,
}

impl Default for ThemeTypography {
    fn default() -> Self {
        Self {
            font_color: "#222222".to_string(),
            base_font_size: 10.5,
            line_height_length: 15.0,
            text_align: "justify".to_string(),
            link_color: "#1a5fb4".to_string(),
            codespan_color: "#b52a1d".to_string(),
            code_font_size: 8.5,
            code_line_height: 1.25,
            code_padding: 3.0,
            code_line_gap: 2.0,
            code_background_color: "#f6f8fa".to_string(),
            code_border_color: "#d0d7de".to_string(),
        }
    }
}

/// Spacing, vertical rhythm, and heading scales.
#[derive(Debug, Clone, PartialEq, Serialize, Deserialize)]
pub struct ThemeSpacing {
    pub vertical_rhythm: f64,
    pub horizontal_rhythm: f64,
    pub block_margin_bottom: f64,
    pub prose_margin_bottom: f64,
    pub list_item_spacing: f64,
    pub heading_margin_top: f64,
    pub heading_margin_bottom: f64,
    pub heading_min_height_after: f64,
    pub heading_h1_scale: f64,
    pub heading_h2_scale: f64,
    pub heading_h3_scale: f64,
    pub heading_h4_scale: f64,
    pub heading_h5_scale: f64,
    pub heading_h6_scale: f64,
}

impl Default for ThemeSpacing {
    fn default() -> Self {
        Self {
            vertical_rhythm: 12.0,
            horizontal_rhythm: 12.0,
            block_margin_bottom: 10.0,
            prose_margin_bottom: 8.0,
            list_item_spacing: 3.0,
            heading_margin_top: 14.0,
            heading_margin_bottom: 6.0,
            heading_min_height_after: 50.0,
            heading_h1_scale: 2.2,
            heading_h2_scale: 1.8,
            heading_h3_scale: 1.4,
            heading_h4_scale: 1.2,
            heading_h5_scale: 1.1,
            heading_h6_scale: 1.0,
        }
    }
}

/// Page break policies before parts and chapters.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct ThemeBreaks {
    pub part_break_before: String,
    pub chapter_break_before: String,
}

impl Default for ThemeBreaks {
    fn default() -> Self {
        Self {
            part_break_before: "always".to_string(),
            chapter_break_before: "always".to_string(),
        }
    }
}

/// Block decorators: admonitions, sidebars, tables, and TOC.
#[derive(Debug, Clone, PartialEq, Serialize, Deserialize)]
pub struct ThemeBlocks {
    pub admonition_padding: [f64; 4],
    pub admonition_rule_color: String,
    pub admonition_rule_width: f64,
    pub sidebar_background_color: String,
    pub sidebar_border_color: String,
    pub sidebar_padding: [f64; 4],
    pub table_cell_padding: [f64; 4],
    pub table_border_color: String,
    pub table_stripe_background_color: String,
    pub table_head_background_color: String,
    pub toc_line_height: f64,
}

impl Default for ThemeBlocks {
    fn default() -> Self {
        Self {
            admonition_padding: [6.0, 8.0, 6.0, 8.0],
            admonition_rule_color: "#1a5fb4".to_string(),
            admonition_rule_width: 3.0,
            sidebar_background_color: "#f8f9fa".to_string(),
            sidebar_border_color: "#c0c0c0".to_string(),
            sidebar_padding: [8.0, 10.0, 8.0, 10.0],
            table_cell_padding: [4.0, 6.0, 4.0, 6.0],
            table_border_color: "#cccccc".to_string(),
            table_stripe_background_color: "#fcfcfc".to_string(),
            table_head_background_color: "#f0f0f0".to_string(),
            toc_line_height: 1.3,
        }
    }
}

/// Header and footer running chrome specifications.
#[derive(Debug, Clone, PartialEq, Serialize, Deserialize)]
pub struct ThemeRunningChrome {
    pub header_height: f64,
    pub header_font_size: f64,
    pub header_font_color: String,
    pub header_border_color: String,
    pub header_left: String,
    pub header_right: String,
    pub footer_height: f64,
    pub footer_font_size: f64,
    pub footer_font_color: String,
    pub footer_border_color: String,
    pub footer_left: String,
    pub footer_right: String,
}

impl Default for ThemeRunningChrome {
    fn default() -> Self {
        Self {
            header_height: 24.0,
            header_font_size: 8.0,
            header_font_color: "#666666".to_string(),
            header_border_color: "#e0e0e0".to_string(),
            header_left: "Tenebris and Lux Master Guide".to_string(),
            header_right: "{section-or-chapter-title}".to_string(),
            footer_height: 24.0,
            footer_font_size: 8.0,
            footer_font_color: "#666666".to_string(),
            footer_border_color: "#e0e0e0".to_string(),
            footer_left: "CreativeWorkzStudio LLC".to_string(),
            footer_right: "{page-number}".to_string(),
        }
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Verification Helpers & Invariant Predicates
// -----------------------------------------------------------------------------

/// Evaluates whether theme metadata contains non-empty identifiers.
///
/// # Examples
///
/// ```rust
/// use tl_tools::guide::GuideThemeMeta;
/// use tl_tools::guide::theme::is_valid_theme_meta;
///
/// let meta = GuideThemeMeta::default();
/// assert!(is_valid_theme_meta(&meta));
/// ```
pub fn is_valid_theme_meta(meta: &GuideThemeMeta) -> bool {
    !meta.theme_name.trim().is_empty()
        && !meta.extends.trim().is_empty()
        && !meta.generated_yaml_path.trim().is_empty()
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( YAML Generation & File Synchronization )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: YAML Generation & File Synchronization
// -----------------------------------------------------------------------------

/// Compiles `docs/guide_theme_config.json` into `docs/themes/<name>-theme.yml`.
pub fn sync_theme_yaml(
    repo_root: &Path,
    config: &GuideConfig,
) -> Result<Option<PathBuf>, String> {
    let Some(ref cfg_path) = config.pdf_theme_data_file else {
        return Ok(None);
    };
    if !cfg_path.is_file() {
        return Ok(None);
    }

    let raw = fs::read_to_string(cfg_path)
        .map_err(|e| format!("Failed to read PDF theme data {:?}: {}", cfg_path, e))?;
    let manifest: GuideThemeManifest = serde_json::from_str(&raw)
        .map_err(|e| format!("Failed to parse {:?}: {}", cfg_path, e))?;

    let meta = manifest.block_2_types.unwrap_or(GuideThemeMeta {
        theme_name: config.pdf_theme_name.clone(),
        extends: default_theme_extends(),
        generated_yaml_path: default_theme_yaml_path(),
    });
    let yaml_path = repo_root.join(&meta.generated_yaml_path);
    if let Some(parent) = yaml_path.parent() {
        fs::create_dir_all(parent)
            .map_err(|e| format!("Failed to create theme dir {:?}: {}", parent, e))?;
    }

    let yaml = render_theme_yaml(&meta.extends, &manifest.block_3_body);
    fs::write(&yaml_path, yaml)
        .map_err(|e| format!("Failed to write theme YAML {:?}: {}", yaml_path, e))?;

    Ok(Some(yaml_path))
}

/// Formats a verified Asciidoctor-PDF 2.3+ theme YAML extending `default`.
///
/// # Examples
///
/// ```rust
/// use tl_tools::guide::theme::{render_theme_yaml, GuideThemeBody};
///
/// let body = GuideThemeBody::default();
/// let yaml = render_theme_yaml("default", &body);
/// assert!(yaml.contains("extends: default"));
/// assert!(yaml.contains("page:"));
/// assert!(yaml.contains("base:"));
/// ```
pub fn render_theme_yaml(extends: &str, body: &GuideThemeBody) -> String {
    let p = &body.page;
    let t = &body.typography;
    let s = &body.spacing;
    let br = &body.breaks;
    let bl = &body.blocks;
    let rc = &body.running_chrome;

    format!(
        "# Auto-generated from docs/guide_theme_config.json by tools/src/guide/theme.rs\n\
         extends: {extends}\n\
         page:\n\
         \x20 size: {size}\n\
         \x20 layout: {layout}\n\
         \x20 background_color: {bg}\n\
         \x20 margin: [{mt}in, {mr}in, {mb}in, {ml}in]\n\
         \x20 margin_inner: {ml}in\n\
         \x20 margin_outer: {mr}in\n\
         base:\n\
         \x20 text_align: {align}\n\
         \x20 font_color: {fc}\n\
         \x20 font_size: {bfs}\n\
         \x20 line_height_length: {lhl}\n\
         \x20 line_height: $base_line_height_length / {bfs}\n\
         vertical_rhythm: {vr}\n\
         horizontal_rhythm: {hr}\n\
         link:\n\
         \x20 font_color: {lc}\n\
         codespan:\n\
         \x20 font_color: {cc}\n\
         heading:\n\
         \x20 text_align: left\n\
         \x20 font_color: {fc}\n\
         \x20 h1_font_size: floor($base_font_size * {h1})\n\
         \x20 h2_font_size: floor($base_font_size * {h2})\n\
         \x20 h3_font_size: round($base_font_size * {h3})\n\
         \x20 h4_font_size: round($base_font_size * {h4})\n\
         \x20 h5_font_size: round($base_font_size * {h5})\n\
         \x20 h6_font_size: round($base_font_size * {h6})\n\
         \x20 margin_top: {hmt}\n\
         \x20 margin_bottom: {hmb}\n\
         \x20 min_height_after: {hmha}\n\
         \x20 part_break_before: {pbb}\n\
         \x20 chapter_break_before: {cbb}\n\
         block:\n\
         \x20 margin_bottom: {bmb}\n\
         prose:\n\
         \x20 margin_bottom: {pmb}\n\
         list:\n\
         \x20 item_spacing: {lis}\n\
         code:\n\
         \x20 font_size: {cfs}\n\
         \x20 line_height: {clh}\n\
         \x20 padding: {cpad}\n\
         \x20 line_gap: {cgap}\n\
         \x20 background_color: {cbg}\n\
         \x20 border_color: {cbd}\n\
         \x20 border_width: 0.6\n\
         admonition:\n\
         \x20 column_rule_color: {arule}\n\
         \x20 column_rule_width: {awid}\n\
         \x20 padding: [{ap0}, {ap1}, {ap2}, {ap3}]\n\
         sidebar:\n\
         \x20 background_color: {sbg}\n\
         \x20 border_color: {sbd}\n\
         \x20 padding: [{sp0}, {sp1}, {sp2}, {sp3}]\n\
         table:\n\
         \x20 border_color: {tbd}\n\
         \x20 cell_padding: [{tp0}, {tp1}, {tp2}, {tp3}]\n\
         \x20 head:\n\
         \x20   background_color: {thbg}\n\
         \x20   font_style: bold\n\
         \x20 body:\n\
         \x20   stripe_background_color: {tsbg}\n\
         toc:\n\
         \x20 line_height: {toclh}\n\
         header:\n\
         \x20 height: {hh}\n\
         \x20 font_size: {hfs}\n\
         \x20 font_color: {hfc}\n\
         \x20 border_color: {hbc}\n\
         \x20 border_width: 0.25\n\
         \x20 vertical_align: middle\n\
         \x20 recto:\n\
         \x20   left:\n\
         \x20     content: '{hl}'\n\
         \x20   right:\n\
         \x20     content: '{hr_txt}'\n\
         \x20 verso:\n\
         \x20   left:\n\
         \x20     content: '{hr_txt}'\n\
         \x20   right:\n\
         \x20     content: '{hl}'\n\
         footer:\n\
         \x20 height: {fh}\n\
         \x20 font_size: {ffs}\n\
         \x20 font_color: {ffc}\n\
         \x20 border_color: {fbc}\n\
         \x20 border_width: 0.25\n\
         \x20 vertical_align: middle\n\
         \x20 recto:\n\
         \x20   left:\n\
         \x20     content: '{fl}'\n\
         \x20   right:\n\
         \x20     content: '{fr}'\n\
         \x20 verso:\n\
         \x20   left:\n\
         \x20     content: '{fr}'\n\
         \x20   right:\n\
         \x20     content: '{fl}'\n",
        extends = extends,
        size = p.size,
        layout = p.layout,
        bg = p.background_color,
        mt = p.margin_top_in,
        mr = p.margin_right_in,
        mb = p.margin_bottom_in,
        ml = p.margin_left_in,
        align = t.text_align,
        fc = t.font_color,
        bfs = t.base_font_size,
        lhl = t.line_height_length,
        vr = s.vertical_rhythm,
        hr = s.horizontal_rhythm,
        lc = t.link_color,
        cc = t.codespan_color,
        h1 = s.heading_h1_scale,
        h2 = s.heading_h2_scale,
        h3 = s.heading_h3_scale,
        h4 = s.heading_h4_scale,
        h5 = s.heading_h5_scale,
        h6 = s.heading_h6_scale,
        hmt = s.heading_margin_top,
        hmb = s.heading_margin_bottom,
        hmha = s.heading_min_height_after,
        pbb = br.part_break_before,
        cbb = br.chapter_break_before,
        bmb = s.block_margin_bottom,
        pmb = s.prose_margin_bottom,
        lis = s.list_item_spacing,
        cfs = t.code_font_size,
        clh = t.code_line_height,
        cpad = t.code_padding,
        cgap = t.code_line_gap,
        cbg = t.code_background_color,
        cbd = t.code_border_color,
        arule = bl.admonition_rule_color,
        awid = bl.admonition_rule_width,
        ap0 = bl.admonition_padding[0],
        ap1 = bl.admonition_padding[1],
        ap2 = bl.admonition_padding[2],
        ap3 = bl.admonition_padding[3],
        sbg = bl.sidebar_background_color,
        sbd = bl.sidebar_border_color,
        sp0 = bl.sidebar_padding[0],
        sp1 = bl.sidebar_padding[1],
        sp2 = bl.sidebar_padding[2],
        sp3 = bl.sidebar_padding[3],
        tbd = bl.table_border_color,
        tp0 = bl.table_cell_padding[0],
        tp1 = bl.table_cell_padding[1],
        tp2 = bl.table_cell_padding[2],
        tp3 = bl.table_cell_padding[3],
        thbg = bl.table_head_background_color,
        tsbg = bl.table_stripe_background_color,
        toclh = bl.toc_line_height,
        hh = rc.header_height,
        hfs = rc.header_font_size,
        hfc = rc.header_font_color,
        hbc = rc.header_border_color,
        hl = rc.header_left,
        hr_txt = rc.header_right,
        fh = rc.footer_height,
        ffs = rc.footer_font_size,
        ffc = rc.footer_font_color,
        fbc = rc.footer_border_color,
        fl = rc.footer_left,
        fr = rc.footer_right,
    )
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
    fn test_section_1_nominal_theme_construction() {
        let meta = GuideThemeMeta::default();
        assert_eq!(meta.theme_name, "cornerstone-book");
        assert_eq!(meta.extends, "default");
        assert!(is_valid_theme_meta(&meta));

        let body = GuideThemeBody::default();
        assert_eq!(body.page.size, "letter");
        assert_eq!(body.typography.font_color, "#222222");
    }

    #[test]
    fn test_section_1_nominal_theme_yaml_rendering() {
        let body = GuideThemeBody::default();
        let yaml = render_theme_yaml("default", &body);
        assert!(yaml.contains("extends: default"));
        assert!(yaml.contains("page:"));
        assert!(yaml.contains("base:"));
        assert!(yaml.contains("heading:"));
        assert!(yaml.contains("code:"));
        assert!(yaml.contains("admonition:"));
        assert!(yaml.contains("header:"));
        assert!(yaml.contains("footer:"));
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_missing_theme_file() {
        let cfg = GuideConfig {
            pdf_theme_data_file: Some(PathBuf::from("/nonexistent/theme.json")),
            ..crate::guide::default_config(Path::new("."))
        };
        let res = sync_theme_yaml(Path::new("."), &cfg);
        assert!(res.is_ok());
        assert_eq!(res.unwrap(), None);
    }

    #[test]
    fn test_section_2_never_fail_empty_meta_validation() {
        let bad_meta = GuideThemeMeta {
            theme_name: "".to_string(),
            extends: "".to_string(),
            generated_yaml_path: "".to_string(),
        };
        assert!(!is_valid_theme_meta(&bad_meta));
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_yaml_contains_required_sections() {
        let body = GuideThemeBody::default();
        let yaml = render_theme_yaml("base-theme", &body);
        for token in ["extends: base-theme", "margin:", "vertical_rhythm:", "border_width:"] {
            assert!(
                yaml.contains(token),
                "Rendered YAML missing required invariant token: {}",
                token
            );
        }
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_theme_sync_round_trip() {
        let temp_dir = std::env::temp_dir();
        let theme_json_path = temp_dir.join("test_theme_config.json");
        let manifest = GuideThemeManifest {
            block_2_types: Some(GuideThemeMeta {
                theme_name: "test-book".to_string(),
                extends: "default".to_string(),
                generated_yaml_path: "test-book-theme.yml".to_string(),
            }),
            block_3_body: GuideThemeBody::default(),
        };
        let json_str = serde_json::to_string_pretty(&manifest).expect("Serialize");
        fs::write(&theme_json_path, json_str).expect("Write");

        let cfg = GuideConfig {
            pdf_theme_name: "test-book".to_string(),
            pdf_theme_data_file: Some(theme_json_path.clone()),
            ..crate::guide::default_config(&temp_dir)
        };

        let sync_res = sync_theme_yaml(&temp_dir, &cfg);
        assert!(sync_res.is_ok());
        let synced_path = sync_res.unwrap().expect("Must generate YAML");
        assert!(synced_path.is_file());

        let yaml_content = fs::read_to_string(&synced_path).expect("Read");
        assert!(yaml_content.contains("extends: default"));

        let _ = fs::remove_file(theme_json_path);
        let _ = fs::remove_file(synced_path);
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Diagnostic Self-Audit
// -----------------------------------------------------------------------------

/// Performs an in-process diagnostic preflight self-audit of theme generator invariants.
///
/// # Examples
///
/// ```rust
/// use tl_tools::guide::theme::self_audit;
///
/// assert!(self_audit().is_ok());
/// ```
pub fn self_audit() -> Result<(), String> {
    let body = GuideThemeBody::default();
    let yaml = render_theme_yaml("default", &body);
    if !yaml.contains("extends: default") {
        return Err("render_theme_yaml failed to emit extends token".to_string());
    }
    if !yaml.contains("page:") {
        return Err("render_theme_yaml failed to emit page block".to_string());
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

/// Verifies that the theme subsystem is ready for operation.
///
/// # Examples
///
/// ```rust
/// use tl_tools::guide::theme::is_theme_subsystem_ready;
///
/// assert!(is_theme_subsystem_ready());
/// ```
pub fn is_theme_subsystem_ready() -> bool {
    self_audit().is_ok()
}
