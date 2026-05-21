"""Release ABI/API drift gate for manifest publication."""

from __future__ import annotations

import re
from pathlib import Path
from typing import Any

from objc3c_shared.json_io import load_json_object as load_json
from objc3c_shared.json_io import write_json_file
from objc3c_shared.schema_registry import validate_registered_schema
from objc3c_tooling.paths import repo_rel
from stdlib_surface.artifacts import extract_stdlib_abi_signatures

ROOT = Path(__file__).resolve().parents[2]
GOVERNANCE_SCHEMA_ID = "objc3c-abi-api-governance-v1"
GOVERNANCE_MANIFEST = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "release_foundation"
    / "abi_api_governance.json"
)
SUMMARY_PATH = (
    ROOT / "tmp" / "reports" / "release-foundation" / "abi-api-drift-summary.json"
)
SUMMARY_CONTRACT_ID = "objc3c.release.foundation.abi_api_drift.summary.v1"
REQUIRED_SOURCE_MANIFEST_KEYS = {
    "stdlib_module_inventory",
    "stdlib_stability_policy",
    "stdlib_semantic_policy",
    "stdlib_advanced_helper_package_surface",
    "frontend_c_api_helper_contract",
    "frontend_c_api_runner_contract",
    "release_operations_versioning_model",
    "long_horizon_deprecation_policy",
}
REQUIRED_SURFACE_CLASSES = {
    "stdlib-public-api",
    "stdlib-runtime-abi",
    "frontend-c-api-helper",
    "frontend-c-api-type-alias",
    "compiler-artifact-schema",
    "package-lockfile",
}
REQUIRED_RELEASE_BLOCKED_TRANSITIONS = {
    "public-symbol-removal-without-deprecation-window",
    "runtime-symbol-removal-without-deprecation-window",
    "signature-change-without-major-line",
    "private-helper-graduation-without-policy",
    "unsupported-upgrade-route",
    "unsupported-downgrade-route",
    "package-abi-identity-drift",
    "schema-breaking-change-without-major-line",
}
ABI_VERSION_RE = re.compile(r"^[0-9]+[.][0-9]+$")


def _repo_path(raw_path: str) -> Path:
    return ROOT / Path(raw_path)


def _display_path(path: Path) -> str:
    try:
        return repo_rel(path)
    except ValueError:
        return path.as_posix()


def _load_checked_json(raw_path: str, label: str, failures: list[str]) -> dict[str, Any]:
    path = _repo_path(raw_path)
    if not path.is_file():
        failures.append(f"{label} referenced missing file {raw_path}")
        return {}
    try:
        return load_json(path)
    except Exception as exc:
        failures.append(f"{label} failed to load {raw_path}: {exc}")
        return {}


def _string_list(payload: dict[str, Any], field_name: str, failures: list[str]) -> list[str]:
    values = payload.get(field_name)
    if not isinstance(values, list) or not all(isinstance(value, str) and value for value in values):
        failures.append(f"{field_name} must be a non-empty string list")
        return []
    return list(values)


def _string_map(payload: dict[str, Any], field_name: str, failures: list[str]) -> dict[str, str]:
    values = payload.get(field_name)
    if not isinstance(values, dict) or not values:
        failures.append(f"{field_name} must be a non-empty string map")
        return {}
    result: dict[str, str] = {}
    for key, value in values.items():
        if not isinstance(key, str) or not key or not isinstance(value, str) or not value:
            failures.append(f"{field_name} contains a malformed signature entry")
            continue
        result[key] = value
    return result


def _load_governance(path: Path, failures: list[str]) -> dict[str, Any]:
    if not path.is_file():
        failures.append(f"missing ABI/API governance manifest {_display_path(path)}")
        return {}
    try:
        payload = load_json(path)
        validate_registered_schema(payload, GOVERNANCE_SCHEMA_ID, label=_display_path(path))
        return payload
    except Exception as exc:
        failures.append(f"ABI/API governance manifest failed schema validation: {exc}")
        return {}


def _surface_entries_by_module(
    governance: dict[str, Any],
    field_name: str,
    failures: list[str],
) -> dict[str, dict[str, Any]]:
    entries = governance.get(field_name)
    if not isinstance(entries, list):
        failures.append(f"{field_name} must be a list")
        return {}
    by_module: dict[str, dict[str, Any]] = {}
    for entry in entries:
        if not isinstance(entry, dict) or not isinstance(entry.get("module"), str):
            failures.append(f"{field_name} contained a malformed entry")
            continue
        module = str(entry["module"])
        if module in by_module:
            failures.append(f"{field_name} contains duplicate module {module}")
            continue
        by_module[module] = entry
    return by_module


def _compatibility_class_ids(
    governance: dict[str, Any],
    failures: list[str],
) -> set[str]:
    taxonomy = governance.get("abi_version_taxonomy")
    if not isinstance(taxonomy, dict):
        failures.append("abi_version_taxonomy must be an object")
        return set()
    classes = taxonomy.get("compatibility_classes")
    if not isinstance(classes, list) or not classes:
        failures.append("abi_version_taxonomy.compatibility_classes must be a non-empty list")
        return set()
    class_ids: set[str] = set()
    for entry in classes:
        if not isinstance(entry, dict) or not isinstance(entry.get("class_id"), str):
            failures.append("abi_version_taxonomy contains a malformed compatibility class")
            continue
        class_id = str(entry["class_id"])
        if class_id in class_ids:
            failures.append(f"abi_version_taxonomy contains duplicate class {class_id}")
            continue
        if entry.get("release_blocker") is not True:
            failures.append(f"compatibility class {class_id} is not release-blocking")
        class_ids.add(class_id)
    return class_ids


