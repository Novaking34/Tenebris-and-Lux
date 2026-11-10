// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/assets/visual_synth.rs
// @id            tools.assets.visual_synth
// @layer         Layer 3: Developer Toolchain & Procedural Art Synthesis
// @version       0.1.0-dev
// @interface_rev Rev 4
// @date          2026-11-08
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        Pure Compiled Rust 2D Software Rasterizer & PNG Synthesizer
// @role          Generates pixel art character sprites, isometric tiles, relic
//                icons, 4-directional sprite sheets, and tilesets without
//                external image processing libraries or Python runtimes.
// @invariants    1. Zero external interpreter dependencies (pure compiled Rust).
//                2. Zero external crate dependencies for PNG encoding (RFC 1950/1951).
//                3. Exact dimension conformance to presentation catalogs.
//                4. Color palette adherence to High Fantasy Hope & Sacyra biomes.
// =============================================================================

//! # Pure Rust Procedural Visual Asset Synthesizer
//!
//! Implements a zero-dependency 2D RGBA canvas and PNG chunk encoder to generate
//! canonical sprites, tilesets, and icons for *Tenebris and Lux*.

use std::fs::{self, File};
use std::io::Write;
use std::path::Path;

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES & PALETTES
// Stratum: Developer Tooling | Machine: tools.assets.visual_synth
// =============================================================================

/// 32-bit RGBA color representation.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct Color {
    pub r: u8,
    pub g: u8,
    pub b: u8,
    pub a: u8,
}

impl Color {
    pub const fn rgba(r: u8, g: u8, b: u8, a: u8) -> Self {
        Self { r, g, b, a }
    }

    pub const fn rgb(r: u8, g: u8, b: u8) -> Self {
        Self { r, g, b, a: 255 }
    }

    pub const fn transparent() -> Self {
        Self { r: 0, g: 0, b: 0, a: 0 }
    }
}

// Canonical High Fantasy Hope Palettes
pub const LUX_GOLD_BRIGHT: Color = Color::rgb(255, 240, 184);
pub const LUX_GOLD_MID: Color    = Color::rgb(240, 206, 120);
pub const LUX_GOLD_DEEP: Color   = Color::rgb(140, 112, 48);

pub const TENEBRIS_PURPLE_BRIGHT: Color = Color::rgb(206, 184, 255);
pub const TENEBRIS_PURPLE_MID: Color    = Color::rgb(138, 120, 200);
pub const TENEBRIS_PURPLE_DEEP: Color   = Color::rgb(69, 48, 102);

pub const STEEL_BRIGHT: Color = Color::rgb(216, 225, 232);
pub const STEEL_MID: Color    = Color::rgb(125, 139, 153);
pub const STEEL_DARK: Color   = Color::rgb(56, 66, 77);

pub const SKIN_LIGHT: Color = Color::rgb(250, 220, 190);
pub const SKIN_SHADE: Color = Color::rgb(220, 180, 150);

pub const HAIR_DARK: Color   = Color::rgb(45, 35, 30);
pub const HAIR_SILVER: Color = Color::rgb(200, 210, 220);

pub const LEATHER_LIGHT: Color = Color::rgb(140, 100, 60);
pub const LEATHER_DARK: Color  = Color::rgb(84, 61, 43);

pub const ROYAL_CRIMSON_MID: Color  = Color::rgb(142, 40, 52);
pub const ROYAL_CRIMSON_DARK: Color = Color::rgb(102, 29, 38);

pub const SACYRA_GREEN_BRIGHT: Color = Color::rgb(72, 158, 91);
pub const SACYRA_GREEN_BASE: Color   = Color::rgb(46, 111, 64);
pub const SACYRA_GREEN_DARK: Color   = Color::rgb(27, 67, 40);
pub const SACYRA_FLOWER_GOLD: Color  = Color::rgb(244, 162, 97);
pub const SACYRA_FLOWER_WHITE: Color = Color::rgb(255, 250, 240);

pub const STONE_ROAD_LIGHT: Color = Color::rgb(122, 137, 166);
pub const STONE_ROAD_BASE: Color  = Color::rgb(96, 109, 133);
pub const STONE_ROAD_DARK: Color  = Color::rgb(56, 66, 77);

