#!/usr/bin/env python3
"""Fail-closed validator for M250-C002 lowering/runtime modular split scaffolding."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m250-lowering-runtime-stability-invariant-proofs-modular-split-scaffolding-contract-c002-v1"

ARTIFACTS: dict[str, Path] = {
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "scaffold_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_lowering_runtime_stability_invariant_scaffold.h",
    "frontend_pipeline_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "c001_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_lowering_runtime_stability_invariant_proofs_c001_expectations.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_lowering_runtime_stability_invariant_proofs_modular_split_scaffolding_c002_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_c002_lowering_runtime_stability_invariant_proofs_modular_split_scaffolding_packet.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "frontend_types": (
        ("M250-C002-TYP-01", "struct Objc3LoweringRuntimeStabilityInvariantScaffold {"),
        ("M250-C002-TYP-02", "bool invariant_proofs_ready = false;"),
        ("M250-C002-TYP-03", "bool modular_split_ready = false;"),
        (
            "M250-C002-TYP-04",
            "Objc3LoweringRuntimeStabilityInvariantScaffold lowering_runtime_stability_invariant_scaffold;",
        ),
    ),
    "scaffold_header": (
        (
            "M250-C002-SCA-01",
            "inline std::string BuildObjc3LoweringRuntimeStabilityInvariantScaffoldKey(",
        ),
        (
            "M250-C002-SCA-02",
            "inline Objc3LoweringRuntimeStabilityInvariantScaffold BuildObjc3LoweringRuntimeStabilityInvariantScaffold(",
        ),
        ("M250-C002-SCA-03", "scaffold.invariant_proofs_ready ="),
        ("M250-C002-SCA-04", "scaffold.modular_split_ready ="),
        (
            "M250-C002-SCA-05",
            "scaffold.scaffold_key = BuildObjc3LoweringRuntimeStabilityInvariantScaffoldKey(scaffold);",
        ),
        (
            "M250-C002-SCA-06",
            "inline bool IsObjc3LoweringRuntimeStabilityInvariantScaffoldReady(",
        ),
        ("M250-C002-SCA-07", 'scaffold.failure_reason = "runtime dispatch contract is inconsistent";'),
        ("M250-C002-SCA-08", 'scaffold.failure_reason = "lowering/runtime modular split scaffold not ready";'),
    ),
    "frontend_pipeline_source": (
        (
            "M250-C002-PIP-01",
            '#include "pipeline/objc3_lowering_runtime_stability_invariant_scaffold.h"',
        ),
        (
            "M250-C002-PIP-02",
            "result.lowering_runtime_stability_invariant_scaffold =",
        ),
        (
            "M250-C002-PIP-03",
            "BuildObjc3LoweringRuntimeStabilityInvariantScaffold(",
        ),
    ),
    "architecture_doc": (
        (
            "M250-C002-ARCH-01",
            "M250 lane-C C002 modular split scaffolding anchors lowering/runtime stability",
        ),
        (
            "M250-C002-ARCH-02",
            "`pipeline/objc3_lowering_runtime_stability_invariant_scaffold.h`",
        ),
    ),
    "c001_contract_doc": (
        (
            "M250-C002-C001-01",
            "Contract ID: `objc3c-lowering-runtime-stability-invariant-proofs-freeze/m250-c001-v1`",
        ),
    ),
    "contract_doc": (
        (
            "M250-C002-DOC-01",
            "Contract ID: `objc3c-lowering-runtime-stability-invariant-proofs-modular-split-scaffolding/m250-c002-v1`",
        ),
        ("M250-C002-DOC-02", "Objc3LoweringRuntimeStabilityInvariantScaffold"),
        ("M250-C002-DOC-03", "BuildObjc3LoweringRuntimeStabilityInvariantScaffold"),
        (
            "M250-C002-DOC-04",
            "python scripts/check_m250_c002_lowering_runtime_stability_invariant_proofs_modular_split_scaffolding_contract.py",
        ),
        (
            "M250-C002-DOC-05",
            "python -m pytest tests/tooling/test_check_m250_c002_lowering_runtime_stability_invariant_proofs_modular_split_scaffolding_contract.py -q",
        ),
        ("M250-C002-DOC-06", "npm run check:objc3c:m250-c002-lane-c-readiness"),
        (
            "M250-C002-DOC-07",
            "tmp/reports/m250/M250-C002/lowering_runtime_stability_invariant_proofs_modular_split_scaffolding_contract_summary.json",
        ),
    ),
    "packet_doc": (
        ("M250-C002-PKT-01", "Packet: `M250-C002`"),
        ("M250-C002-PKT-02", "Lane: `C`"),
        ("M250-C002-PKT-03", "Dependencies: `M250-C001`"),
        (
            "M250-C002-PKT-04",
            "scripts/check_m250_c002_lowering_runtime_stability_invariant_proofs_modular_split_scaffolding_contract.py",
        ),
        (
            "M250-C002-PKT-05",
            "tests/tooling/test_check_m250_c002_lowering_runtime_stability_invariant_proofs_modular_split_scaffolding_contract.py",
        ),
    ),
    "package_json": (
        (
            "M250-C002-CFG-01",
            '"check:objc3c:m250-c002-lowering-runtime-stability-invariant-proofs-modular-split-scaffolding-contract"',
        ),
        (
            "M250-C002-CFG-02",
            '"test:tooling:m250-c002-lowering-runtime-stability-invariant-proofs-modular-split-scaffolding-contract"',
        ),
        ("M250-C002-CFG-03", '"check:objc3c:m250-c002-lane-c-readiness"'),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M250-C002-FORB-01", "scaffold.invariant_proofs_ready = true;"),
        ("M250-C002-FORB-02", "scaffold.modular_split_ready = true;"),
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
            "tmp/reports/m250/M250-C002/"
            "lowering_runtime_stability_invariant_proofs_modular_split_scaffolding_contract_summary.json"
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
