#!/usr/bin/env python3
from __future__ import annotations

import json
import re
from dataclasses import dataclass
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "security_hardening" / "macro_package_provenance_trust_policy.json"
RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_security_hardening.md"
SEMA_PATHS = [
    ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp",
    ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract_metaprogramming_surfaces.h",
    ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes_metaprogramming_macro_property_summaries.inc",
    ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes_integration_surface_function_builder_metadata_publication.inc",
]
PROCESS_PATHS = [
    ROOT / "native" / "objc3c" / "src" / "io" / "objc3_metaprogramming_macro_host_cache_artifact.cpp",
    ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process_metaprogramming_contracts.h",
]
RUNTIME_ACCEPTANCE_PATHS = [
    ROOT / "scripts" / "check_objc3c_runtime_acceptance.py",
    ROOT / "scripts" / "objc3c_runtime_acceptance" / "summary_metaprogramming_sections.py",
    ROOT / "scripts" / "objc3c_runtime_acceptance" / "domains" / "metaprogramming_macro_safety_assertions.py",
    ROOT / "scripts" / "objc3c_runtime_acceptance" / "domains" / "metaprogramming_macro_safety_negative_cases.py",
    ROOT / "scripts" / "objc3c_runtime_acceptance" / "domains" / "metaprogramming_semantic_surfaces.py",
]
OUT_DIR = ROOT / "tmp" / "reports" / "security-hardening" / "macro-trust-policy"
JSON_OUT = OUT_DIR / "macro_trust_policy_summary.json"
MD_OUT = OUT_DIR / "macro_trust_policy_summary.md"

ATTR_RE = re.compile(r"objc_(macro(?:_(?:package|provenance|cache_key|sandbox))?)\(named\(\"([^\"]+)\"\)\)")
SHA256_RE = re.compile(r"^sha256:[0-9a-f]{6,64}$")
FULL_SHA256_RE = re.compile(r"^sha256:[0-9a-f]{64}$")


@dataclass(frozen=True)
class MacroEntry:
    path: str
    line: int
    macro: str | None
    package: str | None
    provenance: str | None
    cache_key: str | None
    sandbox_policy: str | None
    pure_function: bool
    method_topology: bool

    def candidate(self) -> dict[str, str | None]:
        return {
            "macro": self.macro,
            "package": self.package,
            "provenance": self.provenance,
            "cache_key": self.cache_key,
            "sandbox_policy": self.sandbox_policy,
        }


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON object expected at {path}")
    return payload


def digest_is_full_sha256(value: Any) -> bool:
    return isinstance(value, str) and FULL_SHA256_RE.match(value) is not None


def provenance_token_is_supported(value: Any) -> bool:
    return isinstance(value, str) and SHA256_RE.match(value) is not None


def extract_macro_entries(path: Path) -> list[MacroEntry]:
    entries: list[MacroEntry] = []
    for line_no, line in enumerate(path.read_text(encoding="utf-8").splitlines(), start=1):
        if "objc_macro" not in line:
            continue
        attrs: dict[str, str] = {}
        for match in ATTR_RE.finditer(line):
            attrs[match.group(1)] = match.group(2)
        if not attrs:
            continue
        stripped = line.lstrip()
        entries.append(
            MacroEntry(
                path=repo_rel(path),
                line=line_no,
                macro=attrs.get("macro"),
                package=attrs.get("macro_package"),
                provenance=attrs.get("macro_provenance"),
                cache_key=attrs.get("macro_cache_key"),
                sandbox_policy=attrs.get("macro_sandbox"),
                pure_function=stripped.startswith("pure fn "),
                method_topology=stripped.startswith("-") or stripped.startswith("+"),
            )
        )
    return entries


def artifact_key(payload: dict[str, Any]) -> tuple[Any, Any, Any, Any]:
    return (
        payload.get("package"),
        payload.get("macro"),
        payload.get("provenance"),
        payload.get("cache_key"),
    )


