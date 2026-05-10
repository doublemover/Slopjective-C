Set-StrictMode -Version Latest

function New-RunnableToolchainPackageApplicationManifestSection {
  return [ordered]@{
    application_architecture_runbook = "docs/runbooks/objc3c_application_architecture_testing.md"
    application_architecture_boundary_inventory = "tests/tooling/fixtures/application_architecture_testing/boundary_inventory.json"
    application_architecture_testing_semantics = "tests/tooling/fixtures/application_architecture_testing/first_party_testing_semantics.json"
    application_architecture_template_semantics = "tests/tooling/fixtures/application_architecture_testing/project_template_workspace_semantics.json"
    application_architecture_layering_semantics = "tests/tooling/fixtures/application_architecture_testing/canonical_application_architecture_semantics.json"
    application_architecture_artifact_contract = "tests/tooling/fixtures/application_architecture_testing/artifact_contract.json"
    application_architecture_evidence_schema = "schemas/objc3c-application-architecture-evidence-summary-v1.schema.json"
    application_architecture_surface = [ordered]@{
      canonical_workspace_materializer = "scripts/materialize_objc3c_canonical_application_workspace.py"
      template_harness_checker = "scripts/check_application_architecture_template_harness.py"
      integration_validation = "scripts/check_objc3c_application_architecture_integration.py"
      runnable_end_to_end_validation = "scripts/check_objc3c_runnable_application_architecture_end_to_end.py"
    }
    application_architecture_scripts = [ordered]@{
      boundary_inventory_summary = "scripts/build_application_architecture_testing_boundary_inventory_summary.py"
      semantics_summary = "scripts/build_application_architecture_testing_semantic_summary.py"
      template_workspace_summary = "scripts/build_application_architecture_template_workspace_summary.py"
      layering_summary = "scripts/build_application_architecture_layering_summary.py"
      artifact_contract_summary = "scripts/build_application_architecture_artifact_contract_summary.py"
      template_harness_validation = "scripts/check_application_architecture_template_harness.py"
      canonical_workspace_materializer = "scripts/materialize_objc3c_canonical_application_workspace.py"
      integration_validation = "scripts/check_objc3c_application_architecture_integration.py"
      runnable_end_to_end_validation = "scripts/check_objc3c_runnable_application_architecture_end_to_end.py"
    }
    application_architecture_public_actions = @(
      "materialize-project-template",
      "materialize-canonical-application-workspace",
      "validate-application-architecture",
      "validate-runnable-application-architecture"
    )
    package_ecosystem_runbook = "docs/runbooks/objc3c_package_ecosystem.md"
    package_ecosystem_boundary_inventory = "tests/tooling/fixtures/package_ecosystem/boundary_inventory.json"
    package_ecosystem_dependency_lock_policy = "tests/tooling/fixtures/package_ecosystem/dependency_lock_policy.json"
    package_ecosystem_local_workspace_mirror_semantics = "tests/tooling/fixtures/package_ecosystem/local_workspace_mirror_semantics.json"
    package_ecosystem_registry_publication_semantics = "tests/tooling/fixtures/package_ecosystem/registry_publication_semantics.json"
    package_ecosystem_artifact_contract = "tests/tooling/fixtures/package_ecosystem/artifact_contract.json"
    package_ecosystem_package_authoring_contract = "tests/tooling/fixtures/package_ecosystem/package_authoring_workflow_contract.json"
    package_ecosystem_registry_mirror_contract = "tests/tooling/fixtures/package_ecosystem/registry_mirror_reproducibility_contract.json"
    package_ecosystem_lock_schema = "schemas/objc3c-package-lock-v1.schema.json"
    package_ecosystem_mirror_schema = "schemas/objc3c-package-offline-mirror-index-v1.schema.json"
    package_ecosystem_surface = [ordered]@{
      lock_generator = "scripts/build_objc3c_package_lock.py"
      authoring_validation = "scripts/check_objc3c_package_authoring_workflow.py"
      mirror_generator = "scripts/build_objc3c_package_mirror.py"
      mirror_validation = "scripts/check_objc3c_package_registry_mirror_reproducibility.py"
      integration_validation = "scripts/check_objc3c_package_ecosystem_integration.py"
      runnable_end_to_end_validation = "scripts/check_objc3c_runnable_package_ecosystem_end_to_end.py"
    }
    package_ecosystem_scripts = [ordered]@{
      boundary_inventory_summary = "scripts/build_package_ecosystem_boundary_inventory_summary.py"
      dependency_lock_policy_summary = "scripts/build_package_ecosystem_dependency_lock_policy_summary.py"
      local_workspace_mirror_summary = "scripts/build_package_ecosystem_local_workspace_mirror_summary.py"
      registry_publication_summary = "scripts/build_package_ecosystem_registry_publication_summary.py"
      artifact_contract_summary = "scripts/build_package_ecosystem_artifact_contract_summary.py"
      lock_generator = "scripts/build_objc3c_package_lock.py"
      authoring_validation = "scripts/check_objc3c_package_authoring_workflow.py"
      mirror_generator = "scripts/build_objc3c_package_mirror.py"
      mirror_validation = "scripts/check_objc3c_package_registry_mirror_reproducibility.py"
      integration_validation = "scripts/check_objc3c_package_ecosystem_integration.py"
      runnable_end_to_end_validation = "scripts/check_objc3c_runnable_package_ecosystem_end_to_end.py"
    }
    package_ecosystem_public_actions = @(
      "build-package-lock",
      "validate-package-authoring",
      "validate-package-mirror",
      "validate-package-ecosystem",
      "validate-runnable-package-ecosystem"
    )
  }
}

Export-ModuleMember -Function @(
  "New-RunnableToolchainPackageApplicationManifestSection"
)
