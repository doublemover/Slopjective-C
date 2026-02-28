from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TYPES_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
MAIN_CPP = ROOT / "native" / "objc3c" / "src" / "main.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_frontend_types_header_is_used_by_main() -> None:
    assert TYPES_HEADER.exists()
    main_cpp = _read(MAIN_CPP)
    assert '#include "pipeline/objc3_frontend_types.h"' in main_cpp
    assert "struct FunctionInfo {" not in main_cpp
    assert "struct Objc3FrontendPipelineResult {" not in main_cpp