def _transition_sets(
    governance: dict[str, Any],
    failures: list[str],
) -> dict[str, list[str]]:
    policy = governance.get("allowed_transition_policy")
    if not isinstance(policy, dict):
        failures.append("allowed_transition_policy must be an object")
        return {}
    raw_sets = policy.get("transition_sets")
    if not isinstance(raw_sets, dict) or not raw_sets:
        failures.append("allowed_transition_policy.transition_sets must be a non-empty object")
        return {}
    transition_sets: dict[str, list[str]] = {}
    for name, transitions in raw_sets.items():
        if not isinstance(name, str) or not name:
            failures.append("allowed transition set has malformed name")
            continue
        if (
            not isinstance(transitions, list)
            or not transitions
            or not all(isinstance(value, str) and value for value in transitions)
        ):
            failures.append(f"allowed transition set {name} must be a non-empty string list")
            continue
        if len(set(transitions)) != len(transitions):
            failures.append(f"allowed transition set {name} contains duplicate transitions")
        transition_sets[name] = list(transitions)

    blocked = policy.get("release_blocked_transitions")
    if (
        not isinstance(blocked, list)
        or not blocked
        or not all(isinstance(value, str) and value for value in blocked)
    ):
        failures.append("allowed_transition_policy.release_blocked_transitions is malformed")
    else:
        missing = sorted(REQUIRED_RELEASE_BLOCKED_TRANSITIONS - set(blocked))
        if missing:
            failures.append(
                "allowed transition policy lost release blockers: " + ", ".join(missing)
            )
    return transition_sets


def _deprecation_state_map(
    deprecation_policy: dict[str, Any],
    failures: list[str],
) -> dict[str, dict[str, Any]]:
    states = deprecation_policy.get("deprecation_states")
    if not isinstance(states, list):
        failures.append("deprecation policy has no deprecation_states")
        return {}
    result: dict[str, dict[str, Any]] = {}
    for entry in states:
        if not isinstance(entry, dict) or not isinstance(entry.get("state"), str):
            failures.append("deprecation policy contains malformed state entry")
            continue
        state = str(entry["state"])
        if state in result:
            failures.append(f"deprecation policy contains duplicate state {state}")
            continue
        result[state] = entry
    return result


def _nested_value(payload: dict[str, Any], path: list[str]) -> Any:
    current: Any = payload
    for segment in path:
        if not isinstance(current, dict):
            return None
        current = current.get(segment)
    return current


def _load_source_manifests(
    governance: dict[str, Any],
    failures: list[str],
) -> dict[str, dict[str, Any]]:
    source_manifests = governance.get("source_manifests")
    if not isinstance(source_manifests, dict):
        failures.append("source_manifests must be an object")
        return {}
    missing_keys = sorted(REQUIRED_SOURCE_MANIFEST_KEYS - set(source_manifests))
    extra_keys = sorted(set(source_manifests) - REQUIRED_SOURCE_MANIFEST_KEYS)
    if missing_keys:
        failures.append("source_manifests missing required keys: " + ", ".join(missing_keys))
    if extra_keys:
        failures.append("source_manifests contains ungoverned keys: " + ", ".join(extra_keys))

    loaded: dict[str, dict[str, Any]] = {}
    for key in sorted(REQUIRED_SOURCE_MANIFEST_KEYS & set(source_manifests)):
        raw_path = source_manifests.get(key)
        if not isinstance(raw_path, str) or not raw_path:
            failures.append(f"source_manifests.{key} must be a non-empty repo path")
            continue
        loaded[key] = _load_checked_json(raw_path, f"source_manifests.{key}", failures)
    return loaded


def _validate_abi_version_taxonomy(
    governance: dict[str, Any],
    loaded: dict[str, dict[str, Any]],
    class_ids: set[str],
    failures: list[str],
) -> None:
    taxonomy = governance.get("abi_version_taxonomy")
    if not isinstance(taxonomy, dict):
        failures.append("abi_version_taxonomy must be an object")
        return
    versioning = loaded.get("release_operations_versioning_model", {})
    current_major_line = taxonomy.get("current_major_line")
    current_minor_line = taxonomy.get("current_minor_line")
    if current_major_line != versioning.get("supported_major_line"):
        failures.append("ABI taxonomy current_major_line drifted from release versioning model")
    if current_minor_line != versioning.get("current_minor_line"):
        failures.append("ABI taxonomy current_minor_line drifted from release versioning model")
    supported_major_lines = taxonomy.get("supported_major_lines")
    if not isinstance(supported_major_lines, list) or current_major_line not in supported_major_lines:
        failures.append("ABI taxonomy does not include the current supported major line")

    surface_classes = taxonomy.get("surface_classes")
    if not isinstance(surface_classes, dict):
        failures.append("abi_version_taxonomy.surface_classes must be an object")
        return
    missing_surfaces = sorted(REQUIRED_SURFACE_CLASSES - set(surface_classes))
    if missing_surfaces:
        failures.append("ABI taxonomy missing surface classes: " + ", ".join(missing_surfaces))
    for surface_kind, class_id in surface_classes.items():
        if not isinstance(surface_kind, str) or not isinstance(class_id, str):
            failures.append("ABI taxonomy surface_classes contains malformed entry")
            continue
        if class_id not in class_ids:
            failures.append(f"ABI taxonomy surface {surface_kind} references unknown class {class_id}")


