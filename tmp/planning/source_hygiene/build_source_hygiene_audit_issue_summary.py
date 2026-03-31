#!/usr/bin/env python3
from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any

ROOT = Path(r"C:/Users/sneak/Development/Slopjective-C")
AUDIT_SCRIPT = ROOT / "scripts/check_source_hygiene_authenticity.py"
SUMMARY_PATH = ROOT / "tmp/reports/source_hygiene/source_hygiene_audit_summary.json"
OUT_DIR = ROOT / "tmp/reports/m315/M315-D002"
JSON_OUT = OUT_DIR / "source_hygiene_audit_issue_summary.json"
MD_OUT = OUT_DIR / "source_hygiene_audit_issue_summary.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    result = subprocess.run(
        [sys.executable, str(AUDIT_SCRIPT)],
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=False,
    )
    payload = read_json(SUMMARY_PATH)
    summary = {
        "issue": "M315-D002",
        "audit_script": "scripts/check_source_hygiene_authenticity.py",
        "runner_entrypoint": "python scripts/objc3c_public_workflow_runner.py check-source-hygiene-authenticity",
        "package_script": "check:objc3c:source-hygiene",
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
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# M315-D002 Source Hygiene Audit Summary\n\n"
        f"- Audit script: `{summary['audit_script']}`\n"
        f"- Runner entrypoint: `{summary['runner_entrypoint']}`\n"
        f"- Package script: `{summary['package_script']}`\n"
        f"- Enforced checks: `{', '.join(summary['check_ids'])}`\n"
        f"- Status: `{'PASS' if summary['ok'] else 'FAIL'}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
