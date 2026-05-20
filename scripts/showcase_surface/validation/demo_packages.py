from __future__ import annotations

from typing import Any

from .constants import EXPECTED_EXAMPLE_IDS

EXPECTED_COVERAGE_DOMAINS = {
    "auroraBoard": "object-model",
    "signalMesh": "concurrency",
    "patchKit": "interop",
}
EXPECTED_EXIT_CODES = {
    "auroraBoard": 33,
    "signalMesh": 13,
    "patchKit": 7,
}
REQUIRED_PACKAGE_MANIFEST_FIELDS = [
    "showcase_demo_packages_manifest",
    "showcase_demo_packages",
    "copied_files",
]


def demo_packages_by_example(payload: dict[str, Any]) -> dict[str, dict[str, Any]]:
    packages = payload.get("packages", [])
    if not isinstance(packages, list):
        return {}
    return {
        str(entry.get("example_id")): entry
        for entry in packages
        if isinstance(entry, dict) and isinstance(entry.get("example_id"), str)
    }


def _portfolio_entries_by_id(portfolio_payload: dict[str, Any]) -> dict[str, dict[str, Any]]:
    examples = portfolio_payload.get("examples", [])
    if not isinstance(examples, list):
        return {}
    return {
        str(entry.get("id")): entry
        for entry in examples
        if isinstance(entry, dict) and isinstance(entry.get("id"), str)
    }


def _all_checked_in_inputs(paths: object) -> bool:
    return isinstance(paths, list) and all(
        isinstance(path, str)
        and path
        and not path.startswith("tmp/")
        and not path.startswith("artifacts/")
        for path in paths
    )


def validate_demo_packages_contract(
    payload: dict[str, Any],
    portfolio_payload: dict[str, Any],
) -> str | None:
    if payload.get("contract_id") != "objc3c.showcase.demo.packages.v1":
        return "demo packages contract_id drifted"
    if payload.get("schema_version") != 1:
        return "demo packages schema_version drifted"
    if payload.get("portfolio_manifest") != "showcase/portfolio.json":
        return "demo packages portfolio_manifest drifted"
    if payload.get("package_bridge") != "objc3c":
        return "demo packages package_bridge drifted"
    if payload.get("package_stage_root") != "tmp/pkg/objc3c-native-runnable-toolchain":
        return "demo packages package_stage_root drifted"

    source_authority = payload.get("source_authority")
    if not isinstance(source_authority, dict):
        return "demo packages source_authority missing"
    if source_authority.get("forbidden_source_roots") != ["tmp", "artifacts"]:
        return "demo packages forbidden source roots drifted"

    reproducibility_contract = payload.get("reproducibility_contract")
    if not isinstance(reproducibility_contract, dict):
        return "demo packages reproducibility_contract missing"
    if reproducibility_contract.get("lockfile") != "package-lock.json":
        return "demo packages lockfile drifted"
    if reproducibility_contract.get("offline_mirror_validation_action") != "validate-package-mirror":
        return "demo packages offline mirror action drifted"
    if reproducibility_contract.get("tamper_rejection_diagnostic") != "O3PKG8054":
        return "demo packages tamper diagnostic drifted"
    if reproducibility_contract.get("package_manifest_fields") != REQUIRED_PACKAGE_MANIFEST_FIELDS:
        return "demo packages package manifest fields drifted"

    public_actions = payload.get("public_actions")
    if public_actions != {
        "surface": "check-showcase-surface",
        "integrated": "validate-showcase",
        "packaged": "validate-runnable-showcase",
        "package": "package-runnable-toolchain",
        "lock": "build-package-lock",
        "mirror": "validate-package-mirror",
        "replay": "test-execution-replay",
    }:
        return "demo packages public actions drifted"

    package_entries = demo_packages_by_example(payload)
    if list(package_entries) != EXPECTED_EXAMPLE_IDS:
        return "demo package example ids drifted"

    portfolio_entries = _portfolio_entries_by_id(portfolio_payload)
    if list(portfolio_entries) != EXPECTED_EXAMPLE_IDS:
        return "showcase portfolio example ids drifted for demo packages"

    for example_id in EXPECTED_EXAMPLE_IDS:
        package = package_entries[example_id]
        portfolio_entry = portfolio_entries[example_id]
        if package.get("package_id") != f"showcase:{example_id}":
            return f"demo package package_id drifted for {example_id}"
        if package.get("coverage_domain") != EXPECTED_COVERAGE_DOMAINS[example_id]:
            return f"demo package coverage_domain drifted for {example_id}"
        if package.get("source") != portfolio_entry.get("source"):
            return f"demo package source drifted for {example_id}"
        if package.get("workspace_manifest") != portfolio_entry.get("workspace_manifest"):
            return f"demo package workspace_manifest drifted for {example_id}"
        if package.get("expected_exit_code") != EXPECTED_EXIT_CODES[example_id]:
            return f"demo package expected_exit_code drifted for {example_id}"
        if package.get("required_story_capabilities") != portfolio_entry.get("story_capabilities"):
            return f"demo package story capabilities drifted for {example_id}"
        if package.get("stdlib_modules") != portfolio_entry.get("stdlib_followup_modules"):
            return f"demo package stdlib modules drifted for {example_id}"
        if not _all_checked_in_inputs(package.get("manifest_inputs")):
            return f"demo package manifest inputs must be checked-in paths for {example_id}"
        smoke_commands = package.get("smoke_commands")
        if not isinstance(smoke_commands, list) or not all(
            isinstance(command, str) and command.startswith("npm run objc3c -- ")
            for command in smoke_commands
        ):
            return f"demo package smoke commands drifted for {example_id}"
        runtime_backed_behavior = package.get("runtime_backed_behavior")
        if not isinstance(runtime_backed_behavior, list) or not runtime_backed_behavior:
            return f"demo package runtime behavior missing for {example_id}"

    return None

