Set-StrictMode -Version Latest

function Get-ManifestProvenanceAdoptionGovernanceFiles {
  return @(
    "scripts/render_objc3c_public_command_surface.py",
    "scripts/build_adoption_legibility_boundary_inventory_summary.py",
    "scripts/build_adoption_legibility_public_claim_policy_summary.py",
    "scripts/build_adoption_legibility_capability_comparison_summary.py",
    "scripts/build_adoption_legibility_adoption_replay_summary.py",
    "scripts/build_adoption_legibility_artifact_contract_summary.py",
    "scripts/build_objc3c_adoption_legibility_evidence.py",
    "scripts/check_objc3c_adoption_legibility_integration.py",
    "scripts/publish_objc3c_adoption_legibility_metadata.py",
    "scripts/build_governance_budget_inventory_summary.py",
    "scripts/build_governance_policy_summary.py",
    "scripts/build_governance_maintainer_review_summary.py",
    "scripts/build_governance_extension_review_policy_summary.py",
    "scripts/build_governance_extension_review_workflow_summary.py",
    "scripts/build_governance_stewardship_semantics_summary.py",
    "scripts/check_governance_sustainability_schema_surface.py",
    "scripts/check_governance_sustainability_budget_enforcement.py",
    "scripts/build_governance_anti_regression_summary.py",
    "scripts/build_governance_artifact_contract_summary.py",
    "scripts/check_objc3c_governance_sustainability_integration.py",
    "scripts/build_objc3c_governance_sustainability_evidence.py",
    "scripts/publish_objc3c_governance_sustainability_metadata.py"
  )
}

Export-ModuleMember -Function @("Get-ManifestProvenanceAdoptionGovernanceFiles")
