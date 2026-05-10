from __future__ import annotations

from typing import Any

from objc3c_cross_module_semantic_contracts_diagnostics.catalog import CONTRACT_ID


def find_model(node: Any) -> dict[str, Any] | None:
    if isinstance(node, dict):
        if node.get("contract_id") == CONTRACT_ID:
            return node
        for value in node.values():
            found = find_model(value)
            if found is not None:
                return found
    elif isinstance(node, list):
        for value in node:
            found = find_model(value)
            if found is not None:
                return found
    return None


def diagnostic_matches(diagnostics: list[dict[str, Any]], code: str, line: int, column: int) -> bool:
    return any(
        diag.get("code") == code
        and int(diag.get("line", -1)) == line
        and int(diag.get("column", -1)) == column
        for diag in diagnostics
    )
