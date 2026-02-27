#!/usr/bin/env python3
"""Validate perf and determinism telemetry for objc3c refactor guardrails."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
MODE = "objc3c-refactor-perf-guard-v1"
SCHEMA_VERSION = "objc3c-refactor-perf-telemetry.v1"

EXIT_OK = 0
EXIT_GUARD_FAIL = 1
EXIT_INPUT_ERROR = 2


class TelemetryInputError(ValueError):
    """Raised when telemetry input cannot be loaded safely."""


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="check_objc3c_refactor_perf_guard.py",
        description=(
            "Fail-closed checker for objc3c refactor perf/determinism telemetry."
        ),
    )
    parser.add_argument(
        "--telemetry",
        type=Path,
        required=True,
        help="Path to telemetry JSON payload.",
    )
    parser.add_argument(
        "--max-regression-pct",
        type=float,
        default=10.0,
        help="Allowed slowdown percentage for overall and per-fixture checks (default: 10.0).",
    )
    parser.add_argument(
        "--max-jitter-ms",
        type=float,
        default=8.0,
        help="Allowed jitter between determinism run A/B elapsed timings (default: 8.0).",
    )
    parser.add_argument(
        "--contract-mode",
        action="store_true",
        help="Emit deterministic JSON summary to stdout for CI/local contracts.",
    )
    return parser


def canonical_json_text(payload: dict[str, object]) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def add_violation(violations: list[dict[str, str]], check_id: str, detail: str) -> None:
    violations.append({"check_id": check_id, "detail": detail})


def parse_positive_number(value: object) -> float | None:
    if isinstance(value, bool):
        return None
    if not isinstance(value, (int, float)):
        return None
    parsed = float(value)
    if parsed <= 0:
        return None
    return parsed


def parse_bool(value: object) -> bool | None:
    if isinstance(value, bool):
        return value
    return None


def parse_sha256(value: object) -> str | None:
    if not isinstance(value, str):
        return None
    if len(value) != 64:
        return None
    if not all(char in "0123456789abcdef" for char in value):
        return None
    return value


def parse_canonical_fixture(value: object) -> str | None:
    if not isinstance(value, str):
        return None
    if not value or value.strip() != value:
        return None
    if "\\" in value:
        return None
    return value


def require_object(
    payload: dict[str, object], key: str, violations: list[dict[str, str]], check_id: str
) -> dict[str, object] | None:
    candidate = payload.get(key)
    if not isinstance(candidate, dict):
        add_violation(violations, check_id, f"telemetry.{key} must be an object")
        return None
    return candidate


def load_telemetry(path: Path) -> dict[str, object]:
    resolved = path.resolve()
    if not resolved.exists():
        raise TelemetryInputError(f"telemetry file not found: {display_path(resolved)}")
    if not resolved.is_file():
        raise TelemetryInputError(f"telemetry path is not a file: {display_path(resolved)}")
    try:
        raw = resolved.read_text(encoding="utf-8")
    except UnicodeDecodeError as exc:
        raise TelemetryInputError(
            f"telemetry file is not valid UTF-8: {display_path(resolved)}"
        ) from exc
    except OSError as exc:
        raise TelemetryInputError(
            f"unable to read telemetry file {display_path(resolved)}: {exc}"
        ) from exc

    try:
        payload = json.loads(raw)
    except json.JSONDecodeError as exc:
        raise TelemetryInputError(
            f"telemetry file is not valid JSON: {display_path(resolved)} "
            f"({exc.msg} at {exc.lineno}:{exc.colno})"
        ) from exc
    if not isinstance(payload, dict):
        raise TelemetryInputError(
            f"telemetry root must be an object: {display_path(resolved)}"
        )
    return payload


def evaluate_contract(
    payload: dict[str, object],
    *,
    max_regression_pct: float,
    max_jitter_ms: float,
) -> tuple[dict[str, object], list[dict[str, str]]]:
    violations: list[dict[str, str]] = []
    observed: dict[str, object] = {}

    if payload.get("schema_version") != SCHEMA_VERSION:
        add_violation(
            violations,
            "TEL-001",
            (
                "telemetry.schema_version drift: "
                f"expected={SCHEMA_VERSION!r} observed={payload.get('schema_version')!r}"
            ),
        )

    baseline = require_object(payload, "baseline", violations, "TEL-002")
    candidate = require_object(payload, "candidate", violations, "TEL-003")
    samples = payload.get("samples")
    if not isinstance(samples, list) or len(samples) == 0:
        add_violation(
            violations,
            "TEL-004",
            "telemetry.samples must be a non-empty list",
        )
        samples = []

    baseline_elapsed_ms: float | None = None
    candidate_elapsed_ms: float | None = None
    if baseline is not None:
        baseline_elapsed_ms = parse_positive_number(baseline.get("elapsed_ms"))
        if baseline_elapsed_ms is None:
            add_violation(
                violations,
                "TEL-005",
                "telemetry.baseline.elapsed_ms must be a positive number",
            )
    if candidate is not None:
        candidate_elapsed_ms = parse_positive_number(candidate.get("elapsed_ms"))
        if candidate_elapsed_ms is None:
            add_violation(
                violations,
                "TEL-006",
                "telemetry.candidate.elapsed_ms must be a positive number",
            )

    max_allowed_elapsed_ms: float | None = None
    if baseline_elapsed_ms is not None and candidate_elapsed_ms is not None:
        max_allowed_elapsed_ms = baseline_elapsed_ms * (1.0 + (max_regression_pct / 100.0))
        regression_pct = ((candidate_elapsed_ms - baseline_elapsed_ms) / baseline_elapsed_ms) * 100.0
        observed["overall_regression_pct"] = round(regression_pct, 6)
        if candidate_elapsed_ms > max_allowed_elapsed_ms:
            add_violation(
                violations,
                "PERF-001",
                (
                    "overall elapsed regression exceeds limit: "
                    f"candidate={candidate_elapsed_ms:.6f}ms "
                    f"allowed={max_allowed_elapsed_ms:.6f}ms "
                    f"limit_pct={max_regression_pct:.6f}"
                ),
            )

    determinism = None
    if candidate is not None:
        determinism = candidate.get("determinism")
        if not isinstance(determinism, dict):
            add_violation(
                violations,
                "TEL-007",
                "telemetry.candidate.determinism must be an object",
            )
            determinism = None

    if determinism is not None:
        run_a_elapsed_ms = parse_positive_number(determinism.get("run_a_elapsed_ms"))
        run_b_elapsed_ms = parse_positive_number(determinism.get("run_b_elapsed_ms"))
        run_a_hash = parse_sha256(determinism.get("run_a_artifact_sha256"))
        run_b_hash = parse_sha256(determinism.get("run_b_artifact_sha256"))
        run_b_cache_hit = parse_bool(determinism.get("run_b_cache_hit"))

        if run_a_elapsed_ms is None:
            add_violation(
                violations,
                "TEL-008",
                "telemetry.candidate.determinism.run_a_elapsed_ms must be a positive number",
            )
        if run_b_elapsed_ms is None:
            add_violation(
                violations,
                "TEL-009",
                "telemetry.candidate.determinism.run_b_elapsed_ms must be a positive number",
            )
        if run_a_hash is None:
            add_violation(
                violations,
                "TEL-010",
                "telemetry.candidate.determinism.run_a_artifact_sha256 must be lowercase sha256",
            )
        if run_b_hash is None:
            add_violation(
                violations,
                "TEL-011",
                "telemetry.candidate.determinism.run_b_artifact_sha256 must be lowercase sha256",
            )
        if run_b_cache_hit is None:
            add_violation(
                violations,
                "TEL-012",
                "telemetry.candidate.determinism.run_b_cache_hit must be boolean",
            )

        if run_a_hash is not None and run_b_hash is not None and run_a_hash != run_b_hash:
            add_violation(
                violations,
                "DET-001",
                "artifact hash drift between determinism run_a and run_b",
            )
        if run_b_cache_hit is False:
            add_violation(
                violations,
                "DET-002",
                "determinism run_b must report cache_hit=true",
            )
        if run_a_elapsed_ms is not None and run_b_elapsed_ms is not None:
            jitter_ms = abs(run_a_elapsed_ms - run_b_elapsed_ms)
            observed["determinism_jitter_ms"] = round(jitter_ms, 6)
            if jitter_ms > max_jitter_ms:
                add_violation(
                    violations,
                    "DET-003",
                    (
                        "determinism jitter exceeds limit: "
                        f"jitter_ms={jitter_ms:.6f} max_jitter_ms={max_jitter_ms:.6f}"
                    ),
                )

    valid_sample_rows = 0
    worst_fixture_regression_pct: float | None = None
    fixture_names: list[str] = []
    for index, sample in enumerate(samples):
        if not isinstance(sample, dict):
            add_violation(
                violations,
                "TEL-013",
                f"telemetry.samples[{index}] must be an object",
            )
            continue

        fixture = parse_canonical_fixture(sample.get("fixture"))
        baseline_sample = parse_positive_number(sample.get("baseline_elapsed_ms"))
        candidate_sample = parse_positive_number(sample.get("candidate_elapsed_ms"))

        if fixture is None:
            add_violation(
                violations,
                "TEL-014",
                f"telemetry.samples[{index}].fixture must be a canonical fixture path",
            )
            continue
        if baseline_sample is None:
            add_violation(
                violations,
                "TEL-015",
                f"telemetry.samples[{index}].baseline_elapsed_ms must be a positive number",
            )
            continue
        if candidate_sample is None:
            add_violation(
                violations,
                "TEL-016",
                f"telemetry.samples[{index}].candidate_elapsed_ms must be a positive number",
            )
            continue

        fixture_names.append(fixture)
        valid_sample_rows += 1
        sample_regression_pct = ((candidate_sample - baseline_sample) / baseline_sample) * 100.0
        if worst_fixture_regression_pct is None or sample_regression_pct > worst_fixture_regression_pct:
            worst_fixture_regression_pct = sample_regression_pct

        max_sample_elapsed = baseline_sample * (1.0 + (max_regression_pct / 100.0))
        if candidate_sample > max_sample_elapsed:
            add_violation(
                violations,
                "PERF-002",
                (
                    f"{fixture} regression exceeds limit: "
                    f"candidate={candidate_sample:.6f}ms "
                    f"allowed={max_sample_elapsed:.6f}ms "
                    f"limit_pct={max_regression_pct:.6f}"
                ),
            )

    if fixture_names:
        if fixture_names != sorted(fixture_names):
            add_violation(
                violations,
                "TEL-017",
                "telemetry.samples fixtures must be sorted lexicographically",
            )
        if len(set(fixture_names)) != len(fixture_names):
            add_violation(
                violations,
                "TEL-018",
                "telemetry.samples fixtures must be unique",
            )

    observed["sample_count"] = valid_sample_rows
    if baseline_elapsed_ms is not None:
        observed["baseline_elapsed_ms"] = round(baseline_elapsed_ms, 6)
    if candidate_elapsed_ms is not None:
        observed["candidate_elapsed_ms"] = round(candidate_elapsed_ms, 6)
    if max_allowed_elapsed_ms is not None:
        observed["max_allowed_elapsed_ms"] = round(max_allowed_elapsed_ms, 6)
    if worst_fixture_regression_pct is not None:
        observed["worst_fixture_regression_pct"] = round(worst_fixture_regression_pct, 6)

    violations = sorted(violations, key=lambda item: (item["check_id"], item["detail"]))
    status = "PASS" if not violations else "FAIL"
    summary = {
        "mode": MODE,
        "limits": {
            "max_regression_pct": max_regression_pct,
            "max_jitter_ms": max_jitter_ms,
        },
        "observed": observed,
        "status": status,
        "violation_count": len(violations),
        "violations": violations,
    }
    return summary, violations


def render_human_fail(summary: dict[str, object]) -> str:
    violations = summary["violations"]
    assert isinstance(violations, list)
    lines = [
        "status: FAIL",
        f"mode: {summary['mode']}",
        f"violation_count: {summary['violation_count']}",
        "violations:",
    ]
    for item in violations:
        assert isinstance(item, dict)
        lines.append(f"- [{item['check_id']}] {item['detail']}")
    return "\n".join(lines) + "\n"


def render_human_pass(summary: dict[str, object], telemetry_path: Path) -> str:
    observed = summary["observed"]
    assert isinstance(observed, dict)
    return (
        "status: PASS\n"
        f"mode: {summary['mode']}\n"
        f"telemetry: {display_path(telemetry_path)}\n"
        f"overall_regression_pct: {observed.get('overall_regression_pct', 'n/a')}\n"
        f"worst_fixture_regression_pct: {observed.get('worst_fixture_regression_pct', 'n/a')}\n"
        f"determinism_jitter_ms: {observed.get('determinism_jitter_ms', 'n/a')}\n"
    )


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    if args.max_regression_pct <= 0:
        print(
            "objc3c-refactor-perf-guard: error: --max-regression-pct must be > 0",
            file=sys.stderr,
        )
        return EXIT_INPUT_ERROR
    if args.max_jitter_ms <= 0:
        print(
            "objc3c-refactor-perf-guard: error: --max-jitter-ms must be > 0",
            file=sys.stderr,
        )
        return EXIT_INPUT_ERROR

    try:
        telemetry_payload = load_telemetry(args.telemetry)
    except TelemetryInputError as exc:
        print(f"objc3c-refactor-perf-guard: error: {exc}", file=sys.stderr)
        return EXIT_INPUT_ERROR

    summary, violations = evaluate_contract(
        telemetry_payload,
        max_regression_pct=float(args.max_regression_pct),
        max_jitter_ms=float(args.max_jitter_ms),
    )
    summary["telemetry"] = display_path(args.telemetry.resolve())
    summary["contract_mode"] = bool(args.contract_mode)

    if args.contract_mode:
        print(canonical_json_text(summary), end="")
    elif violations:
        print(render_human_fail(summary), file=sys.stderr, end="")
    else:
        print(render_human_pass(summary, args.telemetry), end="")

    return EXIT_OK if not violations else EXIT_GUARD_FAIL


if __name__ == "__main__":
    raise SystemExit(main())
