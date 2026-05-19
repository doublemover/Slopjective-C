#!/usr/bin/env python3
"""Materialize the canonical application workspace from showcase and stdlib sources."""

from __future__ import annotations

import argparse
from pathlib import Path

from objc3c_application_materialization.copy_materialization import (
    materialize_canonical_workspace,
    resolve_output_dir,
)
from objc3c_application_materialization.inputs import load_canonical_application_inputs
from objc3c_application_materialization.manifests import (
    canonical_workspace_manifest_payload,
    canonical_workspace_summary_payload,
)
from objc3c_application_materialization.result_rendering import (
    canonical_workspace_readme_lines,
    print_canonical_workspace_result,
    write_lines,
    write_payload,
)


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "application_architecture_testing" / "canonical_application_architecture_semantics.json"
PORTFOLIO_PATH = ROOT / "showcase" / "portfolio.json"
STDLIB_PROGRAM_SURFACE_PATH = ROOT / "stdlib" / "program_surface.json"
DEFAULT_OUTPUT_ROOT = ROOT / "tmp" / "artifacts" / "application-architecture-testing" / "canonical-workspace"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "application-architecture-testing" / "canonical-application-workspace-summary.json"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--out-dir",
        default="",
        help="Target output directory. Defaults to tmp/artifacts/application-architecture-testing/canonical-workspace.",
    )
    return parser.parse_args()


def resolve_out_dir(raw_out_dir: str) -> Path:
    return resolve_output_dir(
        root=ROOT, raw_out_dir=raw_out_dir, default_output_root=DEFAULT_OUTPUT_ROOT
    )


def main() -> int:
    args = parse_args()
    inputs = load_canonical_application_inputs(
        contract_path=CONTRACT_PATH,
        portfolio_path=PORTFOLIO_PATH,
        stdlib_program_surface_path=STDLIB_PROGRAM_SURFACE_PATH,
    )
    output_root = resolve_out_dir(args.out_dir)
    materialization = materialize_canonical_workspace(
        root=ROOT,
        output_root=output_root,
        portfolio=inputs.portfolio,
        stdlib_program_surface_path=STDLIB_PROGRAM_SURFACE_PATH,
    )
    write_lines(materialization.readme, canonical_workspace_readme_lines())

    write_payload(
        materialization.workspace_manifest,
        canonical_workspace_manifest_payload(
            root=ROOT,
            contract=inputs.contract,
            contract_path=CONTRACT_PATH,
            portfolio_path=PORTFOLIO_PATH,
            stdlib_program_surface_path=STDLIB_PROGRAM_SURFACE_PATH,
            materialization=materialization,
        ),
    )
    write_payload(
        SUMMARY_PATH,
        canonical_workspace_summary_payload(
            root=ROOT,
            contract=inputs.contract,
            contract_path=CONTRACT_PATH,
            stdlib_program_surface=inputs.stdlib_program_surface,
            materialization=materialization,
        ),
    )
    print_canonical_workspace_result(
        root=ROOT,
        output_root=materialization.output_root,
        workspace_manifest=materialization.workspace_manifest,
        summary_path=SUMMARY_PATH,
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
