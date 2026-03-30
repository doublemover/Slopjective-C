#!/usr/bin/env python3
"""Reduce failing checked-in stress seeds into machine-owned replay capsules."""

from __future__ import annotations

import argparse
import hashlib
import json
import subprocess
import sys
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
COMPILER = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
MANIFEST_PATH = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "minimization_manifest.json"
ARTIFACT_SURFACE_PATH = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "artifact_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "stress" / "minimization-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.stress.minimization.summary.v1"


@dataclass(frozen=True)
class MinCase:
    case_id: str
    subsystem: str
    source_path: Path


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"expected JSON object at {repo_rel(path)}")
    return payload


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--compiler", type=Path, default=COMPILER)
    parser.add_argument("--manifest", type=Path, default=MANIFEST_PATH)
    parser.add_argument("--artifact-surface", type=Path, default=ARTIFACT_SURFACE_PATH)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    parser.add_argument("--timeout-sec", type=float, default=5.0)
    parser.add_argument("--contract-mode", action="store_true")
    return parser.parse_args(argv)


def load_manifest(path: Path) -> list[MinCase]:
    payload = load_json(path)
    if payload.get("contract_id") != "objc3c.stress.minimization.manifest.v1":
        raise RuntimeError("stress minimization manifest contract_id drifted")
    if payload.get("schema_version") != 1:
        raise RuntimeError("stress minimization manifest schema_version drifted")
    cases = payload.get("cases")
    if not isinstance(cases, list) or not cases:
        raise RuntimeError("stress minimization manifest missing cases")
    selected: list[MinCase] = []
    for item in cases:
        if not isinstance(item, dict):
            raise RuntimeError("stress minimization manifest contains a non-object case")
        case_id = item.get("case_id")
        subsystem = item.get("subsystem")
        source_path = item.get("source_path")
        if not isinstance(case_id, str) or not case_id:
            raise RuntimeError("stress minimization manifest case missing case_id")
        if subsystem not in {"parser", "semantic"}:
            raise RuntimeError(f"stress minimization manifest case {case_id} has invalid subsystem")
        if not isinstance(source_path, str) or not source_path:
            raise RuntimeError(f"stress minimization manifest case {case_id} missing source_path")
        resolved = (ROOT / source_path).resolve()
        if not resolved.is_file():
            raise RuntimeError(f"stress minimization manifest references missing source {source_path}")
        selected.append(MinCase(case_id=case_id, subsystem=subsystem, source_path=resolved))
    return selected


def validate_artifact_surface(path: Path) -> dict[str, Any]:
    payload = load_json(path)
    if payload.get("contract_id") != "objc3c.stress.artifact.surface.v1":
        raise RuntimeError("stress artifact surface contract_id drifted")
    if payload.get("schema_version") != 1:
        raise RuntimeError("stress artifact surface schema_version drifted")
    return payload


def compile_source(
    compiler: Path,
    source_text: str,
    work_dir: Path,
    timeout_sec: float,
) -> dict[str, Any]:
    work_dir.mkdir(parents=True, exist_ok=True)
    source_path = work_dir / "candidate.objc3"
    out_dir = work_dir / "out"
    source_path.write_text(source_text, encoding="utf-8")
    completed = subprocess.run(
        [
            str(compiler),
            str(source_path),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
        ],
        cwd=ROOT,
        check=False,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        timeout=timeout_sec,
    )
    stdout_text = completed.stdout.replace("\r\n", "\n")
    stderr_text = completed.stderr.replace("\r\n", "\n")
    combined_output = f"{stdout_text}\n{stderr_text}"
    diagnostic_lines = [
        line.strip()
        for line in combined_output.splitlines()
        if line.strip() and ("error" in line.lower() or "o3" in line.lower())
    ]
    if not diagnostic_lines:
        diagnostic_lines = [line.strip() for line in combined_output.splitlines() if line.strip()][:5]
    if not diagnostic_lines:
        diagnostic_lines = [f"returncode:{completed.returncode}"]
    signature_payload = {
        "returncode": completed.returncode,
        "diagnostic_lines": diagnostic_lines,
    }
    signature_text = json.dumps(signature_payload, sort_keys=True)
    return {
        "returncode": completed.returncode,
        "stdout": stdout_text,
        "stderr": stderr_text,
        "diagnostic_lines": diagnostic_lines,
        "signature_sha256": hashlib.sha256(signature_text.encode("utf-8")).hexdigest(),
        "signature_payload": signature_payload,
    }


