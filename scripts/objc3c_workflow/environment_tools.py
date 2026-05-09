"""Host tool discovery for objc3c workflow actions."""

from __future__ import annotations

import shutil


PWSH = shutil.which("pwsh") or "pwsh"
NPX = shutil.which("npx.cmd") or shutil.which("npx") or "npx"


__all__ = ["NPX", "PWSH"]
