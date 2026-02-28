from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
IR_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PIPELINE_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


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


def test_cmake_registers_ir_target() -> None:
    cmake = _read(CMAKE_FILE)
    assert "add_library(objc3c_ir STATIC" in cmake
    assert "src/ir/objc3_ir_emitter.cpp" in cmake
    assert "target_link_libraries(objc3c-native PRIVATE" in cmake
    assert "objc3c_ir" in cmake
