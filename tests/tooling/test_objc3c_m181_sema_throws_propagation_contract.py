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


M181_HEADING = "M181 sema/type throws propagation contract (M181-B001)"
M181_ANCHOR = '<a id="m181-sema-type-throws-propagation-contract-m181-b001"></a>'


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


def test_m181_sema_throws_propagation_doc_section_and_anchor() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    _assert_regex(
        fragment,
        rf"{re.escape(M181_ANCHOR)}\s*\n## {re.escape(M181_HEADING)}",
    )

    section = _extract_h2_section(fragment, M181_HEADING)
    for marker in (
        "Objc3ThrowsPropagationSummary",
        "BuildThrowsPropagationSummaryFromCrossModuleConformanceSummary",
        "throws_propagation_sites_total",
        "throws_propagation_normalized_sites_total + throws_propagation_cache_invalidation_candidate_sites_total == throws_propagation_sites_total",
        "deterministic_throws_propagation_handoff",
        "python -m pytest tests/tooling/test_objc3c_m181_sema_throws_propagation_contract.py -q",
    ):
        assert marker in section


def test_m181_sema_throws_propagation_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3ThrowsPropagationSummary {",
        "std::size_t throws_propagation_sites = 0;",
        "std::size_t cache_invalidation_candidate_sites = 0;",
        "Objc3ThrowsPropagationSummary throws_propagation_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "throws_propagation_sites_total",
        "throws_propagation_cache_invalidation_candidate_sites_total",
        "deterministic_throws_propagation_handoff",
        "surface.throws_propagation_summary.throws_propagation_sites ==",
    ):
        assert marker in sema_pass_manager_contract
    _assert_regex(
        sema_pass_manager_contract,
        r"throws_propagation_summary\.normalized_sites\s*\+\s*"
        r"surface\.throws_propagation_summary\.cache_invalidation_candidate_sites\s*==\s*"
        r"surface\.throws_propagation_summary\.throws_propagation_sites",
    )

    for marker in (
        "IsEquivalentThrowsPropagationSummary",
        "result.throws_propagation_summary =",
        "result.deterministic_throws_propagation_handoff =",
        "result.parity_surface.throws_propagation_summary =",
        "result.parity_surface.throws_propagation_sites_total =",
        "result.parity_surface.deterministic_throws_propagation_handoff =",
    ):
        assert marker in sema_pass_manager
    _assert_regex(
        sema_pass_manager,
        r"type_metadata_handoff\.throws_propagation_summary\s*"
        r"\.normalized_sites\s*\+\s*"
        r"result\.type_metadata_handoff\.throws_propagation_summary\s*"
        r"\.cache_invalidation_candidate_sites\s*==\s*"
        r"result\.type_metadata_handoff\.throws_propagation_summary\s*"
        r"\.throws_propagation_sites",
    )
    _assert_regex(
        sema_pass_manager,
        r"parity_surface\.throws_propagation_summary\.normalized_sites\s*\+\s*"
        r"result\.parity_surface\.throws_propagation_summary\s*"
        r"\.cache_invalidation_candidate_sites\s*==\s*"
        r"result\.parity_surface\.throws_propagation_summary\s*"
        r"\.throws_propagation_sites",
    )

    for marker in (
        "BuildThrowsPropagationSummaryFromCrossModuleConformanceSummary(",
        "surface.throws_propagation_summary =",
        "handoff.throws_propagation_summary =",
        "handoff.throws_propagation_summary.deterministic",
    ):
        assert marker in sema_passes
    _assert_regex(
        sema_passes,
        r"summary\.normalized_sites\s*\+\s*summary\.cache_invalidation_candidate_sites\s*==\s*"
        r"summary\.throws_propagation_sites",
    )
    _assert_regex(
        sema_passes,
        r"handoff\.throws_propagation_summary\.normalized_sites\s*\+\s*"
        r"handoff\.throws_propagation_summary\.cache_invalidation_candidate_sites\s*==\s*"
        r"handoff\.throws_propagation_summary\.throws_propagation_sites",
    )