pub const WATER_SHALLOW: Color = Color::rgb(56, 152, 236);
pub const WATER_DEEP: Color    = Color::rgb(33, 78, 128);
pub const WATER_FOAM: Color    = Color::rgb(216, 236, 255);

pub const VOID_ABYSS_DEEP: Color   = Color::rgb(15, 12, 25);
pub const VOID_GLOW_CYAN: Color    = Color::rgb(66, 226, 214);
pub const VOID_GLOW_MAGENTA: Color = Color::rgb(209, 35, 53);

// =============================================================================
// BLOCK 3: BODY BLOCK / 2D SOFTWARE CANVAS & PNG ENCODER
// Stratum: Developer Tooling | Machine: tools.assets.visual_synth
// =============================================================================

/// In-memory RGBA raster canvas.
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

// =============================================================================
// PROCEDURAL PIXEL ART GENERATORS
// =============================================================================

pub fn synth_elijah() -> Canvas {
    let mut c = Canvas::new(32, 32);
    // Shadow
    c.fill_ellipse(10, 28, 22, 31, Color::rgba(20, 25, 35, 120));
    // Cape
    c.fill_rect(11, 14, 20, 25, Color::rgb(30, 36, 52));
    // Greaves
    c.fill_rect(12, 23, 14, 28, STEEL_DARK);
    c.fill_rect(17, 23, 19, 28, STEEL_DARK);
    c.fill_rect(11, 28, 14, 30, STEEL_MID);
    c.fill_rect(17, 28, 20, 30, STEEL_MID);
    // Plate Cuirass & Belt
    c.fill_rect(12, 14, 19, 22, STEEL_MID);
    c.fill_rect(13, 15, 18, 20, STEEL_BRIGHT);
    // Sun crest
    c.line(15, 16, 16, 16, LUX_GOLD_BRIGHT);
    c.line(16, 15, 16, 18, LUX_GOLD_BRIGHT);
    c.fill_rect(12, 21, 19, 22, LEATHER_DARK);
    c.set_pixel(15, 21, LUX_GOLD_MID);
    // Helmet & Visor
    c.fill_rect(13, 7, 18, 13, STEEL_MID);
    c.fill_rect(14, 8, 17, 12, STEEL_BRIGHT);
    c.line(14, 10, 17, 10, Color::rgb(20, 25, 30));
    c.line(15, 4, 16, 6, LUX_GOLD_BRIGHT);
    // Shield of Lux
    c.fill_rect(6, 12, 11, 19, LUX_GOLD_DEEP);
    c.fill_rect(7, 13, 10, 18, LUX_GOLD_MID);
    c.set_pixel(8, 20, LUX_GOLD_MID);
    c.fill_rect(8, 15, 9, 16, LUX_GOLD_BRIGHT);
    // Gauntlet & sword scabbard
    c.fill_rect(20, 14, 22, 18, STEEL_MID);
    c.line(22, 15, 22, 24, STEEL_BRIGHT);
    c.set_pixel(22, 14, LUX_GOLD_MID);
    c
}

pub fn synth_katrina() -> Canvas {
    let mut c = Canvas::new(32, 32);
    c.fill_ellipse(10, 28, 22, 31, Color::rgba(20, 25, 35, 120));
    c.fill_rect(11, 14, 20, 25, TENEBRIS_PURPLE_DEEP);
    c.fill_rect(12, 24, 14, 29, LEATHER_DARK);
    c.fill_rect(17, 24, 19, 29, LEATHER_DARK);
    c.fill_rect(12, 14, 19, 21, LEATHER_LIGHT);
    c.line(12, 18, 19, 18, LEATHER_DARK);
    c.set_pixel(15, 18, STEEL_BRIGHT);
    // Hooded cowl & face
    c.fill_rect(12, 6, 19, 13, TENEBRIS_PURPLE_DEEP);
    c.fill_rect(13, 7, 18, 12, TENEBRIS_PURPLE_MID);
    c.fill_rect(14, 9, 17, 11, SKIN_LIGHT);
    c.set_pixel(14, 9, Color::rgb(40, 50, 70));
    c.set_pixel(16, 9, Color::rgb(40, 50, 70));
    c.set_pixel(13, 11, HAIR_SILVER);
    c.set_pixel(18, 11, HAIR_SILVER);
    c.set_pixel(15, 14, TENEBRIS_PURPLE_BRIGHT);
    // Sword of Tenebris
    c.fill_rect(20, 15, 21, 17, LEATHER_LIGHT);
    c.line(22, 11, 25, 23, TENEBRIS_PURPLE_MID);
    c.line(23, 12, 24, 20, TENEBRIS_PURPLE_BRIGHT);
    c.line(20, 13, 23, 15, STEEL_MID);
    c
}

