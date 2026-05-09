"""Release Claims publication and shutdown runtime acceptance cases."""

from __future__ import annotations

import json
import re
from pathlib import Path

from ..case_result import CaseResult
from ..commands import run
from ..core import (
    DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES,
    NATIVE_EXE,
    RELEASE_CLAIMABLE_SURFACE_FIXTURE,
    ROOT,
    compile_fixture_with_args,
    expect,
)


def check_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "scaffold-retirement-deprecated-sidecar-compatibility-diagnostics"
    fixture = ROOT / Path(RELEASE_CLAIMABLE_SURFACE_FIXTURE)

    compile_deprecated_dir = case_dir / "compile-deprecated"
    compile_deprecated_dir.mkdir(parents=True, exist_ok=True)
    for filename in DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES:
        (compile_deprecated_dir / filename).write_text("{}", encoding="utf-8")
    compile_reject = run(
        [
            str(NATIVE_EXE),
            str(fixture),
            "--out-dir",
            str(compile_deprecated_dir),
            "--emit-prefix",
            "module",
        ]
    )
    compile_reject_text = (compile_reject.stderr or compile_reject.stdout).strip()
    expect(
        compile_reject.returncode != 0,
        "expected native compile to fail closed when deprecated claim/scaffold sidecars are present",
    )
    expect(
        "deprecated claim/scaffold compatibility sidecar(s) detected"
        in compile_reject_text,
        "expected native compile rejection to publish the deprecated sidecar compatibility diagnostic",
    )

    validate_compile_dir = case_dir / "validate-source"
    compile_fixture_with_args(fixture, validate_compile_dir)
    report_path = validate_compile_dir / "module.objc3-conformance-report.json"
    for filename in DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES:
        (validate_compile_dir / filename).write_text("{}", encoding="utf-8")
    validate_dir = case_dir / "validate-output"
    validate_dir.mkdir(parents=True, exist_ok=True)
    validate_reject = run(
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
    validate_reject_text = (validate_reject.stderr or validate_reject.stdout).strip()
    expect(
        validate_reject.returncode != 0,
        "expected conformance validation to fail closed when deprecated claim/scaffold sidecars are present next to the validated report",
    )
    expect(
        "deprecated claim/scaffold compatibility sidecar(s) detected"
        in validate_reject_text,
        "expected conformance validation rejection to publish the deprecated sidecar compatibility diagnostic",
    )

    return CaseResult(
        case_id="scaffold-retirement-deprecated-sidecar-compatibility-diagnostics",
        probe="compile-and-validate-with-deprecated-claim-sidecars-present",
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "deprecated_sidecar_filenames": DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES,
            "compile_reject_returncode": compile_reject.returncode,
            "validate_reject_returncode": validate_reject.returncode,
        },
    )


def check_claim_publication_dashboard_schema_surface_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "claim-publication-dashboard-schema-surface"
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
        "expected conformance validation to publish the dashboard artifact successfully",
    )

    dashboard_path = validate_dir / "module.objc3-dashboard-status.json"
    dashboard = json.loads(dashboard_path.read_text(encoding="utf-8"))
    schema = json.loads(
        (ROOT / "schemas" / "objc3-conformance-dashboard-status-v1.schema.json").read_text(
            encoding="utf-8"
        )
    )
    required_keys = schema.get("required", [])
    expect(
        all(key in dashboard for key in required_keys),
        "expected dashboard artifact to publish every required top-level schema field",
    )
    expect(
        dashboard.get("schema_id") == "objc3-conformance-dashboard-status/v1"
        and dashboard.get("schema_version") == 1
        and dashboard.get("dashboard_version") == "0.11.0"
        and dashboard.get("release_label") == "v0.11"
        and dashboard.get("status") == "pass",
        "expected dashboard artifact to preserve the live schema identity and release status surface",
    )
    expect(
        [entry.get("profile_id") for entry in dashboard.get("profiles", [])]
        == ["core", "strict", "strict-concurrency", "strict-system"],
        "expected dashboard artifact to publish one schema-shaped profile row for each claimed profile",
    )
    expect(
        [entry.get("dependency_id") for entry in dashboard.get("dependencies", [])]
        == ["B-04", "B-10", "B-11", "B-12"],
        "expected dashboard artifact to publish the schema-shaped dependency inventory",
    )
    artifact_paths = [entry.get("artifact_path") for entry in dashboard.get("artifacts", [])]
    expect(
        artifact_paths
        == [
            "module.objc3-conformance-report.json",
            "module.objc3-conformance-publication.json",
            "module.objc3-conformance-validation.json",
            "module.objc3-release-evidence-operation.json",
        ],
        "expected dashboard artifact to preserve the live emitted artifact refs behind the schema surface",
    )
    expect(
        all(
            isinstance(entry.get("file_sha256"), str)
            and re.fullmatch(r"[a-f0-9]{64}", entry["file_sha256"])
            for entry in dashboard.get("artifacts", [])
        ),
        "expected dashboard artifact to publish schema-shaped lowercase 64-hex file digests for each emitted artifact ref",
    )
    expect(
        dashboard.get("summary", {}).get("profile_counts")
        == {"pass": 4, "fail": 0, "blocked": 0, "incomplete": 0}
        and dashboard.get("summary", {}).get("dependency_counts")
        == {"pass": 4, "fail": 0, "blocked": 0, "stale": 0, "missing": 0},
        "expected dashboard artifact to publish deterministic schema-shaped summary counts",
    )
    expect(
        dashboard.get("refresh", {}).get("trigger") == "manual-replay"
        and dashboard.get("refresh", {}).get("stale_dependency_ids") == []
        and dashboard.get("refresh", {}).get("escalation_state") == "none",
        "expected dashboard artifact to publish deterministic refresh telemetry",
    )
    expect(
        len(dashboard.get("change_history", [])) == 1
        and dashboard["change_history"][0].get("change_kind") == "refresh-only",
        "expected dashboard artifact to publish deterministic change history over the schema surface",
    )

    return CaseResult(
        case_id="claim-publication-dashboard-schema-surface",
        probe="compile-validate-and-inspect-schema-shaped-dashboard-artifact",
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "dashboard_schema_id": dashboard.get("schema_id"),
            "artifact_paths": artifact_paths,
            "profile_ids": [entry.get("profile_id") for entry in dashboard.get("profiles", [])],
        },
    )


