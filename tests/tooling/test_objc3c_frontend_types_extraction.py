from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TYPES_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
ARTIFACTS_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.h"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_frontend_types_header_is_used_by_pipeline_artifacts() -> None:
    assert TYPES_HEADER.exists()
    types_header = _read(TYPES_HEADER)
    assert '#include "parse/objc3_diagnostics_bus.h"' in types_header
    assert '#include "parse/objc3_parser_contract.h"' in types_header
    assert '#include "ast/objc3_ast.h"' not in types_header
    assert "Objc3ParsedProgram program;" in types_header
    assert "Objc3FrontendDiagnosticsBus stage_diagnostics;" in types_header
    artifacts_header = _read(ARTIFACTS_HEADER)
    assert '#include "pipeline/objc3_frontend_types.h"' in artifacts_header
    assert "struct FunctionInfo {" not in artifacts_header
    assert "struct Objc3FrontendPipelineResult {" not in artifacts_header