pub fn synth_casatti() -> Canvas {
    let mut c = Canvas::new(32, 32);
    c.fill_ellipse(10, 28, 22, 31, Color::rgba(20, 25, 35, 120));
    c.fill_rect(10, 13, 21, 24, ROYAL_CRIMSON_DARK);
    c.fill_rect(12, 23, 14, 29, LEATHER_DARK);
    c.fill_rect(17, 23, 19, 29, LEATHER_DARK);
    c.fill_rect(12, 13, 19, 20, ROYAL_CRIMSON_MID);
    c.line(15, 14, 15, 19, LUX_GOLD_MID);
    c.line(13, 15, 18, 15, LUX_GOLD_MID);
    c.fill_rect(13, 8, 18, 12, SKIN_LIGHT);
    c.set_pixel(14, 9, Color::rgb(40, 30, 25));
    c.set_pixel(17, 9, Color::rgb(40, 30, 25));
    c.fill_rect(12, 5, 19, 7, ROYAL_CRIMSON_MID);
    c.line(18, 3, 21, 6, LUX_GOLD_BRIGHT);
    c.line(9, 18, 8, 27, STEEL_BRIGHT);
    c
}

pub fn synth_observer() -> Canvas {
    let mut c = Canvas::new(32, 32);
    c.fill_ellipse(10, 28, 22, 31, Color::rgba(20, 25, 35, 120));
    c.fill_rect(11, 14, 15, 26, LUX_GOLD_MID);
    c.fill_rect(16, 14, 20, 26, TENEBRIS_PURPLE_MID);
    c.fill_rect(15, 17, 16, 19, Color::rgb(46, 204, 113));
    c.fill_rect(12, 6, 19, 13, Color::rgb(30, 36, 52));
    c.line(14, 9, 15, 9, LUX_GOLD_BRIGHT);
    c.line(16, 9, 17, 9, TENEBRIS_PURPLE_BRIGHT);
    c
}

pub fn synth_guard() -> Canvas {
    let mut c = Canvas::new(32, 32);
    c.fill_ellipse(10, 28, 22, 31, Color::rgba(20, 25, 35, 120));
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

pub fn synth_dream_eater() -> Canvas {
    let mut c = Canvas::new(32, 32);
    c.fill_ellipse(8, 8, 23, 23, Color::rgb(30, 18, 52));
    c.fill_ellipse(10, 10, 21, 21, Color::rgb(53, 28, 84));
    let tendrils = [(7, 26), (6, 29), (11, 27), (10, 31), (20, 27), (21, 31), (24, 26)];
    for &(tx, ty) in &tendrils {
        c.set_pixel(tx, ty, VOID_GLOW_CYAN);
    }
    c.fill_rect(11, 12, 13, 14, VOID_GLOW_CYAN);
    c.set_pixel(12, 13, Color::rgb(255, 255, 255));
    c.fill_rect(18, 12, 20, 14, VOID_GLOW_CYAN);
    c.set_pixel(19, 13, Color::rgb(255, 255, 255));
    c
}

pub fn synth_shadow_stalker() -> Canvas {
    let mut c = Canvas::new(32, 32);
    c.fill_ellipse(6, 24, 26, 30, Color::rgba(20, 18, 28, 140));
    c.fill_ellipse(8, 13, 23, 22, Color::rgb(17, 17, 22));
    c.line(10, 20, 8, 27, Color::rgb(17, 17, 22));
    c.line(21, 20, 23, 27, Color::rgb(17, 17, 22));
    c.line(7, 13, 9, 13, VOID_GLOW_MAGENTA);
    c.line(22, 16, 28, 8, Color::rgb(17, 17, 22));
    c.set_pixel(28, 7, VOID_GLOW_MAGENTA);
    c
}

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
    c.line(0, 11, 23, 0, hi);
    c.line(23, 0, 47, 11, hi);
    c.line(0, 11, 23, 23, sh);
    c.line(23, 23, 47, 11, sh);
    c
}

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

