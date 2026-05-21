#!/usr/bin/env python3
from __future__ import annotations

from objc3c_tooling.paths import repo_rel
from platform_hardening_contracts import (
    BOUNDARY_INVENTORY_PATH,
    SUPPORT_EVIDENCE_SUMMARY_PATH,
    SUPPORT_TIER_POLICY_PATH,
    SUPPORTED_PLATFORMS_PATH,
    UNSUPPORTED_HOST_POLICY_PATH,
    build_support_evidence_summary,
    load_json_object,
    load_platform_toolchain_support_evidence,
    validate_platform_toolchain_support_evidence,
    write_json,
)


def main() -> int:
    boundary = load_json_object(BOUNDARY_INVENTORY_PATH)
    tier_policy = load_json_object(SUPPORT_TIER_POLICY_PATH)
    supported_platforms = load_json_object(SUPPORTED_PLATFORMS_PATH)
    unsupported_host_policy = load_json_object(UNSUPPORTED_HOST_POLICY_PATH)
    evidence = load_platform_toolchain_support_evidence()
    validate_platform_toolchain_support_evidence(
        evidence,
        boundary=boundary,
        supported_platforms=supported_platforms,
        tier_policy=tier_policy,
        unsupported_host_policy=unsupported_host_policy,
    )
    summary = build_support_evidence_summary(evidence)
    write_json(SUPPORT_EVIDENCE_SUMMARY_PATH, summary)
    print(f"summary_path: {repo_rel(SUPPORT_EVIDENCE_SUMMARY_PATH)}")
    print("objc3c-platform-support-evidence: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
