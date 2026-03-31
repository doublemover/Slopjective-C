#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "security_hardening" / "macro_package_provenance_trust_policy.json"
RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_security_hardening.md"
SEMA_PATH = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
PROCESS_PATH = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
RUNTIME_ACCEPTANCE_PATH = ROOT / "scripts" / "check_objc3c_runtime_acceptance.py"
OUT_DIR = ROOT / "tmp" / "reports" / "security-hardening" / "macro-trust-policy"
JSON_OUT = OUT_DIR / "macro_trust_policy_summary.json"
MD_OUT = OUT_DIR / "macro_trust_policy_summary.md"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON object expected at {path}")
    return payload


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    sema_text = SEMA_PATH.read_text(encoding="utf-8")
    process_text = PROCESS_PATH.read_text(encoding="utf-8")
    runtime_acceptance_text = RUNTIME_ACCEPTANCE_PATH.read_text(encoding="utf-8")

    checks = {
        "all_authoritative_code_paths_exist": all((ROOT / path).exists() for path in contract["authoritative_code_paths"]),
        "all_positive_fixtures_exist": all((ROOT / path).is_file() for path in contract["positive_fixture_paths"]),
        "all_negative_fixtures_exist": all((ROOT / path).is_file() for path in contract["negative_fixture_paths"]),
        "sema_mentions_required_attributes": all(attribute in sema_text for attribute in contract["required_attributes"]),
        "sema_mentions_required_enforcement": all(flag in sema_text for flag in contract["required_enforcement_flags"]),
        "process_mentions_host_process_cache_inputs": "metaprogramming macro host process/cache" in process_text,
        "runtime_acceptance_mentions_macro_package_provenance_surface": "runtime_metaprogramming_package_provenance_source_surface" in runtime_acceptance_text,
        "runtime_acceptance_mentions_macro_safety_surface": "macro_safety_surface" in runtime_acceptance_text,
        "runbook_mentions_macro_trust_semantics": "Current macro/package/provenance trust semantics:" in runbook_text,
        "runbook_mentions_non_claims": "remote package trust" in runbook_text or "remote provenance verification" in runbook_text,
    }

    payload = {
        "contract_id": "objc3c.security.hardening.macro.package.provenance.trust.policy.summary.v1",
        "source_contract_id": contract["contract_id"],
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/build_security_hardening_macro_trust_policy_summary.py",
        "authoritative_code_path_count": len(contract["authoritative_code_paths"]),
        "required_attribute_count": len(contract["required_attributes"]),
        "required_enforcement_flag_count": len(contract["required_enforcement_flags"]),
        "positive_fixture_count": len(contract["positive_fixture_paths"]),
        "negative_fixture_count": len(contract["negative_fixture_paths"]),
        "non_claim_count": len(contract["non_claims"]),
        "checks": checks,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Macro Package Provenance Trust Policy Summary\n\n"
        f"- Contract: `{payload['source_contract_id']}`\n"
        f"- Authoritative code paths: `{payload['authoritative_code_path_count']}`\n"
        f"- Required attributes: `{payload['required_attribute_count']}`\n"
        f"- Required enforcement flags: `{payload['required_enforcement_flag_count']}`\n"
        f"- Positive fixtures: `{payload['positive_fixture_count']}`\n"
        f"- Negative fixtures: `{payload['negative_fixture_count']}`\n"
        f"- Non-claims: `{payload['non_claim_count']}`\n"
        f"- Status: `{payload['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
