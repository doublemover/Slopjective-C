from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TYPES_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
ARTIFACTS_HEADER = ROOT / "native" / "objc3c" / "src" / "artifacts" / "objc3_frontend_artifacts.h"


def _read(path: Path) -> str:
    text = path.read_text(encoding="utf-8")
    expanded: list[str] = []
    for line in text.splitlines():
        expanded.append(line)
        stripped = line.strip()
        if not stripped.startswith('#include "') or "_parts/" not in stripped:
            continue
        include_path = stripped.split('"', 2)[1]
        target = ROOT / "native" / "objc3c" / "src" / include_path
        if target.exists():
            expanded.append(target.read_text(encoding="utf-8"))
    return "\n".join(expanded)


def _assert_in_order(text: str, snippets: list[str]) -> None:
    cursor = -1
    for snippet in snippets:
        index = text.find(snippet)
        assert index != -1, f"missing snippet: {snippet}"
        assert index > cursor, f"snippet out of order: {snippet}"
        cursor = index


def test_frontend_types_header_is_used_by_pipeline_artifacts() -> None:
    assert TYPES_HEADER.exists()
    types_header = _read(TYPES_HEADER)
    assert '#include "parse/objc3_diagnostics_bus.h"' in types_header
    assert '#include "parse/objc3_parser_contract.h"' in types_header
    assert '#include "ast/objc3_ast.h"' not in types_header
    assert '#include "config/objc3_language_profile.h"' in types_header
    assert "objc3c::config::kCanonicalLanguageVersion" in types_header
    assert "enum class Objc3FrontendLanguageProfile : std::uint8_t" in types_header
    assert "kCanonical = 0u," in types_header
    assert "kLegacy = 1u," in types_header
    assert "std::uint8_t language_version = kObjc3DefaultLanguageVersion;" in types_header
    assert "Objc3FrontendLanguageProfile language_profile = Objc3FrontendLanguageProfile::kCanonical;" in types_header
    assert "bool legacy_literal_diagnostics = false;" in types_header
    _assert_in_order(
        types_header,
        [
            "std::uint8_t language_version = kObjc3DefaultLanguageVersion;",
            "Objc3FrontendLanguageProfile language_profile = Objc3FrontendLanguageProfile::kCanonical;",
            "bool legacy_literal_diagnostics = false;",
            "Objc3LoweringContract lowering;",
        ],
    )
    assert "Objc3ParsedProgram program;" in types_header
    assert "Objc3FrontendDiagnosticsBus stage_diagnostics;" in types_header
    artifacts_header = _read(ARTIFACTS_HEADER)
    assert '#include "pipeline/objc3_frontend_types.h"' in artifacts_header
    assert "struct FunctionInfo {" not in artifacts_header
    assert "struct Objc3FrontendPipelineResult {" not in artifacts_header
