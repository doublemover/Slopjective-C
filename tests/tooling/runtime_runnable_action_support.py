from __future__ import annotations

from pathlib import Path

from scripts.objc3c_workflow.action_catalog_runtime_runnable_conformance import (
    RUNTIME_RUNNABLE_CONFORMANCE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_runtime_runnable_e2e import (
    RUNTIME_RUNNABLE_E2E_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_handlers_runtime_runnable_conformance import (
    RUNTIME_RUNNABLE_CONFORMANCE_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_runtime_runnable_e2e import (
    RUNTIME_RUNNABLE_E2E_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.actions import runtime_runnable_conformance
from scripts.objc3c_workflow.actions import runtime_runnable_e2e
from scripts.objc3c_workflow.actions.runtime_runnable_groups import (
    RUNTIME_CLOSURE_CLAIM_KIND,
    RUNTIME_CLOSURE_FORBIDDEN_CLAIM_SHAPES,
    RUNTIME_CLOSURE_HARD_CUTOVER_REQUIREMENTS,
    RUNTIME_CLOSURE_PUBLICATION_MODE,
    runtime_closure_forbidden_claim_contracts,
)

ROOT = Path(__file__).resolve().parents[2]
ACTION_ROOT = ROOT / "scripts" / "objc3c_workflow" / "actions"

OWNER_MODULES = (
    "runtime_runnable_block_arc",
    "runtime_runnable_bootstrap",
    "runtime_runnable_concurrency",
    "runtime_runnable_error",
    "runtime_runnable_interop",
    "runtime_runnable_metaprogramming",
    "runtime_runnable_object_model",
    "runtime_runnable_release_candidate",
    "runtime_runnable_storage_reflection",
)

OWNER_EXPORTS = {
    "runtime_runnable_block_arc": {
        "RUNNABLE_BLOCK_ARC_CONFORMANCE_ACTION",
        "RUNNABLE_BLOCK_ARC_CONFORMANCE_PY",
        "RUNNABLE_BLOCK_ARC_E2E_ACTION",
        "RUNNABLE_BLOCK_ARC_E2E_PY",
        "RUNNABLE_BLOCK_ARC_ROUTE",
        "VALIDATE_BLOCK_ARC_CONFORMANCE_ACTION",
        "VALIDATE_RUNNABLE_BLOCK_ARC_ACTION",
        "action_validate_block_arc_conformance",
        "action_validate_runnable_block_arc",
    },
    "runtime_runnable_bootstrap": {
        "RUNNABLE_BOOTSTRAP_E2E_ACTION",
        "RUNNABLE_BOOTSTRAP_E2E_PY",
        "RUNNABLE_BOOTSTRAP_ROUTE",
        "VALIDATE_RUNNABLE_BOOTSTRAP_ACTION",
        "action_validate_runnable_bootstrap",
    },
    "runtime_runnable_concurrency": {
        "RUNNABLE_CONCURRENCY_CONFORMANCE_ACTION",
        "RUNNABLE_CONCURRENCY_CONFORMANCE_PY",
        "RUNNABLE_CONCURRENCY_E2E_ACTION",
        "RUNNABLE_CONCURRENCY_E2E_PY",
        "RUNNABLE_CONCURRENCY_ROUTE",
        "VALIDATE_CONCURRENCY_CONFORMANCE_ACTION",
        "VALIDATE_RUNNABLE_CONCURRENCY_ACTION",
        "action_validate_concurrency_conformance",
        "action_validate_runnable_concurrency",
    },
    "runtime_runnable_error": {
        "RUNNABLE_ERROR_CONFORMANCE_ACTION",
        "RUNNABLE_ERROR_CONFORMANCE_PY",
        "RUNNABLE_ERROR_E2E_ACTION",
        "RUNNABLE_ERROR_E2E_PY",
        "RUNNABLE_ERROR_ROUTE",
        "VALIDATE_ERROR_CONFORMANCE_ACTION",
        "VALIDATE_RUNNABLE_ERROR_ACTION",
        "action_validate_error_conformance",
        "action_validate_runnable_error",
    },
    "runtime_runnable_interop": {
        "RUNNABLE_INTEROP_CONFORMANCE_ACTION",
        "RUNNABLE_INTEROP_CONFORMANCE_PY",
        "RUNNABLE_INTEROP_E2E_ACTION",
        "RUNNABLE_INTEROP_E2E_PY",
        "RUNNABLE_INTEROP_ROUTE",
        "VALIDATE_INTEROP_CONFORMANCE_ACTION",
        "VALIDATE_RUNNABLE_INTEROP_ACTION",
        "action_validate_interop_conformance",
        "action_validate_runnable_interop",
    },
    "runtime_runnable_metaprogramming": {
        "RUNNABLE_METAPROGRAMMING_CONFORMANCE_ACTION",
        "RUNNABLE_METAPROGRAMMING_CONFORMANCE_PY",
        "RUNNABLE_METAPROGRAMMING_E2E_ACTION",
        "RUNNABLE_METAPROGRAMMING_E2E_PY",
        "RUNNABLE_METAPROGRAMMING_ROUTE",
        "VALIDATE_METAPROGRAMMING_CONFORMANCE_ACTION",
        "VALIDATE_RUNNABLE_METAPROGRAMMING_ACTION",
        "action_validate_metaprogramming_conformance",
        "action_validate_runnable_metaprogramming",
    },
    "runtime_runnable_object_model": {
        "RUNNABLE_OBJECT_MODEL_CONFORMANCE_ACTION",
        "RUNNABLE_OBJECT_MODEL_CONFORMANCE_PY",
        "RUNNABLE_OBJECT_MODEL_E2E_ACTION",
        "RUNNABLE_OBJECT_MODEL_E2E_PY",
        "RUNNABLE_OBJECT_MODEL_ROUTE",
        "VALIDATE_OBJECT_MODEL_CONFORMANCE_ACTION",
        "VALIDATE_RUNNABLE_OBJECT_MODEL_ACTION",
        "action_validate_object_model_conformance",
        "action_validate_runnable_object_model",
    },
    "runtime_runnable_release_candidate": {
        "RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_ACTION",
        "RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_PY",
        "RUNNABLE_RELEASE_CANDIDATE_E2E_ACTION",
        "RUNNABLE_RELEASE_CANDIDATE_E2E_PY",
        "RUNNABLE_RELEASE_CANDIDATE_ROUTE",
        "VALIDATE_RELEASE_CANDIDATE_CONFORMANCE_ACTION",
        "VALIDATE_RUNNABLE_RELEASE_CANDIDATE_ACTION",
        "action_validate_release_candidate_conformance",
        "action_validate_runnable_release_candidate",
    },
    "runtime_runnable_storage_reflection": {
        "RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_ACTION",
        "RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_PY",
        "RUNNABLE_STORAGE_REFLECTION_E2E_ACTION",
        "RUNNABLE_STORAGE_REFLECTION_E2E_PY",
        "RUNNABLE_STORAGE_REFLECTION_ROUTE",
        "VALIDATE_RUNNABLE_STORAGE_REFLECTION_ACTION",
        "VALIDATE_STORAGE_REFLECTION_CONFORMANCE_ACTION",
        "action_validate_storage_reflection_conformance",
        "action_validate_runnable_storage_reflection",
    },
}

EXPECTED_E2E_ACTIONS = [
    "validate-runnable-bootstrap",
    "validate-runnable-block-arc",
    "validate-runnable-concurrency",
    "validate-runnable-object-model",
    "validate-runnable-storage-reflection",
    "validate-runnable-error",
    "validate-runnable-interop",
    "validate-runnable-metaprogramming",
    "validate-runnable-release-candidate",
]

EXPECTED_CONFORMANCE_ACTIONS = [
    "validate-block-arc-conformance",
    "validate-concurrency-conformance",
    "validate-object-model-conformance",
    "validate-storage-reflection-conformance",
    "validate-error-conformance",
    "validate-interop-conformance",
    "validate-metaprogramming-conformance",
    "validate-release-candidate-conformance",
]

RUNTIME_CLOSURE_OWNER_METADATA = {
    "validate-block-arc-conformance": (
        "tests/tooling/fixtures/block_arc_closure/owner_split_contract.json",
        "tests/tooling/fixtures/block_arc_closure/boundary_inventory.json",
        "tests/tooling/fixtures/block_arc_closure/executable_proof_abi_contract.json",
    ),
    "validate-runnable-block-arc": (
        "tests/tooling/fixtures/block_arc_closure/owner_split_contract.json",
        "tests/tooling/fixtures/block_arc_closure/boundary_inventory.json",
        "tests/tooling/fixtures/block_arc_closure/executable_proof_abi_contract.json",
    ),
    "validate-concurrency-conformance": (
        "tests/tooling/fixtures/concurrency_runtime_closure/owner_split_contract.json",
        "tests/tooling/fixtures/concurrency_runtime_closure/boundary_inventory.json",
        "tests/tooling/fixtures/concurrency_runtime_closure/executable_proof_abi_contract.json",
    ),
    "validate-runnable-concurrency": (
        "tests/tooling/fixtures/concurrency_runtime_closure/owner_split_contract.json",
        "tests/tooling/fixtures/concurrency_runtime_closure/boundary_inventory.json",
        "tests/tooling/fixtures/concurrency_runtime_closure/executable_proof_abi_contract.json",
    ),
    "validate-error-conformance": (
        "tests/tooling/fixtures/error_runtime_closure/owner_split_contract.json",
        "tests/tooling/fixtures/error_runtime_closure/boundary_inventory.json",
        "tests/tooling/fixtures/error_runtime_closure/executable_proof_abi_contract.json",
    ),
    "validate-runnable-error": (
        "tests/tooling/fixtures/error_runtime_closure/owner_split_contract.json",
        "tests/tooling/fixtures/error_runtime_closure/boundary_inventory.json",
        "tests/tooling/fixtures/error_runtime_closure/executable_proof_abi_contract.json",
    ),
}
