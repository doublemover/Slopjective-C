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


M187_HEADING = "M187 sema/type await lowering and suspension state contract (M187-B001)"
M187_ANCHOR = '<a id="m187-sema-type-await-lowering-suspension-state-contract-m187-b001"></a>'


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


def test_m187_sema_await_suspension_doc_section_and_anchor() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    _assert_regex(
        fragment,
        rf"{re.escape(M187_ANCHOR)}\s*\n## {re.escape(M187_HEADING)}",
    )

    section = _extract_h2_section(fragment, M187_HEADING)
    for marker in (
        "Objc3AwaitLoweringSuspensionStateSummary",
        "BuildAwaitLoweringSuspensionStateSummaryFromProgramAst",
        "await_lowering_suspension_state_lowering_sites_total",
        "await_lowering_suspension_state_lowering_normalized_sites_total + await_lowering_suspension_state_lowering_gate_blocked_sites_total == await_lowering_suspension_state_lowering_sites_total",
        "await_lowering_suspension_state_lowering_await_state_machine_sites_total <= await_lowering_suspension_state_lowering_await_suspension_point_sites_total",
        "deterministic_await_lowering_suspension_state_lowering_handoff",
        "python -m pytest tests/tooling/test_objc3c_m187_sema_await_suspension_contract.py -q",
    ):
        assert marker in section


def test_m187_sema_await_suspension_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3AwaitLoweringSuspensionStateSummary {",
        "std::size_t await_suspension_sites = 0;",
        "std::size_t await_suspension_point_sites = 0;",
        "std::size_t await_state_machine_sites = 0;",
        "Objc3AwaitLoweringSuspensionStateSummary await_lowering_suspension_state_lowering_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "await_lowering_suspension_state_lowering_sites_total",
        "await_lowering_suspension_state_lowering_await_keyword_sites_total",
        "await_lowering_suspension_state_lowering_await_suspension_point_sites_total",
        "await_lowering_suspension_state_lowering_await_resume_sites_total",
        "await_lowering_suspension_state_lowering_await_state_machine_sites_total",
        "await_lowering_suspension_state_lowering_await_continuation_sites_total",
        "await_lowering_suspension_state_lowering_normalized_sites_total",
        "await_lowering_suspension_state_lowering_gate_blocked_sites_total",
        "await_lowering_suspension_state_lowering_contract_violation_sites_total",
        "deterministic_await_lowering_suspension_state_lowering_handoff",
        "surface.await_lowering_suspension_state_lowering_summary",
    ):
        assert marker in sema_pass_manager_contract
    _assert_regex(
        sema_pass_manager_contract,
        r"surface\.await_lowering_suspension_state_lowering_summary\s*"
        r"\.await_suspension_sites\s*==",
    )
    _assert_regex(
        sema_pass_manager_contract,
        r"surface\.await_lowering_suspension_state_lowering_summary\s*\.normalized_sites\s*\+\s*"
        r"surface\.await_lowering_suspension_state_lowering_summary\s*\.gate_blocked_sites\s*==\s*"
        r"surface\.await_lowering_suspension_state_lowering_summary\s*\.await_suspension_sites",
    )
    _assert_regex(
        sema_pass_manager_contract,
        r"await_lowering_suspension_state_lowering_summary\s*\.await_state_machine_sites\s*<=\s*"
        r"surface\.await_lowering_suspension_state_lowering_summary\s*\.await_suspension_point_sites",
    )

    for marker in (
        "IsEquivalentAwaitLoweringSuspensionStateSummary",
        "result.await_lowering_suspension_state_lowering_summary =",
        "result.deterministic_await_lowering_suspension_state_lowering_handoff =",
        "result.parity_surface.await_lowering_suspension_state_lowering_summary =",
        "result.parity_surface.await_lowering_suspension_state_lowering_sites_total =",
        "deterministic_await_lowering_suspension_state_lowering_handoff =",
    ):
        assert marker in sema_pass_manager
    _assert_regex(
        sema_pass_manager,
        r"result\.type_metadata_handoff\s*\.await_lowering_suspension_state_lowering_summary\s*"
        r"\.normalized_sites\s*\+\s*"
        r"result\.type_metadata_handoff\s*\.await_lowering_suspension_state_lowering_summary\s*"
        r"\.gate_blocked_sites\s*==\s*"
        r"result\.type_metadata_handoff\s*\.await_lowering_suspension_state_lowering_summary\s*"
        r"\.await_suspension_sites",
    )
    _assert_regex(
        sema_pass_manager,
        r"result\.parity_surface\s*\.await_lowering_suspension_state_lowering_summary\s*"
        r"\.normalized_sites\s*\+\s*"
        r"result\.parity_surface\s*\.await_lowering_suspension_state_lowering_summary\s*"
        r"\.gate_blocked_sites\s*==\s*"
        r"result\.parity_surface\s*\.await_lowering_suspension_state_lowering_summary\s*"
        r"\.await_suspension_sites",
    )

    for marker in (
        "BuildAwaitLoweringSuspensionStateSummaryFromProgramAst(",
        "surface.await_lowering_suspension_state_lowering_summary =",
        "handoff.await_lowering_suspension_state_lowering_summary =",
        "handoff.await_lowering_suspension_state_lowering_summary.deterministic",
    ):
        assert marker in sema_passes
    _assert_regex(
        sema_passes,
        r"is_partitioned\(\s*declaration\.await_suspension_normalized_sites,\s*"
        r"declaration\.await_suspension_gate_blocked_sites,\s*declaration\.await_suspension_sites\s*\)",
    )
    _assert_regex(
        sema_passes,
        r"is_partitioned\(\s*summary\.normalized_sites,\s*"
        r"summary\.gate_blocked_sites,\s*summary\.await_suspension_sites\s*\)",
    )
    _assert_regex(
        sema_passes,
        r"handoff\.await_lowering_suspension_state_lowering_summary\s*\.normalized_sites\s*\+\s*"
        r"handoff\.await_lowering_suspension_state_lowering_summary\s*\.gate_blocked_sites\s*==\s*"
        r"handoff\.await_lowering_suspension_state_lowering_summary\s*\.await_suspension_sites",
    )