def _validate_surface_governance_metadata(
    surface: dict[str, Any],
    *,
    surface_kind: str,
    governance: dict[str, Any],
    class_ids: set[str],
    transition_sets: dict[str, list[str]],
    deprecation_states: dict[str, dict[str, Any]],
    failures: list[str],
) -> None:
    taxonomy = governance.get("abi_version_taxonomy", {})
    surface_classes = taxonomy.get("surface_classes") if isinstance(taxonomy, dict) else {}
    expected_class = (
        surface_classes.get(surface_kind) if isinstance(surface_classes, dict) else None
    )
    class_id = surface.get("compatibility_class")
    if class_id != expected_class:
        failures.append(
            f"{surface_kind} compatibility class drifted: expected {expected_class!r}, observed {class_id!r}"
        )
    if not isinstance(class_id, str) or class_id not in class_ids:
        failures.append(f"{surface_kind} references unknown compatibility class {class_id!r}")

    transition_set = surface.get("allowed_transition_set")
    if not isinstance(transition_set, str) or transition_set not in transition_sets:
        failures.append(f"{surface_kind} references unknown transition set {transition_set!r}")
    deprecation_state = surface.get("deprecation_state")
    if not isinstance(deprecation_state, str) or deprecation_state not in deprecation_states:
        failures.append(f"{surface_kind} references unknown deprecation state {deprecation_state!r}")

    if surface.get("release_blocker") is not True:
        failures.append(f"{surface_kind} must be release-blocking")
    if not isinstance(surface.get("surface_owner"), str) or not surface.get("surface_owner"):
        failures.append(f"{surface_kind} must name a symbol surface owner")
    for field_name in ("introduced_in", "minimum_supported_abi"):
        value = surface.get(field_name)
        if not isinstance(value, str) or ABI_VERSION_RE.match(value) is None:
            failures.append(f"{surface_kind} {field_name} must be a major.minor ABI version")
    maximum_supported_abi = surface.get("maximum_supported_abi")
    if maximum_supported_abi is not None and (
        not isinstance(maximum_supported_abi, str)
        or ABI_VERSION_RE.match(maximum_supported_abi) is None
    ):
        failures.append(f"{surface_kind} maximum_supported_abi must be null or a major.minor ABI version")


def _validate_deprecation_lifecycle(
    governance: dict[str, Any],
    loaded: dict[str, dict[str, Any]],
    deprecation_states: dict[str, dict[str, Any]],
    failures: list[str],
) -> None:
    lifecycle = governance.get("deprecation_lifecycle")
    source_manifests = governance.get("source_manifests", {})
    if not isinstance(lifecycle, dict):
        failures.append("deprecation_lifecycle must be an object")
        return
    expected_source = (
        source_manifests.get("long_horizon_deprecation_policy")
        if isinstance(source_manifests, dict)
        else None
    )
    if lifecycle.get("policy_source") != expected_source:
        failures.append("deprecation_lifecycle policy_source drifted from source_manifests")

    allowed_states = {
        value
        for value in lifecycle.get("public_claim_allowed_states", [])
        if isinstance(value, str)
    }
    blocking_states = {
        value
        for value in lifecycle.get("public_claim_blocking_states", [])
        if isinstance(value, str)
    }
    for state in sorted(allowed_states):
        entry = deprecation_states.get(state)
        if not isinstance(entry, dict):
            failures.append(f"deprecation lifecycle references unknown allowed state {state}")
        elif entry.get("public_claim_allowed") is not True:
            failures.append(f"deprecation lifecycle allowed state {state} blocks public claims")
    for state in sorted(blocking_states):
        entry = deprecation_states.get(state)
        if not isinstance(entry, dict):
            failures.append(f"deprecation lifecycle references unknown blocking state {state}")
        elif entry.get("public_claim_allowed") is not False:
            failures.append(f"deprecation lifecycle blocking state {state} still allows public claims")

    warning_classes = lifecycle.get("required_warning_classes")
    if not isinstance(warning_classes, dict):
        failures.append("deprecation_lifecycle.required_warning_classes must be an object")
    else:
        for state, expected_warning in warning_classes.items():
            entry = deprecation_states.get(str(state))
            if not isinstance(entry, dict):
                failures.append(f"deprecation lifecycle warning class references unknown state {state}")
                continue
            if entry.get("required_warning_class") != expected_warning:
                failures.append(f"deprecation lifecycle warning class drifted for {state}")

    required_fields = set(
        value
        for value in lifecycle.get("required_evidence_fields", [])
        if isinstance(value, str)
    )
    deprecation_policy = loaded.get("long_horizon_deprecation_policy", {})
    published_fields = set(
        value
        for value in deprecation_policy.get("required_publication_fields", [])
        if isinstance(value, str)
    )
    missing_fields = sorted(required_fields - published_fields)
    if missing_fields:
        failures.append(
            "deprecation lifecycle evidence fields missing from policy: "
            + ", ".join(missing_fields)
        )


