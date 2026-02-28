from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
PIPELINE_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "frontend_pipeline_contract.h"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m194_frontend_atomics_memory_order_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M194 frontend atomics and memory-order mapping",
        "IsAssignmentOperatorToken(TokenKind kind)",
        "MatchAssignmentOperator(std::string &op)",
        "if (Match(TokenKind::AmpersandEqual)) {",
        "if (Match(TokenKind::PipeEqual)) {",
        "if (Match(TokenKind::CaretEqual)) {",
        "if (Match(TokenKind::LessLessEqual)) {",
        "if (Match(TokenKind::GreaterGreaterEqual)) {",
        "TryGetCompoundAssignmentBinaryOpcode(...)",
        "NoThrowFailClosed",
        "kRuntimeDispatchDefaultArgs = 4",
        "kRuntimeDispatchDefaultSymbol = \"objc3_msgsend_i32\"",
        "python -m pytest tests/tooling/test_objc3c_m194_frontend_atomics_memory_order_contract.py -q",
    ):
        assert text in fragment


def test_m194_frontend_atomics_memory_order_markers_map_to_sources() -> None:
    parser_source = _read(PARSER_SOURCE)
    lowering_contract_source = _read(LOWERING_CONTRACT_SOURCE)
    pipeline_contract_source = _read(PIPELINE_CONTRACT_SOURCE)

    for marker in (
        "static bool IsAssignmentOperatorToken(TokenKind kind) {",
        "bool MatchAssignmentOperator(std::string &op) {",
        "if (Match(TokenKind::AmpersandEqual)) {",
        "if (Match(TokenKind::PipeEqual)) {",
        "if (Match(TokenKind::CaretEqual)) {",
        "if (Match(TokenKind::LessLessEqual)) {",
        "if (Match(TokenKind::GreaterGreaterEqual)) {",
    ):
        assert marker in parser_source

    for marker in (
        "bool TryGetCompoundAssignmentBinaryOpcode(const std::string &op, std::string &opcode) {",
        "if (op == \"&=\") {",
        "opcode = \"and\";",
        "if (op == \"|=\") {",
        "opcode = \"or\";",
        "if (op == \"^=\") {",
        "opcode = \"xor\";",
        "if (op == \"<<=\") {",
        "opcode = \"shl\";",
        "if (op == \">>=\") {",
        "opcode = \"ashr\";",
    ):
        assert marker in lowering_contract_source

    for marker in (
        "inline constexpr std::size_t kRuntimeDispatchDefaultArgs = 4;",
        "inline constexpr const char *kRuntimeDispatchDefaultSymbol = \"objc3_msgsend_i32\";",
        "enum class ErrorPropagationModel : std::uint8_t {",
        "NoThrowFailClosed = 0,",
    ):
        assert marker in pipeline_contract_source
