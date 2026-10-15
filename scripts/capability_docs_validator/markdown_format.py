"""Canonical formatting for the generated support Markdown projections."""

from __future__ import annotations

import subprocess
from pathlib import Path

from objc3c_tooling.paths import ROOT
from .errors import CapabilityDocsError


def format_support_docs(documents: dict[Path, str]) -> dict[Path, str]:
    prettier = ROOT / "node_modules/prettier/bin/prettier.cjs"
    if not prettier.is_file():
        raise CapabilityDocsError("repository Markdown formatter is missing; run npm ci")
    formatted = {}
    for path, source in documents.items():
        try:
            result = subprocess.run(
                ["node", str(prettier), "--stdin-filepath", str(path)],
                input=source, text=True, encoding="utf-8", capture_output=True, cwd=ROOT,
                timeout=30, check=False,
            )
        except (OSError, subprocess.TimeoutExpired) as error:
            raise CapabilityDocsError(f"support Markdown formatting failed: {error}") from error
        if result.returncode:
            raise CapabilityDocsError(f"support Markdown formatting failed: {result.stderr.strip()}")
        formatted[path] = result.stdout
    return formatted
