from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from check_stdlib_surface_model import CanonicalModuleSurface
from objc3c_runnable_stdlib_foundation_e2e.manifest.validation import (
    validate_surface_payloads,
)
from stdlib_surface.compatibility import validate_compatibility_gates


def _core_surface() -> CanonicalModuleSurface:
    return CanonicalModuleSurface(
        module="objc3.core",
        implementation_module="objc3_core",
        capability_id="objc3.cap.core",
        required_profile="Core and above",
        workspace_root="stdlib/modules/objc3.core",
        source="stdlib/modules/objc3.core/module.objc3",
        smoke_source="stdlib/modules/objc3.core/smoke.objc3",
        manifest="stdlib/modules/objc3.core/module.json",
    )


def _semantic_policy() -> dict[str, Any]:
    return {
        "core_semantics": {
            "runtime_core_abi": "runtime",
            "capability_query_encoding": "capability",
            "option_unwrap_or": "option",
            "string_view_length": "string length",
            "string_view_prefix_units": "string prefix",
            "array_count": "array count",
            "array_prefix_count": "array prefix",
            "map_entry_value_or": "map value",
        }
    }


def _compatibility_gates() -> dict[str, Any]:
    return {
        "contract_id": "objc3c.stdlib.compatibility_gates.v1",
        "schema_version": 1,
        "workspace_contract": "stdlib/workspace.json",
        "module_inventory": "stdlib/module_inventory.json",
        "package_surface": "stdlib/package_surface.json",
        "semantic_policy": "stdlib/semantic_policy.json",
        "stability_policy": "stdlib/stability_policy.json",
        "stdlib_major_version": 1,
        "abi_gate": {
            "mode": "manifest-signature-exact-match",
            "runtime_abi_source": "stdlib/modules/objc3.core/module.json#/runtime_abi",
            "runtime_abi_signature_source": "stdlib/modules/objc3.core/module.json#/runtime_abi_signatures",
            "public_export_signature_source": "stdlib/modules/objc3.core/module.json#/abi_signatures",
            "runtime_contract_header": "native/objc3c/src/runtime/stdlib/core_runtime_contract.h",
            "runtime_implementation": "native/objc3c/src/runtime/stdlib/core_runtime.cpp",
            "drift_disposition": "fail-closed-before-package-claim",
        },
        "semantic_gate": {
            "mode": "stdlib-v1-explicit-semantic-policy",
            "semantic_policy_source": "stdlib/semantic_policy.json",
            "required_semantics": [
                "runtime_core_abi",
                "capability_query_encoding",
                "option_unwrap_or",
                "string_view_length",
                "string_view_prefix_units",
                "array_count",
                "array_prefix_count",
                "map_entry_value_or",
            ],
            "capability_ordinals_present": [1, 2, 3, 4],
            "capability_ordinals_fail_closed": [0, -1, 5, 99],
            "strict_system_ordinal": 5,
            "drift_disposition": "fail-closed-before-public-support-claim",
        },
        "package_gate": {
            "package_stage_root": "tmp/pkg/objc3c-native-runnable-toolchain",
            "required_manifest_fields": [
                "stdlib_compatibility_gates",
                "stdlib_compatibility_gate_summary",
            ],
            "required_public_actions": [
                "check-stdlib-surface",
                "validate-stdlib-foundation",
                "validate-runnable-stdlib-foundation",
                "package-runnable-toolchain",
            ],
            "drift_disposition": "fail-closed-in-packaged-stdlib-validation",
        },
        "conformance_gate": {
            "runtime_probe": "tests/tooling/runtime/stdlib_core_runtime_probe.cpp",
            "positive_fixture": "tests/tooling/fixtures/native/execution/positive/stdlib_core_runtime_helpers.objc3",
            "negative_fixture": "tests/tooling/fixtures/native/execution/negative/stdlib_core_runtime_helper_signature_conflict.objc3",
            "positive_fixture_sidecar": "tests/tooling/fixtures/native/execution/positive/stdlib_core_runtime_helpers.meta.json",
            "negative_fixture_sidecar": "tests/tooling/fixtures/native/execution/negative/stdlib_core_runtime_helper_signature_conflict.meta.json",
            "required_commands": [
                "npm run objc3c -- test-execution-smoke",
                "npm run objc3c -- test-execution-replay",
                "npm run objc3c -- validate-stdlib-foundation",
                "npm run objc3c -- validate-runnable-stdlib-foundation",
            ],
        },
        "unsupported_surfaces": [
            "stdlib major-version retired adapter routes",
            "alternate stdlib import lanes",
            "runtime helper signature aliases",
            "strict-system capability widening into core profile",
            "source-of-truth compatibility state under tmp",
        ],
    }


def test_stdlib_compatibility_gate_accepts_runtime_backed_v1_contract() -> None:
    error, validation = validate_compatibility_gates(
        root=ROOT,
        compatibility_gates=_compatibility_gates(),
        module_surfaces=[_core_surface()],
        semantic_policy=_semantic_policy(),
    )

    assert error is None
    assert validation is not None
    assert validation.semantic_gate["capability_ordinals_fail_closed"] == [0, -1, 5, 99]


