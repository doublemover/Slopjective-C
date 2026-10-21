#!/usr/bin/env python3
"""Validate package security hardening source contracts."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_package_manager.trust import (  # noqa: E402
    PACKAGE_MANAGER_TAMPER_CODE,
    collect_extraction_plan_failures,
    collect_trust_policy_failures,
    default_trust_policy_payload,
    package_extraction_plan_payload,
)
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file  # noqa: E402
from objc3c_tooling.paths import repo_rel  # noqa: E402

CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "package_ecosystem"
    / "package_security_hardening_contract.json"
)
PACKAGE_MODEL_CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "package_ecosystem"
    / "package_manager_model_contract.json"
)
SUMMARY_PATH = (
    ROOT
    / "tmp"
    / "reports"
    / "package-ecosystem"
    / "package-security-hardening-summary.json"
)

SOURCE_TOKEN_REQUIREMENTS = (
    (
        "scripts/objc3c_package_manager/trust.py",
        (
            "PACKAGE_EXTRACTION_PLAN_CONTRACT_ID",
            "collect_extraction_plan_failures",
            "collect_filesystem_extraction_plan_failures",
            "resolve_package_trust_cli_path",
            "release_registry_trust_root_policy",
        ),
    ),
    (
        "scripts/sign_objc3c_package.py",
        (
            "resolve_package_trust_cli_path",
            "reject_existing=True",
        ),
    ),
    (
        "scripts/verify_objc3c_package.py",
        ("resolve_package_trust_cli_path",),
    ),
    (
        "scripts/objc3c_package_manager/operations.py",
        (
            "extraction_plan_digest",
            "package_extraction_plan_payload",
        ),
    ),
    (
        "scripts/objc3c_package_manager/install_distribution.py",
        (
            "collect_filesystem_extraction_plan_failures",
            "before-filesystem-mutation",
        ),
    ),
    (
        "schemas/objc3c-package-signing-trust-v1.schema.json",
        (
            "extraction_path_policy",
            "installer_update_key_policy",
            "release_registry_trust_root_policy",
        ),
    ),
    (
        "schemas/objc3c-package-lock-v1.schema.json",
        ("release_registry_trust_root_policy",),
    ),
)

REQUIRED_NEGATIVE_CASES = {
    "absolute-extraction-path",
    "parent-traversal-extraction-path",
    "symlink-extraction-entry",
    "overwrite-existing-install-path",
    "duplicate-extraction-path",
    "case-conflicting-extraction-path",
    "installer-key-active-overclaim",
    "update-key-missing",
    "release-trust-root-active",
    "registry-trust-root-active",
    "absolute-sign-cli-input",
    "overwrite-signature-output",
}


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _validate_source_tokens(failures: list[str]) -> None:
    for raw_path, tokens in SOURCE_TOKEN_REQUIREMENTS:
        path = ROOT / raw_path
        expect(path.is_file(), f"missing package security source: {raw_path}", failures)
        if not path.is_file():
            continue
        text = _read(path)
        for token in tokens:
            expect(token in text, f"package security token missing from {raw_path}: {token}", failures)


def _validate_contract(contract: dict[str, Any], failures: list[str]) -> None:
    expect(
        contract.get("contract_id")
        == "objc3c.package_ecosystem.package_security_hardening.v1",
        "package security contract id drifted",
        failures,
    )
    expect("#8223" in contract.get("issue_refs", []), "package security issue #8223 missing", failures)
    expect("#8204" in contract.get("issue_refs", []), "package umbrella issue #8204 missing", failures)
    expect(contract.get("diagnostic_code") == PACKAGE_MANAGER_TAMPER_CODE, "package security diagnostic drifted", failures)
    expect(
        contract.get("schema") == "schemas/objc3c-package-signing-trust-v1.schema.json",
        "package security schema path drifted",
        failures,
    )

    path_policy = contract.get("path_safety_contract", {})
    expect(isinstance(path_policy, dict), "package security path safety contract missing", failures)
    if isinstance(path_policy, dict):
        for field_name in (
            "before_filesystem_mutation_required",
            "reject_absolute_paths",
            "reject_parent_traversal",
            "reject_symlink_entries",
            "reject_overwrite_existing_paths",
            "reject_duplicate_paths",
            "reject_case_conflicting_paths",
        ):
            expect(path_policy.get(field_name) is True, f"package security path policy disabled {field_name}", failures)
        expect(path_policy.get("fallback_extraction_success_allowed") is False, "package security fallback extraction success widened", failures)

    negative_cases = contract.get("negative_cases", [])
    expect(isinstance(negative_cases, list), "package security negative cases missing", failures)
    if isinstance(negative_cases, list):
        case_ids = {str(case.get("case_id")) for case in negative_cases if isinstance(case, dict)}
        missing = sorted(REQUIRED_NEGATIVE_CASES - case_ids)
        expect(not missing, "package security negative case inventory missing: " + ", ".join(missing), failures)
        for case in negative_cases:
            if not isinstance(case, dict):
                continue
            expected_failure = str(case.get("expected_failure", ""))
            expect(expected_failure.startswith(f"{PACKAGE_MANAGER_TAMPER_CODE}:"), f"package security negative case diagnostic drifted: {case.get('case_id')}", failures)


def _validate_trust_policy(failures: list[str]) -> None:
    policy = default_trust_policy_payload()
    expect(collect_trust_policy_failures(policy) == [], "default package trust policy must validate", failures)

    mutated = default_trust_policy_payload()
    mutated["extraction_path_policy"]["reject_symlink_entries"] = False
    mutated["installer_update_key_policy"]["installer_key_state"] = "active"
    mutated["release_registry_trust_root_policy"]["fallback_trust_root_allowed"] = True
    mutated["trust_roots"][1]["key_state"] = "active"
    mutated["trust_roots"][2]["key_state"] = "active"
    policy_failures = collect_trust_policy_failures(mutated)
    for expected in (
        f"{PACKAGE_MANAGER_TAMPER_CODE}: extraction path policy disabled reject_symlink_entries",
        f"{PACKAGE_MANAGER_TAMPER_CODE}: installer/update key policy installer_key_state is not reserved-fail-closed",
        f"{PACKAGE_MANAGER_TAMPER_CODE}: release/registry fallback trust root allowed",
        f"{PACKAGE_MANAGER_TAMPER_CODE}: release trust root objc3c-release-signing-root-v1 is not reserved-fail-closed",
        f"{PACKAGE_MANAGER_TAMPER_CODE}: registry trust root objc3c-registry-signing-root-v1 is not reserved-fail-closed",
    ):
        expect(expected in policy_failures, f"package security trust policy did not fail closed: {expected}", failures)


def _validate_extraction_plan(failures: list[str]) -> None:
    safe_plan = package_extraction_plan_payload(
        plan_id="package-security-safe-plan",
        entries=[
            {
                "path": "tmp/artifacts/package-ecosystem/install-validation/clean-root/objc3c/packages/fixture/package.json",
                "entry_type": "file",
                "mutation": "write",
                "package_id": "fixture:package.security",
                "order": 0,
            }
        ],
    )
    expect(collect_extraction_plan_failures(safe_plan) == [], "safe package extraction plan failed", failures)

    unsafe_plan = package_extraction_plan_payload(
        plan_id="package-security-unsafe-plan",
        entries=[
            {"path": "/absolute/package.json", "entry_type": "file", "mutation": "write"},
            {"path": "../escape/package.json", "entry_type": "file", "mutation": "write"},
            {"path": "objc3c/packages/Foo/package.json", "entry_type": "file", "mutation": "write"},
            {"path": "objc3c/packages/foo/package.json", "entry_type": "file", "mutation": "write"},
            {"path": "objc3c/packages/foo/package.json", "entry_type": "file", "mutation": "write"},
            {"path": "objc3c/packages/link", "entry_type": "symlink", "mutation": "write"},
            {"path": "objc3c/packages/existing/package.json", "entry_type": "file", "mutation": "write"},
        ],
    )
    plan_failures = collect_extraction_plan_failures(
        unsafe_plan,
        existing_paths=["objc3c/packages/existing/package.json"],
    )
    for expected in (
        "absolute package extraction path rejected",
        "parent traversal package extraction path rejected",
        "case-conflicting package extraction path rejected",
        "duplicate package extraction path rejected",
        "symlink package extraction path rejected",
        "overwrite package extraction path rejected",
    ):
        expect(any(expected in failure for failure in plan_failures), f"package extraction plan did not fail closed: {expected}", failures)


def main() -> int:
    failures: list[str] = []
    contract = load_json(CONTRACT_PATH)
    package_model_contract = load_json(PACKAGE_MODEL_CONTRACT_PATH)

    _validate_contract(contract, failures)
    _validate_source_tokens(failures)
    _validate_trust_policy(failures)
    _validate_extraction_plan(failures)

    required_actions = set(package_model_contract.get("required_public_actions", []))
    expect(
        "validate-package-security-hardening" in required_actions,
        "package security public replay action missing from package manager contract",
        failures,
    )

    payload = {
        "contract_id": "objc3c.package_ecosystem.package_security_hardening.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "contract": repo_rel(CONTRACT_PATH),
        "issue_refs": contract.get("issue_refs", []),
        "diagnostic_code": PACKAGE_MANAGER_TAMPER_CODE,
        "negative_case_count": len(contract.get("negative_cases", [])),
        "public_replay_action": "npm run objc3c -- validate-package-security-hardening",
        "claim_boundary": contract.get("claim_boundary"),
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload, sort_keys=True)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    if failures:
        print("objc3c-package-security-hardening: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("objc3c-package-security-hardening: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
