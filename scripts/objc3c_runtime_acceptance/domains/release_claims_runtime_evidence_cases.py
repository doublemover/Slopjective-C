"""Release Claims final release evidence runtime acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path

from ..case_result import CaseResult
from ..commands import run
from ..probes import compile_probe, parse_key_value_output, run_probe
from ..core import (
    NATIVE_EXE,
    PRIVATE_RELEASE_CANDIDATE_EVIDENCE_RUNTIME_BOUNDARY,
    RELEASE_CANDIDATE_EVIDENCE_RUNTIME_PROBE,
    RELEASE_CLAIMABLE_SURFACE_FIXTURE,
    ROOT,
    RUNTIME_FINAL_RELEASE_EVIDENCE_DESCAFFOLDING_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_RELEASE_CANDIDATE_CLAIM_ABI_SURFACE_CONTRACT_ID,
    compile_fixture_with_args,
    expect,
)


def check_final_release_evidence_descaffolding_implementation_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "final-release-evidence-descaffolding-implementation"
    fixture = ROOT / Path(RELEASE_CLAIMABLE_SURFACE_FIXTURE)
    compile_dir = case_dir / "compile"
    compile_fixture_with_args(fixture, compile_dir)

    report_path = compile_dir / "module.objc3-conformance-report.json"
    validate_dir = case_dir / "validate"
    validate_dir.mkdir(parents=True, exist_ok=True)
    validation = run(
        [
            str(NATIVE_EXE),
            "--validate-objc3-conformance",
            str(report_path),
            "--out-dir",
            str(validate_dir),
            "--emit-prefix",
            "module",
            "--emit-objc3-conformance-format",
            "json",
        ]
    )
    expect(
        validation.returncode == 0,
        "expected conformance validation to succeed for the final release evidence implementation case",
    )

    manifest = json.loads((compile_dir / "module.manifest.json").read_text(encoding="utf-8"))
    implementation_surface = manifest.get(
        "runtime_final_release_evidence_descaffolding_implementation_surface"
    )
    expect(
        isinstance(implementation_surface, dict),
        "expected compiled fixture manifest to publish runtime_final_release_evidence_descaffolding_implementation_surface",
    )
    expect(
        implementation_surface.get("contract_id")
        == RUNTIME_FINAL_RELEASE_EVIDENCE_DESCAFFOLDING_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        "expected compiled fixture manifest to publish the final release evidence descaffolding implementation surface contract",
    )
    expect(
        implementation_surface.get("runtime_release_candidate_claim_abi_surface_contract_id")
        == RUNTIME_RELEASE_CANDIDATE_CLAIM_ABI_SURFACE_CONTRACT_ID,
        "expected final release evidence implementation surface to depend on the release-candidate claim ABI surface",
    )
    expect(
        implementation_surface.get("private_release_candidate_evidence_testing_boundary")
        == PRIVATE_RELEASE_CANDIDATE_EVIDENCE_RUNTIME_BOUNDARY,
        "expected final release evidence implementation surface to preserve the private evidence snapshot boundary",
    )
    expect(
        implementation_surface.get("validation_artifact_name")
        == "module.objc3-conformance-validation.json"
        and implementation_surface.get("release_evidence_operation_artifact_name")
        == "module.objc3-release-evidence-operation.json"
        and implementation_surface.get("dashboard_status_artifact_name")
        == "module.objc3-dashboard-status.json"
        and implementation_surface.get("advanced_feature_gate_artifact_name")
        == "module.objc3-advanced-feature-gate.json"
        and implementation_surface.get("release_candidate_matrix_artifact_name")
        == "module.objc3-release-candidate-matrix.json",
        "expected final release evidence implementation surface to publish the final artifact inventory",
    )

    validate_artifacts = sorted(path.name for path in validate_dir.glob("module.objc3-*.json"))
    expect(
        validate_artifacts
        == [
            "module.objc3-advanced-feature-gate.json",
            "module.objc3-conformance-validation.json",
            "module.objc3-dashboard-status.json",
            "module.objc3-release-candidate-matrix.json",
            "module.objc3-release-evidence-operation.json",
        ],
        "expected validation output to preserve the final release evidence artifact inventory",
    )

    probe = ROOT / Path(RELEASE_CANDIDATE_EVIDENCE_RUNTIME_PROBE)
    exe_path = case_dir / "release_candidate_evidence_runtime_probe.exe"
    compile_probe(clangxx, probe, exe_path, [])
    payload = parse_key_value_output(
        run_probe(exe_path), "release-candidate evidence runtime probe"
    )
    expect(
        payload.get("copy_status") == 0
        and payload.get("validation_artifact_ready") == 1
        and payload.get("release_evidence_operation_ready") == 1
        and payload.get("dashboard_status_ready") == 1
        and payload.get("advanced_feature_gate_ready") == 1
        and payload.get("release_candidate_matrix_ready") == 1
        and payload.get("deprecated_paths_shutdown") == 1
        and payload.get("deterministic") == 1,
        "expected final release evidence runtime probe to publish a ready deterministic implementation snapshot",
    )
    expect(
        payload.get("validation_artifact_name")
        == "module.objc3-conformance-validation.json"
        and payload.get("release_evidence_operation_artifact_name")
        == "module.objc3-release-evidence-operation.json"
        and payload.get("dashboard_status_artifact_name")
        == "module.objc3-dashboard-status.json"
        and payload.get("advanced_feature_gate_artifact_name")
        == "module.objc3-advanced-feature-gate.json"
        and payload.get("release_candidate_matrix_artifact_name")
        == "module.objc3-release-candidate-matrix.json",
        "expected final release evidence runtime probe to preserve the final artifact inventory",
    )

    return CaseResult(
        case_id="final-release-evidence-descaffolding-implementation",
        probe=RELEASE_CANDIDATE_EVIDENCE_RUNTIME_PROBE,
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "validate_artifacts": validate_artifacts,
            "validation_model": payload.get("validation_model"),
        },
    )


__all__ = [
    "check_final_release_evidence_descaffolding_implementation_case",
]
