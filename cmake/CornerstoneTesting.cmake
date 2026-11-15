# ==============================================================================
# BLOCK 1: METADATA BLOCK
# ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
# ==============================================================================
# @project       Tenebris and Lux
# @file          cmake/CornerstoneTesting.cmake
# @id            build.cmake.testing
# @layer         Layer 4 Verification Suite (Build Orchestration)
# @archetype     operational | stateful
# @version       0.1.2-dev
# @interface_rev Rev 5
# @date          2026-11-14
# @author        CreativeWorkzStudio LLC / ProfessorSeanEX
# @repository    https://github.com/Novaking34/tenebris-lux
# @system        Cornerstone Game Engine Four-Tier Verification Battery
# @role          Modular CMake Subsystem: Defines cornerstone_add_test_executable()
#                and registers all 65 Layer 4 CTest verification suites spanning
#                Functional, Unit, Integrated, System, C++ probe, and CLI checks.
# @maturity      Rev 5 Baseline | 65 CTest suites routed cleanly to build/tests/.
# @chassis       zero-heap | Links compiled L0/L1/L2 OBJECT libraries once.
# @scale         O(T) test executable graph across 62 test targets and 65 test cases.
# @seams         in: tests/, tenebris_engine_util, tenebris_game_sim | out: build/tests/*
# @invariants    1. Test executables routed to build/tests/ via target properties.
#                2. Strict ISO C warning purity (-Wall -Wextra -Wpedantic).
#                3. Uniform workspace root working directory for all CTest entries.
#
# PURPOSE & ARCHITECTURAL ROLE:
#   Constructs the full 4-tier verification battery for the Cornerstone Game Engine
#   and Tenebris and Lux RPG, ensuring every cog and domain system is audited.
# ==============================================================================

# ==============================================================================
# BLOCK 2: OPENING BLOCK
# ( CTest Initialization & Helper Function Definition )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 4 Verification Suite (Build Orchestration)
# Module:        build.cmake.testing
# Mission:       Initialize CTest infrastructure and define test registration helpers.
# ==============================================================================

include(CTest)

if(BUILD_TESTING)
    # Accumulator populated by cornerstone_add_test_executable() and finalized in Block 5.
    set(CORNERSTONE_CTEST_NAMES "")

    # --------------------------------------------------------------------------
    # Sub-Assembly 2.1: Test Executable & CTest Registration Helper
    # --------------------------------------------------------------------------
    # Architectural Rationale ("Why"):
    #   1. Structured Isolation: Automatically routes test executables to build/tests/,
    #      keeping the root build directory and application bin/ directory clean.
    #   2. Bedrock Linkage: Links tenebris_engine_util into every test harness,
    #      ensuring uniform zero-heap chassis assertions, clock pacer timing,
    #      and forensic logging without duplicating source compilation.
    #   3. Uniform CTest Tracking: Appends each registered test name to the
    #      CORNERSTONE_CTEST_NAMES list so Block 5 can bind identical working
    #      directories and environment settings across all 65+ test cases.
    # --------------------------------------------------------------------------
    function(cornerstone_add_test_executable)
        set(_one TARGET TEST_NAME)
        set(_multi SOURCES LIBS EXTRA_INCLUDES LABELS)
        cmake_parse_arguments(TST "" "${_one}" "${_multi}" ${ARGN})

        add_executable("${TST_TARGET}" ${TST_SOURCES})
        set_target_properties("${TST_TARGET}" PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
        )
        target_include_directories("${TST_TARGET}" PRIVATE
            ${TEST_INCLUDES}
            ${TST_EXTRA_INCLUDES}
        )
        target_link_libraries("${TST_TARGET}" PRIVATE
            tenebris_engine_util
            ${TST_LIBS}
        )
        target_compile_options("${TST_TARGET}" PRIVATE ${GAME_WARNING_FLAGS})

        add_test(NAME "${TST_TEST_NAME}" COMMAND "${TST_TARGET}")
        if(TST_LABELS)
            set_tests_properties("${TST_TEST_NAME}" PROPERTIES LABELS "${TST_LABELS}")
        endif()

        set(_updated_list ${CORNERSTONE_CTEST_NAMES} "${TST_TEST_NAME}")
        set(CORNERSTONE_CTEST_NAMES "${_updated_list}" PARENT_SCOPE)
    endfunction()

