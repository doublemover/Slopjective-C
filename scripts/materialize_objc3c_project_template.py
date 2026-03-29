#!/usr/bin/env python3
"""Materialize a machine-owned objc3c project template and demo harness."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
from pathlib import Path
from typing import Sequence


ROOT = Path(__file__).resolve().parents[1]
PORTFOLIO = ROOT / "showcase" / "portfolio.json"
PUBLIC_RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
TEMPLATE_ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "project-template"
TEMPLATE_REPORT_ROOT = ROOT / "tmp" / "reports" / "project-template"
TEMPLATE_CONTRACT_ID = "objc3c.project.template.surface.v1"
HARNESS_CONTRACT_ID = "objc3c.project.template.demo.harness.v1"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--example", default="auroraBoard")
    return parser.parse_args(argv)


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def read_json(path: Path) -> object:
    return json.loads(path.read_text(encoding="utf-8"))


def extract_line_value(stdout: str, prefix: str) -> str:
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if line.startswith(prefix):
            return line.split(":", 1)[1].strip()
    return ""


def run_step(name: str, command: list[str]) -> dict[str, object]:
    completed = subprocess.run(
        command,
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if completed.stdout:
        sys.stdout.write(completed.stdout)
    if completed.stderr:
        sys.stderr.write(completed.stderr)
    return {
        "name": name,
        "command": command,
        "exit_code": completed.returncode,
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

    template_root = TEMPLATE_ARTIFACT_ROOT / args.example
    report_root = TEMPLATE_REPORT_ROOT / args.example
    template_source = template_root / "src" / "main.objc3"
    template_readme = template_root / "README.md"
    template_manifest = template_root / "template.json"
    harness_path = report_root / "demo-harness.json"

    template_source.parent.mkdir(parents=True, exist_ok=True)
    report_root.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(example_source, template_source)
    template_readme.write_text(
        "\n".join(
            [
                f"# {args.example} Template",
                "",
                "This machine-owned template is derived from the checked-in showcase portfolio.",
                "",
                f"- source example: `{example_record['source']}`",
                f"- generated source: `{display_path(template_source)}`",
                "- live commands:",
                f"  - `python scripts/objc3c_public_workflow_runner.py materialize-project-template --example {args.example}`",
                f"  - `python scripts/objc3c_public_workflow_runner.py materialize-playground-workspace {display_path(template_source)}`",
                f"  - `python scripts/objc3c_public_workflow_runner.py benchmark-runtime-inspector {display_path(template_source)}`",
                "  - `python scripts/objc3c_public_workflow_runner.py inspect-bonus-tool-integration`",
            ]
        )
        + "\n",
        encoding="utf-8",
    )

    template_manifest.write_text(
        json.dumps(
            {
                "contract_id": TEMPLATE_CONTRACT_ID,
                "schema_version": 1,
                "example_id": args.example,
                "source_origin": str(example_record["source"]),
                "template_root": display_path(template_root),
                "template_source": display_path(template_source),
                "template_readme": display_path(template_readme),
                "tutorial_guides": [
                    "docs/tutorials/getting_started.md",
                    "docs/tutorials/build_run_verify.md",
                    "docs/tutorials/guided_walkthrough.md",
                ],
                "public_actions": [
                    "materialize-project-template",
                    "inspect-bonus-tool-integration",
                    "materialize-playground-workspace",
                    "benchmark-runtime-inspector",
                ],
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )

    integration_step = run_step(
        "inspect-bonus-tool-integration",
        [sys.executable, str(PUBLIC_RUNNER), "inspect-bonus-tool-integration"],
    )
    playground_step = run_step(
        "materialize-playground-workspace",
        [
            sys.executable,
            str(PUBLIC_RUNNER),
            "materialize-playground-workspace",
            display_path(template_source),
        ],
    )
    benchmark_step = run_step(
        "benchmark-runtime-inspector",
        [
            sys.executable,
            str(PUBLIC_RUNNER),
            "benchmark-runtime-inspector",
            display_path(template_source),
        ],
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

    payload = {
        "contract_id": HARNESS_CONTRACT_ID,
        "schema_version": 1,
        "ok": not failures,
        "failures": failures,
        "template_contract": display_path(template_manifest),
        "template_source": display_path(template_source),
        "integration_report": integration_report,
        "playground_workspace": playground_workspace,
        "benchmark_report": benchmark_report,
        "public_actions": [
            "materialize-project-template",
            "inspect-bonus-tool-integration",
            "materialize-playground-workspace",
            "benchmark-runtime-inspector",
        ],
    }
    harness_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"template_path: {display_path(template_manifest)}")
    print(f"harness_path: {display_path(harness_path)}")
    if failures:
        print("project-template: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("project-template: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
