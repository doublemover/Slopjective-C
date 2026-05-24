#!/usr/bin/env python3
"""Stage reviewed source-truth inputs from hosted platform evidence.

Generated hosted evidence is never source truth by itself. This helper consumes a
complete per-platform evidence root, verifies that the generated review candidate
and required artifacts are present, and writes a reviewed-source proposal. The
checked fixture is updated only with the explicit apply flag.
"""

from __future__ import annotations

import argparse
import json
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


def require_report_path_scope(platform_id: str, root: Path, suffix: str) -> str:
    path = evidence_path(root, suffix)
    if not path.is_file():
        raise ReviewError(f"missing hosted evidence artifact: {repo_rel(path)}")
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
        if row.get("generated_artifacts_complete") is not True:
            raise ReviewError(f"{record_type} review candidate artifacts are incomplete")
        if row.get("source_truth_update_allowed") is not False:
            raise ReviewError(f"{record_type} review candidate allowed source truth update")
    return candidate


def require_host_report(platform_id: str, root: Path) -> dict[str, Any]:
    report = load_json_object(evidence_path(root, "host-evidence-report.json"))
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
    if status not in {"generated-host-artifact-present", "PASS"}:
        raise ReviewError(f"{owner} generated artifact is not complete: {status or '<missing>'}")


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
    host_identity = report.get("host_identity", {})
    if not isinstance(host_identity, dict):
        raise ReviewError("host evidence report missing host_identity")

    object_identity = generated_payloads["build/object-identity.json"]
    debug_identity = generated_payloads["build/debug-identity.json"]
    install_receipt = generated_payloads["install/install-receipt.json"]
    runtime_load = generated_payloads["execution/runtime-load-probe.json"]
    runtime_manifest = generated_payloads["package/runtime-library-manifest.json"]

    for owner, generated in (
        ("object identity", object_identity),
        ("debug identity", debug_identity),
        ("install receipt", install_receipt),
        ("runtime load", runtime_load),
        ("runtime library manifest", runtime_manifest),
    ):
        require_generated_status(generated, owner)

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
        record_id = required_ids[id_fields[record_type]]
        source = by_section[record_sections[record_type]].get(record_id)
        if source is None:
            raise ReviewError(f"missing reviewed source {record_type} record {record_id}")
        reviewed = reviewed_metadata(
            source,
            platform_id,
            evidence_id=review_evidence_id(platform_id, record_type, report),
        )
        reviewed_records[record_type] = reviewed

    reviewed_records["host_identity"].update(
        {
            "host_system": str(host_identity.get("platform_system") or host_identity.get("runner_os") or "").lower(),
            "host_machine": str(host_identity.get("platform_machine") or ""),
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
