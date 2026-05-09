"""Core build, docs, hygiene, and command-surface action handlers."""

from __future__ import annotations

from scripts.objc3c_workflow.action_handlers_defaults import action_build_default
from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import (
    application_surfaces,
    developer_tooling,
    docs,
    hygiene,
    native_build,
)

CORE_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "build-default": action_build_default,
    "build-native-binaries": native_build.action_build_native_binaries,
    "build-native-contracts": native_build.action_build_native_contracts,
    "build-native-full": native_build.action_build_native_full,
    "build-native-reconfigure": native_build.action_build_native_reconfigure,
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
    "lint": hygiene.action_lint,
    "check-dependency-boundaries": hygiene.action_check_dependency_boundaries,
    "check-llvm-capabilities": developer_tooling.action_check_llvm_capabilities,
    "check-hosted-llvm-capabilities": developer_tooling.action_check_hosted_llvm_capabilities,
    "check-release-evidence": hygiene.action_check_release_evidence,
    "check-source-hygiene-authenticity": hygiene.action_check_source_hygiene_authenticity,
    "check-source-hygiene-hard-cutover": hygiene.action_check_source_hygiene_hard_cutover,
    "check-task-hygiene": hygiene.action_check_task_hygiene,
    "check-showcase-surface": application_surfaces.action_check_showcase_surface,
    "check-stdlib-surface": application_surfaces.action_check_stdlib_surface,
    "validate-showcase-runtime": application_surfaces.action_validate_showcase_runtime,
    "validate-showcase": application_surfaces.action_validate_showcase,
    "validate-runnable-showcase": application_surfaces.action_validate_runnable_showcase,
    "validate-getting-started": application_surfaces.action_validate_getting_started,
    "check-repo-superclean-surface": hygiene.action_check_repo_superclean_surface,
    "validate-documentation-surface": docs.action_validate_documentation_surface,
    "validate-repo-superclean": hygiene.action_validate_repo_superclean,
    "compile-objc3c": native_build.action_compile_objc3c,
}
