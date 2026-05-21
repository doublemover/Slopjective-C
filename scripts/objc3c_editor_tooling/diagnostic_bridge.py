from __future__ import annotations

from typing import Any


LSP_SEVERITY_BY_OBJC3C_SEVERITY = {
    "fatal": 1,
    "error": 1,
    "warning": 2,
    "note": 3,
    "info": 3,
    "hint": 4,
}


def _positive_int(value: object, default: int) -> int:
    if isinstance(value, int) and value > 0:
        return value
    return default


def _compiler_position(payload: object, *, default_line: int = 1, default_column: int = 1) -> dict[str, int]:
    if not isinstance(payload, dict):
        return {"line": default_line, "column": default_column}
    return {
        "line": _positive_int(payload.get("line"), default_line),
        "column": _positive_int(payload.get("column"), default_column),
    }


def _diagnostic_compiler_range(diagnostic: dict[str, Any]) -> dict[str, dict[str, int]]:
    span = diagnostic.get("span")
    if isinstance(span, dict):
        start = _compiler_position(span.get("start"))
        end = _compiler_position(
            span.get("end"),
            default_line=start["line"],
            default_column=start["column"],
        )
        return {"start": start, "end": end}

    line = _positive_int(diagnostic.get("line"), 1)
    column = _positive_int(diagnostic.get("column"), 1)
    return {
        "start": {"line": line, "column": column},
        "end": {"line": line, "column": column + 1},
    }


def _lsp_position(position: dict[str, int]) -> dict[str, int]:
    return {
        "line": max(position["line"] - 1, 0),
        "character": max(position["column"] - 1, 0),
    }


def _lsp_range(compiler_range: dict[str, dict[str, int]]) -> dict[str, dict[str, int]]:
    return {
        "start": _lsp_position(compiler_range["start"]),
        "end": _lsp_position(compiler_range["end"]),
    }


def _fixit_range(fixit: object) -> dict[str, dict[str, int]] | None:
    if not isinstance(fixit, dict):
        return None
    range_payload = fixit.get("range")
    if not isinstance(range_payload, dict):
        return None
    start = _compiler_position(range_payload.get("start"))
    end = _compiler_position(
        range_payload.get("end"),
        default_line=start["line"],
        default_column=start["column"],
    )
    if (end["line"], end["column"]) < (start["line"], start["column"]):
        return None
    return {"start": start, "end": end}


def _diagnostic_code(diagnostic: dict[str, Any]) -> str:
    return str(diagnostic.get("code", "") or "")


def lsp_diagnostic_record(source_path: str, diagnostic: dict[str, Any]) -> dict[str, Any]:
    severity = str(diagnostic.get("severity", "error") or "error")
    compiler_range = _diagnostic_compiler_range(diagnostic)
    record: dict[str, Any] = {
        "source": "objc3c",
        "code": _diagnostic_code(diagnostic),
        "severity": LSP_SEVERITY_BY_OBJC3C_SEVERITY.get(severity, 1),
        "severity_name": severity,
        "message": str(diagnostic.get("message", "") or ""),
        "range": _lsp_range(compiler_range),
        "compiler_range": compiler_range,
        "source_path": source_path,
        "phase": str(diagnostic.get("phase", "") or ""),
        "category": str(diagnostic.get("category", "") or ""),
    }
    explanation = diagnostic.get("explanation")
    if isinstance(explanation, str) and explanation:
        record["explanation"] = explanation
    recovery = diagnostic.get("recovery")
    if isinstance(recovery, dict):
        record["data"] = {"objc3c_recovery": recovery}
    return record


