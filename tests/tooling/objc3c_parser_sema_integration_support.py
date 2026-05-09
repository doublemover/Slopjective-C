from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PARSER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser_contract.h"
PARSER_CORE_PRELUDE = (
    ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser_core_profile_prelude.inc"
)
PARSER_CORE_BODY = (
    ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser_core_blocks_and_expressions.inc"
)
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.h"
SEMA_HANDOFF_SCAFFOLD = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_parser_sema_handoff_scaffold.h"
PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
PIPELINE_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
PIPELINE_ORCHESTRATION_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "frontend_pipeline_orchestration.cpp"
PIPELINE_STAGE_RUNNER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "frontend_pipeline_stage_runner.cpp"
PIPELINE_SEMA_STAGE_RUNNER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "frontend_pipeline_sema_stage_runner.cpp"
ARTIFACTS_HEADER = ROOT / "native" / "objc3c" / "src" / "artifacts" / "objc3_frontend_artifacts.h"
ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "artifacts" / "objc3_frontend_artifacts.cpp"
DIAG_ARTIFACTS_HEADER = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_diagnostics_artifacts.h"
DRIVER_OBJC3_PATH = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
PARSE_CMAKE_FILE = ROOT / "native" / "objc3c" / "src" / "parse" / "CMakeLists.txt"
SEMA_CMAKE_FILE = ROOT / "native" / "objc3c" / "src" / "sema" / "CMakeLists.txt"
BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"


def read_source(path: Path, seen: set[Path] | None = None) -> str:
    if seen is None:
        seen = set()
    if path in seen:
        return ""
    seen.add(path)
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
            expanded.append(read_source(target, seen))
    return "\n".join(expanded)


def assert_contains_all(text: str, snippets: tuple[str, ...] | list[str]) -> None:
    for snippet in snippets:
        assert snippet in text, f"missing snippet: {snippet}"


def assert_in_order(text: str, snippets: list[str]) -> None:
    cursor = -1
    for snippet in snippets:
        index = text.find(snippet)
        assert index != -1, f"missing snippet: {snippet}"
        assert index > cursor, f"snippet out of order: {snippet}"
        cursor = index
