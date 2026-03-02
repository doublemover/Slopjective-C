#!/usr/bin/env python3
"""Fail-closed validator for M250-B002 semantic stability modular split scaffolding."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m250-semantic-stability-spec-delta-closure-modular-split-scaffolding-contract-b002-v1"

ARTIFACTS: dict[str, Path] = {
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "scaffold_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_semantic_stability_spec_delta_closure_scaffold.h",
    "frontend_pipeline_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_semantic_stability_spec_delta_closure_modular_split_scaffolding_b002_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_b002_semantic_stability_spec_delta_closure_modular_split_scaffolding_packet.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "frontend_types": (
        ("M250-B002-TYP-01", "struct Objc3SemanticStabilitySpecDeltaClosureScaffold {"),
        ("M250-B002-TYP-02", "bool spec_delta_closed = false;"),
        ("M250-B002-TYP-03", "bool modular_split_ready = false;"),
        (
            "M250-B002-TYP-04",
            "Objc3SemanticStabilitySpecDeltaClosureScaffold semantic_stability_spec_delta_closure_scaffold;",
        ),
    ),
    "scaffold_header": (
        (
            "M250-B002-SCA-01",
            "inline std::string BuildObjc3SemanticStabilitySpecDeltaClosureScaffoldKey(",
        ),
        (
            "M250-B002-SCA-02",
            "inline Objc3SemanticStabilitySpecDeltaClosureScaffold BuildObjc3SemanticStabilitySpecDeltaClosureScaffold(",
        ),
        ("M250-B002-SCA-03", "scaffold.spec_delta_closed ="),
        ("M250-B002-SCA-04", "scaffold.modular_split_ready ="),
        (
            "M250-B002-SCA-05",
            "scaffold.scaffold_key = BuildObjc3SemanticStabilitySpecDeltaClosureScaffoldKey(scaffold);",
        ),
        (
            "M250-B002-SCA-06",
            "inline bool IsObjc3SemanticStabilitySpecDeltaClosureScaffoldReady(",
        ),
        ("M250-B002-SCA-07", 'scaffold.failure_reason = "semantic stability spec delta is not closed";'),
        ("M250-B002-SCA-08", 'scaffold.failure_reason = "semantic stability modular split scaffold not ready";'),
    ),
    "frontend_pipeline_source": (
        (
            "M250-B002-PIP-01",
            '#include "pipeline/objc3_semantic_stability_spec_delta_closure_scaffold.h"',
        ),
        (
            "M250-B002-PIP-02",
            "result.semantic_stability_spec_delta_closure_scaffold =",
        ),
        (
            "M250-B002-PIP-03",
            "BuildObjc3SemanticStabilitySpecDeltaClosureScaffold(",
        ),
    ),
    "architecture_doc": (
        ("M250-B002-ARCH-01", "M250 lane-B B002 modular split scaffolding anchors semantic-stability closure"),
        ("M250-B002-ARCH-02", "`pipeline/objc3_semantic_stability_spec_delta_closure_scaffold.h`"),
    ),
    "contract_doc": (
        (
            "M250-B002-DOC-01",
            "Contract ID: `objc3c-semantic-stability-spec-delta-closure-modular-split-scaffolding/m250-b002-v1`",
        ),
        ("M250-B002-DOC-02", "Objc3SemanticStabilitySpecDeltaClosureScaffold"),
        ("M250-B002-DOC-03", "BuildObjc3SemanticStabilitySpecDeltaClosureScaffold"),
        (
            "M250-B002-DOC-04",
            "python scripts/check_m250_b002_semantic_stability_spec_delta_closure_modular_split_scaffolding_contract.py",
        ),
        (
            "M250-B002-DOC-05",
            "python -m pytest tests/tooling/test_check_m250_b002_semantic_stability_spec_delta_closure_modular_split_scaffolding_contract.py -q",
        ),
        ("M250-B002-DOC-06", "npm run check:objc3c:m250-b002-lane-b-readiness"),
        (
            "M250-B002-DOC-07",
            "tmp/reports/m250/M250-B002/semantic_stability_spec_delta_closure_modular_split_scaffolding_contract_summary.json",
        ),
    ),
    "packet_doc": (
        ("M250-B002-PKT-01", "Packet: `M250-B002`"),
        ("M250-B002-PKT-02", "Lane: `B`"),
        (
            "M250-B002-PKT-03",
            "scripts/check_m250_b002_semantic_stability_spec_delta_closure_modular_split_scaffolding_contract.py",
        ),
        (
            "M250-B002-PKT-04",
            "tests/tooling/test_check_m250_b002_semantic_stability_spec_delta_closure_modular_split_scaffolding_contract.py",
        ),
    ),
    "package_json": (
        (
            "M250-B002-CFG-01",
            '"check:objc3c:m250-b002-semantic-stability-spec-delta-closure-modular-split-scaffolding-contract"',
        ),
        (
            "M250-B002-CFG-02",
            '"test:tooling:m250-b002-semantic-stability-spec-delta-closure-modular-split-scaffolding-contract"',
        ),
        ("M250-B002-CFG-03", '"check:objc3c:m250-b002-lane-b-readiness"'),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M250-B002-FORB-01", "scaffold.spec_delta_closed = true;"),
        ("M250-B002-FORB-02", "scaffold.modular_split_ready = true;"),
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
        default=Path(
            "tmp/reports/m250/M250-B002/"
            "semantic_stability_spec_delta_closure_modular_split_scaffolding_contract_summary.json"
        ),
    )
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    for artifact, path in ARTIFACTS.items():
        text = load_text(path, artifact=artifact)

        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                passed_checks += 1
            else:
                findings.append(Finding(artifact, check_id, f"expected snippet missing: {snippet}"))

        for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                findings.append(Finding(artifact, check_id, f"forbidden snippet present: {snippet}"))
            else:
                passed_checks += 1

    ok = not findings
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": total_checks,
        "checks_passed": passed_checks,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in findings
        ],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if ok:
        return 0
    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
