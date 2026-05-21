"""Build and verify the portable public conformance suite package."""

from __future__ import annotations

import hashlib
import shutil
from collections.abc import Iterable
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.json_io import write_json_file, write_text_file
from objc3c_tooling.paths import repo_rel

from .contracts import (
    DEFAULT_PACKAGE_ROOT,
    MANIFEST_PATH,
    PACKAGE_CASE_CONTRACT_ID,
    PACKAGE_CONTRACT_ID,
    PACKAGE_CONTRACT_PATH,
    PACKAGE_MANIFEST_CONTRACT_ID,
    PACKAGE_REPLAY_EVIDENCE_PATH,
    PACKAGE_SUMMARY_CONTRACT_ID,
    PUBLIC_COMMAND_PREFIX,
    ROOT,
)


PACKAGE_README = """# Objective-C 3 Public Conformance Suite

This generated package is replay evidence only. The checked repository manifest
and fixture contracts remain the source of truth.

Run `npm run objc3c -- validate-public-conformance-suite` from this package to
verify the packaged case manifests and source hashes. Generated tmp reports
prove replay; they do not create support claims.
"""

PACKAGE_JSON = {
    "name": "objc3-public-conformance-suite",
    "private": True,
    "scripts": {
        "objc3c": "python tools/replay_public_conformance_suite.py",
    },
}

REPLAY_TOOL = r'''#!/usr/bin/env python3
"""Replay the packaged Objective-C 3 public conformance suite."""

from __future__ import annotations

import hashlib
import json
import sys
from datetime import datetime, timezone
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
PUBLIC_PREFIX = "npm run objc3c -- "
ACTION_PROFILES = {
    "validate-conformance-corpus": {"core"},
    "validate-interop-conformance": {"stdlib-package"},
    "validate-release-candidate-conformance": {"release-candidate"},
    "validate-public-conformance-suite": {"core", "stdlib-package", "release-candidate"},
}


def load_json(path: Path) -> dict:
    with path.open("r", encoding="utf-8") as handle:
        payload = json.load(handle)
    if not isinstance(payload, dict):
        raise RuntimeError(f"{path.as_posix()} must contain a JSON object")
    return payload


def write_json(path: Path, payload: dict) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def fail(message: str) -> int:
    print(f"public-conformance-suite-replay: FAIL\n- {message}", file=sys.stderr)
    return 1


def main(argv: list[str]) -> int:
    action = argv[0] if argv else "validate-public-conformance-suite"
    profiles = ACTION_PROFILES.get(action)
    if profiles is None:
        return fail(f"unsupported packaged public conformance action: {action}")

    manifest = load_json(ROOT / "package-manifest.json")
    if manifest.get("contract_id") != "objc3c.public_conformance_suite.package_manifest.v1":
        return fail("package-manifest contract_id drifted")
    if manifest.get("source_truth_policy", {}).get("tmp_source_truth_allowed") is not False:
        return fail("package manifest permits tmp source truth")
    artifact_contract = manifest.get("artifact_contract", {})
    generated_policy = artifact_contract.get("generated_output_policy", {})
    if generated_policy.get("generated_outputs_committable") is not False:
        return fail("package manifest permits generated outputs to be committed")
    if generated_policy.get("generated_outputs_can_define_support") is not False:
        return fail("package manifest permits generated outputs to define support")
    if not all(str(root).startswith("tmp/") for root in generated_policy.get("allowed_generated_roots", [])):
        return fail("package manifest generated output boundary escaped tmp")

    public_commands = manifest.get("public_commands", [])
    if not all(isinstance(command, str) and command.startswith(PUBLIC_PREFIX) for command in public_commands):
        return fail("package manifest contains a non-public command")

    source_count = 0
    for source in manifest.get("source_files", []):
        source_path = ROOT / source["package_path"]
        if not source_path.is_file():
            return fail(f"missing packaged source: {source['package_path']}")
        if sha256_file(source_path) != source["sha256"]:
            return fail(f"packaged source hash drifted: {source['package_path']}")
        source_count += 1

    selected_cases = [
        case for case in manifest.get("cases", [])
        if profiles.intersection(set(case.get("profile_ids", [])))
    ]
    if not selected_cases:
        return fail(f"no packaged cases selected for {action}")

    for case in selected_cases:
        case_manifest = load_json(ROOT / case["case_manifest"])
        if case_manifest.get("contract_id") != "objc3c.public_conformance_suite.case.v1":
            return fail(f"{case['case_id']} case contract_id drifted")
        if case_manifest.get("case_id") != case["case_id"]:
            return fail(f"{case['case_id']} case id drifted")
        if case_manifest.get("release_gate") is not True:
            return fail(f"{case['case_id']} is not a release gate")
        if not case_manifest.get("positive_evidence") or not case_manifest.get("negative_evidence"):
            return fail(f"{case['case_id']} lacks positive or negative evidence")
        provenance = case_manifest.get("fixture_provenance", {})
        if provenance.get("origin") != "checked-in-public-suite":
            return fail(f"{case['case_id']} fixture provenance is not checked-in public suite")
        if provenance.get("source_owned") is not True:
            return fail(f"{case['case_id']} fixture provenance is not source-owned")
        if provenance.get("internal_only") is not False or provenance.get("generated") is not False:
            return fail(f"{case['case_id']} fixture provenance is internal or generated")
        command = case_manifest.get("runnable_command", "")
        if not isinstance(command, str) or not command.startswith(PUBLIC_PREFIX):
            return fail(f"{case['case_id']} command is not public")

    report_path = ROOT / "tmp" / "reports" / "conformance" / f"{action}.json"
    write_json(
        report_path,
        {
            "contract_id": "objc3c.public_conformance_suite.packaged_replay.v1",
            "generated_at_utc": datetime.now(timezone.utc).isoformat(),
            "status": "PASS",
            "action": action,
            "profiles": sorted(profiles),
            "suite_id": manifest["suite_id"],
            "suite_version": manifest["suite_version"],
            "case_count": len(selected_cases),
            "source_file_count": source_count,
            "tmp_source_truth_allowed": False,
            "generated_reports_are_evidence_only": True,
        },
    )
    print(f"summary_path: {report_path.relative_to(ROOT).as_posix()}")
    print("public-conformance-suite-replay: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
'''


