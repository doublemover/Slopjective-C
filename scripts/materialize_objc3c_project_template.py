#!/usr/bin/env python3
"""Materialize a machine-owned objc3c project template and demo harness."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Sequence

from objc3c_tooling.paths import display_path
from scripts.objc3c_workflow.public_command_api import public_workflow_command
from objc3c_tooling.subprocesses import run_timed
from objc3c_tooling.public_workflow_output import extract_line_value
from objc3c_application_materialization.copy_materialization import (
    materialize_project_template_source,
    project_template_paths,
)
from objc3c_application_materialization.inputs import (
    application_architecture_contract_paths,
)
from objc3c_application_materialization.manifests import (
    project_harness_payload,
    project_template_manifest_payload,
)
from objc3c_application_materialization.result_rendering import (
    print_project_template_result,
    project_template_readme_lines,
    write_lines,
    write_payload,
)


ROOT = Path(__file__).resolve().parents[1]
PORTFOLIO = ROOT / "showcase" / "portfolio.json"
TEMPLATE_ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "project-template"
TEMPLATE_REPORT_ROOT = ROOT / "tmp" / "reports" / "project-template"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--example", default="auroraBoard")
    return parser.parse_args(argv)

def read_json(path: Path) -> object:
    return json.loads(path.read_text(encoding="utf-8"))


def run_step(name: str, command: list[str]) -> dict[str, object]:
    completed = run_timed(command, cwd=ROOT, echo=True)
    return {
        "name": name,
        "command": command,
        "exit_code": completed.returncode,
        "duration_ms": completed.duration_ms,
        "stdout": completed.stdout,
    }


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    args = parse_args(sys.argv[1:])
    portfolio = read_json(PORTFOLIO)
    examples = portfolio.get("examples", [])
    example_record = next(
        (
            entry
            for entry in examples
            if isinstance(entry, dict) and entry.get("id") == args.example
        ),
        None,
    )
    if example_record is None:
        print(f"unknown showcase example: {args.example}", file=sys.stderr)
        return 2

    example_source = ROOT / str(example_record["source"])
    if not example_source.is_file():
        print(f"missing showcase source: {display_path(example_source)}", file=sys.stderr)
        return 1

    paths = project_template_paths(
        artifact_root=TEMPLATE_ARTIFACT_ROOT,
        report_root=TEMPLATE_REPORT_ROOT,
        example_id=args.example,
    )
    application_architecture_contracts = application_architecture_contract_paths(ROOT)

    materialize_project_template_source(example_source=example_source, paths=paths)
    write_lines(
        paths.template_readme,
        project_template_readme_lines(
            example_id=args.example,
            source_origin=example_record["source"],
            template_source=paths.template_source,
            root=ROOT,
        ),
    )
    write_payload(
        paths.template_manifest,
        project_template_manifest_payload(
            root=ROOT,
            example_id=args.example,
            example_record=example_record,
            paths=paths,
            application_architecture_contracts=application_architecture_contracts,
        ),
    )

    integration_step = run_step(
        "inspect-bonus-tool-integration",
        public_workflow_command("inspect-bonus-tool-integration"),
    )
    playground_step = run_step(
        "materialize-playground-workspace",
        public_workflow_command(
            "materialize-playground-workspace",
            display_path(paths.template_source, root=ROOT),
        ),
    )
    benchmark_step = run_step(
        "benchmark-runtime-inspector",
        public_workflow_command(
            "benchmark-runtime-inspector",
            display_path(paths.template_source, root=ROOT),
        ),
    )

    failures: list[str] = []
    for step in (integration_step, playground_step, benchmark_step):
        expect(step["exit_code"] == 0, f"{step['name']} failed", failures)

    integration_report = extract_line_value(str(integration_step["stdout"]), "summary_path:")
    playground_workspace = extract_line_value(str(playground_step["stdout"]), "workspace_path:")
    benchmark_report = extract_line_value(str(benchmark_step["stdout"]), "summary_path:")

    expect(integration_report != "", "inspect-bonus-tool-integration did not publish summary_path", failures)
    expect(playground_workspace != "", "materialize-playground-workspace did not publish workspace_path", failures)
    expect(benchmark_report != "", "benchmark-runtime-inspector did not publish summary_path", failures)

    write_payload(
        paths.harness_path,
        project_harness_payload(
            root=ROOT,
            failures=failures,
            paths=paths,
            integration_report=integration_report,
            playground_workspace=playground_workspace,
            benchmark_report=benchmark_report,
        ),
    )
    print_project_template_result(root=ROOT, paths=paths)
    if failures:
        print("project-template: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("project-template: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
