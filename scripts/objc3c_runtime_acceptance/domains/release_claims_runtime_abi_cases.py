"""Release Claims runtime ABI acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path

from ..expectation_matching import expect
from ..case_result import CaseResult
from ..fixture_compilation import compile_fixture_with_args
from ..paths import ROOT
from ..probes import compile_probe, parse_key_value_output, run_probe
from ..c_api import (
    PRIVATE_RELEASE_CANDIDATE_CLAIM_RUNTIME_ABI_BOUNDARY,
    PUBLIC_RUNTIME_ABI_BOUNDARY,
    RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
    RUNTIME_PUBLIC_HEADER_PATH,
)
from ..runtime_contract_release import (
    RELEASE_CANDIDATE_CLAIM_RUNTIME_ABI_PROBE,
    RELEASE_CLAIMABLE_SURFACE_FIXTURE,
    RUNTIME_RELEASE_CANDIDATE_CLAIM_ABI_SURFACE_CONTRACT_ID,
)
from .release_claims_owner_contracts import release_claims_case_summary


def check_release_candidate_runtime_claim_abi_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "release-candidate-runtime-claim-abi"
    fixture = ROOT / Path(RELEASE_CLAIMABLE_SURFACE_FIXTURE)
    compile_dir = case_dir / "compile"
    compile_fixture_with_args(fixture, compile_dir)

    manifest = json.loads((compile_dir / "module.manifest.json").read_text(encoding="utf-8"))
    runtime_claim_abi_surface = manifest.get("runtime_release_candidate_claim_abi_surface")
    expect(
        isinstance(runtime_claim_abi_surface, dict),
        "expected compiled fixture manifest to publish runtime_release_candidate_claim_abi_surface",
    )
    expect(
        runtime_claim_abi_surface.get("contract_id")
        == RUNTIME_RELEASE_CANDIDATE_CLAIM_ABI_SURFACE_CONTRACT_ID,
        "expected compiled fixture manifest to publish the release-candidate claim ABI surface contract",
    )
    expect(
        runtime_claim_abi_surface.get("public_header_path") == RUNTIME_PUBLIC_HEADER_PATH
        and runtime_claim_abi_surface.get("internal_header_path")
        == RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "expected release-candidate claim ABI surface to preserve the runtime header paths",
    )
    expect(
        runtime_claim_abi_surface.get("public_runtime_abi_boundary")
        == PUBLIC_RUNTIME_ABI_BOUNDARY,
        "expected release-candidate claim ABI surface to preserve the public runtime ABI boundary",
    )
    expect(
        runtime_claim_abi_surface.get("private_release_candidate_claim_testing_boundary")
        == PRIVATE_RELEASE_CANDIDATE_CLAIM_RUNTIME_ABI_BOUNDARY,
        "expected release-candidate claim ABI surface to preserve the private claim snapshot boundary",
    )
    expect(
        runtime_claim_abi_surface.get("release_candidate_claim_snapshot_symbol")
        == "objc3_runtime_copy_release_candidate_claim_snapshot_for_testing"
        and runtime_claim_abi_surface.get("release_candidate_claim_snapshot_type")
        == "objc3_runtime_release_candidate_claim_snapshot",
        "expected release-candidate claim ABI surface to publish the runtime snapshot symbol and type",
    )
    expect(
        runtime_claim_abi_surface.get("claimed_profile_ids")
        == ["core", "strict", "strict-concurrency", "strict-system"]
        and runtime_claim_abi_surface.get("targeted_profile_ids")
        == ["strict", "strict-concurrency", "strict-system"],
        "expected release-candidate claim ABI surface to publish the claimed and targeted profile sets",
    )
    expect(
        runtime_claim_abi_surface.get("authoritative_probe_paths")
        == [RELEASE_CANDIDATE_CLAIM_RUNTIME_ABI_PROBE],
        "expected release-candidate claim ABI surface to publish the authoritative runtime probe path",
    )

    probe = ROOT / Path(RELEASE_CANDIDATE_CLAIM_RUNTIME_ABI_PROBE)
    exe_path = case_dir / "release_candidate_claim_runtime_probe.exe"
    compile_probe(clangxx, probe, exe_path, [])
    payload = parse_key_value_output(
        run_probe(exe_path), "release-candidate runtime claim ABI probe"
    )
    expect(
        payload.get("copy_status") == 0
        and payload.get("claim_bundle_ready") == 1
        and payload.get("deterministic") == 1,
        "expected release-candidate runtime claim ABI probe to publish a ready deterministic snapshot",
    )
    expect(
        payload.get("selected_profile") == "core"
        and payload.get("claimed_profile_ids_csv")
        == "core,strict,strict-concurrency,strict-system"
        and payload.get("targeted_profile_ids_csv")
        == "strict,strict-concurrency,strict-system",
        "expected release-candidate runtime claim ABI probe to publish the live selected, claimed, and targeted profile sets",
    )
    expect(
        payload.get("conformance_publication_contract_id")
        == "objc3c.driver.conformance.report.publication.v1"
        and payload.get("conformance_claim_operations_contract_id")
        == "objc3c.toolchain.conformance.claim.operations.v1"
        and payload.get("release_evidence_operation_contract_id")
        == "objc3c.tooling.release.evidence.toolchain.operations.v1"
        and payload.get("dashboard_status_publication_contract_id")
        == "objc3c.tooling.dashboard.status.publication.v1"
        and payload.get("release_candidate_matrix_contract_id")
        == "objc3c.tooling.release.candidate.execution.matrix.v1",
        "expected release-candidate runtime claim ABI probe to preserve the live publication contract set",
    )
    expect(
        payload.get("dashboard_schema_path")
        == "schemas/objc3-conformance-dashboard-status-v1.schema.json"
        and payload.get("gate_script_path") == "scripts/check_release_evidence.py"
        and payload.get("runbook_reference_path")
        == "spec/conformance/release_evidence_gate_maintenance.md",
        "expected release-candidate runtime claim ABI probe to preserve the live dashboard schema and release evidence operator paths",
    )

    return CaseResult(
        case_id="release-candidate-runtime-claim-abi",
        probe=RELEASE_CANDIDATE_CLAIM_RUNTIME_ABI_PROBE,
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="linked-runtime-probe",
        passed=True,
        summary=release_claims_case_summary(
            "release-candidate-runtime-claim-abi",
            {
                "selected_profile": payload.get("selected_profile"),
                "claimed_profile_ids_csv": payload.get("claimed_profile_ids_csv"),
                "targeted_profile_ids_csv": payload.get("targeted_profile_ids_csv"),
            },
        ),
    )


__all__ = [
    "check_release_candidate_runtime_claim_abi_case",
]
