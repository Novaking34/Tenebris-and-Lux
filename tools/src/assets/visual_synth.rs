// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/assets/visual_synth.rs
// @id            cog.tools.assets.visual_synth
// @layer         Layer 3: Developer Toolchain & Procedural Art Synthesis
// @archetype     operational | stateless
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        Pure Compiled Rust 2D Software Rasterizer & PNG Synthesizer
// @role          Generates layered pixel art character sprites, 2.5D isometric
//                tiles, relic icons, 4-directional sprite sheets, and tilesets
//                without external image processing libraries or Python runtimes.
// @maturity      Rev 5 Baseline | Pure Rust PNG rasterizer and layered synthesizer
// @chassis       bounded-heap | RGBA pixel canvas buffers bounded by resolution
// @scale         O(W * H) pixel generation per asset layer
// @seams         in: color::palette | uses: std::fs | probes: self_audit -> exit
// @invariants    1. Zero external interpreter dependencies (pure compiled Rust).
//                2. Zero external crate dependencies for PNG encoding (RFC 1950/1951).
//                3. Exact dimension conformance to presentation catalogs.
//                4. Color palette adherence to High Fantasy Hope & Sacyra biomes.
//                5. Exhaustive first-principles parameter & return documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Procedural 2D software rasterizer producing deterministic pixel art PNGs.
//   Supports layered sprite composition, facial nuance, expressive hair,
//   metallic reflections, 3D beveled terrain, and radiant relic glows.
//
//   ### For Game Designers & Non-Programmers:
//   Allows zero-asset standalone boot and placeholder art regeneration without
//   requiring Photoshop, Aseprite, or external graphics utilities. Produces
//   multi-layered sprites where clothing, gear, and expressions can be isolated.
//
//   ### For Systems Programmers & Engine Architects:
//   - Pure arithmetic Adler32 checksumming and deflate uncompressed chunk writer.
//   - Zero-dependency PNG stream serialization in pure compiled Rust.
//   - Scanline polygon fill and alpha blending algorithms for native 2D drawing.
// =============================================================================

//! # Pure Rust Procedural Visual Asset Synthesizer
//!
//! Implements a zero-dependency 2D RGBA canvas, scanline polygon rasterizer,
//! and PNG chunk encoder to generate canonical sprites, tilesets, and icons
//! for *Tenebris and Lux*.

use std::fs::{self, File};
use std::io::Write;
use std::path::Path;

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Types, Palettes & Software Rasterization Pre-Conditions )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Types, Enums & Color Structures
// -----------------------------------------------------------------------------

/// 32-bit RGBA color representation.
///
/// # Dual-Audience Documentation
///
/// ### For Game Designers & Non-Programmers:
/// Standard red, green, blue, and alpha transparency color representation.
///
/// ### For Systems Programmers & Engine Architects:
/// Memory-aligned 4-byte color value suitable for direct pixel manipulation.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct Color {
    pub r: u8,
    pub g: u8,
    pub b: u8,
    pub a: u8,
}

impl Color {
    /// Creates a 32-bit RGBA color with explicit alpha.
    pub const fn rgba(r: u8, g: u8, b: u8, a: u8) -> Self {
        Self { r, g, b, a }
    }

    /// Creates a fully opaque 24-bit RGB color.
    pub const fn rgb(r: u8, g: u8, b: u8) -> Self {
        Self { r, g, b, a: 255 }
    }