def check_final_claim_publication_deprecated_path_shutdown_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "final-claim-publication-deprecated-path-shutdown"
    fixture = ROOT / Path(RELEASE_CLAIMABLE_SURFACE_FIXTURE)

    compile_dir = case_dir / "compile"
    compile_fixture_with_args(fixture, compile_dir)
    compile_artifacts = sorted(path.name for path in compile_dir.glob("module.objc3-*.json"))
    expect(
        compile_artifacts
        == [
            "module.objc3-advanced-feature-gate.json",
            "module.objc3-conformance-publication.json",
            "module.objc3-conformance-report.json",
            "module.objc3-release-candidate-matrix.json",
        ],
        "expected native compile to publish only the live claim publication and release-candidate sidecars",
    )
    expect(
        all(not (compile_dir / filename).exists() for filename in DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES),
        "expected native compile to stop emitting every deprecated claim/scaffold sidecar filename",
    )

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
        "expected conformance validation to publish the full final claim publication bundle",
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
        "expected conformance validation to publish the final post-publication release artifacts and no deprecated sidecars",
    )
    expect(
        all(not (validate_dir / filename).exists() for filename in DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES),
        "expected conformance validation to keep deprecated claim/scaffold sidecar paths shut down",
    )

    release_evidence_operation = json.loads(
        (validate_dir / "module.objc3-release-evidence-operation.json").read_text(
            encoding="utf-8"
        )
    )
    advanced_feature_gate = json.loads(
        (validate_dir / "module.objc3-advanced-feature-gate.json").read_text(
            encoding="utf-8"
        )
    )
    release_candidate_matrix = json.loads(
        (validate_dir / "module.objc3-release-candidate-matrix.json").read_text(
            encoding="utf-8"
        )
    )
    expect(
        release_evidence_operation.get("operation_model")
        == "validation-publishes-release-evidence-command-surface-and-dashboard-status-over-the-final-claim-publication-artifact-set",
        "expected release evidence operation artifact to describe the final claim publication bundle instead of the retired dashboard-ready summary path",
    )
    expect(
        advanced_feature_gate.get("surface_kind") == "native-cli-validation"
        and release_candidate_matrix.get("surface_kind") == "native-cli-validation",
        "expected validation-emitted gate and matrix artifacts to identify the live validation publication path",
    )
    expect(
        advanced_feature_gate.get("dashboard_artifact_expected")
        == "module.objc3-dashboard-status.json"
        and release_candidate_matrix.get("advanced_feature_gate_artifact")
        == "module.objc3-advanced-feature-gate.json",
        "expected validation-emitted release artifacts to preserve the final live artifact wiring",
    )
    expect(
        release_candidate_matrix.get("matrix_model")
        == "release-candidate-matrix-freezes-cross-lane-advanced-feature-evidence-over-the-final-claim-publication-artifact-set",
        "expected release candidate matrix to describe the final claim publication artifact set instead of emitted sidecars generically",
    )

    return CaseResult(
        case_id="final-claim-publication-deprecated-path-shutdown",
        probe="compile-validate-and-inspect-the-final-claim-publication-bundle",
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "compile_artifacts": compile_artifacts,
            "validate_artifacts": validate_artifacts,
            "validation_surface_kind": advanced_feature_gate.get("surface_kind"),
        },
    )


__all__ = [
    "check_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_case",
    "check_claim_publication_dashboard_schema_surface_case",
    "check_final_claim_publication_deprecated_path_shutdown_case",
]