def repo_path(relative_path: str) -> Path:
    return ROOT / relative_path


def normalized_repo_path(path: str) -> str:
    return path.replace("\\", "/").strip("/")


def package_relative_source_path(repo_relative_path: str) -> str:
    return f"sources/{normalized_repo_path(repo_relative_path)}"


def safe_case_filename(case_id: str) -> str:
    return case_id.replace("/", "_").replace("\\", "_") + ".json"


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def require_object(value: Any, *, field: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        raise RuntimeError(f"{field} must be an object")
    return value


def require_list(value: Any, *, field: str) -> list[Any]:
    if not isinstance(value, list):
        raise RuntimeError(f"{field} must be an array")
    return value


def require_public_command(command: str, *, field: str) -> None:
    if not command.startswith(PUBLIC_COMMAND_PREFIX):
        raise RuntimeError(f"{field} is not public: {command}")


def require_fixture_provenance(case: dict[str, Any], *, field: str) -> dict[str, Any]:
    provenance = require_object(case.get("fixture_provenance"), field=field)
    if provenance.get("origin") != "checked-in-public-suite":
        raise RuntimeError(f"{field}.origin must be checked-in-public-suite")
    if provenance.get("owner") != "objc3-public-conformance":
        raise RuntimeError(f"{field}.owner drifted")
    if provenance.get("source_owned") is not True:
        raise RuntimeError(f"{field}.source_owned must be true")
    if provenance.get("internal_only") is not False:
        raise RuntimeError(f"{field}.internal_only must be false")
    if provenance.get("generated") is not False:
        raise RuntimeError(f"{field}.generated must be false")
    return provenance


def require_checked_source_file(relative_path: str, *, field: str) -> str:
    normalized = normalized_repo_path(relative_path)
    if normalized.startswith("tmp/"):
        raise RuntimeError(f"{field} uses tmp as source truth: {normalized}")
    path = repo_path(normalized)
    if not path.is_file():
        raise RuntimeError(f"{field} is missing: {normalized}")
    return normalized


def load_suite_inputs() -> tuple[dict[str, Any], dict[str, Any], dict[str, Any]]:
    manifest = load_json(MANIFEST_PATH)
    replay_evidence = load_json(PACKAGE_REPLAY_EVIDENCE_PATH)
    package_contract = load_json(PACKAGE_CONTRACT_PATH)
    return manifest, replay_evidence, package_contract


def validate_package_contract(
    manifest: dict[str, Any],
    replay_evidence: dict[str, Any],
    package_contract: dict[str, Any],
) -> None:
    if package_contract.get("contract_id") != PACKAGE_CONTRACT_ID:
        raise RuntimeError("public suite package contract_id drifted")
    if package_contract.get("schema_version") != 1:
        raise RuntimeError("public suite package contract schema_version drifted")
    if package_contract.get("source_manifest") != repo_rel(MANIFEST_PATH):
        raise RuntimeError("public suite package contract source_manifest drifted")
    if package_contract.get("package_replay_evidence") != repo_rel(PACKAGE_REPLAY_EVIDENCE_PATH):
        raise RuntimeError("public suite package replay evidence path drifted")
    if package_contract.get("suite_id") != manifest.get("suite_id"):
        raise RuntimeError("public suite package contract suite_id drifted from manifest")
    if replay_evidence.get("source_manifest") != repo_rel(MANIFEST_PATH):
        raise RuntimeError("package replay evidence source_manifest drifted")
    if replay_evidence.get("package_stage_root") != manifest["package_surface"]["package_stage_root"]:
        raise RuntimeError("package replay evidence stage root drifted from manifest")

    expected_source_owned_contracts = {
        repo_rel(MANIFEST_PATH),
        repo_rel(PACKAGE_REPLAY_EVIDENCE_PATH),
        repo_rel(PACKAGE_CONTRACT_PATH),
    }
    declared_source_owned_contracts = {
        require_checked_source_file(str(path), field="package source-owned contract")
        for path in require_list(package_contract.get("source_owned_contracts"), field="source_owned_contracts")
    }
    if declared_source_owned_contracts != expected_source_owned_contracts:
        raise RuntimeError("public suite package source-owned contract set drifted")

    generated_boundary = require_object(
        package_contract.get("generated_output_boundary"),
        field="generated_output_boundary",
    )
    required_tmp_roots = {
        normalized_repo_path(str(path))
        for path in require_list(generated_boundary.get("required_tmp_roots"), field="required_tmp_roots")
    }
    manifest_tmp_roots = {
        normalized_repo_path(str(path))
        for path in require_list(
            manifest["artifact_contract"]["generated_output_policy"]["allowed_generated_roots"],
            field="manifest generated roots",
        )
    }
    if required_tmp_roots != manifest_tmp_roots:
        raise RuntimeError("public suite package generated output roots drifted from manifest")
    if not all(path.startswith("tmp/") for path in required_tmp_roots):
        raise RuntimeError("public suite package generated output roots must stay under tmp")
    if generated_boundary.get("generated_outputs_committable") is not False:
        raise RuntimeError("public suite package generated_outputs_committable must be false")
    if generated_boundary.get("generated_outputs_can_define_support") is not False:
        raise RuntimeError("public suite package generated_outputs_can_define_support must be false")

    for entry in require_list(replay_evidence.get("packaged_entrypoints"), field="packaged_entrypoints"):
        entry_object = require_object(entry, field="packaged entrypoint")
        require_public_command(str(entry_object.get("command", "")), field=f"{entry_object.get('profile_id')}.command")
        if entry_object.get("requires_repo_checkout") is not False:
            raise RuntimeError(f"{entry_object.get('profile_id')} must not require a repo checkout")
        if entry_object.get("requires_network") is not False:
            raise RuntimeError(f"{entry_object.get('profile_id')} must not require network access")

    required_fail_closed = {
        "tmp artifacts are never source truth",
        "compatibility modes and fallback gates stay disabled",
        "unsupported claims cannot be promoted by packaged replay",
        "every public-stable case carries positive and negative checked evidence",
        "every public-stable case carries source-owned fixture provenance",
        "generated package outputs stay under tmp and are never checked-in source contracts",
    }
    declared = set(
        require_list(package_contract.get("fail_closed_invariants"), field="fail_closed_invariants")
    )
    missing = required_fail_closed - declared
    if missing:
        missing_text = ", ".join(sorted(missing))
        raise RuntimeError(f"public suite package contract missing fail-closed invariants: {missing_text}")


def iter_case_source_paths(case: dict[str, Any]) -> Iterable[str]:
    for key in ("source_manifest", "conformance_fixture", "traceability_fixture"):
        yield str(case[key])
    for key in ("positive_evidence", "negative_evidence"):
        for source_path in require_list(case.get(key), field=f"{case['case_id']}.{key}"):
            yield str(source_path)


def collect_source_files(
    manifest: dict[str, Any],
    replay_evidence: dict[str, Any],
    package_contract: dict[str, Any],
) -> list[str]:
    paths: set[str] = {
        repo_rel(MANIFEST_PATH),
        repo_rel(PACKAGE_REPLAY_EVIDENCE_PATH),
        repo_rel(PACKAGE_CONTRACT_PATH),
    }
    source_truth = require_object(manifest.get("source_truth"), field="source_truth")
    for key in ("corpus_surface", "support_claim_runnable_evidence_catalog", "capability_matrix", "evidence_map"):
        paths.add(str(source_truth[key]))
    for source_path in require_list(replay_evidence.get("required_packaged_inputs"), field="required_packaged_inputs"):
        paths.add(str(source_path))
    for source_path in require_list(package_contract.get("required_packaged_inputs"), field="required_packaged_inputs"):
        paths.add(str(source_path))
    for phase in require_list(manifest.get("phase_taxonomy"), field="phase_taxonomy"):
        paths.add(str(require_object(phase, field="phase row")["source_manifest"]))
    for case in require_list(manifest.get("suite_cases"), field="suite_cases"):
        case_object = require_object(case, field="suite case")
        paths.update(iter_case_source_paths(case_object))
    return sorted({require_checked_source_file(path, field="package source") for path in paths})


def clean_package_root(package_root: Path) -> None:
    root_resolved = ROOT.resolve()
    package_resolved = package_root.resolve()
    tmp_resolved = (ROOT / "tmp").resolve()
    try:
        package_resolved.relative_to(tmp_resolved)
    except ValueError as exc:
        raise RuntimeError(f"refusing to clean package root outside tmp: {package_resolved}") from exc
    try:
        package_resolved.relative_to(root_resolved)
    except ValueError as exc:
        raise RuntimeError(f"refusing to clean package root outside repo: {package_resolved}") from exc
    if package_root.exists():
        shutil.rmtree(package_root)
    package_root.mkdir(parents=True, exist_ok=True)


def copy_sources(package_root: Path, source_paths: list[str]) -> list[dict[str, str]]:
    copied: list[dict[str, str]] = []
    for relative_path in source_paths:
        source = repo_path(relative_path)
        package_relative = package_relative_source_path(relative_path)
        destination = package_root / package_relative
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, destination)
        copied.append(
            {
                "repo_path": relative_path,
                "package_path": package_relative,
                "sha256": sha256_file(destination),
            }
        )
    return copied


