"""Entrypoint selection for live and packaged platform-hardening checks."""

from __future__ import annotations

from platform_hardening_contracts import ROOT

from .live_workflow import run_live_integration
from .packaged_smoke import run_packaged_bundle_smoke


def main() -> int:
    if not (ROOT / "native" / "objc3c" / "src" / "main.cpp").is_file():
        return run_packaged_bundle_smoke()
    return run_live_integration()
