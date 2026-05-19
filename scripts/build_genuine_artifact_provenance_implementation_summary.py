#!/usr/bin/env python3
from __future__ import annotations

from objc3c_tooling.json_io import write_json_file
import json
import subprocess
from pathlib import Path
from typing import Any
from objc3c_tooling.subprocesses import command_text, python_script_command

ROOT = Path(__file__).resolve().parents[1]
GENERATOR = ROOT / "scripts/generate_conformance_evidence_index.py"
INPUT_ROOT = ROOT / "reports" / "conformance"
INDEX_OUTPUT = ROOT / "tmp" / "reports" / "release_evidence" / "evidence-index.json"
EMPTY_INPUT_ROOT = ROOT / "tmp" / "reports" / "release_evidence" / "empty-input"
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/source_hygiene/genuine_artifact_provenance_contract.json"
OUT_DIR = ROOT / "tmp/reports/source-hygiene/genuine-artifact-provenance-implementation"
JSON_OUT = OUT_DIR / "genuine_artifact_provenance_implementation_summary.json"
MD_OUT = OUT_DIR / "genuine_artifact_provenance_implementation_summary.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def read_json_if_available(path: Path) -> dict[str, Any]:
    if not path.is_file():
        return {}
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError:
        return {}
    return payload if isinstance(payload, dict) else {}


def normalize(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    expected_generator_command = command_text(
        python_script_command("scripts/generate_conformance_evidence_index.py")
    )
    checked_in_input_root_present = INPUT_ROOT.is_dir()
    selected_input_root = INPUT_ROOT
    allow_empty_index = False
    if not checked_in_input_root_present:
        EMPTY_INPUT_ROOT.mkdir(parents=True, exist_ok=True)
        selected_input_root = EMPTY_INPUT_ROOT
        allow_empty_index = True
    selected_input_root_arg = normalize(selected_input_root)
    generator_command = python_script_command(
        GENERATOR,
        "--input-root",
        selected_input_root_arg,
        "--output",
        normalize(INDEX_OUTPUT),
        "--release-label",
        "v0.11",
    )
    if allow_empty_index:
        generator_command.append("--allow-empty")
    result = subprocess.run(
        generator_command,
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=False,
    )
    payload = read_json_if_available(INDEX_OUTPUT) if result.returncode == 0 else {}
    envelope = payload.get("artifact_authenticity", {})
    replay = payload.get("replay", {})
    command = replay.get("command")

    required_fields = contract["required_envelope_fields"]
    checks = {
        "generator_exit_zero": result.returncode == 0,
        "output_written": bool(payload) and INDEX_OUTPUT.is_file(),
        "envelope_present": isinstance(envelope, dict) and bool(envelope),
        "replay_present": isinstance(replay, dict) and bool(replay),
        "required_envelope_fields_present": isinstance(envelope, dict)
        and all(field in envelope for field in required_fields),
        "contract_provenance_class_matches_output": envelope.get("provenance_class") == contract["provenance_class"],
        "generator_command_matches_canonical_helper": envelope.get("generator_or_compile_path") == expected_generator_command,
        "output_path_under_allowed_roots": any(
            envelope.get("output_path", "").startswith(root) for root in contract["output_roots"]
        ),
        "input_root_matches_selected_input_root": envelope.get("input_root") == selected_input_root_arg,
        "output_path_matches_release_evidence_index": envelope.get("output_path") == normalize(INDEX_OUTPUT),
        "replay_uses_canonical_generator": isinstance(command, list)
        and command[:2] == python_script_command("scripts/generate_conformance_evidence_index.py"),
        "replay_includes_input_and_output_paths": isinstance(command, list)
        and "--input-root" in command
        and "--output" in command,
    }

    summary = {
        "issue": "source-hygiene-genuine-artifact-provenance-implementation",
        "generator": normalize(GENERATOR),
        "input_root": selected_input_root_arg,
        "checked_in_input_root_present": checked_in_input_root_present,
        "allow_empty_index": allow_empty_index,
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
    write_json_file(JSON_OUT, summary)
    MD_OUT.write_text(
        "# Genuine Artifact Provenance Implementation Summary\n\n"
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
