#!/usr/bin/env python3
"""Lane-E readiness runner for M267-E001."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

COMMANDS: tuple[tuple[str, list[str]], ...] = (
    ("ensure-fast-native-build", [sys.executable, str(ROOT / "scripts" / "ensure_objc3c_native_build.py"), "--mode", "fast"]),
    ("build-native-docs", [sys.executable, str(ROOT / "scripts" / "build_objc3c_native_docs.py")]),
    ("m267-a002-lane-a", [sys.executable, str(ROOT / "scripts" / "run_m267_a002_lane_a_readiness.py")]),
    ("m267-b003-static", [sys.executable, str(ROOT / "scripts" / "check_m267_b003_bridging_legality_and_diagnostic_completion_edge_case_and_compatibility_completion.py"), "--skip-dynamic-probes"]),
    ("m267-b003-pytest", [sys.executable, "-m", "pytest", str(ROOT / "tests" / "tooling" / "test_check_m267_b003_bridging_legality_and_diagnostic_completion_edge_case_and_compatibility_completion.py"), "-q"]),
    ("m267-c003-lane-c", [sys.executable, str(ROOT / "scripts" / "run_m267_c003_lane_c_readiness.py")]),
    ("m267-d003-lane-d", [sys.executable, str(ROOT / "scripts" / "run_m267_d003_lane_d_readiness.py")]),
    ("m267-e001-check", [sys.executable, str(ROOT / "scripts" / "check_m267_e001_error_model_conformance_gate_contract_and_architecture_freeze.py")]),
    ("m267-e001-pytest", [sys.executable, "-m", "pytest", str(ROOT / "tests" / "tooling" / "test_check_m267_e001_error_model_conformance_gate_contract_and_architecture_freeze.py"), "-q"]),
)


def main() -> int:
    for label, command in COMMANDS:
        completed = subprocess.run(command, cwd=ROOT, text=True)
        if completed.returncode != 0:
            print(f"[fail] {label}", file=sys.stderr)
            return completed.returncode
    print("[ok] M267-E001 lane-E readiness passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
