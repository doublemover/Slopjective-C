"""Documentation, native docs, site, and markdown workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from ..environment import MARKDOWN_GLOBS, NPX
from .docs_paths import (
    DOCUMENTATION_SURFACE_PY,
    NATIVE_DOCS_PY,
    PUBLIC_COMMAND_SURFACE_PY,
    SITE_PY,
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