pub fn synth_shield_icon() -> Canvas {
    let mut c = Canvas::new(24, 24);
    c.fill_rect(4, 4, 19, 14, LUX_GOLD_DEEP);
    c.fill_rect(6, 6, 17, 13, LUX_GOLD_MID);
    c.line(11, 20, 12, 20, LUX_GOLD_MID);
    c.fill_rect(9, 8, 14, 13, LUX_GOLD_BRIGHT);
    c.set_pixel(11, 10, Color::rgb(255, 255, 255));
    c
}

pub fn synth_sword_icon() -> Canvas {
    let mut c = Canvas::new(24, 24);
    c.line(7, 15, 18, 4, TENEBRIS_PURPLE_DEEP);
    c.line(7, 14, 17, 3, TENEBRIS_PURPLE_BRIGHT);
    c.line(8, 16, 19, 5, TENEBRIS_PURPLE_MID);
    c.line(4, 12, 10, 18, STEEL_BRIGHT);
    c.line(5, 17, 3, 19, LEATHER_DARK);
    c
}

pub fn synth_spritesheet(base: &Canvas) -> Canvas {
    let mut sheet = Canvas::new(128, 128);
    for row in 0..4 {
        for col in 0..4 {
            let mut frame = base.clone();
            if row == 1 {
                frame = frame.flip_h();
            } else if row == 3 {
                frame.fill_rect(13, 8, 18, 12, Color::rgb(35, 40, 55));
            }
            let step_y = if col % 2 == 1 { 1 } else { 0 };
            sheet.blit(&frame, (col * 32) as i32, (row * 32 + step_y) as i32);
        }
    }
    sheet
}

pub fn synth_sacyra_tileset() -> Canvas {
    let mut c = Canvas::new(256, 256);
    c.fill_rect(0, 0, 255, 255, SACYRA_GREEN_BASE);
    // Row 1: Stone
    c.fill_rect(0, 32, 255, 63, STONE_ROAD_BASE);
    // Row 2: Forest
    c.fill_rect(0, 64, 255, 95, SACYRA_GREEN_DARK);
    // Row 3: Walls
    c.fill_rect(0, 96, 255, 127, Color::rgb(67, 77, 99));
    // Row 4: Water
    c.fill_rect(0, 128, 255, 159, WATER_SHALLOW);
    c
}

pub fn synth_dreamscape_tileset() -> Canvas {
    let mut c = Canvas::new(256, 256);
    c.fill_rect(0, 0, 255, 255, VOID_ABYSS_DEEP);
    c.fill_rect(0, 0, 255, 31, Color::rgb(24, 30, 52));
    c.fill_rect(0, 32, 255, 63, Color::rgb(38, 48, 80));
    c.fill_rect(0, 64, 255, 95, Color::rgb(10, 8, 16));
    c
}

/// Generates all canonical visual assets into `game/assets/`.
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

    // Isometric Tiles
    let cobble = synth_iso_diamond(STONE_ROAD_BASE, STONE_ROAD_LIGHT, STONE_ROAD_DARK);
    cobble.save_png(&assets_dir.join("sprites/tiles/cobblestone_48x24.png"))?;
    count += 1;

    let grass = synth_iso_diamond(SACYRA_GREEN_BASE, SACYRA_GREEN_BRIGHT, SACYRA_GREEN_DARK);
    grass.save_png(&assets_dir.join("sprites/tiles/grass_48x24.png"))?;
    count += 1;

    let void_crag = synth_iso_diamond(Color::rgb(35, 25, 45), Color::rgb(65, 45, 85), Color::rgb(15, 10, 20));
    void_crag.save_png(&assets_dir.join("sprites/tiles/void_crag_48x24.png"))?;
    count += 1;

    let water = synth_iso_diamond(WATER_SHALLOW, WATER_FOAM, WATER_DEEP);
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