def case_payload(
    *,
    manifest: dict[str, Any],
    case: dict[str, Any],
    source_index: dict[str, dict[str, str]],
) -> dict[str, Any]:
    source_paths = sorted(
        {
            require_checked_source_file(path, field=f"{case['case_id']} source")
            for path in iter_case_source_paths(case)
        }
    )
    return {
        "contract_id": PACKAGE_CASE_CONTRACT_ID,
        "schema_version": 1,
        "suite_id": manifest["suite_id"],
        "suite_version": manifest["suite_version"],
        "case_id": case["case_id"],
        "phase": case["phase"],
        "feature_id": case["feature_id"],
        "capability_id": case["capability_id"],
        "support_claim": case["support_claim"],
        "stable_case_index": case["stable_case_index"],
        "fixture_provenance": case["fixture_provenance"],
        "expectation": case["expectation"],
        "profile_ids": case["profile_ids"],
        "runnable_command": case["runnable_command"],
        "platform_requirements": case["platform_requirements"],
        "packaging_class": case["packaging_class"],
        "release_gate": case["release_gate"],
        "positive_evidence": case["positive_evidence"],
        "negative_evidence": case["negative_evidence"],
        "source_truth_requirements": case.get("source_truth_requirements", []),
        "packaged_source_files": [source_index[path] for path in source_paths],
    }


