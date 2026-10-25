// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          time_utils.c
// @layer         Core Utilities Layer (Layer 0)
// @version       0.0.1
// @epoch         2026.10
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @role          Implementation of High-Resolution Timing & Frame Sleep
// @invariants    Rank 1 (< >): Form precedes fullness.
//                1. Monotonic precision without clock drift or leap adjustments.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// =============================================================================

#define _POSIX_C_SOURCE 199309L

#include "util/time_utils.h"

#include <time.h>

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Monotonic Clock Implementations )
// =============================================================================

double time_get_monotonic_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1000000.0;
}

double time_get_monotonic_us(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000000.0 + (double)ts.tv_nsec / 1000.0;
}

void time_sleep_ms(double ms)
{
    if (ms <= 0.0) return;
    struct timespec req;
    req.tv_sec = (time_t)(ms / 1000.0);
    req.tv_nsec = (long)((ms - (double)req.tv_sec * 1000.0) * 1000000.0);
    nanosleep(&req, NULL);
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

bool time_utils_audit(void)
{
    double t1 = time_get_monotonic_ms();
    time_sleep_ms(2.0);
    double t2 = time_get_monotonic_ms();

    if (t2 < t1) return false;      // Non-monotonic violation
    if ((t2 - t1) < 1.0) return false; // Slept too briefly

    return true;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

// End of time_utils.c
