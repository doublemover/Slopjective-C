"""Release Claims final release evidence runtime acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path

from ..expectation_matching import expect
from ..case_result import CaseResult
from ..fixture_compilation import compile_fixture_with_args
from ..paths import NATIVE_EXE, ROOT
from ..process_execution import run
from ..probes import compile_probe, parse_key_value_output, run_probe
from .release_claims_owner_contracts import release_claims_case_summary
from .release_claims_runtime_evidence_assertions import (
    CURRENT_RELEASE_EVIDENCE_MANIFEST_SURFACE_KEY,
    expect_current_release_evidence_owner_payload_surface,
    expect_final_release_probe_payload,
    expect_final_release_validate_artifacts,
)
from ..runtime_contract_release import (
    RELEASE_CANDIDATE_EVIDENCE_RUNTIME_PROBE,
    RELEASE_CLAIMABLE_SURFACE_FIXTURE,
)


CURRENT_RELEASE_EVIDENCE_OWNER_PAYLOAD_CASE_ID = "current-release-evidence-owner-payload"


def check_current_release_evidence_owner_payload_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / CURRENT_RELEASE_EVIDENCE_OWNER_PAYLOAD_CASE_ID
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
        "expected conformance validation to succeed for the current release evidence owner payload case",
    )

    manifest = json.loads((compile_dir / "module.manifest.json").read_text(encoding="utf-8"))
    owner_payload_surface = manifest.get(CURRENT_RELEASE_EVIDENCE_MANIFEST_SURFACE_KEY)
    expect_current_release_evidence_owner_payload_surface(owner_payload_surface)

    validate_artifacts = sorted(path.name for path in validate_dir.glob("module.objc3-*.json"))
    expect_final_release_validate_artifacts(validate_artifacts)

    probe = ROOT / Path(RELEASE_CANDIDATE_EVIDENCE_RUNTIME_PROBE)
    exe_path = case_dir / "release_candidate_evidence_runtime_probe.exe"
    compile_probe(clangxx, probe, exe_path, [])
    payload = parse_key_value_output(
        run_probe(exe_path), "release-candidate evidence runtime probe"
    )
    expect_final_release_probe_payload(payload)

    return CaseResult(
        case_id=CURRENT_RELEASE_EVIDENCE_OWNER_PAYLOAD_CASE_ID,
        probe=RELEASE_CANDIDATE_EVIDENCE_RUNTIME_PROBE,
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="linked-runtime-probe",
        passed=True,
        summary=release_claims_case_summary(
            CURRENT_RELEASE_EVIDENCE_OWNER_PAYLOAD_CASE_ID,
            {
                "validate_artifacts": validate_artifacts,
                "validation_model": payload.get("validation_model"),
            },
        ),
    )


__all__ = [
    "CURRENT_RELEASE_EVIDENCE_OWNER_PAYLOAD_CASE_ID",
    "check_current_release_evidence_owner_payload_case",
]
