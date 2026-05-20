#!/usr/bin/env python3
"""Validate the release-evidence gate inputs and generated evidence index."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any

from objc3c_tooling.subprocesses import command_text, python_script_command

ROOT = Path(__file__).resolve().parents[1]
INDEX_SCRIPT = ROOT / "scripts" / "generate_conformance_evidence_index.py"
PUBLIC_CLAIM_DRIFT_SCRIPT = ROOT / "scripts" / "check_objc3c_public_claim_drift.py"
CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "release_evidence_contract"
    / "release_evidence_gate.json"
)
EMPTY_INPUT_ROOT = ROOT / "tmp" / "reports" / "release_evidence" / "empty-input"
REPORTS_CONFORMANCE_ROOT = ROOT / "reports" / "conformance"
SCHEMA_ID = "objc3-conformance-evidence-index/v1"
ARTIFACT_AUTHENTICITY_SCHEMA_ID = "objc3c.artifact.authenticity.schema.v1"

CONTRACT_ID = "objc3c.release_evidence.gate_contract.v1"


class ReleaseEvidenceContractError(RuntimeError):
    pass


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


def load_release_evidence_contract(path: Path = CONTRACT_PATH) -> dict[str, Any]:
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise ReleaseEvidenceContractError(
            f"missing release evidence contract {path.relative_to(ROOT).as_posix()}"
        ) from exc
    except json.JSONDecodeError as exc:
        raise ReleaseEvidenceContractError(
            f"{path.relative_to(ROOT).as_posix()}: invalid JSON parse ({exc.msg})"
        ) from exc
    if not isinstance(payload, dict):
        raise ReleaseEvidenceContractError(
            "release evidence contract must be a JSON object"
        )
    if payload.get("contract_id") != CONTRACT_ID:
        raise ReleaseEvidenceContractError(
            f"release evidence contract_id must be {CONTRACT_ID}"
        )
    return payload


def require_string(payload: dict[str, Any], key: str, label: str) -> str:
    value = payload.get(key)
    if not isinstance(value, str) or not value:
        raise ReleaseEvidenceContractError(f"{label}.{key} must be a non-empty string")
    return value


def normalize_contract_path(raw_path: str, label: str, *, allow_tmp: bool) -> str:
    normalized = raw_path.replace("\\", "/")
    path = Path(normalized)
    if path.is_absolute() or ".." in path.parts:
        raise ReleaseEvidenceContractError(f"{label} must be a repository-relative path")
    if not allow_tmp and normalized.startswith("tmp/"):
        raise ReleaseEvidenceContractError(f"{label} must not use tmp as source truth")
    return normalized


def release_label_from_contract(contract: dict[str, Any]) -> str:
    return require_string(contract, "release_label", "release_evidence_contract")


def schema_data_pairs_from_contract(
    contract: dict[str, Any],
) -> tuple[tuple[str, str], ...]:
    pairs = contract.get("schema_data_pairs")
    if not isinstance(pairs, list) or not pairs:
        raise ReleaseEvidenceContractError("schema_data_pairs must be a non-empty list")

    seen_ids: set[str] = set()
    pair_ids: list[str] = []
    normalized_pairs: list[tuple[str, str]] = []
    for index, raw_pair in enumerate(pairs):
        if not isinstance(raw_pair, dict):
            raise ReleaseEvidenceContractError(
                f"schema_data_pairs[{index}] must be an object"
            )
        pair_label = f"schema_data_pairs[{index}]"
        pair_id = require_string(raw_pair, "id", pair_label)
        if pair_id in seen_ids:
            raise ReleaseEvidenceContractError(f"duplicate schema_data_pairs id {pair_id}")
        seen_ids.add(pair_id)
        pair_ids.append(pair_id)
        schema_path = normalize_contract_path(
            require_string(raw_pair, "schema", pair_label),
            f"{pair_label}.schema",
            allow_tmp=False,
        )
        data_path = normalize_contract_path(
            require_string(raw_pair, "data", pair_label),
            f"{pair_label}.data",
            allow_tmp=False,
        )
        normalized_pairs.append((schema_path, data_path))

    if pair_ids != sorted(pair_ids, key=str.casefold):
        raise ReleaseEvidenceContractError("schema_data_pairs must be sorted by id")
    return tuple(normalized_pairs)


def empty_input_mode_from_contract(contract: dict[str, Any]) -> dict[str, Any]:
    mode = contract.get("empty_input_mode")
    if not isinstance(mode, dict):
        raise ReleaseEvidenceContractError("empty_input_mode must be an object")
    if mode.get("allowed") is not True:
        raise ReleaseEvidenceContractError(
            "empty_input_mode.allowed must be true or the gate must fail"
        )
    if mode.get("blocks_public_claims") is not True:
        raise ReleaseEvidenceContractError(
            "empty_input_mode.blocks_public_claims must be true"
        )
    reason = mode.get("reason")
    if not isinstance(reason, str) or not reason:
        raise ReleaseEvidenceContractError(
            "empty_input_mode.reason must be a non-empty string"
        )
    blockers = mode.get("blocking_issue_refs")
    if not isinstance(blockers, list) or not blockers:
        raise ReleaseEvidenceContractError(
            "empty_input_mode.blocking_issue_refs must be a non-empty list"
        )
    for index, blocker in enumerate(blockers):
        if not isinstance(blocker, str) or not blocker.startswith("#"):
            raise ReleaseEvidenceContractError(
                f"empty_input_mode.blocking_issue_refs[{index}] must be an issue ref"
            )
    return mode


def generated_index_contract(contract: dict[str, Any]) -> dict[str, Any]:
    generated_index = contract.get("generated_index")
    if not isinstance(generated_index, dict):
        raise ReleaseEvidenceContractError("generated_index must be an object")
    schema_id = require_string(generated_index, "schema_id", "generated_index")
    if schema_id != SCHEMA_ID:
        raise ReleaseEvidenceContractError(
            f"generated_index.schema_id must be {SCHEMA_ID}"
        )
    output_path = normalize_contract_path(
        require_string(generated_index, "output_path", "generated_index"),
        "generated_index.output_path",
        allow_tmp=True,
    )
    if not output_path.startswith("tmp/reports/release_evidence/"):
        raise ReleaseEvidenceContractError(
            "generated_index.output_path must stay under tmp/reports/release_evidence"
        )
    authenticity = generated_index.get("artifact_authenticity")
    if not isinstance(authenticity, dict):
        raise ReleaseEvidenceContractError(
            "generated_index.artifact_authenticity must be an object"
        )
    if authenticity.get("authenticity_schema_id") != ARTIFACT_AUTHENTICITY_SCHEMA_ID:
        raise ReleaseEvidenceContractError(
            "generated_index.artifact_authenticity.authenticity_schema_id "
            f"must be {ARTIFACT_AUTHENTICITY_SCHEMA_ID}"
        )
    return {
        **generated_index,
        "output_path": output_path,
        "artifact_authenticity": authenticity,
    }


def validate_schema_data_pair_files(
    schema_data_pairs: tuple[tuple[str, str], ...],
) -> set[str]:
    required_artifact_paths: set[str] = set()
    for schema_path, data_path in schema_data_pairs:
        for relative_path in (schema_path, data_path):
            try:
                load_json(relative_path)
            except FileNotFoundError as exc:
                raise ReleaseEvidenceContractError(
                    f"missing required file {relative_path}"
                ) from exc
            except ValueError as exc:
                raise ReleaseEvidenceContractError(str(exc)) from exc
        required_artifact_paths.add(data_path)
    return required_artifact_paths


def main() -> int:
    if not INDEX_SCRIPT.is_file():
        return fail("missing index generator scripts/generate_conformance_evidence_index.py")
    if not PUBLIC_CLAIM_DRIFT_SCRIPT.is_file():
        return fail("missing public claim drift checker scripts/check_objc3c_public_claim_drift.py")

    try:
        contract = load_release_evidence_contract()
        schema_data_pairs = schema_data_pairs_from_contract(contract)
        release_label = release_label_from_contract(contract)
        generated_index = generated_index_contract(contract)
        empty_input_mode = empty_input_mode_from_contract(contract)
    except ReleaseEvidenceContractError as exc:
        return fail(str(exc))

    required_artifact_paths: set[str] = set()
    input_root_arg = REPORTS_CONFORMANCE_ROOT.relative_to(ROOT).as_posix()
    allow_empty_index = False
    index_output = ROOT / generated_index["output_path"]

    if REPORTS_CONFORMANCE_ROOT.is_dir():
        claim_drift_result = subprocess.run(
            python_script_command(PUBLIC_CLAIM_DRIFT_SCRIPT, "--check"),
            cwd=ROOT,
            text=True,
            capture_output=True,
            check=False,
        )
        if claim_drift_result.stdout:
            sys.stdout.write(claim_drift_result.stdout)
        if claim_drift_result.stderr:
            sys.stderr.write(claim_drift_result.stderr)
        if claim_drift_result.returncode != 0:
            return fail(
                f"public claim drift gate failed with exit code {claim_drift_result.returncode}"
            )

        try:
            required_artifact_paths = validate_schema_data_pair_files(schema_data_pairs)
        except ReleaseEvidenceContractError as exc:
            return fail(str(exc))
    else:
        EMPTY_INPUT_ROOT.mkdir(parents=True, exist_ok=True)
        input_root_arg = EMPTY_INPUT_ROOT.relative_to(ROOT).as_posix()
        allow_empty_index = True
        print(
            "release-evidence: checked-in conformance corpus is absent; "
            "using contract-authorized generated-only empty index mode "
            f"({', '.join(empty_input_mode['blocking_issue_refs'])})"
        )

    index_output.parent.mkdir(parents=True, exist_ok=True)
    index_command = python_script_command(
        INDEX_SCRIPT,
        "--input-root",
        input_root_arg,
        "--output",
        str(index_output),
        "--release-label",
        release_label,
    )
    if allow_empty_index:
        index_command.append("--allow-empty")
    result = subprocess.run(
        index_command,
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
        index_payload = json.loads(index_output.read_text(encoding="utf-8"))
    except FileNotFoundError:
        return fail(
            "generated index was not written to "
            f"{index_output.relative_to(ROOT).as_posix()}"
        )
    except json.JSONDecodeError as exc:
        return fail(f"generated index is invalid JSON ({exc.msg})")

    if index_payload.get("schema_id") != generated_index["schema_id"]:
        return fail("generated index missing required schema_id")
    if not isinstance(index_payload.get("artifacts"), list):
        return fail("generated index missing artifacts list")
    envelope = index_payload.get("artifact_authenticity")
    if not isinstance(envelope, dict):
        return fail("generated index missing artifact_authenticity envelope")
    expected_envelope = {
        **generated_index["artifact_authenticity"],
        "generator_or_compile_path": command_text(
            python_script_command("scripts/generate_conformance_evidence_index.py")
        ),
        "input_root": input_root_arg,
        "output_path": index_output.relative_to(ROOT).as_posix(),
    }
    for field_name, expected_value in expected_envelope.items():
        if envelope.get(field_name) != expected_value:
            return fail(
                "generated index artifact_authenticity field "
                f"{field_name} expected {expected_value!r}, "
                f"observed {envelope.get(field_name)!r}"
            )
    replay = index_payload.get("replay")
    if not isinstance(replay, dict):
        return fail("generated index missing replay instructions")
    replay_command = replay.get("command")
    if replay.get("cwd") != ".":
        return fail("generated index replay instructions must set cwd to repository root")
    expected_replay_prefix = python_script_command(
        "scripts/generate_conformance_evidence_index.py"
    )
    if (
        not isinstance(replay_command, list)
        or replay_command[:2] != expected_replay_prefix
    ):
        return fail(
            "generated index replay instructions must invoke the canonical generator"
        )

    artifact_paths = {
        artifact.get("artifact_path")
        for artifact in index_payload["artifacts"]
        if isinstance(artifact, dict)
    }
    missing_artifact_paths = sorted(required_artifact_paths - artifact_paths)
    if missing_artifact_paths:
        joined = ", ".join(missing_artifact_paths)
        return fail(f"generated index missing required artifact references: {joined}")

    print(f"release-evidence: PASS ({index_output.relative_to(ROOT).as_posix()})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
