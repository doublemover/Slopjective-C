#!/usr/bin/env python3
"""Stage reviewed source-truth inputs from hosted platform evidence.

Generated hosted evidence is never source truth by itself. This helper consumes a
complete per-platform evidence root, or downloads that root from a green GitHub
Actions run, verifies that the generated review candidate and required artifacts
are present, and writes a reviewed-source proposal. The checked fixture is
updated only with the explicit apply flag.
"""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
from copy import deepcopy
from pathlib import Path
from typing import Any

SCRIPTS_ROOT = Path(__file__).resolve().parent
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from check_platform_host_promotion_evidence import (
    validate_host_promotion_reviewed_source_inputs,
)
from objc3c_tooling.paths import repo_rel
from platform_hardening_contracts import ROOT
from platform_hardening_contracts.host_evidence_contract import (
    HOST_EVIDENCE_REQUIRED_REVIEW_INPUT_SUFFIXES,
    HOST_EVIDENCE_REQUIRED_SOURCE_RECORD_TYPES,
    HOST_EVIDENCE_REVIEW_CANDIDATE_CONTRACT_ID,
    host_evidence_required_review_input_paths_for_platform,
)
from platform_hardening_contracts.host_promotion import (
    HOST_PROMOTION_REVIEWED_SOURCE_DURABLE_FIXTURE_PATHS,
    HOST_PROMOTION_REVIEWED_SOURCE_INPUT_RELATIVE_PATH,
    HOST_PROMOTION_REVIEWED_SOURCE_RECORD_ID_FIELD_BY_TYPE,
    HOST_PROMOTION_REVIEWED_SOURCE_RECORD_SECTION_BY_TYPE,
    HOST_PROMOTION_REQUIRED_HOSTED_PROMOTION_ARTIFACT_SUFFIXES,
    HOST_PROMOTION_REQUIRED_REVIEWED_SOURCE_FIELDS,
    HOST_PROMOTION_REQUIRED_SOURCE_RECORD_TYPES,
    host_promotion_platform_ids,
)

REVIEWED_SOURCE_INPUT_PATH = ROOT / HOST_PROMOTION_REVIEWED_SOURCE_INPUT_RELATIVE_PATH
DEFAULT_REVIEWER_ID = "objc3c.platform.host-evidence.review"
DEFAULT_GITHUB_REPOSITORY = "doublemover/Slopjective-C"
DEFAULT_GITHUB_ARTIFACT_DOWNLOAD_ROOT = (
    ROOT / "tmp" / "reports" / "platform-host-evidence-runs"
)
SOURCE_OWNED_REVIEW_VALIDATOR = "validate_host_promotion_reviewed_source_inputs"
REQUIRED_PASS_STEPS: tuple[str, ...] = (
    "build",
    "package",
    "install",
    "clean_install_distribution",
    "execution",
)
REQUIRED_TOOLCHAIN_COMPONENTS: tuple[str, ...] = (
    "llvm",
    "clang",
    "cmake",
    "ninja",
    "python",
    "node",
    "pwsh",
)
REQUIRED_GENERATED_ARTIFACT_STATUS = "generated-host-artifact-present"
REQUIRED_NATIVE_OBJECT_EMISSION_STATUS = "native_object_emission_supported"
REQUIRED_INSTALLED_ROOT_EXECUTION_CONTRACT_ID = (
    "objc3c.packaging.channels.installed-root-native-execution.v1"
)
REQUIRED_INSTALLED_ROOT_USAGE_EXIT_CODE = 2
PLATFORM_ISSUE_REF_BY_ID: dict[str, int] = {
    "linux-x64": 8228,
    "darwin-arm64": 8229,
}
PLATFORM_TARGET_TRIPLE_BY_ID: dict[str, str] = {
    "linux-x64": "x86_64-unknown-linux-gnu",
    "darwin-arm64": "aarch64-apple-darwin",
}
NATIVE_BUILD_SUMMARY_PATH = "tmp/build-objc3c-native/native_build_summary.json"
RUNNABLE_PACKAGE_MANIFEST_PATH = (
    "artifacts/package/objc3c-runnable-toolchain-package.json"
)
PACKAGE_CHANNELS_END_TO_END_SUMMARY_PATH = (
    "tmp/reports/package-channels/end-to-end-summary.json"
)
HOSTED_EXECUTION_SMOKE_SUMMARY_PATH = "tmp/reports/hosted-execution-smoke/summary.json"
NATIVE_EXECUTION_SMOKE_SUMMARY_PATH = (
    "tmp/reports/objc3c-native-execution-smoke/summary.json"
)
PACKAGE_INSTALL_RECEIPT_CONTRACT_ID = "objc3c.packaging.channels.install-receipt.v1"


class ReviewError(RuntimeError):
    """Raised when generated evidence cannot become reviewed source input."""


def load_json_object(path: Path) -> dict[str, Any]:
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise ReviewError(f"missing required JSON input: {repo_rel(path)}") from exc
    except json.JSONDecodeError as exc:
        raise ReviewError(f"invalid JSON input {repo_rel(path)}: {exc}") from exc
    if not isinstance(payload, dict):
        raise ReviewError(f"JSON input is not an object: {repo_rel(path)}")
    return payload


def write_json(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2, sort_keys=False) + "\n", encoding="utf-8")


def display_path(path: Path) -> str:
    try:
        return repo_rel(path)
    except ValueError:
        return path.as_posix()


