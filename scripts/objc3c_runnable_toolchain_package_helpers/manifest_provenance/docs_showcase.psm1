Set-StrictMode -Version Latest

function Get-ManifestProvenanceDocsShowcaseFiles {
  return @(
    "showcase/README.md",
    "showcase/portfolio.json",
    "showcase/demo_packages.json",
    "showcase/tutorial_walkthrough.json",
    "showcase/auroraBoard/main.objc3",
    "showcase/auroraBoard/workspace.json",
    "showcase/signalMesh/main.objc3",
    "showcase/signalMesh/workspace.json",
    "showcase/patchKit/main.objc3",
    "showcase/patchKit/workspace.json",
    "docs/runbooks/objc3c_application_architecture_testing.md",
    "docs/runbooks/objc3c_adoption_legibility.md",
    "docs/runbooks/objc3c_governance_sustainability.md",
    "docs/runbooks/objc3c_package_ecosystem.md",
    "docs/runbooks/objc3c_conformance_corpus.md",
    "docs/runbooks/objc3c_compiler_throughput.md",
    "docs/runbooks/objc3c_developer_tooling.md",
    "docs/runbooks/objc3c_platform_hardening.md",
    "docs/runbooks/objc3c_public_command_surface.md",
    "docs/runbooks/objc3c_packaging_channels.md",
    "docs/runbooks/objc3c_release_foundation.md",
    "docs/runbooks/objc3c_release_operations.md",
    "docs/runbooks/objc3c_runtime_performance.md",
    "docs/runbooks/objc3c_stdlib_program.md",
    "docs/tutorials/README.md",
    "docs/tutorials/getting_started.md",
    "docs/tutorials/objc2_to_objc3_migration.md",
    "docs/tutorials/objc2_swift_cpp_comparison.md",
    "docs/tutorials/build_run_verify.md",
    "docs/tutorials/guided_walkthrough.md",
    "site/src/index.body.md"
  )
}

Export-ModuleMember -Function @("Get-ManifestProvenanceDocsShowcaseFiles")
