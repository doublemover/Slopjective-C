from __future__ import annotations

import subprocess
import sys
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
CHECKER = (
    REPO_ROOT
    / "scripts"
    / "check_m315_c003_genuine_replay_artifact_regeneration_and_provenance_capture_core_feature_implementation.py"
)


def main() -> int:
    result = subprocess.run([sys.executable, str(CHECKER)], cwd=REPO_ROOT)
    if result.returncode != 0:
        return result.returncode
    print("M315-C003 lane-C readiness passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
