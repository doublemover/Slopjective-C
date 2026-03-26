from __future__ import annotations

import subprocess
import sys
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
CHECKER = (
    REPO_ROOT
    / "scripts"
    / "check_m315_b005_comment_constexpr_and_contract_string_decontamination_sweep_edge_case_and_compatibility_completion.py"
)


def main() -> int:
    result = subprocess.run([sys.executable, str(CHECKER)], cwd=REPO_ROOT)
    if result.returncode != 0:
        return result.returncode
    print("M315-B005 lane-B readiness passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
