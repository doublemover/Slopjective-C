#!/usr/bin/env python3
from __future__ import annotations

from objc3c_tooling.json_io import write_json_file
import json
import subprocess
from pathlib import Path
from typing import Any
from objc3c_tooling.subprocesses import python_script_command

ROOT = Path(__file__).resolve().parents[1]
AUDIT_SCRIPT = ROOT / "scripts/check_source_hygiene_authenticity.py"
SUMMARY_PATH = ROOT / "tmp/reports/source_hygiene/source_hygiene_audit_summary.json"
OUT_DIR = ROOT / "tmp/reports/source-hygiene/audit-issue-summary"
JSON_OUT = OUT_DIR / "source_hygiene_audit_issue_summary.json"
MD_OUT = OUT_DIR / "source_hygiene_audit_issue_summary.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    result = subprocess.run(
        python_script_command(AUDIT_SCRIPT),
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=False,
    )
    payload = read_json(SUMMARY_PATH)
    summary = {
        "issue": "source-hygiene-audit-issue-summary",
        "audit_script": "scripts/check_source_hygiene_authenticity.py",
        "runner_entrypoint": "npm run objc3c -- check-source-hygiene-authenticity",
        "package_bridge": "objc3c",
        "check_count": len(payload["checks"]),
        "generator_exit_zero": result.returncode == 0,
        "audit_ok": payload.get("ok") is True,
        "check_ids": [entry["check_id"] for entry in payload["checks"]],
        "summary_path": "tmp/reports/source_hygiene/source_hygiene_audit_summary.json",
        "stdout": result.stdout.strip(),
        "stderr": result.stderr.strip(),
        "ok": result.returncode == 0 and payload.get("ok") is True,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    write_json_file(JSON_OUT, summary)
    MD_OUT.write_text(
        "# Source Hygiene Audit Summary\n\n"
        f"- Audit script: `{summary['audit_script']}`\n"
        f"- Runner entrypoint: `{summary['runner_entrypoint']}`\n"
        f"- Package bridge: `{summary['package_bridge']}`\n"
        f"- Enforced checks: `{', '.join(summary['check_ids'])}`\n"
        f"- Status: `{'PASS' if summary['ok'] else 'FAIL'}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
