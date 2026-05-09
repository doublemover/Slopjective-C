from objc3c_parser_sema_integration_support import (
    ARTIFACTS_HEADER,
    ARTIFACTS_SOURCE,
    BUILD_SCRIPT,
    DIAG_ARTIFACTS_HEADER,
    DRIVER_OBJC3_PATH,
    PARSE_CMAKE_FILE,
    PIPELINE_ORCHESTRATION_SOURCE,
    PIPELINE_SEMA_STAGE_RUNNER,
    PIPELINE_STAGE_RUNNER,
    PIPELINE_TYPES,
    SEMA_CMAKE_FILE,
    assert_contains_all,
    assert_in_order,
    read_source,
)


def test_ast_builder_scaffold_is_registered_in_build_surfaces() -> None:
    parse_cmake = read_source(PARSE_CMAKE_FILE)
    sema_cmake = read_source(SEMA_CMAKE_FILE)
    build_script = read_source(BUILD_SCRIPT)
    assert "objc3_ast_builder.cpp" in parse_cmake
    assert "target_link_libraries(objc3c_parse PUBLIC" in parse_cmake
    assert "target_link_libraries(objc3c_sema PUBLIC" in sema_cmake
    assert "add_library(objc3c_sema_type_system INTERFACE)" in sema_cmake
    assert "target_link_libraries(objc3c_sema_type_system INTERFACE" in sema_cmake
    assert "objc3c_sema_type_system" in sema_cmake

    assert_in_order(
        parse_cmake,
        [
            "add_library(objc3c_parse STATIC",
            "objc3_ast_builder.cpp",
            "target_link_libraries(objc3c_parse PUBLIC",
        ],
    )

    assert_in_order(
        sema_cmake,
        [
            "add_library(objc3c_sema STATIC",
            "target_link_libraries(objc3c_sema PUBLIC",
            "add_library(objc3c_sema_type_system INTERFACE)",
            "target_link_libraries(objc3c_sema_type_system INTERFACE",
        ],
    )

    assert '"native/objc3c/src/parse/objc3_ast_builder.cpp"' in build_script


def test_frontend_pipeline_artifact_boundary_uses_diagnostics_bus_contract() -> None:
    pipeline_types = read_source(PIPELINE_TYPES)
    pipeline_orchestration = read_source(PIPELINE_ORCHESTRATION_SOURCE)
    pipeline_stage_runner = read_source(PIPELINE_STAGE_RUNNER)
    sema_stage_runner = read_source(PIPELINE_SEMA_STAGE_RUNNER)
    artifacts_header = read_source(ARTIFACTS_HEADER)
    artifacts_source = read_source(ARTIFACTS_SOURCE)
    diag_header = read_source(DIAG_ARTIFACTS_HEADER)
    driver_source = read_source(DRIVER_OBJC3_PATH)

    assert_contains_all(
        pipeline_types,
        (
            "Objc3FrontendDiagnosticsBus stage_diagnostics;",
            "std::array<std::size_t, 3> sema_diagnostics_after_pass = {0, 0, 0};",
            "Objc3SemaParityContractSurface sema_parity_surface;",
        ),
    )
    assert_contains_all(
        pipeline_orchestration,
        (
            "result.sema_diagnostics_after_pass = sema_result.diagnostics_after_pass;",
            "result.sema_parity_surface = sema_result.parity_surface;",
        ),
    )
    assert_contains_all(
        sema_stage_runner,
        (
            "sema_input.language_profile = Objc3SemaLanguageProfile::Canonical;",
            "sema_input.canonical_literal_rejection_counts.yes_literal_sites =",
            "sema_input.canonical_literal_rejection_counts.no_literal_sites =",
            "sema_input.canonical_literal_rejection_counts.null_literal_sites =",
            "result.canonical_literal_rejection_counts.yes_literal_sites;",
        ),
    )
    assert "TransportObjc3DiagnosticsToParsedProgram(result.stage_diagnostics," in pipeline_stage_runner

    assert_contains_all(
        artifacts_header,
        (
            "Objc3FrontendDiagnosticsBus stage_diagnostics;",
            "std::vector<std::string> post_pipeline_diagnostics;",
        ),
    )
    assert_contains_all(
        artifacts_source,
        (
            "bundle.stage_diagnostics = pipeline_result.stage_diagnostics;",
            "bundle.diagnostics = FlattenStageDiagnostics(bundle.stage_diagnostics);",
            '\\"language_version\\":',
            '\\"language_profile\\":\\"',
            '\\"canonical_literal_rejection_diagnostics\\":',
            "LanguageProfileName(options.language_profile)",
            "sema_pass_manager",
            "diagnostics_after_build",
            "diagnostics_emitted_by_build",
            "diagnostics_monotonic",
            "deterministic_semantic_diagnostics",
            "deterministic_type_metadata_handoff",
            "parity_ready",
        ),
    )
    assert_in_order(
        artifacts_source,
        [
            'manifest << "  \\"frontend\\": {\\n";',
            'manifest << "    \\"language_version\\":"',
            'manifest << "    \\"language_profile\\":\\""',
            'manifest << "    \\"canonical_literal_rejection_diagnostics\\":true,',
            'manifest << "    \\"max_message_send_args\\":"',
        ],
    )

    assert "const Objc3FrontendDiagnosticsBus &stage_diagnostics" in diag_header
    assert "artifacts.stage_diagnostics" in driver_source
    assert "artifacts.post_pipeline_diagnostics" in driver_source
