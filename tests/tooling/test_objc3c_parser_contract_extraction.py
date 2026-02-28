from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PARSER_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser_contract.h"
PARSER_HEADER = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.h"
PIPELINE_TYPES_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
IR_HEADER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_parser_contract_surface_exists() -> None:
    contract = _read(PARSER_CONTRACT_HEADER)
    assert "struct Objc3ParsedProgram {" in contract
    assert "Objc3Program ast;" in contract
    assert "MutableObjc3ParsedProgramAst(" in contract
    assert "Objc3ParsedProgramAst(" in contract
    assert "using Objc3ParsedGlobalDecl = GlobalDecl;" in contract
    assert "using Objc3ParsedFunctionDecl = FunctionDecl;" in contract


def test_parser_pipeline_and_ir_use_parser_contract_boundary() -> None:
    parser_header = _read(PARSER_HEADER)
    pipeline_types = _read(PIPELINE_TYPES_HEADER)
    ir_header = _read(IR_HEADER)
    assert '#include "parse/objc3_parser_contract.h"' in parser_header
    assert "ParseObjc3Program(const Objc3LexTokenStream &tokens)" in parser_header
    assert '#include "parse/objc3_parser_contract.h"' in pipeline_types
    assert "Objc3ParsedProgram program;" in pipeline_types
    assert '#include "parse/objc3_parser_contract.h"' in ir_header
    assert '#include "ast/objc3_ast.h"' not in ir_header
