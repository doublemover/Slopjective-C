from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
PIPELINE_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _assert_in_order(text: str, snippets: list[str]) -> None:
    cursor = -1
    for snippet in snippets:
        index = text.find(snippet)
        assert index != -1, f"missing snippet: {snippet}"
        assert index > cursor, f"snippet out of order: {snippet}"
        cursor = index


def test_lowering_contract_module_is_wired() -> None:
    assert HEADER.exists()
    assert SOURCE.exists()
    assert '#include "lower/objc3_lowering_contract.h"' in _read(PIPELINE_TYPES)
    header = _read(HEADER)
    source = _read(SOURCE)
    assert "struct Objc3LoweringIRBoundary {" in header
    assert "kObjc3SelectorGlobalOrdering" in header
    assert "TryNormalizeObjc3LoweringContract(" in header
    assert "TryBuildObjc3LoweringIRBoundary(" in header
    assert "Objc3LoweringIRBoundaryReplayKey(" in header
    assert "TryGetCompoundAssignmentBinaryOpcode" in header
    assert "TryNormalizeObjc3LoweringContract(" in source
    assert "TryBuildObjc3LoweringIRBoundary(" in source
    assert "Objc3LoweringIRBoundaryReplayKey(" in source
    assert "runtime_dispatch_symbol=" in source
    assert "runtime_dispatch_arg_slots=" in source
    assert "selector_global_ordering=" in source
    assert '#include "lex/objc3_lexer.h"' not in header
    assert '#include "lex/objc3_lexer.h"' not in source
    assert '#include "parse/objc3_parser.h"' not in header
    assert '#include "parse/objc3_parser.h"' not in source
    assert '#include "parse/objc3_parser_contract.h"' not in header
    assert '#include "parse/objc3_parser_contract.h"' not in source
    assert '#include "token/objc3_token.h"' not in header
    assert '#include "token/objc3_token.h"' not in source


def test_lowering_ir_boundary_replay_key_is_canonical_and_replay_safe() -> None:
    source = _read(SOURCE)

    _assert_in_order(
        source,
        [
            "if (!TryNormalizeObjc3LoweringContract(input, normalized, error)) {",
            "boundary.runtime_dispatch_arg_slots = normalized.max_message_send_args;",
            "boundary.runtime_dispatch_symbol = normalized.runtime_dispatch_symbol;",
            "boundary.selector_global_ordering = kObjc3SelectorGlobalOrdering;",
        ],
    )

    _assert_in_order(
        source,
        [
            '"runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol',
            '";runtime_dispatch_arg_slots=" + std::to_string(boundary.runtime_dispatch_arg_slots)',
            '";selector_global_ordering=" + boundary.selector_global_ordering;',
        ],
    )

    assert "invalid lowering contract max_message_send_args:" in source
    assert "invalid lowering contract runtime_dispatch_symbol" in source


def test_cmake_links_lowering_contract_target() -> None:
    cmake = _read(CMAKE_FILE)
    assert "add_library(objc3c_lower STATIC" in cmake
    assert "src/lower/objc3_lowering_contract.cpp" in cmake
    assert "target_link_libraries(objc3c-native PRIVATE" in cmake
    assert "objc3c_lower" in cmake
