#!/usr/bin/env python3
"""Materialize the checked-in stdlib workspace into a machine-owned artifact root."""

from __future__ import annotations

import argparse
from pathlib import Path

from objc3c_application_materialization.copy_materialization import (
    materialize_stdlib_workspace,
    resolve_stdlib_output_dir,
)
from objc3c_application_materialization.inputs import load_stdlib_workspace_inputs
from objc3c_application_materialization.manifests import stdlib_workspace_summary_payload
from objc3c_application_materialization.result_rendering import (
    print_stdlib_workspace_result,
    write_payload,
)


ROOT = Path(__file__).resolve().parents[1]
WORKSPACE_PATH = ROOT / "stdlib" / "workspace.json"
DEFAULT_OUTPUT_ROOT = ROOT / "tmp" / "artifacts" / "stdlib" / "workspace"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Copy the checked-in stdlib workspace to a machine-owned artifact root."
    )
    parser.add_argument(
        "--out-dir",
        default="",
        help="Target output directory. Defaults to tmp/artifacts/stdlib/workspace/<timestamp_pid>.",
    )
    return parser.parse_args()

def resolve_out_dir(raw_out_dir: str) -> Path:
    return resolve_stdlib_output_dir(
        root=ROOT, raw_out_dir=raw_out_dir, default_output_root=DEFAULT_OUTPUT_ROOT
    )


def main() -> int:
    inputs = load_stdlib_workspace_inputs(root=ROOT, workspace_path=WORKSPACE_PATH)
    output_root = resolve_out_dir(parse_args().out_dir)
    materialization = materialize_stdlib_workspace(
        root=ROOT, output_root=output_root, inventory=inputs.inventory
    )
    write_payload(
        materialization.summary_path,
        stdlib_workspace_summary_payload(
            root=ROOT,
            workspace_path=WORKSPACE_PATH,
            workspace=inputs.workspace,
            materialization=materialization,
        ),
    )
    print_stdlib_workspace_result(
        root=ROOT,
        output_root=materialization.output_root,
        summary_path=materialization.summary_path,
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
