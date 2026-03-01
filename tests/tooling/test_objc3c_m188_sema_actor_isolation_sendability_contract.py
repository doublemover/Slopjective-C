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


M188_HEADING = "M188 sema/type actor isolation and sendability checks and diagnostics contract (M188-B001)"
M188_ANCHOR = '<a id="m188-sema-type-actor-isolation-sendability-checks-diagnostics-contract-m188-b001"></a>'


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


def test_m188_sema_actor_isolation_sendability_doc_section_and_anchor() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    _assert_regex(
        fragment,
        rf"{re.escape(M188_ANCHOR)}\s*\n## {re.escape(M188_HEADING)}",
    )

    section = _extract_h2_section(fragment, M188_HEADING)
    for marker in (
        "Objc3ActorIsolationSendabilitySummary",
        "BuildActorIsolationSendabilitySummaryFromIntegrationSurface",
        "BuildActorIsolationSendabilitySummaryFromTypeMetadataHandoff",
        "actor_isolation_sendability_sites_total",
        "non_sendable_crossing_sites_total",
        "actor_isolation_sendability_normalized_sites_total + actor_isolation_sendability_gate_blocked_sites_total == actor_isolation_sendability_sites_total",
        "actor_isolation_sendability_gate_blocked_sites_total <= non_sendable_crossing_sites_total",
        "deterministic_actor_isolation_sendability_handoff",
        "python -m pytest tests/tooling/test_objc3c_m188_sema_actor_isolation_sendability_contract.py -q",
    ):
        assert marker in section


def test_m188_sema_actor_isolation_sendability_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3ActorIsolationSendabilitySummary {",
        "std::size_t actor_isolation_sendability_sites = 0;",
        "std::size_t actor_isolation_decl_sites = 0;",
        "std::size_t actor_hop_sites = 0;",
        "std::size_t sendable_annotation_sites = 0;",
        "std::size_t non_sendable_crossing_sites = 0;",
        "std::size_t isolation_boundary_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "std::size_t gate_blocked_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "Objc3ActorIsolationSendabilitySummary actor_isolation_sendability_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "actor_isolation_sendability_sites_total",
        "actor_isolation_decl_sites_total",
        "actor_hop_sites_total",
        "sendable_annotation_sites_total",
        "non_sendable_crossing_sites_total",
        "actor_isolation_sendability_isolation_boundary_sites_total",
        "actor_isolation_sendability_normalized_sites_total",
        "actor_isolation_sendability_gate_blocked_sites_total",
        "actor_isolation_sendability_contract_violation_sites_total",
        "deterministic_actor_isolation_sendability_handoff",
        "surface.actor_isolation_sendability_summary",
    ):
        assert marker in sema_pass_manager_contract

    _assert_regex(
        sema_pass_manager_contract,
        r"actor_isolation_sendability_summary\s*\.normalized_sites\s*\+\s*"
        r"surface\.actor_isolation_sendability_summary\s*\.gate_blocked_sites\s*==\s*"
        r"surface\.actor_isolation_sendability_summary\s*\.actor_isolation_sendability_sites",
    )
    _assert_regex(
        sema_pass_manager_contract,
        r"actor_isolation_sendability_summary\s*\.gate_blocked_sites\s*<=\s*"
        r"surface\.actor_isolation_sendability_summary\s*\.non_sendable_crossing_sites",
    )

    for marker in (
        "IsEquivalentActorIsolationSendabilitySummary",
        "result.actor_isolation_sendability_summary =",
        "result.deterministic_actor_isolation_sendability_handoff =",
        "result.parity_surface.actor_isolation_sendability_summary =",
        "result.parity_surface.actor_isolation_sendability_sites_total =",
        "result.parity_surface.deterministic_actor_isolation_sendability_handoff =",
    ):
        assert marker in sema_pass_manager

    _assert_regex(
        sema_pass_manager,
        r"type_metadata_handoff\s*\.actor_isolation_sendability_summary\s*"
        r"\.normalized_sites\s*\+\s*"
        r"result\.type_metadata_handoff\s*\.actor_isolation_sendability_summary\s*"
        r"\.gate_blocked_sites\s*==\s*"
        r"result\.type_metadata_handoff\s*\.actor_isolation_sendability_summary\s*"
        r"\.actor_isolation_sendability_sites",
    )
    _assert_regex(
        sema_pass_manager,
        r"type_metadata_handoff\s*\.actor_isolation_sendability_summary\s*"
        r"\.gate_blocked_sites\s*<=\s*"
        r"result\.type_metadata_handoff\s*\.actor_isolation_sendability_summary\s*"
        r"\.non_sendable_crossing_sites",
    )
    _assert_regex(
        sema_pass_manager,
        r"parity_surface\.actor_isolation_sendability_summary\s*"
        r"\.normalized_sites\s*\+\s*"
        r"result\.parity_surface\.actor_isolation_sendability_summary\s*"
        r"\.gate_blocked_sites\s*==\s*"
        r"result\.parity_surface\.actor_isolation_sendability_summary\s*"
        r"\.actor_isolation_sendability_sites",
    )
    _assert_regex(
        sema_pass_manager,
        r"parity_surface\.actor_isolation_sendability_summary\s*"
        r"\.gate_blocked_sites\s*<=\s*"
        r"result\.parity_surface\.actor_isolation_sendability_summary\s*"
        r"\.non_sendable_crossing_sites",
    )

    for marker in (
        "BuildActorIsolationSendabilitySummaryFromIntegrationSurface(",
        "BuildActorIsolationSendabilitySummaryFromTypeMetadataHandoff(",
        "surface.actor_isolation_sendability_summary =",
        "handoff.actor_isolation_sendability_summary =",
        "handoff.actor_isolation_sendability_summary.deterministic",
    ):
        assert marker in sema_passes

    _assert_regex(
        sema_passes,
        r"summary\.normalized_sites\s*\+\s*summary\.gate_blocked_sites\s*==\s*"
        r"summary\.actor_isolation_sendability_sites",
    )
    _assert_regex(
        sema_passes,
        r"summary\.gate_blocked_sites\s*<=\s*summary\.non_sendable_crossing_sites",
    )
    _assert_regex(
        sema_passes,
        r"handoff\.actor_isolation_sendability_summary\s*\.normalized_sites\s*\+\s*"
        r"handoff\.actor_isolation_sendability_summary\s*\.gate_blocked_sites\s*==\s*"
        r"handoff\.actor_isolation_sendability_summary\s*\.actor_isolation_sendability_sites",
    )