def build_registry_indexes(registry: dict[str, Any]) -> dict[str, Any]:
    packages = {
        str(package["package"]): package
        for package in registry.get("packages", [])
        if isinstance(package, dict) and isinstance(package.get("package"), str)
    }
    trusted_keys = {
        str(key["key_id"]): key
        for key in registry.get("trusted_signing_keys", [])
        if isinstance(key, dict) and isinstance(key.get("key_id"), str)
    }
    signed_artifacts = {
        artifact_key(artifact): artifact
        for artifact in registry.get("signed_artifacts", [])
        if isinstance(artifact, dict)
    }
    revoked_artifacts = {
        artifact_key(revocation)
        for revocation in registry.get("revocations", [])
        if isinstance(revocation, dict)
    }
    return {
        "packages": packages,
        "trusted_keys": trusted_keys,
        "signed_artifacts": signed_artifacts,
        "revoked_artifacts": revoked_artifacts,
    }


def validate_replay_metadata(record: dict[str, Any]) -> str | None:
    replay = record.get("replay_metadata")
    if not isinstance(replay, dict):
        return "missing-replay-metadata"
    if replay.get("schema_version") != 1:
        return "invalid-replay-metadata"
    if replay.get("deterministic") is not True:
        return "invalid-replay-metadata"
    if not isinstance(replay.get("cache_namespace"), str) or not replay.get("cache_namespace"):
        return "invalid-replay-metadata"
    if not digest_is_full_sha256(replay.get("input_digest")):
        return "invalid-replay-metadata"
    if not digest_is_full_sha256(replay.get("output_digest")):
        return "invalid-replay-metadata"
    return None


def denial_reason(
    candidate: dict[str, Any],
    indexes: dict[str, Any],
    signature_record: dict[str, Any] | None = None,
) -> str | None:
    package_name = candidate.get("package")
    package = indexes["packages"].get(package_name)
    if not isinstance(package_name, str) or package is None:
        return "unknown-package"
    if package.get("trust_state") != "trusted":
        return "untrusted-package"
    if package.get("default_sandbox") != "deny":
        return "sandbox-default-not-deny"
    if package.get("network_allowed") is not False or package.get("filesystem_write_allowed") is not False:
        return "sandbox-policy-denied"
    if candidate.get("macro") not in package.get("allowed_macros", []):
        return "macro-not-allowed"
    if candidate.get("sandbox_policy") not in package.get("allowed_sandbox_policies", []):
        return "sandbox-policy-denied"
    if artifact_key(candidate) in indexes["revoked_artifacts"]:
        return "revoked-provenance"
    if not provenance_token_is_supported(candidate.get("provenance")):
        return "invalid-provenance-token"

    record = signature_record if signature_record is not None else indexes["signed_artifacts"].get(artifact_key(candidate))
    if not isinstance(record, dict):
        return "missing-signature"
    for field_name in ("package", "macro", "provenance", "cache_key", "sandbox_policy"):
        if record.get(field_name) != candidate.get(field_name):
            return "signature-binding-mismatch"

    signer_key_id = record.get("signer_key_id")
    signer = indexes["trusted_keys"].get(signer_key_id)
    if not isinstance(signer_key_id, str) or signer is None:
        return "untrusted-signer"
    if signer.get("trust_state") != "trusted":
        return "untrusted-signer"
    if package_name not in signer.get("allowed_packages", []):
        return "signer-package-not-allowed"
    if not digest_is_full_sha256(record.get("manifest_digest")):
        return "invalid-signature-metadata"
    if not digest_is_full_sha256(record.get("signature_digest")):
        return "invalid-signature-metadata"
    return validate_replay_metadata(record)


def source_entry_denial_reason(entry: MacroEntry, indexes: dict[str, Any]) -> str | None:
    if entry.macro is None:
        return "missing-macro-marker"
    if entry.package is None or entry.provenance is None:
        return "missing-package-or-provenance"
    if entry.cache_key is None:
        return "missing-cache-key"
    if entry.sandbox_policy is None:
        return "missing-sandbox-policy"
    if entry.method_topology:
        return "unsupported-callable-topology"
    if not entry.pure_function:
        return "nondeterministic-callable"
    return denial_reason(entry.candidate(), indexes)


