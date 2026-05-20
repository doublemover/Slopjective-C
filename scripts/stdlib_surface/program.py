from __future__ import annotations

from pathlib import Path
from typing import Any


def validate_program_surface(
    *,
    root: Path,
    program_surface: dict[str, Any],
) -> str | None:
    if program_surface.get("contract_id") != "objc3c.stdlib.program_surface.v1":
        return "program surface contract_id drifted"
    if program_surface.get("schema_version") != 1:
        return "program surface schema_version drifted"
    if program_surface.get("workspace_contract") != "stdlib/workspace.json":
        return "program surface workspace_contract drifted"
    if program_surface.get("package_surface") != "stdlib/package_surface.json":
        return "program surface package_surface drifted"
    if program_surface.get("advanced_architecture") != "stdlib/advanced_architecture.json":
        return "program surface advanced_architecture drifted"
    if program_surface.get("stdlib_readme") != "stdlib/README.md":
        return "program surface stdlib_readme drifted"
    if program_surface.get("runbook") != "docs/runbooks/objc3c_stdlib_program.md":
        return "program surface runbook drifted"
    if program_surface.get("site_entry") != "site/src/index.body.md":
        return "program surface site_entry drifted"
    if program_surface.get("tutorial_readme") != "docs/tutorials/README.md":
        return "program surface tutorial_readme drifted"
    if program_surface.get("getting_started_readme") != "docs/tutorials/getting_started.md":
        return "program surface getting_started_readme drifted"
    if program_surface.get("comparison_readme") != "docs/tutorials/objc2_swift_cpp_comparison.md":
        return "program surface comparison_readme drifted"
    if program_surface.get("showcase_readme") != "showcase/README.md":
        return "program surface showcase_readme drifted"
    if program_surface.get("showcase_portfolio") != "showcase/portfolio.json":
        return "program surface showcase_portfolio drifted"
    if program_surface.get("guided_walkthrough_manifest") != "showcase/tutorial_walkthrough.json":
        return "program surface guided_walkthrough_manifest drifted"
    if program_surface.get("machine_output_root") != "tmp/artifacts/stdlib":
        return "program surface machine_output_root drifted"
    if program_surface.get("machine_report_root") != "tmp/reports/stdlib":
        return "program surface machine_report_root drifted"
    if program_surface.get("package_stage_root") != "tmp/pkg/objc3c-native-runnable-toolchain":
        return "program surface package_stage_root drifted"
    if (
        program_surface.get("publish_model")
        != "shared-runnable-toolchain-bundle-carries-live-stdlib-docs-tutorials-site-and-showcase-surfaces"
    ):
        return "program surface publish_model drifted"
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
        return "program surface publish_inputs drifted"
    if program_surface.get("staged_manifest_fields") != [
        "stdlib_program_surface",
        "stdlib_program_command_surfaces",
        "stdlib_program_publish_inputs",
        "stdlib_program_examples",
    ]:
        return "program surface staged_manifest_fields drifted"
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
        return "program surface workflow_surface drifted"
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
        return "program surface public_actions drifted"
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
        return "program surface command_surfaces drifted"
    if program_surface.get("onboarding_policy") != {
        "entry_order": [
            "repo-health",
            "single-example-compile",
            "showcase-example-selection",
            "canonical-conversion-or-comparison-doc",
        ],
        "runnable_claim_rule": "only capabilities backed by checked-in compile and shared validation flows may be presented as runnable-now stories",
        "comparison_claim_rule": "comparison-only capabilities must be framed as canonical pattern-conversion guidance rather than runnable parity or retired-source support stories",
        "command_truth_rule": "package.json objc3c is the authoritative public npm entrypoint; workflow examples use npm run objc3c -- <action>",
        "machine_noise_rule": "tmp artifacts and archived redirect material may not appear as the primary onboarding route",
    }:
        return "program surface onboarding_policy drifted"

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
            return f"program surface {path_key} is malformed"
        if not (root / raw_path).exists():
            return f"program surface live path missing: {raw_path}"
    publish_inputs = program_surface.get("publish_inputs")
    if not isinstance(publish_inputs, list) or not publish_inputs:
        return "program surface publish_inputs missing"
    for raw_path in publish_inputs:
        if not isinstance(raw_path, str) or not raw_path:
            return "program surface publish_inputs contains malformed path"
        if not (root / raw_path).exists():
            return f"program surface publish input missing: {raw_path}"

    return None
