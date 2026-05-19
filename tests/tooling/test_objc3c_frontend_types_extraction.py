from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TYPES_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
ARTIFACTS_HEADER = ROOT / "native" / "objc3c" / "src" / "artifacts" / "objc3_frontend_artifacts.h"
RETIRED_FRONTEND_TYPE_SHARD_DIR = TYPES_HEADER.with_name("objc3_frontend_types_" + "parts")
FRONTEND_TYPE_HEADERS = [
    ROOT / "native" / "objc3c" / "src" / "pipeline" / "results" / "compile_options.h",
    ROOT / "native" / "objc3c" / "src" / "lower" / "model" / "lowered_runtime_surface.h",
    ROOT / "native" / "objc3c" / "src" / "pipeline" / "results" / "phase_result.h",
    ROOT / "native" / "objc3c" / "src" / "sema" / "model" / "semantic_effect.h",
    ROOT / "native" / "objc3c" / "src" / "sema" / "model" / "semantic_program.h",
    ROOT / "native" / "objc3c" / "src" / "lower" / "model" / "lowered_module.h",
    ROOT / "native" / "objc3c" / "src" / "sema" / "model" / "semantic_symbol.h",
    ROOT / "native" / "objc3c" / "src" / "sema" / "model" / "semantic_ownership.h",
    ROOT / "native" / "objc3c" / "src" / "runtime" / "metadata" / "runtime_metadata_model.h",
    ROOT / "native" / "objc3c" / "src" / "artifacts" / "evidence" / "evidence_record.h",
    ROOT / "native" / "objc3c" / "src" / "sema" / "model" / "semantic_type.h",
    ROOT / "native" / "objc3c" / "src" / "runtime" / "metadata" / "class_metadata.h",
    ROOT / "native" / "objc3c" / "src" / "runtime" / "metadata" / "property_metadata.h",
    ROOT / "native" / "objc3c" / "src" / "runtime" / "metadata" / "selector_metadata.h",
    ROOT / "native" / "objc3c" / "src" / "artifacts" / "evidence" / "capability_status.h",
    ROOT / "native" / "objc3c" / "src" / "artifacts" / "reports" / "report_dto.h",
    ROOT / "native" / "objc3c" / "src" / "runtime" / "metadata" / "runtime_metadata_bootstrap.h",
    ROOT / "native" / "objc3c" / "src" / "pipeline" / "results" / "compile_result.h",
]


def _read(path: Path) -> str:
    text = path.read_text(encoding="utf-8")
    expanded: list[str] = []
    for line in text.splitlines():
        expanded.append(line)
        stripped = line.strip()
        if not stripped.startswith('#include "'):
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
    frontend_umbrella = _read(TYPES_HEADER)
    removed_include_fragment = "_parts" + "/"
    assert '#include "pipeline/results/compile_result.h"' in frontend_umbrella
    assert removed_include_fragment not in frontend_umbrella
    assert not RETIRED_FRONTEND_TYPE_SHARD_DIR.exists()

    for header in FRONTEND_TYPE_HEADERS:
        assert header.exists()
        assert len(header.read_text(encoding="utf-8").splitlines()) <= 1000

    types_header = "\n".join(_read(header) for header in FRONTEND_TYPE_HEADERS)
    assert '#include "parse/objc3_diagnostics_bus.h"' in types_header
    assert '#include "parse/objc3_parser_contract.h"' in types_header
    assert '#include "ast/objc3_ast.h"' not in types_header
    assert '#include "config/objc3_language_profile.h"' in types_header
    assert "objc3c::config::kCanonicalLanguageVersion" in types_header
    assert "enum class Objc3FrontendLanguageProfile : std::uint8_t" in types_header
    assert "kCanonical = 0u," in types_header
    assert "kLegacy" not in types_header
    assert "std::uint8_t language_version = kObjc3DefaultLanguageVersion;" in types_header
    assert "Objc3FrontendLanguageProfile language_profile = Objc3FrontendLanguageProfile::kCanonical;" in types_header
    _assert_in_order(
        types_header,
        [
            "std::uint8_t language_version = kObjc3DefaultLanguageVersion;",
            "Objc3FrontendLanguageProfile language_profile = Objc3FrontendLanguageProfile::kCanonical;",
            "Objc3LoweringContract lowering;",
        ],
    )
    assert "Objc3ParsedProgram program;" in types_header
    assert "Objc3FrontendDiagnosticsBus stage_diagnostics;" in types_header
    artifacts_header = _read(ARTIFACTS_HEADER)
    assert '#include "pipeline/objc3_frontend_types.h"' in artifacts_header
    assert "struct FunctionInfo {" not in artifacts_header
    assert "struct Objc3FrontendPipelineResult {" not in artifacts_header
