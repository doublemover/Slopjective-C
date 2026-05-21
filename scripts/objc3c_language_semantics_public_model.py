"""Validator for the Objective-C 3.0 public language semantics model."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
import sys
from typing import Any

SCRIPTS_ROOT = Path(__file__).resolve().parent
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_tooling.json_io import JsonSchemaValidationError
from objc3c_tooling.json_io import require_json_object
from objc3c_tooling.json_io import validate_json_schema
from objc3c_tooling.paths import ROOT
from objc3c_tooling.paths import repo_rel


CONTRACT_ID = "objc3c.language.semantics.public-model.v1"
CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "language_semantics_public_model"
    / "public_language_semantics_contract.json"
)
SCHEMA_PATH = ROOT / "schemas" / "objc3c-language-semantics-public-model-v1.schema.json"
MODEL_HEADER_PATH = ROOT / "native" / "objc3c" / "src" / "sema" / "model" / "language_semantics_public_model.h"
REPORT_PATH = ROOT / "tmp" / "reports" / "language-semantics-public-model.json"

REQUIRED_ISSUES = {8160, 8163, 8164, 8165, 8166, 8167}
REQUIRED_SUPPORT_CLAIMS = {
    "objc3c.behavior.language.generics.public-type-parameters",
    "objc3c.behavior.language.modules.visibility-rebuild-contract",
    "objc3c.behavior.language.protocols.existential-witness-model",
    "objc3c.behavior.language.interop.foreign-surface-contract",
    "objc3c.behavior.language.ownership.memory-model",
    "objc3c.behavior.language.concurrency.public-usability-model",
}
REQUIRED_EVIDENCE_IDS = {
    "objc3c.evidence.language_semantics.public_model.fixture",
    "objc3c.evidence.language_semantics.public_model.schema",
    "objc3c.evidence.language_semantics.public_model.sema_header",
    "objc3c.evidence.language_semantics.public_model.validator",
    "objc3c.evidence.language_semantics.module_interop.fixture",
    "objc3c.evidence.language_semantics.module_interop.validator",
}
REQUIRED_SURFACES = {
    "generic_type_system",
    "module_identity_visibility_model",
    "protocol_existential_witness_model",
    "interop_bridge_model",
    "ownership_memory_model",
    "concurrency_usability_model",
}
FALSE_UNSUPPORTED_POLICY_FIELDS = {
    "fallback_or_compatibility_shim_allowed",
    "legacy_objc2_compatibility_claimed",
    "higher_kinded_types_claimed",
    "generic_collection_abi_claimed",
    "swift_protocol_bridge_claimed",
    "distributed_actors_claimed",
    "swift_abi_import_claimed",
    "cpp_template_import_claimed",
}


@dataclass(frozen=True)
class LanguageSemanticsPublicModelValidationResult:
    payload: dict[str, Any]
    failures: list[str]

    @property
    def passed(self) -> bool:
        return not self.failures


def _as_list(value: object) -> list[object]:
    return value if isinstance(value, list) else []


def _as_dict(value: object) -> dict[str, Any]:
    return value if isinstance(value, dict) else {}


def _require_path_with_tokens(path_text: str, tokens: list[object], failures: list[str]) -> None:
    path = ROOT / path_text
    if not path.is_file():
        failures.append(f"source anchor missing: {path_text}")
        return
    text = path.read_text(encoding="utf-8")
    for token in tokens:
        if not isinstance(token, str) or token not in text:
            failures.append(f"source anchor token missing from {path_text}: {token}")


def _collect_public_states(rows: list[object]) -> set[str]:
    return {
        str(row.get("public_state"))
        for row in rows
        if isinstance(row, dict) and row.get("public_state")
    }


def _validate_issue_mapping(contract: dict[str, Any], failures: list[str]) -> dict[str, Any]:
    mapping = _as_dict(contract.get("issue_mapping"))
    issues = {int(issue) for issue in _as_list(mapping.get("primary_issues"))}
    support_claims = {str(claim) for claim in _as_list(mapping.get("support_claims"))}
    evidence_ids = {str(evidence_id) for evidence_id in _as_list(mapping.get("evidence_ids_required"))}

    if not REQUIRED_ISSUES.issubset(issues):
        failures.append("language semantics public model must map issues #8160, #8163, #8164, #8165, #8166, and #8167")
    if not REQUIRED_SUPPORT_CLAIMS.issubset(support_claims):
        failures.append("language semantics public model support claims are incomplete")
    if not REQUIRED_EVIDENCE_IDS.issubset(evidence_ids):
        failures.append("language semantics public model evidence ids are incomplete")

    return {
        "issues": sorted(issues),
        "support_claims": sorted(support_claims),
        "evidence_ids_required": sorted(evidence_ids),
    }


def _validate_source_truth(contract: dict[str, Any], failures: list[str]) -> dict[str, Any]:
    source_truth = _as_dict(contract.get("source_truth"))
    schema_path = str(source_truth.get("schema_path", ""))
    model_header_path = str(source_truth.get("model_header_path", ""))
    workflow_action = str(source_truth.get("workflow_action", ""))

    if schema_path != repo_rel(SCHEMA_PATH):
        failures.append("language semantics schema path drifted")
    if model_header_path != repo_rel(MODEL_HEADER_PATH):
        failures.append("language semantics model header path drifted")
    if workflow_action != "validate-language-semantics-public-model":
        failures.append("language semantics workflow action drifted")

    for anchor in _as_list(source_truth.get("source_anchors")):
        if isinstance(anchor, dict):
            _require_path_with_tokens(
                str(anchor.get("path", "")),
                _as_list(anchor.get("tokens")),
                failures,
            )
        else:
            failures.append("language semantics source anchor is not an object")

    return {
        "schema_path": schema_path,
        "model_header_path": model_header_path,
        "workflow_action": workflow_action,
        "source_anchor_count": len(_as_list(source_truth.get("source_anchors"))),
    }


def _validate_generic_surface(surface: dict[str, Any], failures: list[str]) -> None:
    type_parameters = _as_list(surface.get("type_parameters"))
    specialization_policy = _as_dict(surface.get("specialization_policy"))

    if "supported" not in _collect_public_states(type_parameters):
        failures.append("generic type system must publish at least one supported type parameter")
    if not any("Persistable" in _as_list(row.get("bounds")) for row in type_parameters if isinstance(row, dict)):
        failures.append("generic type system must bind at least one type parameter to a protocol constraint")

    identity_fields = {str(field) for field in _as_list(specialization_policy.get("identity_key_fields"))}
    required_identity_fields = {
        "declaration_owner",
        "parameter_names_source_order",
        "argument_canonical_spelling",
        "constraint_protocols_lexicographic",
    }
    if not required_identity_fields.issubset(identity_fields):
        failures.append("generic specialization identity key fields are incomplete")
    if specialization_policy.get("runtime_metadata_required") is not True:
        failures.append("generic specialization policy must require runtime metadata identity")
    unsupported = {str(pattern) for pattern in _as_list(specialization_policy.get("unsupported_patterns"))}
    for required in ("higher_kinded_type_parameters", "generic_collection_abi"):
        if required not in unsupported:
            failures.append(f"generic unsupported policy missing: {required}")


def _validate_protocol_surface(surface: dict[str, Any], failures: list[str]) -> None:
    identity = _as_dict(surface.get("existential_identity"))
    witness_table = _as_dict(surface.get("witness_table"))
    conformance_metadata = _as_dict(surface.get("conformance_metadata"))
    composition_policy = _as_dict(surface.get("composition_policy"))
    unsupported_semantics = _as_dict(surface.get("unsupported_semantics"))

    identity_fields = {str(field) for field in _as_list(identity.get("key_fields"))}
    if not {"protocols_lexicographic", "module_owner"}.issubset(identity_fields):
        failures.append("protocol existential identity must be module-owned and lexicographic")
    witness_fields = {str(field) for field in _as_list(witness_table.get("key_fields"))}
    if not {"protocol_owner_identity", "conformance_owner_identity", "requirement_selector_or_property_key"}.issubset(witness_fields):
        failures.append("protocol witness table key fields are incomplete")
    if witness_table.get("runtime_lookup_anchor") != "QueryRealizedClassProtocolConformanceUnlocked":
        failures.append("protocol witness table must anchor to runtime conformance lookup")
    if witness_table.get("associated_type_policy") != "rejected":
        failures.append("protocol associated types must stay rejected until executable proof lands")
    if conformance_metadata.get("runtime_record") != "ProtocolConformanceMatch":
        failures.append("protocol conformance metadata must bind ProtocolConformanceMatch")
    if conformance_metadata.get("builder") != "BuildRuntimeProtocolExistentialWitnessMetadata":
        failures.append("protocol conformance metadata must use the runtime witness metadata builder")
    conformance_fields = {str(field) for field in _as_list(conformance_metadata.get("key_fields"))}
    if not {"protocol_owner_identity", "conformance_owner_identity", "runtime_lookup_anchor"}.issubset(conformance_fields):
        failures.append("protocol conformance metadata key fields are incomplete")
    associated_types = _as_dict(unsupported_semantics.get("associated_types"))
    dynamic_dispatch = _as_dict(unsupported_semantics.get("dynamic_existential_dispatch"))
    if associated_types.get("public_state") != "rejected" or associated_types.get("diagnostic_code") != "O3P100":
        failures.append("protocol associated-type policy must publish rejected O3P100 diagnostics")
    if dynamic_dispatch.get("public_state") != "rejected" or dynamic_dispatch.get("diagnostic_code") != "O3S314":
        failures.append("dynamic protocol existential dispatch must publish rejected O3S314 diagnostics")
    if any(composition_policy.get(field) != "rejected" for field in ("duplicates", "unknown_protocols", "conflicting_requirements")):
        failures.append("protocol composition policy must reject duplicate, unknown, and conflicting protocols")


def _validate_module_surface(surface: dict[str, Any], failures: list[str]) -> None:
    identity_fields = {str(field) for field in _as_list(surface.get("module_identity_key_fields"))}
    required_identity_fields = {
        "module_name",
        "metadata_version",
        "abi_identity",
        "package_lock_identity",
    }
    if not required_identity_fields.issubset(identity_fields):
        failures.append("module identity key fields are incomplete")

    visibility_policy = _as_dict(surface.get("visibility_policy"))
    if visibility_policy.get("hidden_import_access") != "rejected":
        failures.append("module visibility policy must reject hidden import access")
    if visibility_policy.get("reexports") != "public-import-required":
        failures.append("module reexport policy must require public imports")

    rebuild_policy = _as_dict(surface.get("rebuild_policy"))
    if rebuild_policy.get("deterministic") is not True:
        failures.append("module rebuild policy must be deterministic")
    rebuild_inputs = {str(value) for value in _as_list(rebuild_policy.get("identity_inputs"))}
    required_inputs = {
        "module_identity",
        "import_graph",
        "visibility",
        "foreign_surfaces",
        "package_lock",
    }
    if not required_inputs.issubset(rebuild_inputs):
        failures.append("module rebuild identity inputs are incomplete")
    invalidation_conditions = {
        str(value) for value in _as_list(rebuild_policy.get("invalidation_conditions"))
    }
    if "visibility-surface-drift" not in invalidation_conditions:
        failures.append("module rebuild policy must invalidate on visibility drift")


def _validate_interop_surface(surface: dict[str, Any], failures: list[str]) -> None:
    lanes = [lane for lane in _as_list(surface.get("lanes")) if isinstance(lane, dict)]
    lane_states = {
        str(lane.get("language")): str(lane.get("public_state"))
        for lane in lanes
    }
    if set(lane_states) != {"c", "objc2", "swift", "cpp"}:
        failures.append("interop bridge lanes must cover C, ObjC2, Swift, and C++ exactly")
    for supported_lane in ("c", "objc2"):
        if lane_states.get(supported_lane) != "supported":
            failures.append(f"interop bridge lane must be supported: {supported_lane}")
    for reserved_lane in ("swift", "cpp"):
        if lane_states.get(reserved_lane) != "reserved":
            failures.append(f"interop bridge lane must be reserved without executable proof: {reserved_lane}")
    for lane in lanes:
        anchors = _as_list(lane.get("evidence_anchors"))
        if lane.get("public_state") == "supported" and not anchors:
            failures.append(f"supported interop lane is missing evidence anchors: {lane.get('language')}")

    type_policy = _as_dict(surface.get("foreign_type_contract_policy"))
    for field in ("symbol_owner_required", "abi_alignment_required", "ownership_policy_required"):
        if type_policy.get(field) is not True:
            failures.append(f"foreign type contract policy must require {field}")

    unsupported_policy = _as_dict(surface.get("unsupported_lane_policy"))
    if unsupported_policy.get("swift_abi_import") != "reserved":
        failures.append("Swift ABI import must be reserved until executable proof lands")
    if unsupported_policy.get("cpp_template_instantiation_import") != "reserved":
        failures.append("C++ template instantiation import must be reserved until executable proof lands")
    if unsupported_policy.get("objc2_retired_source_syntax") != "rejected":
        failures.append("ObjC2 retired source syntax must be rejected")


def _validate_ownership_surface(surface: dict[str, Any], failures: list[str]) -> None:
    qualifier_flows = _as_list(surface.get("qualifier_flows"))
    qualifiers = {
        str(row.get("qualifier")): str(row.get("public_state"))
        for row in qualifier_flows
        if isinstance(row, dict)
    }
    for required in ("strong", "weak", "owned", "borrowed"):
        if qualifiers.get(required) != "supported":
            failures.append(f"ownership qualifier must be supported: {required}")
    if qualifiers.get("unowned") != "rejected":
        failures.append("ownership model must reject unowned until it has runtime proof")

    cleanup_paths = {str(path) for path in _as_list(surface.get("cleanup_paths"))}
    if cleanup_paths != {"normal", "error", "async", "interop"}:
        failures.append("ownership cleanup paths must cover normal, error, async, and interop")
    escape_analysis = _as_dict(surface.get("escape_analysis"))
    if any(escape_analysis.get(field) != "rejected" for field in ("borrowed_escape", "use_after_consume", "unsafe_async_crossing")):
        failures.append("ownership escape analysis must reject unsafe crossings")
    if surface.get("runtime_result_contract") != "RuntimeResultFailClosedOwnershipModel":
        failures.append("ownership model must use RuntimeResultFailClosedOwnershipModel")


def _validate_concurrency_surface(surface: dict[str, Any], failures: list[str]) -> None:
    public_effects = _as_list(surface.get("public_effects"))
    effects = {str(row.get("effect")) for row in public_effects if isinstance(row, dict)}
    for required in ("async", "await", "actor", "executor", "task_group", "cancellation", "continuation"):
        if required not in effects:
            failures.append(f"concurrency public effect missing: {required}")
    if any(row.get("requires_diagnostic") is not True for row in public_effects if isinstance(row, dict)):
        failures.append("concurrency public effects must require diagnostics")

    executor_policy = _as_dict(surface.get("executor_policy"))
    if executor_policy.get("hop_required_for_cross_executor") is not True:
        failures.append("concurrency executor policy must require cross-executor hops")
    actor_isolation = _as_dict(surface.get("actor_isolation"))
    if actor_isolation.get("sendability_boundary") is not True:
        failures.append("concurrency actor isolation must require sendability boundary")
    if actor_isolation.get("mailbox_runtime_anchor") != "actor_mailbox_enqueue":
        failures.append("concurrency actor isolation must anchor to actor_mailbox_enqueue")
    task_group_policy = _as_dict(surface.get("task_group_policy"))
    if task_group_policy.get("cancellation_propagates") is not True:
        failures.append("task group cancellation must propagate")
    if task_group_policy.get("wait_next_hops_executor") is not True:
        failures.append("task group wait_next must hop executors")
    continuation_policy = _as_dict(surface.get("continuation_policy"))
    if continuation_policy.get("ownership_handoff") != "uses-ownership-memory-model":
        failures.append("continuation policy must consume the ownership memory model")


def _validate_surfaces(contract: dict[str, Any], failures: list[str]) -> dict[str, Any]:
    surfaces = _as_dict(contract.get("typed_model_surfaces"))
    surface_names = set(surfaces)
    if not REQUIRED_SURFACES.issubset(surface_names):
        failures.append("language semantics public model surfaces are incomplete")

    _validate_generic_surface(_as_dict(surfaces.get("generic_type_system")), failures)
    _validate_module_surface(_as_dict(surfaces.get("module_identity_visibility_model")), failures)
    _validate_protocol_surface(_as_dict(surfaces.get("protocol_existential_witness_model")), failures)
    _validate_interop_surface(_as_dict(surfaces.get("interop_bridge_model")), failures)
    _validate_ownership_surface(_as_dict(surfaces.get("ownership_memory_model")), failures)
    _validate_concurrency_surface(_as_dict(surfaces.get("concurrency_usability_model")), failures)

    return {
        "surface_names": sorted(surface_names),
        "generic_type_parameter_count": len(_as_list(_as_dict(surfaces.get("generic_type_system")).get("type_parameters"))),
        "ownership_qualifier_count": len(_as_list(_as_dict(surfaces.get("ownership_memory_model")).get("qualifier_flows"))),
        "concurrency_effect_count": len(_as_list(_as_dict(surfaces.get("concurrency_usability_model")).get("public_effects"))),
    }


def _validate_handoffs(contract: dict[str, Any], failures: list[str]) -> list[dict[str, Any]]:
    handoffs = [handoff for handoff in _as_list(contract.get("cross_issue_handoffs")) if isinstance(handoff, dict)]
    pairs = {(int(handoff.get("from_issue", 0)), int(handoff.get("to_issue", 0))) for handoff in handoffs}
    required_pairs = {
        (8160, 8164),
        (8164, 8160),
        (8163, 8165),
        (8165, 8163),
        (8166, 8167),
        (8167, 8166),
    }
    if not required_pairs.issubset(pairs):
        failures.append("cross-issue handoffs must link generics/protocols, modules/interop, and ownership/concurrency both directions")
    if any(not str(handoff.get("semantic_contract", "")) for handoff in handoffs):
        failures.append("cross-issue handoffs must state semantic contracts")
    return handoffs


def _validate_unsupported_policy(contract: dict[str, Any], failures: list[str]) -> dict[str, Any]:
    unsupported_policy = _as_dict(contract.get("unsupported_policy"))
    for field in FALSE_UNSUPPORTED_POLICY_FIELDS:
        if unsupported_policy.get(field) is not False:
            failures.append(f"unsupported policy must keep {field}=false")
    return unsupported_policy


def validate_language_semantics_public_model(
    contract_path: Path = CONTRACT_PATH,
) -> LanguageSemanticsPublicModelValidationResult:
    failures: list[str] = []
    schema = require_json_object(SCHEMA_PATH)
    contract = require_json_object(contract_path)

    try:
        validate_json_schema(contract, schema, label=repo_rel(contract_path))
    except JsonSchemaValidationError as exc:
        failures.append(str(exc))

    if contract.get("contract_id") != CONTRACT_ID:
        failures.append("language semantics public model contract_id drifted")

    issue_mapping = _validate_issue_mapping(contract, failures)
    source_truth = _validate_source_truth(contract, failures)
    surfaces = _validate_surfaces(contract, failures)
    handoffs = _validate_handoffs(contract, failures)
    unsupported_policy = _validate_unsupported_policy(contract, failures)

    payload: dict[str, Any] = {
        "contract_id": "objc3c.language.semantics.public-model.validation.v1",
        "status": "PASS" if not failures else "FAIL",
        "contract_path": repo_rel(contract_path),
        "schema_path": repo_rel(SCHEMA_PATH),
        "model_header_path": repo_rel(MODEL_HEADER_PATH),
        "issue_mapping": issue_mapping,
        "source_truth": source_truth,
        "surfaces": surfaces,
        "handoff_count": len(handoffs),
        "unsupported_policy": unsupported_policy,
        "failures": failures,
    }
    return LanguageSemanticsPublicModelValidationResult(payload=payload, failures=failures)


__all__ = [
    "CONTRACT_ID",
    "CONTRACT_PATH",
    "MODEL_HEADER_PATH",
    "REQUIRED_ISSUES",
    "REQUIRED_SUPPORT_CLAIMS",
    "REQUIRED_SURFACES",
    "SCHEMA_PATH",
    "LanguageSemanticsPublicModelValidationResult",
    "validate_language_semantics_public_model",
]
