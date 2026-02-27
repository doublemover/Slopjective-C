#!/usr/bin/env python3
"""Validate release evidence artifacts and generated index output."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
import tempfile
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

from jsonschema import Draft202012Validator
from jsonschema.exceptions import SchemaError, ValidationError

ROOT = Path(__file__).resolve().parents[1]
MODE = "release-evidence-contract-v2"
DEFAULT_CONTRACT_ID = "release-evidence-contract/default"

DEFAULT_SCHEMA_DATA_PAIRS = (
    (
        "runtime-manifest",
        Path("schemas/objc3-runtime-2025Q4.manifest.schema.json"),
        Path("reports/conformance/manifests/objc3-runtime-2025Q4.manifest.json"),
    ),
    (
        "abi-manifest",
        Path("schemas/objc3-abi-2025Q4.schema.json"),
        Path("reports/conformance/manifests/objc3-abi-2025Q4.example.json"),
    ),
    (
        "conformance-evidence-bundle",
        Path("schemas/objc3-conformance-evidence-bundle-v1.schema.json"),
        Path("reports/conformance/bundles/objc3-conformance-evidence-bundle-v0.11.example.json"),
    ),
)

INDEX_REQUIRED_KEYS: tuple[str, ...] = (
    "schema_id",
    "index_version",
    "release_label",
    "generated_at",
    "input_root",
    "artifact_count",
    "profile_count",
    "release_count",
    "artifacts",
    "profiles",
    "releases",
)

SCOPE_ORDER = {
    "contract": 0,
    "pair": 1,
    "index": 2,
}


@dataclass(frozen=True)
class SchemaDataPair:
    pair_id: str
    schema_path: Path
    data_path: Path


@dataclass(frozen=True)
class IndexGenerationConfig:
    generated_at: str | None
    strict_generated_at: bool


@dataclass(frozen=True)
class ReleaseEvidenceContract:
    contract_id: str
    release_label: str | None
    schema_data_pairs: tuple[SchemaDataPair, ...]
    index_generation: IndexGenerationConfig


@dataclass(frozen=True)
class DriftFinding:
    scope: str
    check_id: str
    detail: str


class HardFailError(RuntimeError):
    """Raised when validator execution cannot continue safely."""


def normalize_repo_path(path: Path) -> str:
    try:
        return path.resolve().relative_to(ROOT).as_posix()
    except ValueError:
        return path.resolve().as_posix()


def resolve_repo_path(raw_path: str) -> Path:
    candidate = Path(raw_path)
    if not candidate.is_absolute():
        candidate = ROOT / candidate
    return candidate.resolve()


def shell_quote(token: str) -> str:
    if any(character.isspace() for character in token):
        return f'"{token}"'
    return token


def render_command(tokens: list[str]) -> str:
    return " ".join(shell_quote(token) for token in tokens)


def normalize_scope(scope: str) -> str:
    if scope == "contract":
        return "contract"
    if scope == "index":
        return "index"
    if scope.startswith("pair["):
        return "pair"
    return "unknown"


def finding_sort_key(finding: DriftFinding) -> tuple[int, str, str, str]:
    scope_kind = normalize_scope(finding.scope)
    rank = SCOPE_ORDER.get(scope_kind, 99)
    return (rank, finding.scope, finding.check_id, finding.detail)


def add_finding(
    findings: list[DriftFinding],
    *,
    scope: str,
    check_id: str,
    detail: str,
) -> None:
    findings.append(DriftFinding(scope=scope, check_id=check_id, detail=detail))


def read_text_hard_fail(path: Path, *, artifact: str) -> str:
    display_path = normalize_repo_path(path)
    if not path.exists():
        raise HardFailError(f"{artifact} file does not exist: {display_path}")
    if not path.is_file():
        raise HardFailError(f"{artifact} path is not a file: {display_path}")
    try:
        return path.read_text(encoding="utf-8")
    except UnicodeDecodeError as exc:
        raise HardFailError(f"{artifact} file is not valid UTF-8: {display_path}") from exc
    except OSError as exc:
        raise HardFailError(f"unable to read {artifact} file {display_path}: {exc}") from exc


def parse_json_hard_fail(raw_text: str, *, path: Path, artifact: str) -> object:
    try:
        return json.loads(raw_text)
    except json.JSONDecodeError as exc:
        raise HardFailError(
            f"{artifact} file is not valid JSON: {normalize_repo_path(path)} "
            f"(line {exc.lineno} column {exc.colno}: {exc.msg})"
        ) from exc


def coerce_non_empty_string(value: Any) -> str | None:
    if not isinstance(value, str):
        return None
    stripped = value.strip()
    return stripped or None


def parse_schema_data_pairs(
    value: Any,
    *,
    findings: list[DriftFinding],
) -> tuple[SchemaDataPair, ...]:
    if not isinstance(value, list) or not value:
        add_finding(
            findings,
            scope="contract",
            check_id="CON-02",
            detail="schema_data_pairs must be a non-empty array",
        )
        return ()

    pairs: list[SchemaDataPair] = []
    seen_ids: set[str] = set()

    for index, item in enumerate(value):
        entry_ref = f"schema_data_pairs[{index}]"
        if not isinstance(item, dict):
            add_finding(
                findings,
                scope="contract",
                check_id="CON-03",
                detail=f"{entry_ref} must be an object",
            )
            continue

        pair_id = coerce_non_empty_string(item.get("id"))
        if pair_id is None:
            add_finding(
                findings,
                scope="contract",
                check_id="CON-04",
                detail=f"{entry_ref}.id must be a non-empty string",
            )
            continue
        if pair_id in seen_ids:
            add_finding(
                findings,
                scope="contract",
                check_id="CON-05",
                detail=f"{entry_ref}.id duplicates existing identifier {pair_id!r}",
            )
            continue
        seen_ids.add(pair_id)

        schema_raw = coerce_non_empty_string(item.get("schema"))
        if schema_raw is None:
            add_finding(
                findings,
                scope="contract",
                check_id="CON-06",
                detail=f"{entry_ref}.schema must be a non-empty string",
            )
            continue
        data_raw = coerce_non_empty_string(item.get("data"))
        if data_raw is None:
            add_finding(
                findings,
                scope="contract",
                check_id="CON-07",
                detail=f"{entry_ref}.data must be a non-empty string",
            )
            continue

        pairs.append(
            SchemaDataPair(
                pair_id=pair_id,
                schema_path=resolve_repo_path(schema_raw),
                data_path=resolve_repo_path(data_raw),
            )
        )

    return tuple(pairs)


def parse_index_generation(
    value: Any,
    *,
    findings: list[DriftFinding],
) -> IndexGenerationConfig | None:
    if value is None:
        return IndexGenerationConfig(
            generated_at=None,
            strict_generated_at=False,
        )
    if not isinstance(value, dict):
        add_finding(
            findings,
            scope="contract",
            check_id="CON-08",
            detail="index_generation must be an object",
        )
        return None

    generated_at_raw = value.get("generated_at")
    if generated_at_raw is not None and not isinstance(generated_at_raw, str):
        add_finding(
            findings,
            scope="contract",
            check_id="CON-09",
            detail="index_generation.generated_at must be string or null",
        )
        return None

    strict_generated_at_raw = value.get("strict_generated_at", False)
    if not isinstance(strict_generated_at_raw, bool):
        add_finding(
            findings,
            scope="contract",
            check_id="CON-10",
            detail="index_generation.strict_generated_at must be boolean",
        )
        return None

    generated_at = generated_at_raw.strip() if isinstance(generated_at_raw, str) else None
    if generated_at == "":
        add_finding(
            findings,
            scope="contract",
            check_id="CON-11",
            detail="index_generation.generated_at cannot be empty",
        )
        return None

    return IndexGenerationConfig(
        generated_at=generated_at,
        strict_generated_at=strict_generated_at_raw,
    )


def default_contract() -> ReleaseEvidenceContract:
    pairs = tuple(
        SchemaDataPair(
            pair_id=pair_id,
            schema_path=resolve_repo_path(schema_rel.as_posix()),
            data_path=resolve_repo_path(data_rel.as_posix()),
        )
        for pair_id, schema_rel, data_rel in DEFAULT_SCHEMA_DATA_PAIRS
    )
    return ReleaseEvidenceContract(
        contract_id=DEFAULT_CONTRACT_ID,
        release_label=None,
        schema_data_pairs=pairs,
        index_generation=IndexGenerationConfig(
            generated_at=None,
            strict_generated_at=False,
        ),
    )


def extract_release_label(payload: object) -> str | None:
    if not isinstance(payload, dict):
        return None
    direct = coerce_non_empty_string(payload.get("release_label"))
    if direct is not None:
        return direct
    train = coerce_non_empty_string(payload.get("release_train"))
    if train is not None:
        return train
    baseline = payload.get("spec_baseline")
    if isinstance(baseline, dict):
        baseline_train = coerce_non_empty_string(baseline.get("release_train"))
        if baseline_train is not None:
            return baseline_train
    return None


def infer_release_label_from_pairs(
    pairs: Sequence[SchemaDataPair],
    *,
    findings: list[DriftFinding],
) -> str | None:
    observed_labels: set[str] = set()
    for pair in pairs:
        try:
            payload = parse_json_hard_fail(
                read_text_hard_fail(pair.data_path, artifact=f"pair[{pair.pair_id}] data"),
                path=pair.data_path,
                artifact=f"pair[{pair.pair_id}] data",
            )
        except HardFailError:
            continue
        observed = extract_release_label(payload)
        if observed is not None:
            observed_labels.add(observed)

    if len(observed_labels) == 1:
        return next(iter(observed_labels))
    if len(observed_labels) > 1:
        add_finding(
            findings,
            scope="contract",
            check_id="CON-13",
            detail=(
                "multiple release labels inferred from schema_data_pairs: "
                + ", ".join(sorted(observed_labels))
            ),
        )
    return None


def load_contract(contract_arg: str | None) -> tuple[ReleaseEvidenceContract | None, list[DriftFinding]]:
    findings: list[DriftFinding] = []
    if contract_arg is None:
        return default_contract(), findings

    contract_path = resolve_repo_path(contract_arg)
    raw_text = read_text_hard_fail(contract_path, artifact="contract")
    payload = parse_json_hard_fail(raw_text, path=contract_path, artifact="contract")

    if not isinstance(payload, dict):
        raise HardFailError(
            f"contract root is not a JSON object: {normalize_repo_path(contract_path)}"
        )

    contract_id = coerce_non_empty_string(payload.get("contract_id")) or normalize_repo_path(
        contract_path
    )
    release_label = payload.get("release_label")
    if release_label is not None:
        release_label = coerce_non_empty_string(release_label)
        if release_label is None:
            add_finding(
                findings,
                scope="contract",
                check_id="CON-01",
                detail="release_label must be a non-empty string or null",
            )

    schema_data_pairs = parse_schema_data_pairs(
        payload.get("schema_data_pairs"),
        findings=findings,
    )
    index_generation = parse_index_generation(
        payload.get("index_generation"),
        findings=findings,
    )
    if findings or index_generation is None or not schema_data_pairs:
        return None, findings

    return (
        ReleaseEvidenceContract(
        contract_id=contract_id,
        release_label=release_label,
        schema_data_pairs=schema_data_pairs,
        index_generation=index_generation,
    )
        ,
        findings,
    )


def resolve_release_label(
    cli_release_label: str | None,
    *,
    contract: ReleaseEvidenceContract,
    findings: list[DriftFinding],
) -> str | None:
    for raw_value in (cli_release_label, contract.release_label):
        label = coerce_non_empty_string(raw_value)
        if label is not None:
            return label
    inferred_label = infer_release_label_from_pairs(contract.schema_data_pairs, findings=findings)
    if inferred_label is not None:
        return inferred_label
    add_finding(
        findings,
        scope="contract",
        check_id="CON-12",
        detail=(
            f"{contract.contract_id}: failed to resolve release label "
            "(set --release-label, contract release_label, or provide inferable data payloads)"
        ),
    )
    return None


def validate_pairs(
    pairs: Sequence[SchemaDataPair],
    findings: list[DriftFinding],
) -> None:
    for pair in pairs:
        scope = f"pair[{pair.pair_id}]"
        schema_path = pair.schema_path
        data_path = pair.data_path

        schema_display = normalize_repo_path(schema_path)
        data_display = normalize_repo_path(data_path)

        schema_exists = True
        if not schema_path.exists():
            add_finding(
                findings,
                scope=scope,
                check_id="PAIR-01",
                detail=f"missing required schema file: {schema_display}",
            )
            schema_exists = False
        elif not schema_path.is_file():
            add_finding(
                findings,
                scope=scope,
                check_id="PAIR-02",
                detail=f"schema path is not a file: {schema_display}",
            )
            schema_exists = False

        data_exists = True
        if not data_path.exists():
            add_finding(
                findings,
                scope=scope,
                check_id="PAIR-03",
                detail=f"missing required data file: {data_display}",
            )
            data_exists = False
        elif not data_path.is_file():
            add_finding(
                findings,
                scope=scope,
                check_id="PAIR-04",
                detail=f"data path is not a file: {data_display}",
            )
            data_exists = False

        if not (schema_exists and data_exists):
            continue

        try:
            schema_raw = schema_path.read_text(encoding="utf-8")
        except UnicodeDecodeError:
            add_finding(
                findings,
                scope=scope,
                check_id="PAIR-05",
                detail=f"schema file is not valid UTF-8: {schema_display}",
            )
            continue
        except OSError as exc:
            add_finding(
                findings,
                scope=scope,
                check_id="PAIR-06",
                detail=f"unable to read schema file {schema_display}: {exc}",
            )
            continue

        try:
            data_raw = data_path.read_text(encoding="utf-8")
        except UnicodeDecodeError:
            add_finding(
                findings,
                scope=scope,
                check_id="PAIR-07",
                detail=f"data file is not valid UTF-8: {data_display}",
            )
            continue
        except OSError as exc:
            add_finding(
                findings,
                scope=scope,
                check_id="PAIR-08",
                detail=f"unable to read data file {data_display}: {exc}",
            )
            continue

        try:
            schema_payload = json.loads(schema_raw)
        except json.JSONDecodeError as exc:
            add_finding(
                findings,
                scope=scope,
                check_id="PAIR-09",
                detail=(
                    f"schema file is not valid JSON: {schema_display} "
                    f"(line {exc.lineno} column {exc.colno}: {exc.msg})"
                ),
            )
            continue

        try:
            data_payload = json.loads(data_raw)
        except json.JSONDecodeError as exc:
            add_finding(
                findings,
                scope=scope,
                check_id="PAIR-10",
                detail=(
                    f"data file is not valid JSON: {data_display} "
                    f"(line {exc.lineno} column {exc.colno}: {exc.msg})"
                ),
            )
            continue

        if not isinstance(schema_payload, dict):
            add_finding(
                findings,
                scope=scope,
                check_id="PAIR-11",
                detail=f"schema root is not a JSON object: {schema_display}",
            )
            continue

        try:
            Draft202012Validator.check_schema(schema_payload)
        except SchemaError as exc:
            add_finding(
                findings,
                scope=scope,
                check_id="PAIR-12",
                detail=f"schema definition is invalid: {exc.message}",
            )
            continue
        except Exception as exc:
            add_finding(
                findings,
                scope=scope,
                check_id="PAIR-13",
                detail=f"schema validation setup failed: {exc}",
            )
            continue

        try:
            Draft202012Validator(schema_payload).validate(data_payload)
        except ValidationError as exc:
            path = ".".join(str(p) for p in exc.path) or "<root>"
            add_finding(
                findings,
                scope=scope,
                check_id="PAIR-14",
                detail=f"schema validation failed at {path}: {exc.message}",
            )
        except Exception as exc:
            add_finding(
                findings,
                scope=scope,
                check_id="PAIR-15",
                detail=f"validation error: {exc}",
            )


def generate_and_check_index(
    *,
    pairs: Sequence[SchemaDataPair],
    release_label: str,
    index_generation: IndexGenerationConfig,
    findings: list[DriftFinding],
) -> None:
    index_script = ROOT / "scripts/generate_conformance_evidence_index.py"
    index_script_display = normalize_repo_path(index_script)
    if not index_script.exists():
        raise HardFailError(f"index-generator file does not exist: {index_script_display}")
    if not index_script.is_file():
        raise HardFailError(f"index-generator path is not a file: {index_script_display}")

    with tempfile.NamedTemporaryFile(
        prefix="release-evidence-index-",
        suffix=".json",
        delete=False,
        dir=ROOT,
    ) as handle:
        temp_path = Path(handle.name)

    try:
        command: list[str] = [
            sys.executable,
            str(index_script),
            "--output",
            str(temp_path),
            "--release-label",
            release_label,
        ]
        if index_generation.generated_at is not None:
            command.extend(["--generated-at", index_generation.generated_at])
        if index_generation.strict_generated_at:
            command.append("--strict-generated-at")

        proc = subprocess.run(
            command,
            cwd=ROOT,
            capture_output=True,
            text=True,
            check=False,
        )
        if proc.returncode != 0:
            detail = proc.stderr.strip() or proc.stdout.strip() or "unknown error"
            detail = " ".join(detail.split())
            raise HardFailError(
                f"index-generator failed (exit {proc.returncode}): {detail}"
            )
        if not temp_path.exists():
            raise HardFailError("index-generator did not create output file")

        generated_raw = read_text_hard_fail(temp_path, artifact="generated index")
        payload = parse_json_hard_fail(
            generated_raw,
            path=temp_path,
            artifact="generated index",
        )

        if not isinstance(payload, dict):
            add_finding(
                findings,
                scope="index",
                check_id="IDX-01",
                detail="generated evidence index root is not a JSON object",
            )
            return

        missing = sorted(set(INDEX_REQUIRED_KEYS).difference(payload.keys()))
        if missing:
            add_finding(
                findings,
                scope="index",
                check_id="IDX-02",
                detail=f"missing required key(s): {', '.join(missing)}",
            )
            return

        artifacts = payload.get("artifacts")
        if not isinstance(artifacts, list) or not artifacts:
            add_finding(
                findings,
                scope="index",
                check_id="IDX-03",
                detail="generated evidence index has no artifacts[] entries",
            )
            return

        observed_release_label = payload.get("release_label")
        if observed_release_label != release_label:
            add_finding(
                findings,
                scope="index",
                check_id="IDX-04",
                detail=(
                    f"generated release_label mismatch: expected {release_label!r} "
                    f"but found {observed_release_label!r}"
                ),
            )

        if index_generation.strict_generated_at:
            observed_generated_at = payload.get("generated_at")
            if observed_generated_at != index_generation.generated_at:
                add_finding(
                    findings,
                    scope="index",
                    check_id="IDX-05",
                    detail=(
                        "strict generated_at mismatch: expected "
                        f"{index_generation.generated_at!r} but found "
                        f"{observed_generated_at!r}"
                    ),
                )

        artifact_paths = {
            item.get("artifact_path")
            for item in artifacts
            if isinstance(item, dict) and isinstance(item.get("artifact_path"), str)
        }
        for pair in pairs:
            required_data_path = normalize_repo_path(pair.data_path)
            if required_data_path not in artifact_paths:
                add_finding(
                    findings,
                    scope=f"pair[{pair.pair_id}]",
                    check_id="IDX-06",
                    detail=(
                        "generated evidence index does not reference required artifact: "
                        f"{required_data_path}"
                    ),
                )
    except HardFailError:
        raise
    except Exception as exc:
        raise HardFailError(f"index-generator verification failed: {exc}") from exc
    finally:
        if temp_path.exists():
            temp_path.unlink()


def build_rerun_command(contract_arg: str | None, release_label_arg: str | None) -> str:
    tokens = ["python", "scripts/check_release_evidence.py"]
    if contract_arg is not None:
        contract_path = resolve_repo_path(contract_arg)
        tokens.extend(["--contract", normalize_repo_path(contract_path)])
    if release_label_arg is not None:
        label = coerce_non_empty_string(release_label_arg)
        if label is not None:
            tokens.extend(["--release-label", label])
    return render_command(tokens)


def render_drift_report(
    findings: list[DriftFinding],
    *,
    contract_arg: str | None,
    release_label_arg: str | None,
) -> str:
    ordered = sorted(findings, key=finding_sort_key)
    lines = [
        f"release-evidence: contract drift detected ({len(ordered)} issue(s)).",
        "drift findings:",
    ]
    for finding in ordered:
        lines.append(f"- {finding.scope}:{finding.check_id}")
        lines.append(f"  {finding.detail}")
    lines.extend(
        [
            "remediation:",
            "1. Restore missing/invalid release-evidence contract inputs or artifact "
            "references, then rerun.",
            "2. Re-run validator:",
            build_rerun_command(contract_arg, release_label_arg),
        ]
    )
    return "\n".join(lines)


def render_success_report(*, contract: ReleaseEvidenceContract, release_label: str) -> str:
    lines = [
        "release-evidence: OK",
        f"- mode={MODE}",
        f"- contract_id={contract.contract_id}",
        f"- release_label={release_label}",
        f"- schema_data_pairs={len(contract.schema_data_pairs)}",
        "- fail_closed=true",
    ]
    return "\n".join(lines)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description=(
            "Validate release evidence artifacts (schemas + examples + "
            "generated evidence index)."
        )
    )
    parser.add_argument(
        "--contract",
        default=None,
        help=(
            "Optional path to release evidence contract JSON. "
            "When omitted, built-in default schema/data pair paths are used."
        ),
    )
    parser.add_argument(
        "--release-label",
        default=None,
        help=(
            "Optional release label override for generated index validation. "
            "Defaults to contract release_label, then inferred label from data payloads."
        ),
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)

    findings: list[DriftFinding] = []
    contract: ReleaseEvidenceContract | None = None
    release_label: str | None = None

    try:
        contract, contract_findings = load_contract(args.contract)
        findings.extend(contract_findings)
        if contract is not None:
            release_label = resolve_release_label(
                args.release_label,
                contract=contract,
                findings=findings,
            )
            if release_label is not None:
                validate_pairs(contract.schema_data_pairs, findings)
                has_pair_findings = any(
                    normalize_scope(finding.scope) == "pair" for finding in findings
                )
                if not has_pair_findings:
                    generate_and_check_index(
                        pairs=contract.schema_data_pairs,
                        release_label=release_label,
                        index_generation=contract.index_generation,
                        findings=findings,
                    )

        if findings:
            print(
                render_drift_report(
                    findings,
                    contract_arg=args.contract,
                    release_label_arg=args.release_label,
                ),
                file=sys.stderr,
            )
            return 1

        if contract is None or release_label is None:
            print("release-evidence: error: validator reached invalid terminal state", file=sys.stderr)
            return 2

        print(
            render_success_report(
                contract=contract,
                release_label=release_label,
            )
        )
        return 0
    except HardFailError as exc:
        print(f"release-evidence: error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
