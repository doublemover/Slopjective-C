from __future__ import annotations

from pathlib import Path
from typing import Any

from .assertions import expect
from .constants import PACKAGE_CONTRACT_ID
from .models import ShowcasePackageSurface
from .tooling import load_json, normalize_rel_path


def package_path(package_root: Path, manifest_value: object) -> Path:
    return package_root / normalize_rel_path(str(manifest_value))


def require_file(path: Path, message: str) -> None:
    expect(path.is_file(), message)


def validate_tutorial_guides(package_root: Path, tutorial_guides: object) -> None:
    expect(
        isinstance(tutorial_guides, list) and tutorial_guides,
        "package manifest did not publish tutorial guides",
    )
    for tutorial_path in tutorial_guides:
        expect(
            isinstance(tutorial_path, str) and tutorial_path,
            "package manifest published a malformed tutorial guide path",
        )
        expect(
            (package_root / normalize_rel_path(tutorial_path)).is_file(),
            f"packaged runnable toolchain missing tutorial guide {tutorial_path}",
        )


def validate_bonus_experience_surfaces(bonus_experience_surfaces: object) -> None:
    expect(
        isinstance(bonus_experience_surfaces, dict),
        "package manifest did not publish bonus experience surface metadata",
    )
    assert isinstance(bonus_experience_surfaces, dict)
    expect(
        bonus_experience_surfaces.get("playground", {}).get("public_actions") == [
            "materialize-playground-workspace",
            "compile-objc3c",
            "inspect-playground-repro",
            "inspect-compile-observability",
            "trace-compile-stages",
        ],
        "package manifest playground action surface drifted",
    )
    expect(
        "scripts/probe_objc3c_llvm_capabilities.py"
        in bonus_experience_surfaces.get("runtime_inspector", {}).get("source_roots", []),
        "package manifest runtime inspector surface did not publish the capability probe source root",
    )
    expect(
        "docs/tutorials/getting_started.md"
        in bonus_experience_surfaces.get("template_and_demo_harness", {}).get("source_roots", []),
        "package manifest template/demo surface did not publish getting_started.md",
    )
    expect(
        "scripts/materialize_objc3c_project_template.py"
        in bonus_experience_surfaces.get("template_and_demo_harness", {}).get("source_roots", []),
        "package manifest template/demo surface did not publish the live template generator source root",
    )


def validate_bonus_tool_integration_surface(bonus_tool_integration_surface: object) -> None:
    expect(
        isinstance(bonus_tool_integration_surface, dict),
        "package manifest did not publish bonus-tool integration surface metadata",
    )
    assert isinstance(bonus_tool_integration_surface, dict)
    expect(
        bonus_tool_integration_surface.get("portfolio_contract") == "showcase/portfolio.json",
        "package manifest bonus-tool integration surface drifted from showcase/portfolio.json",
    )
    expect(
        "materialize-project-template" in bonus_tool_integration_surface.get("public_actions", []),
        "package manifest bonus-tool integration surface did not publish materialize-project-template",
    )


def validate_command_surfaces(command_surfaces: object) -> None:
    expect(isinstance(command_surfaces, dict), "package manifest missing command surfaces")
    assert isinstance(command_surfaces, dict)
    expect(
        command_surfaces.get("build_playground") == "npm run objc3c -- materialize-playground-workspace",
        "package manifest missing build_playground command surface",
    )
    expect(
        command_surfaces.get("build_template") == "npm run objc3c -- materialize-project-template",
        "package manifest missing build_template command surface",
    )
    expect(
        command_surfaces.get("inspect_bonus_tools") == "npm run objc3c -- inspect-bonus-tool-integration",
        "package manifest missing inspect_bonus_tools command surface",
    )
    expect(
        command_surfaces.get("inspect_playground") == "npm run objc3c -- inspect-playground-repro",
        "package manifest missing inspect_playground command surface",
    )
    expect(
        command_surfaces.get("inspect_benchmark") == "npm run objc3c -- benchmark-runtime-inspector",
        "package manifest missing inspect_benchmark command surface",
    )
    expect(
        command_surfaces.get("inspect_capabilities") == "npm run objc3c -- inspect-capability-explorer",
        "package manifest missing inspect_capabilities command surface",
    )
    expect(
        command_surfaces.get("inspect_runtime") == "npm run objc3c -- inspect-runtime-inspector",
        "package manifest missing inspect_runtime command surface",
    )
    expect(
        command_surfaces.get("trace_stages") == "npm run objc3c -- trace-compile-stages",
        "package manifest missing trace_stages command surface",
    )
    expect(
        command_surfaces.get("developer_tooling") == "npm run objc3c -- validate-developer-tooling",
        "package manifest missing developer_tooling command surface",
    )
    expect(
        command_surfaces.get("getting_started") == "npm run objc3c -- validate-getting-started",
        "package manifest missing getting_started command surface",
    )


