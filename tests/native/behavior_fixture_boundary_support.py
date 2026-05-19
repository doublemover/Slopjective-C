import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_ROOT = ROOT / "scripts"
if str(SCRIPT_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPT_ROOT))

from objc3c_tooling.behavior_fixtures import (
    PHASE_ORDER,
    FIXTURE_ROOT,
    NATIVE_ROOT,
    REQUIRED_TREE,
    RETIRED_SURFACE_TAGS,
    BehaviorFixture,
    STRICT_KINDS,
    load_behavior_fixture_catalog,
    load_manifest_fixture_entries,
)

NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
EXPECTED_BOUNDARY_BY_KIND = {
    "positive": "canonical-positive",
    "negative": "canonical-rejection",
    "rejection": "canonical-rejection",
    "strict-error": "canonical-strict-error",
}
RETIRED_POSITIVE_SURFACE_TERMS = (
    "old-mode",
    "gate",
    "retired-route",
    "compat",
    "migration-lane",
)
RETIRED_SURFACE_CONTRACT_INDEX = (
    ROOT / "tests" / "conformance" / "hard_cutover_retired_surface_fixture_contracts.json"
)
POSITIVE_RESIDUE_AUDIT = (
    ROOT / "tests" / "conformance" / "hard_cutover_positive_residue_audit.json"
)
FIXTURE_BOUNDARY_CONTRACTS = (
    ROOT / "tests" / "conformance" / "hard_cutover_fixture_boundary_contracts.json"
)
PHASE_OWNER_CONTRACTS = (
    ROOT / "tests" / "conformance" / "hard_cutover_behavior_phase_owner_contracts.json"
)
STRICT_REJECTION_NAME_SUFFIXES = (
    "_rejected.objc3",
    "_strict_error.objc3",
    "_contract.objc3",
)
EXPECTED_FIXTURE_FAMILY_INDEX = {
    "canonical_native_manifest": ("behavior_manifest", "canonical_behavior_fixtures"),
    "phase_owner_contracts": ("phase_owner_contract_index", "canonical_behavior_fixtures"),
    "retired_surface_matrix": ("retired_surface_index", "canonical_behavior_fixtures"),
    "parser_behavior": ("phase_fixture_family", "parser_lexer_ast"),
    "sema_behavior": ("phase_fixture_family", "semantic_diagnostics"),
    "lowering_behavior": ("phase_fixture_family", "lowering_and_ir"),
    "ir_behavior": ("phase_fixture_family", "lowering_and_ir"),
    "runtime_behavior": ("phase_fixture_family", "runtime_dispatch_registration"),
    "e2e_behavior": ("phase_fixture_family", "canonical_behavior_fixtures"),
    "generated_boundary": ("provenance_only_fixture_family", "pipeline_artifacts_config_json"),
    "tooling_native_execution_metadata": ("metadata_fixture_family", "runtime_dispatch_registration"),
}
HARD_CUTOVER_CONTRACTS = {
    "tests/native/parser/negative/legacy_yes_literal_alias_rejected.objc3": (
        "rejection",
        "O3C002",
    ),
    "tests/native/parser/negative/legacy_no_literal_alias_rejected.objc3": (
        "rejection",
        "O3C002",
    ),
    "tests/native/parser/negative/removed_retired_mode_flag_rejected.objc3": (
        "rejection",
        "O3C001",
    ),
    "tests/native/parser/negative/removed_parser_retired_route_flag_rejected.objc3": (
        "rejection",
        "OBJC3-E-REMOVED-RETIRED_ROUTE-FLAG",
    ),
    "tests/native/sema/errors/removed_compatibility_gate_rejected.objc3": (
        "negative",
        "OBJC3-E-REMOVED-COMPATIBILITY-GATE",
    ),
    "tests/native/sema/errors/unsupported_arc_ownership_qualifier_rejected.objc3": (
        "negative",
        "O3S221",
    ),
    "tests/native/sema/concurrency/throws_feature_claim_rejected.objc3": (
        "negative",
        "O3S221",
    ),
    "tests/native/lowering/objc_runtime/numeric_zero_receiver_requires_runtime_dispatch_strict_error.objc3": (
        "strict-error",
        "link.unresolved_symbol",
    ),
    "tests/native/lowering/errors/removed_runtime_dispatch_retired_route_flag_rejected.objc3": (
        "strict-error",
        "OBJC3-E-REMOVED-RUNTIME-RETIRED_ROUTE",
    ),
    "tests/native/ir/runtime_calls/non_nil_receiver_runtime_call_contract.objc3": (
        "strict-error",
        "link.unresolved_symbol",
    ),
    "tests/native/runtime/dispatch/nonzero_constant_receiver_dispatch_strict_error.objc3": (
        "strict-error",
        "O3RT002",
    ),
    "tests/native/e2e/negative_execution/runtime_dispatch_unknown_receiver_strict_error.objc3": (
        "strict-error",
        "O3RT002",
    ),
}


def load_json(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def canonical_manifest_entries() -> tuple[dict, list[dict]]:
    manifest = load_json(FIXTURE_ROOT / "canonical" / "manifest.json")
    entries = load_manifest_fixture_entries(FIXTURE_ROOT / "canonical" / "manifest.json")
    return manifest, entries


def generated_manifest_entries() -> tuple[dict, list[dict]]:
    manifest = load_json(FIXTURE_ROOT / "generated" / "manifest.json")
    entries = load_manifest_fixture_entries(FIXTURE_ROOT / "generated" / "manifest.json")
    return manifest, entries
