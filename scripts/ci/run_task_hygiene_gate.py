#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
REGISTRY_PATH = ROOT / "spec" / "governance" / "objc3c_task_hygiene_registry.json"


def load_registry() -> list[dict[str, str]]:
    payload = json.loads(REGISTRY_PATH.read_text(encoding="utf-8"))
    sequence = payload.get("sequence")
    if not isinstance(sequence, list):
        raise RuntimeError("task hygiene registry sequence must be a list")
    normalized: list[dict[str, str]] = []
    for entry in sequence:
        if not isinstance(entry, dict):
            raise RuntimeError("task hygiene registry entries must be objects")
        source_script = entry.get("source_script")
        command = entry.get("command")
        if not isinstance(source_script, str) or not isinstance(command, str):
            raise RuntimeError("task hygiene registry entries require source_script and command strings")
        normalized.append({"source_script": source_script, "command": command})
    return normalized


def run_command(repo_root: Path, source_script: str, command: str) -> int:
    print(f"[check:task-hygiene] {source_script}", flush=True)
    completed = subprocess.run(command, cwd=repo_root, shell=True, check=False)
    return int(completed.returncode)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Run task-hygiene commands in deterministic order.")
    parser.add_argument("--dry-run", action="store_true", help="Print planned direct commands without executing them.")
    args = parser.parse_args(argv)

    sequence = load_registry()

    if args.dry_run:
        for entry in sequence:
            print(f"{entry['source_script']} => {entry['command']}")
        return 0

    for entry in sequence:
        code = run_command(ROOT, entry["source_script"], entry["command"])
        if code != 0:
            return code

    print(f"[check:task-hygiene] sequence complete ({len(sequence)} command(s))", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
