#!/usr/bin/env python3
"""Validate the live reader-facing documentation surface and machine-appendix boundary."""

from __future__ import annotations

import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]

README_PATH = ROOT / "README.md"
SITE_BODY_PATH = ROOT / "site" / "src" / "index.body.md"
SITE_POLICY_PATH = ROOT / "site" / "src" / "README.md"
NATIVE_OWNERSHIP_PATH = ROOT / "docs" / "objc3c-native" / "src" / "OWNERSHIP.md"
PUBLIC_COMMAND_SURFACE_PATH = ROOT / "docs" / "runbooks" / "objc3c_public_command_surface.md"


def require_token(text: str, token: str, *, path: Path, errors: list[str]) -> None:
    if token not in text:
        errors.append(f"{path.relative_to(ROOT).as_posix()}: missing required token {token!r}")


def main() -> int:
    errors: list[str] = []

    readme = README_PATH.read_text(encoding="utf-8")
    for token in (
        "## Start Here",
        "## Fresh Setup",
        "## First Working Session",
        "## Public Command Surface",
        "## Spec Structure",
        "published site",
        "docs/runbooks/objc3c_public_command_surface.md",
    ):
        require_token(readme, token, path=README_PATH, errors=errors)

    site_body = SITE_BODY_PATH.read_text(encoding="utf-8")
    for token in (
        "## At a Glance {#toc-status-scope-note}",
        "## Quick Routes {#toc-quick-routes}",
        "## Reader Promises {#toc-reader-promises}",
        "## Specification Map {#toc-front-matter}",
        "## Language Parts {#toc-parts}",
        "[README.md](../README.md)",
        "[docs/objc3c-native.md](../docs/objc3c-native.md)",
        "[legacy spec redirects](../docs/reference/legacy_spec_anchor_index.md#legacy-files)",
    ):
        require_token(site_body, token, path=SITE_BODY_PATH, errors=errors)

    site_policy = SITE_POLICY_PATH.read_text(encoding="utf-8")
    require_token(site_policy, "## Tone and Accessibility Rules", path=SITE_POLICY_PATH, errors=errors)

    native_ownership = NATIVE_OWNERSHIP_PATH.read_text(encoding="utf-8")
    require_token(
        native_ownership,
        "## Public Doc Style And Accessibility Rules",
        path=NATIVE_OWNERSHIP_PATH,
        errors=errors,
    )

    command_surface = PUBLIC_COMMAND_SURFACE_PATH.read_text(encoding="utf-8")
    for token in (
        "operator-facing appendix",
        "## Operator Notes",
        "Treat this file as a generated machine-facing appendix",
    ):
        require_token(
            command_surface,
            token,
            path=PUBLIC_COMMAND_SURFACE_PATH,
            errors=errors,
        )

    if errors:
        print("documentation-surface: FAIL", file=sys.stderr)
        for error in errors:
            print(f"- {error}", file=sys.stderr)
        return 1

    print("documentation-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
