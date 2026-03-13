#!/usr/bin/env python3
from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CHECKER = ROOT / "scripts" / "check_m267_b001_error_carrier_and_propagation_semantic_model_contract_and_architecture_freeze.py"


def main() -> int:
    completed = subprocess.run([sys.executable, str(CHECKER)], cwd=ROOT, check=False)
    return completed.returncode


if __name__ == "__main__":
    raise SystemExit(main())
