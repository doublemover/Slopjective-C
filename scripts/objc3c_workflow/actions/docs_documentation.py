"""Documentation, native docs, site, and markdown workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from ..environment import MARKDOWN_GLOBS, NPX
from .docs_paths import (
    NATIVE_DOCS_SCRIPT,
    NATIVE_DOCS_PY,
    SITE_PY,
)

BUILD_NATIVE_DOCS_ACTION = "build-native-docs"
CHECK_NATIVE_DOCS_ACTION = "check-native-docs"
VALIDATE_UMBRELLA_READINESS_ACTION = "validate-umbrella-readiness"

BUILD_NATIVE_DOCS_SUMMARY = "build the generated native implementation docs"
CHECK_NATIVE_DOCS_SUMMARY = "check generated native implementation docs for drift"
VALIDATE_UMBRELLA_READINESS_SUMMARY = (
    "validate schema-backed umbrella capability readiness gates"
)

BUILD_NATIVE_DOCS_BACKEND = f"python:{NATIVE_DOCS_SCRIPT}"
CHECK_NATIVE_DOCS_BACKEND = f"{BUILD_NATIVE_DOCS_BACKEND} --check"
VALIDATE_UMBRELLA_READINESS_BACKEND = (
    "python:scripts/check_objc3c_umbrella_readiness.py --check"
)

NATIVE_DOCS_VALIDATION_TIER = "docs"
UMBRELLA_READINESS_VALIDATION_TIER = "docs"

NATIVE_DOCS_GUARANTEE_OWNER = (
    "generated native implementation documentation stays in sync with "
    "docs/objc3c-native/src inputs"
)
UMBRELLA_READINESS_GUARANTEE_OWNER = (
    "broad umbrella capability rows remain blocked until schema-backed "
    "readiness prerequisites, evidence, fixtures, docs, and negative "
    "boundaries are satisfied"
)

BUILD_SITE_COMMAND = (sys.executable, SITE_PY)
CHECK_SITE_COMMAND = (sys.executable, SITE_PY, "--check")
BUILD_NATIVE_DOCS_COMMAND = (sys.executable, NATIVE_DOCS_PY)
CHECK_NATIVE_DOCS_COMMAND = (sys.executable, NATIVE_DOCS_PY, "--check")
VALIDATE_UMBRELLA_READINESS_COMMAND = (
    sys.executable,
    "scripts/check_objc3c_umbrella_readiness.py",
    "--check",
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


def action_check_markdown(_: list[str]) -> int:
    return run([NPX, "prettier", "--check", *MARKDOWN_GLOBS])


def action_format_markdown(_: list[str]) -> int:
    return run([NPX, "prettier", "--write", *MARKDOWN_GLOBS])


def action_lint_markdown(_: list[str]) -> int:
    return run([NPX, "markdownlint-cli2", *MARKDOWN_GLOBS])


def action_validate_umbrella_readiness(_: list[str]) -> int:
    return run([str(part) for part in VALIDATE_UMBRELLA_READINESS_COMMAND])
