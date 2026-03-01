import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


M186_HEADING = "M186 sema/type async continuation contract (M186-B001)"
M186_ANCHOR = '<a id="m186-sema-type-async-continuation-contract-m186-b001"></a>'


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


def test_m186_sema_async_continuation_doc_section_and_anchor() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    _assert_regex(
        fragment,
        rf"{re.escape(M186_ANCHOR)}\s*\n## {re.escape(M186_HEADING)}",
    )

    section = _extract_h2_section(fragment, M186_HEADING)
    for marker in (
        "Objc3AsyncContinuationSummary",
        "BuildAsyncContinuationSummaryFromIntegrationSurface",
        "BuildAsyncContinuationSummaryFromTypeMetadataHandoff",
        "async_continuation_sites_total",
        "async_continuation_normalized_sites_total + async_continuation_gate_blocked_sites_total == async_continuation_sites_total",
        "deterministic_async_continuation_handoff",
        "python -m pytest tests/tooling/test_objc3c_m186_sema_async_continuation_contract.py -q",
    ):
        assert marker in section


def test_m186_sema_async_continuation_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3AsyncContinuationSummary {",
        "std::size_t async_continuation_sites = 0;",
        "std::size_t async_keyword_sites = 0;",
        "std::size_t continuation_allocation_sites = 0;",
        "std::size_t continuation_resume_sites = 0;",
        "std::size_t continuation_suspend_sites = 0;",
        "std::size_t async_state_machine_sites = 0;",
        "Objc3AsyncContinuationSummary async_continuation_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "async_continuation_sites_total",
        "async_continuation_async_keyword_sites_total",
        "async_continuation_async_function_sites_total",
        "async_continuation_allocation_sites_total",
        "async_continuation_resume_sites_total",
        "async_continuation_suspend_sites_total",
        "async_continuation_state_machine_sites_total",
        "async_continuation_normalized_sites_total",
        "async_continuation_gate_blocked_sites_total",
        "async_continuation_contract_violation_sites_total",
        "deterministic_async_continuation_handoff",
        "surface.async_continuation_summary.async_continuation_sites ==",
    ):
        assert marker in sema_pass_manager_contract

    _assert_regex(
        sema_pass_manager_contract,
        r"surface\.async_continuation_summary\.normalized_sites\s*\+\s*"
        r"surface\.async_continuation_summary\.gate_blocked_sites\s*==\s*"
        r"surface\.async_continuation_summary\.async_continuation_sites",
    )

    for marker in (
        "IsEquivalentAsyncContinuationSummary",
        "result.async_continuation_summary =",
        "result.deterministic_async_continuation_handoff =",
        "result.parity_surface.async_continuation_summary =",
        "result.parity_surface.async_continuation_sites_total =",
        "result.parity_surface.deterministic_async_continuation_handoff =",
    ):
        assert marker in sema_pass_manager

    _assert_regex(
        sema_pass_manager,
        r"type_metadata_handoff\.async_continuation_summary\.normalized_sites\s*\+\s*"
        r"result\.type_metadata_handoff\.async_continuation_summary\s*"
        r"\.gate_blocked_sites\s*==\s*"
        r"result\.type_metadata_handoff\.async_continuation_summary\s*"
        r"\.async_continuation_sites",
    )
    _assert_regex(
        sema_pass_manager,
        r"parity_surface\.async_continuation_summary\.normalized_sites\s*\+\s*"
        r"result\.parity_surface\.async_continuation_summary\s*"
        r"\.gate_blocked_sites\s*==\s*"
        r"result\.parity_surface\.async_continuation_summary\s*"
        r"\.async_continuation_sites",
    )

    for marker in (
        "BuildAsyncContinuationSummaryFromIntegrationSurface(",
        "BuildAsyncContinuationSummaryFromTypeMetadataHandoff(",
        "surface.async_continuation_summary =",
        "handoff.async_continuation_summary =",
        "handoff.async_continuation_summary.deterministic",
    ):
        assert marker in sema_passes

    _assert_regex(
        sema_passes,
        r"summary\.normalized_sites\s*\+\s*summary\.gate_blocked_sites\s*==\s*"
        r"summary\.async_continuation_sites",
    )
    _assert_regex(
        sema_passes,
        r"handoff\.async_continuation_summary\.normalized_sites\s*\+\s*"
        r"handoff\.async_continuation_summary\.gate_blocked_sites\s*==\s*"
        r"handoff\.async_continuation_summary\.async_continuation_sites",
    )
