"""Developer tooling, playground, and bonus-experience workflow action facade."""

from __future__ import annotations

import sys

from ..commands import run
from .developer_tooling_paths import (
    DEVELOPER_TOOLING_INTEGRATION_PY,
    RUNNABLE_DEVELOPER_TOOLING_E2E_PY,
)
from .developer_tooling_bonus import (
    action_inspect_bonus_tool_integration,
    action_materialize_project_template,
    action_validate_bonus_experiences,
    action_validate_runnable_bonus_experiences,
)
from .developer_tooling_dumps import (
    action_inspect_compile_observability,
    action_inspect_runtime_inspector,
    action_trace_compile_stages,
)
from .developer_tooling_llvm import (
    action_check_hosted_llvm_capabilities,
    action_check_llvm_capabilities,
    action_inspect_capability_explorer,
    action_test_capability_routed_source_parity,
)
from .developer_tooling_playground import (
    action_format_objc3c,
    action_inspect_editor_tooling,
    action_inspect_playground_repro,
    action_materialize_playground_workspace,
)


def action_validate_developer_tooling(_: list[str]) -> int:
    return run([sys.executable, str(DEVELOPER_TOOLING_INTEGRATION_PY)])


def action_validate_runnable_developer_tooling(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_DEVELOPER_TOOLING_E2E_PY)])
