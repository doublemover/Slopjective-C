from __future__ import annotations

import json
from copy import deepcopy
from pathlib import Path
from typing import Any

from objc3c_shared.schema_registry import validate_registered_schema

ROOT = Path(__file__).resolve().parents[1]
SCHEMA_ID = "objc3c-standalone-textual-interface-payload-v1"
FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "module_interfaces"
    / "standalone_textual_interface_payload.json"
)
DIAGNOSTIC = "O3IFC8238"
PUBLIC_VALIDATE_COMMAND = (
    "npm run objc3c -- validate-standalone-textual-interface-payload"
)
REQUIRED_ISSUE_REFS = {8238, 8208, 8233, 8234}
REQUIRED_SOURCE_TRUTH_POLICY = {
    "source_truth": "native-artifact-schema-fixture-public-command",
    "local_temp_source_truth_allowed": False,
    "generated_output_source_truth_allowed": False,
    "fallback_success_allowed": False,
    "schema_registry_required": True,
    "public_command_evidence_required": True,
    "capability_truth_row": "modules.standalone-textual-interface-payload",
    "umbrella_readiness_issue_ref": 8208,
}
REQUIRED_NEGATIVE_CASE_IDS = {
    "stale-schema",
    "unlocked-import",
    "hidden-declaration",
    "count-drift",
    "reserved-roundtrip",
    "typed-throws-abi-lowering",
    "typed-throws-interface-contract-drift",
    "value-optional-lowering",
    "value-optional-layout-drift",
}