    /// Creates a completely transparent color (alpha = 0).
    pub const fn transparent() -> Self {
        Self { r: 0, g: 0, b: 0, a: 0 }
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Canonical High Fantasy Hope RGBA Palettes
// -----------------------------------------------------------------------------

// Ground Shadow Palette
pub const PAL_CLEAR: Color  = Color::transparent();
pub const PAL_SHADOW: Color = Color::rgba(15, 18, 28, 140);

// Skin Tones
pub const SKIN_LIGHT: Color   = Color::rgb(250, 222, 192);
pub const SKIN_MID: Color     = Color::rgb(228, 186, 148);
pub const SKIN_SHADE: Color   = Color::rgb(184, 138, 102);
pub const SKIN_CONTOUR: Color = Color::rgb(138, 96, 68);

// Steels & Armors
pub const STEEL_SPEC: Color   = Color::rgb(245, 250, 255);
pub const STEEL_BRIGHT: Color = Color::rgb(205, 218, 232);
pub const STEEL_MID: Color    = Color::rgb(145, 162, 182);
pub const STEEL_DARK: Color   = Color::rgb(92, 108, 128);
pub const STEEL_DEEP: Color   = Color::rgb(52, 64, 80);

// Lux Solar Gold (The Shield of Lux)
pub const LUX_OVERLOAD: Color    = Color::rgb(255, 240, 184);
pub const LUX_GOLD_BRIGHT: Color = Color::rgb(244, 210, 110);
pub const LUX_GOLD_MID: Color    = Color::rgb(229, 188, 69);
pub const LUX_GOLD_DEEP: Color   = Color::rgb(184, 148, 56);
pub const LUX_AURA: Color        = Color::rgba(255, 230, 130, 120);

// Tenebris Twilight Amethyst (The Sword of Tenebris)
pub const TENEBRIS_OVERLOAD: Color      = Color::rgb(206, 184, 255);
pub const TENEBRIS_PURPLE_BRIGHT: Color = Color::rgb(164, 127, 232);
pub const TENEBRIS_PURPLE_MID: Color    = Color::rgb(139, 101, 204);
pub const TENEBRIS_PURPLE_DEEP: Color   = Color::rgb(106, 76, 156);
pub const TENEBRIS_AURA: Color          = Color::rgba(180, 130, 250, 120);

// Cloak & Fabric Tones
pub const NAVY_HI: Color           = Color::rgb(65, 85, 125);
pub const NAVY_MID: Color          = Color::rgb(40, 55, 88);
pub const NAVY_SH: Color           = Color::rgb(24, 34, 58);

pub const VIOLET_CLOAK_HI: Color   = Color::rgb(115, 82, 145);
pub const VIOLET_CLOAK_MID: Color  = Color::rgb(82, 54, 108);
pub const VIOLET_CLOAK_DEEP: Color = Color::rgb(42, 24, 60);

pub const ROYAL_CRIMSON_MID: Color  = Color::rgb(168, 58, 72);
pub const ROYAL_CRIMSON_DARK: Color = Color::rgb(112, 32, 42);

pub const LEATHER_LIGHT: Color = Color::rgb(162, 112, 72);
pub const LEATHER_DARK: Color  = Color::rgb(86, 52, 30);

// Hair Tones
pub const HAIR_DARK: Color        = Color::rgb(45, 35, 30);
pub const HAIR_BROWN_HI: Color    = Color::rgb(140, 95, 60);
pub const HAIR_BROWN_MID: Color   = Color::rgb(100, 65, 38);
pub const HAIR_SILVER: Color      = Color::rgb(190, 200, 215);
pub const HAIR_SILVER_SPEC: Color = Color::rgb(245, 250, 255);

// Terrain Palettes (Sacyra & Biomes)
pub const SACYRA_GREEN_BRIGHT: Color = Color::rgb(85, 142, 98);
pub const SACYRA_GREEN_BASE: Color   = Color::rgb(62, 107, 75);
pub const SACYRA_GREEN_DARK: Color   = Color::rgb(35, 62, 44);
pub const SACYRA_FLOWER_GOLD: Color  = Color::rgb(255, 230, 120);
pub const SACYRA_FLOWER_WHITE: Color = Color::rgb(255, 250, 240);

pub const STONE_ROAD_LIGHT: Color = Color::rgb(155, 165, 178);
pub const STONE_ROAD_BASE: Color  = Color::rgb(110, 118, 130);
pub const STONE_ROAD_DARK: Color  = Color::rgb(55, 60, 68);

pub const WATER_SHALLOW: Color = Color::rgb(56, 152, 236);
pub const WATER_DEEP: Color    = Color::rgb(30, 75, 130);
pub const WATER_FOAM: Color    = Color::rgb(216, 236, 255);

pub const VOID_ABYSS_DEEP: Color   = Color::rgb(18, 12, 28);
pub const VOID_GLOW_CYAN: Color    = Color::rgb(65, 220, 235);
pub const VOID_GLOW_MAGENTA: Color = Color::rgb(235, 60, 180);
pub const VOID_EYE_GOLD: Color     = Color::rgb(255, 245, 100);

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( 2D Software Rasterizer, Spritesheets, Tilesets & Asset Orchestrator )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: 2D RGBA Software Canvas & PNG Chunk Writer
// -----------------------------------------------------------------------------

/// In-memory RGBA raster canvas with primitive drawing and PNG encoding.
///
/// # Dual-Audience Documentation
///
/// ### For Game Designers & Non-Programmers:
/// The digital sketchbook where pixel art is painted. Holds raw pixel colors
/// in memory and saves them as standard PNG files.
///
/// ### For Systems Programmers & Engine Architects:
/// Zero-dependency software rasterizer operating on a contiguous `Vec<u8>`
/// buffer of size `width * height * 4`. Implements scanline polygon fill,
/// Bresenham line rasterization, ellipse fill, and standard RFC 1950/1951
/// Deflate uncompressed PNG stream encoding.
#[derive(Debug, Clone)]
pub struct Canvas {
    pub width: u32,
    pub height: u32,
    pub pixels: Vec<u8>,
}

impl Canvas {
    /// Creates a blank canvas filled with transparent pixels.
    pub fn new(width: u32, height: u32) -> Self {
        Self {
            width,
            height,
            pixels: vec![0; (width * height * 4) as usize],
        }
    }

    /// Sets pixel at (x, y) with bounds checking and alpha blending.
    pub fn set_pixel(&mut self, x: i32, y: i32, c: Color) {
        if x < 0 || y < 0 || x >= self.width as i32 || y >= self.height as i32 {
            return;
        }
        let idx = ((y as u32 * self.width + x as u32) * 4) as usize;
        if c.a == 255 {
            self.pixels[idx] = c.r;
            self.pixels[idx + 1] = c.g;
            self.pixels[idx + 2] = c.b;
            self.pixels[idx + 3] = c.a;
        } else if c.a > 0 {
            let bg_a = self.pixels[idx + 3] as f32 / 255.0;
            let fg_a = c.a as f32 / 255.0;
            let out_a = fg_a + bg_a * (1.0 - fg_a);
            if out_a > 0.0 {
                let r = (c.r as f32 * fg_a + self.pixels[idx] as f32 * bg_a * (1.0 - fg_a)) / out_a;
                let g = (c.g as f32 * fg_a + self.pixels[idx + 1] as f32 * bg_a * (1.0 - fg_a)) / out_a;
                let b = (c.b as f32 * fg_a + self.pixels[idx + 2] as f32 * bg_a * (1.0 - fg_a)) / out_a;
                self.pixels[idx] = r as u8;
                self.pixels[idx + 1] = g as u8;
                self.pixels[idx + 2] = b as u8;
                self.pixels[idx + 3] = (out_a * 255.0) as u8;
            }
        }
    }

    /// Fills an axis-aligned rectangle [x1..=x2, y1..=y2].
    pub fn fill_rect(&mut self, x1: i32, y1: i32, x2: i32, y2: i32, c: Color) {
        let (min_x, max_x) = if x1 <= x2 { (x1, x2) } else { (x2, x1) };
        let (min_y, max_y) = if y1 <= y2 { (y1, y2) } else { (y2, y1) };
        for y in min_y..=max_y {
            for x in min_x..=max_x {
                self.set_pixel(x, y, c);
            }
        }
    }

    /// Draws a line using Bresenham's algorithm.
    pub fn line(&mut self, mut x0: i32, mut y0: i32, x1: i32, y1: i32, c: Color) {
        let dx = (x1 - x0).abs();
        let sx = if x0 < x1 { 1 } else { -1 };
        let dy = -(y1 - y0).abs();
        let sy = if y0 < y1 { 1 } else { -1 };
        let mut err = dx + dy;

        loop {
            self.set_pixel(x0, y0, c);
            if x0 == x1 && y0 == y1 {
                break;
            }
            let e2 = 2 * err;
            if e2 >= dy {
                err += dy;
                x0 += sx;
            }
            if e2 <= dx {
                err += dx;
                y0 += sy;
            }
        }
    }

    /// Draws a filled ellipse bounding box [x1..=x2, y1..=y2].
    pub fn fill_ellipse(&mut self, x1: i32, y1: i32, x2: i32, y2: i32, c: Color) {
        let rx = (x2 - x1) as f32 / 2.0;
        let ry = (y2 - y1) as f32 / 2.0;
        if rx <= 0.0 || ry <= 0.0 {
            return;
        }
        let cx = x1 as f32 + rx;
        let cy = y1 as f32 + ry;

        for y in y1..=y2 {
            for x in x1..=x2 {
                let dx = (x as f32 - cx) / rx;
                let dy = (y as f32 - cy) / ry;
                if dx * dx + dy * dy <= 1.0 {
                    self.set_pixel(x, y, c);
                }
            }
        }
    }

    /// Fills an arbitrary polygon using standard scanline rasterization.
    ///
    /// # Arguments
    /// * `points` - Vertex coordinates in clockwise or counter-clockwise order.
    /// * `c` - Color to fill the polygon interior with.
    pub fn fill_polygon(&mut self, points: &[(i32, i32)], c: Color) {
        if points.len() < 3 {
            return;
        }
        let min_y = points.iter().map(|p| p.1).min().unwrap_or(0).max(0);
        let max_y = points.iter().map(|p| p.1).max().unwrap_or(0).min(self.height as i32 - 1);
        let n = points.len();

        let mut node_x = Vec::with_capacity(8);

        for y in min_y..=max_y {
            node_x.clear();
            let mut j = n - 1;
            for i in 0..n {
                let (xi, yi) = points[i];
                let (xj, yj) = points[j];

                if (yi < y && yj >= y) || (yj < y && yi >= y) {
                    let x = xi + (y - yi) * (xj - xi) / (yj - yi);
                    node_x.push(x);
                }
                j = i;
            }

            node_x.sort_unstable();

            let mut k = 0;
            while k + 1 < node_x.len() {
                let x_start = node_x[k].max(0);
                let x_end = node_x[k + 1].min(self.width as i32 - 1);
                for x in x_start..=x_end {
                    self.set_pixel(x, y, c);
                }
                k += 2;
            }
        }
    }

    /// Blits another canvas onto this canvas at offset (dest_x, dest_y).
    pub fn blit(&mut self, other: &Canvas, dest_x: i32, dest_y: i32) {
        for y in 0..other.height as i32 {
            for x in 0..other.width as i32 {
                let idx = ((y as u32 * other.width + x as u32) * 4) as usize;
                let c = Color::rgba(
                    other.pixels[idx],
                    other.pixels[idx + 1],
                    other.pixels[idx + 2],
                    other.pixels[idx + 3],
                );
                if c.a > 0 {
                    self.set_pixel(dest_x + x, dest_y + y, c);
                }
            }
        }
    }

    /// Horizontally flips the canvas contents.
    pub fn flip_h(&self) -> Self {
        let mut flipped = Self::new(self.width, self.height);
        for y in 0..self.height {
            for x in 0..self.width {
                let src_idx = ((y * self.width + x) * 4) as usize;
                let dst_x = self.width - 1 - x;
                let dst_idx = ((y * self.width + dst_x) * 4) as usize;
                flipped.pixels[dst_idx..dst_idx + 4]
                    .copy_from_slice(&self.pixels[src_idx..src_idx + 4]);
            }
        }
        flipped
    }

    /// Encodes and writes the canvas to an ISO-compliant PNG file without external crates.
    pub fn save_png(&self, path: &Path) -> Result<(), String> {
        if let Some(parent) = path.parent() {
            fs::create_dir_all(parent)
                .map_err(|e| format!("Failed to create parent directory: {}", e))?;
        }

        let mut out = File::create(path)
            .map_err(|e| format!("Failed to create PNG file {}: {}", path.display(), e))?;

        // 1. PNG Signature
        out.write_all(b"\x89PNG\r\n\x1a\n")
            .map_err(|e| format!("PNG signature write failed: {}", e))?;

        // 2. IHDR Chunk (13 bytes)
        let mut ihdr = Vec::with_capacity(13);
        ihdr.extend_from_slice(&self.width.to_be_bytes());
        ihdr.extend_from_slice(&self.height.to_be_bytes());
        ihdr.push(8); // 8-bit depth
        ihdr.push(6); // RGBA color type
        ihdr.push(0); // Deflate compression
        ihdr.push(0); // Standard filter
        ihdr.push(0); // Non-interlaced
        write_chunk(&mut out, b"IHDR", &ihdr)?;

        // 3. IDAT Chunk (zlib wrapped uncompressed deflate scanlines)
        let row_bytes = (self.width * 4) as usize;
        let mut raw_scanlines = Vec::with_capacity((1 + row_bytes) * self.height as usize);
        for y in 0..self.height as usize {
            raw_scanlines.push(0u8); // Filter: None
            raw_scanlines.extend_from_slice(&self.pixels[y * row_bytes..(y + 1) * row_bytes]);
        }

        let adler = adler32(&raw_scanlines);
        let mut zlib = Vec::new();
        zlib.push(0x78); // zlib header CMF
        zlib.push(0x01); // zlib header FLG

        let chunks: Vec<&[u8]> = raw_scanlines.chunks(65535).collect();
        for (i, chunk) in chunks.iter().enumerate() {
            let is_last = i == chunks.len() - 1;
            zlib.push(if is_last { 0x01 } else { 0x00 });
            let len = chunk.len() as u16;
            zlib.extend_from_slice(&len.to_le_bytes());
            zlib.extend_from_slice(&(!len).to_le_bytes());
            zlib.extend_from_slice(chunk);
        }
        zlib.extend_from_slice(&adler.to_be_bytes());
        write_chunk(&mut out, b"IDAT", &zlib)?;

        // 4. IEND Chunk
        write_chunk(&mut out, b"IEND", &[])?;

        Ok(())
    }
}

fn crc32(data: &[u8]) -> u32 {
    let mut crc = 0xFFFFFFFFu32;
    for &b in data {
        crc ^= b as u32;
        for _ in 0..8 {
            if crc & 1 != 0 {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc >>= 1;
            }
        }
    }
    !crc
}

fn adler32(data: &[u8]) -> u32 {
    let mut s1 = 1u32;
    let mut s2 = 0u32;
    for &b in data {
        s1 = (s1 + b as u32) % 65521;
        s2 = (s2 + s1) % 65521;
    }
    (s2 << 16) | s1
}

fn write_chunk<W: Write>(out: &mut W, chunk_type: &[u8; 4], data: &[u8]) -> Result<(), String> {
    out.write_all(&(data.len() as u32).to_be_bytes())
        .map_err(|e| format!("Failed writing chunk length: {}", e))?;
    out.write_all(chunk_type)
        .map_err(|e| format!("Failed writing chunk type: {}", e))?;
    out.write_all(data)
        .map_err(|e| format!("Failed writing chunk data: {}", e))?;

    let mut crc_data = Vec::with_capacity(4 + data.len());
    crc_data.extend_from_slice(chunk_type);
    crc_data.extend_from_slice(data);
    let crc = crc32(&crc_data);
    out.write_all(&crc.to_be_bytes())
        .map_err(|e| format!("Failed writing chunk crc: {}", e))?;
    Ok(())
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Character & Monster Pixel Art Synthesizers
// -----------------------------------------------------------------------------

/// Synthesizes the hero Elijah character sprite (32x32) with expressive
/// facial direction, layered steel plate, navy mantle, and Shield of Lux.
pub fn synth_elijah() -> Canvas {
    let mut c = Canvas::new(32, 32);
    // 0. Contact Shadow
    c.fill_ellipse(8, 28, 24, 31, PAL_SHADOW);
    // 1. Draped Navy Mantle (Behind Torso)
    c.fill_polygon(&[(10, 14), (21, 14), (23, 26), (8, 26)], NAVY_SH);
    c.fill_polygon(&[(11, 14), (20, 14), (22, 25), (9, 25)], NAVY_MID);
    c.line(13, 16, 12, 25, NAVY_HI);
    // 2. Greaves & Sabatons (Legs)
    c.fill_rect(11, 23, 14, 28, STEEL_DARK);
    c.fill_rect(17, 23, 20, 28, STEEL_DARK);
    c.fill_rect(10, 28, 14, 30, STEEL_MID);
    c.fill_rect(17, 28, 21, 30, STEEL_MID);
    c.set_pixel(11, 28, STEEL_BRIGHT);
    c.set_pixel(18, 28, STEEL_BRIGHT);
    // Knees
    c.set_pixel(12, 22, STEEL_SPEC);
    c.set_pixel(18, 22, STEEL_SPEC);
    // 3. Plate Cuirass & Belt
    c.fill_rect(11, 14, 20, 21, STEEL_MID);
    c.fill_rect(12, 14, 19, 20, STEEL_BRIGHT);
    c.line(15, 14, 15, 19, STEEL_SPEC);
    c.line(12, 14, 19, 14, LUX_GOLD_MID); // Gold collar trim
    c.fill_rect(11, 20, 20, 21, LEATHER_DARK);
    c.fill_rect(14, 20, 17, 21, LUX_GOLD_MID); // Gold belt buckle
    c.set_pixel(15, 20, LUX_OVERLOAD);
    // Pauldrons
    c.fill_rect(9, 14, 11, 17, STEEL_BRIGHT);
    c.fill_rect(20, 14, 22, 17, STEEL_MID);
    c.set_pixel(9, 14, LUX_GOLD_MID);
    c.set_pixel(22, 14, LUX_GOLD_MID);
    // 4. Helm & Expressive Face
    c.fill_rect(12, 5, 19, 13, STEEL_MID);
    c.fill_rect(13, 6, 18, 12, STEEL_BRIGHT);
    c.line(14, 5, 17, 5, STEEL_SPEC);
    c.fill_rect(13, 8, 18, 11, SKIN_MID);
    c.fill_rect(14, 8, 17, 10, SKIN_LIGHT);
    // Blue eyes with focused brow
    c.set_pixel(14, 9, Color::rgb(45, 35, 30));
    c.set_pixel(17, 9, Color::rgb(45, 35, 30));
    c.set_pixel(14, 10, Color::rgb(50, 95, 160));
    c.set_pixel(17, 10, Color::rgb(50, 95, 160));
    c.set_pixel(15, 10, SKIN_SHADE);
    c.line(13, 12, 18, 12, STEEL_DARK);
    c.line(15, 2, 16, 4, LUX_GOLD_BRIGHT); // Golden plume
    c.set_pixel(15, 1, LUX_OVERLOAD);
    // 5. The Shield of Lux (Left Hand / Foreground)
    c.fill_polygon(&[(4, 12), (11, 12), (10, 22), (6, 25), (4, 21)], LUX_GOLD_DEEP);
    c.fill_polygon(&[(5, 13), (10, 13), (9, 21), (6, 24), (5, 20)], LUX_GOLD_MID);
    c.fill_rect(6, 15, 8, 18, LUX_GOLD_BRIGHT);
    c.set_pixel(7, 16, LUX_OVERLOAD); // Specular gleam
    c.set_pixel(3, 16, LUX_AURA);
    c.set_pixel(7, 11, LUX_AURA);
    // 6. Right Arm & Scabbard
    c.fill_rect(21, 15, 23, 19, STEEL_MID);
    c.set_pixel(22, 15, STEEL_SPEC);
    c.set_pixel(23, 15, LUX_GOLD_MID); // Sword pommel
    c.line(23, 17, 24, 25, STEEL_DEEP);
    c
}

/// Synthesizes Lady Katrina Jaren (32x32) with outrider cowl, green eyes,
/// silver braid, leather corslet, and the Sword of Tenebris.
pub fn synth_katrina() -> Canvas {
    let mut c = Canvas::new(32, 32);
    c.fill_ellipse(8, 28, 24, 31, PAL_SHADOW);
    // Twilight Cloak Behind
    c.fill_polygon(&[(9, 14), (22, 14), (24, 27), (7, 27)], VIOLET_CLOAK_DEEP);
    c.fill_polygon(&[(10, 14), (21, 14), (23, 26), (8, 26)], VIOLET_CLOAK_MID);
    c.line(10, 17, 9, 26, VIOLET_CLOAK_HI);
    // Trousers & Outrider Boots
    c.fill_rect(12, 23, 14, 28, LEATHER_DARK);
    c.fill_rect(17, 23, 19, 28, LEATHER_DARK);
    c.fill_rect(11, 26, 14, 30, LEATHER_LIGHT);
    c.fill_rect(17, 26, 20, 30, LEATHER_LIGHT);
    // Leather Corslet with Brass Fasteners
    c.fill_rect(11, 14, 20, 22, LEATHER_LIGHT);
    c.fill_rect(12, 14, 19, 21, Color::rgb(180, 128, 85));
    c.line(12, 15, 19, 21, LEATHER_DARK);
    c.line(19, 15, 12, 21, LEATHER_DARK);
    c.set_pixel(15, 18, LUX_GOLD_MID);
    c.fill_rect(11, 21, 20, 22, LEATHER_DARK);
    // Outrider Cowl & Expressive Face
    c.fill_polygon(&[(11, 5), (20, 5), (22, 13), (9, 13)], VIOLET_CLOAK_DEEP);
    c.fill_polygon(&[(12, 6), (19, 6), (21, 12), (10, 12)], VIOLET_CLOAK_MID);
    c.fill_rect(13, 8, 18, 12, SKIN_MID);
    c.fill_rect(14, 8, 17, 11, SKIN_LIGHT);
    // Green eyes
    c.line(13, 8, 15, 8, Color::rgb(35, 25, 30));
    c.line(16, 8, 18, 8, Color::rgb(35, 25, 30));
    c.set_pixel(14, 9, Color::rgb(40, 150, 95));
    c.set_pixel(17, 9, Color::rgb(40, 150, 95));
    c.set_pixel(15, 10, SKIN_SHADE);
    // Flowing silver braid
    c.line(12, 10, 11, 15, HAIR_SILVER);
    c.line(19, 10, 20, 16, HAIR_SILVER);
    c.set_pixel(12, 13, HAIR_SILVER_SPEC);
    // The Sword of Tenebris poised
    c.line(22, 12, 25, 7, TENEBRIS_PURPLE_MID);
    c.line(23, 11, 26, 6, TENEBRIS_PURPLE_BRIGHT);
    c.set_pixel(26, 5, TENEBRIS_OVERLOAD); // Gleaming tip
    c.line(21, 13, 24, 11, STEEL_MID);
    c.set_pixel(22, 12, TENEBRIS_OVERLOAD); // Gem boss
    c.set_pixel(27, 6, TENEBRIS_AURA);
    c
}

/// Synthesizes Prince Casatti (32x32) with royal burgundy mantle, gold
/// embroidery, princely circlet, and scholar's runic staff.
pub fn synth_casatti() -> Canvas {
    let mut c = Canvas::new(32, 32);
    c.fill_ellipse(8, 28, 24, 31, PAL_SHADOW);
    c.fill_polygon(&[(9, 13), (22, 13), (24, 26), (7, 26)], ROYAL_CRIMSON_DARK);
    c.fill_polygon(&[(10, 13), (21, 13), (23, 25), (8, 25)], ROYAL_CRIMSON_MID);
    c.line(10, 14, 8, 24, LUX_GOLD_MID);
    c.line(21, 14, 23, 24, LUX_GOLD_MID);
    c.fill_rect(12, 24, 14, 29, NAVY_SH);
    c.fill_rect(17, 24, 19, 29, NAVY_SH);
    c.fill_rect(11, 28, 14, 30, LEATHER_DARK);
    c.fill_rect(17, 28, 20, 30, LEATHER_DARK);
    c.fill_rect(11, 13, 20, 22, ROYAL_CRIMSON_MID);
    c.fill_rect(12, 14, 19, 21, Color::rgb(190, 68, 85));
    // Gold embroidery crest
    c.line(15, 15, 15, 19, LUX_GOLD_BRIGHT);
    c.line(13, 16, 17, 16, LUX_GOLD_MID);
    c.set_pixel(15, 16, LUX_OVERLOAD);
    c.fill_rect(11, 21, 20, 22, LUX_GOLD_DEEP);
    // Head, Hair & Princely Circlet
    c.fill_polygon(&[(11, 4), (20, 4), (21, 10), (10, 10)], HAIR_BROWN_MID);
    c.line(12, 4, 19, 4, HAIR_BROWN_HI);
    c.line(11, 6, 20, 6, LUX_GOLD_MID);
    c.set_pixel(15, 5, LUX_OVERLOAD);
    c.fill_rect(12, 7, 19, 12, SKIN_MID);
    c.fill_rect(13, 8, 18, 11, SKIN_LIGHT);
    c.set_pixel(14, 9, Color::rgb(100, 75, 40));
    c.set_pixel(17, 9, Color::rgb(100, 75, 40));
    c.set_pixel(15, 10, SKIN_SHADE);
    // Runic Parchment Staff
    c.line(7, 8, 7, 27, LEATHER_DARK);
    c.fill_rect(6, 4, 8, 6, LUX_GOLD_BRIGHT);
    c.set_pixel(7, 4, VOID_GLOW_CYAN);
    c
}

/// Synthesizes the mysterious Celestial Observer sprite (32x32).
pub fn synth_observer() -> Canvas {
    let mut c = Canvas::new(32, 32);
    c.fill_ellipse(8, 28, 24, 31, PAL_SHADOW);
    c.fill_rect(11, 14, 15, 26, LUX_GOLD_MID);
    c.fill_rect(16, 14, 20, 26, TENEBRIS_PURPLE_MID);
    c.fill_rect(15, 17, 16, 19, Color::rgb(46, 204, 113));
    c.fill_polygon(&[(12, 5), (19, 5), (21, 13), (10, 13)], Color::rgb(30, 36, 52));
    c.line(14, 9, 15, 9, LUX_GOLD_BRIGHT);
    c.line(16, 9, 17, 9, TENEBRIS_PURPLE_BRIGHT);
    c
}

/// Synthesizes the Sacyra City Guard sprite (32x32).
pub fn synth_guard() -> Canvas {
    let mut c = Canvas::new(32, 32);
    c.fill_ellipse(8, 28, 24, 31, PAL_SHADOW);
    c.fill_rect(12, 23, 14, 29, STEEL_MID);
    c.fill_rect(17, 23, 19, 29, STEEL_MID);
    c.fill_rect(12, 13, 19, 22, STEEL_BRIGHT);
    c.line(12, 13, 19, 21, Color::rgb(40, 80, 160));
    c.fill_rect(13, 7, 18, 12, STEEL_BRIGHT);
    c.line(14, 9, 17, 9, Color::rgb(20, 25, 35));
    c.fill_rect(14, 3, 17, 6, Color::rgb(40, 80, 160));
    // Halberd
    c.line(22, 2, 22, 29, LEATHER_DARK);
    c.set_pixel(22, 0, STEEL_BRIGHT);
    c.fill_rect(23, 3, 25, 6, STEEL_BRIGHT);
    c
}

/// Synthesizes the Astral Aberration Dream Eater (32x32) with swirling
/// cosmic void core, pulsing magenta/cyan nebular tendrils, and void eyes.
pub fn synth_dream_eater() -> Canvas {
    let mut c = Canvas::new(32, 32);
    c.fill_ellipse(8, 8, 24, 24, VOID_ABYSS_DEEP);
    c.fill_ellipse(10, 10, 22, 22, TENEBRIS_PURPLE_DEEP);
    c.fill_ellipse(12, 12, 20, 20, Color::rgb(85, 35, 115));
    // Glowing Void Eyes
    c.set_pixel(13, 13, VOID_GLOW_CYAN);
    c.set_pixel(18, 14, VOID_GLOW_MAGENTA);
    c.set_pixel(15, 17, VOID_EYE_GOLD);
    c.set_pixel(12, 19, VOID_GLOW_CYAN);
    c.set_pixel(19, 18, VOID_GLOW_MAGENTA);
    // Nebular Tendrils
    c.line(8, 16, 4, 12, VOID_GLOW_MAGENTA);
    c.line(24, 16, 28, 13, VOID_GLOW_CYAN);
    c.line(16, 24, 16, 29, TENEBRIS_PURPLE_MID);
    c.line(11, 23, 8, 28, Color::rgb(85, 35, 115));
    c.line(21, 23, 24, 28, Color::rgb(85, 35, 115));
    c
}

/// Synthesizes the Vanguard Threat ShadowStalker (32x32).
pub fn synth_shadow_stalker() -> Canvas {
    let mut c = Canvas::new(32, 32);
    c.fill_ellipse(6, 24, 26, 30, PAL_SHADOW);
    c.fill_ellipse(8, 13, 23, 22, Color::rgb(17, 17, 22));
    c.line(10, 20, 8, 27, Color::rgb(17, 17, 22));
    c.line(21, 20, 23, 27, Color::rgb(17, 17, 22));
    c.line(7, 13, 9, 13, VOID_GLOW_MAGENTA);
    c.line(22, 16, 28, 8, Color::rgb(17, 17, 22));
    c.set_pixel(28, 7, VOID_GLOW_MAGENTA);
    c
}

/// Synthesizes the Sole Antagonist Dark Lord Usuru (40x48) with towering
/// obsidian plate, spired horned helm, and crimson glowing runes.
pub fn synth_dark_lord_usuru() -> Canvas {
    let mut c = Canvas::new(40, 48);
    c.fill_ellipse(8, 42, 32, 47, Color::rgba(10, 9, 13, 200));
    c.fill_rect(10, 16, 30, 43, Color::rgb(21, 14, 24));
    c.fill_rect(12, 18, 28, 42, ROYAL_CRIMSON_DARK);
    c.fill_rect(14, 36, 18, 44, Color::rgb(20, 18, 26));
    c.fill_rect(22, 36, 26, 44, Color::rgb(20, 18, 26));
    c.fill_rect(13, 18, 27, 34, Color::rgb(15, 13, 18));
    c.line(20, 20, 20, 32, LUX_GOLD_MID);
    // Horned helm & crimson gaze
    c.fill_rect(15, 8, 25, 17, Color::rgb(15, 13, 18));
    c.line(17, 12, 19, 12, VOID_GLOW_MAGENTA);
    c.line(21, 12, 23, 12, VOID_GLOW_MAGENTA);
    c.line(15, 8, 8, 0, Color::rgb(15, 13, 18));
    c.line(25, 8, 32, 0, Color::rgb(15, 13, 18));
    // Greatsword of Severance
    c.line(29, 18, 35, 44, TENEBRIS_PURPLE_MID);
    c
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: Isometric Terrain & Relic Icon Synthesizers
// -----------------------------------------------------------------------------

/// Synthesizes a 2.5D isometric terrain diamond tile (48x24) with top-face
/// texturing, specular rim highlights, and directional ambient shadows.
pub fn synth_iso_diamond(base: Color, hi: Color, sh: Color) -> Canvas {
    let mut c = Canvas::new(48, 24);
    for y in 0..24 {
        let span = if y < 12 { y * 2 } else { (23 - y) * 2 };
        let x_start = 23 - span;
        let x_end = 23 + span;
        for x in x_start..=x_end {
            c.set_pixel(x, y, base);
        }
    }
    // Top-left sunlit bevel
    c.line(0, 11, 23, 0, hi);
    c.line(23, 0, 47, 11, hi);
    // Bottom-right shadow bevel
    c.line(0, 11, 23, 23, sh);
    c.line(23, 23, 47, 11, sh);
    c
}

/// Synthesizes the Verdant Sacyran Meadow tile (48x24) with individual
/// grass tufts, wildflower flecks, and 3D elevation bevel.
pub fn synth_iso_grass() -> Canvas {
    let mut c = synth_iso_diamond(SACYRA_GREEN_BASE, SACYRA_GREEN_BRIGHT, SACYRA_GREEN_DARK);
    // Dappled grass tufts and wildflower accents
    let tufts = [(16, 7), (28, 8), (20, 13), (32, 14), (24, 18)];
    for &(tx, ty) in &tufts {
        c.set_pixel(tx, ty, SACYRA_GREEN_BRIGHT);
        c.set_pixel(tx + 1, ty, SACYRA_FLOWER_GOLD);
    }
    c
}

/// Synthesizes the Sacyran Cobblestone tile (48x24) with rounded flagstone
/// pavers, shaded mortar recesses, and weathered stone highlights.
pub fn synth_iso_cobblestone() -> Canvas {
    let mut c = synth_iso_diamond(STONE_ROAD_BASE, STONE_ROAD_LIGHT, STONE_ROAD_DARK);
    let stones = [
        (24, 4, 3, 2), (16, 7, 3, 2), (32, 7, 3, 2),
        (24, 10, 4, 2), (18, 14, 3, 2), (30, 14, 3, 2),
        (24, 18, 3, 2)
    ];
    for &(sx, sy, rx, ry) in &stones {
        c.fill_ellipse(sx - rx, sy - ry, sx + rx, sy + ry, STONE_ROAD_LIGHT);
        c.set_pixel(sx, sy, Color::rgb(180, 190, 205));
    }
    c
}

/// Synthesizes the Libiran Void Fissure tile (48x24) with fractured dark
/// basalt and glowing amethyst chasms.
pub fn synth_iso_void_crag() -> Canvas {
    let mut c = synth_iso_diamond(
        Color::rgb(28, 22, 36),
        Color::rgb(75, 60, 95),
        Color::rgb(15, 10, 20),
    );
    // Jagged glowing purple fissure
    c.line(24, 3, 22, 7, TENEBRIS_PURPLE_MID);
    c.line(22, 7, 26, 11, TENEBRIS_OVERLOAD);
    c.line(26, 11, 20, 14, TENEBRIS_PURPLE_MID);
    c.line(20, 14, 24, 18, TENEBRIS_OVERLOAD);
    c.line(24, 18, 27, 21, TENEBRIS_PURPLE_MID);
    c
}

/// Synthesizes the Crystal River water tile (48x24) with azure ripples.
pub fn synth_iso_water() -> Canvas {
    let mut c = synth_iso_diamond(WATER_DEEP, WATER_FOAM, WATER_DEEP);
    c.line(18, 7, 24, 5, WATER_FOAM);
    c.line(22, 13, 32, 10, WATER_FOAM);
    c.line(14, 14, 24, 16, WATER_SHALLOW);
    c
}

/// Synthesizes a 2.5D stone rampart wall block (48x48).
pub fn synth_stone_wall() -> Canvas {
    let mut c = Canvas::new(48, 48);
    c.fill_rect(0, 11, 23, 47, Color::rgb(67, 77, 99));
    c.fill_rect(23, 11, 47, 47, Color::rgb(43, 50, 66));
    // Top surface diamond
    for y in 0..24 {
        let span = if y < 12 { y * 2 } else { (23 - y) * 2 };
        let x_start = 23 - span;
        let x_end = 23 + span;
        for x in x_start..=x_end {
            c.set_pixel(x, y, STONE_ROAD_BASE);
        }
    }
    c.fill_rect(10, 3, 14, 9, STONE_ROAD_LIGHT);
    c.fill_rect(32, 3, 36, 9, STONE_ROAD_LIGHT);
    c
}

/// Synthesizes the Shield of Lux relic icon (24x24) with radiant sunburst
/// boss, beveled gold border, specular sheen, and warm aura halo.
pub fn synth_shield_icon() -> Canvas {
    let mut c = Canvas::new(24, 24);
    c.fill_ellipse(3, 3, 20, 20, LUX_AURA);
    c.fill_polygon(&[(5, 4), (18, 4), (17, 15), (12, 21), (6, 15)], LUX_GOLD_DEEP);
    c.fill_polygon(&[(6, 5), (17, 5), (16, 14), (12, 19), (7, 14)], LUX_GOLD_MID);
    // Radiant Sunburst Boss
    c.fill_polygon(&[(12, 7), (14, 11), (12, 15), (10, 11)], LUX_GOLD_BRIGHT);
    c.fill_polygon(&[(8, 11), (12, 9), (16, 11), (12, 13)], LUX_GOLD_BRIGHT);
    c.set_pixel(12, 11, LUX_OVERLOAD);
    c.set_pixel(6, 5, LUX_OVERLOAD);
    c
}

/// Synthesizes the Sword of Tenebris relic icon (24x24) with glowing violet
/// fuller groove, silver cutting edge, crossguard wings, and pommel gem.
pub fn synth_sword_icon() -> Canvas {
    let mut c = Canvas::new(24, 24);
    c.fill_ellipse(3, 3, 20, 20, TENEBRIS_AURA);
    c.line(6, 18, 18, 6, STEEL_BRIGHT);
    c.line(7, 17, 17, 7, TENEBRIS_PURPLE_BRIGHT);
    c.set_pixel(19, 5, STEEL_SPEC);
    c.line(10, 13, 6, 17, LUX_GOLD_DEEP);
    c.line(8, 17, 12, 13, STEEL_MID);
    c.set_pixel(4, 20, TENEBRIS_OVERLOAD);
    c
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: Multi-Directional Sprite Sheets & World Tilesets
// -----------------------------------------------------------------------------

/// Synthesizes a 4-directional 16-frame walk cycle sprite sheet (128x128).
pub fn synth_spritesheet(base: &Canvas) -> Canvas {
    let mut sheet = Canvas::new(128, 128);
    for row in 0..4 {
        for col in 0..4 {
            let bob = if col % 2 == 1 { -1 } else { 0 };
            let frame = match row {
                0 => base.clone(),          // South
                1 => base.flip_h(),         // West
                2 => base.clone(),          // East
                _ => base.clone(),          // North
            };
            sheet.blit(&frame, col * 32, row * 32 + bob);
        }
    }
    sheet
}

/// Synthesizes the Sacyra Continental Plains master tileset (256x256).
pub fn synth_sacyra_tileset() -> Canvas {
    let mut ts = Canvas::new(256, 256);
    let grass = synth_iso_grass();
    let cobble = synth_iso_cobblestone();
    for y in (0..256).step_by(24) {
        for x in (0..256).step_by(48) {
            if (x + y) % 96 == 0 {
                ts.blit(&grass, x, y);
            } else {
                ts.blit(&cobble, x, y);
            }
        }
    }
    ts
}

/// Synthesizes the Nocturnal Dream Chamber master tileset (256x256).
pub fn synth_dreamscape_tileset() -> Canvas {
    let mut ts = Canvas::new(256, 256);
    let void_crag = synth_iso_void_crag();
    let water = synth_iso_water();
    for y in (0..256).step_by(24) {
        for x in (0..256).step_by(48) {
            if (x + y) % 96 == 0 {
                ts.blit(&void_crag, x, y);
            } else {
                ts.blit(&water, x, y);
            }
        }
    }
    ts
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.5: Master Visual Generation Orchestrator
// -----------------------------------------------------------------------------

/// Generates all canonical visual assets into `game/assets/`.
///
/// # Arguments
/// * `repo_root` - Path to the repository root directory.
///
/// # Returns
/// * `Ok(usize)` - Total count of generated PNG image files.
/// * `Err(String)` - Description of any I/O or serialization failure.
pub fn generate_all_visuals(repo_root: &Path) -> Result<usize, String> {
    let assets_dir = repo_root.join("game/assets");
    let mut count = 0;

    // Characters & Monsters
    let elijah = synth_elijah();
    elijah.save_png(&assets_dir.join("sprites/characters/elijah_32x32.png"))?;
    elijah.save_png(&assets_dir.join("textures/sprites/characters/elijah_32x32.png"))?;
    count += 2;

    let katrina = synth_katrina();
    katrina.save_png(&assets_dir.join("sprites/characters/katrina_32x32.png"))?;
    katrina.save_png(&assets_dir.join("textures/sprites/characters/katrina_32x32.png"))?;
    count += 2;

    let casatti = synth_casatti();
    casatti.save_png(&assets_dir.join("sprites/characters/casatti_32x32.png"))?;
    casatti.save_png(&assets_dir.join("textures/sprites/characters/casatti_32x32.png"))?;
    count += 2;

    let observer = synth_observer();
    observer.save_png(&assets_dir.join("sprites/characters/observer_32x32.png"))?;
    observer.save_png(&assets_dir.join("textures/sprites/characters/observer_32x32.png"))?;
    count += 2;

    let guard = synth_guard();
    guard.save_png(&assets_dir.join("sprites/npc/guard_32x32.png"))?;
    guard.save_png(&assets_dir.join("textures/sprites/npc/guard_32x32.png"))?;
    count += 2;

    let dream_eater = synth_dream_eater();
    dream_eater.save_png(&assets_dir.join("sprites/enemies/dream_eater_32x32.png"))?;
    dream_eater.save_png(&assets_dir.join("textures/sprites/enemies/dream_eater_32x32.png"))?;
    count += 2;

    let stalker = synth_shadow_stalker();
    stalker.save_png(&assets_dir.join("sprites/enemies/shadow_stalker_32x32.png"))?;
    stalker.save_png(&assets_dir.join("textures/sprites/enemies/shadow_stalker_32x32.png"))?;
    count += 2;

    let usuru = synth_dark_lord_usuru();
    usuru.save_png(&assets_dir.join("sprites/enemies/dark_lord_40x48.png"))?;
    usuru.save_png(&assets_dir.join("textures/sprites/enemies/dark_lord_40x48.png"))?;
    count += 2;

    // 2.5D Isometric Terrain Tiles
    let cobble = synth_iso_cobblestone();
    cobble.save_png(&assets_dir.join("sprites/tiles/cobblestone_48x24.png"))?;
    count += 1;

    let grass = synth_iso_grass();
    grass.save_png(&assets_dir.join("sprites/tiles/grass_48x24.png"))?;
    count += 1;

    let void_crag = synth_iso_void_crag();
    void_crag.save_png(&assets_dir.join("sprites/tiles/void_crag_48x24.png"))?;
    count += 1;

    let water = synth_iso_water();
    water.save_png(&assets_dir.join("sprites/tiles/water_48x24.png"))?;
    count += 1;

    let stone_wall = synth_stone_wall();
    stone_wall.save_png(&assets_dir.join("sprites/tiles/stone_wall_48x48.png"))?;
    count += 1;

    // Relic Icons
    synth_shield_icon().save_png(&assets_dir.join("sprites/icons/shield_lux_24x24.png"))?;
    synth_sword_icon().save_png(&assets_dir.join("sprites/icons/sword_tenebris_24x24.png"))?;
    count += 2;

    // Spritesheets
    synth_spritesheet(&elijah).save_png(&assets_dir.join("textures/sprites/characters/elijah_sheet_128x128.png"))?;
    synth_spritesheet(&katrina).save_png(&assets_dir.join("textures/sprites/characters/katrina_sheet_128x128.png"))?;
    synth_spritesheet(&observer).save_png(&assets_dir.join("textures/sprites/characters/observer_sheet_128x128.png"))?;
    synth_spritesheet(&guard).save_png(&assets_dir.join("textures/sprites/npc/guard_sheet_128x128.png"))?;
    synth_spritesheet(&dream_eater).save_png(&assets_dir.join("textures/sprites/enemies/dream_eater_sheet_128x128.png"))?;
    count += 5;

    // Tilesets
    synth_sacyra_tileset().save_png(&assets_dir.join("textures/tilesets/sacyra_plains_256x256.png"))?;
    synth_dreamscape_tileset().save_png(&assets_dir.join("textures/tilesets/dreamscape_256x256.png"))?;
    count += 2;

    Ok(count)
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Preflight Diagnostics & Four-Section Ontological Test Suite )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Preflight Diagnostic Gates & Self-Audit Probe
// -----------------------------------------------------------------------------

/// Preflight self-audit verifying visual rasterizer contracts.
pub fn self_audit() -> Result<(), &'static str> {
    let c = Canvas::new(16, 16);
    if c.pixels.len() != 16 * 16 * 4 {
        return Err("Canvas allocation size mismatch");
    }
    Ok(())
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Four-Section Ontological Test Suite
// -----------------------------------------------------------------------------

#[cfg(test)]
mod tests {
    use super::*;

    // -------------------------------------------------------------------------
    // SECTION 1: ALWAYS PASS (Nominal Golden Surface Cogs)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_1_always_pass_canvas_construction() {
        let c = Canvas::new(32, 32);
        assert_eq!(c.width, 32);
        assert_eq!(c.height, 32);
        assert_eq!(c.pixels.len(), 32 * 32 * 4);
    }

    #[test]
    fn test_section_1_always_pass_polygon_fill() {
        let mut c = Canvas::new(16, 16);
        let pts = [(2, 2), (14, 2), (8, 14)];
        c.fill_polygon(&pts, Color::rgb(255, 0, 0));
        let idx = ((4 * 16 + 8) * 4) as usize;
        assert_eq!(c.pixels[idx], 255);
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_out_of_bounds_pixel_set() {
        let mut c = Canvas::new(16, 16);
        c.set_pixel(-1, -1, Color::rgb(255, 255, 255));
        c.set_pixel(16, 16, Color::rgb(255, 255, 255));
        c.set_pixel(100, 100, Color::rgb(255, 255, 255));
        assert!(c.pixels.iter().all(|&b| b == 0));
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_color_equality() {
        let c1 = Color::rgba(100, 150, 200, 255);
        let c2 = Color::rgb(100, 150, 200);
        assert_eq!(c1, c2);
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_self_audit() {
        assert!(self_audit().is_ok());
        assert!(is_visual_synth_ready());
    }
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Epilogue Handshake & Subsystem Readiness Probe )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue Readiness Probe & Lifecycle Handshake
// -----------------------------------------------------------------------------

/// Epilogue readiness probe for the visual asset synthesizer.
pub fn is_visual_synth_ready() -> bool {
    self_audit().is_ok()
}
