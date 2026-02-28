from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PARSER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser_contract.h"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
SEMA_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.h"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"
BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_parser_contract_exports_parsed_program_aliases() -> None:
    contract = _read(PARSER_CONTRACT)
    assert "struct Objc3ParsedProgram {" in contract
    assert "Objc3Program ast;" in contract
    assert "using Objc3ParsedGlobalDecl = GlobalDecl;" in contract
    assert "using Objc3ParsedFunctionDecl = FunctionDecl;" in contract
    assert "MutableObjc3ParsedProgramAst(" in contract
    assert "Objc3ParsedProgramAst(" in contract


def test_parser_uses_ast_builder_scaffold() -> None:
    parser = _read(PARSER_SOURCE)
    assert '#include "parse/objc3_ast_builder.h"' in parser
    assert "Objc3AstBuilder ast_builder_;" in parser
    assert "ast_builder_.BeginProgram()" in parser
    assert "ast_builder_.AddGlobalDecl(program, std::move(*decl));" in parser
    assert "ast_builder_.AddFunctionDecl(program, std::move(*fn));" in parser


def test_sema_header_consumes_parser_contract_outputs() -> None:
    sema = _read(SEMA_HEADER)
    assert '#include "parse/objc3_parser_contract.h"' in sema
    assert '#include "ast/objc3_ast.h"' not in sema
    assert "BuildSemanticIntegrationSurface(const Objc3ParsedProgram &program," in sema
    assert "ValidatePureContractSemanticDiagnostics(const Objc3ParsedProgram &program," in sema
    assert "ValidateSemanticBodies(const Objc3ParsedProgram &program, const Objc3SemanticIntegrationSurface &surface," in sema


def test_ast_builder_scaffold_is_registered_in_build_surfaces() -> None:
    cmake = _read(CMAKE_FILE)
    build_script = _read(BUILD_SCRIPT)
    assert "src/parse/objc3_ast_builder.cpp" in cmake
    assert '"native/objc3c/src/parse/objc3_ast_builder.cpp"' in build_script
