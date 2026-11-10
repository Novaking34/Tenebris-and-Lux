/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_gui_hud_unit.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Unit Verification: Heads-Up Display (HUD) & Chronicle Feed
@role          Isolates and validates gui_hud.c across all 4 ontological
               testing tiers: Always Pass, Never Fail, Assertion & Guarantee,
               and Emergence/Stress.
@invariants    1. Zero dynamic heap allocations in test execution (ALLOC_ZERO_HEAP).
               2. Chronicle ring buffer strictly maintains FIFO order and capacity limits.
               3. Headless rendering operations complete safely without display connection.
               4. Universal 5-Block Lifecycle Protocol enforced.

PURPOSE & ARCHITECTURAL ROLE:
  Verifies narrative chronicle ring-buffer mechanics, FIFO message shift on
  overflow, header title rendering, hero vitals display, and HUD self-audits.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & ONTOLOGY
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.viewport.hud
================================================================================
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "viewport/hud/gui_hud.h"
#include "viewport/styling/gui_palette.h"
#include "hero/sim_state.h"

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
static void test_hud_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal HUD Baseline)...\n");

    /* 1.1 Chronicle ring buffer appending */
    gui_log_t log;
    memset(&log, 0, sizeof(log));

    gui_hud_log_append(&log, "Event 1: Elijah awakens in the Dream Chamber.");
    TEST_ASSERT(log.count == 1, "Log count is 1 after first entry");
    TEST_ASSERT(strcmp(log.lines[0], "Event 1: Elijah awakens in the Dream Chamber.") == 0, "Line 0 matches event 1");

    gui_hud_log_append(&log, "Event 2: Shield of Lux hums with radiant gold.");
    TEST_ASSERT(log.count == 2, "Log count is 2 after second entry");
    TEST_ASSERT(strcmp(log.lines[1], "Event 2: Shield of Lux hums with radiant gold.") == 0, "Line 1 matches event 2");

    /* 1.2 HUD self-audit */
    bool audit_ok = gui_hud_audit();
    TEST_ASSERT(audit_ok, "gui_hud_audit passes unconditionally");

    /* 1.3 HUD diagnostic dump card */
    char card[512];
    int card_len = gui_hud_execute_dump_card(card, sizeof(card));
    TEST_ASSERT(card_len > 0, "gui_hud_execute_dump_card returns valid byte length");
    TEST_ASSERT(strstr(card, "VIEWPORT HUD SUBSYSTEM DIAGNOSTIC CARD") != NULL, "Card contains title");

    printf("  [PASS-1] Section 1 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (NEGATIVE FAULT TOLERANCE) ]
--------------------------------------------------------------------------------
*/
static void test_hud_never_fail(void)
{
    printf("  [PASS-2] Running Section 2: Never Fail (Negative Fault Tolerance)...\n");

    /* 2.1 NULL pointers to log append */
    gui_log_t log;
    memset(&log, 0, sizeof(log));
    gui_hud_log_append(NULL, "Test message");
    gui_hud_log_append(&log, NULL);
    TEST_ASSERT(log.count == 0, "NULL inputs do not increment log count");

    /* 2.2 Headless rendering with NULL display handles */
    gui_palette_t pal;
    gui_palette_init_default(&pal);

    gui_panel_rect_t header_rect = { 10, 10, 840, 32, "Header" };
    gui_panel_rect_t status_rect = { 590, 52, 260, 374, "Status" };
    gui_panel_rect_t chronicle_rect = { 10, 436, 570, 174, "Chronicle" };

    player_state_t state;
    sim_state_init(&state);

    gui_hud_render_header(NULL, 0, NULL, &header_rect, &pal, NULL);
    gui_hud_render_status(NULL, 0, NULL, &status_rect, &state, &pal, NULL);
    gui_hud_render_chronicle(NULL, 0, NULL, &chronicle_rect, &log, &pal, NULL);

    /* 2.3 Headless rendering with NULL rect or state */
    gui_hud_render_header(NULL, 0, NULL, NULL, &pal, NULL);
    gui_hud_render_status(NULL, 0, NULL, &status_rect, NULL, &pal, NULL);
    gui_hud_render_chronicle(NULL, 0, NULL, &chronicle_rect, NULL, &pal, NULL);

    /* 2.4 Diagnostic card with NULL/zero capacity */
    char dummy[64];
    TEST_ASSERT(gui_hud_execute_dump_card(NULL, 0) == -1, "gui_hud_execute_dump_card handles NULL buffer safely");
    TEST_ASSERT(gui_hud_execute_dump_card(dummy, 0) == -1, "gui_hud_execute_dump_card handles zero capacity safely");

    printf("  [PASS-2] Section 2 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (FIFO RING BUFFER INVARIANTS) ]
--------------------------------------------------------------------------------
*/
static void test_hud_assertions_and_guarantees(void)
{
    printf("  [PASS-3] Running Section 3: Assertion & Guarantee (FIFO Ring Invariants)...\n");

    /* 3.1 Fill ring buffer to exact capacity (GUI_MAX_LOG_LINES = 16) */
    gui_log_t log;
    memset(&log, 0, sizeof(log));

    char buf[64];
    for (int i = 0; i < GUI_MAX_LOG_LINES; i++) {
        snprintf(buf, sizeof(buf), "Chronicle message #%d", i);
        gui_hud_log_append(&log, buf);
    }
    TEST_ASSERT(log.count == GUI_MAX_LOG_LINES, "Ring buffer fills to max capacity");
    TEST_ASSERT(strcmp(log.lines[0], "Chronicle message #0") == 0, "Oldest element is at index 0");
    TEST_ASSERT(strcmp(log.lines[GUI_MAX_LOG_LINES - 1], "Chronicle message #15") == 0, "Newest element is at last index");

    /* 3.2 Append 17th element: triggers FIFO shift */
    gui_hud_log_append(&log, "Chronicle message #16 (Overflow)");
    TEST_ASSERT(log.count == GUI_MAX_LOG_LINES, "Count remains clamped at GUI_MAX_LOG_LINES");
    TEST_ASSERT(strcmp(log.lines[0], "Chronicle message #1") == 0, "Message #0 was dropped; message #1 is now at index 0");
    TEST_ASSERT(strcmp(log.lines[GUI_MAX_LOG_LINES - 1], "Chronicle message #16 (Overflow)") == 0, "New message sits at last index");

    printf("  [PASS-3] Section 3 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE & STRESS (RAPID LOG OVERFLOW) ]
--------------------------------------------------------------------------------
*/
static void test_hud_emergence(void)
{
    printf("  [PASS-4] Running Section 4: Emergence & Stress (Rapid Log Overflow)...\n");

    /* 4.1 1,000 rapid log appends causing continuous ring rolling */
    gui_log_t log;
    memset(&log, 0, sizeof(log));

    char buf[64];
    for (int i = 0; i < 1000; i++) {
        snprintf(buf, sizeof(buf), "Battle Turn Event #%d: Strike against Usuru's minions", i);
        gui_hud_log_append(&log, buf);
        TEST_ASSERT(log.count <= GUI_MAX_LOG_LINES, "Log count never exceeds capacity");
    }

    /* Last message must match index 999 */
    snprintf(buf, sizeof(buf), "Battle Turn Event #999: Strike against Usuru's minions");
    TEST_ASSERT(strcmp(log.lines[GUI_MAX_LOG_LINES - 1], buf) == 0, "Last line matches turn 999 event");

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
    printf("   Unit Verification: Heads-Up Display (HUD) & Chronicle\n");
    printf("============================================================\n");

    test_hud_always_pass();
    test_hud_never_fail();
    test_hud_assertions_and_guarantees();
    test_hud_emergence();

    printf("============================================================\n");
    printf("   All GUI HUD Unit Tests Passed Successfully (4/4)\n");
    printf("============================================================\n");
    return 0;
}

