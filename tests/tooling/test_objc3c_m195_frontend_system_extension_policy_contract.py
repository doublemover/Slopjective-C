from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
FRONTEND_ANCHOR_SOURCE = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
PIPELINE_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "frontend_pipeline_contract.h"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m195_frontend_system_extension_policy_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M195 frontend system-extension conformance and policy",
        "ValidateSupportedLanguageVersion(...)",
        "ValidateSupportedCompatibilityMode(...)",
        "TryNormalizeObjc3LoweringContract(...)",
        "kRuntimeDispatchDefaultArgs = 4",
        "kRuntimeDispatchMaxArgs = 16",
        "kRuntimeDispatchDefaultSymbol = \"objc3_msgsend_i32\"",
        "frontend_options.lowering.max_message_send_args = options.max_message_send_args;",
        "frontend_options.lowering.runtime_dispatch_symbol = options.runtime_dispatch_symbol;",
        "python -m pytest tests/tooling/test_objc3c_m195_frontend_system_extension_policy_contract.py -q",
    ):
        assert text in fragment


def test_m195_frontend_system_extension_policy_markers_map_to_sources() -> None:
    frontend_anchor_source = _read(FRONTEND_ANCHOR_SOURCE)
    pipeline_contract_source = _read(PIPELINE_CONTRACT_SOURCE)
    lowering_contract_source = _read(LOWERING_CONTRACT_SOURCE)

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
        "inline constexpr const char *kRuntimeDispatchDefaultSymbol = \"objc3_msgsend_i32\";",
    ):
        assert marker in pipeline_contract_source

    for marker in (
        "bool TryNormalizeObjc3LoweringContract(const Objc3LoweringContract &input,",
        "if (input.max_message_send_args > kObjc3RuntimeDispatchMaxArgs) {",
        "if (!IsValidRuntimeDispatchSymbol(input.runtime_dispatch_symbol)) {",
    ):
        assert marker in lowering_contract_source