def required_flags_present(contract: dict[str, Any], text: str) -> bool:
    return all(flag in text for flag in contract["required_enforcement_flags"])


def evaluate_trust_registry(
    contract: dict[str, Any],
    registry: dict[str, Any],
) -> tuple[dict[str, bool], dict[str, Any], list[str]]:
    indexes = build_registry_indexes(registry)
    failures: list[str] = []
    positive_entries: list[MacroEntry] = []
    negative_entries: list[MacroEntry] = []

    for raw_path in contract["positive_fixture_paths"]:
        positive_entries.extend(extract_macro_entries(ROOT / raw_path))
    for raw_path in contract["negative_fixture_paths"]:
        negative_entries.extend(extract_macro_entries(ROOT / raw_path))

    positive_failures = []
    for entry in positive_entries:
        reason = source_entry_denial_reason(entry, indexes)
        if reason is not None:
            positive_failures.append(f"{entry.path}:{entry.line}:{reason}")

    negative_allowed = []
    negative_denials = []
    for entry in negative_entries:
        reason = source_entry_denial_reason(entry, indexes)
        if reason is None:
            negative_allowed.append(f"{entry.path}:{entry.line}")
        else:
            negative_denials.append({"path": entry.path, "line": entry.line, "reason": reason})

    expected_denial_results: dict[str, str] = {}
    for case in registry.get("expected_denials", []):
        expect(isinstance(case, dict), "expected_denials entries must be objects")
        candidate = case.get("candidate")
        expect(isinstance(candidate, dict), "expected denial candidate must be an object")
        signature_record = case.get("signature_record")
        expect(signature_record is None or isinstance(signature_record, dict), "signature_record must be an object when present")
        actual_reason = denial_reason(candidate, indexes, signature_record)
        case_id = str(case.get("case_id"))
        expected_reason = str(case.get("expected_reason"))
        expected_denial_results[case_id] = actual_reason or "allowed"
        if actual_reason != expected_reason:
            failures.append(f"{case_id} expected {expected_reason}, got {actual_reason or 'allowed'}")

    required_trust = contract.get("required_trust_checks", [])
    fail_closed_requirements = registry.get("fail_closed_requirements", {})
    declared_requirement_checks = {
        str(check_name): fail_closed_requirements.get(check_name) is True
        for check_name in required_trust
    }
    trust_checks = {
        "registry_contract_id_matches": registry.get("contract_id")
        == "objc3c.security.hardening.macro.supply-chain.trust.registry.v1",
        "required_trust_checks_declared": all(
            fail_closed_requirements.get(check_name) is True for check_name in required_trust
        ),
        **declared_requirement_checks,
        "positive_fixture_macros_signed": bool(positive_entries) and not positive_failures,
        "negative_fixture_macros_denied": bool(negative_entries) and not negative_allowed,
        "expected_denials_enforced": not failures,
        "revocation_cases_present": any(
            case.get("expected_reason") == "revoked-provenance"
            for case in registry.get("expected_denials", [])
            if isinstance(case, dict)
        ),
        "replay_metadata_required": any(
            case.get("expected_reason") == "missing-replay-metadata"
            for case in registry.get("expected_denials", [])
            if isinstance(case, dict)
        ),
    }
    if positive_failures:
        failures.extend(f"positive fixture denied: {failure}" for failure in positive_failures)
    if negative_allowed:
        failures.extend(f"negative fixture allowed: {entry}" for entry in negative_allowed)

    details = {
        "trust_registry": registry.get("contract_id"),
        "positive_macro_entry_count": len(positive_entries),
        "negative_macro_entry_count": len(negative_entries),
        "negative_source_denials": negative_denials,
        "expected_denial_results": expected_denial_results,
        "expected_denial_count": len(registry.get("expected_denials", [])),
        "signed_artifact_count": len(registry.get("signed_artifacts", [])),
        "revocation_count": len(registry.get("revocations", [])),
    }
    return trust_checks, details, failures


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    trust_registry_path = ROOT / str(contract["macro_supply_chain_trust_registry"])
    registry = read_json(trust_registry_path)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    sema_text = "\n".join(path.read_text(encoding="utf-8") for path in SEMA_PATHS)
    process_text = "\n".join(path.read_text(encoding="utf-8") for path in PROCESS_PATHS)
    runtime_acceptance_text = "\n".join(path.read_text(encoding="utf-8") for path in RUNTIME_ACCEPTANCE_PATHS)

    trust_checks, trust_details, trust_failures = evaluate_trust_registry(contract, registry)
    checks = {
        "all_authoritative_code_paths_exist": all((ROOT / path).exists() for path in contract["authoritative_code_paths"]),
        "all_positive_fixtures_exist": all((ROOT / path).is_file() for path in contract["positive_fixture_paths"]),
        "all_negative_fixtures_exist": all((ROOT / path).is_file() for path in contract["negative_fixture_paths"]),
        "trust_registry_exists": trust_registry_path.is_file(),
        "sema_mentions_required_attributes": all(attribute in sema_text for attribute in contract["required_attributes"]),
        "sema_mentions_required_enforcement": required_flags_present(contract, sema_text),
        "process_mentions_host_process_cache_inputs": "metaprogramming macro host process/cache" in process_text,
        "runtime_acceptance_mentions_macro_package_provenance_surface": "runtime_metaprogramming_package_provenance_source_surface" in runtime_acceptance_text
        or "macro_package_provenance_positive.objc3" in runtime_acceptance_text,
        "runtime_acceptance_mentions_macro_safety_surface": "macro_safety_surface" in runtime_acceptance_text
        or "expect_macro_safety_surface" in runtime_acceptance_text,
        "runbook_mentions_macro_trust_semantics": "Current macro/package/provenance trust semantics:" in runbook_text,
        "runbook_mentions_non_claims": "remote package trust" in runbook_text or "remote provenance verification" in runbook_text,
        **trust_checks,
    }
    failures = [name for name, passed in checks.items() if not passed]
    failures.extend(trust_failures)

    payload = {
        "contract_id": "objc3c.security.hardening.macro.package.provenance.trust.policy.summary.v1",
        "source_contract_id": contract["contract_id"],
        "status": "PASS" if not failures else "FAIL",
        "runner_path": "scripts/build_security_hardening_macro_trust_policy_summary.py",
        "trust_registry_path": repo_rel(trust_registry_path),
        "authoritative_code_path_count": len(contract["authoritative_code_paths"]),
        "required_attribute_count": len(contract["required_attributes"]),
        "required_enforcement_flag_count": len(contract["required_enforcement_flags"]),
        "required_trust_check_count": len(contract["required_trust_checks"]),
        "positive_fixture_count": len(contract["positive_fixture_paths"]),
        "negative_fixture_count": len(contract["negative_fixture_paths"]),
        "non_claim_count": len(contract["non_claims"]),
        "checks": checks,
        "trust_policy": trust_details,
        "failures": failures,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    write_json_file(JSON_OUT, payload)
    MD_OUT.write_text(
        "# Macro Package Provenance Trust Policy Summary\n\n"
        f"- Contract: `{payload['source_contract_id']}`\n"
        f"- Trust registry: `{payload['trust_registry_path']}`\n"
        f"- Authoritative code paths: `{payload['authoritative_code_path_count']}`\n"
        f"- Required attributes: `{payload['required_attribute_count']}`\n"
        f"- Required enforcement flags: `{payload['required_enforcement_flag_count']}`\n"
        f"- Required trust checks: `{payload['required_trust_check_count']}`\n"
        f"- Positive fixtures: `{payload['positive_fixture_count']}`\n"
        f"- Negative fixtures: `{payload['negative_fixture_count']}`\n"
        f"- Signed artifacts: `{trust_details['signed_artifact_count']}`\n"
        f"- Expected denials: `{trust_details['expected_denial_count']}`\n"
        f"- Non-claims: `{payload['non_claim_count']}`\n"
        f"- Status: `{payload['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
