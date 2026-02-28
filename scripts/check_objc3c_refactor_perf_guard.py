#!/usr/bin/env python3
"""Validate perf and determinism telemetry for objc3c refactor guardrails."""

from __future__ import annotations

import argparse
import json
import os
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MODE = "objc3c-refactor-perf-guard-v1"
SCHEMA_VERSION = "objc3c-refactor-perf-telemetry.v1"
STRICT_MODE = "strict"
DEFAULT_MODE = "default"

DEFAULT_MAX_REGRESSION_PCT = 10.0
DEFAULT_MAX_JITTER_MS = 8.0
DEFAULT_MAX_MEMORY_REGRESSION_PCT = 10.0
STRICT_MAX_REGRESSION_PCT = 5.0
STRICT_MAX_JITTER_MS = 4.0
STRICT_MAX_MEMORY_REGRESSION_PCT = 5.0

DEFAULT_STRICT_TELEMETRY = ROOT / "tmp" / "objc3c_refactor_perf_guard_validation.json"
DEFAULT_STRICT_EVIDENCE = (
    ROOT / "tmp" / "artifacts" / "objc3c_refactor_perf_guard" / "strict_summary.json"
)

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
        default=None,
        help=(
            "Path to telemetry JSON payload. In --strict mode this may be omitted "
            "and defaults to OBJC3C_REFACTOR_PERF_TELEMETRY or "
            "tmp/objc3c_refactor_perf_guard_validation.json."
        ),
    )
    parser.add_argument(
        "--strict",
        action="store_true",
        help=(
            "Enable strict extraction-stage guardrails. Tightens thresholds, "
            "requires memory telemetry, enables contract JSON output, and emits "
            "CI-friendly evidence by default."
        ),
    )
    parser.add_argument(
        "--max-regression-pct",
        type=float,
        default=None,
        help=(
            "Allowed slowdown percentage for overall/per-fixture checks. "
            "Defaults: 10.0 (default mode), 5.0 (strict mode)."
        ),
    )
    parser.add_argument(
        "--max-jitter-ms",
        type=float,
        default=None,
        help=(
            "Allowed jitter between determinism run A/B elapsed timings. "
            "Defaults: 8.0 (default mode), 4.0 (strict mode)."
        ),
    )
    parser.add_argument(
        "--max-memory-regression-pct",
        type=float,
        default=None,
        help=(
            "Allowed candidate peak RSS increase percentage versus baseline. "
            "Defaults: 10.0 (default mode), 5.0 (strict mode)."
        ),
    )
    parser.add_argument(
        "--contract-mode",
        action="store_true",
        help="Emit deterministic JSON summary to stdout for CI/local contracts.",
    )
    parser.add_argument(
        "--evidence-output",
        type=Path,
        default=None,
        help=(
            "Optional path to write deterministic JSON evidence summary. "
            "In --strict mode this defaults to tmp/artifacts/objc3c_refactor_perf_guard/strict_summary.json."
        ),
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


def resolve_limit(
    value: float | None,
    *,
    default_value: float,
    strict_value: float,
    strict_mode: bool,
    option_name: str,
) -> float:
    resolved = strict_value if strict_mode else default_value
    if value is not None:
        resolved = float(value)
    if resolved <= 0:
        raise TelemetryInputError(f"{option_name} must be > 0")
    if strict_mode and resolved > strict_value:
        raise TelemetryInputError(
            f"{option_name} must be <= {strict_value:.6f} in --strict mode"
        )
    return resolved


def resolve_telemetry_path(
    *,
    telemetry_arg: Path | None,
    strict_mode: bool,
) -> Path:
    if telemetry_arg is not None:
        return telemetry_arg
    if strict_mode:
        telemetry_env = os.environ.get("OBJC3C_REFACTOR_PERF_TELEMETRY")
        if telemetry_env and telemetry_env.strip():
            return Path(telemetry_env.strip())
        return DEFAULT_STRICT_TELEMETRY
    raise TelemetryInputError("--telemetry is required unless --strict is set")


def resolve_evidence_output_path(
    *,
    evidence_arg: Path | None,
    strict_mode: bool,
) -> Path | None:
    if evidence_arg is not None:
        return evidence_arg
    if strict_mode:
        return DEFAULT_STRICT_EVIDENCE
    return None


def write_evidence_summary(path: Path, summary: dict[str, object]) -> None:
    resolved = path.resolve()
    try:
        resolved.parent.mkdir(parents=True, exist_ok=True)
        resolved.write_text(canonical_json_text(summary), encoding="utf-8")
    except OSError as exc:
        raise TelemetryInputError(
            f"unable to write evidence output {display_path(resolved)}: {exc}"
        ) from exc


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
    max_memory_regression_pct: float,
    strict_mode: bool,
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
    baseline_peak_rss_mb: float | None = None
    candidate_peak_rss_mb: float | None = None
    if baseline is not None:
        baseline_elapsed_ms = parse_positive_number(baseline.get("elapsed_ms"))
        if baseline_elapsed_ms is None:
            add_violation(
                violations,
                "TEL-005",
                "telemetry.baseline.elapsed_ms must be a positive number",
            )
        baseline_peak_rss_mb = parse_positive_number(baseline.get("peak_rss_mb"))
        if strict_mode and baseline_peak_rss_mb is None:
            add_violation(
                violations,
                "TEL-019",
                "telemetry.baseline.peak_rss_mb must be a positive number in strict mode",
            )
    if candidate is not None:
        candidate_elapsed_ms = parse_positive_number(candidate.get("elapsed_ms"))
        if candidate_elapsed_ms is None:
            add_violation(
                violations,
                "TEL-006",
                "telemetry.candidate.elapsed_ms must be a positive number",
            )
        candidate_peak_rss_mb = parse_positive_number(candidate.get("peak_rss_mb"))
        if strict_mode and candidate_peak_rss_mb is None:
            add_violation(
                violations,
                "TEL-020",
                "telemetry.candidate.peak_rss_mb must be a positive number in strict mode",
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

    max_allowed_peak_rss_mb: float | None = None
    if baseline_peak_rss_mb is not None and candidate_peak_rss_mb is not None:
        max_allowed_peak_rss_mb = baseline_peak_rss_mb * (
            1.0 + (max_memory_regression_pct / 100.0)
        )
        memory_regression_pct = (
            (candidate_peak_rss_mb - baseline_peak_rss_mb) / baseline_peak_rss_mb
        ) * 100.0
        observed["memory_regression_pct"] = round(memory_regression_pct, 6)
        if candidate_peak_rss_mb > max_allowed_peak_rss_mb:
            add_violation(
                violations,
                "PERF-003",
                (
                    "candidate peak_rss regression exceeds limit: "
                    f"candidate={candidate_peak_rss_mb:.6f}MB "
                    f"allowed={max_allowed_peak_rss_mb:.6f}MB "
                    f"limit_pct={max_memory_regression_pct:.6f}"
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
    if baseline_peak_rss_mb is not None:
        observed["baseline_peak_rss_mb"] = round(baseline_peak_rss_mb, 6)
    if candidate_peak_rss_mb is not None:
        observed["candidate_peak_rss_mb"] = round(candidate_peak_rss_mb, 6)
    if max_allowed_peak_rss_mb is not None:
        observed["max_allowed_peak_rss_mb"] = round(max_allowed_peak_rss_mb, 6)
    if worst_fixture_regression_pct is not None:
        observed["worst_fixture_regression_pct"] = round(worst_fixture_regression_pct, 6)

    violations = sorted(violations, key=lambda item: (item["check_id"], item["detail"]))
    status = "PASS" if not violations else "FAIL"
    summary = {
        "mode": MODE,
        "limits": {
            "max_regression_pct": max_regression_pct,
            "max_jitter_ms": max_jitter_ms,
            "max_memory_regression_pct": max_memory_regression_pct,
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
        f"profile: {summary.get('profile', DEFAULT_MODE)}",
        f"violation_count: {summary['violation_count']}",
        "violations:",
    ]
    for item in violations:
        assert isinstance(item, dict)
        lines.append(f"- [{item['check_id']}] {item['detail']}")
    evidence_output = summary.get("evidence_output")
    if isinstance(evidence_output, str):
        lines.append(f"evidence_output: {evidence_output}")
    return "\n".join(lines) + "\n"


def render_human_pass(summary: dict[str, object], telemetry_path: Path) -> str:
    observed = summary["observed"]
    assert isinstance(observed, dict)
    return (
        "status: PASS\n"
        f"mode: {summary['mode']}\n"
        f"profile: {summary.get('profile', DEFAULT_MODE)}\n"
        f"telemetry: {display_path(telemetry_path)}\n"
        f"overall_regression_pct: {observed.get('overall_regression_pct', 'n/a')}\n"
        f"worst_fixture_regression_pct: {observed.get('worst_fixture_regression_pct', 'n/a')}\n"
        f"memory_regression_pct: {observed.get('memory_regression_pct', 'n/a')}\n"
        f"determinism_jitter_ms: {observed.get('determinism_jitter_ms', 'n/a')}\n"
        f"evidence_output: {summary.get('evidence_output', 'n/a')}\n"
    )


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    try:
        max_regression_pct = resolve_limit(
            args.max_regression_pct,
            default_value=DEFAULT_MAX_REGRESSION_PCT,
            strict_value=STRICT_MAX_REGRESSION_PCT,
            strict_mode=bool(args.strict),
            option_name="--max-regression-pct",
        )
        max_jitter_ms = resolve_limit(
            args.max_jitter_ms,
            default_value=DEFAULT_MAX_JITTER_MS,
            strict_value=STRICT_MAX_JITTER_MS,
            strict_mode=bool(args.strict),
            option_name="--max-jitter-ms",
        )
        max_memory_regression_pct = resolve_limit(
            args.max_memory_regression_pct,
            default_value=DEFAULT_MAX_MEMORY_REGRESSION_PCT,
            strict_value=STRICT_MAX_MEMORY_REGRESSION_PCT,
            strict_mode=bool(args.strict),
            option_name="--max-memory-regression-pct",
        )
        telemetry_path = resolve_telemetry_path(
            telemetry_arg=args.telemetry,
            strict_mode=bool(args.strict),
        )
        evidence_output_path = resolve_evidence_output_path(
            evidence_arg=args.evidence_output,
            strict_mode=bool(args.strict),
        )
        contract_mode = bool(args.contract_mode or args.strict)
    except TelemetryInputError as exc:
        print(f"objc3c-refactor-perf-guard: error: {exc}", file=sys.stderr)
        return EXIT_INPUT_ERROR

    try:
        telemetry_payload = load_telemetry(telemetry_path)
    except TelemetryInputError as exc:
        print(f"objc3c-refactor-perf-guard: error: {exc}", file=sys.stderr)
        return EXIT_INPUT_ERROR

    summary, violations = evaluate_contract(
        telemetry_payload,
        max_regression_pct=max_regression_pct,
        max_jitter_ms=max_jitter_ms,
        max_memory_regression_pct=max_memory_regression_pct,
        strict_mode=bool(args.strict),
    )
    summary["profile"] = STRICT_MODE if args.strict else DEFAULT_MODE
    summary["strict_mode"] = bool(args.strict)
    summary["telemetry"] = display_path(telemetry_path.resolve())
    summary["contract_mode"] = contract_mode
    if evidence_output_path is not None:
        summary["evidence_output"] = display_path(evidence_output_path.resolve())

    if evidence_output_path is not None:
        try:
            write_evidence_summary(evidence_output_path, summary)
        except TelemetryInputError as exc:
            print(f"objc3c-refactor-perf-guard: error: {exc}", file=sys.stderr)
            return EXIT_INPUT_ERROR

    if contract_mode:
        print(canonical_json_text(summary), end="")
    elif violations:
        print(render_human_fail(summary), file=sys.stderr, end="")
    else:
        print(render_human_pass(summary, telemetry_path), end="")

    return EXIT_OK if not violations else EXIT_GUARD_FAIL


if __name__ == "__main__":
    raise SystemExit(main())
