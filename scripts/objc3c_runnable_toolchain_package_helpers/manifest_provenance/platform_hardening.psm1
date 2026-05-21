Set-StrictMode -Version Latest

function Get-ManifestProvenancePlatformHardeningFiles {
  return @(
    "scripts/probe_objc3c_llvm_capabilities.py",
    "scripts/build_objc3c_platform_support_matrix.py",
    "scripts/platform_hardening_contracts.py",
    "scripts/check_objc3c_platform_hardening_integration.py",
    "scripts/check_objc3c_runnable_platform_hardening_end_to_end.py",
    "scripts/build_platform_hardening_boundary_inventory_summary.py",
    "scripts/build_platform_hardening_support_tier_policy_summary.py",
    "scripts/build_platform_hardening_unsupported_host_policy_summary.py",
    "scripts/build_platform_hardening_toolchain_archive_policy_summary.py",
    "scripts/check_platform_hardening_support_evidence.py",
    "scripts/build_platform_hardening_artifact_contract_summary.py",
    "scripts/check_platform_hardening_build_package_validation.py",
    "scripts/check_platform_hardening_toolchain_range_replay.py",
    "scripts/check_platform_hardening_install_matrix_integration.py",
    "schemas/objc3c-platform-toolchain-support-evidence-v1.schema.json",
    "tests/tooling/fixtures/platform_hardening/platform_toolchain_support_evidence.json"
  )
}

Export-ModuleMember -Function @("Get-ManifestProvenancePlatformHardeningFiles")
