from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

COMMANDS: tuple[tuple[str, list[str]], ...] = (
    (
        "ensure-fast-native-build",
        [sys.executable, str(ROOT / "scripts" / "ensure_objc3c_native_build.py"), "--mode", "fast"],
    ),
    (
        "build-native-docs",
        [sys.executable, str(ROOT / "scripts" / "build_objc3c_native_docs.py")],
    ),
    (
        "m267-c003-static",
        [sys.executable, str(ROOT / "scripts" / "check_m267_c003_result_and_bridging_artifact_replay_completion_core_feature_expansion.py"), "--skip-dynamic-probes"],
    ),
    (
        "m267-d002-static",
        [sys.executable, str(ROOT / "scripts" / "check_m267_d002_live_catch_bridge_and_runtime_integration_core_feature_implementation.py"), "--skip-dynamic-probes"],
    ),
    (
        "m267-d003-check",
        [sys.executable, str(ROOT / "scripts" / "check_m267_d003_cross_module_error_surface_preservation_hardening_edge_case_and_compatibility_completion.py")],
    ),
    (
        "m267-d003-pytest",
        [sys.executable, "-m", "pytest", str(ROOT / "tests" / "tooling" / "test_check_m267_d003_cross_module_error_surface_preservation_hardening_edge_case_and_compatibility_completion.py"), "-q"],
    ),
)


def main() -> int:
    for label, command in COMMANDS:
        completed = subprocess.run(command, cwd=ROOT, text=True)
        if completed.returncode != 0:
            print(f"[fail] {label}", file=sys.stderr)
            return completed.returncode
    print("[ok] M267-D003 lane-D readiness passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
