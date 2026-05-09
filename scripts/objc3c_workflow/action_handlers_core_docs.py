"""Core docs and public command handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import docs

CORE_DOCS_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "build-site": docs.action_build_site,
    "check-site": docs.action_check_site,
    "build-native-docs": docs.action_build_native_docs,
    "check-native-docs": docs.action_check_native_docs,
    "build-public-command-surface": docs.action_build_public_command_surface,
    "check-public-command-surface": docs.action_check_public_command_surface,
    "build-public-command-contract": docs.action_build_public_command_contract,
    "check-public-command-contract": docs.action_check_public_command_contract,
    "check-public-command-budget": docs.action_check_public_command_budget,
    "check-documentation-surface": docs.action_check_documentation_surface,
    "check-markdown": docs.action_check_markdown,
    "format-markdown": docs.action_format_markdown,
    "lint-markdown": docs.action_lint_markdown,
    "validate-documentation-surface": docs.action_validate_documentation_surface,
}