def validate_showcase_portfolio(
    *,
    showcase_portfolio: Path,
    showcase_examples: list[Any],
) -> None:
    portfolio_payload = load_json(showcase_portfolio)
    expect(
        portfolio_payload.get("contract_id") == "objc3c.showcase.portfolio.surface.v1",
        "packaged showcase portfolio published the wrong contract id",
    )
    expect(
        [
            entry.get("id")
            for entry in portfolio_payload.get("examples", [])
            if isinstance(entry, dict)
        ]
        == [entry.get("example_id") for entry in showcase_examples if isinstance(entry, dict)],
        "packaged showcase portfolio drifted from the manifest example inventory",
    )


def load_showcase_package_surface(
    *,
    package_root: Path,
    manifest_path: Path,
) -> ShowcasePackageSurface:
    manifest = load_json(manifest_path)
    expect(
        manifest.get("contract_id") == PACKAGE_CONTRACT_ID,
        "runnable toolchain package manifest published the wrong contract id",
    )

    compile_wrapper = package_path(package_root, manifest["compile_wrapper"])
    runtime_library = package_path(package_root, manifest["runtime_library"])
    showcase_portfolio = package_path(package_root, manifest["showcase_portfolio"])
    showcase_readme = package_path(package_root, manifest["showcase_readme"])
    guided_walkthrough_manifest = package_path(
        package_root,
        manifest["guided_walkthrough_manifest"],
    )
    repo_superclean_surface = package_path(package_root, manifest["repo_superclean_surface"])
    capability_probe_script = package_path(package_root, manifest["capability_probe_script"])
    tutorial_guides = manifest.get("tutorial_guides")
    showcase_examples = manifest.get("showcase_examples")
    bonus_experience_surfaces = manifest.get("bonus_experience_surfaces")
    bonus_tool_integration_surface = manifest.get("bonus_tool_integration_surface")
    command_surfaces = manifest.get("command_surfaces", {})

    require_file(showcase_portfolio, "packaged runnable toolchain missing showcase portfolio")
    require_file(showcase_readme, "packaged runnable toolchain missing showcase README")
    require_file(
        guided_walkthrough_manifest,
        "packaged runnable toolchain missing guided walkthrough manifest",
    )
    require_file(
        repo_superclean_surface,
        "packaged runnable toolchain missing repo superclean surface contract",
    )
    require_file(capability_probe_script, "packaged runnable toolchain missing capability probe script")
    validate_tutorial_guides(package_root, tutorial_guides)
    expect(
        isinstance(showcase_examples, list) and showcase_examples,
        "package manifest did not publish showcase examples",
    )
    assert isinstance(showcase_examples, list)
    validate_bonus_experience_surfaces(bonus_experience_surfaces)
    validate_bonus_tool_integration_surface(bonus_tool_integration_surface)
    validate_command_surfaces(command_surfaces)
    validate_showcase_portfolio(
        showcase_portfolio=showcase_portfolio,
        showcase_examples=showcase_examples,
    )

    return ShowcasePackageSurface(
        manifest=manifest,
        compile_wrapper=compile_wrapper,
        runtime_library=runtime_library,
        showcase_portfolio=showcase_portfolio,
        showcase_readme=showcase_readme,
        guided_walkthrough_manifest=guided_walkthrough_manifest,
        repo_superclean_surface=repo_superclean_surface,
        capability_probe_script=capability_probe_script,
        showcase_examples=showcase_examples,
    )


__all__ = [
    "load_showcase_package_surface",
    "package_path",
    "require_file",
    "validate_bonus_experience_surfaces",
    "validate_bonus_tool_integration_surface",
    "validate_command_surfaces",
    "validate_showcase_portfolio",
    "validate_tutorial_guides",
]
