#!/usr/bin/env python3
from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
GENERATOR = ROOT / "scripts/generate_conformance_evidence_index.py"
INPUT_ROOT = ROOT / "reports/conformance"
INDEX_OUTPUT = ROOT / "tmp/reports/release_evidence/evidence-index.json"
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/source_hygiene/genuine_artifact_provenance_contract.json"
OUT_DIR = ROOT / "tmp/reports/m315/M315-C002"
JSON_OUT = OUT_DIR / "genuine_artifact_provenance_implementation_summary.json"
MD_OUT = OUT_DIR / "genuine_artifact_provenance_implementation_summary.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def normalize(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    result = subprocess.run(
        [
            sys.executable,
            str(GENERATOR),
            "--input-root",
            normalize(INPUT_ROOT),
            "--output",
            normalize(INDEX_OUTPUT),
            "--release-label",
            "v0.11",
        ],
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=False,
    )
    payload = read_json(INDEX_OUTPUT)
    envelope = payload.get("artifact_authenticity", {})
    replay = payload.get("replay", {})
    command = replay.get("command")

    required_fields = contract["required_envelope_fields"]
    checks = {
        "generator_exit_zero": result.returncode == 0,
        "output_written": INDEX_OUTPUT.is_file(),
        "envelope_present": isinstance(envelope, dict),
        "replay_present": isinstance(replay, dict),
        "required_envelope_fields_present": isinstance(envelope, dict)
        and all(field in envelope for field in required_fields),
        "contract_provenance_class_matches_output": envelope.get("provenance_class") == contract["provenance_class"],
        "contract_generator_family_matches_output": envelope.get("generator_or_compile_path") in contract["generator_families"],
        "output_path_under_allowed_roots": any(
            envelope.get("output_path", "").startswith(root) for root in contract["output_roots"]
        ),
        "input_root_matches_reports_conformance": envelope.get("input_root") == "reports/conformance",
        "output_path_matches_release_evidence_index": envelope.get("output_path") == "tmp/reports/release_evidence/evidence-index.json",
        "replay_uses_canonical_generator": isinstance(command, list)
        and command[:2] == ["python", "scripts/generate_conformance_evidence_index.py"],
        "replay_includes_input_and_output_paths": isinstance(command, list)
        and "--input-root" in command
        and "--output" in command,
    }

    summary = {
        "issue": "M315-C002",
        "generator": normalize(GENERATOR),
        "input_root": normalize(INPUT_ROOT),
        "output_path": normalize(INDEX_OUTPUT),
        "artifact_count": payload.get("artifact_count"),
        "profile_count": payload.get("profile_count"),
        "release_count": payload.get("release_count"),
        "artifact_authenticity": envelope,
        "replay": replay,
        "required_envelope_fields": required_fields,
        "generator_stdout": result.stdout.strip(),
        "generator_stderr": result.stderr.strip(),
        "checks": checks,
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# M315-C002 Genuine Artifact Provenance Implementation Summary\n\n"
        f"- Generator: `{summary['generator']}`\n"
        f"- Output: `{summary['output_path']}`\n"
        f"- Artifact count: `{summary['artifact_count']}`\n"
        f"- Replay command: `{' '.join(command) if isinstance(command, list) else ''}`\n"
        f"- Status: `{'PASS' if summary['ok'] else 'FAIL'}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
