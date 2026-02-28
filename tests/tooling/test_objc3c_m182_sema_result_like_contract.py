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


M182_HEADING = "M182 sema/type result-like lowering contract (M182-B001)"
M182_ANCHOR = '<a id="m182-sema-type-result-like-lowering-contract-m182-b001"></a>'


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


def test_m182_sema_result_like_doc_section_and_anchor() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    _assert_regex(
        fragment,
        rf"{re.escape(M182_ANCHOR)}\s*\n## {re.escape(M182_HEADING)}",
    )

    section = _extract_h2_section(fragment, M182_HEADING)
    for marker in (
        "Objc3ResultLikeLoweringSummary",
        "BuildResultLikeLoweringSummaryFromProgramAst",
        "result_like_lowering_sites_total",
        "result_like_lowering_normalized_sites_total + result_like_lowering_branch_merge_sites_total == result_like_lowering_sites_total",
        "result_like_lowering_result_success_sites_total + result_like_lowering_result_failure_sites_total == result_like_lowering_normalized_sites_total",
        "deterministic_result_like_lowering_handoff",
        "python -m pytest tests/tooling/test_objc3c_m182_sema_result_like_contract.py -q",
    ):
        assert marker in section


def test_m182_sema_result_like_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3ResultLikeLoweringSummary {",
        "std::size_t result_like_sites = 0;",
        "std::size_t result_success_sites = 0;",
        "std::size_t result_failure_sites = 0;",
        "std::size_t branch_merge_sites = 0;",
        "Objc3ResultLikeLoweringSummary result_like_lowering_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "result_like_lowering_sites_total",
        "result_like_lowering_result_success_sites_total",
        "result_like_lowering_result_failure_sites_total",
        "result_like_lowering_branch_merge_sites_total",
        "deterministic_result_like_lowering_handoff",
        "surface.result_like_lowering_summary.result_like_sites ==",
    ):
        assert marker in sema_pass_manager_contract
    _assert_regex(
        sema_pass_manager_contract,
        r"result_like_lowering_summary\.normalized_sites\s*\+\s*"
        r"surface\.result_like_lowering_summary\.branch_merge_sites\s*==\s*"
        r"surface\.result_like_lowering_summary\.result_like_sites",
    )
    _assert_regex(
        sema_pass_manager_contract,
        r"result_like_lowering_summary\.result_success_sites\s*\+\s*"
        r"surface\.result_like_lowering_summary\.result_failure_sites\s*==\s*"
        r"surface\.result_like_lowering_summary\.normalized_sites",
    )

    for marker in (
        "IsEquivalentResultLikeLoweringSummary",
        "result.result_like_lowering_summary =",
        "result.deterministic_result_like_lowering_handoff =",
        "result.parity_surface.result_like_lowering_summary =",
        "result.parity_surface.result_like_lowering_sites_total =",
        "result.parity_surface.deterministic_result_like_lowering_handoff =",
    ):
        assert marker in sema_pass_manager
    _assert_regex(
        sema_pass_manager,
        r"type_metadata_handoff\.result_like_lowering_summary\s*"
        r"\.normalized_sites\s*\+\s*"
        r"result\.type_metadata_handoff\.result_like_lowering_summary\s*"
        r"\.branch_merge_sites\s*==\s*"
        r"result\.type_metadata_handoff\.result_like_lowering_summary\s*"
        r"\.result_like_sites",
    )
    _assert_regex(
        sema_pass_manager,
        r"type_metadata_handoff\.result_like_lowering_summary\s*"
        r"\.result_success_sites\s*\+\s*"
        r"result\.type_metadata_handoff\.result_like_lowering_summary\s*"
        r"\.result_failure_sites\s*==\s*"
        r"result\.type_metadata_handoff\.result_like_lowering_summary\s*"
        r"\.normalized_sites",
    )
    _assert_regex(
        sema_pass_manager,
        r"parity_surface\.result_like_lowering_summary\.normalized_sites\s*\+\s*"
        r"result\.parity_surface\.result_like_lowering_summary\s*"
        r"\.branch_merge_sites\s*==\s*"
        r"result\.parity_surface\.result_like_lowering_summary\s*"
        r"\.result_like_sites",
    )

    for marker in (
        "BuildResultLikeLoweringSummaryFromProgramAst(",
        "surface.result_like_lowering_summary =",
        "handoff.result_like_lowering_summary =",
        "handoff.result_like_lowering_summary.deterministic",
    ):
        assert marker in sema_passes
    _assert_regex(
        sema_passes,
        r"is_partitioned\(\s*summary\.normalized_sites,\s*"
        r"summary\.branch_merge_sites,\s*summary\.result_like_sites\s*\)",
    )
    _assert_regex(
        sema_passes,
        r"is_partitioned\(\s*summary\.result_success_sites,\s*"
        r"summary\.result_failure_sites,\s*summary\.normalized_sites\s*\)",
    )
    _assert_regex(
        sema_passes,
        r"handoff\.result_like_lowering_summary\.normalized_sites\s*\+\s*"
        r"handoff\.result_like_lowering_summary\.branch_merge_sites\s*==\s*"
        r"handoff\.result_like_lowering_summary\.result_like_sites",
    )
