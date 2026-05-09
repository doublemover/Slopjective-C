"""Developer inspection and formatter handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import developer_tooling

DEVELOPER_INSPECTION_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "inspect-capability-explorer": developer_tooling.action_inspect_capability_explorer,
    "inspect-playground-repro": developer_tooling.action_inspect_playground_repro,
    "inspect-compile-observability": developer_tooling.action_inspect_compile_observability,
    "inspect-runtime-inspector": developer_tooling.action_inspect_runtime_inspector,
    "inspect-editor-tooling": developer_tooling.action_inspect_editor_tooling,
    "format-objc3c": developer_tooling.action_format_objc3c,
}
