#!/usr/bin/env python3
"""Fail-closed validator for M226-C002 parse-lowering modular split contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-c002-parse-lowering-modular-split-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "readiness_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_parse_lowering_readiness_surface.h",
    "frontend_types_header": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "pipeline_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp",
    "artifacts_header": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.h",
    "artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "contract_doc": ROOT / "docs" / "contracts" / "m226_parse_lowering_modular_split_c002_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "readiness_surface_header": (
        (
            "M226-C002-MOD-01",
            "inline Objc3ParseLoweringReadinessSurface BuildObjc3ParseLoweringReadinessSurface(",
        ),
        (
            "M226-C002-MOD-02",
            "TryBuildObjc3LoweringIRBoundary(options.lowering, lowering_boundary, lowering_error)",
        ),
        ("M226-C002-MOD-03", "surface.ready_for_lowering = diagnostics_clear &&"),
        (
            "M226-C002-MOD-04",
            "inline bool IsObjc3ParseLoweringReadinessSurfaceReady(const Objc3ParseLoweringReadinessSurface &surface,",
        ),
    ),
    "frontend_types_header": (
        ("M226-C002-TYP-01", "struct Objc3ParseLoweringReadinessSurface {"),
        ("M226-C002-TYP-02", "bool ready_for_lowering = false;"),
        ("M226-C002-TYP-03", "std::string lowering_boundary_replay_key;"),
        ("M226-C002-TYP-04", "Objc3ParseLoweringReadinessSurface parse_lowering_readiness_surface;"),
    ),
    "pipeline_source": (
        ("M226-C002-PIP-01", '#include "pipeline/objc3_parse_lowering_readiness_surface.h"'),
        (
            "M226-C002-PIP-02",
            "result.parse_lowering_readiness_surface = BuildObjc3ParseLoweringReadinessSurface(result, options);",
        ),
    ),
    "artifacts_header": (
        ("M226-C002-ART-H-01", "Objc3ParseLoweringReadinessSurface parse_lowering_readiness_surface;"),
    ),
    "artifacts_source": (
        (
            "M226-C002-ART-01",
            "bundle.parse_lowering_readiness_surface = BuildObjc3ParseLoweringReadinessSurface(pipeline_result, options);",
        ),
        ("M226-C002-ART-02", "IsObjc3ParseLoweringReadinessSurfaceReady(bundle.parse_lowering_readiness_surface,"),
        (
            "M226-C002-ART-03",
            "\"LLVM IR emission failed: parse-to-lowering readiness check failed: \" +",
        ),
        ("M226-C002-ART-04", '\\"parse_lowering_readiness\\": {\\"ready_for_lowering\\": '),
        ("M226-C002-ART-05", '\\"lowering_boundary_replay_key\\":\\"'),
        ("M226-C002-ART-06", '\\"failure_reason\\":\\"'),
    ),
    "contract_doc": (
        (
            "M226-C002-DOC-01",
            "Contract ID: `objc3c-parse-lowering-modular-split-contract/m226-c002-v1`",
        ),
        ("M226-C002-DOC-02", "objc3_parse_lowering_readiness_surface.h"),
        ("M226-C002-DOC-03", "python scripts/check_m226_c002_parse_lowering_modular_split_contract.py"),
        (
            "M226-C002-DOC-04",
            "python -m pytest tests/tooling/test_check_m226_c002_parse_lowering_modular_split_contract.py -q",
        ),
    ),
}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path("tmp/reports/m226/m226_c002_parse_lowering_modular_split_contract_summary.json"),
    )
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    findings: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for artifact, path in ARTIFACTS.items():
        text = load_text(path, artifact=artifact)
        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
            checks_total += 1
            if snippet in text:
                checks_passed += 1
            else:
                findings.append(Finding(artifact, check_id, f"expected snippet missing: {snippet}"))

    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in findings
        ],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if not findings:
        return 0

    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
