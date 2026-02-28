from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
SEMANTIC_PASSES_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
SEMA_PASS_MANAGER_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m198_lowering_runtime_swift_metadata_bridge_section_present() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## M198 lowering/runtime swift metadata bridge",
        "tmp/artifacts/compilation/objc3c-native/m198/lowering-runtime-swift-metadata-bridge/",
        "tmp/reports/objc3c-native/m198/lowering-runtime-swift-metadata-bridge/",
        "tmp/artifacts/compilation/objc3c-native/m198/lowering-runtime-swift-metadata-bridge/module.ll",
        "tmp/artifacts/compilation/objc3c-native/m198/lowering-runtime-swift-metadata-bridge/module.manifest.json",
        "tmp/artifacts/compilation/objc3c-native/m198/lowering-runtime-swift-metadata-bridge/module.diagnostics.json",
        "tmp/reports/objc3c-native/m198/lowering-runtime-swift-metadata-bridge/abi-ir-anchors.txt",
        "tmp/reports/objc3c-native/m198/lowering-runtime-swift-metadata-bridge/swift-metadata-bridge-source-anchors.txt",
        "BuildSemanticTypeMetadataHandoff(...)",
        "param_has_invalid_type_suffix",
        "deterministic_type_metadata_handoff",
        "type_metadata_global_entries",
        "type_metadata_function_entries",
        "Objc3IRFrontendMetadata",
        "Objc3LoweringIRBoundaryReplayKey(...)",
        '"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}',
        "python -m pytest tests/tooling/test_objc3c_m198_lowering_swift_metadata_bridge_contract.py -q",
    ):
        assert text in fragment


def test_m198_lowering_runtime_swift_metadata_bridge_source_anchors_map_to_sources() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    semantic_passes_source = _read(SEMANTIC_PASSES_SOURCE)
    sema_pass_manager_source = _read(SEMA_PASS_MANAGER_SOURCE)
    frontend_artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    ir_emitter_source = _read(IR_EMITTER_SOURCE)
    lowering_contract_source = _read(LOWERING_CONTRACT_SOURCE)

    mapped_source_anchors = (
        (
            "info.param_has_invalid_type_suffix.push_back(HasInvalidParamTypeSuffix(param));",
            semantic_passes_source,
            "info.param_has_invalid_type_suffix.push_back(HasInvalidParamTypeSuffix(param));",
        ),
        (
            "metadata.param_has_invalid_type_suffix = source.param_has_invalid_type_suffix;",
            semantic_passes_source,
            "metadata.param_has_invalid_type_suffix = source.param_has_invalid_type_suffix;",
        ),
        (
            "metadata.param_has_invalid_type_suffix.size() == metadata.arity;",
            semantic_passes_source,
            "metadata.param_has_invalid_type_suffix.size() == metadata.arity;",
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
            '<< ",\\"deterministic_type_metadata_handoff\\":"',
            frontend_artifacts_source,
            '<< ",\\"deterministic_type_metadata_handoff\\":"',
        ),
        (
            '<< ",\\"type_metadata_global_entries\\":"',
            frontend_artifacts_source,
            '<< ",\\"type_metadata_global_entries\\":"',
        ),
        (
            '<< ",\\"type_metadata_function_entries\\":"',
            frontend_artifacts_source,
            '<< ",\\"type_metadata_function_entries\\":"',
        ),
        (
            'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol',
            frontend_artifacts_source,
            'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol',
        ),
        (
            '<< "\\",\\"runtime_dispatch_arg_slots\\":" << options.lowering.max_message_send_args',
            frontend_artifacts_source,
            '<< "\\",\\"runtime_dispatch_arg_slots\\":" << options.lowering.max_message_send_args',
        ),
        (
            '<< ",\\"selector_global_ordering\\":\\"lexicographic\\"},\\n";',
            frontend_artifacts_source,
            '<< ",\\"selector_global_ordering\\":\\"lexicographic\\"},\\n";',
        ),
        (
            "Objc3IRFrontendMetadata ir_frontend_metadata;",
            frontend_artifacts_source,
            "Objc3IRFrontendMetadata ir_frontend_metadata;",
        ),
        (
            "if (!EmitObjc3IRText(pipeline_result.program, options.lowering, ir_frontend_metadata, bundle.ir_text, ir_error)) {",
            frontend_artifacts_source,
            "if (!EmitObjc3IRText(pipeline_result.program, options.lowering, ir_frontend_metadata, bundle.ir_text, ir_error)) {",
        ),
        (
            'out << "; frontend_profile = language_version=" << static_cast<unsigned>(frontend_metadata_.language_version)',
            ir_emitter_source,
            'out << "; frontend_profile = language_version=" << static_cast<unsigned>(frontend_metadata_.language_version)',
        ),
        (
            'out << "!objc3.frontend = !{!0}\\n";',
            ir_emitter_source,
            'out << "!objc3.frontend = !{!0}\\n";',
        ),
        (
            "Objc3LoweringIRBoundaryReplayKey(...)",
            lowering_contract_source,
            "std::string Objc3LoweringIRBoundaryReplayKey(const Objc3LoweringIRBoundary &boundary) {",
        ),
        (
            'return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +',
            lowering_contract_source,
            'return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +',
        ),
    )

    for doc_anchor, source_text, source_anchor in mapped_source_anchors:
        assert doc_anchor in fragment
        assert source_anchor in source_text