def _validate_source_manifest_coverage(
    governance: dict[str, Any],
    loaded: dict[str, dict[str, Any]],
    failures: list[str],
) -> int:
    inventory = loaded.get("stdlib_module_inventory", {})
    modules = inventory.get("canonical_modules")
    if not isinstance(modules, list):
        failures.append("stdlib module inventory did not publish canonical_modules")
        return 0

    inventory_by_module: dict[str, dict[str, Any]] = {}
    for entry in modules:
        if not isinstance(entry, dict) or not isinstance(entry.get("module"), str):
            failures.append("stdlib module inventory contains a malformed module entry")
            continue
        module = str(entry["module"])
        if module in inventory_by_module:
            failures.append(f"stdlib module inventory contains duplicate module {module}")
            continue
        inventory_by_module[module] = entry

    public_surfaces = _surface_entries_by_module(governance, "stdlib_public_api", failures)
    runtime_surfaces = _surface_entries_by_module(governance, "stdlib_runtime_abi", failures)
    policies = governance.get("module_policies")
    policy_modules = set(policies) if isinstance(policies, dict) else set()
    inventory_modules = set(inventory_by_module)
    public_modules = set(public_surfaces)

    if inventory_modules != public_modules:
      missing = sorted(inventory_modules - public_modules)
      extra = sorted(public_modules - inventory_modules)
      if missing:
          failures.append("ABI/API governance missing stdlib_public_api modules: " + ", ".join(missing))
      if extra:
          failures.append("ABI/API governance has non-inventory stdlib_public_api modules: " + ", ".join(extra))
    if inventory_modules != policy_modules:
      missing = sorted(inventory_modules - policy_modules)
      extra = sorted(policy_modules - inventory_modules)
      if missing:
          failures.append("ABI/API governance missing module_policies: " + ", ".join(missing))
      if extra:
          failures.append("ABI/API governance has non-inventory module_policies: " + ", ".join(extra))

    stability = loaded.get("stdlib_stability_policy", {})
    semantic = loaded.get("stdlib_semantic_policy", {})
    advanced = loaded.get("stdlib_advanced_helper_package_surface", {})
    profile_gates = stability.get("profile_gates") if isinstance(stability, dict) else {}
    semver = semantic.get("module_semver") if isinstance(semantic, dict) else {}
    advanced_modules = advanced.get("advanced_helper_modules") if isinstance(advanced, dict) else []
    advanced_profile_gates = advanced.get("advanced_helper_profile_gates") if isinstance(advanced, dict) else {}

    for module, inventory_entry in sorted(inventory_by_module.items()):
        public_entry = public_surfaces.get(module)
        if isinstance(public_entry, dict):
            for field_name in ("manifest", "source"):
                if public_entry.get(field_name) != inventory_entry.get(field_name):
                    failures.append(
                        f"ABI/API governance {field_name} drifted from module inventory for {module}"
                    )
        policy = policies.get(module) if isinstance(policies, dict) else None
        if isinstance(policy, dict):
            expected_profile = inventory_entry.get("required_profile")
            if policy.get("required_profile") != expected_profile:
                failures.append(f"module policy required_profile drifted for {module}")
            if isinstance(profile_gates, dict) and profile_gates.get(module) != expected_profile:
                failures.append(f"stdlib stability profile gate drifted for {module}")
            if not isinstance(semver, dict) or module not in semver:
                failures.append(f"stdlib semantic policy missing module_semver for {module}")

    advanced_inventory_modules = {
        entry.get("canonical_module")
        for entry in advanced_modules
        if isinstance(entry, dict) and isinstance(entry.get("canonical_module"), str)
    }
    unknown_advanced_modules = sorted(advanced_inventory_modules - inventory_modules)
    if unknown_advanced_modules:
        failures.append(
            "advanced helper package surface references non-inventory modules: "
            + ", ".join(unknown_advanced_modules)
        )
    if isinstance(advanced_profile_gates, dict):
        for module in sorted(advanced_inventory_modules):
            inventory_profile = inventory_by_module.get(module, {}).get("required_profile")
            if advanced_profile_gates.get(module) != inventory_profile:
                failures.append(f"advanced helper profile gate drifted for {module}")

    for module in sorted(set(runtime_surfaces) - inventory_modules):
        failures.append(f"ABI/API governance has non-inventory stdlib_runtime_abi module {module}")

    return len(inventory_by_module)


def _approved_graduations(governance: dict[str, Any]) -> set[tuple[str, str]]:
    approved: set[tuple[str, str]] = set()
    records = governance.get("private_helper_graduation_records", [])
    if not isinstance(records, list):
        return approved
    for record in records:
        if not isinstance(record, dict):
            continue
        if record.get("policy_state") != "approved":
            continue
        module = record.get("module")
        symbol = record.get("symbol")
        if isinstance(module, str) and isinstance(symbol, str):
            approved.add((module, symbol))
    return approved


def _module_policy_failures(
    *,
    module_name: str,
    symbols: list[str],
    governance: dict[str, Any],
    policy_kind: str,
) -> list[str]:
    policies = governance.get("module_policies")
    if not isinstance(policies, dict):
        return ["module_policies must be an object"]
    policy = policies.get(module_name)
    if not isinstance(policy, dict):
        return [f"missing module policy for {module_name}"]

    prefix_field = (
        "allowed_runtime_prefixes" if policy_kind == "runtime-abi" else "allowed_public_prefixes"
    )
    prefixes = tuple(
        value
        for value in policy.get(prefix_field, [])
        if isinstance(value, str) and value
    )
    allowed_symbols = {
        value
        for value in policy.get("allowed_public_symbols", [])
        if isinstance(value, str) and value
    }
    approved_graduations = _approved_graduations(governance)
    failures: list[str] = []
    for symbol in symbols:
        if symbol in allowed_symbols or any(symbol.startswith(prefix) for prefix in prefixes):
            continue
        if (module_name, symbol) in approved_graduations:
            continue
        failures.append(
            f"private helper graduation lacks policy for {module_name}.{symbol}"
        )
    return failures


