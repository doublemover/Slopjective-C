"""Developer inspection and formatter handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import (
    developer_tooling_dump_actions,
    developer_tooling_llvm_explorer,
    developer_tooling_playground,
    migration_workflow,
)

DEVELOPER_INSPECTION_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "inspect-capability-explorer": developer_tooling_llvm_explorer.action_inspect_capability_explorer,
    "inspect-playground-repro": developer_tooling_playground.action_inspect_playground_repro,
    "inspect-compile-observability": developer_tooling_dump_actions.action_inspect_compile_observability,
    "inspect-runtime-inspector": developer_tooling_dump_actions.action_inspect_runtime_inspector,
    "inspect-editor-tooling": developer_tooling_playground.action_inspect_editor_tooling,
    "format-objc3c": developer_tooling_playground.action_format_objc3c,
    "rewrite-objc3c-source": developer_tooling_playground.action_rewrite_objc3c_source,
    "analyze-migration-source": migration_workflow.action_analyze_migration_source,
    "rewrite-migration-source": migration_workflow.action_rewrite_migration_source,
    "validate-migration-workflow": migration_workflow.action_validate_migration_workflow,
    "check-developer-diagnostic-quality": developer_tooling_playground.action_check_developer_diagnostic_quality,
}
