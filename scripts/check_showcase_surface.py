#!/usr/bin/env python3
"""Validate the live showcase example surface through the public compiler path."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
PORTFOLIO = ROOT / "showcase" / "portfolio.json"


def fail(message: str) -> int:
    print(f"showcase-surface: FAIL\n- {message}", file=sys.stderr)
    return 1


def run(command: list[str]) -> int:
    result = subprocess.run(command, cwd=ROOT, check=False)
    return result.returncode


def main() -> int:
    if not PORTFOLIO.is_file():
        return fail(f"missing showcase portfolio: {PORTFOLIO}")

    payload = json.loads(PORTFOLIO.read_text(encoding="utf-8"))
    if payload.get("contract_id") != "objc3c.showcase.portfolio.surface.v1":
        return fail("unexpected contract_id")
    if payload.get("schema_version") != 1:
        return fail("unexpected schema_version")
    if payload.get("showcase_root") != "showcase":
        return fail("showcase_root drifted")
    if payload.get("machine_output_root") != "tmp/artifacts/showcase":
        return fail("machine_output_root drifted")

    entrypoints = payload.get("public_entrypoints")
    if entrypoints != {
        "build_native": "build:objc3c-native",
        "compile_example": "compile:objc3c",
        "check_surface": "check:showcase:surface",
        "package_runnable_toolchain": "package:objc3c-native:runnable-toolchain",
    }:
        return fail("public_entrypoints drifted")

    examples = payload.get("examples")
    if not isinstance(examples, list) or len(examples) != 3:
        return fail("examples inventory drifted")

    ids = [entry.get("id") for entry in examples if isinstance(entry, dict)]
    if ids != ["auroraBoard", "signalMesh", "patchKit"]:
        return fail("example ids drifted")

    if run([sys.executable, str(RUNNER), "build-native-binaries"]) != 0:
        return fail("build-native-binaries failed")

    machine_output_root = ROOT / "tmp" / "artifacts" / "showcase"
    machine_output_root.mkdir(parents=True, exist_ok=True)

    for entry in examples:
        if not isinstance(entry, dict):
            return fail("example entry must be an object")
        example_id = entry.get("id")
        source = entry.get("source")
        if not isinstance(example_id, str) or not isinstance(source, str):
            return fail("example entry missing id/source")
        source_path = ROOT / source
        if not source_path.is_file():
            return fail(f"missing showcase source: {source}")
        out_dir = machine_output_root / example_id
        command = [
            sys.executable,
            str(RUNNER),
            "compile-objc3c",
            source,
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
        ]
        if run(command) != 0:
            return fail(f"compile failed for showcase example {example_id}")

    print("showcase-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
