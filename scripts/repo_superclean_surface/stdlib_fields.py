"""Stdlib surface fields for the repo superclean checker."""

from __future__ import annotations

from .model import SurfaceField


STDLIB_SURFACE_FIELDS = (
    SurfaceField(
        "stdlib_foundation_surface",
        {
            "workspace_contract": "stdlib/workspace.json",
            "module_inventory": "stdlib/module_inventory.json",
            "stability_policy": "stdlib/stability_policy.json",
            "package_surface": "stdlib/package_surface.json",
            "core_architecture": "stdlib/core_architecture.json",
            "advanced_architecture": "stdlib/advanced_architecture.json",
            "semantic_policy": "stdlib/semantic_policy.json",
            "lowering_import_surface": "stdlib/lowering_import_surface.json",
            "advanced_helper_package_surface": "stdlib/advanced_helper_package_surface.json",
            "source_roots": [
                "stdlib/README.md",
                "stdlib/advanced_helper_package_surface.json",
                "stdlib/modules/objc3.core/module.objc3",
                "stdlib/modules/objc3.errors/module.objc3",
                "stdlib/modules/objc3.concurrency/module.objc3",
                "stdlib/modules/objc3.keypath/module.objc3",
                "stdlib/modules/objc3.system/module.objc3",
            ],
            "report_roots": [
                "tmp/artifacts/stdlib",
                "tmp/reports/stdlib",
                "tmp/pkg/objc3c-native-runnable-toolchain",
            ],
            "public_actions": [
                "check-stdlib-surface",
                "materialize-stdlib-workspace",
                "validate-stdlib-foundation",
                "validate-stdlib-advanced",
                "validate-runnable-stdlib-advanced",
                "validate-runnable-stdlib-foundation",
                "package-runnable-toolchain",
            ],
        },
        "stdlib_foundation_surface drifted",
    ),
    SurfaceField(
        "stdlib_program_surface",
        {
            "program_contract": "stdlib/program_surface.json",
            "stdlib_readme": "stdlib/README.md",
            "runbook": "docs/runbooks/objc3c_stdlib_program.md",
            "site_entry": "site/src/index.body.md",
            "publish_inputs": [
                "stdlib/README.md",
                "docs/runbooks/objc3c_stdlib_program.md",
                "docs/tutorials/README.md",
                "docs/tutorials/getting_started.md",
                "docs/tutorials/objc2_swift_cpp_comparison.md",
                "showcase/README.md",
                "showcase/portfolio.json",
                "showcase/tutorial_walkthrough.json",
                "site/src/index.body.md",
            ],
            "report_roots": [
                "tmp/artifacts/stdlib",
                "tmp/reports/stdlib",
                "tmp/pkg/objc3c-native-runnable-toolchain",
            ],
            "workflow_surface": {
                "report_root": "tmp/reports/stdlib",
                "showcase_report_root": "tmp/reports/showcase",
                "tutorial_report_root": "tmp/reports/tutorials",
                "integration_entrypoint": "validate-stdlib-program",
                "packaged_validation_entrypoint": "validate-runnable-stdlib-program",
                "integration_actions": [
                    "validate-getting-started",
                    "validate-showcase",
                    "validate-stdlib-foundation",
                    "inspect-capability-explorer",
                ],
                "release_actions": [
                    "validate-runnable-showcase",
                    "validate-runnable-stdlib-foundation",
                    "package-runnable-toolchain",
                ],
            },
            "public_actions": [
                "check-showcase-surface",
                "validate-getting-started",
                "validate-showcase",
                "validate-runnable-showcase",
                "validate-stdlib-foundation",
                "validate-runnable-stdlib-foundation",
                "validate-stdlib-program",
                "validate-runnable-stdlib-program",
                "inspect-capability-explorer",
                "package-runnable-toolchain",
            ],
        },
        "stdlib_program_surface drifted",
    ),
)
