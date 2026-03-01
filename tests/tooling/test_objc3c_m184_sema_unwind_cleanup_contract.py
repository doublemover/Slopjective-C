import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


M184_HEADING = "M184 sema/type unwind cleanup contract (M184-B001)"
M184_ANCHOR = '<a id="m184-sema-type-unwind-cleanup-contract-m184-b001"></a>'


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


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


def test_m184_sema_unwind_cleanup_doc_section_and_anchor() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    _assert_regex(
        fragment,
        rf"{re.escape(M184_ANCHOR)}\s*\n## {re.escape(M184_HEADING)}",
    )

    section = _extract_h2_section(fragment, M184_HEADING)
    for marker in (
        "Objc3UnwindCleanupSummary",
        "BuildUnwindCleanupSummaryFromThrowsAndResultSummaries",
        "unwind_cleanup_sites_total",
        "unwind_cleanup_normalized_sites_total + unwind_cleanup_fail_closed_sites_total == unwind_cleanup_sites_total",
        "deterministic_unwind_cleanup_handoff",
        "python -m pytest tests/tooling/test_objc3c_m184_sema_unwind_cleanup_contract.py -q",
    ):
        assert marker in section


def test_m184_sema_unwind_cleanup_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3UnwindCleanupSummary {",
        "std::size_t unwind_cleanup_sites = 0;",
        "std::size_t exceptional_exit_sites = 0;",
        "std::size_t cleanup_action_sites = 0;",
        "std::size_t fail_closed_sites = 0;",
        "Objc3UnwindCleanupSummary unwind_cleanup_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "unwind_cleanup_sites_total",
        "unwind_cleanup_exceptional_exit_sites_total",
        "unwind_cleanup_action_sites_total",
        "unwind_cleanup_scope_sites_total",
        "unwind_cleanup_resume_sites_total",
        "unwind_cleanup_normalized_sites_total",
        "unwind_cleanup_fail_closed_sites_total",
        "unwind_cleanup_contract_violation_sites_total",
        "deterministic_unwind_cleanup_handoff",
        "surface.unwind_cleanup_summary.unwind_cleanup_sites ==",
    ):
        assert marker in sema_pass_manager_contract

    _assert_regex(
        sema_pass_manager_contract,
        r"surface\.unwind_cleanup_summary\.normalized_sites\s*\+\s*"
        r"surface\.unwind_cleanup_summary\.fail_closed_sites\s*==\s*"
        r"surface\.unwind_cleanup_summary\.unwind_cleanup_sites",
    )

    for marker in (
        "IsEquivalentUnwindCleanupSummary",
        "result.unwind_cleanup_summary =",
        "result.deterministic_unwind_cleanup_handoff =",
        "result.parity_surface.unwind_cleanup_summary =",
        "result.parity_surface.unwind_cleanup_sites_total =",
        "result.parity_surface.deterministic_unwind_cleanup_handoff =",
    ):
        assert marker in sema_pass_manager

    _assert_regex(
        sema_pass_manager,
        r"type_metadata_handoff\.unwind_cleanup_summary\.normalized_sites\s*\+\s*"
        r"result\.type_metadata_handoff\.unwind_cleanup_summary\s*"
        r"\.fail_closed_sites\s*==\s*"
        r"result\.type_metadata_handoff\.unwind_cleanup_summary\s*"
        r"\.unwind_cleanup_sites",
    )
    _assert_regex(
        sema_pass_manager,
        r"parity_surface\.unwind_cleanup_summary\.normalized_sites\s*\+\s*"
        r"result\.parity_surface\.unwind_cleanup_summary\s*"
        r"\.fail_closed_sites\s*==\s*"
        r"result\.parity_surface\.unwind_cleanup_summary\s*"
        r"\.unwind_cleanup_sites",
    )

    for marker in (
        "BuildUnwindCleanupSummaryFromThrowsAndResultSummaries(",
        "surface.unwind_cleanup_summary =",
        "handoff.unwind_cleanup_summary =",
        "handoff.unwind_cleanup_summary.deterministic",
    ):
        assert marker in sema_passes

    _assert_regex(
        sema_passes,
        r"summary\.normalized_sites\s*\+\s*summary\.fail_closed_sites\s*==\s*"
        r"summary\.unwind_cleanup_sites",
    )
    _assert_regex(
        sema_passes,
        r"handoff\.unwind_cleanup_summary\.normalized_sites\s*\+\s*"
        r"handoff\.unwind_cleanup_summary\.fail_closed_sites\s*==\s*"
        r"handoff\.unwind_cleanup_summary\.unwind_cleanup_sites",
    )
