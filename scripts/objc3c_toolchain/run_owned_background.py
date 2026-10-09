"""Run a project-owned Windows build script without an interactive console.

The implementation program stores runners and process records in its own
``temp/full-compatibility`` area. Each invocation starts one runner, closes
stdin, redirects output, and returns its exact process identity. It never
changes terminal preferences or retires an existing process.
"""

from __future__ import annotations

import argparse
import datetime
import json
import os
from pathlib import Path
import re
import subprocess


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--runner", required=True)
    parser.add_argument("--label", required=True)
    args = parser.parse_args()
    if os.name != "nt":
        parser.error("This launcher is qualified for the Windows execution profile")
    root = Path(__file__).resolve().parents[2]
    owned = (root / "temp/full-compatibility").resolve()
    runner = (root / args.runner).resolve()
    if not runner.is_relative_to(owned) or not runner.is_file() or runner.suffix != ".ps1":
        parser.error("Runner must be an existing task-owned PowerShell script")
    if not re.fullmatch(r"[a-z0-9-]+", args.label):
        parser.error("Invalid log label")
    powershell = Path(r"C:\Program Files\PowerShell\7\pwsh.exe")
    if not powershell.is_file():
        parser.error("The admitted PowerShell 7 executable is unavailable")
    logs = root / "out/toolchains/objc3-development"
    logs.mkdir(parents=True, exist_ok=True)
    command = [str(powershell), "-NoLogo", "-NoProfile", "-NonInteractive",
               "-File", str(runner)]
    stdout_path = logs / f"build-{args.label}.log"
    stderr_path = logs / f"build-{args.label}.err"
    record_path = owned / f"process-{args.label}.json"
    if any(path.exists() for path in (stdout_path, stderr_path, record_path)):
        parser.error("This run label already has evidence; choose a fresh label")
    with stdout_path.open("xb") as stdout, stderr_path.open("xb") as stderr:
        process = subprocess.Popen(
            command, cwd=root, stdin=subprocess.DEVNULL, stdout=stdout,
            stderr=stderr, close_fds=True,
            creationflags=subprocess.CREATE_NO_WINDOW | subprocess.CREATE_NEW_PROCESS_GROUP,
        )
    record = {
        "pid": process.pid, "runner": str(runner), "command": command,
        "created_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
        "stdin": "DEVNULL", "stdout": str(stdout_path), "stderr": str(stderr_path),
        "create_no_window": True, "new_process_group": True, "noninteractive": True,
    }
    record_path.write_text(json.dumps(record, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(record, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
