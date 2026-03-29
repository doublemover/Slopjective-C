#!/usr/bin/env python3
"""Validate the live showcase example surface through the public compiler path."""

from __future__ import annotations

import argparse
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


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Validate the live showcase example surface through the public compiler path."
    )
    parser.add_argument(
        "--example",
        action="append",
        default=[],
        help="Compile only the named showcase example id. Repeatable.",
    )
    parser.add_argument(
        "--capability",
        action="append",
        default=[],
        help="Compile only showcase examples advertising the named story capability. Repeatable.",
    )
    return parser.parse_args(argv)


def main() -> int:
    args = parse_args(sys.argv[1:])
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
    if payload.get("machine_report_root") != "tmp/reports/showcase":
        return fail("machine_report_root drifted")
    if payload.get("package_stage_root") != "tmp/pkg/objc3c-native-runnable-toolchain":
        return fail("package_stage_root drifted")

    entrypoints = payload.get("public_entrypoints")
    if entrypoints != {
        "build_native": "build:objc3c-native",
        "compile_example": "compile:objc3c",
        "check_surface": "check:showcase:surface",
        "package_runnable_toolchain": "package:objc3c-native:runnable-toolchain",
        "execution_smoke": "test:objc3c:execution-smoke",
        "execution_replay": "test:objc3c:execution-replay-proof",
    }:
        return fail("public_entrypoints drifted")

    build_run_package_surface = payload.get("build_run_package_surface")
    if build_run_package_surface != {
        "emit_prefix": "module",
        "workspace_layout": "checked-in showcase directories rooted at showcase/<example-id>",
        "artifact_root": "tmp/artifacts/showcase",
        "report_root": "tmp/reports/showcase",
        "package_stage_root": "tmp/pkg/objc3c-native-runnable-toolchain",
        "build_native_entrypoint": "build:objc3c-native",
        "compile_entrypoint": "compile:objc3c",
        "surface_check_entrypoint": "check:showcase:surface",
        "package_entrypoint": "package:objc3c-native:runnable-toolchain",
        "execution_smoke_entrypoint": "test:objc3c:execution-smoke",
        "execution_replay_entrypoint": "test:objc3c:execution-replay-proof",
    }:
        return fail("build_run_package_surface drifted")

    examples = payload.get("examples")
    if not isinstance(examples, list) or len(examples) != 3:
        return fail("examples inventory drifted")

    ids = [entry.get("id") for entry in examples if isinstance(entry, dict)]
    if ids != ["auroraBoard", "signalMesh", "patchKit"]:
        return fail("example ids drifted")

    requested_ids = set(args.example)
    if requested_ids:
        unknown_ids = sorted(requested_ids.difference(ids))
        if unknown_ids:
            return fail(f"unknown showcase example ids: {', '.join(unknown_ids)}")

    requested_capabilities = set(args.capability)
    known_capabilities = {
        capability
        for entry in examples
        if isinstance(entry, dict)
        for capability in entry.get("story_capabilities", [])
        if isinstance(capability, str)
    }
    if requested_capabilities:
        unknown_capabilities = sorted(requested_capabilities.difference(known_capabilities))
        if unknown_capabilities:
            return fail(
                f"unknown showcase capabilities: {', '.join(unknown_capabilities)}"
            )

    selected_examples: list[dict[str, object]] = []
    for entry in examples:
        if not isinstance(entry, dict):
            return fail("example entry must be an object")
        example_id = entry.get("id")
        if not isinstance(example_id, str):
            return fail("example entry missing id/source")
        capabilities = {
            capability
            for capability in entry.get("story_capabilities", [])
            if isinstance(capability, str)
        }
        if requested_ids and example_id not in requested_ids:
            continue
        if requested_capabilities and not (capabilities & requested_capabilities):
            continue
        selected_examples.append(entry)

    if not selected_examples:
        return fail("selection produced no showcase examples")

    if run([sys.executable, str(RUNNER), "build-native-binaries"]) != 0:
        return fail("build-native-binaries failed")

    machine_output_root = ROOT / "tmp" / "artifacts" / "showcase"
    machine_output_root.mkdir(parents=True, exist_ok=True)

    for entry in selected_examples:
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

    selected_ids = ", ".join(
        entry["id"] for entry in selected_examples if isinstance(entry.get("id"), str)
    )
    print(f"showcase-surface: OK ({selected_ids})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
