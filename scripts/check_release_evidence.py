#!/usr/bin/env python3
"""Validate the release-evidence gate inputs and generated evidence index."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
INDEX_SCRIPT = ROOT / "scripts" / "generate_conformance_evidence_index.py"
INDEX_OUTPUT = ROOT / "tmp" / "reports" / "release_evidence" / "evidence-index.json"
SCHEMA_ID = "objc3-conformance-evidence-index/v1"
ARTIFACT_AUTHENTICITY_SCHEMA_ID = "objc3c.artifact.authenticity.schema.v1"

REQUIRED_SCHEMA_DATA_PAIRS: tuple[tuple[str, str], ...] = (
    (
        "schemas/objc3-runtime-2025Q4.manifest.schema.json",
        "reports/conformance/manifests/objc3-runtime-2025Q4.manifest.json",
    ),
    (
        "schemas/objc3-abi-2025Q4.schema.json",
        "reports/conformance/manifests/objc3-abi-2025Q4.example.json",
    ),
    (
        "schemas/objc3-conformance-evidence-bundle-v1.schema.json",
        "reports/conformance/bundles/objc3-conformance-evidence-bundle-v0.11.example.json",
    ),
)


def fail(message: str) -> int:
    print(f"release-evidence: {message}", file=sys.stderr)
    return 1


def load_json(relative_path: str) -> dict[str, Any] | list[Any]:
    full_path = ROOT / relative_path
    if not full_path.is_file():
        raise FileNotFoundError(relative_path)
    try:
        return json.loads(full_path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        raise ValueError(f"{relative_path}: invalid JSON parse ({exc.msg})") from exc


def main() -> int:
    if not INDEX_SCRIPT.is_file():
        return fail("missing index generator scripts/generate_conformance_evidence_index.py")

    required_artifact_paths: set[str] = set()
    for schema_path, data_path in REQUIRED_SCHEMA_DATA_PAIRS:
        for relative_path in (schema_path, data_path):
            try:
                load_json(relative_path)
            except FileNotFoundError:
                return fail(f"missing required file {relative_path}")
            except ValueError as exc:
                return fail(str(exc))
        required_artifact_paths.add(data_path)

    INDEX_OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    result = subprocess.run(
        [
            sys.executable,
            str(INDEX_SCRIPT),
            "--output",
            str(INDEX_OUTPUT),
            "--release-label",
            "v0.11",
        ],
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=False,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    if result.returncode != 0:
        return fail(f"index generation failed with exit code {result.returncode}")

    try:
        index_payload = json.loads(INDEX_OUTPUT.read_text(encoding="utf-8"))
    except FileNotFoundError:
        return fail(f"generated index was not written to {INDEX_OUTPUT.relative_to(ROOT).as_posix()}")
    except json.JSONDecodeError as exc:
        return fail(f"generated index is invalid JSON ({exc.msg})")

    if index_payload.get("schema_id") != SCHEMA_ID:
        return fail("generated index missing required schema_id")
    if not isinstance(index_payload.get("artifacts"), list):
        return fail("generated index missing artifacts list")
    envelope = index_payload.get("artifact_authenticity")
    if not isinstance(envelope, dict):
        return fail("generated index missing artifact_authenticity envelope")
    expected_envelope = {
        "authenticity_schema_id": ARTIFACT_AUTHENTICITY_SCHEMA_ID,
        "provenance_class": "genuine_generated_output",
        "surface_id": "objc3c.public_conformance.evidence_index.v1",
        "artifact_family_id": "objc3c.genuine_generated_output.conformance_evidence_index.v1",
        "report_family_id": "objc3c.genuine_generated_output.release_evidence_index_report.v1",
        "generator_or_compile_path": "python scripts/generate_conformance_evidence_index.py",
        "input_root": "reports/conformance",
        "output_path": "tmp/reports/release_evidence/evidence-index.json",
    }
    for field_name, expected_value in expected_envelope.items():
        if envelope.get(field_name) != expected_value:
            return fail(
                f"generated index artifact_authenticity field {field_name} expected {expected_value!r}, observed {envelope.get(field_name)!r}"
            )
    replay = index_payload.get("replay")
    if not isinstance(replay, dict):
        return fail("generated index missing replay instructions")
    replay_command = replay.get("command")
    if replay.get("cwd") != ".":
        return fail("generated index replay instructions must set cwd to repository root")
    if not isinstance(replay_command, list) or replay_command[:2] != ["python", "scripts/generate_conformance_evidence_index.py"]:
        return fail("generated index replay instructions must invoke the canonical generator")

    artifact_paths = {
        artifact.get("artifact_path")
        for artifact in index_payload["artifacts"]
        if isinstance(artifact, dict)
    }
    missing_artifact_paths = sorted(required_artifact_paths - artifact_paths)
    if missing_artifact_paths:
        joined = ", ".join(missing_artifact_paths)
        return fail(f"generated index missing required artifact references: {joined}")

    print(f"release-evidence: PASS ({INDEX_OUTPUT.relative_to(ROOT).as_posix()})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
