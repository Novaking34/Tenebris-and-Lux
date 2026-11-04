// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          dispatch_types.h
// @id            sim.dispatch.types
// @layer         Layer 1: Simulation Engine
// @subsystem     Command Dispatcher Subsystem (sim/dispatch)
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Ontological type definitions for command tokenization, alias mapping,
//   domain classification, and domain handler execution signatures.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Foundational simulation leaf).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure immutable data definitions).
//   - Latency Budget:        < 5 microseconds per classification.
//
// AXIOMATIC INVARIANTS:
//   1. Input buffers unconditionally null-terminated.
//   2. Domain is strictly bounded: 0 <= domain <= DOMAIN_UNKNOWN.
//   3. Alias resolutions converge in exactly 1 hop (no cycles).
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_DISPATCH_TYPES_H
#define TENEBRIS_LUX_SIM_DISPATCH_TYPES_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_DISPATCH_VERB_LEN 32
#define MAX_DISPATCH_ARG_LEN  128
#define MAX_DISPATCH_INPUT    256

/**
 * @brief Command domain taxonomy for orchestrated routing.
 */
typedef enum {
    DOMAIN_SYSTEM  = 0,
    DOMAIN_COMBAT  = 1,
    DOMAIN_ITEMS   = 2,
    DOMAIN_WORLD   = 3,
    DOMAIN_UNKNOWN = 4
} dispatch_domain_t;

/**
 * @brief Canonical command definition in the command dictionary.
 */
typedef struct {
    char              verb[MAX_DISPATCH_VERB_LEN];
    dispatch_domain_t domain;
    uint8_t           min_args;
    uint8_t           max_args;
    const char       *usage;
    const char       *description;
} command_def_t;

/**
 * @brief Synonym and shorthand mapping to canonical verb.
 */
typedef struct {
    char alias[MAX_DISPATCH_VERB_LEN];
    char canonical[MAX_DISPATCH_VERB_LEN];
    char default_arg[MAX_DISPATCH_ARG_LEN];
} alias_def_t;

/**
 * @brief Tokenized execution context passed to domain handlers.
 */
typedef struct {
    char              raw_input[MAX_DISPATCH_INPUT];
    char              canonical_verb[MAX_DISPATCH_VERB_LEN];
    char              arg[MAX_DISPATCH_ARG_LEN];
    dispatch_domain_t domain;
} dispatch_context_t;

#endif // TENEBRIS_LUX_SIM_DISPATCH_TYPES_H
