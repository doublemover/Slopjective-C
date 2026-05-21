from __future__ import annotations

from typing import Any

from objc3c_editor_tooling.input_loading import CompileInvocationResult


def compile_summary_exit_code(result: CompileInvocationResult) -> int | None:
    if result.summary_available:
        return None
    return result.returncode if result.returncode != 0 else 1


def diagnostics_entries(diagnostics_payload: dict[str, Any]) -> list[Any]:
    entries = diagnostics_payload.get("diagnostics", [])
    return entries if isinstance(entries, list) else []


def source_graph_consumer_status(
    source_graph: dict[str, Any] | None,
    capability_id: str,
    default_unpublished_reason: str,
) -> dict[str, Any]:
    if not isinstance(source_graph, dict):
        return {
            "supported": False,
            "support_class": "fail-closed-unpublished",
            "evidence_ids": [],
            "fail_closed": True,
            "unpublished_reason": default_unpublished_reason,
        }
    capabilities = source_graph.get("consumer_capabilities", {})
    status = capabilities.get(capability_id, {}) if isinstance(capabilities, dict) else {}
    if not isinstance(status, dict):
        status = {}
    supported = status.get("supported") is True
    evidence_ids = status.get("evidence_ids", [])
    evidence_ids = evidence_ids if isinstance(evidence_ids, list) else []
    unpublished_reason = str(status.get("unpublished_reason", "") or "")
    return {
        "supported": supported,
        "support_class": str(
            status.get(
                "support_class",
                "source-graph-backed" if supported else "source-graph-fail-closed",
            )
            or ("source-graph-backed" if supported else "source-graph-fail-closed")
        ),
        "evidence_ids": [str(evidence_id) for evidence_id in evidence_ids] if supported else [],
        "fail_closed": not supported,
        "unpublished_reason": "" if supported else (unpublished_reason or default_unpublished_reason),
    }
