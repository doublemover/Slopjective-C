"""Stress-minimization orchestration and artifact materialization."""

from __future__ import annotations

import argparse
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import repo_rel

from .execution import compile_source
from .fixtures import load_manifest, validate_artifact_surface
from .minimization import reduce_source
from .models import MinCase
from .paths import ROOT
from .reporting import (
    build_failure_summary,
    build_invocation_payload,
    build_reduced_summary,
    build_reducer_plan,
    build_summary_payload,
    emit_result,
    write_json,
)


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
    write_json(failure_dir / "invocation.json", build_invocation_payload(compiler, case, timeout_sec))
    write_json(failure_dir / "failure-summary.json", build_failure_summary(case, baseline))
    write_json(failure_dir / "stable-signature.json", baseline["signature_payload"])

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
    write_json(minimized_dir / "reducer-plan.json", build_reducer_plan(case, attempts))
    write_json(
        minimized_dir / "reduced-summary.json",
        build_reduced_summary(case, original_source, reduced_source, baseline["signature_sha256"]),
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


def run_stress_minimization(args: argparse.Namespace) -> int:
    compiler = args.compiler.resolve()
    if not compiler.is_file():
        raise RuntimeError(f"stress minimization compiler missing: {repo_rel(compiler)}")
    if args.timeout_sec <= 0:
        raise RuntimeError("stress minimization timeout must be > 0")

    manifest_path = args.manifest.resolve()
    artifact_surface_path = args.artifact_surface.resolve()
    cases = load_manifest(manifest_path)
    artifact_surface = validate_artifact_surface(artifact_surface_path)

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

    summary_out = args.summary_out
    payload = build_summary_payload(
        generated_at_utc=datetime.now(timezone.utc).isoformat(),
        manifest_path=manifest_path,
        artifact_surface_path=artifact_surface_path,
        failure_root=failure_root,
        minimized_root=minimized_root,
        artifact_surface=artifact_surface,
        case_summaries=case_summaries,
    )
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    write_json(summary_out, payload)
    emit_result(payload, summary_out, bool(args.contract_mode))
    return 0


__all__ = ["materialize_case", "run_stress_minimization"]
