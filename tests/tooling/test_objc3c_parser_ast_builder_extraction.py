from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
AST_BUILDER_HEADER = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_ast_builder_contract.h"
AST_BUILDER_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_ast_builder_contract.cpp"
PIPELINE_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"
BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_ast_builder_contract_module_exists() -> None:
    assert AST_BUILDER_HEADER.exists()
    assert AST_BUILDER_SOURCE.exists()
    header = _read(AST_BUILDER_HEADER)
    source = _read(AST_BUILDER_SOURCE)

    assert "struct Objc3AstBuilderResult" in header
    assert '#include "parse/objc3_parser_contract.h"' in header
    assert '#include "ast/objc3_ast.h"' not in header
    assert "Objc3ParsedProgram program;" in header
    assert "std::vector<std::string> diagnostics;" in header
    assert "BuildObjc3AstFromTokens" in header
    assert '#include "parse/objc3_parser.h"' in source
    assert "ParseObjc3Program(tokens)" in source


def test_pipeline_consumes_ast_builder_contract() -> None:
    pipeline = _read(PIPELINE_SOURCE)
    assert '#include "parse/objc3_ast_builder_contract.h"' in pipeline
    assert '#include "parse/objc3_parser.h"' not in pipeline
    assert "BuildObjc3AstFromTokens(tokens)" in pipeline


def test_build_surfaces_register_ast_builder_contract() -> None:
    cmake = _read(CMAKE_FILE)
    build_script = _read(BUILD_SCRIPT)
    assert "src/parse/objc3_ast_builder_contract.cpp" in cmake
    assert '"native/objc3c/src/parse/objc3_ast_builder_contract.cpp"' in build_script