def build_package(
    *,
    package_root: Path = DEFAULT_PACKAGE_ROOT,
    generated_at_utc: str | None = None,
) -> dict[str, Any]:
    manifest, replay_evidence, package_contract = load_suite_inputs()
    validate_package_contract(manifest, replay_evidence, package_contract)
    source_paths = collect_source_files(manifest, replay_evidence, package_contract)
    clean_package_root(package_root)
    copied_sources = copy_sources(package_root, source_paths)
    source_index = {entry["repo_path"]: entry for entry in copied_sources}

    cases = [
        require_object(case, field="suite case")
        for case in require_list(manifest["suite_cases"], field="suite_cases")
    ]
    case_entries: list[dict[str, Any]] = []
    stable_case_indices: list[int] = []
    for case in cases:
        require_public_command(str(case["runnable_command"]), field=f"{case['case_id']}.runnable_command")
        stable_case_index = case.get("stable_case_index")
        if not isinstance(stable_case_index, int):
            raise RuntimeError(f"{case['case_id']} stable_case_index must be an integer")
        stable_case_indices.append(stable_case_index)
        require_fixture_provenance(case, field=f"{case['case_id']}.fixture_provenance")
        if case.get("packaging_class") != "public-stable":
            raise RuntimeError(f"{case['case_id']} is not public-stable")
        if case.get("release_gate") is not True:
            raise RuntimeError(f"{case['case_id']} is not a release gate")
        if not require_list(case.get("positive_evidence"), field=f"{case['case_id']}.positive_evidence"):
            raise RuntimeError(f"{case['case_id']} has no positive evidence")
        if not require_list(case.get("negative_evidence"), field=f"{case['case_id']}.negative_evidence"):
            raise RuntimeError(f"{case['case_id']} has no negative evidence")
        case_file = f"cases/{safe_case_filename(str(case['case_id']))}"
        write_json_file(package_root / case_file, case_payload(manifest=manifest, case=case, source_index=source_index))
        case_entries.append(
            {
                "case_id": case["case_id"],
                "stable_case_index": stable_case_index,
                "phase": case["phase"],
                "expectation": case["expectation"],
                "support_claim": case["support_claim"],
                "runnable_command": case["runnable_command"],
                "profile_ids": case["profile_ids"],
                "release_gate": case["release_gate"],
                "case_manifest": case_file,
            }
        )
    if stable_case_indices != list(range(1, len(cases) + 1)):
        raise RuntimeError("public suite stable_case_index values must be contiguous and manifest-ordered")

    profile_commands = {
        str(profile["profile_id"]): str(profile["default_command"])
        for profile in require_list(manifest["public_profiles"], field="public_profiles")
    }
    for command in profile_commands.values():
        require_public_command(command, field="public profile command")

    generated = generated_at_utc or datetime.now(timezone.utc).isoformat()
    package_manifest = {
        "contract_id": PACKAGE_MANIFEST_CONTRACT_ID,
        "schema_version": 1,
        "generated_at_utc": generated,
        "suite_id": manifest["suite_id"],
        "suite_version": manifest["suite_version"],
        "public_status": manifest["public_status"],
        "source_manifest": repo_rel(MANIFEST_PATH),
        "package_contract": repo_rel(PACKAGE_CONTRACT_PATH),
        "package_replay_evidence": repo_rel(PACKAGE_REPLAY_EVIDENCE_PATH),
        "source_truth_policy": {
            "checked_in_source_truth_required": True,
            "tmp_source_truth_allowed": False,
            "generated_reports_are_evidence_only": True,
        },
        "artifact_contract": manifest["artifact_contract"],
        "offline_compatible": True,
        "profile_commands": profile_commands,
        "packaged_entrypoints": replay_evidence["packaged_entrypoints"],
        "phase_ids": [phase["phase_id"] for phase in manifest["phase_taxonomy"]],
        "cases": case_entries,
        "source_files": copied_sources,
        "case_count": len(case_entries),
        "source_file_count": len(copied_sources),
        "support_claims": sorted({str(case["support_claim"]) for case in cases}),
        "public_commands": sorted({str(case["runnable_command"]) for case in cases} | set(profile_commands.values())),
    }
    write_json_file(package_root / "package-manifest.json", package_manifest)
    write_json_file(
        package_root / "replay-plan.json",
        {
            "contract_id": "objc3c.public_conformance_suite.replay_plan.v1",
            "schema_version": 1,
            "suite_id": manifest["suite_id"],
            "suite_version": manifest["suite_version"],
            "entrypoints": replay_evidence["packaged_entrypoints"],
            "fail_closed_checks": package_contract["fail_closed_invariants"],
        },
    )
    write_json_file(package_root / "package.json", PACKAGE_JSON)
    write_text_file(package_root / "tools" / "replay_public_conformance_suite.py", REPLAY_TOOL)
    write_text_file(package_root / "README.md", PACKAGE_README)
    return package_manifest


