Set-StrictMode -Version Latest

function Get-ManifestProvenanceApplicationPackageFiles {
  return @(
    "scripts/materialize_objc3c_project_template.py",
    "scripts/materialize_objc3c_canonical_application_workspace.py",
    "scripts/check_application_architecture_template_harness.py",
    "scripts/check_objc3c_application_architecture_integration.py",
    "scripts/check_objc3c_runnable_application_architecture_end_to_end.py",
    "scripts/build_application_architecture_testing_boundary_inventory_summary.py",
    "scripts/build_application_architecture_testing_semantic_summary.py",
    "scripts/build_application_architecture_template_workspace_summary.py",
    "scripts/build_application_architecture_layering_summary.py",
    "scripts/build_application_architecture_artifact_contract_summary.py",
    "scripts/build_package_ecosystem_boundary_inventory_summary.py",
    "scripts/build_package_ecosystem_dependency_lock_policy_summary.py",
    "scripts/build_package_ecosystem_local_workspace_mirror_summary.py",
    "scripts/build_package_ecosystem_registry_publication_summary.py",
    "scripts/build_package_ecosystem_artifact_contract_summary.py",
    "scripts/package_ecosystem_contracts.py",
    "scripts/build_objc3c_package_lock.py",
    "scripts/check_objc3c_package_authoring_workflow.py",
    "scripts/build_objc3c_package_mirror.py",
    "scripts/check_objc3c_package_registry_mirror_reproducibility.py",
    "scripts/check_objc3c_package_ecosystem_integration.py",
    "scripts/check_objc3c_runnable_package_ecosystem_end_to_end.py"
  )
}

Export-ModuleMember -Function @("Get-ManifestProvenanceApplicationPackageFiles")