def _validate_stdlib_symbol_surface(
    surface: dict[str, Any],
    *,
    governance: dict[str, Any],
    surface_kind: str,
    class_ids: set[str],
    transition_sets: dict[str, list[str]],
    deprecation_states: dict[str, dict[str, Any]],
    failures: list[str],
) -> int:
    surface_contract_kind = (
        "stdlib-runtime-abi" if surface_kind == "runtime-abi" else "stdlib-public-api"
    )
    _validate_surface_governance_metadata(
        surface,
        surface_kind=surface_contract_kind,
        governance=governance,
        class_ids=class_ids,
        transition_sets=transition_sets,
        deprecation_states=deprecation_states,
        failures=failures,
    )
    module_name = surface.get("module")
    manifest_path = surface.get("manifest")
    source_path = surface.get("source")
    symbol_field = surface.get("symbol_set_field")
    signature_field = surface.get("signature_field")
    expected_symbols = surface.get("symbols")
    if not all(
        isinstance(value, str) and value
        for value in (module_name, manifest_path, source_path, symbol_field, signature_field)
    ) or not isinstance(expected_symbols, dict):
        failures.append(f"{surface_kind} surface entry is malformed")
        return 0

    manifest = _load_checked_json(manifest_path, f"{surface_kind} manifest", failures)
    source_file = _repo_path(source_path)
    if not source_file.is_file():
        failures.append(f"{surface_kind} source referenced missing file {source_path}")
        return 0

    if manifest.get("canonical_module") != module_name:
        failures.append(f"{surface_kind} manifest module drifted for {module_name}")
    if manifest.get("source") != source_path:
        failures.append(f"{surface_kind} manifest source drifted for {module_name}")

    actual_symbols = _string_list(manifest, symbol_field, failures)
    manifest_signatures = _string_map(manifest, signature_field, failures)
    source_signatures = extract_stdlib_abi_signatures(
        source_file.read_text(encoding="utf-8")
    )

    expected_signature_map = {
        str(symbol): str(signature)
        for symbol, signature in expected_symbols.items()
        if isinstance(symbol, str) and isinstance(signature, str)
    }
    expected_names = set(expected_signature_map)
    actual_names = set(actual_symbols)
    added = sorted(actual_names - expected_names)
    removed = sorted(expected_names - actual_names)
    if added:
        failures.append(
            f"unreviewed {surface_kind} symbol addition in {module_name}: "
            + ", ".join(added)
        )
    if removed:
        failures.append(
            f"unreviewed {surface_kind} symbol removal in {module_name}: "
            + ", ".join(removed)
        )
    if set(manifest_signatures) != actual_names:
        failures.append(f"{surface_kind} signature manifest keys drifted for {module_name}")

    for symbol in sorted(expected_names & actual_names):
        expected_signature = expected_signature_map[symbol]
        manifest_signature = manifest_signatures.get(symbol)
        source_signature = source_signatures.get(symbol)
        if manifest_signature != expected_signature:
            failures.append(
                f"{surface_kind} signature drifted for {module_name}.{symbol}: "
                f"expected {expected_signature!r}, observed {manifest_signature!r}"
            )
        if source_signature != manifest_signature:
            failures.append(
                f"{surface_kind} source signature drifted for {module_name}.{symbol}: "
                f"manifest {manifest_signature!r}, source {source_signature!r}"
            )

    failures.extend(
        _module_policy_failures(
            module_name=module_name,
            symbols=actual_symbols,
            governance=governance,
            policy_kind=surface_kind,
        )
    )
    return len(expected_signature_map)


def _validate_frontend_c_api(
    governance: dict[str, Any],
    *,
    class_ids: set[str],
    transition_sets: dict[str, list[str]],
    deprecation_states: dict[str, dict[str, Any]],
    failures: list[str],
) -> int:
    surface = governance.get("frontend_c_api")
    if not isinstance(surface, dict):
        failures.append("frontend_c_api must be an object")
        return 0
    for surface_kind in ("frontend-c-api-helper", "frontend-c-api-type-alias"):
        _validate_surface_governance_metadata(
            surface,
            surface_kind=surface_kind,
            governance=governance,
            class_ids=class_ids,
            transition_sets=transition_sets,
            deprecation_states=deprecation_states,
            failures=failures,
        )
    helper_contract_path = surface.get("helper_contract")
    runner_contract_path = surface.get("runner_contract")
    if not isinstance(helper_contract_path, str) or not isinstance(runner_contract_path, str):
        failures.append("frontend_c_api contract paths are malformed")
        return 0

    helper_contract = _load_checked_json(
        helper_contract_path, "frontend C API helper contract", failures
    )
    runner_contract = _load_checked_json(
        runner_contract_path, "frontend C API runner contract", failures
    )
    expected_helpers = _string_list(surface, "required_helpers", failures)
    expected_type_aliases = _string_list(surface, "required_type_aliases", failures)
    actual_helpers = _string_list(helper_contract, "required_helpers", failures)
    actual_type_aliases = _string_list(helper_contract, "required_type_aliases", failures)

    for label, expected, actual in (
        ("frontend C API helper", expected_helpers, actual_helpers),
        ("frontend C API type alias", expected_type_aliases, actual_type_aliases),
    ):
        added = sorted(set(actual) - set(expected))
        removed = sorted(set(expected) - set(actual))
        if added:
            failures.append(f"unreviewed {label} addition: " + ", ".join(added))
        if removed:
            failures.append(f"unreviewed {label} removal: " + ", ".join(removed))

    owner_contract = helper_contract.get("owner_contract")
    if not isinstance(owner_contract, dict):
        failures.append("frontend C API helper owner_contract is malformed")
    else:
        if owner_contract.get("no_retired_route_or_evidence_log_claims") is not True:
            failures.append("frontend C API helper contract permits retired/evidence-log claims")
        if (
            owner_contract.get("public_private_partition_owner")
            != surface.get("public_private_partition_owner")
        ):
            failures.append("frontend C API public/private partition owner drifted")

    runner_owner_contract = runner_contract.get("owner_contract")
    if not isinstance(runner_owner_contract, dict):
        failures.append("frontend C API runner owner_contract is malformed")
    elif runner_owner_contract.get("no_retired_route_or_evidence_log_claims") is not True:
        failures.append("frontend C API runner contract permits retired/evidence-log claims")
    _validate_frontend_c_api_live_contract(surface, helper_contract, failures)
    return len(expected_helpers) + len(expected_type_aliases)


