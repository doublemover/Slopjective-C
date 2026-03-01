#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path


def build_task_hygiene_sequence() -> list[str]:
    closeout_ids = [135, 137, 138, 139, 140, 141, 142, 143, 144, 145, *range(153, 193)]
    closeout_scripts = [f"check:compiler-closeout:m{milestone_id}" for milestone_id in closeout_ids]
    return [
        "check:planning-hygiene",
        *closeout_scripts,
        "check:catalog-status-integrity",
        "check:catalog-status-metadata",
        "check:open-blocker-audit:repo-root:fixtures",
        "extract:open-issues",
        "check:issue-drift",
    ]


def load_declared_npm_scripts(repo_root: Path) -> set[str]:
    package_json_path = repo_root / "package.json"
    package_payload = json.loads(package_json_path.read_text(encoding="utf-8"))
    scripts = package_payload.get("scripts", {})
    if not isinstance(scripts, dict):
        raise RuntimeError("package.json scripts field must be an object")
    return set(str(key) for key in scripts.keys())


def run_npm_script(repo_root: Path, script_name: str) -> int:
    print(f"[check:task-hygiene] npm run {script_name}", flush=True)
    completed = subprocess.run(["npm", "run", script_name], cwd=repo_root)
    return int(completed.returncode)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Run check:task-hygiene gate scripts in deterministic order.")
    parser.add_argument("--dry-run", action="store_true", help="Print planned script sequence without executing npm.")
    args = parser.parse_args(argv)

    repo_root = Path(__file__).resolve().parents[2]
    sequence = build_task_hygiene_sequence()

    declared_scripts = load_declared_npm_scripts(repo_root)
    missing_scripts = [script_name for script_name in sequence if script_name not in declared_scripts]
    if missing_scripts:
        print("check:task-hygiene gate FAIL: missing npm scripts:", file=sys.stderr)
        for script_name in missing_scripts:
            print(f"- {script_name}", file=sys.stderr)
        return 1

    if args.dry_run:
        for script_name in sequence:
            print(script_name)
        return 0

    for script_name in sequence:
        code = run_npm_script(repo_root, script_name)
        if code != 0:
            return code

    print(f"[check:task-hygiene] sequence complete ({len(sequence)} script(s))", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
