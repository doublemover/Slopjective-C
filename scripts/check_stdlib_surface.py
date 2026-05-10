#!/usr/bin/env python3
"""Validate the checked-in stdlib boundary and canonical module inventory."""

from __future__ import annotations

import sys
from pathlib import Path

from objc3c_tooling.json_io import load_json_any as load_json
from objc3c_tooling.paths import repo_rel
from stdlib_surface.contracts import validate_document_headers
from stdlib_surface.inventory import validate_inventory_and_policy

from check_stdlib_surface_model import (
    StdlibSurfaceDocuments,
    StdlibSurfacePaths,
    StdlibSurfaceReport,
    write_stdlib_surface_report,
)


PATHS = StdlibSurfacePaths.from_script(Path(__file__))
ROOT = PATHS.root


def fail(message: str) -> int:
    print(f"stdlib-surface: FAIL\n- {message}", file=sys.stderr)
    return 1


def main() -> int:
    for surface_input in PATHS.required_inputs():
        if not surface_input.path.is_file():
            return fail(f"{surface_input.missing_message}: {repo_rel(surface_input.path)}")

    documents = StdlibSurfaceDocuments.load(PATHS)
    core_architecture = documents.core_architecture
    advanced_architecture = documents.advanced_architecture
    semantic_policy = documents.semantic_policy
    lowering_import_surface = documents.lowering_import_surface
    advanced_helper_package_surface = documents.advanced_helper_package_surface
    program_surface = documents.program_surface

    document_header_error = validate_document_headers(documents)
    if document_header_error is not None:
        return fail(document_header_error)

    inventory_error, inventory_validation = validate_inventory_and_policy(documents)
    if inventory_error is not None:
        return fail(inventory_error)
    if inventory_validation is None:
        raise RuntimeError("stdlib surface inventory validation did not return a payload")
    module_surfaces = inventory_validation.module_surfaces
    layers = inventory_validation.layers
    module_imports = inventory_validation.module_imports
    package_imports_by_module = inventory_validation.package_imports_by_module
    inventory_module_names = inventory_validation.inventory_module_names
    advanced_api_families = inventory_validation.advanced_api_families

    for module_surface in module_surfaces:
        for path_key in ("workspace_root", "source", "smoke_source", "manifest"):
            raw_path = module_surface.path_for(path_key)
            path = ROOT / raw_path
            if path_key == "workspace_root":
                if not path.is_dir():
                    return fail(f"missing module workspace root: {raw_path}")
            else:
                if not path.is_file():
                    return fail(f"missing module artifact: {raw_path}")
        manifest_payload = load_json(ROOT / module_surface.manifest)
        if manifest_payload.get("contract_id") != "objc3c.stdlib.module.surface.v1":
            return fail(f"module manifest contract_id drifted for {module_surface.module}")
        if manifest_payload.get("canonical_module") != module_surface.module:
            return fail(f"module manifest canonical_module drifted for {module_surface.module}")
        if manifest_payload.get("implementation_module") != module_surface.implementation_module:
            return fail(f"module manifest implementation_module drifted for {module_surface.module}")
        if manifest_payload.get("capability_id") != module_surface.capability_id:
            return fail(f"module manifest capability_id drifted for {module_surface.module}")
        if manifest_payload.get("workspace_root") != module_surface.workspace_root:
            return fail(f"module manifest workspace_root drifted for {module_surface.module}")
        if (
            manifest_payload.get("module_semver")
            != semantic_policy.get("module_semver", {}).get(module_surface.module)
        ):
            return fail(f"module manifest module_semver drifted for {module_surface.module}")
        if manifest_payload.get("source") != module_surface.source:
            return fail(f"module manifest source drifted for {module_surface.module}")
        if manifest_payload.get("smoke_source") != module_surface.smoke_source:
            return fail(f"module manifest smoke_source drifted for {module_surface.module}")
        source_text = (ROOT / module_surface.source).read_text(encoding="utf-8")
        expected_decl = module_surface.expected_source_declaration()
        if expected_decl not in source_text:
            return fail(f"module source declaration drifted for {module_surface.module}")
        package_import = package_imports_by_module.get(module_surface.module)
        if package_import is None:
            return fail(f"package surface missing canonical module {module_surface.module}")
        if package_import.implementation_module != module_surface.implementation_module:
            return fail(f"package surface implementation_module drifted for {module_surface.module}")
        if package_import.source_declaration != expected_decl:
            return fail(f"package surface source_declaration drifted for {module_surface.module}")
        manifest_api_families = manifest_payload.get("api_families")
        if not isinstance(manifest_api_families, list) or not all(
            isinstance(value, str) and value for value in manifest_api_families
        ):
            return fail(f"module manifest api_families malformed for {module_surface.module}")
        manifest_exports = manifest_payload.get("exports")
        if not isinstance(manifest_exports, list) or not all(
            isinstance(value, str) and value for value in manifest_exports
        ):
            return fail(f"module manifest exports malformed for {module_surface.module}")
        for export_name in manifest_exports:
            if export_name not in source_text:
                return fail(
                    f"module source missing exported symbol spelling {export_name} for {module_surface.module}"
                )

    architecture_live_paths = core_architecture.get("live_paths")
    if not isinstance(architecture_live_paths, list) or not architecture_live_paths:
        return fail("core architecture missing live_paths")
    for raw_path in architecture_live_paths:
        if not isinstance(raw_path, str) or not raw_path:
            return fail("core architecture live_paths entry malformed")
        path = ROOT / raw_path
        if not path.exists():
            return fail(f"core architecture live path missing: {raw_path}")

    advanced_live_paths = advanced_architecture.get("live_paths")
    if not isinstance(advanced_live_paths, list) or not advanced_live_paths:
        return fail("advanced architecture missing live_paths")
    for raw_path in advanced_live_paths:
        if not isinstance(raw_path, str) or not raw_path:
            return fail("advanced architecture live_paths entry malformed")
        path = ROOT / raw_path
        if not path.exists():
            return fail(f"advanced architecture live path missing: {raw_path}")

    architecture_api_families = core_architecture.get("api_families")
    if not isinstance(architecture_api_families, dict) or not architecture_api_families:
        return fail("core architecture missing api_families")
    inventory_modules_by_name = {module_surface.module: module_surface for module_surface in module_surfaces}
    for module_name, families in architecture_api_families.items():
        if not isinstance(module_name, str) or module_name not in inventory_modules_by_name:
            return fail(f"core architecture referenced unknown module {module_name}")
        if not isinstance(families, list) or not families:
            return fail(f"core architecture api_families malformed for {module_name}")
        manifest_payload = load_json(ROOT / inventory_modules_by_name[module_name].manifest)
        if manifest_payload.get("api_families") != families:
            return fail(f"module manifest api_families drifted for {module_name}")
    architecture_required_exports = core_architecture.get("required_exports")
    if not isinstance(architecture_required_exports, dict) or not architecture_required_exports:
        return fail("core architecture missing required_exports")
    for module_name, required_exports in architecture_required_exports.items():
        if not isinstance(module_name, str) or module_name not in inventory_modules_by_name:
            return fail(f"core architecture required_exports referenced unknown module {module_name}")
        if not isinstance(required_exports, list) or not required_exports:
            return fail(f"core architecture required_exports malformed for {module_name}")
        manifest_payload = load_json(ROOT / inventory_modules_by_name[module_name].manifest)
        if manifest_payload.get("exports") != required_exports:
            return fail(f"module manifest exports drifted for {module_name}")

    if not isinstance(advanced_api_families, dict) or not advanced_api_families:
        return fail("advanced architecture missing api_families")
    for module_name, families in advanced_api_families.items():
        if not isinstance(module_name, str) or module_name not in inventory_modules_by_name:
            return fail(f"advanced architecture referenced unknown module {module_name}")
        if not isinstance(families, list) or not families:
            return fail(f"advanced architecture api_families malformed for {module_name}")
        manifest_payload = load_json(ROOT / inventory_modules_by_name[module_name].manifest)
        if manifest_payload.get("api_families") != families:
            return fail(f"module manifest advanced api_families drifted for {module_name}")

    advanced_required_exports = advanced_architecture.get("required_exports")
    if not isinstance(advanced_required_exports, dict) or not advanced_required_exports:
        return fail("advanced architecture missing required_exports")
    for module_name, required_exports in advanced_required_exports.items():
        if not isinstance(module_name, str) or module_name not in inventory_modules_by_name:
            return fail(f"advanced architecture required_exports referenced unknown module {module_name}")
        if not isinstance(required_exports, list) or not required_exports:
            return fail(f"advanced architecture required_exports malformed for {module_name}")
        manifest_payload = load_json(ROOT / inventory_modules_by_name[module_name].manifest)
        if manifest_payload.get("exports") != required_exports:
            return fail(f"module manifest advanced exports drifted for {module_name}")

    semantic_module_semver = semantic_policy.get("module_semver")
    if not isinstance(semantic_module_semver, dict) or not semantic_module_semver:
        return fail("semantic policy missing module_semver")
    for module_name, version_payload in semantic_module_semver.items():
        if module_name not in inventory_modules_by_name:
            return fail(f"semantic policy referenced unknown module {module_name}")
        if not isinstance(version_payload, dict):
            return fail(f"semantic policy module_semver malformed for {module_name}")
        if version_payload != {"major": 1, "minor": 0, "patch": 0}:
            return fail(f"semantic policy module_semver drifted for {module_name}")

    error_semantics = semantic_policy.get("error_semantics")
    if not isinstance(error_semantics, dict):
        return fail("semantic policy missing error_semantics")
    if error_semantics.get("result_ok_tag") != 1 or error_semantics.get("result_err_tag") != 2:
        return fail("semantic policy result tag values drifted")
    if error_semantics.get("result_bridge_diagnostic") != (
        "returns 0 when option presence matches the provided result tag, otherwise 30601"
    ):
        return fail("semantic policy result_bridge_diagnostic drifted")
    if error_semantics.get("text_data_compatibility_diagnostic") != (
        "returns 0 when unit_count equals byte_count, otherwise 30602"
    ):
        return fail("semantic policy text_data_compatibility_diagnostic drifted")

    keypath_semantics = semantic_policy.get("keypath_semantics")
    if not isinstance(keypath_semantics, dict):
        return fail("semantic policy missing keypath_semantics")
    if keypath_semantics.get("text_compatibility_diagnostic") != (
        "returns 0 when text_units is at least component_count, otherwise 30603"
    ):
        return fail("semantic policy text_compatibility_diagnostic drifted")
    if keypath_semantics.get("typed_keypath_metadata") != (
        "remains count-and-component preserving until runtime-backed keypath metadata lands"
    ):
        return fail("semantic policy typed_keypath_metadata drifted")
    if keypath_semantics.get("reflection_interop") != (
        "must preserve the caller-visible keypath component count and diagnostic behavior across module boundaries"
    ):
        return fail("semantic policy reflection_interop drifted")
    if keypath_semantics.get("runtime_composition_adapter") != (
        "must not invent ownership or allocation semantics beyond the checked-in keypath component and compatibility helpers"
    ):
        return fail("semantic policy runtime_composition_adapter drifted")

    concurrency_semantics = semantic_policy.get("concurrency_semantics")
    if not isinstance(concurrency_semantics, dict):
        return fail("semantic policy missing concurrency_semantics")
    if concurrency_semantics.get("spawn_token") != (
        "returns seed plus 1 as the current deterministic child-spawn token placeholder"
    ):
        return fail("semantic policy spawn_token drifted")
    if concurrency_semantics.get("cancellation_checkpoint") != (
        "returns 1 when the provided flag is nonzero and 0 otherwise"
    ):
        return fail("semantic policy cancellation_checkpoint drifted")
    if concurrency_semantics.get("family_growth_rule") != (
        "structured-child-spawn detached-spawn join-and-wait task-group-scope cancellation-observation and executor-hop helpers may grow additively inside objc3.concurrency"
    ):
        return fail("semantic policy concurrency family_growth_rule drifted")
    if concurrency_semantics.get("layering_rule") != (
        "objc3.concurrency may depend only on objc3.core and objc3.errors within stdlib major version 1"
    ):
        return fail("semantic policy concurrency layering_rule drifted")

    system_semantics = semantic_policy.get("system_semantics")
    if not isinstance(system_semantics, dict):
        return fail("semantic policy missing system_semantics")
    if system_semantics.get("resource_token") != (
        "returns seed plus 4 as the current deterministic strict-system resource token placeholder"
    ):
        return fail("semantic policy system resource_token drifted")
    if system_semantics.get("profile_gate") != (
        "objc3.system helpers remain reserved for Strict System claims and must not become unconditional Core imports"
    ):
        return fail("semantic policy system profile_gate drifted")
    if system_semantics.get("runtime_composition_hook") != (
        "strict-system runtime-composition helpers must be explicit profile-gated entrypoints rather than implicit side effects in core helpers"
    ):
        return fail("semantic policy system runtime_composition_hook drifted")
    if system_semantics.get("layering_rule") != (
        "objc3.system may depend on objc3.core objc3.errors objc3.concurrency and objc3.keypath but those modules may not depend on objc3.system"
    ):
        return fail("semantic policy system layering_rule drifted")

    artifact_filenames = lowering_import_surface.get("artifact_filenames")
    if artifact_filenames != {
        "object": "module.obj",
        "compile_manifest": "module.manifest.json",
        "runtime_registration_manifest": "module.runtime-registration-manifest.json",
    }:
        return fail("lowering/import surface artifact_filenames drifted")

    import_surface = lowering_import_surface.get("import_surface")
    if import_surface != {
        "model": "canonical-spec-module-ids-map-to-identifier-safe-implementation-module-declarations",
        "module_imports_source": "stdlib/package_surface.json",
        "identity_fields": ["canonical_module", "implementation_module", "source_declaration"],
    }:
        return fail("lowering/import surface import_surface drifted")

    if lowering_import_surface.get("public_actions") != [
        "check-stdlib-surface",
        "materialize-stdlib-workspace",
        "validate-stdlib-foundation",
        "validate-runnable-stdlib-foundation",
        "package-runnable-toolchain",
    ]:
        return fail("lowering/import surface public_actions drifted")

    if advanced_helper_package_surface.get("contract_id") != "objc3c.stdlib.advanced_helper_package_surface.v1":
        return fail("advanced helper package surface contract_id drifted")
    if advanced_helper_package_surface.get("schema_version") != 1:
        return fail("advanced helper package surface schema_version drifted")
    if advanced_helper_package_surface.get("workspace_contract") != "stdlib/workspace.json":
        return fail("advanced helper package surface workspace_contract drifted")
    if advanced_helper_package_surface.get("package_surface") != "stdlib/package_surface.json":
        return fail("advanced helper package surface package_surface drifted")
    if advanced_helper_package_surface.get("advanced_architecture") != "stdlib/advanced_architecture.json":
        return fail("advanced helper package surface advanced_architecture drifted")
    if advanced_helper_package_surface.get("module_inventory") != "stdlib/module_inventory.json":
        return fail("advanced helper package surface module_inventory drifted")
    if advanced_helper_package_surface.get("machine_output_root") != "tmp/artifacts/stdlib":
        return fail("advanced helper package surface machine_output_root drifted")
    if advanced_helper_package_surface.get("machine_report_root") != "tmp/reports/stdlib":
        return fail("advanced helper package surface machine_report_root drifted")
    if advanced_helper_package_surface.get("package_stage_root") != "tmp/pkg/objc3c-native-runnable-toolchain":
        return fail("advanced helper package surface package_stage_root drifted")
    if advanced_helper_package_surface.get("staged_manifest_fields") != [
        "stdlib_advanced_architecture",
        "stdlib_advanced_helper_package_surface",
        "advanced_helper_modules",
        "advanced_helper_command_surfaces",
        "advanced_helper_profile_gates",
    ]:
        return fail("advanced helper package surface staged_manifest_fields drifted")
    if advanced_helper_package_surface.get("advanced_helper_command_surfaces") != {
        "check_stdlib_surface": "npm run objc3c -- check-stdlib-surface",
        "build_stdlib": "npm run objc3c -- materialize-stdlib-workspace",
        "validate_stdlib_advanced": "npm run objc3c -- validate-stdlib-advanced",
        "validate_runnable_stdlib_advanced": "npm run objc3c -- validate-runnable-stdlib-advanced",
        "package_runnable_toolchain": "npm run objc3c -- package-runnable-toolchain",
    }:
        return fail("advanced helper package surface command surfaces drifted")
    if advanced_helper_package_surface.get("public_actions") != [
        "check-stdlib-surface",
        "materialize-stdlib-workspace",
        "validate-stdlib-advanced",
        "validate-runnable-stdlib-advanced",
        "package-runnable-toolchain",
    ]:
        return fail("advanced helper package surface public_actions drifted")

    if program_surface.get("contract_id") != "objc3c.stdlib.program_surface.v1":
        return fail("program surface contract_id drifted")
    if program_surface.get("schema_version") != 1:
        return fail("program surface schema_version drifted")
    if program_surface.get("workspace_contract") != "stdlib/workspace.json":
        return fail("program surface workspace_contract drifted")
    if program_surface.get("package_surface") != "stdlib/package_surface.json":
        return fail("program surface package_surface drifted")
    if program_surface.get("advanced_architecture") != "stdlib/advanced_architecture.json":
        return fail("program surface advanced_architecture drifted")
    if program_surface.get("stdlib_readme") != "stdlib/README.md":
        return fail("program surface stdlib_readme drifted")
    if program_surface.get("runbook") != "docs/runbooks/objc3c_stdlib_program.md":
        return fail("program surface runbook drifted")
    if program_surface.get("site_entry") != "site/src/index.body.md":
        return fail("program surface site_entry drifted")
    if program_surface.get("tutorial_readme") != "docs/tutorials/README.md":
        return fail("program surface tutorial_readme drifted")
    if program_surface.get("getting_started_readme") != "docs/tutorials/getting_started.md":
        return fail("program surface getting_started_readme drifted")
    if program_surface.get("comparison_readme") != "docs/tutorials/objc2_swift_cpp_comparison.md":
        return fail("program surface comparison_readme drifted")
    if program_surface.get("showcase_readme") != "showcase/README.md":
        return fail("program surface showcase_readme drifted")
    if program_surface.get("showcase_portfolio") != "showcase/portfolio.json":
        return fail("program surface showcase_portfolio drifted")
    if program_surface.get("guided_walkthrough_manifest") != "showcase/tutorial_walkthrough.json":
        return fail("program surface guided_walkthrough_manifest drifted")
    if program_surface.get("machine_output_root") != "tmp/artifacts/stdlib":
        return fail("program surface machine_output_root drifted")
    if program_surface.get("machine_report_root") != "tmp/reports/stdlib":
        return fail("program surface machine_report_root drifted")
    if program_surface.get("package_stage_root") != "tmp/pkg/objc3c-native-runnable-toolchain":
        return fail("program surface package_stage_root drifted")
    if (
        program_surface.get("publish_model")
        != "shared-runnable-toolchain-bundle-carries-live-stdlib-docs-tutorials-site-and-showcase-surfaces"
    ):
        return fail("program surface publish_model drifted")
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
        return fail("program surface publish_inputs drifted")
    if program_surface.get("staged_manifest_fields") != [
        "stdlib_program_surface",
        "stdlib_program_command_surfaces",
        "stdlib_program_publish_inputs",
        "stdlib_program_examples",
    ]:
        return fail("program surface staged_manifest_fields drifted")
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
        return fail("program surface workflow_surface drifted")
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
        return fail("program surface public_actions drifted")
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
        return fail("program surface command_surfaces drifted")
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
        return fail("program surface onboarding_policy drifted")

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
            return fail(f"program surface {path_key} is malformed")
        if not (ROOT / raw_path).exists():
            return fail(f"program surface live path missing: {raw_path}")
    publish_inputs = program_surface.get("publish_inputs")
    if not isinstance(publish_inputs, list) or not publish_inputs:
        return fail("program surface publish_inputs missing")
    for raw_path in publish_inputs:
        if not isinstance(raw_path, str) or not raw_path:
            return fail("program surface publish_inputs contains malformed path")
        if not (ROOT / raw_path).exists():
            return fail(f"program surface publish input missing: {raw_path}")

    capability_demo_examples = program_surface.get("capability_demo_examples")
    if not isinstance(capability_demo_examples, list) or len(capability_demo_examples) != 3:
        return fail("program surface missing capability_demo_examples")
    showcase_portfolio = load_json(ROOT / str(program_surface["showcase_portfolio"]))
    showcase_examples = showcase_portfolio.get("examples")
    if not isinstance(showcase_examples, list):
        return fail("showcase portfolio examples are malformed")
    showcase_examples_by_id = {
        str(entry.get("id")): entry
        for entry in showcase_examples
        if isinstance(entry, dict) and isinstance(entry.get("id"), str)
    }
    for entry in capability_demo_examples:
        if not isinstance(entry, dict):
            return fail("program surface published a malformed capability demo entry")
        demo_id = entry.get("id")
        source = entry.get("source")
        workspace_manifest = entry.get("workspace_manifest")
        stdlib_followup_modules = entry.get("stdlib_followup_modules")
        story_capabilities = entry.get("story_capabilities")
        if not isinstance(demo_id, str) or not demo_id:
            return fail("program surface capability demo id is malformed")
        if not isinstance(source, str) or not source:
            return fail(f"program surface source is malformed for {demo_id}")
        if not isinstance(workspace_manifest, str) or not workspace_manifest:
            return fail(f"program surface workspace_manifest is malformed for {demo_id}")
        if not isinstance(stdlib_followup_modules, list) or not all(
            isinstance(value, str) and value for value in stdlib_followup_modules
        ):
            return fail(f"program surface stdlib_followup_modules are malformed for {demo_id}")
        if not isinstance(story_capabilities, list) or not all(
            isinstance(value, str) and value for value in story_capabilities
        ):
            return fail(f"program surface story_capabilities are malformed for {demo_id}")
        if not (ROOT / source).is_file():
            return fail(f"program surface source path missing for {demo_id}")
        if not (ROOT / workspace_manifest).is_file():
            return fail(f"program surface workspace manifest missing for {demo_id}")
        showcase_entry = showcase_examples_by_id.get(demo_id)
        if showcase_entry is None:
            return fail(f"program surface referenced unknown showcase example {demo_id}")
        if showcase_entry.get("source") != source:
            return fail(f"program surface source drifted for {demo_id}")
        if showcase_entry.get("workspace_manifest") != workspace_manifest:
            return fail(f"program surface workspace_manifest drifted for {demo_id}")
        if showcase_entry.get("stdlib_followup_modules") != stdlib_followup_modules:
            return fail(f"program surface stdlib_followup_modules drifted for {demo_id}")
        if showcase_entry.get("story_capabilities") != story_capabilities:
            return fail(f"program surface story_capabilities drifted for {demo_id}")
        for module_name in stdlib_followup_modules:
            if module_name not in inventory_module_names:
                return fail(f"program surface referenced unknown stdlib module {module_name} for {demo_id}")

    advanced_helper_modules = advanced_helper_package_surface.get("advanced_helper_modules")
    if not isinstance(advanced_helper_modules, list) or len(advanced_helper_modules) != 3:
        return fail("advanced helper package surface missing advanced_helper_modules")
    for entry in advanced_helper_modules:
        if not isinstance(entry, dict):
            return fail("advanced helper package surface published a malformed module entry")
        canonical_module = entry.get("canonical_module")
        implementation_module = entry.get("implementation_module")
        manifest = entry.get("manifest")
        source = entry.get("source")
        smoke_source = entry.get("smoke_source")
        if not all(
            isinstance(value, str) and value
            for value in (canonical_module, implementation_module, manifest, source, smoke_source)
        ):
            return fail("advanced helper package surface published a malformed module entry")

    write_stdlib_surface_report(
        StdlibSurfaceReport(
            paths=PATHS,
            canonical_modules=module_surfaces,
            layers=layers,
            module_imports=module_imports,
            api_families=architecture_api_families,
            advanced_api_families=advanced_api_families,
            required_exports=architecture_required_exports,
            advanced_required_exports=advanced_required_exports,
            module_semver=semantic_module_semver,
            artifact_filenames=artifact_filenames,
            advanced_helper_modules=advanced_helper_modules,
            capability_demo_examples=capability_demo_examples,
        )
    )
    print(f"summary_path: {repo_rel(PATHS.summary)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
