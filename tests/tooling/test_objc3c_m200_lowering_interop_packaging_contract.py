from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
FRONTEND_OPTIONS_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_frontend_options.cpp"
FRONTEND_ANCHOR_SOURCE = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
C_API_RUNNER_SOURCE = ROOT / "native" / "objc3c" / "src" / "tools" / "objc3c_frontend_c_api_runner.cpp"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m200_lowering_runtime_interop_integration_packaging_section_present() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## M200 lowering/runtime interop integration suite and packaging",
        "tmp/artifacts/compilation/objc3c-native/m200/lowering-runtime-interop-integration-packaging/",
        "tmp/reports/objc3c-native/m200/lowering-runtime-interop-integration-packaging/",
        "tmp/artifacts/compilation/objc3c-native/m200/lowering-runtime-interop-integration-packaging/module.ll",
        "tmp/artifacts/compilation/objc3c-native/m200/lowering-runtime-interop-integration-packaging/module.manifest.json",
        "tmp/artifacts/compilation/objc3c-native/m200/lowering-runtime-interop-integration-packaging/module.diagnostics.json",
        "tmp/reports/objc3c-native/m200/lowering-runtime-interop-integration-packaging/abi-ir-anchors.txt",
        "tmp/reports/objc3c-native/m200/lowering-runtime-interop-integration-packaging/interop-packaging-source-anchors.txt",
        "; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic",
        "; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>",
        "!objc3.frontend = !{!0}",
        "declare i32 @<symbol>(i32, ptr, i32, ..., i32)",
        '"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}',
        "python -m pytest tests/tooling/test_objc3c_m200_lowering_interop_packaging_contract.py -q",
    ):
        assert text in fragment


def test_m200_lowering_runtime_interop_packaging_source_anchors_map_to_sources() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    frontend_options_source = _read(FRONTEND_OPTIONS_SOURCE)
    frontend_anchor_source = _read(FRONTEND_ANCHOR_SOURCE)
    c_api_runner_source = _read(C_API_RUNNER_SOURCE)
    lowering_contract_source = _read(LOWERING_CONTRACT_SOURCE)
    emitter_source = _read(IR_EMITTER_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)

    mapped_source_anchors = (
        (
            "options.lowering.runtime_dispatch_symbol = cli_options.runtime_dispatch_symbol;",
            frontend_options_source,
            "options.lowering.runtime_dispatch_symbol = cli_options.runtime_dispatch_symbol;",
        ),
        (
            "frontend_options.lowering.runtime_dispatch_symbol = options.runtime_dispatch_symbol;",
            frontend_anchor_source,
            "frontend_options.lowering.runtime_dispatch_symbol = options.runtime_dispatch_symbol;",
        ),
        (
            "compile_options.runtime_dispatch_symbol = runtime_symbol;",
            c_api_runner_source,
            "compile_options.runtime_dispatch_symbol = runtime_symbol;",
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
            'out << "; lowering_ir_boundary = " << Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_) << "\\n";',
            emitter_source,
            'out << "; lowering_ir_boundary = " << Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_) << "\\n";',
        ),
        (
            'out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";',
            emitter_source,
            'out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";',
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
    )

    for doc_anchor, source_text, source_anchor in mapped_source_anchors:
        assert doc_anchor in fragment
        assert source_anchor in source_text
