import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


M180_HEADING = "M180 sema/type cross-module conformance contract (M180-B001)"
M180_ANCHOR = '<a id="m180-sema-type-cross-module-conformance-contract-m180-b001"></a>'


def _extract_h2_section(doc: str, heading: str) -> str:
    match = re.search(
        rf"^## {re.escape(heading)}\n(?P<body>.*?)(?=^## |\Z)",
        doc,
        flags=re.MULTILINE | re.DOTALL,
    )
    assert match is not None, f"missing section heading: {heading}"
    return match.group(0)


def _assert_regex(text: str, pattern: str) -> None:
    assert re.search(pattern, text, flags=re.MULTILINE | re.DOTALL), pattern


def test_m180_sema_cross_module_conformance_doc_section_and_anchor() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    _assert_regex(
        fragment,
        rf"{re.escape(M180_ANCHOR)}\s*\n## {re.escape(M180_HEADING)}",
    )

    section = _extract_h2_section(fragment, M180_HEADING)
    for marker in (
        "Objc3CrossModuleConformanceSummary",
        "BuildCrossModuleConformanceSummaryFromIncrementalModuleCacheInvalidationSummary",
        "cross_module_conformance_sites_total",
        "cross_module_conformance_normalized_sites_total + cross_module_conformance_cache_invalidation_candidate_sites_total == cross_module_conformance_sites_total",
        "deterministic_cross_module_conformance_handoff",
        "python -m pytest tests/tooling/test_objc3c_m180_sema_cross_module_conformance_contract.py -q",
    ):
        assert marker in section


def test_m180_sema_cross_module_conformance_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3CrossModuleConformanceSummary {",
        "std::size_t cross_module_conformance_sites = 0;",
        "std::size_t cache_invalidation_candidate_sites = 0;",
        "Objc3CrossModuleConformanceSummary cross_module_conformance_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "cross_module_conformance_sites_total",
        "cross_module_conformance_cache_invalidation_candidate_sites_total",
        "deterministic_cross_module_conformance_handoff",
        "surface.cross_module_conformance_summary.cross_module_conformance_sites ==",
    ):
        assert marker in sema_pass_manager_contract
    _assert_regex(
        sema_pass_manager_contract,
        r"cross_module_conformance_summary\.normalized_sites\s*\+\s*"
        r"surface\.cross_module_conformance_summary\.cache_invalidation_candidate_sites\s*==\s*"
        r"surface\.cross_module_conformance_summary\.cross_module_conformance_sites",
    )

    for marker in (
        "IsEquivalentCrossModuleConformanceSummary",
        "result.cross_module_conformance_summary =",
        "result.deterministic_cross_module_conformance_handoff =",
        "result.parity_surface.cross_module_conformance_summary =",
        "result.parity_surface.cross_module_conformance_sites_total =",
        "result.parity_surface.deterministic_cross_module_conformance_handoff =",
    ):
        assert marker in sema_pass_manager
    _assert_regex(
        sema_pass_manager,
        r"type_metadata_handoff\.cross_module_conformance_summary\s*"
        r"\.normalized_sites\s*\+\s*"
        r"result\.type_metadata_handoff\.cross_module_conformance_summary\s*"
        r"\.cache_invalidation_candidate_sites\s*==\s*"
        r"result\.type_metadata_handoff\.cross_module_conformance_summary\s*"
        r"\.cross_module_conformance_sites",
    )
    _assert_regex(
        sema_pass_manager,
        r"parity_surface\.cross_module_conformance_summary\.normalized_sites\s*\+\s*"
        r"result\.parity_surface\.cross_module_conformance_summary\s*"
        r"\.cache_invalidation_candidate_sites\s*==\s*"
        r"result\.parity_surface\.cross_module_conformance_summary\s*"
        r"\.cross_module_conformance_sites",
    )

    for marker in (
        "BuildCrossModuleConformanceSummaryFromIncrementalModuleCacheInvalidationSummary(",
        "surface.cross_module_conformance_summary =",
        "handoff.cross_module_conformance_summary =",
        "handoff.cross_module_conformance_summary.deterministic",
    ):
        assert marker in sema_passes
    _assert_regex(
        sema_passes,
        r"summary\.normalized_sites\s*\+\s*summary\.cache_invalidation_candidate_sites\s*==\s*"
        r"summary\.cross_module_conformance_sites",
    )
    _assert_regex(
        sema_passes,
        r"handoff\.cross_module_conformance_summary\.normalized_sites\s*\+\s*"
        r"handoff\.cross_module_conformance_summary\.cache_invalidation_candidate_sites\s*==\s*"
        r"handoff\.cross_module_conformance_summary\.cross_module_conformance_sites",
    )