# ==============================================================================
# BLOCK 3: BODY BLOCK
# ( Four-Tier Verification Suite Registration )
# ==============================================================================

    # --------------------------------------------------------------------------
    # Sub-Assembly 3.1: Functional Debugging Harnesses (Coarse Surface)
    # --------------------------------------------------------------------------
    cornerstone_add_test_executable(
        TARGET         test_functional
        TEST_NAME      functional_cogs_test
        SOURCES        tests/functional/test_functional.c
        LIBS           tenebris_engine_viewport tenebris_game_sim ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         functional engine sim
    )
    cornerstone_add_test_executable(
        TARGET    test_io_functional
        TEST_NAME functional_io_test
        SOURCES   tests/functional/test_io_functional.c
        LABELS    functional io
    )
    cornerstone_add_test_executable(
        TARGET    test_log_functional
        TEST_NAME functional_log_test
        SOURCES   tests/functional/test_log_functional.c
        LABELS    functional log
    )
    cornerstone_add_test_executable(
        TARGET    test_debug_functional
        TEST_NAME functional_debug_test
        SOURCES   tests/functional/test_debug_functional.c
        LABELS    functional debug
    )
    cornerstone_add_test_executable(
        TARGET    test_core_functional
        TEST_NAME functional_core_test
        SOURCES   tests/functional/test_core_functional.c
        LIBS      m
        LABELS    functional core
    )
    cornerstone_add_test_executable(
        TARGET    test_audio_functional
        TEST_NAME functional_audio_test
        SOURCES   tests/functional/test_audio_functional.c
        LABELS    functional audio
    )
    cornerstone_add_test_executable(
        TARGET         test_viewport_functional
        TEST_NAME      functional_viewport_test
        SOURCES        tests/functional/test_viewport_functional.c
        LIBS           tenebris_engine_viewport tenebris_game_sim ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         functional viewport
    )

    # --------------------------------------------------------------------------
    # Sub-Assembly 3.2: Isolated Subsystem Unit Harnesses
    # --------------------------------------------------------------------------
    # Core Utilities (6 Operational + 6 Fallback Monoliths)
    cornerstone_add_test_executable(
        TARGET    test_math_utils_unit
        TEST_NAME unit_math_utils_test
        SOURCES   tests/unit/core/test_math_utils_unit.c
        LIBS      m
        LABELS    unit core
    )
    cornerstone_add_test_executable(
        TARGET    test_math_utils_fallback_unit
        TEST_NAME unit_math_utils_fallback_test
        SOURCES   tests/unit/core/test_math_utils_fallback_unit.c
        LIBS      m
        LABELS    unit core fallback
    )
    cornerstone_add_test_executable(
        TARGET    test_time_utils_unit
        TEST_NAME unit_time_utils_test
        SOURCES   tests/unit/core/test_time_utils_unit.c
        LABELS    unit core
    )
    cornerstone_add_test_executable(
        TARGET    test_time_utils_fallback_unit
        TEST_NAME unit_time_utils_fallback_test
        SOURCES   tests/unit/core/test_time_utils_fallback_unit.c
        LABELS    unit core fallback
    )
    cornerstone_add_test_executable(
        TARGET    test_string_utils_unit
        TEST_NAME unit_string_utils_test
        SOURCES   tests/unit/core/test_string_utils_unit.c
        LABELS    unit core
    )
    cornerstone_add_test_executable(
        TARGET    test_string_utils_fallback_unit
        TEST_NAME unit_string_utils_fallback_test
        SOURCES   tests/unit/core/test_string_utils_fallback_unit.c
        LABELS    unit core fallback
    )
    cornerstone_add_test_executable(
        TARGET    test_geom_utils_unit
        TEST_NAME unit_geom_utils_test
        SOURCES   tests/unit/core/test_geom_utils_unit.c
        LIBS      m
        LABELS    unit core
    )
    cornerstone_add_test_executable(
        TARGET    test_geom_utils_fallback_unit
        TEST_NAME unit_geom_utils_fallback_test
        SOURCES   tests/unit/core/test_geom_utils_fallback_unit.c
        LIBS      m
        LABELS    unit core fallback
    )
    cornerstone_add_test_executable(
        TARGET    test_clock_pacer_unit
        TEST_NAME unit_clock_pacer_test
        SOURCES   tests/unit/core/test_clock_pacer_unit.c
        LABELS    unit core
    )
    cornerstone_add_test_executable(
        TARGET    test_clock_pacer_fallback_unit
        TEST_NAME unit_clock_pacer_fallback_test
        SOURCES   tests/unit/core/test_clock_pacer_fallback_unit.c
        LABELS    unit core fallback
    )
    cornerstone_add_test_executable(
        TARGET    test_formula_eval_unit
        TEST_NAME unit_formula_eval_test
        SOURCES   tests/unit/core/test_formula_eval_unit.c
        LIBS      m
        LABELS    unit core
    )
    cornerstone_add_test_executable(
        TARGET    test_formula_eval_fallback_unit
        TEST_NAME unit_formula_eval_fallback_test
        SOURCES   tests/unit/core/test_formula_eval_fallback_unit.c
        LIBS      m
        LABELS    unit core fallback
    )

    # Layer 0 I/O Subsystems (4 Operational + 1 Tier-0 BIOS Fallback)
    cornerstone_add_test_executable(
        TARGET    test_io_fallback_unit
        TEST_NAME unit_io_fallback_test
        SOURCES   tests/unit/io/test_io_fallback_unit.c
        LABELS    unit io fallback
    )
    cornerstone_add_test_executable(
        TARGET    test_fs_utils_unit
        TEST_NAME unit_fs_utils_test
        SOURCES   tests/unit/io/test_fs_utils_unit.c
        LABELS    unit io
    )
    cornerstone_add_test_executable(
        TARGET    test_path_finder_unit
        TEST_NAME unit_path_finder_test
        SOURCES   tests/unit/io/test_path_finder_unit.c
        LABELS    unit io
    )
    cornerstone_add_test_executable(
        TARGET    test_json_utils_unit
        TEST_NAME unit_json_utils_test
        SOURCES   tests/unit/io/test_json_utils_unit.c
        LABELS    unit io
    )
    cornerstone_add_test_executable(
        TARGET    test_master_loader_unit
        TEST_NAME unit_master_loader_test
        SOURCES   tests/unit/io/test_master_loader_unit.c
        LABELS    unit io
    )

    # Forensic Telemetry Logging Subsystems
    cornerstone_add_test_executable(
        TARGET    test_log_unit
        TEST_NAME unit_log_engine_test
        SOURCES   tests/unit/log/test_log_unit.c
        LABELS    unit log
    )
    cornerstone_add_test_executable(
        TARGET    test_log_fallback_unit
        TEST_NAME unit_log_fallback_test
        SOURCES   tests/unit/log/test_log_fallback_unit.c
        LABELS    unit log fallback
    )

    # Interactive State Debugger Subsystems
    cornerstone_add_test_executable(
        TARGET    test_debug_unit
        TEST_NAME unit_debug_engine_test
        SOURCES   tests/unit/debug/test_debug_unit.c
        LABELS    unit debug
    )
    cornerstone_add_test_executable(
        TARGET    test_debug_fallback_unit
        TEST_NAME unit_debug_fallback_test
        SOURCES   tests/unit/debug/test_debug_fallback_unit.c
        LABELS    unit debug fallback
    )

    # Audio Bus & Fallback Subsystems
    cornerstone_add_test_executable(
        TARGET    test_audio_bus_unit
        TEST_NAME unit_audio_bus_test
        SOURCES   tests/unit/audio/test_audio_bus_unit.c
        LABELS    unit audio
    )
    cornerstone_add_test_executable(
        TARGET    test_audio_fallback_unit
        TEST_NAME unit_audio_fallback_test
        SOURCES   tests/unit/audio/test_audio_fallback_unit.c
        LABELS    unit audio fallback
    )

    # Viewport Isolated Module Unit Tests
    cornerstone_add_test_executable(
        TARGET         test_reel_unit
        TEST_NAME      unit_reel_test
        SOURCES        tests/unit/viewport/test_reel_unit.c engine/src/viewport/reel/reel.c
        LIBS           ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         unit viewport
    )
    cornerstone_add_test_executable(
        TARGET    test_reel_fallback_unit
        TEST_NAME unit_reel_fallback_test
        SOURCES   tests/unit/viewport/test_reel_fallback_unit.c engine/src/viewport/reel/reel_fallback.c
        LABELS    unit viewport fallback
    )
    cornerstone_add_test_executable(
        TARGET         test_gui_palette_unit
        TEST_NAME      unit_gui_palette_test
        SOURCES        tests/unit/viewport/test_gui_palette_unit.c engine/src/viewport/styling/gui_palette.c
        LIBS           ${X11_LIBRARIES}
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         unit viewport
    )
    cornerstone_add_test_executable(
        TARGET    test_gui_layout_unit
        TEST_NAME unit_gui_layout_test
        SOURCES   tests/unit/viewport/test_gui_layout_unit.c engine/src/viewport/hud/gui_layout.c
        LABELS    unit viewport
    )
    cornerstone_add_test_executable(
        TARGET         test_gui_screens_unit
        TEST_NAME      unit_gui_screens_test
        SOURCES        tests/unit/viewport/test_gui_screens_unit.c
        LIBS           tenebris_engine_viewport tenebris_game_sim ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         unit viewport
    )
    cornerstone_add_test_executable(
        TARGET         test_gui_widgets_unit
        TEST_NAME      unit_gui_widgets_test
        SOURCES        tests/unit/viewport/test_gui_widgets_unit.c
                       engine/src/viewport/hud/gui_widgets.c
                       engine/src/viewport/hud/gui_layout.c
                       engine/src/viewport/styling/gui_palette.c
        LIBS           ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         unit viewport
    )
    cornerstone_add_test_executable(
        TARGET         test_gui_fonts_unit
        TEST_NAME      unit_gui_fonts_test
        SOURCES        tests/unit/viewport/test_gui_fonts_unit.c engine/src/viewport/styling/gui_fonts.c
        LIBS           ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         unit viewport
    )
    cornerstone_add_test_executable(
        TARGET         test_gui_assets_unit
        TEST_NAME      unit_gui_assets_test
        SOURCES        tests/unit/viewport/test_gui_assets_unit.c
                       engine/src/viewport/styling/gui_assets.c
                       engine/src/viewport/styling/gui_palette.c
        LIBS           ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         unit viewport
    )
    cornerstone_add_test_executable(
        TARGET         test_gui_view2d_unit
        TEST_NAME      unit_gui_view2d_test
        SOURCES        tests/unit/viewport/test_gui_view2d_unit.c
        LIBS           tenebris_engine_viewport tenebris_game_sim ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         unit viewport
    )
    cornerstone_add_test_executable(
        TARGET         test_gui_hud_unit
        TEST_NAME      unit_gui_hud_test
        SOURCES        tests/unit/viewport/test_gui_hud_unit.c
        LIBS           tenebris_engine_viewport tenebris_game_sim ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         unit viewport
    )
    cornerstone_add_test_executable(
        TARGET         test_gui_raster_unit
        TEST_NAME      unit_gui_raster_test
        SOURCES        tests/unit/viewport/test_gui_raster_unit.c
        LIBS           tenebris_engine_viewport tenebris_game_sim ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         unit viewport
    )
    cornerstone_add_test_executable(
        TARGET    test_gui_fallback_unit
        TEST_NAME unit_gui_fallback_test
        SOURCES   tests/unit/viewport/test_gui_fallback_unit.c engine/src/viewport/raster/gui_fallback.c
        LABELS    unit viewport fallback
    )

    # Layer 1 Game Simulation Domain Unit Tests
    cornerstone_add_test_executable(
        TARGET    test_combat
        TEST_NAME unit_combat_test
        SOURCES   tests/unit/test_combat_unit.c
        LIBS      tenebris_game_sim
        LABELS    unit sim
    )
    cornerstone_add_test_executable(
        TARGET    test_items
        TEST_NAME unit_items_test
        SOURCES   tests/unit/test_items_unit.c
        LIBS      tenebris_game_sim
        LABELS    unit sim
    )
    cornerstone_add_test_executable(
        TARGET    test_world
        TEST_NAME unit_world_test
        SOURCES   tests/unit/test_world_unit.c
        LIBS      tenebris_game_sim
        LABELS    unit sim
    )
    cornerstone_add_test_executable(
        TARGET    test_dispatch
        TEST_NAME unit_dispatch_test
        SOURCES   tests/unit/test_dispatch_unit.c
        LIBS      tenebris_game_sim
        LABELS    unit sim
    )

    # --------------------------------------------------------------------------
    # Sub-Assembly 3.3: Cross-Subsystem Integrated Harnesses
    # --------------------------------------------------------------------------
    cornerstone_add_test_executable(
        TARGET         test_utils
        TEST_NAME      integrated_utils_test
        SOURCES        tests/integrated/test_utils.c
        LIBS           tenebris_engine_viewport tenebris_game_sim ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         integrated engine
    )
    cornerstone_add_test_executable(
        TARGET    test_sim_integrated
        TEST_NAME integrated_sim_test
        SOURCES   tests/integrated/test_sim_integrated.c
        LIBS      tenebris_game_sim
        LABELS    integrated sim
    )
    cornerstone_add_test_executable(
        TARGET    test_io_integrated
        TEST_NAME integrated_io_test
        SOURCES   tests/integrated/test_io_integrated.c
        LABELS    integrated io
    )
    cornerstone_add_test_executable(
        TARGET    test_log_integrated
        TEST_NAME integrated_log_test
        SOURCES   tests/integrated/test_log_integrated.c
        LABELS    integrated log
    )
    cornerstone_add_test_executable(
        TARGET    test_debug_integrated
        TEST_NAME integrated_debug_test
        SOURCES   tests/integrated/test_debug_integrated.c
        LABELS    integrated debug
    )
    cornerstone_add_test_executable(
        TARGET    test_core_integrated
        TEST_NAME integrated_core_test
        SOURCES   tests/integrated/test_core_integrated.c
        LIBS      m
        LABELS    integrated core
    )
    cornerstone_add_test_executable(
        TARGET    test_audio_integrated
        TEST_NAME integrated_audio_test
        SOURCES   tests/integrated/test_audio_integrated.c
        LIBS      tenebris_game_sim
        LABELS    integrated audio
    )
    cornerstone_add_test_executable(
        TARGET         test_viewport_integrated
        TEST_NAME      integrated_viewport_test
        SOURCES        tests/integrated/test_viewport_integrated.c
        LIBS           tenebris_engine_viewport tenebris_game_sim ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         integrated viewport
    )

    # --------------------------------------------------------------------------
    # Sub-Assembly 3.4: System-Level Endurance, Fuzz & Stress Harnesses
    # --------------------------------------------------------------------------
    cornerstone_add_test_executable(
        TARGET    test_game_invariants
        TEST_NAME system_game_invariants_test
        SOURCES   tests/system/test_game_invariants.c
        LIBS      tenebris_game_sim
        LABELS    system sim
    )
    cornerstone_add_test_executable(
        TARGET    test_gameplay_scenarios
        TEST_NAME system_gameplay_scenarios_test
        SOURCES   tests/system/test_gameplay_scenarios.c
        LIBS      tenebris_game_sim
        LABELS    system sim
    )
    cornerstone_add_test_executable(
        TARGET    test_io_system
        TEST_NAME system_io_test
        SOURCES   tests/system/test_io_system.c
        LABELS    system io
    )
    cornerstone_add_test_executable(
        TARGET    test_log_system
        TEST_NAME system_log_test
        SOURCES   tests/system/test_log_system.c
        LABELS    system log
    )
    cornerstone_add_test_executable(
        TARGET    test_debug_system
        TEST_NAME system_debug_test
        SOURCES   tests/system/test_debug_system.c
        LABELS    system debug
    )
    cornerstone_add_test_executable(
        TARGET    test_core_system
        TEST_NAME system_core_test
        SOURCES   tests/system/test_core_system.c
        LIBS      m
        LABELS    system core
    )
    cornerstone_add_test_executable(
        TARGET    test_audio_system
        TEST_NAME system_audio_test
        SOURCES   tests/system/test_audio_system.c
        LABELS    system audio
    )
    cornerstone_add_test_executable(
        TARGET         test_viewport_system
        TEST_NAME      system_viewport_test
        SOURCES        tests/system/test_viewport_system.c
        LIBS           tenebris_engine_viewport tenebris_game_sim ${X11_LIBRARIES} m
        EXTRA_INCLUDES ${X11_INCLUDE_DIR}
        LABELS         system viewport
    )

# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ( Opt-In C++20/C++17 Header & Companion .hpp Verification Probes )
# ==============================================================================

    # --------------------------------------------------------------------------
    # Sub-Assembly 4.1: C++ Header & ROM Companion Verification Probe
    # --------------------------------------------------------------------------
# ==============================================================================
# BLOCK 4: AUDIT PROCEDURES
# ( Optional Modern C++20/C++17 Public Header Compatibility Verification Probe )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 4 Verification Suite (Build Orchestration)
# Module:        build.cmake.testing
# Mission:       Audit all public C headers and companion .hpp ROM headers under C++ compiler.
# ==============================================================================

    # --------------------------------------------------------------------------
    # Sub-Assembly 4.1: Modern C++ Public Header Compatibility Probe
    # --------------------------------------------------------------------------
    # Architectural Rationale ("Why"):
    #   1. Header Portability (Invariant 21): Cornerstone engine headers must be
    #      100% cleanly consumable by C++ compilation units without compiler warnings,
    #      symbol clashes, or missing 'extern "C"' guards.
    #   2. C++ Companion Verification: Validates that generated .hpp headers
    #      (log_telemetry_rom.hpp, debug_diagnostics_rom.hpp, io_config_rom.hpp)
    #      correctly wrap C static .rodata assets in std::string_view without syntax errors.
    #   3. Automated Probe Synthesis: Generates a self-auditing .cpp translation
    #      unit on the fly and builds it with strict -Wall -Wextra -Wpedantic flags.
    # --------------------------------------------------------------------------
    if(CORNERSTONE_VERIFY_CPP_HEADERS)
        include(CheckLanguage)
        check_language(CXX)
        if(CMAKE_CXX_COMPILER)
            enable_language(CXX)
            if(CORNERSTONE_CXX_STANDARD STREQUAL "AUTO")
                if("cxx_std_20" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
                    set(_cs_cxx_tier 20)
                else()
                    set(_cs_cxx_tier 17)
                endif()
            elseif(CORNERSTONE_CXX_STANDARD MATCHES "^(17|20)$")
                set(_cs_cxx_tier "${CORNERSTONE_CXX_STANDARD}")
            else()
                message(FATAL_ERROR
                    "Invalid CORNERSTONE_CXX_STANDARD='${CORNERSTONE_CXX_STANDARD}'. Expected AUTO, 20, or 17."
                )
            endif()

            message(STATUS
                "[Cornerstone] C++ header compatibility tier: C++${_cs_cxx_tier} "
                "(requested: ${CORNERSTONE_CXX_STANDARD}, floor: C++17, extensions: OFF)"
            )

            set(_cpp_probe_src "${CMAKE_CURRENT_BINARY_DIR}/generated/cornerstone_cpp_compat.cpp")
            file(WRITE "${_cpp_probe_src}"
                "/* Auto-generated Cornerstone C++20/17 Header & Companion .hpp Probe */\n"
                "#include \"core/core.h\"\n"
                "#include \"log/log.h\"\n"
                "#include \"log/log_fallback.h\"\n"
                "#include \"debug/debug.h\"\n"
                "#include \"debug/debug_fallback.h\"\n"
                "#include \"io/io.h\"\n"
                "#include \"audio/audio.h\"\n"
                "#include \"viewport/viewport.h\"\n"
                "#include \"generated/log_telemetry_rom.hpp\"\n"
                "#include \"generated/debug_diagnostics_rom.hpp\"\n"
                "#include \"generated/io_config_rom.hpp\"\n"
                "CS_STATIC_ASSERT(CS_CXX_STD_TIER >= 17, \"Cornerstone requires C++17+ floor\");\n"
                "int main(void) {\n"
                "    auto tel_view = cornerstone::rom::get_log_rom_asset_t_view(0);\n"
                "    return (diag_severity_is_valid(SEVERITY_PASS) &&\n"
                "            core_dimensions_are_valid(640, 360) &&\n"
                "            tel_view.is_valid()) ? 0 : 1;\n"
                "}\n"
            )
            add_executable(test_cpp_header_compat "${_cpp_probe_src}")
            set_target_properties(test_cpp_header_compat PROPERTIES
                CXX_STANDARD ${_cs_cxx_tier}
                CXX_STANDARD_REQUIRED ON
                CXX_EXTENSIONS OFF
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
            )
            target_include_directories(test_cpp_header_compat PRIVATE
                ${ENGINE_PUBLIC_INCLUDES}
                ${X11_INCLUDE_DIR}
                "${CMAKE_CURRENT_BINARY_DIR}"
            )
            target_link_libraries(test_cpp_header_compat PRIVATE
                tenebris_engine_util
                ${X11_LIBRARIES}
                m
            )
            target_compile_options(test_cpp_header_compat PRIVATE ${GAME_WARNING_FLAGS})
            add_test(NAME audit_cpp_headers_test COMMAND test_cpp_header_compat)
            set_tests_properties(audit_cpp_headers_test PROPERTIES LABELS "audit;cpp")
            list(APPEND CORNERSTONE_CTEST_NAMES audit_cpp_headers_test)
        else()
            message(WARNING "CORNERSTONE_VERIFY_CPP_HEADERS=ON ignored: no C++ compiler detected")
        endif()
    endif()

# ==============================================================================
# BLOCK 5: CLOSING BLOCK
# ( CLI Interface Contracts & CTest Execution Handshake )
# ==============================================================================
# Engine:        Cornerstone Game Engine
# Stratum:       Layer 4 Verification Suite (Build Orchestration)
# Module:        build.cmake.testing
# Mission:       Bind CLI contract tests and establish uniform workspace root execution.
# ==============================================================================

    # --------------------------------------------------------------------------
    # Sub-Assembly 5.1: Primary Binary CLI & Boot Audit Registrations
    # --------------------------------------------------------------------------
    # Architectural Rationale ("Why"):
    #   Verifies the end-to-end command-line contract of the shipping executable
    #   under CTest without requiring an interactive window manager:
    #   - cli_help_test: Validates flag parsing and designer usage instructions.
    #   - cli_version_test: Confirms milestone version and engine provenance string.
    #   - subsystem_audit_test: Executes all in-binary Block 4 audit tripwires,
    #     measuring zero-heap budget and affirming the [GRADE:+100] clean bill of health.
    # --------------------------------------------------------------------------
    add_test(NAME cli_help_test COMMAND tenebris-lux --help)
    add_test(NAME cli_version_test COMMAND tenebris-lux --version)
    add_test(NAME subsystem_audit_test COMMAND tenebris-lux --audit)
    set_tests_properties(
        cli_help_test
        cli_version_test
        subsystem_audit_test
        PROPERTIES LABELS "cli;audit"
    )
    list(APPEND CORNERSTONE_CTEST_NAMES
        cli_help_test
        cli_version_test
        subsystem_audit_test
    )

    # --------------------------------------------------------------------------
    # Sub-Assembly 5.2: Uniform Working Directory Binding for All CTest Entries
    # --------------------------------------------------------------------------
    # Architectural Rationale ("Why"):
    #   Ensures that every test harness executes with the repository root as its
    #   current working directory. This guarantees that relative VFS paths
    #   (e.g., engine/config/ and game/data/) resolve identically across both
    #   manual CLI runs and automated CTest CI runs.
    # --------------------------------------------------------------------------
    set_tests_properties(${CORNERSTONE_CTEST_NAMES}
        PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endif()
