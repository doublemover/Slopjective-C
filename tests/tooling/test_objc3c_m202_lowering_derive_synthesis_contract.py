from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
SEMA_PASSES_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
SEMA_PASS_MANAGER_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m202_lowering_runtime_derive_synthesis_pipeline_section_present() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## M202 lowering/runtime derive/synthesis pipeline",
        "tmp/artifacts/compilation/objc3c-native/m202/lowering-runtime-derive-synthesis-pipeline/",
        "tmp/reports/objc3c-native/m202/lowering-runtime-derive-synthesis-pipeline/",
        "tmp/artifacts/compilation/objc3c-native/m202/lowering-runtime-derive-synthesis-pipeline/module.ll",
        "tmp/artifacts/compilation/objc3c-native/m202/lowering-runtime-derive-synthesis-pipeline/module.manifest.json",
        "tmp/artifacts/compilation/objc3c-native/m202/lowering-runtime-derive-synthesis-pipeline/module.diagnostics.json",
        "tmp/reports/objc3c-native/m202/lowering-runtime-derive-synthesis-pipeline/abi-ir-anchors.txt",
        "tmp/reports/objc3c-native/m202/lowering-runtime-derive-synthesis-pipeline/derive-synthesis-source-anchors.txt",
        "; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic",
        "; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>",
        "!objc3.frontend = !{!0}",
        "declare i32 @<symbol>(i32, ptr, i32, ..., i32)",
        '"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}',
        "BuildSemanticIntegrationSurface(...)",
        "BuildSemanticTypeMetadataHandoff(...)",
        "IsDeterministicSemanticTypeMetadataHandoff(...)",
        "global_names_lexicographic",
        "functions_lexicographic",
        "deterministic_type_metadata_handoff",
        "type_metadata_global_entries",
        "type_metadata_function_entries",
        "semantic_surface",
        "resolved_global_symbols",
        "resolved_function_symbols",
        "runtime_dispatch_symbol",
        "runtime_dispatch_arg_slots",
        "selector_global_ordering",
        "byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.",
        "python -m pytest tests/tooling/test_objc3c_m202_lowering_derive_synthesis_contract.py -q",
    ):
        assert text in fragment


