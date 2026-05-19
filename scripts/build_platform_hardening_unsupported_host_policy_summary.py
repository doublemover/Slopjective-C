#!/usr/bin/env python3
from __future__ import annotations

import json

from platform_hardening_contracts import (
    PLATFORM_RUNBOOK_PATH,
    UNSUPPORTED_HOST_POLICY_PATH,
    UNSUPPORTED_HOST_POLICY_SUMMARY_PATH,
    load_json_object,
    require_platform_hardening_blocker_metadata,
    require_platform_hardening_owner_policy,
    write_json,
    write_markdown_summary,
)


def main() -> int:
    policy = load_json_object(UNSUPPORTED_HOST_POLICY_PATH)
    owner_policy = require_platform_hardening_owner_policy(policy, surface_name="unsupported host fail-closed policy")
    blocker_metadata = require_platform_hardening_blocker_metadata(
        policy,
        surface_name="unsupported host fail-closed policy",
        required_blockers=("unsupported host attempted build package or install flow",),
    )
    runbook_text = PLATFORM_RUNBOOK_PATH.read_text(encoding="utf-8")
    allowed_fail_closed_actions = policy["allowed_fail_closed_non_build_actions"]

    checks = {
        "hard_fail_classes_present": len(policy["hard_fail_classes"]) >= 4,
        "allowed_fail_closed_non_build_actions_present": len(allowed_fail_closed_actions) >= 1,
        "forbidden_phrases_present": len(policy["forbidden_phrases"]) >= 3,
        "required_claims_present": len(policy["required_claims"]) >= 3,
        "runbook_mentions_unsupported_host_policy": "## Unsupported-Host Fail-Closed Policy" in runbook_text,
        "runbook_mentions_hard_fail_host_matrix": "host OS or host architecture outside the checked-in support matrix" in runbook_text,
        "runbook_mentions_allowed_fail_closed_behavior": "capability inspection and docs-only policy checks may still run" in runbook_text,
        "runbook_mentions_forbidden_supported_language": "`best effort supported`" in runbook_text and "`supported if LLVM is installed`" in runbook_text,
    }

    payload = {
        "contract_id": "objc3c.platform.hardening.unsupported.host.fail_closed.policy.summary.v1",
        "source_contract_id": policy["contract_id"],
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/build_platform_hardening_unsupported_host_policy_summary.py",
        "owner_policy": owner_policy,
        "blocker_metadata": blocker_metadata,
        "hard_fail_class_count": len(policy["hard_fail_classes"]),
        "allowed_fail_closed_non_build_action_count": len(allowed_fail_closed_actions),
        "forbidden_phrase_count": len(policy["forbidden_phrases"]),
        "required_claim_count": len(policy["required_claims"]),
        "checks": checks,
    }

    write_json(UNSUPPORTED_HOST_POLICY_SUMMARY_PATH, payload)
    write_markdown_summary(
        UNSUPPORTED_HOST_POLICY_SUMMARY_PATH.with_suffix(".md"),
        "Unsupported Host Policy Summary",
        (
            ("Contract", payload["source_contract_id"]),
            ("Hard-fail classes", payload["hard_fail_class_count"]),
            ("Allowed fail-closed non-build actions", payload["allowed_fail_closed_non_build_action_count"]),
            ("Forbidden phrases", payload["forbidden_phrase_count"]),
            ("Required claims", payload["required_claim_count"]),
            ("Status", payload["status"]),
        ),
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