def code_actions_for_diagnostic(source_path: str, diagnostic: dict[str, Any]) -> list[dict[str, Any]]:
    code = _diagnostic_code(diagnostic)
    phase = str(diagnostic.get("phase", "") or "")
    actions: list[dict[str, Any]] = []
    fixits = diagnostic.get("fixits", [])
    if not isinstance(fixits, list):
        return actions

    valid_fixits = [
        (fixit, _fixit_range(fixit))
        for fixit in fixits
        if isinstance(fixit, dict) and isinstance(fixit.get("replacement"), str)
    ]
    valid_fixits = [(fixit, compiler_range) for fixit, compiler_range in valid_fixits if compiler_range is not None]
    for index, (fixit, compiler_range) in enumerate(valid_fixits):
        replacement = str(fixit["replacement"])
        title = str(fixit.get("title") or f"Apply objc3c fix-it for {code}")
        actions.append(
            {
                "title": title,
                "kind": "quickfix",
                "isPreferred": len(valid_fixits) == 1,
                "diagnostic_code": code,
                "diagnostic_phase": phase,
                "edit": {
                    "changes": {
                        source_path: [
                            {
                                "range": _lsp_range(compiler_range),
                                "compiler_range": compiler_range,
                                "newText": replacement,
                            }
                        ]
                    }
                },
                "data": {
                    "objc3c_fixit_index": index,
                    "machine_applicable": True,
                    "source": "objc3c-diagnostics-json",
                },
            }
        )
    return actions


def recovery_boundary_record(diagnostic: dict[str, Any]) -> dict[str, Any] | None:
    recovery = diagnostic.get("recovery")
    if not isinstance(recovery, dict):
        return None
    return {
        "diagnostic_code": _diagnostic_code(diagnostic),
        "phase": str(diagnostic.get("phase", "") or ""),
        "category": str(diagnostic.get("category", "") or ""),
        "strategy": str(recovery.get("strategy", "") or ""),
        "boundary": str(recovery.get("boundary", "") or ""),
        "deterministic": recovery.get("deterministic") is True,
        "accepts_invalid_program": recovery.get("accepts_invalid_program") is True,
        "recovery_counts_as_success": recovery.get("recovery_counts_as_success") is True,
        "native_fixture": str(recovery.get("native_fixture", "") or ""),
        "expected_native_code": str(recovery.get("expected_native_code", "") or ""),
    }


def severity_counts(diagnostics: list[dict[str, Any]]) -> dict[str, int]:
    counts: dict[str, int] = {}
    for diagnostic in diagnostics:
        severity = str(diagnostic.get("severity", "unknown") or "unknown")
        counts[severity] = counts.get(severity, 0) + 1
    return dict(sorted(counts.items()))


def build_lsp_diagnostic_transport(
    source_path: str,
    diagnostics: list[Any],
) -> dict[str, Any]:
    structured_diagnostics = [
        diagnostic for diagnostic in diagnostics if isinstance(diagnostic, dict)
    ]
    lsp_diagnostics = [
        lsp_diagnostic_record(source_path, diagnostic)
        for diagnostic in structured_diagnostics
    ]
    code_actions = [
        action
        for diagnostic in structured_diagnostics
        for action in code_actions_for_diagnostic(source_path, diagnostic)
    ]
    recovery_boundaries = [
        recovery
        for diagnostic in structured_diagnostics
        for recovery in [recovery_boundary_record(diagnostic)]
        if recovery is not None
    ]
    return {
        "contract_id": "objc3c.developer.tooling.lsp.diagnostic.transport.v1",
        "source_path": source_path,
        "publish_method": "textDocument/publishDiagnostics",
        "diagnostic_count": len(lsp_diagnostics),
        "severity_counts": severity_counts(structured_diagnostics),
        "diagnostics": lsp_diagnostics,
        "machine_applicable_fixit_count": len(code_actions),
        "code_action_count": len(code_actions),
        "code_actions": code_actions,
        "recovery_boundary_count": len(recovery_boundaries),
        "recovery_boundaries": recovery_boundaries,
        "recovery_acceptance_policy": "recovery metadata is diagnostic context only and never turns an invalid program into a success",
    }
