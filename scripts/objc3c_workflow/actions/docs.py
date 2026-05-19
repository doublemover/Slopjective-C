"""Documentation and public-command workflow action facade."""

from __future__ import annotations

from ..registry_views import actions_matching
from .docs_documentation import (
    action_build_native_docs,
    action_build_site,
    action_check_documentation_surface,
    action_check_markdown,
    action_check_native_docs,
    action_check_site,
    action_format_markdown,
    action_lint_markdown,
    action_validate_documentation_surface,
)
from .docs_paths import (
    DOC_ACTION_MARKERS,
    DOCUMENTATION_SURFACE_PY,
    NATIVE_DOCS_PY,
    PUBLIC_COMMAND_BUDGET_PY,
    PUBLIC_COMMAND_CONTRACT_PY,
    PUBLIC_COMMAND_SURFACE_PY,
    SITE_PY,
)
from .docs_public_commands import (
    action_build_public_command_contract,
    action_build_public_command_surface,
    action_check_public_command_budget,
    action_check_public_command_contract,
    action_check_public_command_surface,
)


def action_names() -> list[str]:
    return actions_matching(
        lambda action, spec: any(
            marker in action or marker in spec.summary
            for marker in DOC_ACTION_MARKERS
        )
    )


__all__ = [
    "DOC_ACTION_MARKERS",
    "DOCUMENTATION_SURFACE_PY",
    "NATIVE_DOCS_PY",
    "PUBLIC_COMMAND_BUDGET_PY",
    "PUBLIC_COMMAND_CONTRACT_PY",
    "PUBLIC_COMMAND_SURFACE_PY",
    "SITE_PY",
    "action_build_native_docs",
    "action_build_public_command_contract",
    "action_build_public_command_surface",
    "action_build_site",
    "action_check_documentation_surface",
    "action_check_markdown",
    "action_check_native_docs",
    "action_check_public_command_budget",
    "action_check_public_command_contract",
    "action_check_public_command_surface",
    "action_check_site",
    "action_format_markdown",
    "action_lint_markdown",
    "action_names",
    "action_validate_documentation_surface",
]
