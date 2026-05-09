"""Documentation, native docs, site, and markdown workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from ..environment import MARKDOWN_GLOBS, NPX
from .docs_paths import (
    DOCUMENTATION_SURFACE_SCRIPT,
    DOCUMENTATION_SURFACE_PY,
    NATIVE_DOCS_SCRIPT,
    NATIVE_DOCS_PY,
    PUBLIC_COMMAND_SURFACE_PY,
    SITE_PY,
)

BUILD_NATIVE_DOCS_ACTION = "build-native-docs"
CHECK_NATIVE_DOCS_ACTION = "check-native-docs"
CHECK_DOCUMENTATION_SURFACE_ACTION = "check-documentation-surface"
VALIDATE_DOCUMENTATION_SURFACE_ACTION = "validate-documentation-surface"

BUILD_NATIVE_DOCS_SUMMARY = "build the generated native implementation docs"
CHECK_NATIVE_DOCS_SUMMARY = "check generated native implementation docs for drift"
CHECK_DOCUMENTATION_SURFACE_SUMMARY = (
    "check the reader-facing documentation structure and machine-appendix boundary"
)
VALIDATE_DOCUMENTATION_SURFACE_SUMMARY = (
    "run the full documentation build and reader-surface validation flow"
)

BUILD_NATIVE_DOCS_BACKEND = f"python:{NATIVE_DOCS_SCRIPT}"
CHECK_NATIVE_DOCS_BACKEND = f"{BUILD_NATIVE_DOCS_BACKEND} --check"
CHECK_DOCUMENTATION_SURFACE_BACKEND = f"python:{DOCUMENTATION_SURFACE_SCRIPT}"
VALIDATE_DOCUMENTATION_SURFACE_BACKEND = (
    "runner-internal + generated documentation checks"
)

NATIVE_DOCS_VALIDATION_TIER = "docs"
DOCUMENTATION_SURFACE_VALIDATION_TIER = "docs"
DOCUMENTATION_VALIDATION_TIER = "docs"

NATIVE_DOCS_GUARANTEE_OWNER = (
    "generated native implementation documentation stays in sync with "
    "docs/objc3c-native/src inputs"
)
DOCUMENTATION_SURFACE_GUARANTEE_OWNER = (
    "reader-facing onboarding, site structure, and machine-appendix boundary "
    "stay accessible and explicit"
)
DOCUMENTATION_VALIDATION_GUARANTEE_OWNER = (
    "site output, native docs, command appendix, and reader-facing onboarding "
    "remain buildable, in sync, and explicit"
)

BUILD_SITE_COMMAND = (sys.executable, SITE_PY)
CHECK_SITE_COMMAND = (sys.executable, SITE_PY, "--check")
BUILD_NATIVE_DOCS_COMMAND = (sys.executable, NATIVE_DOCS_PY)
CHECK_NATIVE_DOCS_COMMAND = (sys.executable, NATIVE_DOCS_PY, "--check")
BUILD_PUBLIC_COMMAND_SURFACE_COMMAND = (sys.executable, PUBLIC_COMMAND_SURFACE_PY)
CHECK_PUBLIC_COMMAND_SURFACE_COMMAND = (
    sys.executable,
    PUBLIC_COMMAND_SURFACE_PY,
    "--check",
)
CHECK_DOCUMENTATION_SURFACE_COMMAND = (sys.executable, DOCUMENTATION_SURFACE_PY)
DOCUMENTATION_VALIDATION_COMMANDS = (
    BUILD_SITE_COMMAND,
    BUILD_NATIVE_DOCS_COMMAND,
    BUILD_PUBLIC_COMMAND_SURFACE_COMMAND,
    CHECK_SITE_COMMAND,
    CHECK_NATIVE_DOCS_COMMAND,
    CHECK_PUBLIC_COMMAND_SURFACE_COMMAND,
    CHECK_DOCUMENTATION_SURFACE_COMMAND,
)


def action_build_site(_: list[str]) -> int:
    rc = run([str(part) for part in BUILD_SITE_COMMAND])
    if rc != 0:
        return rc
    return run([NPX, "prettier", "--write", "site/index.md"])


def action_check_site(_: list[str]) -> int:
    return run([str(part) for part in CHECK_SITE_COMMAND])


def action_build_native_docs(_: list[str]) -> int:
    return run([str(part) for part in BUILD_NATIVE_DOCS_COMMAND])


def action_check_native_docs(_: list[str]) -> int:
    return run([str(part) for part in CHECK_NATIVE_DOCS_COMMAND])


def action_check_documentation_surface(_: list[str]) -> int:
    return run([str(part) for part in CHECK_DOCUMENTATION_SURFACE_COMMAND])


def action_check_markdown(_: list[str]) -> int:
    return run([NPX, "prettier", "--check", *MARKDOWN_GLOBS])


def action_format_markdown(_: list[str]) -> int:
    return run([NPX, "prettier", "--write", *MARKDOWN_GLOBS])


def action_lint_markdown(_: list[str]) -> int:
    return run([NPX, "markdownlint-cli2", *MARKDOWN_GLOBS])


def action_validate_documentation_surface(_: list[str]) -> int:
    for command in DOCUMENTATION_VALIDATION_COMMANDS:
        rc = run([str(part) for part in command])
        if rc != 0:
            return rc
    return 0
