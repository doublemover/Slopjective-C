from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any

from check_stdlib_surface_model import CanonicalModuleSurface


@dataclass(frozen=True)
class CompatibilityGateValidation:
    abi_gate: dict[str, Any]
    semantic_gate: dict[str, Any]
    package_gate: dict[str, Any]
    conformance_gate: dict[str, Any]


def _require_repo_file(root: Path, raw_path: object, label: str) -> str | None:
    if not isinstance(raw_path, str) or not raw_path:
        return f"compatibility gate {label} path malformed"
    if raw_path.startswith("tmp/"):
        return f"compatibility gate {label} cannot use tmp as source of truth"
    if not (root / raw_path).is_file():
        return f"compatibility gate {label} missing: {raw_path}"
    return None


def _core_module_surface(
    module_surfaces: list[CanonicalModuleSurface],
) -> CanonicalModuleSurface | None:
    for module_surface in module_surfaces:
        if module_surface.module == "objc3.core":
            return module_surface
    return None


def validate_compatibility_gates(
    *,
    root: Path,
    compatibility_gates: dict[str, Any],
    module_surfaces: list[CanonicalModuleSurface],
    semantic_policy: dict[str, Any],
) -> tuple[str | None, CompatibilityGateValidation | None]:
    if compatibility_gates.get("contract_id") != "objc3c.stdlib.compatibility_gates.v1":
        return "compatibility gate contract_id drifted", None
    if compatibility_gates.get("schema_version") != 1:
        return "compatibility gate schema_version drifted", None
    if compatibility_gates.get("workspace_contract") != "stdlib/workspace.json":
        return "compatibility gate workspace_contract drifted", None
    if compatibility_gates.get("module_inventory") != "stdlib/module_inventory.json":
        return "compatibility gate module_inventory drifted", None
    if compatibility_gates.get("package_surface") != "stdlib/package_surface.json":
        return "compatibility gate package_surface drifted", None
    if compatibility_gates.get("semantic_policy") != "stdlib/semantic_policy.json":
        return "compatibility gate semantic_policy drifted", None
    if compatibility_gates.get("stability_policy") != "stdlib/stability_policy.json":
        return "compatibility gate stability_policy drifted", None
    if compatibility_gates.get("stdlib_major_version") != 1:
        return "compatibility gate stdlib_major_version drifted", None

    core_module = _core_module_surface(module_surfaces)
    if core_module is None:
        return "compatibility gate missing objc3.core module surface", None

    abi_gate = compatibility_gates.get("abi_gate")
    if not isinstance(abi_gate, dict):
        return "compatibility gate abi_gate missing or malformed", None
    expected_abi_gate = {
        "mode": "manifest-signature-exact-match",
        "runtime_abi_source": f"{core_module.manifest}#/runtime_abi",
        "runtime_abi_signature_source": f"{core_module.manifest}#/runtime_abi_signatures",
        "public_export_signature_source": f"{core_module.manifest}#/abi_signatures",
        "runtime_contract_header": "native/objc3c/src/runtime/stdlib/core_runtime_contract.h",
        "runtime_implementation": "native/objc3c/src/runtime/stdlib/core_runtime.cpp",
        "drift_disposition": "fail-closed-before-package-claim",
    }
    if abi_gate != expected_abi_gate:
        return "compatibility gate abi_gate drifted", None
    for label in ("runtime_contract_header", "runtime_implementation"):
        path_error = _require_repo_file(root, abi_gate.get(label), label)
        if path_error is not None:
            return path_error, None

    semantic_gate = compatibility_gates.get("semantic_gate")
    if not isinstance(semantic_gate, dict):
        return "compatibility gate semantic_gate missing or malformed", None
    if semantic_gate.get("mode") != "stdlib-v1-explicit-semantic-policy":
        return "compatibility gate semantic mode drifted", None
    if semantic_gate.get("semantic_policy_source") != "stdlib/semantic_policy.json":
        return "compatibility gate semantic_policy_source drifted", None
    if semantic_gate.get("capability_ordinals_present") != [1, 2, 3, 4]:
        return "compatibility gate present capability ordinals drifted", None
    if semantic_gate.get("capability_ordinals_fail_closed") != [0, -1, 5, 99]:
        return "compatibility gate fail-closed capability ordinals drifted", None
    if semantic_gate.get("strict_system_ordinal") != 5:
        return "compatibility gate strict_system_ordinal drifted", None
    if semantic_gate.get("drift_disposition") != "fail-closed-before-public-support-claim":
        return "compatibility gate semantic drift disposition drifted", None
    required_semantics = semantic_gate.get("required_semantics")
    core_semantics = semantic_policy.get("core_semantics", {})
    if not isinstance(required_semantics, list) or not required_semantics:
        return "compatibility gate required_semantics malformed", None
    if not isinstance(core_semantics, dict):
        return "compatibility gate core semantic source malformed", None
    for semantic_key in required_semantics:
        if not isinstance(semantic_key, str) or semantic_key not in core_semantics:
            return f"compatibility gate required semantic missing: {semantic_key}", None

    package_gate = compatibility_gates.get("package_gate")
    if not isinstance(package_gate, dict):
        return "compatibility gate package_gate missing or malformed", None
    if package_gate.get("package_stage_root") != "tmp/pkg/objc3c-native-runnable-toolchain":
        return "compatibility gate package_stage_root drifted", None
    if package_gate.get("required_manifest_fields") != [
        "stdlib_compatibility_gates",
        "stdlib_compatibility_gate_summary",
    ]:
        return "compatibility gate required package manifest fields drifted", None
    if package_gate.get("required_public_actions") != [
        "check-stdlib-surface",
        "validate-stdlib-foundation",
        "validate-runnable-stdlib-foundation",
        "package-runnable-toolchain",
    ]:
        return "compatibility gate package public actions drifted", None
    if package_gate.get("drift_disposition") != "fail-closed-in-packaged-stdlib-validation":
        return "compatibility gate package drift disposition drifted", None

    conformance_gate = compatibility_gates.get("conformance_gate")
    if not isinstance(conformance_gate, dict):
        return "compatibility gate conformance_gate missing or malformed", None
    for label in (
        "runtime_probe",
        "positive_fixture",
        "negative_fixture",
        "positive_fixture_sidecar",
        "negative_fixture_sidecar",
    ):
        path_error = _require_repo_file(root, conformance_gate.get(label), label)
        if path_error is not None:
            return path_error, None
    if conformance_gate.get("required_commands") != [
        "npm run objc3c -- test-execution-smoke",
        "npm run objc3c -- test-execution-replay",
        "npm run objc3c -- validate-stdlib-foundation",
        "npm run objc3c -- validate-runnable-stdlib-foundation",
    ]:
        return "compatibility gate conformance commands drifted", None

    unsupported_surfaces = compatibility_gates.get("unsupported_surfaces")
    if not isinstance(unsupported_surfaces, list) or not all(
        isinstance(value, str) and value for value in unsupported_surfaces
    ):
        return "compatibility gate unsupported_surfaces malformed", None
    if "source-of-truth compatibility state under tmp" not in unsupported_surfaces:
        return "compatibility gate tmp source-of-truth rejection missing", None

    return (
        None,
        CompatibilityGateValidation(
            abi_gate=abi_gate,
            semantic_gate=semantic_gate,
            package_gate=package_gate,
            conformance_gate=conformance_gate,
        ),
    )