def test_m202_lowering_runtime_derive_synthesis_source_anchors_map_to_sources() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    sema_passes_source = _read(SEMA_PASSES_SOURCE)
    sema_pass_manager_source = _read(SEMA_PASS_MANAGER_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    lowering_contract_source = _read(LOWERING_CONTRACT_SOURCE)
    emitter_source = _read(IR_EMITTER_SOURCE)

    mapped_source_anchors = (
        (
            "Objc3SemanticIntegrationSurface BuildSemanticIntegrationSurface(const Objc3ParsedProgram &program,",
            sema_passes_source,
            "Objc3SemanticIntegrationSurface BuildSemanticIntegrationSurface(const Objc3ParsedProgram &program,",
        ),
        (
            "Objc3SemanticTypeMetadataHandoff BuildSemanticTypeMetadataHandoff(const Objc3SemanticIntegrationSurface &surface) {",
            sema_passes_source,
            "Objc3SemanticTypeMetadataHandoff BuildSemanticTypeMetadataHandoff(const Objc3SemanticIntegrationSurface &surface) {",
        ),
        (
            "bool IsDeterministicSemanticTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff) {",
            sema_passes_source,
            "bool IsDeterministicSemanticTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff) {",
        ),
        (
            "result.integration_surface = BuildSemanticIntegrationSurface(*input.program, pass_diagnostics);",
            sema_pass_manager_source,
            "result.integration_surface = BuildSemanticIntegrationSurface(*input.program, pass_diagnostics);",
        ),
        (
            "result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);",
            sema_pass_manager_source,
            "result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);",
        ),
        (
            "result.deterministic_type_metadata_handoff =",
            sema_pass_manager_source,
            "result.deterministic_type_metadata_handoff =",
        ),
        (
            "IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);",
            sema_pass_manager_source,
            "IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);",
        ),
        (
            "result.parity_surface.type_metadata_global_entries = result.type_metadata_handoff.global_names_lexicographic.size();",
            sema_pass_manager_source,
            "result.parity_surface.type_metadata_global_entries = result.type_metadata_handoff.global_names_lexicographic.size();",
        ),
        (
            "result.parity_surface.type_metadata_function_entries = result.type_metadata_handoff.functions_lexicographic.size();",
            sema_pass_manager_source,
            "result.parity_surface.type_metadata_function_entries = result.type_metadata_handoff.functions_lexicographic.size();",
        ),
        (
            "result.parity_surface.deterministic_type_metadata_handoff = result.deterministic_type_metadata_handoff;",
            sema_pass_manager_source,
            "result.parity_surface.deterministic_type_metadata_handoff = result.deterministic_type_metadata_handoff;",
        ),
        (
            '<< ",\\"deterministic_type_metadata_handoff\\":"',
            artifacts_source,
            '<< ",\\"deterministic_type_metadata_handoff\\":"',
        ),
        (
            '<< (pipeline_result.sema_parity_surface.deterministic_type_metadata_handoff ? "true" : "false")',
            artifacts_source,
            '<< (pipeline_result.sema_parity_surface.deterministic_type_metadata_handoff ? "true" : "false")',
        ),
        (
            '<< ",\\"type_metadata_global_entries\\":"',
            artifacts_source,
            '<< ",\\"type_metadata_global_entries\\":"',
        ),
        (
            "<< pipeline_result.sema_parity_surface.type_metadata_global_entries",
            artifacts_source,
            "<< pipeline_result.sema_parity_surface.type_metadata_global_entries",
        ),
        (
            '<< ",\\"type_metadata_function_entries\\":"',
            artifacts_source,
            '<< ",\\"type_metadata_function_entries\\":"',
        ),
        (
            '<< pipeline_result.sema_parity_surface.type_metadata_function_entries << "},\\n";',
            artifacts_source,
            '<< pipeline_result.sema_parity_surface.type_metadata_function_entries << "},\\n";',
        ),
        (
            'manifest << "      \\"semantic_surface\\": {\\"declared_globals\\":" << program.globals.size()',
            artifacts_source,
            'manifest << "      \\"semantic_surface\\": {\\"declared_globals\\":" << program.globals.size()',
        ),
        (
            '<< ",\\"resolved_global_symbols\\":" << pipeline_result.integration_surface.globals.size()',
            artifacts_source,
            '<< ",\\"resolved_global_symbols\\":" << pipeline_result.integration_surface.globals.size()',
        ),
        (
            '<< ",\\"resolved_function_symbols\\":" << pipeline_result.integration_surface.functions.size()',
            artifacts_source,
            '<< ",\\"resolved_function_symbols\\":" << pipeline_result.integration_surface.functions.size()',
        ),
        (
            'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol',
            artifacts_source,
            'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol',
        ),
        (
            '<< "\\",\\"runtime_dispatch_arg_slots\\":" << options.lowering.max_message_send_args',
            artifacts_source,
            '<< "\\",\\"runtime_dispatch_arg_slots\\":" << options.lowering.max_message_send_args',
        ),
        (
            '<< ",\\"selector_global_ordering\\":\\"lexicographic\\"},\\n";',
            artifacts_source,
            '<< ",\\"selector_global_ordering\\":\\"lexicographic\\"},\\n";',
        ),
        (
            "Objc3LoweringIRBoundaryReplayKey(...)",
            lowering_contract_source,
            "Objc3LoweringIRBoundaryReplayKey(const Objc3LoweringIRBoundary &boundary)",
        ),
        (
            'return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +',
            lowering_contract_source,
            'return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +',
        ),
        (
            'out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";',
            emitter_source,
            'out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";',
        ),
    )

    for doc_anchor, source_text, source_anchor in mapped_source_anchors:
        assert doc_anchor in fragment
        assert source_anchor in source_text

    assert 'out << "; lowering_ir_boundary = " << Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_) << "\\n";' in (
        emitter_source
    )