def _as_dict(value: Any, field: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        raise RuntimeError(f"{field} must be an object")
    return value


def _as_list(value: Any, field: str) -> list[Any]:
    if not isinstance(value, list):
        raise RuntimeError(f"{field} must be an array")
    return value


def _string_list(value: Any, field: str) -> list[str]:
    items = _as_list(value, field)
    if not all(isinstance(item, str) and item for item in items):
        raise RuntimeError(f"{field} must contain non-empty strings")
    if len(items) != len(set(items)):
        raise RuntimeError(f"{field} must not contain duplicate strings")
    return items


def _int_list(value: Any, field: str) -> list[int]:
    items = _as_list(value, field)
    if not all(isinstance(item, int) for item in items):
        raise RuntimeError(f"{field} must contain integers")
    if len(items) != len(set(items)):
        raise RuntimeError(f"{field} must not contain duplicate integers")
    return items


def _require_string(obj: dict[str, Any], field: str, path: str) -> str:
    value = obj.get(field)
    if not isinstance(value, str) or not value:
        raise RuntimeError(f"{path}.{field} must be a non-empty string")
    return value


def _source_anchor_path(anchor: dict[str, Any], path: str) -> str:
    source_path = _require_string(anchor, "path", path)
    forbidden_prefixes = (
        "tmp/",
        "tmp\\",
        "temp/",
        "temp\\",
        "generated/",
        "generated\\",
        "build/",
        "build\\",
        "dist/",
        "dist\\",
    )
    source_repo_path = Path(source_path)
    if (
        source_path.startswith(forbidden_prefixes)
        or source_repo_path.is_absolute()
        or ".." in source_repo_path.parts
    ):
        raise RuntimeError(f"{path}.path must be checked-in durable source")
    if not (ROOT / source_path).exists():
        raise RuntimeError(f"{path}.path does not exist: {source_path}")
    return source_path


def _validate_source_anchor(owner: dict[str, Any], path: str) -> None:
    anchor = _as_dict(owner.get("source_anchor"), f"{path}.source_anchor")
    _source_anchor_path(anchor, f"{path}.source_anchor")
    for field in ("line", "column"):
        if not isinstance(anchor.get(field), (int, float)):
            raise RuntimeError(f"{path}.source_anchor.{field} must be numeric")


def _validate_import(import_record: dict[str, Any], module_id: str, index: int) -> None:
    path = f"imports[{index}]"
    if _require_string(import_record, "from_module", path) != module_id:
        raise RuntimeError(f"{path}.from_module must match module_id")
    _require_string(import_record, "to_module", path)
    _require_string(import_record, "interface_payload_id", path)
    lock_identity = _require_string(import_record, "lock_identity", path)
    if not lock_identity.startswith("package-lock:") or ":trust:" not in lock_identity:
        raise RuntimeError(f"{path}.lock_identity must be package lock/trust identity")
    capabilities = _string_list(import_record.get("capability_requirements"), f"{path}.capability_requirements")
    if "modules.public-import-lookup" not in capabilities:
        raise RuntimeError(f"{path}.capability_requirements missing module import capability")


def _validate_declaration(
    declaration: dict[str, Any],
    declaration_ids: set[str],
    index: int,
) -> str:
    path = f"declarations[{index}]"
    declaration_id = _require_string(declaration, "declaration_id", path)
    if declaration_id in declaration_ids:
        raise RuntimeError(f"{path}.declaration_id duplicates {declaration_id}")
    declaration_ids.add(declaration_id)
    kind = _require_string(declaration, "kind", path)
    if kind in {"implementation", "private_implementation"}:
        raise RuntimeError(f"{path}.kind must remain reserved metadata")
    if _require_string(declaration, "visibility", path) not in {"public", "package", "internal"}:
        raise RuntimeError(f"{path}.visibility is not importable")
    _require_string(declaration, "name", path)
    type_signature = _as_dict(declaration.get("type_signature"), f"{path}.type_signature")
    effects = _as_dict(declaration.get("effects"), f"{path}.effects")
    _validate_value_optional_contract(type_signature, f"{path}.type_signature")
    _validate_typed_throws_contract(effects, f"{path}.effects")
    for object_field in ("ownership", "runtime_metadata"):
        _as_dict(declaration.get(object_field), f"{path}.{object_field}")
    _as_list(declaration.get("generics"), f"{path}.generics")
    _validate_source_anchor(declaration, path)
    return kind


def _validate_value_optional_contract(type_signature: dict[str, Any], path: str) -> None:
    contract_value = type_signature.get("value_optional_contract")
    if contract_value is None:
        return
    contract = _as_dict(contract_value, f"{path}.value_optional_contract")
    if contract.get("issue_ref") != 8234:
        raise RuntimeError(f"{path}.value_optional_contract.issue_ref must be 8234")
    expected_strings = {
        "canonical_spelling": "Optional<T>",
        "source_status": "semantic-type-signature-admitted-bounded-scalar-runtime-abi",
        "abi_layout_status": "stable-packed-presence-payload-runtime-lowered",
        "abi_layout_id": "objc3.value_optional.inline_presence_payload.v1",
        "runtime_abi_payload_scope": "supported-scalar-payload-forms-only",
        "interface_roundtrip_status": "semantic-carrier-roundtrips-bounded-scalar-runtime-abi",
    }
    for field, expected in expected_strings.items():
        if contract.get(field) != expected:
            raise RuntimeError(
                f"{path}.value_optional_contract.{field} expected {expected!r}, saw {contract.get(field)!r}"
            )
    supported_payloads = _string_list(
        contract.get("supported_runtime_payload_forms"),
        f"{path}.value_optional_contract.supported_runtime_payload_forms",
    )
    if supported_payloads != ["i32"]:
        raise RuntimeError(
            f"{path}.value_optional_contract.supported_runtime_payload_forms must be ['i32']"
        )
    expected_true = (
        "semantic_value_model_supported",
        "explicit_present_absent_construction_modeled",
        "binding_narrowing_supported",
        "unwrap_requires_presence_check",
        "stable_abi_layout_contract_supported",
        "interface_roundtrip_supported",
        "runtime_execution_supported",
        "lowering_supported",
        "ir_payload_emission_supported",
        "call_abi_lowering_supported",
    )
    for field in expected_true:
        if contract.get(field) is not True:
            raise RuntimeError(f"{path}.value_optional_contract.{field} must be true")
    expected_false = (
        "lowercase_alias_accepted",
        "broad_public_runtime_support_claim_allowed",
        "nested_value_optional_runtime_supported",
        "generic_payload_runtime_supported",
        "property_storage_supported",
        "ivar_storage_supported",
        "nil_to_scalar_coercion_allowed",
        "implicit_nil_absence_allowed",
        "unchecked_unwrap_allowed",
        "nullable_pointer_conversion_allowed",
        "throws_result_conversion_allowed",
    )
    for field in expected_false:
        if contract.get(field) is not False:
            raise RuntimeError(f"{path}.value_optional_contract.{field} must be false")


def _validate_typed_throws_contract(effects: dict[str, Any], path: str) -> None:
    contract_value = effects.get("typed_throws")
    if contract_value is None:
        return
    contract = _as_dict(contract_value, f"{path}.typed_throws")
    if contract.get("issue_ref") != 8233:
        raise RuntimeError(f"{path}.typed_throws.issue_ref must be 8233")
    if contract.get("canonical_syntax") != "throws(E)":
        raise RuntimeError(f"{path}.typed_throws.canonical_syntax must be throws(E)")
    throws_kind = _require_string(effects, "throws_kind", path)
    declared_error_type = str(effects.get("declared_error_type", ""))
    if contract.get("throws_kind") != throws_kind:
        raise RuntimeError(f"{path}.typed_throws.throws_kind must match effects.throws_kind")
    if contract.get("declared_error_type") != declared_error_type:
        raise RuntimeError(
            f"{path}.typed_throws.declared_error_type must match effects.declared_error_type"
        )
    expected_key = (
        "throws:none"
        if throws_kind == "none"
        else f"throws:{throws_kind}:{declared_error_type}"
    )
    if contract.get("effect_signature_key") != expected_key:
        raise RuntimeError(f"{path}.typed_throws.effect_signature_key drift")
    if contract.get("silent_erasure_allowed") is not False:
        raise RuntimeError(f"{path}.typed_throws.silent_erasure_allowed must be false")
    if contract.get("multi_payload_supported") is not False:
        raise RuntimeError(f"{path}.typed_throws.multi_payload_supported must be false")
    if throws_kind == "none":
        if contract.get("typed_payload_arity") != 0:
            raise RuntimeError(f"{path}.typed_throws.typed_payload_arity must be 0")
        if contract.get("runtime_execution_claimed") is not False:
            raise RuntimeError(
                f"{path}.typed_throws.runtime_execution_claimed must be false for nonthrowing effects"
            )
        if contract.get("abi_status") != "none":
            raise RuntimeError(f"{path}.typed_throws.abi_status must be none")
        if contract.get("interface_roundtrip_status") != "not-applicable":
            raise RuntimeError(
                f"{path}.typed_throws.interface_roundtrip_status must be not-applicable"
            )
        if contract.get("typed_payload_lowering_ready") is not False:
            raise RuntimeError(f"{path}.typed_throws.typed_payload_lowering_ready must be false")
    elif throws_kind == "typed":
        if not effects.get("throws"):
            raise RuntimeError(f"{path}.throws must be true for typed throws")
        if not declared_error_type:
            raise RuntimeError(f"{path}.declared_error_type must preserve typed throws payload")
        if contract.get("typed_payload_arity") != 1:
            raise RuntimeError(f"{path}.typed_throws.typed_payload_arity must be 1")
        if contract.get("typed_payload_status") != "source-preserved-error-out-abi-lowered":
            raise RuntimeError(
                f"{path}.typed_throws.typed_payload_status must preserve typed payload ABI"
            )
        if contract.get("typed_payload_lowering_ready") is not True:
            raise RuntimeError(f"{path}.typed_throws.typed_payload_lowering_ready must be true")
        if contract.get("runtime_execution_claimed") is not True:
            raise RuntimeError(f"{path}.typed_throws.runtime_execution_claimed must be true")
        if contract.get("abi_status") != "typed-error-out-abi":
            raise RuntimeError(f"{path}.typed_throws.abi_status must be typed-error-out-abi")
        if contract.get("interface_roundtrip_status") != "typed-payload-preserved":
            raise RuntimeError(
                f"{path}.typed_throws.interface_roundtrip_status must be typed-payload-preserved"
            )
    elif throws_kind == "untyped":
        if not effects.get("throws"):
            raise RuntimeError(f"{path}.throws must be true for untyped throws")
        if declared_error_type != "id<Error>":
            raise RuntimeError(f"{path}.declared_error_type must be id<Error> for untyped throws")
        if contract.get("typed_payload_arity") != 0:
            raise RuntimeError(f"{path}.typed_throws.typed_payload_arity must be 0")
        if contract.get("runtime_execution_claimed") is not False:
            raise RuntimeError(
                f"{path}.typed_throws.runtime_execution_claimed must be false for untyped throws"
            )
        if contract.get("abi_status") != "untyped-error-out-abi":
            raise RuntimeError(
                f"{path}.typed_throws.abi_status must be untyped-error-out-abi"
            )
        if contract.get("interface_roundtrip_status") != "not-applicable":
            raise RuntimeError(
                f"{path}.typed_throws.interface_roundtrip_status must be not-applicable"
            )
        if contract.get("typed_payload_lowering_ready") is not False:
            raise RuntimeError(f"{path}.typed_throws.typed_payload_lowering_ready must be false")
    else:
        raise RuntimeError(f"{path}.typed_throws.throws_kind must be none, untyped, or typed")


def _validate_reserved_metadata(metadata: dict[str, Any], index: int) -> None:
    path = f"reserved_metadata[{index}]"
    _require_string(metadata, "surface", path)
    _require_string(metadata, "name", path)
    _require_string(metadata, "status", path)
    diagnostic = _require_string(metadata, "diagnostic", path)
    if not diagnostic.startswith("O3"):
        raise RuntimeError(f"{path}.diagnostic must be a stable O3 code")
    if metadata.get("fail_closed") is not True:
        raise RuntimeError(f"{path}.fail_closed must be true")
    _validate_source_anchor(metadata, path)


def _validate_counts(
    source_counts: dict[str, Any],
    declaration_kinds: list[str],
    reserved_count: int,
) -> None:
    expected = {
        "globals": declaration_kinds.count("global"),
        "protocols": declaration_kinds.count("protocol"),
        "interfaces": declaration_kinds.count("interface")
        + declaration_kinds.count("actor_interface"),
        "implementations_reserved": reserved_count,
        "functions": declaration_kinds.count("function"),
    }
    for field, value in expected.items():
        if source_counts.get(field) != value:
            raise RuntimeError(f"source_counts.{field} expected {value}, saw {source_counts.get(field)}")


def _validate_source_truth_policy(payload: dict[str, Any]) -> None:
    issue_refs = set(_int_list(payload.get("issue_refs"), "issue_refs"))
    missing_issues = REQUIRED_ISSUE_REFS - issue_refs
    if missing_issues:
        raise RuntimeError(f"issue_refs missing {sorted(missing_issues)}")

    policy = _as_dict(payload.get("source_truth_policy"), "source_truth_policy")
    for field, expected in REQUIRED_SOURCE_TRUTH_POLICY.items():
        if policy.get(field) != expected:
            raise RuntimeError(
                f"source_truth_policy.{field} expected {expected!r}, saw {policy.get(field)!r}"
            )


def _declared_negative_case_ids(payload: dict[str, Any]) -> set[str]:
    records = [_as_dict(row, "negative_cases[]") for row in _as_list(payload.get("negative_cases"), "negative_cases")]
    ids: set[str] = set()
    for index, record in enumerate(records):
        path = f"negative_cases[{index}]"
        case_id = _require_string(record, "case_id", path)
        if case_id in ids:
            raise RuntimeError(f"{path}.case_id duplicates {case_id}")
        ids.add(case_id)
        _require_string(record, "target", path)
        _require_string(record, "expected_failure", path)
        if record.get("fail_closed") is not True:
            raise RuntimeError(f"{path}.fail_closed must be true")
    missing = REQUIRED_NEGATIVE_CASE_IDS - ids
    if missing:
        raise RuntimeError(f"negative_cases missing {sorted(missing)}")
    return ids


def validate_payload(payload: dict[str, Any], *, root: Path = ROOT) -> dict[str, Any]:
    del root
    validate_registered_schema(payload, SCHEMA_ID)
    if payload["payload_kind"] != "objc3c.standalone_textual_interface_payload.v1":
        raise RuntimeError("payload_kind is not standalone textual interface v1")
    module_id = _require_string(payload, "module_id", "payload")
    interface_payload_id = _require_string(payload, "interface_payload_id", "payload")
    if interface_payload_id != f"{module_id}:standalone-textual-interface:v1":
        raise RuntimeError("interface_payload_id must be deterministic from module_id")
    _require_string(payload, "package_id", "payload")
    _require_string(payload, "producer_version", "payload")
    _validate_source_anchor(payload, "payload")

    target_constraints = _as_dict(payload.get("target_constraints"), "target_constraints")
    if target_constraints.get("language_profile") != "canonical":
        raise RuntimeError("target_constraints.language_profile must be canonical")
    if target_constraints.get("arc_mode") not in {"enabled", "disabled"}:
        raise RuntimeError("target_constraints.arc_mode must be enabled or disabled")

    imports = [_as_dict(row, "imports[]") for row in _as_list(payload.get("imports"), "imports")]
    for index, import_record in enumerate(imports):
        _validate_import(import_record, module_id, index)

    declarations = [
        _as_dict(row, "declarations[]")
        for row in _as_list(payload.get("declarations"), "declarations")
    ]
    declaration_ids: set[str] = set()
    declaration_kinds = [
        _validate_declaration(row, declaration_ids, index)
        for index, row in enumerate(declarations)
    ]

    reserved_metadata = [
        _as_dict(row, "reserved_metadata[]")
        for row in _as_list(payload.get("reserved_metadata"), "reserved_metadata")
    ]
    for index, metadata in enumerate(reserved_metadata):
        _validate_reserved_metadata(metadata, index)

    roundtrip = _as_dict(payload.get("interface_roundtrip"), "interface_roundtrip")
    if roundtrip.get("payload_id") != interface_payload_id:
        raise RuntimeError("interface_roundtrip.payload_id must match interface_payload_id")
    if roundtrip.get("parse_status") != "supported":
        raise RuntimeError("interface_roundtrip.parse_status must be supported")
    if roundtrip.get("semantic_equivalence_status") != "supported":
        raise RuntimeError("interface_roundtrip.semantic_equivalence_status must be supported")
    if roundtrip.get("drift_diagnostic") != DIAGNOSTIC:
        raise RuntimeError(f"interface_roundtrip.drift_diagnostic must be {DIAGNOSTIC}")

    capabilities = _string_list(payload.get("capability_requirements"), "capability_requirements")
    for capability in (
        "modules.public-import-lookup",
        "modules.standalone-textual-interface-payload",
    ):
        if capability not in capabilities:
            raise RuntimeError(f"capability_requirements missing {capability}")
    commands = _string_list(payload.get("public_commands"), "public_commands")
    if PUBLIC_VALIDATE_COMMAND not in commands:
        raise RuntimeError("public validation command missing")

    _validate_counts(_as_dict(payload.get("source_counts"), "source_counts"), declaration_kinds, len(reserved_metadata))
    _validate_source_truth_policy(payload)
    _declared_negative_case_ids(payload)
    return {
        "status": "PASS",
        "module_id": module_id,
        "interface_payload_id": interface_payload_id,
        "import_count": len(imports),
        "declaration_count": len(declarations),
        "reserved_metadata_count": len(reserved_metadata),
        "replay_key": (
            "objc3c.standalone-textual-interface-import.v1"
            f";payload={interface_payload_id};module={module_id}"
            f";imports={len(imports)};declarations={len(declarations)}"
            f";reserved={len(reserved_metadata)}"
        ),
    }


def negative_payload_cases(payload: dict[str, Any]) -> dict[str, str]:
    cases: dict[str, str] = {}
    stale_schema = deepcopy(payload)
    stale_schema["schema_version"] = "objc3c-standalone-textual-interface-payload-v0"
    cases["stale-schema"] = _failure(stale_schema)

    unlocked_import = deepcopy(payload)
    unlocked_import["imports"][0]["lock_identity"] = "unlocked:Foundation"
    cases["unlocked-import"] = _failure(unlocked_import)

    hidden_declaration = deepcopy(payload)
    hidden_declaration["declarations"][0]["kind"] = "implementation"
    cases["hidden-declaration"] = _failure(hidden_declaration)

    count_drift = deepcopy(payload)
    count_drift["source_counts"]["interfaces"] = 0
    cases["count-drift"] = _failure(count_drift)

    reserved_roundtrip = deepcopy(payload)
    reserved_roundtrip["interface_roundtrip"]["parse_status"] = "reserved-importer-not-landed"
    cases["reserved-roundtrip"] = _failure(reserved_roundtrip)

    typed_throws_abi = deepcopy(payload)
    typed_throws_abi["declarations"][1]["effects"]["typed_throws"][
        "runtime_execution_claimed"
    ] = True
    cases["typed-throws-abi-lowering"] = _failure(typed_throws_abi)

    typed_throws_contract_drift = deepcopy(payload)
    typed_throws_contract_drift["declarations"][1]["effects"]["typed_throws"][
        "declared_error_type"
    ] = "ErasedError"
    cases["typed-throws-interface-contract-drift"] = _failure(
        typed_throws_contract_drift
    )

    value_optional_lowering = deepcopy(payload)
    value_optional_lowering["declarations"][1]["type_signature"][
        "value_optional_contract"
    ]["supported_runtime_payload_forms"] = ["i32", "bool"]
    cases["value-optional-lowering"] = _failure(value_optional_lowering)

    value_optional_layout_drift = deepcopy(payload)
    value_optional_layout_drift["declarations"][1]["type_signature"][
        "value_optional_contract"
    ]["abi_layout_id"] = "objc3.value_optional.drift"
    cases["value-optional-layout-drift"] = _failure(value_optional_layout_drift)

    declared = _declared_negative_case_ids(payload)
    if set(cases) != declared:
        raise RuntimeError(
            "declared negative case ids do not match exercised cases: "
            f"declared={sorted(declared)} exercised={sorted(cases)}"
        )
    return cases


def _failure(payload: dict[str, Any]) -> str:
    try:
        validate_payload(payload)
    except Exception as exc:  # noqa: BLE001 - negative fixture summarization
        return str(exc)
    raise RuntimeError("negative textual interface payload unexpectedly passed")


def main() -> int:
    payload = json.loads(FIXTURE.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError("textual interface fixture must be a JSON object")
    summary = validate_payload(payload)
    summary["negative_cases"] = negative_payload_cases(payload)
    print(json.dumps(summary, indent=2, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
