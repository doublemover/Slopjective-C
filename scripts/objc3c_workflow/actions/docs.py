"""Documentation and public-command workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from ..environment import MARKDOWN_GLOBS, NPX, ROOT
from ..registry_views import actions_matching

DOC_ACTION_MARKERS = (
    "docs",
    "documentation",
    "markdown",
    "site",
    "command-surface",
    "command-contract",
)

SITE_PY = ROOT / "scripts" / "build_site_index.py"
NATIVE_DOCS_PY = ROOT / "scripts" / "build_objc3c_native_docs.py"
PUBLIC_COMMAND_SURFACE_PY = ROOT / "scripts" / "render_objc3c_public_command_surface.py"
PUBLIC_COMMAND_CONTRACT_PY = ROOT / "scripts" / "build_objc3c_public_command_contract.py"
PUBLIC_COMMAND_BUDGET_PY = ROOT / "scripts" / "check_objc3c_public_command_budget.py"
DOCUMENTATION_SURFACE_PY = ROOT / "scripts" / "check_documentation_surface.py"


def action_names() -> list[str]:
    return actions_matching(
        lambda action, spec: any(
            marker in action or marker in spec.summary
            for marker in DOC_ACTION_MARKERS
        )
    )


def action_build_site(_: list[str]) -> int:
    rc = run([sys.executable, str(SITE_PY)])
    if rc != 0:
        return rc
    return run([NPX, "prettier", "--write", "site/index.md"])


def action_check_site(_: list[str]) -> int:
    return run([sys.executable, str(SITE_PY), "--check"])


def action_build_native_docs(_: list[str]) -> int:
    return run([sys.executable, str(NATIVE_DOCS_PY)])


def action_check_native_docs(_: list[str]) -> int:
    return run([sys.executable, str(NATIVE_DOCS_PY), "--check"])


def action_build_public_command_surface(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_COMMAND_SURFACE_PY)])


def action_check_public_command_surface(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_COMMAND_SURFACE_PY), "--check"])


def action_build_public_command_contract(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_COMMAND_CONTRACT_PY)])


def action_check_public_command_contract(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_COMMAND_CONTRACT_PY), "--check"])


def action_check_public_command_budget(_: list[str]) -> int:
    return run([sys.executable, str(PUBLIC_COMMAND_BUDGET_PY)])


def action_check_documentation_surface(_: list[str]) -> int:
    return run([sys.executable, str(DOCUMENTATION_SURFACE_PY)])


def action_check_markdown(_: list[str]) -> int:
    return run([NPX, "prettier", "--check", *MARKDOWN_GLOBS])


def action_format_markdown(_: list[str]) -> int:
    return run([NPX, "prettier", "--write", *MARKDOWN_GLOBS])


def action_lint_markdown(_: list[str]) -> int:
    return run([NPX, "markdownlint-cli2", *MARKDOWN_GLOBS])


def action_validate_documentation_surface(_: list[str]) -> int:
    commands = [
        [sys.executable, str(SITE_PY)],
        [sys.executable, str(NATIVE_DOCS_PY)],
        [sys.executable, str(PUBLIC_COMMAND_SURFACE_PY)],
        [sys.executable, str(SITE_PY), "--check"],
        [sys.executable, str(NATIVE_DOCS_PY), "--check"],
        [sys.executable, str(PUBLIC_COMMAND_SURFACE_PY), "--check"],
        [sys.executable, str(DOCUMENTATION_SURFACE_PY)],
    ]
    for command in commands:
        rc = run(command)
        if rc != 0:
            return rc
    return 0