def test_stdlib_compatibility_gate_rejects_strict_system_fail_open() -> None:
    gates = _compatibility_gates()
    gates["semantic_gate"] = {
        **gates["semantic_gate"],
        "capability_ordinals_fail_closed": [0, -1, 99],
    }

    error, validation = validate_compatibility_gates(
        root=ROOT,
        compatibility_gates=gates,
        module_surfaces=[_core_surface()],
        semantic_policy=_semantic_policy(),
    )

    assert error == "compatibility gate fail-closed capability ordinals drifted"
    assert validation is None


def test_packaged_stdlib_validation_requires_compatibility_summary() -> None:
    manifest = {
        "stdlib_program_command_surfaces": {},
        "stdlib_program_publish_inputs": [],
        "stdlib_program_examples": [],
        "advanced_helper_modules": [],
        "advanced_helper_command_surfaces": {},
        "advanced_helper_profile_gates": {},
        "stdlib_lowering_artifact_filenames": {},
        "stdlib_import_surface": {},
        "stdlib_compatibility_gate_summary": {
            "contract_id": "objc3c.stdlib.compatibility_gates.v1",
            "stdlib_major_version": 1,
            "abi_gate_mode": "manifest-signature-exact-match",
            "semantic_gate_mode": "stdlib-v1-explicit-semantic-policy",
            "package_gate_manifest_fields": [
                "stdlib_compatibility_gates",
                "stdlib_compatibility_gate_summary",
            ],
            "conformance_positive_fixture": "tests/tooling/fixtures/native/execution/positive/stdlib_core_runtime_helpers.objc3",
            "conformance_negative_fixture": "tests/tooling/fixtures/native/execution/negative/stdlib_core_runtime_helper_signature_conflict.objc3",
        },
    }
    validate_surface_payloads(
        manifest=manifest,
        stdlib_surface={"advanced_helper_package_surface": "stdlib/advanced_helper_package_surface.json"},
        package_surface_payload={
            "workspace_contract": "stdlib/workspace.json",
            "lowering_import_surface": "stdlib/lowering_import_surface.json",
            "advanced_helper_package_surface": "stdlib/advanced_helper_package_surface.json",
            "compatibility_gates": "stdlib/compatibility_gates.json",
        },
        compatibility_gates_payload=_compatibility_gates(),
        stdlib_program_surface_payload={
            "command_surfaces": {},
            "publish_inputs": [],
            "capability_demo_examples": [],
        },
        lowering_import_surface_payload={
            "artifact_filenames": {},
            "import_surface": {},
        },
        advanced_helper_package_surface_payload={
            "advanced_helper_modules": [],
            "advanced_helper_command_surfaces": {},
            "advanced_helper_profile_gates": {},
        },
    )


def test_packaged_stdlib_validation_rejects_compatibility_summary_drift() -> None:
    manifest = {
        "stdlib_program_command_surfaces": {},
        "stdlib_program_publish_inputs": [],
        "stdlib_program_examples": [],
        "advanced_helper_modules": [],
        "advanced_helper_command_surfaces": {},
        "advanced_helper_profile_gates": {},
        "stdlib_lowering_artifact_filenames": {},
        "stdlib_import_surface": {},
        "stdlib_compatibility_gate_summary": {
            "contract_id": "objc3c.stdlib.compatibility_gates.v1",
            "stdlib_major_version": 2,
            "abi_gate_mode": "manifest-signature-exact-match",
            "semantic_gate_mode": "stdlib-v1-explicit-semantic-policy",
            "package_gate_manifest_fields": [
                "stdlib_compatibility_gates",
                "stdlib_compatibility_gate_summary",
            ],
            "conformance_positive_fixture": "tests/tooling/fixtures/native/execution/positive/stdlib_core_runtime_helpers.objc3",
            "conformance_negative_fixture": "tests/tooling/fixtures/native/execution/negative/stdlib_core_runtime_helper_signature_conflict.objc3",
        },
    }

    with pytest.raises(RuntimeError, match="major version drifted"):
        validate_surface_payloads(
            manifest=manifest,
            stdlib_surface={"advanced_helper_package_surface": "stdlib/advanced_helper_package_surface.json"},
            package_surface_payload={
                "workspace_contract": "stdlib/workspace.json",
                "lowering_import_surface": "stdlib/lowering_import_surface.json",
                "advanced_helper_package_surface": "stdlib/advanced_helper_package_surface.json",
                "compatibility_gates": "stdlib/compatibility_gates.json",
            },
            compatibility_gates_payload=_compatibility_gates(),
            stdlib_program_surface_payload={
                "command_surfaces": {},
                "publish_inputs": [],
                "capability_demo_examples": [],
            },
            lowering_import_surface_payload={
                "artifact_filenames": {},
                "import_surface": {},
            },
            advanced_helper_package_surface_payload={
                "advanced_helper_modules": [],
                "advanced_helper_command_surfaces": {},
                "advanced_helper_profile_gates": {},
            },
        )
