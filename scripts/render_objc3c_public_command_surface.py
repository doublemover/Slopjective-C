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
    public_scripts = package["scripts"]
    payload = runner_payload()
    actions: list[dict[str, object]] = payload["actions"]  # type: ignore[assignment]
    public_action_entries = [entry for entry in actions if entry["public_scripts"]]

    lines: list[str] = [
        "# Objective-C 3 Public Command Surface",
        "",
        "This runbook is generated from the live public workflow runner metadata.",
        "",
        f"- Current public script count: `{len(public_scripts)}`",
        f"- Runner path: `{payload['runner_path']}`",
        f"- Introspection command: `python {payload['runner_path']} --list-json`",
        "",
        "## Commands",
        "",
        "| Package script | Runner action | Tier | Guarantee owner | Extra args | Backend |",
        "| --- | --- | --- | --- | --- | --- |",
    ]
    for entry in public_action_entries:
        package_script = ", ".join(entry["public_scripts"])
        extra_args = "pass-through" if entry["pass_through_args"] else "fixed-shape"
        tier = entry.get("validation_tier", "") or "-"
        owner = entry.get("guarantee_owner", "") or "-"
        lines.append(f"| `{package_script}` | `{entry['action']}` | `{tier}` | `{owner}` | `{extra_args}` | `{entry['backend']}` |")

    lines.extend(
        [
            "",
            "## Operator Notes",
            "",
            "- Use the package scripts above for normal operator workflows.",
            "- Composite validation entrypoints write an integrated runner summary to `tmp/reports/objc3c-public-workflow/<action>.json`.",
            "- Those integrated summaries record the exact child-suite report paths emitted by smoke, replay, runtime-acceptance, and other live validation scripts.",
            "- `compile:objc3c` and the fixture-backed suite commands accept pass-through arguments for bounded selectors.",
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
