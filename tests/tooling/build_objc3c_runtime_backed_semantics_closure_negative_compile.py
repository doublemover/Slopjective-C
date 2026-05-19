from __future__ import annotations

from typing import Any


def assert_runtime_backed_semantics_token_and_negative_compile_expectations(
    summary: dict[str, Any],
) -> None:
    assert summary["required_ir_tokens"][
        "runtime_backed_semantics_closure = contract=objc3c.runtime.backed.semantics.closure.v1"
    ] is True
    assert summary["negative_compile"]["task_group_without_scope"][
        "expected_codes_present"
    ] is True