def reduce_source(
    compiler: Path,
    case: MinCase,
    original_source: str,
    baseline_signature: str,
    reduced_dir: Path,
    timeout_sec: float,
) -> tuple[str, list[dict[str, Any]]]:
    candidate = original_source
    attempts: list[dict[str, Any]] = []
    changed = True
    while changed:
        changed = False
        lines = candidate.splitlines(keepends=True)
        if len(lines) <= 1:
            break
        for index in range(len(lines)):
            trial_lines = lines[:index] + lines[index + 1 :]
            trial_source = "".join(trial_lines)
            if not trial_source.strip():
                continue
            attempt_dir = reduced_dir / "attempts" / f"{len(attempts):03d}"
            result = compile_source(compiler, trial_source, attempt_dir, timeout_sec)
            accepted = result["signature_sha256"] == baseline_signature and len(trial_source) < len(candidate)
            attempts.append(
                {
                    "attempt_index": len(attempts),
                    "removed_line_index": index,
                    "accepted": accepted,
                    "signature_sha256": result["signature_sha256"],
                    "returncode": result["returncode"],
                    "diagnostic_line_count": len(result["diagnostic_lines"]),
                }
            )
            if accepted:
                candidate = trial_source
                changed = True
                break
    return candidate, attempts


def materialize_case(
    compiler: Path,
    case: MinCase,
    failure_root: Path,
    minimized_root: Path,
    timeout_sec: float,
) -> dict[str, Any]:
    original_source = case.source_path.read_text(encoding="utf-8").replace("\r\n", "\n")
    failure_dir = failure_root / case.case_id
    minimized_dir = minimized_root / case.case_id
    baseline = compile_source(compiler, original_source, failure_dir / "baseline", timeout_sec)
    if baseline["returncode"] == 0:
        raise RuntimeError(f"stress minimization case unexpectedly compiled cleanly: {case.case_id}")
    failure_dir.mkdir(parents=True, exist_ok=True)
    (failure_dir / "source.objc3").write_text(original_source, encoding="utf-8")
    (failure_dir / "invocation.json").write_text(
        json.dumps(
            {
                "compiler": repo_rel(compiler),
                "source_path": repo_rel(case.source_path),
                "timeout_sec": timeout_sec,
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    (failure_dir / "failure-summary.json").write_text(
        json.dumps(
            {
                "case_id": case.case_id,
                "subsystem": case.subsystem,
                "returncode": baseline["returncode"],
                "diagnostic_lines": baseline["diagnostic_lines"],
                "signature_sha256": baseline["signature_sha256"],
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    (failure_dir / "stable-signature.json").write_text(
        json.dumps(baseline["signature_payload"], indent=2) + "\n",
        encoding="utf-8",
    )

    reduced_source, attempts = reduce_source(
        compiler,
        case,
        original_source,
        baseline["signature_sha256"],
        minimized_dir,
        timeout_sec,
    )
    minimized_dir.mkdir(parents=True, exist_ok=True)
    (minimized_dir / "candidate.objc3").write_text(reduced_source, encoding="utf-8")
    (minimized_dir / "reducer-plan.json").write_text(
        json.dumps(
            {
                "case_id": case.case_id,
                "attempt_count": len(attempts),
                "accepted_reduction_count": sum(1 for attempt in attempts if attempt["accepted"]),
                "attempts": attempts,
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    (minimized_dir / "reduced-summary.json").write_text(
        json.dumps(
            {
                "case_id": case.case_id,
                "original_bytes": len(original_source.encode("utf-8")),
                "reduced_bytes": len(reduced_source.encode("utf-8")),
                "original_line_count": len(original_source.splitlines()),
                "reduced_line_count": len(reduced_source.splitlines()),
                "signature_sha256": baseline["signature_sha256"],
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    return {
        "case_id": case.case_id,
        "subsystem": case.subsystem,
        "source_path": repo_rel(case.source_path),
        "failure_dir": repo_rel(failure_dir),
        "minimized_dir": repo_rel(minimized_dir),
        "signature_sha256": baseline["signature_sha256"],
        "attempt_count": len(attempts),
        "accepted_reduction_count": sum(1 for attempt in attempts if attempt["accepted"]),
        "original_bytes": len(original_source.encode("utf-8")),
        "reduced_bytes": len(reduced_source.encode("utf-8")),
    }


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    compiler = args.compiler.resolve()
    if not compiler.is_file():
        raise RuntimeError(f"stress minimization compiler missing: {repo_rel(compiler)}")
    if args.timeout_sec <= 0:
        raise RuntimeError("stress minimization timeout must be > 0")

    cases = load_manifest(args.manifest.resolve())
    artifact_surface = validate_artifact_surface(args.artifact_surface.resolve())

    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    failure_root = ROOT / "tmp" / "artifacts" / "stress" / "failures" / run_id
    minimized_root = ROOT / "tmp" / "artifacts" / "stress" / "minimized" / run_id

    case_summaries = [
        materialize_case(
            compiler,
            case,
            failure_root,
            minimized_root,
            float(args.timeout_sec),
        )
        for case in cases
    ]

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "manifest_path": repo_rel(args.manifest.resolve()),
        "artifact_surface_path": repo_rel(args.artifact_surface.resolve()),
        "failure_root": repo_rel(failure_root),
        "minimized_root": repo_rel(minimized_root),
        "case_count": len(case_summaries),
        "artifact_surface_summary_reports": artifact_surface["summary_reports"],
        "case_summaries": case_summaries,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    if args.contract_mode:
        sys.stdout.write(json.dumps(payload, indent=2) + "\n")
    else:
        print(f"summary_path: {repo_rel(args.summary_out)}")
        print("objc3c-stress-minimization: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
