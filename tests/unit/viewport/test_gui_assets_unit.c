/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_gui_assets_unit.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Unit Verification: Asset Abstraction & Procedural Synthesis
@role          Isolates and validates gui_assets.c across all 4 ontological
               testing tiers: Always Pass, Never Fail, Assertion & Guarantee,
               and Emergence/Stress.
@invariants    1. Zero dynamic heap allocations in test execution (ALLOC_ZERO_HEAP).
               2. Missing artist sprites gracefully degrade to procedural vector synthesis.
               3. Asset lookups are deterministic and reentrant.
               4. Universal 5-Block Lifecycle Protocol enforced.

PURPOSE & ARCHITECTURAL ROLE:
  Verifies asset catalog registration, procedural sprite generation fallback,
  external artist binding, headless rendering, and asset subsystem self-audits.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & ONTOLOGY
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.viewport.assets
================================================================================
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "viewport/styling/gui_assets.h"
#include "viewport/styling/gui_palette.h"

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "[FAIL] %s:%d: Assertion failed: %s (%s)\n", \
                __FILE__, __LINE__, #cond, msg); \
            exit(1); \
        } \
    } while (0)

/*
================================================================================
BLOCK 3: BODY BLOCK / 4-SECTION UNIT TEST SUITES
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN BASELINE) ]
--------------------------------------------------------------------------------
*/
static void test_assets_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Assets Baseline)...\n");

    /* 1.1 Asset catalog initialization */
    gui_assets_init();

    /* 1.2 Query canonical character and relic assets */
    const gui_asset_t *elijah = gui_assets_get(ASSET_HERO_ELIJAH);
    TEST_ASSERT(elijah != NULL, "Elijah asset record exists");
    TEST_ASSERT(elijah->id == ASSET_HERO_ELIJAH, "Asset ID matches ASSET_HERO_ELIJAH");
    TEST_ASSERT(elijah->kind == ASSET_KIND_PROCEDURAL, "Default kind is procedural");

    const gui_asset_t *shield = gui_assets_get(ASSET_RELIC_SHIELD_LUX);
    TEST_ASSERT(shield != NULL, "Shield of Lux asset record exists");

    const gui_asset_t *sword = gui_assets_get(ASSET_RELIC_SWORD_TENEBRIS);
    TEST_ASSERT(sword != NULL, "Sword of Tenebris asset record exists");

    const gui_asset_t *usuru = gui_assets_get(ASSET_ENEMY_DARK_LORD);
    TEST_ASSERT(usuru != NULL, "Dark Lord Usuru asset record exists");

    /* 1.3 Asset audit */
    bool audit_ok = gui_assets_audit();
    TEST_ASSERT(audit_ok, "gui_assets_audit passes unconditionally");

    printf("  [PASS-1] Section 1 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (NEGATIVE FAULT TOLERANCE) ]
--------------------------------------------------------------------------------
*/
static void test_assets_never_fail(void)
{
    printf("  [PASS-2] Running Section 2: Never Fail (Negative Fault Tolerance)...\n");

    /* 2.1 Unregistered or invalid asset IDs return NULL */
    const gui_asset_t *none_asset = gui_assets_get(ASSET_NONE);
    TEST_ASSERT(none_asset == NULL, "ASSET_NONE returns NULL");

    const gui_asset_t *invalid_asset = gui_assets_get((gui_asset_id_t)9999);
    TEST_ASSERT(invalid_asset == NULL, "Out-of-bounds asset ID returns NULL");

    /* 2.2 External binding invalid parameters */
    bool bind_null = gui_assets_bind_external(ASSET_HERO_ELIJAH, NULL);
    TEST_ASSERT(!bind_null, "Binding NULL file path returns false");

    bool bind_invalid = gui_assets_bind_external((gui_asset_id_t)9999, "path/to/img.png");
    TEST_ASSERT(!bind_invalid, "Binding to invalid asset ID returns false");

    /* 2.3 Headless rendering with NULL display handles must not crash */
    gui_palette_t pal;
    gui_palette_init_default(&pal);

    gui_assets_draw_sprite(NULL, 0, NULL, ASSET_HERO_ELIJAH, 100, 100, 32, 0, &pal);
    gui_assets_draw_sprite(NULL, 0, NULL, ASSET_NONE, 100, 100, 32, 0, &pal);
    gui_assets_draw_sprite(NULL, 0, NULL, ASSET_HERO_ELIJAH, 100, 100, 32, 0, NULL);

    gui_assets_draw_tile_detail(NULL, 0, NULL, ASSET_TILE_COBBLESTONE, 0, 0, 48, 24, &pal);
    gui_assets_draw_tile_detail(NULL, 0, NULL, ASSET_NONE, 0, 0, 48, 24, &pal);

    gui_assets_draw_vfx(NULL, 0, NULL, ASSET_VFX_LUX_BARRIO, 100, 100, 20, &pal);
    gui_assets_draw_vfx(NULL, 0, NULL, ASSET_NONE, 100, 100, 20, &pal);

    printf("  [PASS-2] Section 2 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (ASSET METADATA INVARIANTS) ]
--------------------------------------------------------------------------------
*/
static void test_assets_assertions_and_guarantees(void)
{
    printf("  [PASS-3] Running Section 3: Assertion & Guarantee (Asset Axioms)...\n");

    /* 3.1 External binding round-trip */
    const char *test_path = "assets/sprites/elijah.png";
    bool bound = gui_assets_bind_external(ASSET_HERO_ELIJAH, test_path);
    TEST_ASSERT(bound, "Bound external path successfully");

    const gui_asset_t *asset = gui_assets_get(ASSET_HERO_ELIJAH);
    TEST_ASSERT(asset != NULL, "Asset retrieved");
    TEST_ASSERT(asset->artist_file_path != NULL, "artist_file_path is populated");
    TEST_ASSERT(strcmp(asset->artist_file_path, test_path) == 0, "artist_file_path matches registered string");

    /* Restore procedural kind */
    gui_assets_init();

    printf("  [PASS-3] Section 3 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE & STRESS (HIGH-VOLUME ASSET LOOKUP) ]
--------------------------------------------------------------------------------
*/
static void test_assets_emergence(void)
{
    printf("  [PASS-4] Running Section 4: Emergence & Stress (High-Volume Lookups)...\n");

    /* 4.1 20,000 rapid asset lookups */
    gui_asset_id_t test_ids[6] = {
        ASSET_HERO_ELIJAH,
        ASSET_HERO_KATRINA,
        ASSET_HERO_CASATTI,
        ASSET_ENEMY_DARK_LORD,
        ASSET_RELIC_SHIELD_LUX,
        ASSET_RELIC_SWORD_TENEBRIS
    };

    for (int i = 0; i < 20000; i++) {
        gui_asset_id_t id = test_ids[i % 6];
        const gui_asset_t *a = gui_assets_get(id);
        TEST_ASSERT(a != NULL && a->id == id, "High-volume asset lookup verified");
    }

    printf("  [PASS-4] Section 4 passed unconditionally.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST ENTRY POINT
================================================================================
*/
int main(void)
{
    printf("============================================================\n");
    printf("   Unit Verification: Asset Abstraction & Procedural Art\n");
    printf("============================================================\n");

    test_assets_always_pass();
    test_assets_never_fail();
    test_assets_assertions_and_guarantees();
    test_assets_emergence();

    printf("============================================================\n");
    printf("   All GUI Assets Unit Tests Passed Successfully (4/4)\n");
    printf("============================================================\n");
    return 0;
}
