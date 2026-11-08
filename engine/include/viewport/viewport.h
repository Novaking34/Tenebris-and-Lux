/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          viewport.h
@layer         Layer 2: Presentation & Viewports
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Master Viewport Umbrella Subsystem
@role          Aggregates raster, HUD, Movie Reel, and styling sub-assemblies.
@invariants    1. Zero dynamic heap allocations (ALLOC_ZERO_HEAP).
               2. Pure presentation layer: read-only relative to simulation.
================================================================================
*/

#ifndef TENEBRIS_LUX_VIEWPORT_H
#define TENEBRIS_LUX_VIEWPORT_H

/*
================================================================================
BLOCK 2: OPENING / TYPES & DOMAIN ONTOLOGY BLOCK
================================================================================
*/

/* 1. Low-level rasterization & double-buffered X11 context */
#include "viewport/raster/gui_types.h"
#include "viewport/raster/gui.h"
#include "viewport/raster/gui_view2d.h"

/* 2. Heads-Up Display & UI controls */
#include "viewport/hud/gui_hud.h"
#include "viewport/hud/gui_layout.h"
#include "viewport/hud/gui_widgets.h"

/* 3. The Movie Reel & canonical screen frames */
#include "viewport/reel/gui_screens.h"
#include "viewport/reel/reel.h"

/* 4. Visual styling, typography, assets & palettes */
#include "viewport/styling/gui_palette.h"
#include "viewport/styling/gui_fonts.h"
#include "viewport/styling/gui_assets.h"

/*
================================================================================
BLOCK 3: BODY BLOCK
================================================================================
*/

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
================================================================================
*/

/*
================================================================================
BLOCK 5: CLOSING BLOCK
================================================================================
*/

#endif /* TENEBRIS_LUX_VIEWPORT_H */