def verify_package(package_root: Path) -> dict[str, Any]:
    package_manifest_path = package_root / "package-manifest.json"
    package_manifest = load_json(package_manifest_path)
    if package_manifest.get("contract_id") != PACKAGE_MANIFEST_CONTRACT_ID:
        raise RuntimeError("package manifest contract_id drifted")
    if package_manifest.get("source_truth_policy", {}).get("tmp_source_truth_allowed") is not False:
        raise RuntimeError("package manifest permits tmp source truth")
    for command in require_list(package_manifest.get("public_commands"), field="package public_commands"):
        require_public_command(str(command), field="package public command")

    verified_source_count = 0
    for source in require_list(package_manifest.get("source_files"), field="package source_files"):
        source_object = require_object(source, field="package source file")
        package_path = package_root / str(source_object["package_path"])
        if not package_path.is_file():
            raise RuntimeError(f"missing packaged source: {source_object['package_path']}")
        if sha256_file(package_path) != source_object["sha256"]:
            raise RuntimeError(f"packaged source hash drifted: {source_object['package_path']}")
        verified_source_count += 1

    verified_case_count = 0
    for case in require_list(package_manifest.get("cases"), field="package cases"):
        case_object = require_object(case, field="package case")
        case_manifest = load_json(package_root / str(case_object["case_manifest"]))
        if case_manifest.get("contract_id") != PACKAGE_CASE_CONTRACT_ID:
            raise RuntimeError(f"{case_object['case_id']} case contract_id drifted")
        if case_manifest.get("case_id") != case_object["case_id"]:
            raise RuntimeError(f"{case_object['case_id']} case manifest id drifted")
        if case_manifest.get("release_gate") is not True:
            raise RuntimeError(f"{case_object['case_id']} is not a release gate")
        if not case_manifest.get("positive_evidence") or not case_manifest.get("negative_evidence"):
            raise RuntimeError(f"{case_object['case_id']} lacks positive or negative evidence")
        require_fixture_provenance(case_manifest, field=f"{case_object['case_id']}.fixture_provenance")
        require_public_command(
            str(case_manifest.get("runnable_command", "")),
            field=f"{case_object['case_id']}.runnable_command",
        )
        verified_case_count += 1

    return {
        "package_manifest_path": repo_rel(package_manifest_path),
        "package_root": repo_rel(package_root),
        "verified_case_count": verified_case_count,
        "verified_source_file_count": verified_source_count,
        "public_commands": package_manifest["public_commands"],
        "support_claims": package_manifest["support_claims"],
    }


def build_summary(package_manifest: dict[str, Any], verification: dict[str, Any]) -> dict[str, Any]:
    return {
        "contract_id": PACKAGE_SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "suite_id": package_manifest["suite_id"],
        "suite_version": package_manifest["suite_version"],
        "package_manifest_path": verification["package_manifest_path"],
        "package_root": verification["package_root"],
        "case_count": package_manifest["case_count"],
        "source_file_count": package_manifest["source_file_count"],
        "verified_case_count": verification["verified_case_count"],
        "verified_source_file_count": verification["verified_source_file_count"],
        "offline_compatible": package_manifest["offline_compatible"],
        "tmp_source_truth_allowed": package_manifest["source_truth_policy"]["tmp_source_truth_allowed"],
        "generated_reports_are_evidence_only": package_manifest["source_truth_policy"][
            "generated_reports_are_evidence_only"
        ],
        "source_owned_contract_count": len(package_manifest["artifact_contract"]["source_owned_contracts"]),
        "generated_output_roots": package_manifest["artifact_contract"]["generated_output_policy"][
            "allowed_generated_roots"
        ],
        "public_commands": verification["public_commands"],
        "support_claims": verification["support_claims"],
    }
