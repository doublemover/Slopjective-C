import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"


M189_HEADING = "M189 sema/type task runtime interop and cancellation contract (M189-B001)"
M189_ANCHOR = '<a id="m189-sema-type-task-runtime-interop-cancellation-contract-m189-b001"></a>'


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


def test_m189_sema_task_runtime_cancellation_doc_section_and_anchor() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    _assert_regex(
        fragment,
        rf"{re.escape(M189_ANCHOR)}\s*\n## {re.escape(M189_HEADING)}",
    )

    section = _extract_h2_section(fragment, M189_HEADING)
    for marker in (
        "Objc3TaskRuntimeCancellationSummary",
        "BuildTaskRuntimeCancellationSummaryFromIntegrationSurface",
        "BuildTaskRuntimeCancellationSummaryFromTypeMetadataHandoff",
        "task_runtime_cancellation_sites_total",
        "task_runtime_cancellation_runtime_hook_sites_total",
        "task_runtime_cancellation_cancellation_check_sites_total",
        "task_runtime_cancellation_cancellation_handler_sites_total",
        "task_runtime_cancellation_suspension_point_sites_total",
        "task_runtime_cancellation_cancellation_propagation_sites_total",
        "task_runtime_cancellation_normalized_sites_total + task_runtime_cancellation_gate_blocked_sites_total == task_runtime_cancellation_sites_total",
        "task_runtime_cancellation_gate_blocked_sites_total <= task_runtime_cancellation_cancellation_propagation_sites_total",
        "deterministic_task_runtime_cancellation_handoff",
        "python -m pytest tests/tooling/test_objc3c_m189_sema_task_runtime_interop_cancellation_contract.py -q",
    ):
        assert marker in section


def test_m189_sema_task_runtime_cancellation_markers_map_to_sources() -> None:
    sema_contract = _read(SEMA_CONTRACT)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager = _read(SEMA_PASS_MANAGER)
    sema_passes = _read(SEMA_PASSES)

    for marker in (
        "struct Objc3TaskRuntimeCancellationSummary {",
        "std::size_t task_runtime_interop_sites = 0;",
        "std::size_t runtime_hook_sites = 0;",
        "std::size_t cancellation_check_sites = 0;",
        "std::size_t cancellation_handler_sites = 0;",
        "std::size_t suspension_point_sites = 0;",
        "std::size_t cancellation_propagation_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "std::size_t gate_blocked_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "Objc3TaskRuntimeCancellationSummary task_runtime_cancellation_summary;",
    ):
        assert marker in sema_contract

    for marker in (
        "task_runtime_cancellation_sites_total",
        "task_runtime_cancellation_runtime_hook_sites_total",
        "task_runtime_cancellation_cancellation_check_sites_total",
        "task_runtime_cancellation_cancellation_handler_sites_total",
        "task_runtime_cancellation_suspension_point_sites_total",
        "task_runtime_cancellation_cancellation_propagation_sites_total",
        "task_runtime_cancellation_normalized_sites_total",
        "task_runtime_cancellation_gate_blocked_sites_total",
        "task_runtime_cancellation_contract_violation_sites_total",
        "deterministic_task_runtime_cancellation_handoff",
        "surface.task_runtime_cancellation_summary",
    ):
        assert marker in sema_pass_manager_contract

    _assert_regex(
        sema_pass_manager_contract,
        r"task_runtime_cancellation_summary\s*\.normalized_sites\s*\+\s*"
        r"surface\.task_runtime_cancellation_summary\s*\.gate_blocked_sites\s*==\s*"
        r"surface\.task_runtime_cancellation_summary\s*\.task_runtime_interop_sites",
    )
    _assert_regex(
        sema_pass_manager_contract,
        r"task_runtime_cancellation_summary\s*\.gate_blocked_sites\s*<=\s*"
        r"surface\.task_runtime_cancellation_summary\s*\.cancellation_propagation_sites",
    )

    for marker in (
        "IsEquivalentTaskRuntimeCancellationSummary",
        "result.task_runtime_cancellation_summary =",
        "result.deterministic_task_runtime_cancellation_handoff =",
        "result.parity_surface.task_runtime_cancellation_summary =",
        "result.parity_surface.task_runtime_cancellation_sites_total =",
        "result.parity_surface.deterministic_task_runtime_cancellation_handoff =",
    ):
        assert marker in sema_pass_manager

    _assert_regex(
        sema_pass_manager,
        r"type_metadata_handoff\s*\.task_runtime_cancellation_summary\s*"
        r"\.normalized_sites\s*\+\s*"
        r"result\.type_metadata_handoff\s*\.task_runtime_cancellation_summary\s*"
        r"\.gate_blocked_sites\s*==\s*"
        r"result\.type_metadata_handoff\s*\.task_runtime_cancellation_summary\s*"
        r"\.task_runtime_interop_sites",
    )
    _assert_regex(
        sema_pass_manager,
        r"type_metadata_handoff\s*\.task_runtime_cancellation_summary\s*"
        r"\.gate_blocked_sites\s*<=\s*"
        r"result\.type_metadata_handoff\s*\.task_runtime_cancellation_summary\s*"
        r"\.cancellation_propagation_sites",
    )
    _assert_regex(
        sema_pass_manager,
        r"parity_surface\.task_runtime_cancellation_summary\s*"
        r"\.normalized_sites\s*\+\s*"
        r"result\.parity_surface\.task_runtime_cancellation_summary\s*"
        r"\.gate_blocked_sites\s*==\s*"
        r"result\.parity_surface\.task_runtime_cancellation_summary\s*"
        r"\.task_runtime_interop_sites",
    )
    _assert_regex(
        sema_pass_manager,
        r"parity_surface\.task_runtime_cancellation_summary\s*"
        r"\.gate_blocked_sites\s*<=\s*"
        r"result\.parity_surface\.task_runtime_cancellation_summary\s*"
        r"\.cancellation_propagation_sites",
    )

    for marker in (
        "BuildTaskRuntimeCancellationSummaryFromIntegrationSurface(",
        "BuildTaskRuntimeCancellationSummaryFromTypeMetadataHandoff(",
        "surface.task_runtime_cancellation_summary =",
        "handoff.task_runtime_cancellation_summary =",
        "handoff.task_runtime_cancellation_summary.deterministic",
    ):
        assert marker in sema_passes

    _assert_regex(
        sema_passes,
        r"summary\.normalized_sites\s*\+\s*summary\.gate_blocked_sites\s*==\s*"
        r"summary\.task_runtime_interop_sites",
    )
    _assert_regex(
        sema_passes,
        r"summary\.gate_blocked_sites\s*<=\s*summary\.cancellation_propagation_sites",
    )
    _assert_regex(
        sema_passes,
        r"handoff\.task_runtime_cancellation_summary\s*\.normalized_sites\s*\+\s*"
        r"handoff\.task_runtime_cancellation_summary\s*\.gate_blocked_sites\s*==\s*"
        r"handoff\.task_runtime_cancellation_summary\s*\.task_runtime_interop_sites",
    )