def _read_contract_paths(
    contract: dict[str, Any],
    field_name: str,
    label: str,
    failures: list[str],
) -> str:
    paths = contract.get(field_name)
    if not isinstance(paths, list) or not paths:
        failures.append(f"{label} {field_name} must be a non-empty list")
        return ""
    contents: list[str] = []
    for raw_path in paths:
        if not isinstance(raw_path, str) or not raw_path:
            failures.append(f"{label} {field_name} contains a malformed path")
            continue
        path = _repo_path(raw_path)
        if not path.is_file():
            failures.append(f"{label} {field_name} referenced missing file {raw_path}")
            continue
        contents.append(path.read_text(encoding="utf-8"))
    return "\n".join(contents)


def _declared_c_api_type_aliases(header_text: str) -> set[str]:
    return {
        match.group(1)
        for match in re.finditer(
            r"typedef\s+[^;]*?\b(objc3c_frontend_c_[A-Za-z0-9_]+)\s*;",
            header_text,
            flags=re.DOTALL,
        )
    }


def _symbol_is_declared(text: str, symbol: str) -> bool:
    return re.search(rf"\b{re.escape(symbol)}\s*\(", text) is not None


def _validate_frontend_c_api_live_contract(
    surface: dict[str, Any],
    helper_contract: dict[str, Any],
    failures: list[str],
) -> None:
    header_text = _read_contract_paths(
        helper_contract, "header_paths", "frontend C API helper contract", failures
    )
    source_text = _read_contract_paths(
        helper_contract, "source_paths", "frontend C API helper contract", failures
    )
    if not header_text or not source_text:
        return

    required_type_aliases = _string_list(surface, "required_type_aliases", failures)
    required_helpers = _string_list(surface, "required_helpers", failures)
    declared_aliases = _declared_c_api_type_aliases(header_text)
    missing_aliases = sorted(set(required_type_aliases) - declared_aliases)
    if missing_aliases:
        failures.append(
            "frontend C API live headers missing required aliases: "
            + ", ".join(missing_aliases)
        )

    missing_header_helpers = sorted(
        helper for helper in required_helpers if not _symbol_is_declared(header_text, helper)
    )
    if missing_header_helpers:
        failures.append(
            "frontend C API live headers missing required helpers: "
            + ", ".join(missing_header_helpers)
        )
    missing_source_helpers = sorted(
        helper for helper in required_helpers if not _symbol_is_declared(source_text, helper)
    )
    if missing_source_helpers:
        failures.append(
            "frontend C API live sources missing required helpers: "
            + ", ".join(missing_source_helpers)
        )

    for phrase in helper_contract.get("required_header_ownership_phrases", []):
        if isinstance(phrase, str) and phrase and phrase not in header_text:
            failures.append(f"frontend C API live headers missing ownership phrase: {phrase}")
    for snippet in helper_contract.get("required_source_fail_closed_snippets", []):
        if isinstance(snippet, str) and snippet and snippet not in source_text:
            failures.append(f"frontend C API live sources missing fail-closed snippet: {snippet}")


def _validate_compatibility_windows(governance: dict[str, Any], failures: list[str]) -> None:
    policy = governance.get("compatibility_window_policy")
    if not isinstance(policy, dict):
        failures.append("compatibility_window_policy must be an object")
        return
    versioning_path = policy.get("versioning_model")
    deprecation_path = policy.get("deprecation_policy")
    if not isinstance(versioning_path, str) or not isinstance(deprecation_path, str):
        failures.append("compatibility policy dependency paths are malformed")
        return
    versioning = _load_checked_json(versioning_path, "versioning model", failures)
    deprecation = _load_checked_json(deprecation_path, "deprecation policy", failures)

    if versioning.get("supported_major_line") != policy.get("expected_supported_major_line"):
        failures.append("supported major line drifted from ABI/API governance policy")
    if versioning.get("current_minor_line") != policy.get("expected_current_minor_line"):
        failures.append("current minor line drifted from ABI/API governance policy")

    support_windows = versioning.get("support_windows")
    expected_statuses = policy.get("expected_channel_statuses")
    if not isinstance(support_windows, dict) or not isinstance(expected_statuses, dict):
        failures.append("support window policy is malformed")
    else:
        missing_channels = sorted(set(expected_statuses) - set(support_windows))
        extra_channels = sorted(set(support_windows) - set(expected_statuses))
        if missing_channels:
            failures.append("compatibility window missing channels: " + ", ".join(missing_channels))
        if extra_channels:
            failures.append("compatibility window added ungoverned channels: " + ", ".join(extra_channels))
        for channel, expected_status in expected_statuses.items():
            channel_payload = support_windows.get(channel)
            if not isinstance(channel_payload, dict):
                continue
            if channel_payload.get("status") != expected_status:
                failures.append(
                    f"compatibility window status drifted for {channel}: "
                    f"expected {expected_status!r}, observed {channel_payload.get('status')!r}"
                )

    forbidden_claims = set(_string_list(versioning, "forbidden_claims", failures))
    expected_forbidden_claims = set(
        value
        for value in policy.get("expected_forbidden_claims", [])
        if isinstance(value, str)
    )
    missing_forbidden_claims = sorted(expected_forbidden_claims - forbidden_claims)
    if missing_forbidden_claims:
        failures.append(
            "compatibility governance lost forbidden claims: "
            + ", ".join(missing_forbidden_claims)
        )

    blocking_states = {
        value
        for value in policy.get("claim_blocking_deprecation_states", [])
        if isinstance(value, str)
    }
    state_entries = deprecation.get("deprecation_states")
    if not isinstance(state_entries, list):
        failures.append("deprecation policy has no deprecation_states")
    else:
        state_map = {
            entry.get("state"): entry
            for entry in state_entries
            if isinstance(entry, dict) and isinstance(entry.get("state"), str)
        }
        for state in sorted(blocking_states):
            entry = state_map.get(state)
            if not isinstance(entry, dict):
                failures.append(f"deprecation policy missing blocking state {state}")
                continue
            if entry.get("public_claim_allowed") is not False:
                failures.append(f"compatibility window violation: {state} allows public claims")

    transition_policy = deprecation.get("state_transition_policy")
    if not isinstance(transition_policy, dict):
        failures.append("deprecation policy has no state_transition_policy")
    else:
        non_waivable_states = {
            value
            for value in transition_policy.get("non_waivable_states", [])
            if isinstance(value, str)
        }
        missing_non_waivable = sorted(blocking_states - non_waivable_states)
        if missing_non_waivable:
            failures.append(
                "compatibility blocking states became waiverable: "
                + ", ".join(missing_non_waivable)
            )


