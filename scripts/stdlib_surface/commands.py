from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_any as load_json


@dataclass(frozen=True)
class CommandSurfaceValidation:
    artifact_filenames: Any
    advanced_helper_modules: Any
    capability_demo_examples: Any


def validate_command_surfaces(
    *,
    root: Path,
    lowering_import_surface: dict[str, Any],
    advanced_helper_package_surface: dict[str, Any],
    program_surface: dict[str, Any],
    inventory_module_names: set[str],
) -> tuple[str | None, CommandSurfaceValidation | None]:
    artifact_filenames = lowering_import_surface.get("artifact_filenames")
    if artifact_filenames != {
        "object": "module.obj",
        "compile_manifest": "module.manifest.json",
        "runtime_registration_manifest": "module.runtime-registration-manifest.json",
    }:
        return "lowering/import surface artifact_filenames drifted", None

    import_surface = lowering_import_surface.get("import_surface")
    if import_surface != {
        "model": "canonical-spec-module-ids-map-to-identifier-safe-implementation-module-declarations",
        "module_imports_source": "stdlib/package_surface.json",
        "identity_fields": ["canonical_module", "implementation_module", "source_declaration"],
    }:
        return "lowering/import surface import_surface drifted", None

    if lowering_import_surface.get("public_actions") != [
        "check-stdlib-surface",
        "materialize-stdlib-workspace",
        "validate-stdlib-foundation",
        "validate-runnable-stdlib-foundation",
        "package-runnable-toolchain",
    ]:
        return "lowering/import surface public_actions drifted", None

    if advanced_helper_package_surface.get("contract_id") != "objc3c.stdlib.advanced_helper_package_surface.v1":
        return "advanced helper package surface contract_id drifted", None
    if advanced_helper_package_surface.get("schema_version") != 1:
        return "advanced helper package surface schema_version drifted", None
    if advanced_helper_package_surface.get("workspace_contract") != "stdlib/workspace.json":
        return "advanced helper package surface workspace_contract drifted", None
    if advanced_helper_package_surface.get("package_surface") != "stdlib/package_surface.json":
        return "advanced helper package surface package_surface drifted", None
    if advanced_helper_package_surface.get("advanced_architecture") != "stdlib/advanced_architecture.json":
        return "advanced helper package surface advanced_architecture drifted", None
    if advanced_helper_package_surface.get("module_inventory") != "stdlib/module_inventory.json":
        return "advanced helper package surface module_inventory drifted", None
    if advanced_helper_package_surface.get("machine_output_root") != "tmp/artifacts/stdlib":
        return "advanced helper package surface machine_output_root drifted", None
    if advanced_helper_package_surface.get("machine_report_root") != "tmp/reports/stdlib":
        return "advanced helper package surface machine_report_root drifted", None
    if advanced_helper_package_surface.get("package_stage_root") != "tmp/pkg/objc3c-native-runnable-toolchain":
        return "advanced helper package surface package_stage_root drifted", None
    if advanced_helper_package_surface.get("staged_manifest_fields") != [
        "stdlib_advanced_architecture",
        "stdlib_advanced_helper_package_surface",
        "advanced_helper_modules",
        "advanced_helper_command_surfaces",
        "advanced_helper_profile_gates",
    ]:
        return "advanced helper package surface staged_manifest_fields drifted", None
    if advanced_helper_package_surface.get("advanced_helper_command_surfaces") != {
        "check_stdlib_surface": "npm run objc3c -- check-stdlib-surface",
        "build_stdlib": "npm run objc3c -- materialize-stdlib-workspace",
        "validate_stdlib_advanced": "npm run objc3c -- validate-stdlib-advanced",
        "validate_runnable_stdlib_advanced": "npm run objc3c -- validate-runnable-stdlib-advanced",
        "package_runnable_toolchain": "npm run objc3c -- package-runnable-toolchain",
    }:
        return "advanced helper package surface command surfaces drifted", None
    if advanced_helper_package_surface.get("public_actions") != [
        "check-stdlib-surface",
        "materialize-stdlib-workspace",
        "validate-stdlib-advanced",
        "validate-runnable-stdlib-advanced",
        "package-runnable-toolchain",
    ]:
        return "advanced helper package surface public_actions drifted", None

    if program_surface.get("contract_id") != "objc3c.stdlib.program_surface.v1":
        return "program surface contract_id drifted", None
    if program_surface.get("schema_version") != 1:
        return "program surface schema_version drifted", None
    if program_surface.get("workspace_contract") != "stdlib/workspace.json":
        return "program surface workspace_contract drifted", None
    if program_surface.get("package_surface") != "stdlib/package_surface.json":
        return "program surface package_surface drifted", None
    if program_surface.get("advanced_architecture") != "stdlib/advanced_architecture.json":
        return "program surface advanced_architecture drifted", None
    if program_surface.get("stdlib_readme") != "stdlib/README.md":
        return "program surface stdlib_readme drifted", None
    if program_surface.get("runbook") != "docs/runbooks/objc3c_stdlib_program.md":
        return "program surface runbook drifted", None
    if program_surface.get("site_entry") != "site/src/index.body.md":
        return "program surface site_entry drifted", None
    if program_surface.get("tutorial_readme") != "docs/tutorials/README.md":
        return "program surface tutorial_readme drifted", None
    if program_surface.get("getting_started_readme") != "docs/tutorials/getting_started.md":
        return "program surface getting_started_readme drifted", None
    if program_surface.get("comparison_readme") != "docs/tutorials/objc2_swift_cpp_comparison.md":
        return "program surface comparison_readme drifted", None
    if program_surface.get("showcase_readme") != "showcase/README.md":
        return "program surface showcase_readme drifted", None
    if program_surface.get("showcase_portfolio") != "showcase/portfolio.json":
        return "program surface showcase_portfolio drifted", None
    if program_surface.get("guided_walkthrough_manifest") != "showcase/tutorial_walkthrough.json":
        return "program surface guided_walkthrough_manifest drifted", None
    if program_surface.get("machine_output_root") != "tmp/artifacts/stdlib":
        return "program surface machine_output_root drifted", None
    if program_surface.get("machine_report_root") != "tmp/reports/stdlib":
        return "program surface machine_report_root drifted", None
    if program_surface.get("package_stage_root") != "tmp/pkg/objc3c-native-runnable-toolchain":
        return "program surface package_stage_root drifted", None
    if (
        program_surface.get("publish_model")
        != "shared-runnable-toolchain-bundle-carries-live-stdlib-docs-tutorials-site-and-showcase-surfaces"
    ):
        return "program surface publish_model drifted", None
    if program_surface.get("publish_inputs") != [
        "stdlib/README.md",
        "docs/runbooks/objc3c_stdlib_program.md",
        "docs/tutorials/README.md",
        "docs/tutorials/getting_started.md",
        "docs/tutorials/objc2_swift_cpp_comparison.md",
        "showcase/README.md",
        "showcase/portfolio.json",
        "showcase/tutorial_walkthrough.json",
        "site/src/index.body.md",
    ]:
        return "program surface publish_inputs drifted", None
    if program_surface.get("staged_manifest_fields") != [
        "stdlib_program_surface",
        "stdlib_program_command_surfaces",
        "stdlib_program_publish_inputs",
        "stdlib_program_examples",
    ]:
        return "program surface staged_manifest_fields drifted", None
    if program_surface.get("workflow_surface") != {
        "report_root": "tmp/reports/stdlib",
        "showcase_report_root": "tmp/reports/showcase",
        "tutorial_report_root": "tmp/reports/tutorials",
        "integration_entrypoint": "validate-stdlib-program",
        "packaged_validation_entrypoint": "validate-runnable-stdlib-program",
        "integration_actions": [
            "check-documentation-surface",
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
    }:
        return "program surface workflow_surface drifted", None
    if program_surface.get("public_actions") != [
        "check-documentation-surface",
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
    ]:
        return "program surface public_actions drifted", None
    if program_surface.get("command_surfaces") != {
        "check_documentation_surface": "npm run objc3c -- check-documentation-surface",
        "check_showcase_surface": "npm run objc3c -- check-showcase-surface",
        "validate_getting_started": "npm run objc3c -- validate-getting-started",
        "validate_showcase": "npm run objc3c -- validate-showcase",
        "validate_runnable_showcase": "npm run objc3c -- validate-runnable-showcase",
        "validate_stdlib_foundation": "npm run objc3c -- validate-stdlib-foundation",
        "validate_runnable_stdlib_foundation": "npm run objc3c -- validate-runnable-stdlib-foundation",
        "validate_stdlib_program": "npm run objc3c -- validate-stdlib-program",
        "validate_runnable_stdlib_program": "npm run objc3c -- validate-runnable-stdlib-program",
        "inspect_capability_explorer": "npm run objc3c -- inspect-capability-explorer",
        "package_runnable_toolchain": "npm run objc3c -- package-runnable-toolchain",
    }:
        return "program surface command_surfaces drifted", None
    if program_surface.get("onboarding_policy") != {
        "entry_order": [
            "repo-health",
            "single-example-compile",
            "showcase-example-selection",
            "canonical-conversion-or-comparison-doc",
        ],
        "runnable_claim_rule": "only capabilities backed by checked-in compile and shared validation flows may be presented as runnable-now stories",
        "comparison_claim_rule": "comparison-only capabilities must be framed as actor-shaped comparison or canonical conversion guidance rather than runnable parity stories",
        "command_truth_rule": "package.json objc3c is the authoritative public npm entrypoint; workflow examples use npm run objc3c -- <action>",
        "machine_noise_rule": "tmp artifacts and legacy redirect material may not appear as the primary onboarding route",
    }:
        return "program surface onboarding_policy drifted", None

    program_live_paths = (
        "runbook",
        "stdlib_readme",
        "site_entry",
        "tutorial_readme",
        "getting_started_readme",
        "comparison_readme",
        "showcase_readme",
        "showcase_portfolio",
        "guided_walkthrough_manifest",
    )
    for path_key in program_live_paths:
        raw_path = program_surface.get(path_key)
        if not isinstance(raw_path, str) or not raw_path:
            return f"program surface {path_key} is malformed", None
        if not (root / raw_path).exists():
            return f"program surface live path missing: {raw_path}", None
    publish_inputs = program_surface.get("publish_inputs")
    if not isinstance(publish_inputs, list) or not publish_inputs:
        return "program surface publish_inputs missing", None
    for raw_path in publish_inputs:
        if not isinstance(raw_path, str) or not raw_path:
            return "program surface publish_inputs contains malformed path", None
        if not (root / raw_path).exists():
            return f"program surface publish input missing: {raw_path}", None

    capability_demo_examples = program_surface.get("capability_demo_examples")
    if not isinstance(capability_demo_examples, list) or len(capability_demo_examples) != 3:
        return "program surface missing capability_demo_examples", None
    showcase_portfolio = load_json(root / str(program_surface["showcase_portfolio"]))
    showcase_examples = showcase_portfolio.get("examples")
    if not isinstance(showcase_examples, list):
        return "showcase portfolio examples are malformed", None
    showcase_examples_by_id = {
        str(entry.get("id")): entry
        for entry in showcase_examples
        if isinstance(entry, dict) and isinstance(entry.get("id"), str)
    }
    for entry in capability_demo_examples:
        if not isinstance(entry, dict):
            return "program surface published a malformed capability demo entry", None
        demo_id = entry.get("id")
        source = entry.get("source")
        workspace_manifest = entry.get("workspace_manifest")
        stdlib_followup_modules = entry.get("stdlib_followup_modules")
        story_capabilities = entry.get("story_capabilities")
        if not isinstance(demo_id, str) or not demo_id:
            return "program surface capability demo id is malformed", None
        if not isinstance(source, str) or not source:
            return f"program surface source is malformed for {demo_id}", None
        if not isinstance(workspace_manifest, str) or not workspace_manifest:
            return f"program surface workspace_manifest is malformed for {demo_id}", None
        if not isinstance(stdlib_followup_modules, list) or not all(
            isinstance(value, str) and value for value in stdlib_followup_modules
        ):
            return f"program surface stdlib_followup_modules are malformed for {demo_id}", None
        if not isinstance(story_capabilities, list) or not all(
            isinstance(value, str) and value for value in story_capabilities
        ):
            return f"program surface story_capabilities are malformed for {demo_id}", None
        if not (root / source).is_file():
            return f"program surface source path missing for {demo_id}", None
        if not (root / workspace_manifest).is_file():
            return f"program surface workspace manifest missing for {demo_id}", None
        showcase_entry = showcase_examples_by_id.get(demo_id)
        if showcase_entry is None:
            return f"program surface referenced unknown showcase example {demo_id}", None
        if showcase_entry.get("source") != source:
            return f"program surface source drifted for {demo_id}", None
        if showcase_entry.get("workspace_manifest") != workspace_manifest:
            return f"program surface workspace_manifest drifted for {demo_id}", None
        if showcase_entry.get("stdlib_followup_modules") != stdlib_followup_modules:
            return f"program surface stdlib_followup_modules drifted for {demo_id}", None
        if showcase_entry.get("story_capabilities") != story_capabilities:
            return f"program surface story_capabilities drifted for {demo_id}", None
        for module_name in stdlib_followup_modules:
            if module_name not in inventory_module_names:
                return f"program surface referenced unknown stdlib module {module_name} for {demo_id}", None

    advanced_helper_modules = advanced_helper_package_surface.get("advanced_helper_modules")
    if not isinstance(advanced_helper_modules, list) or len(advanced_helper_modules) != 3:
        return "advanced helper package surface missing advanced_helper_modules", None
    for entry in advanced_helper_modules:
        if not isinstance(entry, dict):
            return "advanced helper package surface published a malformed module entry", None
        canonical_module = entry.get("canonical_module")
        implementation_module = entry.get("implementation_module")
        manifest = entry.get("manifest")
        source = entry.get("source")
        smoke_source = entry.get("smoke_source")
        if not all(
            isinstance(value, str) and value
            for value in (canonical_module, implementation_module, manifest, source, smoke_source)
        ):
            return "advanced helper package surface published a malformed module entry", None

    return (
        None,
        CommandSurfaceValidation(
            artifact_filenames=artifact_filenames,
            advanced_helper_modules=advanced_helper_modules,
            capability_demo_examples=capability_demo_examples,
        ),
    )
