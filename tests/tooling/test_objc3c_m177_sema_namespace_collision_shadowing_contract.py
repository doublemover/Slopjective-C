from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m177_sema_namespace_collision_shadowing_section_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M177 sema/type namespace collision and shadowing diagnostics contract (M177-B001)",
        "Objc3NamespaceCollisionShadowingSummary",
        "BuildNamespaceCollisionShadowingSummaryFromModuleImportGraphSummary",
        "namespace_collision_shadowing_sites_total",
        "deterministic_namespace_collision_shadowing_handoff",
        "python -m pytest tests/tooling/test_objc3c_m177_sema_namespace_collision_shadowing_contract.py -q",
    ):
        assert text in fragment


def test_m177_sema_namespace_collision_shadowing_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3NamespaceCollisionShadowingSummary {",
        "std::size_t namespace_collision_shadowing_sites = 0;",
        "std::size_t namespace_segment_sites = 0;",
        "std::size_t import_edge_candidate_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "Objc3NamespaceCollisionShadowingSummary namespace_collision_shadowing_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "namespace_collision_shadowing_sites_total",
        "namespace_collision_shadowing_import_edge_candidate_sites_total",
        "namespace_collision_shadowing_contract_violation_sites_total",
        "deterministic_namespace_collision_shadowing_handoff",
        "surface.namespace_collision_shadowing_summary.namespace_collision_shadowing_sites ==",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "IsEquivalentNamespaceCollisionShadowingSummary",
        "result.namespace_collision_shadowing_summary =",
        "result.deterministic_namespace_collision_shadowing_handoff =",
        "result.parity_surface.namespace_collision_shadowing_summary =",
        "result.parity_surface.namespace_collision_shadowing_sites_total =",
        "result.parity_surface.deterministic_namespace_collision_shadowing_handoff =",
    ):
        assert marker in sema_pass_manager

    for marker in (
        "BuildNamespaceCollisionShadowingSummaryFromModuleImportGraphSummary(",
        "surface.namespace_collision_shadowing_summary =",
        "handoff.namespace_collision_shadowing_summary =",
        "handoff.namespace_collision_shadowing_summary.deterministic",
    ):
        assert marker in sema_passes