def _validate_package_abi_identity(
    governance: dict[str, Any],
    loaded: dict[str, dict[str, Any]],
    *,
    public_symbol_count: int,
    runtime_symbol_count: int,
    frontend_symbol_count: int,
    class_ids: set[str],
    failures: list[str],
) -> dict[str, Any]:
    package_policy = governance.get("package_abi_identity")
    if not isinstance(package_policy, dict):
        failures.append("package_abi_identity must be an object")
        return {}
    lock_schema = package_policy.get("package_lock_schema")
    package_lock_schema = (
        _load_checked_json(lock_schema, "package ABI identity package lock schema", failures)
        if isinstance(lock_schema, str)
        else {}
    )
    if not isinstance(lock_schema, str) or not (_repo_path(lock_schema)).is_file():
        failures.append("package ABI identity references missing package lock schema")
    if package_policy.get("package_lock_action") != "build-package-lock":
        failures.append("package ABI identity package_lock_action drifted")

    identity = package_policy.get("lockfile_abi_identity")
    if not isinstance(identity, dict):
        failures.append("package ABI identity lockfile_abi_identity must be an object")
        return {}
    taxonomy = governance.get("abi_version_taxonomy", {})
    versioning = loaded.get("release_operations_versioning_model", {})
    if identity.get("identity") != (taxonomy.get("identity") if isinstance(taxonomy, dict) else None):
        failures.append("package ABI identity drifted from ABI taxonomy identity")
    if identity.get("governance_manifest") != repo_rel(GOVERNANCE_MANIFEST):
        failures.append("package ABI identity governance_manifest drifted")
    if identity.get("governance_schema") != "schemas/objc3c-abi-api-governance-v1.schema.json":
        failures.append("package ABI identity governance_schema drifted")
    if identity.get("supported_major_line") != versioning.get("supported_major_line"):
        failures.append("package ABI identity supported_major_line drifted")
    if identity.get("current_minor_line") != versioning.get("current_minor_line"):
        failures.append("package ABI identity current_minor_line drifted")
    class_id = identity.get("compatibility_class")
    if not isinstance(class_id, str) or class_id not in class_ids:
        failures.append(f"package ABI identity references unknown compatibility class {class_id!r}")
    package_manager_abi_identity = _nested_value(
        package_lock_schema,
        ["properties", "package_manager", "properties", "abi_identity", "const"],
    )
    package_entry_abi_identity = _nested_value(
        package_lock_schema,
        ["$defs", "package", "properties", "abi_identity", "const"],
    )
    if package_manager_abi_identity != identity.get("identity"):
        failures.append("package lock schema package_manager ABI identity drifted")
    if package_entry_abi_identity != identity.get("identity"):
        failures.append("package lock schema package entry ABI identity drifted")

    expected_counts = {
        "stdlib_public_api_symbol_count": public_symbol_count,
        "stdlib_runtime_abi_symbol_count": runtime_symbol_count,
        "frontend_c_api_symbol_count": frontend_symbol_count,
    }
    for field_name, expected_count in expected_counts.items():
        if identity.get(field_name) != expected_count:
            failures.append(
                f"package ABI identity {field_name} drifted: "
                f"expected {expected_count}, observed {identity.get(field_name)!r}"
            )
    return dict(identity)


