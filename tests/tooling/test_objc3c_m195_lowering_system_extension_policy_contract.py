from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
FRONTEND_ANCHOR_SOURCE = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
PIPELINE_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "frontend_pipeline_contract.h"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m195_lowering_runtime_system_extension_policy_section_present() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## M195 lowering/runtime system-extension conformance and policy",
        "tmp/artifacts/compilation/objc3c-native/m195/lowering-runtime-system-extension-policy/",
        "tmp/reports/objc3c-native/m195/lowering-runtime-system-extension-policy/",
        "tmp/artifacts/compilation/objc3c-native/m195/lowering-runtime-system-extension-policy/module.ll",
        "tmp/artifacts/compilation/objc3c-native/m195/lowering-runtime-system-extension-policy/module.manifest.json",
        "tmp/artifacts/compilation/objc3c-native/m195/lowering-runtime-system-extension-policy/module.diagnostics.json",
        "tmp/reports/objc3c-native/m195/lowering-runtime-system-extension-policy/abi-ir-anchors.txt",
        "tmp/reports/objc3c-native/m195/lowering-runtime-system-extension-policy/system-extension-policy-source-anchors.txt",
        "; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic",
        "; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>",
        "!objc3.frontend = !{!0}",
        "declare i32 @<symbol>(i32, ptr, i32, ..., i32)",
        '"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}',
        "ValidateSupportedLanguageVersion(...)",
        "ValidateSupportedCompatibilityMode(...)",
        "TryNormalizeObjc3LoweringContract(...)",
        'kRuntimeDispatchDefaultSymbol = "objc3_msgsend_i32"',
        'output_dir = "tmp/artifacts/compilation/objc3c-native"',
        "Objc3LoweringIRBoundaryReplayKey(...)",
        'return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +',
        'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol',
        "python -m pytest tests/tooling/test_objc3c_m195_lowering_system_extension_policy_contract.py -q",
    ):
        assert text in fragment


def test_m195_lowering_runtime_system_extension_policy_source_anchor_mapping() -> None:
    frontend_anchor_source = _read(FRONTEND_ANCHOR_SOURCE)
    pipeline_contract_source = _read(PIPELINE_CONTRACT_SOURCE)
    lowering_contract_source = _read(LOWERING_CONTRACT_SOURCE)
    emitter_source = _read(IR_EMITTER_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)

    for marker in (
        "static bool ValidateSupportedLanguageVersion(uint8_t requested_language_version, std::string &error) {",
        "static bool ValidateSupportedCompatibilityMode(uint8_t requested_compatibility_mode, std::string &error) {",
        "if (!TryNormalizeObjc3LoweringContract(frontend_options.lowering, normalized_lowering, lowering_error)) {",
        "frontend_options.lowering.max_message_send_args = options.max_message_send_args;",
        "frontend_options.lowering.runtime_dispatch_symbol = options.runtime_dispatch_symbol;",
    ):
        assert marker in frontend_anchor_source

    for marker in (
        "inline constexpr std::size_t kRuntimeDispatchDefaultArgs = 4;",
        "inline constexpr std::size_t kRuntimeDispatchMaxArgs = 16;",
        'inline constexpr const char *kRuntimeDispatchDefaultSymbol = "objc3_msgsend_i32";',
        'std::string output_dir = "tmp/artifacts/compilation/objc3c-native";',
    ):
        assert marker in pipeline_contract_source

    for marker in (
        "bool TryNormalizeObjc3LoweringContract(const Objc3LoweringContract &input,",
        "error = \"invalid lowering contract runtime_dispatch_symbol (expected [A-Za-z_.$][A-Za-z0-9_.$]*): \" +",
        "std::string Objc3LoweringIRBoundaryReplayKey(const Objc3LoweringIRBoundary &boundary) {",
        'return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +',
    ):
        assert marker in lowering_contract_source

    for marker in (
        'out << "; lowering_ir_boundary = " << Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_) << "\\n";',
        'out << "; frontend_profile = language_version="',
        'out << "!objc3.frontend = !{!0}\\n";',
        'out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";',
    ):
        assert marker in emitter_source

    for marker in (
        'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol',
        '<< "\\",\\"runtime_dispatch_arg_slots\\":" << options.lowering.max_message_send_args',
        '<< ",\\"selector_global_ordering\\":\\"lexicographic\\"},\\n";',
    ):
        assert marker in artifacts_source