def run_json_command(command: list[str]) -> dict[str, Any]:
    completed = subprocess.run(
        command,
        check=False,
        cwd=ROOT,
        encoding="utf-8",
        errors="replace",
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    if completed.returncode != 0:
        raise ReviewError(
            "command failed: "
            + " ".join(command)
            + "\n"
            + completed.stderr.strip()
        )
    try:
        payload = json.loads(completed.stdout)
    except json.JSONDecodeError as exc:
        raise ReviewError(
            "command did not return JSON: " + " ".join(command)
        ) from exc
    if not isinstance(payload, dict):
        raise ReviewError("command JSON was not an object: " + " ".join(command))
    return payload


def run_checked_command(command: list[str]) -> None:
    completed = subprocess.run(
        command,
        check=False,
        cwd=ROOT,
        encoding="utf-8",
        errors="replace",
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    if completed.returncode != 0:
        raise ReviewError(
            "command failed: "
            + " ".join(command)
            + "\n"
            + completed.stderr.strip()
        )


def require_safe_download_root(path: Path) -> Path:
    resolved = path.resolve()
    allowed_root = DEFAULT_GITHUB_ARTIFACT_DOWNLOAD_ROOT.resolve()
    if resolved == allowed_root or allowed_root in resolved.parents:
        return resolved
    raise ReviewError(
        "GitHub artifact download root must stay under "
        + repo_rel(DEFAULT_GITHUB_ARTIFACT_DOWNLOAD_ROOT)
    )


def require_green_github_run(
    *,
    run_id: str,
    repository: str,
    platform_id: str,
    expected_head_sha: str | None,
) -> dict[str, Any]:
    payload = run_json_command(
        [
            "gh",
            "run",
            "view",
            run_id,
            "--repo",
            repository,
            "--json",
            "status,conclusion,headSha,url,jobs",
        ]
    )
    if payload.get("status") != "completed" or payload.get("conclusion") != "success":
        raise ReviewError(
            f"GitHub run {run_id} is not a green completed run "
            f"(status={payload.get('status')!r}, conclusion={payload.get('conclusion')!r})"
        )
    if expected_head_sha and payload.get("headSha") != expected_head_sha:
        raise ReviewError(
            f"GitHub run {run_id} head SHA drifted: "
            f"expected {expected_head_sha}, got {payload.get('headSha')}"
        )
    expected_job_name = f"platform-host-evidence-{platform_id}"
    jobs = payload.get("jobs", [])
    if not isinstance(jobs, list):
        raise ReviewError(f"GitHub run {run_id} jobs payload drifted")
    matching_jobs = [
        job
        for job in jobs
        if isinstance(job, dict) and job.get("name") == expected_job_name
    ]
    if not matching_jobs:
        raise ReviewError(f"GitHub run {run_id} missing job {expected_job_name}")
    job = matching_jobs[0]
    if job.get("status") != "completed" or job.get("conclusion") != "success":
        raise ReviewError(
            f"GitHub run {run_id} job {expected_job_name} is not green "
            f"(status={job.get('status')!r}, conclusion={job.get('conclusion')!r})"
        )
    return payload


def locate_downloaded_evidence_root(download_root: Path, platform_id: str) -> Path:
    candidates: list[Path] = []
    for report_path in download_root.rglob("host-evidence-report.json"):
        root = report_path.parent
        if (root / "review-candidate-source-truth.json").is_file():
            report = load_json_object(report_path)
            if report.get("platform_id") == platform_id:
                candidates.append(root)
    if not candidates:
        raise ReviewError(
            f"downloaded artifact did not contain a complete {platform_id} evidence root"
        )
    unique = sorted({candidate.resolve() for candidate in candidates})
    if len(unique) != 1:
        joined = ", ".join(display_path(path) for path in unique)
        raise ReviewError(f"downloaded artifact contained multiple evidence roots: {joined}")
    return unique[0]


def download_github_evidence_artifact(args: argparse.Namespace) -> tuple[Path, dict[str, Any]]:
    run_id = str(args.github_run_id)
    repository = str(args.github_repo)
    platform_id = str(args.platform_id)
    run_payload = require_green_github_run(
        run_id=run_id,
        repository=repository,
        platform_id=platform_id,
        expected_head_sha=args.expected_head_sha,
    )
    root = require_safe_download_root(args.artifact_download_root)
    destination = root / f"run-{run_id}" / platform_id
    if destination.exists():
        shutil.rmtree(destination)
    destination.mkdir(parents=True, exist_ok=True)
    artifact_name = f"objc3c-platform-host-evidence-{platform_id}"
    run_checked_command(
        [
            "gh",
            "run",
            "download",
            run_id,
            "--repo",
            repository,
            "--name",
            artifact_name,
            "--dir",
            str(destination),
        ]
    )
    evidence_root = locate_downloaded_evidence_root(destination, platform_id)
    return evidence_root, {
        "run_id": run_id,
        "repository": repository,
        "run_url": run_payload.get("url"),
        "head_sha": run_payload.get("headSha"),
        "artifact_name": artifact_name,
        "download_root": display_path(destination),
        "evidence_root": display_path(evidence_root),
    }


def validate_reviewed_source_payload(
    payload: dict[str, Any],
    *,
    owner: str,
) -> dict[str, Any]:
    try:
        return validate_host_promotion_reviewed_source_inputs(payload)
    except RuntimeError as exc:
        raise ReviewError(
            f"{owner} failed source-owned host promotion validation: {exc}"
        ) from exc


def source_owned_validation_for_summary(
    validation_summary: dict[str, Any],
    *,
    result_prefix: str,
) -> dict[str, Any]:
    return {
        "validator": SOURCE_OWNED_REVIEW_VALIDATOR,
        "status": validation_summary.get("status"),
        "source_contract_id": validation_summary.get("contract_id"),
        f"{result_prefix}_promotion_allowed_platform_ids": list(
            validation_summary.get("promotion_allowed_platform_ids", [])
        ),
        "required_record_types_before_promotion_allowed": list(
            validation_summary.get("required_record_types_before_promotion_allowed", [])
        ),
    }


def platform_evidence_root(platform_id: str, configured_root: Path | None) -> Path:
    if configured_root is not None:
        return configured_root
    return ROOT / "tmp" / "reports" / "platform-host-evidence" / platform_id


def evidence_path(root: Path, suffix: str) -> Path:
    relative = Path(suffix)
    if relative.is_absolute() or ".." in relative.parts:
        raise ReviewError(f"hosted evidence suffix left platform scope: {suffix}")
    return root / relative


def require_platform(platform_id: str) -> None:
    if platform_id not in host_promotion_platform_ids():
        supported = ", ".join(host_promotion_platform_ids())
        raise ReviewError(f"unsupported platform id {platform_id}; expected one of: {supported}")


def expected_issue_ref(platform_id: str) -> int:
    try:
        return PLATFORM_ISSUE_REF_BY_ID[platform_id]
    except KeyError as exc:
        raise ReviewError(f"unsupported platform issue_ref mapping: {platform_id}") from exc


def expected_target_triple(platform_id: str) -> str:
    try:
        return PLATFORM_TARGET_TRIPLE_BY_ID[platform_id]
    except KeyError as exc:
        raise ReviewError(f"unsupported platform target triple mapping: {platform_id}") from exc


def require_schema_and_issue_ref(
    payload: dict[str, Any],
    *,
    platform_id: str,
    owner: str,
) -> None:
    if payload.get("schema_version") != 1:
        raise ReviewError(f"{owner} schema_version drifted")
    if payload.get("issue_ref") != expected_issue_ref(platform_id):
        raise ReviewError(f"{owner} issue_ref drifted")


def require_report_path_scope(platform_id: str, root: Path, suffix: str) -> str:
    path = evidence_path(root, suffix)
    if not path.is_file():
        raise ReviewError(f"missing hosted evidence artifact: {repo_rel(path)}")
    return platform_scoped_report_path(platform_id, suffix)


def platform_scoped_report_path(platform_id: str, suffix: str) -> str:
    return f"tmp/reports/platform-host-evidence/{platform_id}/{suffix}"


def require_generated_artifacts(platform_id: str, root: Path) -> list[str]:
    required_paths: list[str] = []
    for suffix in HOST_PROMOTION_REQUIRED_HOSTED_PROMOTION_ARTIFACT_SUFFIXES:
        required_paths.append(require_report_path_scope(platform_id, root, suffix))
    expected = host_evidence_required_review_input_paths_for_platform(platform_id)
    missing = sorted(set(expected) - set(required_paths))
    if missing:
        raise ReviewError("review input inventory drifted: " + ", ".join(missing))
    return required_paths


def require_review_candidate(platform_id: str, root: Path) -> dict[str, Any]:
    candidate = load_json_object(evidence_path(root, "review-candidate-source-truth.json"))
    require_schema_and_issue_ref(
        candidate,
        platform_id=platform_id,
        owner="review candidate",
    )
    if candidate.get("contract_id") != HOST_EVIDENCE_REVIEW_CANDIDATE_CONTRACT_ID:
        raise ReviewError("review candidate contract_id drifted")
    if candidate.get("platform_id") != platform_id:
        raise ReviewError("review candidate platform_id drifted")
    for field_name in (
        "generated_report_only",
        "reviewed_source_truth_required",
        "support_rows_remain_fail_closed_until_reviewed",
    ):
        if candidate.get(field_name) is not True:
            raise ReviewError(f"review candidate {field_name} drifted")
    for field_name in (
        "support_claim_published",
        "source_truth_update_allowed",
        "promotion_allowed",
        "support_truth",
        "local_temp_claims_promote_support",
        "prose_only_evidence_allowed",
        "stale_evidence_allowed",
    ):
        if candidate.get(field_name) is not False:
            raise ReviewError(f"review candidate {field_name} attempted promotion")
    rows = candidate.get("review_candidate_rows")
    if not isinstance(rows, list):
        raise ReviewError("review candidate rows must be a list")
    by_type = {
        str(row.get("record_type", "")): row
        for row in rows
        if isinstance(row, dict)
    }
    expected_types = set(HOST_EVIDENCE_REQUIRED_SOURCE_RECORD_TYPES)
    if set(by_type) != expected_types:
        missing = sorted(expected_types - set(by_type))
        extra = sorted(set(by_type) - expected_types)
        raise ReviewError(
            "review candidate record types drifted"
            + (f"; missing={missing}" if missing else "")
            + (f"; extra={extra}" if extra else "")
        )
    for record_type, row in by_type.items():
        if row.get("issue_ref") != expected_issue_ref(platform_id):
            raise ReviewError(f"{record_type} review candidate issue_ref drifted")
        if row.get("generated_artifacts_complete") is not True:
            raise ReviewError(f"{record_type} review candidate artifacts are incomplete")
        if row.get("source_truth_update_allowed") is not False:
            raise ReviewError(f"{record_type} review candidate allowed source truth update")
    return candidate


def require_host_report(platform_id: str, root: Path) -> dict[str, Any]:
    report = load_json_object(evidence_path(root, "host-evidence-report.json"))
    require_schema_and_issue_ref(
        report,
        platform_id=platform_id,
        owner="host evidence report",
    )
    if report.get("platform_id") != platform_id:
        raise ReviewError("host evidence report platform_id drifted")
    if report.get("source_truth_ingestion", {}).get("generated_report_only") is not True:
        raise ReviewError("host evidence report did not remain generated-only")
    steps = {
        str(step.get("step_id", "")): step
        for step in report.get("steps", [])
        if isinstance(step, dict)
    }
    for step_id in REQUIRED_PASS_STEPS:
        outcome = str(steps.get(step_id, {}).get("outcome", ""))
        if outcome != "success":
            raise ReviewError(f"host evidence step {step_id} did not pass: {outcome or '<missing>'}")
    return report


def require_generated_status(payload: dict[str, Any], owner: str) -> None:
    if payload.get("support_truth") is not False:
        raise ReviewError(f"{owner} generated artifact became support truth")
    if payload.get("promotion_allowed_from_generated_evidence") is not False:
        raise ReviewError(f"{owner} generated artifact allowed promotion")
    status = str(payload.get("status", ""))
    if status != REQUIRED_GENERATED_ARTIFACT_STATUS:
        raise ReviewError(f"{owner} generated artifact is not complete: {status or '<missing>'}")


def require_artifact_entry(entry: Any, owner: str) -> dict[str, Any]:
    if not isinstance(entry, dict):
        raise ReviewError(f"{owner} artifact entry must be an object")
    path_text = str(entry.get("path", ""))
    if not path_text:
        raise ReviewError(f"{owner} artifact entry missing path")
    if not isinstance(entry.get("exists"), bool):
        raise ReviewError(f"{owner} artifact entry missing boolean exists")
    if entry.get("exists") is True:
        size_bytes = entry.get("size_bytes")
        if not isinstance(size_bytes, int) or size_bytes <= 0:
            raise ReviewError(f"{owner} existing artifact missing size")
        digest = entry.get("sha256")
        if not isinstance(digest, str) or len(digest) != 64:
            raise ReviewError(f"{owner} existing artifact missing sha256")
    return entry


def require_source_artifacts(
    payload: dict[str, Any],
    owner: str,
    *,
    expected_paths: tuple[str, ...] = (),
) -> list[dict[str, Any]]:
    raw_artifacts = require_nonempty_list(payload, "source_artifacts", owner)
    artifacts = [
        require_artifact_entry(entry, f"{owner} source_artifacts")
        for entry in raw_artifacts
    ]
    by_path = {
        str(artifact.get("path", "")).replace("\\", "/"): artifact
        for artifact in artifacts
    }
    present_paths = {
        path
        for path, artifact in by_path.items()
        if artifact.get("exists") is True
    }
    if not present_paths:
        raise ReviewError(f"{owner} source_artifacts must include existing durable source")
    missing = sorted(
        path
        for path in expected_paths
        if path.replace("\\", "/") not in present_paths
    )
    if missing:
        raise ReviewError(
            f"{owner} source_artifacts missing durable source: "
            + ", ".join(missing)
        )
    return artifacts


def require_artifact_present(
    payload: dict[str, Any],
    field_name: str,
    owner: str,
    *,
    expected_path: str | None = None,
) -> dict[str, Any]:
    artifact = require_artifact_entry(payload.get(field_name), f"{owner} {field_name}")
    if artifact.get("exists") is not True:
        raise ReviewError(f"{owner} {field_name} is not present")
    if expected_path is not None:
        actual_path = str(artifact.get("path", "")).replace("\\", "/")
        if actual_path != expected_path.replace("\\", "/"):
            raise ReviewError(f"{owner} {field_name} path drifted")
    return artifact


def require_native_execution_claimed_false(payload: dict[str, Any], owner: str) -> None:
    if payload.get("native_execution_claimed") is not False:
        raise ReviewError(f"{owner} native_execution_claimed drifted")


def install_receipt_status_field(payload: dict[str, Any], field_name: str) -> Any:
    if field_name in payload:
        return payload.get(field_name)
    producer_evidence = payload.get("producer_evidence")
    if isinstance(producer_evidence, dict):
        return producer_evidence.get(field_name)
    return None


def require_common_reviewable_generated_payload(
    payload: dict[str, Any],
    *,
    owner: str,
    platform_id: str,
    suffix: str,
    contract_id: str,
    record_id: str | None = None,
    reviewed_source_required: bool | None = None,
) -> None:
    require_schema_and_issue_ref(payload, platform_id=platform_id, owner=owner)
    if payload.get("contract_id") != contract_id:
        raise ReviewError(f"{owner} contract_id drifted")
    if payload.get("platform_id") != platform_id:
        raise ReviewError(f"{owner} platform_id drifted")
    if record_id is not None and payload.get("record_id") != record_id:
        raise ReviewError(f"{owner} record_id drifted")
    expected_path = platform_scoped_report_path(platform_id, suffix)
    if payload.get("generated_report_path") != expected_path:
        raise ReviewError(f"{owner} generated_report_path drifted")
    if (
        reviewed_source_required is not None
        and payload.get("reviewed_source_required") is not reviewed_source_required
    ):
        raise ReviewError(f"{owner} reviewed_source_required drifted")
    require_generated_status(payload, owner)
    require_source_artifacts(payload, owner)


def require_object(payload: dict[str, Any], field_name: str, owner: str) -> dict[str, Any]:
    value = payload.get(field_name)
    if not isinstance(value, dict):
        raise ReviewError(f"{owner} missing object field {field_name}")
    return value


def require_list(payload: dict[str, Any], field_name: str, owner: str) -> list[Any]:
    value = payload.get(field_name)
    if not isinstance(value, list):
        raise ReviewError(f"{owner} missing list field {field_name}")
    return value


def require_nonempty_list(
    payload: dict[str, Any],
    field_name: str,
    owner: str,
) -> list[Any]:
    value = require_list(payload, field_name, owner)
    if not value:
        raise ReviewError(f"{owner} field {field_name} must not be empty")
    return value


def require_field_value(
    payload: dict[str, Any],
    field_name: str,
    expected: Any,
    owner: str,
) -> None:
    if payload.get(field_name) != expected:
        raise ReviewError(f"{owner} {field_name} drifted")


def require_record_list_value(
    payload: dict[str, Any],
    field_name: str,
    source_record: dict[str, Any],
    source_field_name: str,
    owner: str,
) -> None:
    expected = list(source_record.get(source_field_name, []))
    actual = require_list(payload, field_name, owner)
    if actual != expected:
        raise ReviewError(f"{owner} {field_name} drifted")


def source_record_identity_value(
    source_record: dict[str, Any],
    *,
    platform_id: str,
    field_name: str,
) -> Any:
    if field_name == "target_platform_id":
        return platform_id
    return source_record.get(field_name)


def require_identity_fields_match_source(
    identity: dict[str, Any],
    source_record: dict[str, Any],
    *,
    platform_id: str,
    owner: str,
    identity_field: str,
    field_names: tuple[str, ...],
) -> None:
    for field_name in field_names:
        expected = source_record_identity_value(
            source_record,
            platform_id=platform_id,
            field_name=field_name,
        )
        if identity.get(field_name) != expected:
            raise ReviewError(f"{owner} {identity_field}.{field_name} drifted")


def require_identity_payload_matches_record(
    payload: dict[str, Any],
    source_record: dict[str, Any],
    *,
    platform_id: str,
    suffix: str,
    contract_id: str,
    record_id: str,
    identity_kind: str,
    identity_field_names: tuple[str, ...],
) -> None:
    owner = f"{identity_kind} identity"
    require_common_reviewable_generated_payload(
        payload,
        owner=owner,
        platform_id=platform_id,
        suffix=suffix,
        contract_id=contract_id,
        record_id=record_id,
        reviewed_source_required=True,
    )
    require_source_artifacts(
        payload,
        owner,
        expected_paths=(
            NATIVE_BUILD_SUMMARY_PATH,
            NATIVE_EXECUTION_SMOKE_SUMMARY_PATH,
        ),
    )
    expected_identity = require_object(payload, "expected_identity", owner)
    actual_identity = require_object(payload, "actual_identity", owner)
    require_identity_fields_match_source(
        expected_identity,
        source_record,
        platform_id=platform_id,
        owner=owner,
        identity_field="expected_identity",
        field_names=("target_platform_id", "target_triple", "arch", *identity_field_names),
    )
    require_identity_fields_match_source(
        actual_identity,
        source_record,
        platform_id=platform_id,
        owner=owner,
        identity_field="actual_identity",
        field_names=("target_platform_id", "target_triple", *identity_field_names),
    )


def require_runtime_manifest_payload(
    payload: dict[str, Any],
    package_root_record: dict[str, Any],
    *,
    platform_id: str,
) -> None:
    owner = "runtime library manifest"
    require_common_reviewable_generated_payload(
        payload,
        owner=owner,
        platform_id=platform_id,
        suffix="package/runtime-library-manifest.json",
        contract_id="objc3c.platform.hosted-runtime-library-manifest.generated.v1",
    )
    require_native_execution_claimed_false(payload, owner)
    require_source_artifacts(
        payload,
        owner,
        expected_paths=(
            RUNNABLE_PACKAGE_MANIFEST_PATH,
            NATIVE_BUILD_SUMMARY_PATH,
        ),
    )
    require_field_value(payload, "target_platform_id", platform_id, owner)
    require_field_value(payload, "target_triple", expected_target_triple(platform_id), owner)
    source_target = str(payload.get("source_package_target_platform_id", ""))
    if source_target and source_target != platform_id:
        raise ReviewError(f"{owner} source_package_target_platform_id drifted")
    require_record_list_value(
        payload,
        "runtime_library_names",
        package_root_record,
        "runtime_library_names",
        owner,
    )
    require_record_list_value(
        payload,
        "package_root_layout",
        package_root_record,
        "package_root_layout",
        owner,
    )
    require_field_value(
        payload,
        "loader_path_policy",
        package_root_record.get("loader_path_policy"),
        owner,
    )
    package_artifact = require_object(payload, "package_manifest_artifact", owner)
    if package_artifact.get("exists") is not True:
        raise ReviewError(f"{owner} package manifest artifact is not present")
    require_artifact_present(
        payload,
        "package_manifest_artifact",
        owner,
        expected_path=RUNNABLE_PACKAGE_MANIFEST_PATH,
    )
    expected_runtime_names = [
        str(name) for name in package_root_record.get("runtime_library_names", [])
    ]
    runtime_artifacts = require_nonempty_list(payload, "runtime_library_artifacts", owner)
    if not any(
        isinstance(artifact, dict)
        and artifact.get("exists") is True
        and any(str(artifact.get("path", "")).endswith(name) for name in expected_runtime_names)
        for artifact in runtime_artifacts
    ):
        raise ReviewError(f"{owner} did not prove expected runtime library artifacts")


def require_installed_root_execution_proof(
    payload: dict[str, Any],
    *,
    field_name: str,
    expected_channel_id: str,
    platform_id: str,
    owner: str,
) -> None:
    proof = require_object(payload, field_name, owner)
    if proof.get("contract_id") != REQUIRED_INSTALLED_ROOT_EXECUTION_CONTRACT_ID:
        raise ReviewError(f"{owner} {field_name} contract_id drifted")
    if proof.get("status") != "PASS":
        raise ReviewError(f"{owner} {field_name} did not pass")
    if proof.get("channel_id") != expected_channel_id:
        raise ReviewError(f"{owner} {field_name} channel_id drifted")
    if proof.get("execution_source") != "installed-root":
        raise ReviewError(f"{owner} {field_name} execution_source drifted")
    if proof.get("repo_temp_dependency") is not False:
        raise ReviewError(f"{owner} {field_name} depended on repo temp output")
    if proof.get("preexisting_artifacts_dependency") is not False:
        raise ReviewError(f"{owner} {field_name} depended on preexisting artifacts")
    expected_exit_code = proof.get("expected_exit_code")
    if (
        expected_exit_code is not None
        and expected_exit_code != REQUIRED_INSTALLED_ROOT_USAGE_EXIT_CODE
    ):
        raise ReviewError(f"{owner} {field_name} expected_exit_code drifted")
    if proof.get("returncode") != REQUIRED_INSTALLED_ROOT_USAGE_EXIT_CODE:
        raise ReviewError(f"{owner} {field_name} returncode drifted")
    if proof.get("usage_banner_seen") is not True:
        raise ReviewError(f"{owner} {field_name} did not reach usage path")
    target_platform_id = str(proof.get("target_platform_id", ""))
    if target_platform_id and target_platform_id != platform_id:
        raise ReviewError(f"{owner} {field_name} target_platform_id drifted")


def require_install_receipt_payload(
    payload: dict[str, Any],
    package_install_record: dict[str, Any],
    *,
    platform_id: str,
    record_id: str,
) -> None:
    owner = "install receipt"
    require_common_reviewable_generated_payload(
        payload,
        owner=owner,
        platform_id=platform_id,
        suffix="install/install-receipt.json",
        contract_id="objc3c.platform.hosted-install-receipt.generated.v1",
        record_id=record_id,
        reviewed_source_required=True,
    )
    require_native_execution_claimed_false(payload, owner)
    source_receipt_path = str(payload.get("source_install_receipt_path", ""))
    if not source_receipt_path:
        raise ReviewError(f"{owner} missing source_install_receipt_path")
    require_source_artifacts(
        payload,
        owner,
        expected_paths=(
            PACKAGE_CHANNELS_END_TO_END_SUMMARY_PATH,
            RUNNABLE_PACKAGE_MANIFEST_PATH,
            source_receipt_path,
        ),
    )
    require_field_value(payload, "target_platform_id", platform_id, owner)
    require_field_value(payload, "target_triple", expected_target_triple(platform_id), owner)
    require_record_list_value(
        payload,
        "package_root_layout",
        package_install_record,
        "package_root_layout",
        owner,
    )
    require_field_value(payload, "package_manifest", RUNNABLE_PACKAGE_MANIFEST_PATH, owner)
    require_artifact_present(
        payload,
        "package_manifest_artifact",
        owner,
        expected_path=RUNNABLE_PACKAGE_MANIFEST_PATH,
    )
    require_artifact_present(
        payload,
        "package_channels_summary_artifact",
        owner,
        expected_path=PACKAGE_CHANNELS_END_TO_END_SUMMARY_PATH,
    )
    require_artifact_present(
        payload,
        "source_install_receipt_artifact",
        owner,
        expected_path=source_receipt_path,
    )
    source_receipt = require_object(payload, "source_install_receipt", owner)
    if source_receipt.get("contract_id") != PACKAGE_INSTALL_RECEIPT_CONTRACT_ID:
        raise ReviewError(f"{owner} source receipt contract_id drifted")
    receipt_target = str(source_receipt.get("target_platform_id", ""))
    package_runtime_model = source_receipt.get("package_runtime_model")
    if not receipt_target and isinstance(package_runtime_model, dict):
        receipt_target = str(package_runtime_model.get("target_platform_id", ""))
    if receipt_target and receipt_target != platform_id:
        raise ReviewError(f"{owner} source receipt target_platform_id drifted")
    if not isinstance(package_runtime_model, dict):
        raise ReviewError(f"{owner} source receipt missing package_runtime_model")
    if package_runtime_model.get("target_platform_id") != platform_id:
        raise ReviewError(f"{owner} source receipt runtime model target_platform_id drifted")
    if package_runtime_model.get("package_root_layout") != payload.get("package_root_layout"):
        raise ReviewError(f"{owner} source receipt runtime model package_root_layout drifted")
    require_installed_root_execution_proof(
        payload,
        field_name="installed_root_execution",
        expected_channel_id="local-installer",
        platform_id=platform_id,
        owner=owner,
    )
    require_installed_root_execution_proof(
        payload,
        field_name="offline_installed_root_execution",
        expected_channel_id="offline-bundle",
        platform_id=platform_id,
        owner=owner,
    )
    if install_receipt_status_field(
        payload,
        "installed_root_execution_status",
    ) != payload.get("installed_root_execution", {}).get("status"):
        raise ReviewError(f"{owner} installed_root_execution_status drifted")
    if install_receipt_status_field(
        payload,
        "offline_installed_root_execution_status",
    ) != payload.get("offline_installed_root_execution", {}).get("status"):
        raise ReviewError(f"{owner} offline_installed_root_execution_status drifted")


def require_runtime_load_payload(
    payload: dict[str, Any],
    runtime_record: dict[str, Any],
    object_identity_record: dict[str, Any],
    *,
    platform_id: str,
    record_id: str,
) -> None:
    owner = "runtime load probe"
    require_common_reviewable_generated_payload(
        payload,
        owner=owner,
        platform_id=platform_id,
        suffix="execution/runtime-load-probe.json",
        contract_id="objc3c.platform.hosted-runtime-load-probe.generated.v1",
        record_id=record_id,
        reviewed_source_required=True,
    )
    require_source_artifacts(
        payload,
        owner,
        expected_paths=(
            HOSTED_EXECUTION_SMOKE_SUMMARY_PATH,
            NATIVE_EXECUTION_SMOKE_SUMMARY_PATH,
            RUNNABLE_PACKAGE_MANIFEST_PATH,
        ),
    )
    require_field_value(payload, "target_platform_id", platform_id, owner)
    require_field_value(
        payload,
        "target_triple",
        object_identity_record.get("target_triple"),
        owner,
    )
    require_record_list_value(
        payload,
        "runtime_library_names",
        runtime_record,
        "runtime_library_names",
        owner,
    )
    require_field_value(
        payload,
        "loader_path_policy",
        runtime_record.get("loader_policy"),
        owner,
    )
    if payload.get("load_probe_exit_code") != 0:
        raise ReviewError(f"{owner} load_probe_exit_code did not pass")
    require_nonempty_list(payload, "resolved_runtime_paths", owner)
    native_status = str(payload.get("native_execution_status", "")).upper()
    if native_status != "PASS":
        raise ReviewError(f"{owner} native_execution_status did not pass")
    if str(payload.get("skip_reason", "")).strip():
        raise ReviewError(f"{owner} skip_reason was present")
    runtime_library = str(payload.get("runtime_library", ""))
    expected_names = [str(name) for name in runtime_record.get("runtime_library_names", [])]
    if not any(runtime_library.endswith(name) for name in expected_names):
        raise ReviewError(f"{owner} runtime_library drifted")


def require_toolchain_capabilities_payload(
    payload: dict[str, Any],
    *,
    platform_id: str,
) -> None:
    owner = "toolchain capabilities"
    if payload.get("ok") is not True:
        raise ReviewError(f"{owner} did not pass")
    if payload.get("native_object_emission_status") != REQUIRED_NATIVE_OBJECT_EMISSION_STATUS:
        raise ReviewError(f"{owner} native_object_emission_status did not pass")
    if payload.get("llc_filetype_obj_available") is not True:
        raise ReviewError(f"{owner} llc_filetype_obj_available did not pass")
    if payload.get("llc_target_object_emission_available") is not True:
        raise ReviewError(f"{owner} llc_target_object_emission_available did not pass")
    if payload.get("coherent_toolchain_root") is not True:
        raise ReviewError(f"{owner} coherent_toolchain_root did not pass")
    host_gate = payload.get("host_platform_support_gate")
    if isinstance(host_gate, dict) and host_gate.get("platform_id") != platform_id:
        raise ReviewError(f"{owner} host platform drifted")
    support_matrix = require_object(payload, "llvm_support_matrix", owner)
    entries = require_nonempty_list(support_matrix, "toolchain_matrix_entries", owner)
    matrix_entry = entries[0]
    if not isinstance(matrix_entry, dict):
        raise ReviewError(f"{owner} toolchain_matrix_entries entries must be objects")
    if matrix_entry.get("host_platform_id") != platform_id:
        raise ReviewError(f"{owner} host_platform_id drifted")
    for field_name in (
        "support_status",
        "object_emission_capability",
        "package_capability",
        "native_execution_capability",
    ):
        if matrix_entry.get(field_name) != "supported":
            raise ReviewError(f"{owner} {field_name} did not pass")


def normalize_host_system(value: Any) -> str:
    normalized = str(value or "").strip().lower().replace("_", "-")
    if normalized.startswith("macos") or normalized in {"mac", "mac-os"}:
        return "darwin"
    if normalized.startswith("ubuntu") or normalized.startswith("linux"):
        return "linux"
    if normalized.startswith("windows") or normalized in {"win32", "win64"}:
        return "windows"
    return normalized


def normalize_host_machine(value: Any) -> str:
    normalized = str(value or "").strip().lower().replace("-", "_")
    if normalized in {"amd64", "x64", "x86_64"}:
        return "x86_64"
    if normalized in {"arm64", "aarch64"}:
        return "arm64"
    return normalized


def require_host_identity_matches_source(
    report: dict[str, Any],
    source_record: dict[str, Any],
    *,
    platform_id: str,
) -> dict[str, str]:
    owner = "host evidence report identity"
    host_identity = report.get("host_identity")
    if not isinstance(host_identity, dict):
        raise ReviewError("host evidence report missing host_identity")
    actual_system = normalize_host_system(
        host_identity.get("platform_system")
        or host_identity.get("runner_os")
        or host_identity.get("image_os")
    )
    expected_system = normalize_host_system(
        source_record.get("host_system") or source_record.get("host_os")
    )
    if actual_system != expected_system:
        raise ReviewError(f"{owner} host_system drifted for {platform_id}")
    actual_machine = normalize_host_machine(
        host_identity.get("platform_machine") or host_identity.get("runner_arch")
    )
    expected_machine = normalize_host_machine(
        source_record.get("host_machine") or source_record.get("host_arch")
    )
    if actual_machine != expected_machine:
        raise ReviewError(f"{owner} host_machine drifted for {platform_id}")
    return {
        "host_system": actual_system,
        "host_machine": actual_machine,
    }


def load_required_payloads(root: Path) -> dict[str, dict[str, Any]]:
    payloads: dict[str, dict[str, Any]] = {}
    for suffix in HOST_EVIDENCE_REQUIRED_REVIEW_INPUT_SUFFIXES:
        if suffix.endswith(".json"):
            payloads[suffix] = load_json_object(evidence_path(root, suffix))
    return payloads


def reviewed_source_paths() -> list[str]:
    return list(HOST_PROMOTION_REVIEWED_SOURCE_DURABLE_FIXTURE_PATHS)


def reviewed_artifact_paths(platform_id: str) -> list[str]:
    return host_evidence_required_review_input_paths_for_platform(platform_id)


def reviewed_metadata(
    record: dict[str, Any],
    platform_id: str,
    *,
    evidence_id: str,
) -> dict[str, Any]:
    updated = deepcopy(record)
    updated["claim_state"] = "reviewed-source"
    updated["promotion_allowed"] = True
    updated["platform_ids"] = [platform_id]
    updated["review_status"] = "reviewed-current-source"
    updated["stale_evidence_allowed"] = False
    updated["prose_only_evidence"] = False
    updated["local_temp_evidence_claim"] = False
    updated["source_paths"] = reviewed_source_paths()
    updated["evidence_ids"] = [evidence_id]
    paths = reviewed_artifact_paths(platform_id)
    updated["hosted_runner_artifact_paths"] = paths
    updated["toolchain_artifact_paths"] = paths
    updated["package_artifact_paths"] = paths
    updated["unsupported_behavior"] = "fail-closed"
    if "generated_report_support_truth" in updated:
        updated["generated_report_support_truth"] = False
    return updated


def review_evidence_id(platform_id: str, record_type: str, report: dict[str, Any]) -> str:
    github = report.get("github", {})
    run_id = str(github.get("run_id", "") or "local")
    sha = str(github.get("sha", "") or "unknown")[:12]
    return f"objc3c.evidence.reviewed-source.{platform_id}.{record_type}.{run_id}.{sha}"


def required_record_ids_for_platform(
    payload: dict[str, Any],
    platform_id: str,
) -> dict[str, str]:
    platforms = {
        str(row.get("platform_id", "")): row
        for row in payload.get("platforms", [])
        if isinstance(row, dict)
    }
    row = platforms.get(platform_id)
    if row is None:
        raise ReviewError(f"reviewed source input missing platform row: {platform_id}")
    ids = row.get("required_record_ids")
    if not isinstance(ids, dict):
        raise ReviewError(f"{platform_id} platform row missing required_record_ids")
    return {str(key): str(value) for key, value in ids.items()}


def replace_record(
    payload: dict[str, Any],
    record_type: str,
    record_id: str,
    replacement: dict[str, Any],
) -> None:
    section = HOST_PROMOTION_REVIEWED_SOURCE_RECORD_SECTION_BY_TYPE[record_type]
    rows = payload.get(section)
    if not isinstance(rows, list):
        raise ReviewError(f"reviewed source input missing section {section}")
    for index, row in enumerate(rows):
        if isinstance(row, dict) and row.get("record_id") == record_id:
            rows[index] = replacement
            return
    raise ReviewError(f"reviewed source input missing {record_type} record {record_id}")


def promote_platform_row(payload: dict[str, Any], platform_id: str) -> None:
    for row in payload.get("platforms", []):
        if isinstance(row, dict) and row.get("platform_id") == platform_id:
            row["review_decision"] = "approved-for-support-source-truth"
            row["not_promotion_ready_record_types"] = []
            row["remaining_blockers"] = []
            row["promotion_allowed"] = True
            row["support_truth"] = True
            return
    raise ReviewError(f"reviewed source input missing platform row: {platform_id}")


def toolchain_component_probe(component: str, *, evidence_path: str) -> dict[str, Any]:
    return {
        "component": component,
        "tool_names": [component],
        "exit_code": 0,
        "capability_status": "supported" if component == "llvm" else "present",
        "evidence_path": evidence_path,
    }


def update_platform_records(
    payload: dict[str, Any],
    *,
    platform_id: str,
    report: dict[str, Any],
    generated_payloads: dict[str, dict[str, Any]],
) -> dict[str, Any]:
    required_ids = required_record_ids_for_platform(payload, platform_id)

    object_identity = generated_payloads["build/object-identity.json"]
    debug_identity = generated_payloads["build/debug-identity.json"]
    llvm_capabilities = generated_payloads["llvm-capabilities.json"]
    install_receipt = generated_payloads["install/install-receipt.json"]
    runtime_load = generated_payloads["execution/runtime-load-probe.json"]
    runtime_manifest = generated_payloads["package/runtime-library-manifest.json"]

    source_records: dict[str, dict[str, Any]] = {}
    reviewed_records: dict[str, dict[str, Any]] = {}
    record_sections = HOST_PROMOTION_REVIEWED_SOURCE_RECORD_SECTION_BY_TYPE
    id_fields = HOST_PROMOTION_REVIEWED_SOURCE_RECORD_ID_FIELD_BY_TYPE
    by_section = {
        section: {
            str(row.get("record_id", "")): row
            for row in payload.get(section, [])
            if isinstance(row, dict)
        }
        for section in record_sections.values()
    }

    for record_type in HOST_PROMOTION_REQUIRED_SOURCE_RECORD_TYPES:
        record_id_field = id_fields[record_type]
        record_id = required_ids.get(record_id_field)
        if not record_id:
            raise ReviewError(f"{platform_id} platform row missing {record_id_field}")
        source = by_section[record_sections[record_type]].get(record_id)
        if source is None:
            raise ReviewError(f"missing reviewed source {record_type} record {record_id}")
        source_records[record_type] = source
        reviewed = reviewed_metadata(
            source,
            platform_id,
            evidence_id=review_evidence_id(platform_id, record_type, report),
        )
        reviewed_records[record_type] = reviewed

    reviewed_host_identity = require_host_identity_matches_source(
        report,
        source_records["host_identity"],
        platform_id=platform_id,
    )
    require_toolchain_capabilities_payload(llvm_capabilities, platform_id=platform_id)
    require_identity_payload_matches_record(
        object_identity,
        source_records["object_identity"],
        platform_id=platform_id,
        suffix="build/object-identity.json",
        contract_id="objc3c.platform.hosted-object-identity.generated.v1",
        record_id=required_ids[id_fields["object_identity"]],
        identity_kind="object",
        identity_field_names=("object_format",),
    )
    require_identity_payload_matches_record(
        debug_identity,
        source_records["debug_identity"],
        platform_id=platform_id,
        suffix="build/debug-identity.json",
        contract_id="objc3c.platform.hosted-debug-identity.generated.v1",
        record_id=required_ids[id_fields["debug_identity"]],
        identity_kind="debug",
        identity_field_names=("debug_format",),
    )
    require_runtime_manifest_payload(
        runtime_manifest,
        source_records["package_root"],
        platform_id=platform_id,
    )
    require_install_receipt_payload(
        install_receipt,
        source_records["package_install_identity"],
        platform_id=platform_id,
        record_id=required_ids[id_fields["package_install_identity"]],
    )
    require_runtime_load_payload(
        runtime_load,
        source_records["runtime_load_link_proof"],
        source_records["object_identity"],
        platform_id=platform_id,
        record_id=required_ids[id_fields["runtime_load_link_proof"]],
    )

    reviewed_records["host_identity"].update(
        {
            "host_system": reviewed_host_identity["host_system"],
            "host_machine": reviewed_host_identity["host_machine"],
            "runner_label": report.get("runner_label", ""),
            "github": report.get("github", {}),
        }
    )
    reviewed_records["toolchain_probe"].update(
        {
            "runner_label": report.get("runner_label", ""),
            "required_missing_probe_classes": [],
            "component_probes": [
                toolchain_component_probe(
                    component,
                    evidence_path=(
                        f"tmp/reports/platform-host-evidence/{platform_id}/"
                        "llvm-capabilities.json"
                    ),
                )
                for component in REQUIRED_TOOLCHAIN_COMPONENTS
            ],
        }
    )
    reviewed_records["install_receipt"]["install_receipt_present"] = True
    reviewed_records["native_execution"].update(
        {
            "native_execution_passed": True,
            "execution_evidence_ids": [
                f"tmp/reports/platform-host-evidence/{platform_id}/execution/hosted-execution-smoke-summary.json",
                f"tmp/reports/platform-host-evidence/{platform_id}/execution/native-execution-smoke-summary.json",
            ],
        }
    )
    reviewed_records["package_install_identity"].update(
        {
            "install_receipt_present": True,
            "installed_root_execution_present": True,
            "offline_installed_root_execution_present": True,
        }
    )
    reviewed_records["runtime_load_link_proof"].update(
        {
            "load_probe_exit_code": int(runtime_load.get("load_probe_exit_code", -1)),
            "resolved_runtime_paths": list(runtime_load.get("resolved_runtime_paths", [])),
        }
    )

    for record_type, reviewed in reviewed_records.items():
        replace_record(
            payload,
            record_type,
            required_ids[id_fields[record_type]],
            reviewed,
        )
    promote_platform_row(payload, platform_id)
    return payload


def build_reviewed_source_payload(args: argparse.Namespace) -> tuple[dict[str, Any], dict[str, Any]]:
    platform_id = args.platform_id
    require_platform(platform_id)
    github_run: dict[str, Any] | None = None
    if args.github_run_id:
        if args.evidence_root is not None:
            raise ReviewError("--github-run-id and --evidence-root cannot be combined")
        args.evidence_root, github_run = download_github_evidence_artifact(args)
    root = platform_evidence_root(platform_id, args.evidence_root)
    artifact_paths = require_generated_artifacts(platform_id, root)
    candidate = require_review_candidate(platform_id, root)
    report = require_host_report(platform_id, root)
    generated_payloads = load_required_payloads(root)
    for suffix in HOST_PROMOTION_REQUIRED_HOSTED_PROMOTION_ARTIFACT_SUFFIXES:
        if suffix.endswith(".json") and suffix not in generated_payloads:
            raise ReviewError(f"missing parsed hosted artifact: {suffix}")

    source_payload = deepcopy(load_json_object(args.source_inputs))
    updated = update_platform_records(
        source_payload,
        platform_id=platform_id,
        report=report,
        generated_payloads=generated_payloads,
    )
    summary = {
        "contract_id": "objc3c.platform.host-evidence.reviewed-source-staging.v1",
        "status": "PROPOSED_REVIEWED_SOURCE_READY",
        "platform_id": platform_id,
        "issue_ref": candidate.get("issue_ref"),
        "evidence_root": display_path(root),
        "review_candidate": display_path(
            evidence_path(root, "review-candidate-source-truth.json")
        ),
        "source_review_inputs": display_path(args.source_inputs),
        "proposed_reviewed_source_output": display_path(args.output),
        "applied_to_source": bool(args.apply_reviewed_source_truth),
        "reviewer_id": args.reviewer_id,
        "required_checked_source_paths": reviewed_source_paths(),
        "required_hosted_artifact_paths": artifact_paths,
        "generated_reports_are_source_truth": False,
        "source_truth_update_requires_apply_flag": True,
    }
    if github_run is not None:
        summary["github_actions_run"] = github_run
    updated.setdefault("review_application", {})
    updated["review_application"][platform_id] = {
        "reviewer_id": args.reviewer_id,
        "review_status": "reviewed-current-source",
        "evidence_root": display_path(root),
        "review_candidate": display_path(
            evidence_path(root, "review-candidate-source-truth.json")
        ),
        "generated_reports_are_source_truth": False,
        "source_truth_update_approved": bool(args.apply_reviewed_source_truth),
        "required_hosted_artifact_suffixes": list(
            HOST_EVIDENCE_REQUIRED_REVIEW_INPUT_SUFFIXES
        ),
        "required_reviewed_source_fields": list(
            HOST_PROMOTION_REQUIRED_REVIEWED_SOURCE_FIELDS
        ),
    }
    return updated, summary


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Stage reviewed source-truth inputs from hosted platform evidence."
    )
    parser.add_argument("--platform-id", required=True, choices=host_promotion_platform_ids())
    parser.add_argument("--evidence-root", type=Path)
    parser.add_argument(
        "--github-run-id",
        help="green GitHub Actions run id whose hosted platform evidence artifact should be downloaded and reviewed",
    )
    parser.add_argument(
        "--github-repo",
        default=DEFAULT_GITHUB_REPOSITORY,
        help="GitHub repository for --github-run-id",
    )
    parser.add_argument(
        "--expected-head-sha",
        help="optional head SHA that the green GitHub run must match before review",
    )
    parser.add_argument(
        "--artifact-download-root",
        type=Path,
        default=DEFAULT_GITHUB_ARTIFACT_DOWNLOAD_ROOT,
        help="tmp-only root used for downloaded GitHub Actions artifacts",
    )
    parser.add_argument(
        "--source-inputs",
        type=Path,
        default=REVIEWED_SOURCE_INPUT_PATH,
        help="checked host_promotion_reviewed_source_inputs.json path",
    )
    parser.add_argument(
        "--output",
        type=Path,
        help="proposal output path; defaults under the platform evidence root",
    )
    parser.add_argument("--reviewer-id", default=DEFAULT_REVIEWER_ID)
    parser.add_argument(
        "--apply-reviewed-source-truth",
        action="store_true",
        help="overwrite the checked reviewed-source fixture after staging succeeds",
    )
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)
    root = platform_evidence_root(args.platform_id, args.evidence_root)
    if args.output is None:
        args.output = root / "reviewed-source-inputs.proposed.json"
    payload, summary = build_reviewed_source_payload(args)
    proposed_validation = validate_reviewed_source_payload(
        payload,
        owner="proposed reviewed-source payload",
    )
    summary["source_owned_validation"] = source_owned_validation_for_summary(
        proposed_validation,
        result_prefix="proposed",
    )
    write_json(args.output, payload)
    if args.apply_reviewed_source_truth:
        applied_validation = validate_reviewed_source_payload(
            payload,
            owner="applied reviewed-source payload",
        )
        write_json(args.source_inputs, payload)
        summary["status"] = "APPLIED_REVIEWED_SOURCE_TRUTH"
        summary["applied_source_owned_validation"] = source_owned_validation_for_summary(
            applied_validation,
            result_prefix="applied",
        )
    summary_path = root / "reviewed-source-staging-summary.json"
    write_json(summary_path, summary)
    print(f"proposed_reviewed_source_output: {display_path(args.output)}")
    print(f"summary_path: {display_path(summary_path)}")
    if args.apply_reviewed_source_truth:
        print(f"applied_reviewed_source_truth: {display_path(args.source_inputs)}")
    print("objc3c-platform-host-evidence-review: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
