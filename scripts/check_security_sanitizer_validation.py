#!/usr/bin/env python3
"""Validate sanitizer-backed runtime/compiler security hardening coverage."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.json_io import write_report_json
from objc3c_tooling.paths import repo_rel
from scripts.objc3c_workflow.public_command_api import public_workflow_action_names


CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "security_hardening"
    / "sanitizer_validation_contract.json"
)
SOURCE_SURFACE = (
    ROOT / "tests" / "tooling" / "fixtures" / "security_hardening" / "source_surface.json"
)
WORKFLOW_SURFACE = (
    ROOT / "tests" / "tooling" / "fixtures" / "security_hardening" / "workflow_surface.json"
)
PACKAGE_INSTALL_MODEL_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "security_hardening"
    / "sanitizer_package_install_model_contract.json"
)
SUMMARY_PATH = ROOT / "tmp" / "reports" / "security-hardening" / "sanitizer-validation-summary.json"

CONTRACT_ID = "objc3c.security.hardening.sanitizer.validation.contract.v1"
PACKAGE_INSTALL_MODEL_CONTRACT_ID = "objc3c.security.hardening.sanitizer.package-install-model.v1"
SUMMARY_CONTRACT_ID = "objc3c.security.hardening.sanitizer.validation.summary.v1"
REQUIRED_SANITIZERS = {"ASan", "UBSan"}
REQUIRED_COVERAGE_SURFACES = {"native_runtime", "native_compiler"}
RELEASE_RUNTIME_PACKAGE_IDS = [
    "org.objc3c.runtime:objc3c-runtime-release",
    "org.objc3c.runtime:objc3c-runtime-linux-x64-release",
    "org.objc3c.runtime:objc3c-runtime-darwin-arm64-release",
]
REQUIRED_PACKAGE_VARIANTS = {
    "objc3c.toolchain.sanitizer.address": {
        "issue_ref": 8230,
        "sanitizer": "address",
        "package_variant_row_id": "objc3c.package.sanitizer.asan.reserved",
        "package_channel_id": "windows-x64-sanitizer-asan",
        "package_id": "org.objc3c.runtime:objc3c-runtime-asan",
        "compiler_flags": {"-fsanitize=address", "-fno-omit-frame-pointer"},
        "linker_flags": {"-fsanitize=address"},
        "runtime_library_ids": ["objc3-runtime", "clang_rt.asan"],
        "runtime_library_manifest_path": "share/objc3c/sanitizer/asan-runtime-libraries.json",
        "runtime_library_artifacts": [
            "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic-x86_64.dll",
            "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic-x86_64.lib",
            "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic_runtime_thunk-x86_64.lib",
        ],
        "required_metadata_fields": {
            "target_platform_id",
            "sanitizer",
            "runtime_library_ids",
            "runtime_library_manifest_path",
            "runtime_library_manifest_digest",
            "runtime_library_artifacts",
            "missing_runtime_behavior",
            "compiler_flags",
            "linker_flags",
            "environment",
            "package_layout_contract",
            "runtime_probe_contract",
            "install_selection_contract",
            "runtime_mixing_rejection_contract",
            "environment_contract",
            "metadata_freshness_contract",
            "release_runtime_package_ids",
            "release_runtime_mixing_allowed",
            "expected_detection_records",
            "unsupported_host_diagnostics",
        },
        "expected_detection_record_ids": {
            "objc3c.sanitizer.address.heap-use-after-free",
            "objc3c.sanitizer.address.container-overflow",
        },
    },
    "objc3c.toolchain.sanitizer.undefined": {
        "issue_ref": 8231,
        "sanitizer": "undefined",
        "package_variant_row_id": "objc3c.package.sanitizer.ubsan.reserved",
        "package_channel_id": "windows-x64-sanitizer-ubsan",
        "package_id": "org.objc3c.runtime:objc3c-runtime-ubsan",
        "compiler_flags": {"-fsanitize=undefined", "-fno-omit-frame-pointer"},
        "linker_flags": {"-fsanitize=undefined"},
        "runtime_library_ids": ["objc3-runtime", "clang_rt.ubsan"],
        "runtime_library_manifest_path": "share/objc3c/sanitizer/ubsan-runtime-libraries.json",
        "runtime_library_artifacts": [
            "artifacts/runtime/sanitizer/undefined/clang_rt.ubsan_standalone-x86_64.lib",
            "artifacts/runtime/sanitizer/undefined/clang_rt.ubsan_standalone_cxx-x86_64.lib",
        ],
        "required_metadata_fields": {
            "target_platform_id",
            "sanitizer",
            "runtime_library_ids",
            "runtime_library_manifest_path",
            "runtime_library_manifest_digest",
            "runtime_library_artifacts",
            "missing_runtime_behavior",
            "compiler_flags",
            "linker_flags",
            "trap_or_recover_mode",
            "package_layout_contract",
            "runtime_probe_contract",
            "install_selection_contract",
            "runtime_mixing_rejection_contract",
            "environment_contract",
            "trap_recover_contract",
            "metadata_freshness_contract",
            "release_runtime_package_ids",
            "release_runtime_mixing_allowed",
            "expected_detection_records",
            "unsupported_host_diagnostics",
        },
        "expected_detection_record_ids": {
            "objc3c.sanitizer.undefined.signed-integer-overflow",
            "objc3c.sanitizer.undefined.invalid-shift",
        },
    },
}
REQUIRED_PACKAGE_EVIDENCE = {"build", "package", "install", "execution"}
REQUIRED_UNSUPPORTED_DIAGNOSTIC_BLOCKS = {"package", "install", "execution", "publication"}
REQUIRED_PACKAGE_INSTALL_NEGATIVE_KINDS = {
    "missing-install-receipt-field",
    "mixed-runtime",
    "missing-sanitizer-runtime",
    "stale-package-metadata",
    "unsupported-host",
    "default-release-misuse",
}
INSTALL_RECEIPT_SCHEMA_PATH = "schemas/objc3c-package-install-receipt-v1.schema.json"
INSTALL_RECEIPT_CONTRACT_ID = "objc3c.packaging.channels.install-receipt.v1"
SANITIZER_INSTALL_RECEIPT_FIELD = "sanitizer_package_variant"
REQUIRED_SANITIZER_INSTALL_RECEIPT_FIELDS = {
    "package_id",
    "package_variant_row_id",
    "package_channel_id",
    "target_platform_id",
    "sanitizer",
    "runtime_library_ids",
    "metadata_manifest_path",
    "metadata_digest",
    "runtime_library_manifest_path",
    "runtime_library_manifest_digest",
    "runtime_library_artifacts",
    "missing_runtime_behavior",
    "selected_runtime_variant",
    "install_selector",
    "native_execution_contract",
    "support_truth",
    "native_execution_claimed",
    SANITIZER_INSTALL_RECEIPT_FIELD,
}
REQUIRED_NATIVE_EXECUTION_RECORD_FIELDS = {
    "executable_path",
    "target_platform_id",
    "sanitizer",
    "runtime_library_ids",
    "runtime_library_artifacts",
    "environment",
    "exit_code",
    "diagnostic_records",
}
REQUIRED_RUNTIME_PROBE_INPUTS = {
    "target_platform_id",
    "llvm_runtime_root",
    "runtime_library_manifest_path",
    "runtime_library_artifacts",
    "package_root_layout",
    "runtime_library_ids",
}
REQUIRED_METADATA_FRESHNESS_INPUTS = {
    "package_variant_row_id",
    "package_channel_id",
    "package_id",
    "runtime_library_ids",
    "runtime_library_manifest_path",
    "runtime_library_artifacts",
    "compiler_flags",
    "linker_flags",
}


def fail(message: str) -> int:
    print(f"security-sanitizer-validation: {message}", file=sys.stderr)
    return 1


def require_path(raw_path: str, *, file: bool = True) -> Path:
    path = ROOT / raw_path
    if file and not path.is_file():
        raise RuntimeError(f"missing required file {raw_path}")
    if not file and not path.exists():
        raise RuntimeError(f"missing required path {raw_path}")
    return path


def require_text_tokens(path: Path, tokens: list[str], label: str) -> list[str]:
    text = path.read_text(encoding="utf-8")
    missing = [token for token in tokens if token not in text]
    if missing:
        raise RuntimeError(f"{label} missing tokens in {repo_rel(path)}: {', '.join(missing)}")
    return tokens


def require_action_surfaces(action_name: str) -> None:
    registered_actions = set(public_workflow_action_names())
    if action_name not in registered_actions:
        raise RuntimeError(f"workflow registry missing public action {action_name}")

    source_surface = load_json(SOURCE_SURFACE)
    workflow_surface = load_json(WORKFLOW_SURFACE)
    for payload, list_name in (
        (source_surface, "public_actions"),
        (source_surface.get("owner_policy", {}), "owned_actions"),
        (workflow_surface, "required_actions"),
        (workflow_surface.get("owner_policy", {}), "owned_actions"),
        (workflow_surface, "validation_child_actions"),
        (workflow_surface.get("owner_policy", {}), "workflow_child_actions"),
    ):
        actions = payload.get(list_name) if isinstance(payload, dict) else None
        if not isinstance(actions, list) or action_name not in actions:
            raise RuntimeError(f"{list_name} missing {action_name}")


def require_object(value: Any, label: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        raise RuntimeError(f"{label} must be an object")
    return value


def require_string_list(value: Any, label: str) -> list[str]:
    if not isinstance(value, list) or not value:
        raise RuntimeError(f"{label} must be a non-empty list")
    return [str(item) for item in value]


def expected_metadata_manifest_path(sanitizer_name: str) -> str:
    if sanitizer_name == "address":
        return "share/objc3c/sanitizer/asan-metadata.json"
    if sanitizer_name == "undefined":
        return "share/objc3c/sanitizer/ubsan-metadata.json"
    raise RuntimeError(f"unknown sanitizer {sanitizer_name}")


def expected_runtime_library_manifest_path(sanitizer_name: str) -> str:
    if sanitizer_name == "address":
        return "share/objc3c/sanitizer/asan-runtime-libraries.json"
    if sanitizer_name == "undefined":
        return "share/objc3c/sanitizer/ubsan-runtime-libraries.json"
    raise RuntimeError(f"unknown sanitizer {sanitizer_name}")


def expected_runtime_artifact_paths(sanitizer_name: str) -> list[str]:
    if sanitizer_name == "address":
        return list(REQUIRED_PACKAGE_VARIANTS["objc3c.toolchain.sanitizer.address"]["runtime_library_artifacts"])  # type: ignore[arg-type]
    if sanitizer_name == "undefined":
        return list(REQUIRED_PACKAGE_VARIANTS["objc3c.toolchain.sanitizer.undefined"]["runtime_library_artifacts"])  # type: ignore[arg-type]
    raise RuntimeError(f"unknown sanitizer {sanitizer_name}")


def expected_install_receipt_fields(sanitizer_name: str) -> set[str]:
    fields = set(REQUIRED_SANITIZER_INSTALL_RECEIPT_FIELDS)
    if sanitizer_name == "undefined":
        fields.add("trap_or_recover_mode")
    return fields


def normalize_runtime_id(value: str) -> str:
    return value.replace("-", "").replace("_", "").replace(".", "").lower()


def path_matches_runtime_library_id(path: str, runtime_library_id: str) -> bool:
    return normalize_runtime_id(runtime_library_id) in normalize_runtime_id(Path(path).name)


def require_exact_string_set(
    actual: list[str],
    expected: list[str],
    label: str,
) -> None:
    actual_set = set(actual)
    expected_set = set(expected)
    if actual_set != expected_set:
        missing = sorted(expected_set - actual_set)
        unexpected = sorted(actual_set - expected_set)
        raise RuntimeError(f"{label} drifted; missing={missing}, unexpected={unexpected}")


def validate_install_receipt_contract(
    variant_id: str,
    package_runtime_contract: dict[str, Any],
    *,
    sanitizer_name: str,
) -> dict[str, object]:
    install_receipt = require_object(
        package_runtime_contract.get("install_receipt_contract"),
        f"{variant_id}.package_runtime_contract.install_receipt_contract",
    )
    if install_receipt.get("schema") != INSTALL_RECEIPT_SCHEMA_PATH:
        raise RuntimeError(f"{variant_id} install receipt schema drifted")
    schema_path = require_path(INSTALL_RECEIPT_SCHEMA_PATH)
    require_text_tokens(
        schema_path,
        [
            SANITIZER_INSTALL_RECEIPT_FIELD,
            "native_execution_contract",
            "runtime_library_manifest_path",
            "runtime_library_manifest_digest",
            "runtime_library_artifacts",
            "missing_runtime_behavior",
        ],
        f"{variant_id} install receipt schema",
    )
    if install_receipt.get("contract_id") != INSTALL_RECEIPT_CONTRACT_ID:
        raise RuntimeError(f"{variant_id} install receipt contract id drifted")
    if install_receipt.get("receipt_field") != SANITIZER_INSTALL_RECEIPT_FIELD:
        raise RuntimeError(f"{variant_id} install receipt sanitizer field drifted")
    if install_receipt.get("machine_owned") is not True:
        raise RuntimeError(f"{variant_id} install receipt must be machine-owned")
    if install_receipt.get("support_truth") is not False:
        raise RuntimeError(f"{variant_id} install receipt was treated as support truth")
    if install_receipt.get("native_execution_claimed") is not False:
        raise RuntimeError(f"{variant_id} install receipt claimed native execution")
    if install_receipt.get("selected_runtime_variant") != f"sanitizer={sanitizer_name}":
        raise RuntimeError(f"{variant_id} install receipt runtime variant drifted")

    expected_runtime_manifest_path = expected_runtime_library_manifest_path(sanitizer_name)
    if install_receipt.get("runtime_library_manifest_path") != expected_runtime_manifest_path:
        raise RuntimeError(f"{variant_id} install receipt runtime manifest path drifted")
    if install_receipt.get("missing_runtime_behavior") != "fail-closed-before-package-install":
        raise RuntimeError(f"{variant_id} install receipt missing-runtime behavior drifted")

    required_fields = set(
        require_string_list(
            install_receipt.get("required_fields"),
            f"{variant_id}.install_receipt_contract.required_fields",
        )
    )
    expected_fields = expected_install_receipt_fields(sanitizer_name)
    if not expected_fields <= required_fields:
        missing_fields = sorted(expected_fields - required_fields)
        raise RuntimeError(f"{variant_id} install receipt fields missing: {missing_fields}")

    native_execution = require_object(
        install_receipt.get("native_execution_contract"),
        f"{variant_id}.install_receipt_contract.native_execution_contract",
    )
    if native_execution.get("native_execution_required_before_support") is not True:
        raise RuntimeError(f"{variant_id} native execution was not required before support")
    if native_execution.get("native_execution_record_required") is not True:
        raise RuntimeError(f"{variant_id} native execution record was not required")
    if native_execution.get("native_execution_claimed") is not False:
        raise RuntimeError(f"{variant_id} native execution contract claimed execution")
    if (
        native_execution.get("missing_native_execution_behavior")
        != "fail-closed-before-support-promotion"
    ):
        raise RuntimeError(f"{variant_id} missing native execution behavior drifted")
    native_execution_fields = set(
        require_string_list(
            native_execution.get("native_execution_record_fields"),
            f"{variant_id}.native_execution_contract.native_execution_record_fields",
        )
    )
    if not REQUIRED_NATIVE_EXECUTION_RECORD_FIELDS <= native_execution_fields:
        missing_fields = sorted(REQUIRED_NATIVE_EXECUTION_RECORD_FIELDS - native_execution_fields)
        raise RuntimeError(f"{variant_id} native execution record fields missing: {missing_fields}")
    if sanitizer_name == "undefined" and "trap_or_recover_mode" not in native_execution_fields:
        raise RuntimeError(f"{variant_id} UBSan native execution record missing trap_or_recover_mode")

    return {
        "schema": INSTALL_RECEIPT_SCHEMA_PATH,
        "receipt_field": SANITIZER_INSTALL_RECEIPT_FIELD,
        "required_fields": sorted(required_fields),
        "runtime_library_manifest_path": expected_runtime_manifest_path,
        "native_execution_required_before_support": True,
        "native_execution_claimed": False,
    }


def validate_package_runtime_model_contract(
    variant_id: str,
    package_runtime_contract: dict[str, Any],
    *,
    expected_runtime_library_ids: list[str],
    sanitizer_name: str,
) -> dict[str, object]:
    package_layout = require_object(
        package_runtime_contract.get("package_layout_contract"),
        f"{variant_id}.package_runtime_contract.package_layout_contract",
    )
    package_root_layout = require_string_list(
        package_layout.get("package_root_layout"),
        f"{variant_id}.package_layout_contract.package_root_layout",
    )
    if any(path.startswith("tmp/") for path in package_root_layout):
        raise RuntimeError(f"{variant_id} package layout cannot use generated roots")
    metadata_manifest_path = str(package_layout.get("metadata_manifest_path", ""))
    if metadata_manifest_path != expected_metadata_manifest_path(sanitizer_name):
        raise RuntimeError(f"{variant_id} metadata manifest path drifted")
    if metadata_manifest_path not in package_root_layout:
        raise RuntimeError(f"{variant_id} metadata manifest path is absent from package layout")

    runtime_library_manifest_path = str(package_layout.get("runtime_library_manifest_path", ""))
    expected_runtime_manifest_path = expected_runtime_library_manifest_path(sanitizer_name)
    if runtime_library_manifest_path != expected_runtime_manifest_path:
        raise RuntimeError(f"{variant_id} runtime library manifest path drifted")
    if runtime_library_manifest_path not in package_root_layout:
        raise RuntimeError(f"{variant_id} runtime library manifest path is absent from package layout")

    expected_runtime_artifacts = expected_runtime_artifact_paths(sanitizer_name)
    runtime_library_required_entries = require_string_list(
        package_layout.get("runtime_library_required_entries"),
        f"{variant_id}.package_layout_contract.runtime_library_required_entries",
    )
    require_exact_string_set(
        runtime_library_required_entries,
        expected_runtime_artifacts,
        f"{variant_id} runtime library required entries",
    )
    require_exact_string_set(
        [path for path in package_root_layout if path in expected_runtime_artifacts],
        expected_runtime_artifacts,
        f"{variant_id} package layout runtime artifact entries",
    )
    layout_runtime_library_ids = require_string_list(
        package_layout.get("runtime_library_ids"),
        f"{variant_id}.package_layout_contract.runtime_library_ids",
    )
    if layout_runtime_library_ids != expected_runtime_library_ids:
        raise RuntimeError(f"{variant_id} package layout runtime library ids drifted")
    for runtime_library_id in expected_runtime_library_ids:
        if not any(path_matches_runtime_library_id(path, runtime_library_id) for path in package_root_layout):
            raise RuntimeError(f"{variant_id} package layout missing {runtime_library_id}")
    install_receipt_required_fields = set(
        require_string_list(
            package_layout.get("install_receipt_required_fields"),
            f"{variant_id}.package_layout_contract.install_receipt_required_fields",
        )
    )
    expected_receipt_fields = expected_install_receipt_fields(sanitizer_name)
    if not expected_receipt_fields <= install_receipt_required_fields:
        missing_fields = sorted(expected_receipt_fields - install_receipt_required_fields)
        raise RuntimeError(f"{variant_id} package layout receipt fields missing: {missing_fields}")
    if package_layout.get("layout_support_truth") is not False:
        raise RuntimeError(f"{variant_id} package layout was treated as support truth")

    runtime_probe = require_object(
        package_runtime_contract.get("runtime_probe_contract"),
        f"{variant_id}.package_runtime_contract.runtime_probe_contract",
    )
    if runtime_probe.get("probe_required") is not True:
        raise RuntimeError(f"{variant_id} runtime probe contract is not required")
    if (
        require_string_list(runtime_probe.get("required_runtime_library_ids"), f"{variant_id}.runtime_probe_contract.required_runtime_library_ids")
        != expected_runtime_library_ids
    ):
        raise RuntimeError(f"{variant_id} runtime probe library ids drifted")
    probe_inputs = set(require_string_list(runtime_probe.get("probe_inputs"), f"{variant_id}.runtime_probe_contract.probe_inputs"))
    expected_probe_inputs = set(REQUIRED_RUNTIME_PROBE_INPUTS)
    if sanitizer_name == "undefined":
        expected_probe_inputs.add("trap_or_recover_mode")
    if not expected_probe_inputs <= probe_inputs:
        raise RuntimeError(f"{variant_id} runtime probe inputs drifted")
    if runtime_probe.get("missing_runtime_behavior") != "fail-closed-before-package-install":
        raise RuntimeError(f"{variant_id} runtime probe missing-runtime behavior drifted")
    if runtime_probe.get("probe_result_support_truth") is not False:
        raise RuntimeError(f"{variant_id} runtime probe result was treated as support truth")

    install_selection = require_object(
        package_runtime_contract.get("install_selection_contract"),
        f"{variant_id}.package_runtime_contract.install_selection_contract",
    )
    if install_selection.get("selection_mode") != "explicit-opt-in":
        raise RuntimeError(f"{variant_id} install selection is not explicit opt-in")
    if install_selection.get("install_selector") != f"sanitizer={sanitizer_name}":
        raise RuntimeError(f"{variant_id} install selector drifted")
    if install_selection.get("default_release_selection_allowed") is not False:
        raise RuntimeError(f"{variant_id} default release runtime selection was allowed")
    if install_selection.get("default_release_misuse_behavior") != "fail-closed-before-install":
        raise RuntimeError(f"{variant_id} default release misuse behavior drifted")

    mixing = require_object(
        package_runtime_contract.get("runtime_mixing_rejection_contract"),
        f"{variant_id}.package_runtime_contract.runtime_mixing_rejection_contract",
    )
    if mixing.get("release_sanitizer_mixing_allowed") is not False:
        raise RuntimeError(f"{variant_id} release/sanitizer runtime mixing was allowed")
    if mixing.get("mixed_runtime_behavior") != "fail-closed-before-native-execution-claim":
        raise RuntimeError(f"{variant_id} runtime mixing behavior drifted")
    if require_string_list(mixing.get("rejected_release_runtime_package_ids"), f"{variant_id}.runtime_mixing_rejection_contract.rejected_release_runtime_package_ids") != RELEASE_RUNTIME_PACKAGE_IDS:
        raise RuntimeError(f"{variant_id} rejected release runtime package ids drifted")

    metadata_freshness = require_object(
        package_runtime_contract.get("metadata_freshness_contract"),
        f"{variant_id}.package_runtime_contract.metadata_freshness_contract",
    )
    if metadata_freshness.get("source_owned_metadata_required") is not True:
        raise RuntimeError(f"{variant_id} package metadata did not require source-owned metadata")
    if metadata_freshness.get("generated_metadata_support_truth") is not False:
        raise RuntimeError(f"{variant_id} generated metadata was treated as support truth")
    if metadata_freshness.get("stale_package_metadata_behavior") != "fail-closed-before-publication":
        raise RuntimeError(f"{variant_id} stale package metadata behavior drifted")
    freshness_inputs = set(require_string_list(metadata_freshness.get("freshness_inputs"), f"{variant_id}.metadata_freshness_contract.freshness_inputs"))
    expected_freshness_inputs = set(REQUIRED_METADATA_FRESHNESS_INPUTS)
    if sanitizer_name == "address":
        expected_freshness_inputs.add("environment")
    if sanitizer_name == "undefined":
        expected_freshness_inputs.add("trap_or_recover_mode")
    if not expected_freshness_inputs <= freshness_inputs:
        raise RuntimeError(f"{variant_id} metadata freshness inputs drifted")

    environment = require_object(
        package_runtime_contract.get("environment_contract"),
        f"{variant_id}.package_runtime_contract.environment_contract",
    )
    expected_env_var = "ASAN_OPTIONS" if sanitizer_name == "address" else "UBSAN_OPTIONS"
    if environment.get("env_var") != expected_env_var:
        raise RuntimeError(f"{variant_id} sanitizer environment variable drifted")
    if environment.get("missing_environment_behavior") != "fail-closed-before-native-execution-claim":
        raise RuntimeError(f"{variant_id} sanitizer environment fail-closed behavior drifted")
    if environment.get("environment_support_truth") is not False:
        raise RuntimeError(f"{variant_id} sanitizer environment was treated as support truth")
    require_string_list(environment.get("required_options"), f"{variant_id}.environment_contract.required_options")

    if sanitizer_name == "undefined":
        trap_recover = require_object(
            package_runtime_contract.get("trap_recover_contract"),
            f"{variant_id}.package_runtime_contract.trap_recover_contract",
        )
        if trap_recover.get("required_mode_field") != "trap_or_recover_mode":
            raise RuntimeError(f"{variant_id} UBSan trap/recover field drifted")
        if set(require_string_list(trap_recover.get("allowed_modes"), f"{variant_id}.trap_recover_contract.allowed_modes")) != {"trap", "recover"}:
            raise RuntimeError(f"{variant_id} UBSan trap/recover modes drifted")
        if trap_recover.get("default_mode_allowed") is not False:
            raise RuntimeError(f"{variant_id} allowed default UBSan mode")
        if trap_recover.get("missing_mode_behavior") != "fail-closed-before-native-execution-claim":
            raise RuntimeError(f"{variant_id} UBSan missing mode behavior drifted")
        if trap_recover.get("mode_support_truth") is not False:
            raise RuntimeError(f"{variant_id} UBSan mode was treated as support truth")

    install_receipt_summary = validate_install_receipt_contract(
        variant_id,
        package_runtime_contract,
        sanitizer_name=sanitizer_name,
    )

    return {
        "package_layout": package_root_layout,
        "metadata_manifest_path": metadata_manifest_path,
        "runtime_library_manifest_path": runtime_library_manifest_path,
        "runtime_library_required_entries": runtime_library_required_entries,
        "install_receipt": install_receipt_summary,
        "install_selector": str(install_selection["install_selector"]),
        "environment_variable": str(environment["env_var"]),
    }


def validate_package_install_model_fixture(contract: dict[str, Any]) -> dict[str, object]:
    raw_fixture_path = str(contract.get("package_install_model_fixture", ""))
    if raw_fixture_path != repo_rel(PACKAGE_INSTALL_MODEL_PATH):
        raise RuntimeError("package_install_model_fixture drifted from sanitizer package/install model fixture")
    fixture_path = require_path(raw_fixture_path)
    fixture = load_json(fixture_path)
    if fixture.get("contract_id") != PACKAGE_INSTALL_MODEL_CONTRACT_ID:
        raise RuntimeError("unexpected sanitizer package/install model contract_id")
    if fixture.get("native_sanitizer_execution_claimed") is not False:
        raise RuntimeError("package/install fixture must not claim native sanitizer execution")
    if fixture.get("support_promotion_allowed") is not False:
        raise RuntimeError("package/install fixture must not allow sanitizer support promotion")

    positive_fixtures = fixture.get("positive_contract_fixtures")
    if not isinstance(positive_fixtures, list) or not positive_fixtures:
        raise RuntimeError("package/install fixture missing positive contract fixtures")
    positives_by_variant = {
        str(row.get("variant_id", "")): row
        for row in positive_fixtures
        if isinstance(row, dict)
    }
    missing_positive = sorted(set(REQUIRED_PACKAGE_VARIANTS) - set(positives_by_variant))
    if missing_positive:
        raise RuntimeError(f"package/install fixture missing positive variants: {missing_positive}")

    checked_positive_ids: list[str] = []
    for variant_id, expected in REQUIRED_PACKAGE_VARIANTS.items():
        row = positives_by_variant[variant_id]
        sanitizer_name = str(expected["sanitizer"])
        if row.get("issue_ref") != expected["issue_ref"]:
            raise RuntimeError(f"{variant_id} package/install positive issue_ref drifted")
        if row.get("sanitizer") != sanitizer_name:
            raise RuntimeError(f"{variant_id} package/install positive sanitizer drifted")
        if row.get("package_variant_row_id") != expected["package_variant_row_id"]:
            raise RuntimeError(f"{variant_id} package/install positive package row drifted")
        if row.get("package_id") != expected["package_id"]:
            raise RuntimeError(f"{variant_id} package/install positive package id drifted")
        if row.get("claim_state") != "reserved" or row.get("platform_ids") != []:
            raise RuntimeError(f"{variant_id} package/install positive must remain reserved")
        if row.get("support_truth") is not False or row.get("native_execution_claimed") is not False:
            raise RuntimeError(f"{variant_id} package/install positive overclaimed support or execution")
        validate_package_runtime_model_contract(
            variant_id,
            row,
            expected_runtime_library_ids=list(expected["runtime_library_ids"]),  # type: ignore[arg-type]
            sanitizer_name=sanitizer_name,
        )
        checked_positive_ids.append(str(row.get("case_id", variant_id)))

    negative_fixtures = fixture.get("negative_contract_fixtures")
    if not isinstance(negative_fixtures, list) or not negative_fixtures:
        raise RuntimeError("package/install fixture missing negative contract fixtures")
    negative_pairs: set[tuple[str, str]] = set()
    for row in negative_fixtures:
        if not isinstance(row, dict):
            raise RuntimeError("package/install negative fixture entries must be objects")
        variant_id = str(row.get("variant_id", ""))
        failure_kind = str(row.get("failure_kind", ""))
        negative_pairs.add((variant_id, failure_kind))
        if variant_id not in REQUIRED_PACKAGE_VARIANTS:
            raise RuntimeError(f"package/install negative fixture used unknown variant {variant_id}")
        if failure_kind not in REQUIRED_PACKAGE_INSTALL_NEGATIVE_KINDS:
            raise RuntimeError(f"{variant_id} package/install negative fixture used unknown failure_kind {failure_kind}")
        if not str(row.get("failure_class", "")):
            raise RuntimeError(f"{variant_id} package/install negative fixture missing failure_class")
        if not str(row.get("required_behavior", "")).startswith("fail-closed"):
            raise RuntimeError(f"{variant_id} package/install negative fixture does not fail closed")
        if row.get("support_truth") is not False or row.get("native_execution_claimed") is not False:
            raise RuntimeError(f"{variant_id} package/install negative fixture overclaimed support or execution")
        blocks = {str(block) for block in row.get("blocks", [])}
        if not REQUIRED_UNSUPPORTED_DIAGNOSTIC_BLOCKS <= blocks:
            raise RuntimeError(f"{variant_id} package/install negative fixture did not block all promotion surfaces")
    expected_negative_pairs = {
        (variant_id, failure_kind)
        for variant_id in REQUIRED_PACKAGE_VARIANTS
        for failure_kind in REQUIRED_PACKAGE_INSTALL_NEGATIVE_KINDS
    }
    missing_negative = sorted(expected_negative_pairs - negative_pairs)
    if missing_negative:
        raise RuntimeError(f"package/install fixture missing negative cases: {missing_negative}")

    return {
        "fixture_path": repo_rel(fixture_path),
        "positive_case_ids": checked_positive_ids,
        "negative_case_count": len(negative_fixtures),
        "support_promotion_allowed": False,
        "native_sanitizer_execution_claimed": False,
    }


def validate_sanitizer_config(contract: dict[str, Any]) -> dict[str, object]:
    config = contract.get("sanitizer_config")
    if not isinstance(config, dict):
        raise RuntimeError("sanitizer_config must be an object")
    config_path = require_path(str(config.get("path", "")))
    checked_tokens: list[str] = []
    for field in ("required_options", "required_compile_flags", "required_link_flags", "required_sanitizer_tokens"):
        tokens = [str(token) for token in config.get(field, [])]
        if not tokens:
            raise RuntimeError(f"sanitizer_config.{field} must be non-empty")
        checked_tokens.extend(require_text_tokens(config_path, tokens, field))
    return {
        "config_path": repo_rel(config_path),
        "checked_token_count": len(checked_tokens),
    }


def validate_target_applications(contract: dict[str, Any]) -> list[dict[str, str]]:
    applications = contract.get("target_applications")
    if not isinstance(applications, list) or not applications:
        raise RuntimeError("target_applications must be a non-empty list")

    checked: list[dict[str, str]] = []
    for entry in applications:
        if not isinstance(entry, dict):
            raise RuntimeError("target_applications entries must be objects")
        raw_path = str(entry.get("path", ""))
        target_token = str(entry.get("target_token", ""))
        surface = str(entry.get("surface", ""))
        owner = str(entry.get("owner", ""))
        if not raw_path or not target_token or not surface or not owner:
            raise RuntimeError("target application entries require path, target_token, surface, and owner")
        path = require_path(raw_path)
        expected_call = f"objc3c_apply_sanitizers({target_token})"
        require_text_tokens(path, [expected_call], f"target application {surface}")
        checked.append(
            {
                "surface": surface,
                "owner": owner,
                "path": repo_rel(path),
                "target_token": target_token,
            }
        )
    return checked


def validate_coverage_matrix(contract: dict[str, Any]) -> list[dict[str, str]]:
    matrix = contract.get("coverage_matrix")
    if not isinstance(matrix, list) or not matrix:
        raise RuntimeError("coverage_matrix must be a non-empty list")

    seen = {
        (str(entry.get("sanitizer")), str(entry.get("surface")))
        for entry in matrix
        if isinstance(entry, dict)
    }
    expected_pairs = {
        (sanitizer, surface)
        for sanitizer in REQUIRED_SANITIZERS
        for surface in REQUIRED_COVERAGE_SURFACES
    }
    missing_pairs = sorted(expected_pairs - seen)
    if missing_pairs:
        raise RuntimeError(f"coverage_matrix missing pairs: {missing_pairs}")

    checked: list[dict[str, str]] = []
    for entry in matrix:
        if not isinstance(entry, dict):
            raise RuntimeError("coverage_matrix entries must be objects")
        if entry.get("coverage") != "config-backed":
            raise RuntimeError("coverage_matrix entries must be config-backed")
        checked.append(
            {
                "sanitizer": str(entry["sanitizer"]),
                "surface": str(entry["surface"]),
                "owner": str(entry["owner"]),
            }
        )
    return checked


def validate_runtime_package_variants(contract: dict[str, Any]) -> list[dict[str, object]]:
    variants = contract.get("runtime_package_variants")
    if not isinstance(variants, list) or not variants:
        raise RuntimeError("runtime_package_variants must be a non-empty list")

    by_id: dict[str, dict[str, Any]] = {}
    for variant in variants:
        if not isinstance(variant, dict):
            raise RuntimeError("runtime_package_variants entries must be objects")
        variant_id = str(variant.get("variant_id", ""))
        if not variant_id:
            raise RuntimeError("runtime package variant missing variant_id")
        if variant_id in by_id:
            raise RuntimeError(f"duplicate runtime package variant {variant_id}")
        by_id[variant_id] = variant

    missing = sorted(set(REQUIRED_PACKAGE_VARIANTS) - set(by_id))
    if missing:
        raise RuntimeError(f"runtime_package_variants missing reserved variants: {missing}")

    checked: list[dict[str, object]] = []
    for variant_id, expected in REQUIRED_PACKAGE_VARIANTS.items():
        variant = by_id[variant_id]
        issue_ref = int(expected["issue_ref"])  # type: ignore[arg-type]
        sanitizer_name = str(expected["sanitizer"])
        package_variant_row_id = str(expected["package_variant_row_id"])
        package_channel_id = str(expected["package_channel_id"])
        package_id = str(expected["package_id"])
        if variant.get("issue_ref") != issue_ref:
            raise RuntimeError(f"{variant_id} issue_ref drifted")
        if variant.get("sanitizer") != sanitizer_name:
            raise RuntimeError(f"{variant_id} sanitizer identity drifted")
        if variant.get("package_variant_row_id") != package_variant_row_id:
            raise RuntimeError(f"{variant_id} package variant row identity drifted")
        if variant.get("package_channel_id") != package_channel_id:
            raise RuntimeError(f"{variant_id} package channel id drifted")
        if variant.get("package_id") != package_id:
            raise RuntimeError(f"{variant_id} package id drifted")
        if variant.get("claim_state") != "reserved":
            raise RuntimeError(f"{variant_id} must remain reserved until package execution evidence exists")
        if variant.get("native_package_execution_claimed") is not False:
            raise RuntimeError(f"{variant_id} must not claim native package execution")
        if variant.get("unsupported_behavior") != "fail-closed":
            raise RuntimeError(f"{variant_id} package variant does not fail closed")

        build_contract = require_object(variant.get("build_contract"), f"{variant_id}.build_contract")
        compiler_flags = set(require_string_list(build_contract.get("compiler_flags"), f"{variant_id}.build_contract.compiler_flags"))
        linker_flags = set(require_string_list(build_contract.get("linker_flags"), f"{variant_id}.build_contract.linker_flags"))
        require_string_list(build_contract.get("environment_requirements"), f"{variant_id}.build_contract.environment_requirements")
        if not set(expected["compiler_flags"]) <= compiler_flags:  # type: ignore[arg-type]
            raise RuntimeError(f"{variant_id} compiler flags drifted")
        if not set(expected["linker_flags"]) <= linker_flags:  # type: ignore[arg-type]
            raise RuntimeError(f"{variant_id} linker flags drifted")
        if sanitizer_name == "undefined" and build_contract.get("mode") != "explicit-trap-or-recover":
            raise RuntimeError(f"{variant_id} UBSan trap-or-recover mode policy drifted")

        runtime_library_contract = require_object(
            variant.get("runtime_library_contract"),
            f"{variant_id}.runtime_library_contract",
        )
        runtime_library_ids = require_string_list(
            runtime_library_contract.get("runtime_library_ids"),
            f"{variant_id}.runtime_library_contract.runtime_library_ids",
        )
        if runtime_library_ids != expected["runtime_library_ids"]:
            raise RuntimeError(f"{variant_id} runtime library ids drifted")
        if runtime_library_contract.get("missing_runtime_behavior") != "fail-closed-before-package-install":
            raise RuntimeError(f"{variant_id} missing runtime behavior drifted")
        if runtime_library_contract.get("mixed_runtime_behavior") != "fail-closed":
            raise RuntimeError(f"{variant_id} mixed runtime behavior drifted")

        install_guard = require_object(variant.get("install_guard"), f"{variant_id}.install_guard")
        if "default release runtime" not in str(install_guard.get("release_channel_policy", "")).lower():
            raise RuntimeError(f"{variant_id} release-channel install guard does not name default release runtime isolation")
        if install_guard.get("unsupported_platform_behavior") != "fail-closed":
            raise RuntimeError(f"{variant_id} unsupported platform behavior drifted")
        if install_guard.get("missing_runtime_behavior") != "fail-closed-before-package-install":
            raise RuntimeError(f"{variant_id} missing runtime install guard drifted")
        if install_guard.get("mixed_runtime_behavior") != "fail-closed":
            raise RuntimeError(f"{variant_id} mixed runtime install guard drifted")
        if install_guard.get("stale_package_metadata_behavior") != "fail-closed-before-publication":
            raise RuntimeError(f"{variant_id} stale metadata install guard drifted")

        package_runtime_contract = require_object(
            variant.get("package_runtime_contract"),
            f"{variant_id}.package_runtime_contract",
        )
        if package_runtime_contract.get("runtime_probe_required") is not True:
            raise RuntimeError(f"{variant_id} sanitizer runtime probe is not required")
        package_model_summary = validate_package_runtime_model_contract(
            variant_id,
            package_runtime_contract,
            expected_runtime_library_ids=runtime_library_ids,
            sanitizer_name=sanitizer_name,
        )
        if package_runtime_contract.get("default_release_channel_allowed") is not False:
            raise RuntimeError(f"{variant_id} sanitizer package leaked into the default release channel")
        if package_runtime_contract.get("report_artifact_support_truth") is not False:
            raise RuntimeError(f"{variant_id} sanitizer reports were treated as support truth")
        if package_runtime_contract.get("mixed_release_sanitizer_runtime_behavior") != "fail-closed":
            raise RuntimeError(f"{variant_id} mixed release/sanitizer runtime did not fail closed")
        release_runtime_package_ids = require_string_list(
            package_runtime_contract.get("release_runtime_package_ids"),
            f"{variant_id}.package_runtime_contract.release_runtime_package_ids",
        )
        if release_runtime_package_ids != RELEASE_RUNTIME_PACKAGE_IDS:
            raise RuntimeError(f"{variant_id} release runtime package isolation ids drifted")
        if package_id in release_runtime_package_ids:
            raise RuntimeError(f"{variant_id} sanitizer package id matched a release runtime package id")
        if package_runtime_contract.get("release_runtime_mixing_allowed") is not False:
            raise RuntimeError(f"{variant_id} allowed sanitizer/release runtime package mixing")

        detection_records = package_runtime_contract.get("expected_detection_records")
        if not isinstance(detection_records, list) or not detection_records:
            raise RuntimeError(f"{variant_id} missing expected sanitizer detection records")
        detection_record_ids: set[str] = set()
        for record in detection_records:
            record_object = require_object(record, f"{variant_id}.expected_detection_records[]")
            record_id = str(record_object.get("record_id", ""))
            if not record_id:
                raise RuntimeError(f"{variant_id} expected detection record missing record_id")
            detection_record_ids.add(record_id)
            if record_object.get("support_truth") is not False:
                raise RuntimeError(f"{variant_id} detection record was treated as support truth")
            if record_object.get("required_behavior") != "record-only-no-support-promotion":
                raise RuntimeError(f"{variant_id} detection record behavior drifted")
        if detection_record_ids != expected["expected_detection_record_ids"]:
            raise RuntimeError(f"{variant_id} expected detection record ids drifted")

        unsupported_host_diagnostics = package_runtime_contract.get("unsupported_host_diagnostics")
        if not isinstance(unsupported_host_diagnostics, list) or not unsupported_host_diagnostics:
            raise RuntimeError(f"{variant_id} missing unsupported-host diagnostics")
        unsupported_diagnostic_ids: list[str] = []
        for diagnostic in unsupported_host_diagnostics:
            diagnostic_object = require_object(diagnostic, f"{variant_id}.unsupported_host_diagnostics[]")
            diagnostic_id = str(diagnostic_object.get("diagnostic_id", ""))
            if not diagnostic_id:
                raise RuntimeError(f"{variant_id} unsupported-host diagnostic missing diagnostic_id")
            unsupported_diagnostic_ids.append(diagnostic_id)
            if diagnostic_object.get("failure_class") != "unsupported-sanitizer-platform":
                raise RuntimeError(f"{variant_id} unsupported-host diagnostic failure class drifted")
            if diagnostic_object.get("required_behavior") != "fail-closed-before-capability-promotion":
                raise RuntimeError(f"{variant_id} unsupported-host diagnostic behavior drifted")
            blocks = {str(item) for item in diagnostic_object.get("blocks", [])}
            if not REQUIRED_UNSUPPORTED_DIAGNOSTIC_BLOCKS <= blocks:
                raise RuntimeError(f"{variant_id} unsupported-host diagnostic did not block all promotion surfaces")

        required_metadata_fields = {
            str(field)
            for field in package_runtime_contract.get("required_metadata_fields", [])
        }
        if not set(expected["required_metadata_fields"]) <= required_metadata_fields:  # type: ignore[arg-type]
            raise RuntimeError(f"{variant_id} package runtime metadata requirements drifted")
        required_package_evidence = [str(item) for item in variant.get("required_package_evidence", [])]
        if set(required_package_evidence) != REQUIRED_PACKAGE_EVIDENCE:
            raise RuntimeError(f"{variant_id} package variant evidence requirements drifted")
        checked.append(
            {
                "variant_id": variant_id,
                "sanitizer": sanitizer_name,
                "issue_ref": issue_ref,
                "package_variant_row_id": package_variant_row_id,
                "package_id": package_id,
                "claim_state": "reserved",
                "native_package_execution_claimed": False,
                "runtime_probe_required": True,
                "runtime_library_ids": runtime_library_ids,
                "package_model": package_model_summary,
                "release_runtime_mixing_allowed": False,
                "expected_detection_record_ids": sorted(detection_record_ids),
                "unsupported_host_diagnostic_ids": unsupported_diagnostic_ids,
            }
        )
    return checked


def validate_fixture(contract: dict[str, Any]) -> str:
    fixture = require_path(str(contract.get("source_fixture", "")))
    marker = str(contract.get("fixture_required_marker", ""))
    if not marker:
        raise RuntimeError("fixture_required_marker must be non-empty")
    require_text_tokens(fixture, [marker], "source fixture")
    return repo_rel(fixture)


def validate_report_contract(contract: dict[str, Any]) -> dict[str, object]:
    report_contract = contract.get("report_contract")
    if not isinstance(report_contract, dict):
        raise RuntimeError("report_contract must be an object")
    required_root = str(report_contract.get("required_report_root", ""))
    if required_root != "tmp/reports/security-hardening":
        raise RuntimeError("report_contract required_report_root drifted")
    if report_contract.get("native_sanitizer_execution_claimed") is not False:
        raise RuntimeError("contract must not claim native sanitizer execution")
    summary_path = str(contract.get("summary_path", ""))
    if summary_path != repo_rel(SUMMARY_PATH):
        raise RuntimeError("summary_path drifted from sanitizer validation output")
    return {
        "required_report_root": required_root,
        "native_sanitizer_execution_claimed": False,
    }


def main() -> int:
    try:
        contract = load_json(CONTRACT_PATH)
        if contract.get("contract_id") != CONTRACT_ID:
            raise RuntimeError("unexpected sanitizer validation contract_id")
        action_name = str(contract.get("public_action", ""))
        if not action_name:
            raise RuntimeError("public_action must be non-empty")
        require_action_surfaces(action_name)
        config_summary = validate_sanitizer_config(contract)
        target_applications = validate_target_applications(contract)
        coverage_matrix = validate_coverage_matrix(contract)
        package_install_model = validate_package_install_model_fixture(contract)
        runtime_package_variants = validate_runtime_package_variants(contract)
        fixture_path = validate_fixture(contract)
        report_contract = validate_report_contract(contract)
    except RuntimeError as exc:
        return fail(str(exc))

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "source_contract": repo_rel(CONTRACT_PATH),
        "public_action": action_name,
        "sanitizer_config": config_summary,
        "target_applications": target_applications,
        "coverage_matrix": coverage_matrix,
        "package_install_model": package_install_model,
        "runtime_package_variants": runtime_package_variants,
        "source_fixture": fixture_path,
        "report_contract": report_contract,
        "owner_boundaries": contract["owner_boundaries"],
        "forbidden_claims": contract["forbidden_claims"],
    }
    write_report_json(SUMMARY_PATH, payload, sort_keys=False)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("security-sanitizer-validation: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
