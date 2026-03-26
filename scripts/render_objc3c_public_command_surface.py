#!/usr/bin/env python3
"""Render the synchronized public command surface runbook for M314-C003."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
PACKAGE_JSON = ROOT / "package.json"
RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
DEFAULT_OUTPUT = ROOT / "docs" / "runbooks" / "objc3c_public_command_surface.md"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--check", action="store_true")
    return parser.parse_args(argv)


def runner_payload() -> dict[str, object]:
    result = subprocess.run(
        [sys.executable, str(RUNNER), "--list-json"],
        cwd=ROOT,
        check=True,
        capture_output=True,
        text=True,
    )
    return json.loads(result.stdout)


def render_markdown() -> str:
    package = json.loads(PACKAGE_JSON.read_text(encoding="utf-8"))
    surface = package["objc3cCommandSurface"]
    workflow_api = surface["workflowApi"]
    payload = runner_payload()
    actions: list[dict[str, object]] = payload["actions"]  # type: ignore[assignment]

    lines: list[str] = [
        "# Objective-C 3 Public Command Surface",
        "",
        "This runbook is generated from the live public workflow runner metadata.",
        "",
        f"- Budget maximum: `{surface['budgetMaximum']}`",
        f"- Current public script count: `{len(surface['publicScripts'])}`",
        f"- Runner path: `{workflow_api['runnerPath']}`",
        f"- Introspection command: `{workflow_api['introspectionCommand']}`",
        "",
        "## Commands",
        "",
        "| Package script | Runner action | Extra args | Backend |",
        "| --- | --- | --- | --- |",
    ]
    for entry in actions:
        package_script = ", ".join(entry["public_scripts"])
        extra_args = "pass-through" if entry["pass_through_args"] else "fixed-shape"
        lines.append(f"| `{package_script}` | `{entry['action']}` | `{extra_args}` | `{entry['backend']}` |")

    lines.extend(
        [
            "",
            "## Operator Notes",
            "",
            "- Use the package scripts above for normal operator workflows.",
            "- `compile:objc3c` is the only public command that accepts pass-through arguments.",
            "- No additional package-script compatibility aliases remain supported.",
            "- Maintainer-only package scripts are limited to repo hygiene and boundary checks.",
            "",
        ]
    )
    return "\n".join(lines)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    rendered = render_markdown()
    if args.check:
        existing = args.output.read_text(encoding="utf-8")
        if existing != rendered:
            print(f"[fail] runbook out of sync: {args.output}", file=sys.stderr)
            return 1
        print(f"[ok] runbook in sync: {args.output}")
        return 0
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(rendered, encoding="utf-8")
    print(f"[ok] wrote {args.output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
