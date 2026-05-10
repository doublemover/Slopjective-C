from __future__ import annotations

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]

EXIT_BOOTSTRAPPABLE = 0
EXIT_BLOCKED = 1
EXIT_HARD_FAILURE = 2
