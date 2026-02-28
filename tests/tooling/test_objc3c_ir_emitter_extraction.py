from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
IR_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PIPELINE_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
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


def test_ir_emitter_module_exists_and_pipeline_artifacts_use_api() -> None:
    assert IR_HEADER.exists()
    assert IR_SOURCE.exists()
    ir_header = _read(IR_HEADER)
    ir_source = _read(IR_SOURCE)
    assert '#include "parse/objc3_parser_contract.h"' in ir_header
    assert "TryBuildObjc3LoweringIRBoundary(" in ir_source
    assert "ValidateMessageSendArityContract(" in ir_source
    assert "; lowering_ir_boundary = " in ir_source
    assert "message send exceeds runtime dispatch arg slots" in ir_source
    assert "Objc3LoweringIRBoundaryReplayKey(" in ir_source
    assert '#include "ast/objc3_ast.h"' not in ir_header
    assert '#include "lex/objc3_lexer.h"' not in ir_header
    assert '#include "lex/objc3_lexer.h"' not in ir_source
    assert '#include "token/objc3_token.h"' not in ir_header
    assert '#include "token/objc3_token.h"' not in ir_source

    artifacts_cpp = _read(PIPELINE_ARTIFACTS_CPP)
    assert '#include "ir/objc3_ir_emitter.h"' in artifacts_cpp
    assert "class Objc3IREmitter {" not in artifacts_cpp
    assert "EmitObjc3IRText(pipeline_result.program, options.lowering, bundle.ir_text, ir_error)" in artifacts_cpp


def test_ir_emitter_validates_boundary_and_arity_before_emit() -> None:
    ir_source = _read(IR_SOURCE)

    _assert_in_order(
        ir_source,
        [
            "if (!TryBuildObjc3LoweringIRBoundary(lowering_contract, lowering_ir_boundary_, boundary_error_)) {",
            "if (!boundary_error_.empty()) {",
            "if (!ValidateMessageSendArityContract(error)) {",
            "std::ostringstream body;",
        ],
    )

    arity_contract = ir_source.split(
        "bool ValidateMessageSendArityContract(std::string &error) const {",
        1,
    )[1].split(
        "LoweredMessageSend LowerMessageSendExpr",
        1,
    )[0]

    _assert_in_order(
        arity_contract,
        [
            "for (const auto &global : program_.globals) {",
            "if (!ValidateMessageSendArityExpr(global.value.get(), error)) {",
            "for (const auto &fn : program_.functions) {",
            "if (!ValidateMessageSendArityStmt(stmt.get(), error)) {",
        ],
    )

    assert "lowered.args.assign(lowering_ir_boundary_.runtime_dispatch_arg_slots, \"0\");" in ir_source


def test_ir_emitter_prologue_pins_canonical_lowering_replay_comment() -> None:
    ir_source = _read(IR_SOURCE)
    _assert_in_order(
        ir_source,
        [
            'out << "; objc3c native frontend IR\\n";',
            'out << "; lowering_ir_boundary = " << Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_) << "\\n";',
            'out << "source_filename = \\"" << program_.module_name << ".objc3\\"\\n\\n";',
            'out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";',
            "for (std::size_t i = 0; i < lowering_ir_boundary_.runtime_dispatch_arg_slots; ++i) {",
        ],
    )


def test_cmake_registers_ir_target() -> None:
    cmake = _read(CMAKE_FILE)
    assert "add_library(objc3c_ir STATIC" in cmake
    assert "src/ir/objc3_ir_emitter.cpp" in cmake
    assert "target_link_libraries(objc3c_ir PUBLIC" in cmake
    assert "objc3c_lower" in cmake
