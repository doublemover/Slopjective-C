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


M190_HEADING = "M190 sema/type concurrency replay-proof and race-guard contract (M190-B001)"
M190_ANCHOR = '<a id="m190-sema-type-concurrency-replay-proof-race-guard-contract-m190-b001"></a>'


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


def test_m190_sema_concurrency_replay_doc_section_and_anchor() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    _assert_regex(
        fragment,
        rf"{re.escape(M190_ANCHOR)}\s*\n## {re.escape(M190_HEADING)}",
    )

    section = _extract_h2_section(fragment, M190_HEADING)
    for marker in (
        "Objc3ConcurrencyReplayRaceGuardSummary",
        "BuildConcurrencyReplayRaceGuardSummaryFromIntegrationSurface",
        "BuildConcurrencyReplayRaceGuardSummaryFromTypeMetadataHandoff",
        "concurrency_replay_race_guard_sites_total",
        "concurrency_replay_race_guard_concurrency_replay_sites_total",
        "concurrency_replay_race_guard_deterministic_schedule_sites_total + concurrency_replay_race_guard_guard_blocked_sites_total == concurrency_replay_race_guard_concurrency_replay_sites_total",
        "deterministic_concurrency_replay_race_guard_handoff",
        "python -m pytest tests/tooling/test_objc3c_m190_sema_concurrency_replay_race_guard_contract.py -q",
    ):
        assert marker in section


def test_m190_sema_concurrency_replay_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3ConcurrencyReplayRaceGuardSummary {",
        "std::size_t concurrency_replay_race_guard_sites = 0;",
        "std::size_t concurrency_replay_sites = 0;",
        "std::size_t replay_proof_sites = 0;",
        "std::size_t race_guard_sites = 0;",
        "std::size_t task_handoff_sites = 0;",
        "std::size_t actor_isolation_sites = 0;",
        "std::size_t deterministic_schedule_sites = 0;",
        "std::size_t guard_blocked_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "Objc3ConcurrencyReplayRaceGuardSummary concurrency_replay_race_guard_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "concurrency_replay_race_guard_sites_total",
        "concurrency_replay_race_guard_concurrency_replay_sites_total",
        "concurrency_replay_race_guard_replay_proof_sites_total",
        "concurrency_replay_race_guard_race_guard_sites_total",
        "concurrency_replay_race_guard_task_handoff_sites_total",
        "concurrency_replay_race_guard_actor_isolation_sites_total",
        "concurrency_replay_race_guard_deterministic_schedule_sites_total",
        "concurrency_replay_race_guard_guard_blocked_sites_total",
        "concurrency_replay_race_guard_contract_violation_sites_total",
        "deterministic_concurrency_replay_race_guard_handoff",
        "surface.concurrency_replay_race_guard_summary",
    ):
        assert marker in sema_pass_manager_contract

    _assert_regex(
        sema_pass_manager_contract,
        r"concurrency_replay_race_guard_summary\s*\.deterministic_schedule_sites\s*\+\s*"
        r"surface\.concurrency_replay_race_guard_summary\s*\.guard_blocked_sites\s*==\s*"
        r"surface\.concurrency_replay_race_guard_summary\s*\.concurrency_replay_sites",
    )

    for marker in (
        "IsEquivalentConcurrencyReplayRaceGuardSummary",
        "result.concurrency_replay_race_guard_summary =",
        "result.deterministic_concurrency_replay_race_guard_handoff =",
        "result.parity_surface.concurrency_replay_race_guard_summary =",
        "result.parity_surface.concurrency_replay_race_guard_sites_total =",
        "result.parity_surface.deterministic_concurrency_replay_race_guard_handoff =",
    ):
        assert marker in sema_pass_manager

    _assert_regex(
        sema_pass_manager,
        r"type_metadata_handoff\s*\.concurrency_replay_race_guard_summary\s*"
        r"\.deterministic_schedule_sites\s*\+\s*"
        r"result\.type_metadata_handoff\s*\.concurrency_replay_race_guard_summary\s*"
        r"\.guard_blocked_sites\s*==\s*"
        r"result\.type_metadata_handoff\s*\.concurrency_replay_race_guard_summary\s*"
        r"\.concurrency_replay_sites",
    )
    _assert_regex(
        sema_pass_manager,
        r"parity_surface\.concurrency_replay_race_guard_summary\s*"
        r"\.deterministic_schedule_sites\s*\+\s*"
        r"result\.parity_surface\.concurrency_replay_race_guard_summary\s*"
        r"\.guard_blocked_sites\s*==\s*"
        r"result\.parity_surface\.concurrency_replay_race_guard_summary\s*"
        r"\.concurrency_replay_sites",
    )

    for marker in (
        "BuildConcurrencyReplayRaceGuardSummaryFromIntegrationSurface(",
        "BuildConcurrencyReplayRaceGuardSummaryFromTypeMetadataHandoff(",
        "surface.concurrency_replay_race_guard_summary =",
        "handoff.concurrency_replay_race_guard_summary =",
        "handoff.concurrency_replay_race_guard_summary.deterministic",
    ):
        assert marker in sema_passes

    _assert_regex(
        sema_passes,
        r"summary\.deterministic_schedule_sites\s*\+\s*summary\.guard_blocked_sites\s*==\s*"
        r"summary\.concurrency_replay_sites",
    )
    _assert_regex(
        sema_passes,
        r"handoff\.concurrency_replay_race_guard_summary\s*\.deterministic_schedule_sites\s*\+\s*"
        r"handoff\.concurrency_replay_race_guard_summary\s*\.guard_blocked_sites\s*==\s*"
        r"handoff\.concurrency_replay_race_guard_summary\s*\.concurrency_replay_sites",
    )
