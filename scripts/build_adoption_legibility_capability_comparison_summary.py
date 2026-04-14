#!/usr/bin/env python3
"""Build the adoption capability comparison semantics summary."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json


ROOT = Path(__file__).resolve().parents[1]
SEMANTICS_PATH = ROOT / "tests" / "tooling" / "fixtures" / "adoption_legibility" / "capability_comparison_semantics.json"
PUBLIC_CLAIM_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "adoption_legibility" / "public_claim_policy.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "adoption-legibility" / "capability-comparison-summary.json"




def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    semantics = load_json(SEMANTICS_PATH)
    public_claim_policy = load_json(PUBLIC_CLAIM_POLICY)
    failures: list[str] = []

    dependencies = [str(path) for path in semantics.get("depends_on", [])]
    for raw_path in dependencies:
        expect((ROOT / raw_path).is_file(), f"missing dependency {raw_path}", failures)
    common_surfaces = [str(path) for path in semantics.get("required_common_surfaces", [])]
    for raw_path in common_surfaces:
        expect((ROOT / raw_path).is_file(), f"missing common comparison surface {raw_path}", failures)

    axes = semantics.get("comparison_axes", [])
    expect(isinstance(axes, list) and len(axes) >= 3, "comparison axes must cover ObjC2, Swift, and C++", failures)
    adjacent_ecosystems = {str(axis.get("adjacent_ecosystem")) for axis in axes if isinstance(axis, dict)}
    expect({"Objective-C 2", "Swift", "C++"}.issubset(adjacent_ecosystems), "required adjacent ecosystem coverage missing", failures)

    forbidden_from_public_policy = {str(claim).lower() for claim in public_claim_policy.get("forbidden_claims", [])}
    runnable_axis_count = 0
    performance_axis_count = 0
    conformance_or_runbook_axis_count = 0
    for axis in axes if isinstance(axes, list) else []:
        if not isinstance(axis, dict):
            failures.append("comparison axis entry must be an object")
            continue
        axis_id = str(axis.get("axis_id"))
        evidence_paths = [str(path) for path in axis.get("required_evidence", [])]
        forbidden_shapes = [str(shape).lower() for shape in axis.get("forbidden_claim_shapes", [])]
        expect(bool(axis.get("allowed_claim_shape")), f"{axis_id} missing allowed claim shape", failures)
        expect(len(evidence_paths) >= 5, f"{axis_id} evidence set is too narrow", failures)
        expect(len(forbidden_shapes) >= 3, f"{axis_id} forbidden claim shapes are too narrow", failures)
        expect(
            any("parity" in shape or "replacement" in shape or "leadership" in shape for shape in forbidden_shapes),
            f"{axis_id} lacks parity/replacement/leadership guardrail",
            failures,
        )
        if any("cross-major" in shape for shape in forbidden_shapes):
            expect(
                any("cross-major" in claim for claim in forbidden_from_public_policy),
                f"{axis_id} cross-major guardrail is not mirrored in public claim policy",
                failures,
            )
        for raw_path in evidence_paths:
            expect((ROOT / raw_path).is_file(), f"{axis_id} references missing evidence path {raw_path}", failures)
        if any(path.endswith(".objc3") for path in evidence_paths):
            runnable_axis_count += 1
        if any("performance" in path or "baseline" in path for path in evidence_paths):
            performance_axis_count += 1
        if any("conformance" in path or "runbooks" in path for path in evidence_paths):
            conformance_or_runbook_axis_count += 1

    required_publication_fields = semantics.get("required_publication_fields", [])
    expect(
        {"axis_id", "adjacent_ecosystem", "allowed_claim_shape", "evidence_paths", "deferred_behavior"}.issubset(
            set(required_publication_fields)
        ),
        "publication fields do not preserve axis, ecosystem, claim shape, evidence, and deferred behavior",
        failures,
    )
    expect(runnable_axis_count == len(axes), "every comparison axis must name a runnable source example", failures)
    expect(performance_axis_count >= 2, "comparison coverage must include performance or baseline evidence", failures)
    expect(conformance_or_runbook_axis_count == len(axes), "every comparison axis must tie back to runbook or conformance evidence", failures)

    payload = {
        "contract_id": "objc3c.adoption_legibility.capability_comparison.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "semantics": repo_rel(SEMANTICS_PATH),
        "public_claim_policy": repo_rel(PUBLIC_CLAIM_POLICY),
        "comparison_axis_count": len(axes) if isinstance(axes, list) else 0,
        "adjacent_ecosystems": sorted(adjacent_ecosystems),
        "runnable_axis_count": runnable_axis_count,
        "performance_axis_count": performance_axis_count,
        "conformance_or_runbook_axis_count": conformance_or_runbook_axis_count,
        "required_common_surfaces": common_surfaces,
        "required_publication_fields": required_publication_fields,
        "fail_closed_conditions": semantics.get("fail_closed_conditions", []),
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("adoption-legibility-capability-comparison: PASS" if not failures else "adoption-legibility-capability-comparison: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
