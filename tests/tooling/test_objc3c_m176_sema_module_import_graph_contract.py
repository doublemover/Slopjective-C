from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m176_sema_module_import_graph_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M176 sema/type module map ingestion and import graph contract (M176-B001)",
        "Objc3ModuleImportGraphSummary",
        "BuildModuleImportGraphSummaryFromTypeAnnotationAndGenericMetadataSummary",
        "module_import_graph_sites_total",
        "deterministic_module_import_graph_handoff",
        "python -m pytest tests/tooling/test_objc3c_m176_sema_module_import_graph_contract.py -q",
    ):
        assert text in fragment


def test_m176_sema_module_import_graph_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3ModuleImportGraphSummary {",
        "std::size_t module_import_graph_sites = 0;",
        "std::size_t import_edge_candidate_sites = 0;",
        "std::size_t namespace_segment_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "Objc3ModuleImportGraphSummary module_import_graph_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "module_import_graph_sites_total",
        "module_import_graph_import_edge_candidate_sites_total",
        "module_import_graph_contract_violation_sites_total",
        "deterministic_module_import_graph_handoff",
        "surface.module_import_graph_summary.module_import_graph_sites ==",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "IsEquivalentModuleImportGraphSummary",
        "result.module_import_graph_summary =",
        "result.deterministic_module_import_graph_handoff =",
        "result.parity_surface.module_import_graph_summary =",
        "result.parity_surface.module_import_graph_sites_total =",
        "result.parity_surface.deterministic_module_import_graph_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildModuleImportGraphSummaryFromTypeAnnotationAndGenericMetadataSummary(",
        "surface.module_import_graph_summary =",
        "handoff.module_import_graph_summary =",
        "handoff.module_import_graph_summary.deterministic",
    ):
        assert marker in sema_passes