def _validate_compiler_artifact_compatibility(
    governance: dict[str, Any],
    *,
    class_ids: set[str],
    transition_sets: dict[str, list[str]],
    failures: list[str],
) -> int:
    policy = governance.get("compiler_artifact_compatibility")
    if not isinstance(policy, dict):
        failures.append("compiler_artifact_compatibility must be an object")
        return 0
    required_transition = policy.get("required_schema_transition_policy")
    transition_policy = governance.get("allowed_transition_policy", {})
    blocked = (
        transition_policy.get("release_blocked_transitions")
        if isinstance(transition_policy, dict)
        else []
    )
    if not isinstance(blocked, list) or required_transition not in blocked:
        failures.append("compiler artifact compatibility required schema transition is not release-blocking")

    surfaces = policy.get("artifact_schema_surfaces")
    if not isinstance(surfaces, list) or not surfaces:
        failures.append("compiler_artifact_compatibility.artifact_schema_surfaces is malformed")
        return 0
    seen: set[str] = set()
    for surface in surfaces:
        if not isinstance(surface, dict):
            failures.append("compiler artifact schema surface is malformed")
            continue
        surface_id = surface.get("surface_id")
        if not isinstance(surface_id, str) or not surface_id:
            failures.append("compiler artifact schema surface is missing surface_id")
            continue
        if surface_id in seen:
            failures.append(f"compiler artifact schema surface duplicated {surface_id}")
            continue
        seen.add(surface_id)
        class_id = surface.get("compatibility_class")
        if not isinstance(class_id, str) or class_id not in class_ids:
            failures.append(f"compiler artifact {surface_id} references unknown compatibility class {class_id!r}")
        transition_set = surface.get("allowed_transition_set")
        if not isinstance(transition_set, str) or transition_set not in transition_sets:
            failures.append(f"compiler artifact {surface_id} references unknown transition set {transition_set!r}")
        schema_path = surface.get("schema")
        identity_path = surface.get("identity_path")
        expected_identity = surface.get("expected_identity")
        if (
            not isinstance(schema_path, str)
            or not isinstance(identity_path, list)
            or not all(isinstance(segment, str) for segment in identity_path)
            or not isinstance(expected_identity, str)
        ):
            failures.append(f"compiler artifact {surface_id} schema identity contract is malformed")
            continue
        schema_payload = _load_checked_json(schema_path, f"compiler artifact {surface_id} schema", failures)
        if schema_payload:
            observed_identity = _nested_value(schema_payload, list(identity_path))
            if observed_identity != expected_identity:
                failures.append(
                    f"compiler artifact {surface_id} identity drifted: "
                    f"expected {expected_identity!r}, observed {observed_identity!r}"
                )
    return len(seen)


def run_check(
    *,
    governance_path: Path = GOVERNANCE_MANIFEST,
    summary_path: Path = SUMMARY_PATH,
) -> tuple[int, dict[str, Any]]:
    failures: list[str] = []
    governance = _load_governance(governance_path, failures)
    source_manifest_count = 0
    public_symbol_count = 0
    runtime_symbol_count = 0
    frontend_symbol_count = 0
    compiler_artifact_schema_count = 0
    package_abi_identity: dict[str, Any] = {}

    if governance:
        loaded_source_manifests = _load_source_manifests(governance, failures)
        class_ids = _compatibility_class_ids(governance, failures)
        transition_sets = _transition_sets(governance, failures)
        deprecation_states = _deprecation_state_map(
            loaded_source_manifests.get("long_horizon_deprecation_policy", {}),
            failures,
        )
        _validate_abi_version_taxonomy(
            governance,
            loaded_source_manifests,
            class_ids,
            failures,
        )
        _validate_deprecation_lifecycle(
            governance,
            loaded_source_manifests,
            deprecation_states,
            failures,
        )
        source_manifest_count = _validate_source_manifest_coverage(
            governance, loaded_source_manifests, failures
        )
        for surface in governance.get("stdlib_public_api", []):
            if isinstance(surface, dict):
                public_symbol_count += _validate_stdlib_symbol_surface(
                    surface,
                    governance=governance,
                    surface_kind="public-api",
                    class_ids=class_ids,
                    transition_sets=transition_sets,
                    deprecation_states=deprecation_states,
                    failures=failures,
                )
            else:
                failures.append("stdlib_public_api contained a malformed entry")
        for surface in governance.get("stdlib_runtime_abi", []):
            if isinstance(surface, dict):
                runtime_symbol_count += _validate_stdlib_symbol_surface(
                    surface,
                    governance=governance,
                    surface_kind="runtime-abi",
                    class_ids=class_ids,
                    transition_sets=transition_sets,
                    deprecation_states=deprecation_states,
                    failures=failures,
                )
            else:
                failures.append("stdlib_runtime_abi contained a malformed entry")
        frontend_symbol_count = _validate_frontend_c_api(
            governance,
            class_ids=class_ids,
            transition_sets=transition_sets,
            deprecation_states=deprecation_states,
            failures=failures,
        )
        _validate_compatibility_windows(governance, failures)
        package_abi_identity = _validate_package_abi_identity(
            governance,
            loaded_source_manifests,
            public_symbol_count=public_symbol_count,
            runtime_symbol_count=runtime_symbol_count,
            frontend_symbol_count=frontend_symbol_count,
            class_ids=class_ids,
            failures=failures,
        )
        compiler_artifact_schema_count = _validate_compiler_artifact_compatibility(
            governance,
            class_ids=class_ids,
            transition_sets=transition_sets,
            failures=failures,
        )

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS" if not failures else "FAIL",
        "governance_manifest": _display_path(governance_path),
        "governance_schema": GOVERNANCE_SCHEMA_ID,
        "release_blocker_issue_refs": governance.get("release_blocker_issue_refs", []),
        "source_manifest_count": source_manifest_count,
        "public_api_symbol_count": public_symbol_count,
        "runtime_abi_symbol_count": runtime_symbol_count,
        "frontend_c_api_symbol_count": frontend_symbol_count,
        "governed_symbol_surface_count": (
            len(governance.get("stdlib_public_api", []))
            + len(governance.get("stdlib_runtime_abi", []))
            + (1 if isinstance(governance.get("frontend_c_api"), dict) else 0)
            if governance
            else 0
        ),
        "compiler_artifact_schema_count": compiler_artifact_schema_count,
        "package_abi_identity": package_abi_identity,
        "compatibility_policy": (
            governance.get("compatibility_window_policy", {}) if governance else {}
        ),
        "failure_count": len(failures),
        "failures": failures,
    }
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(summary_path, summary)
    return (0 if not failures else 1), summary


def main() -> int:
    rc, summary = run_check()
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print(
        "objc3c-release-abi-api-drift: PASS"
        if rc == 0
        else f"objc3c-release-abi-api-drift: FAIL ({summary['failure_count']} failures)"
    )
    return rc
