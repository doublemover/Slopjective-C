from __future__ import annotations

import importlib
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


def test_runtime_runnable_facades_delegate_to_domain_owners() -> None:
    for facade_name in ("runtime_runnable_conformance", "runtime_runnable_e2e"):
        facade_text = (ACTION_ROOT / f"{facade_name}.py").read_text(encoding="utf-8")

        for module_name in OWNER_MODULES:
            assert importlib.import_module(
                f"scripts.objc3c_workflow.actions.{module_name}"
            )
            if module_name == "runtime_runnable_bootstrap":
                expected = facade_name == "runtime_runnable_e2e"
            else:
                expected = True
            assert (f"from .{module_name} import" in facade_text) == expected
        assert "from ..environment import ROOT" not in facade_text
        assert "run_python_check" not in facade_text
        assert "def " not in facade_text


def test_runtime_runnable_facades_preserve_public_actions() -> None:
    assert (
        runtime_runnable_conformance.action_validate_block_arc_conformance
        is importlib.import_module(
            "scripts.objc3c_workflow.actions.runtime_runnable_block_arc"
        ).action_validate_block_arc_conformance
    )
    assert (
        runtime_runnable_conformance.action_validate_error_conformance
        is importlib.import_module(
            "scripts.objc3c_workflow.actions.runtime_runnable_error"
        ).action_validate_error_conformance
    )
    assert (
        runtime_runnable_e2e.action_validate_runnable_interop
        is importlib.import_module(
            "scripts.objc3c_workflow.actions.runtime_runnable_interop"
        ).action_validate_runnable_interop
    )
    assert (
        runtime_runnable_e2e.action_validate_runnable_release_candidate
        is importlib.import_module(
            "scripts.objc3c_workflow.actions.runtime_runnable_release_candidate"
        ).action_validate_runnable_release_candidate
    )


def test_runtime_runnable_modules_expose_stable_exports() -> None:
    for module_name, expected_exports in OWNER_EXPORTS.items():
        module = importlib.import_module(
            f"scripts.objc3c_workflow.actions.{module_name}"
        )
        assert expected_exports <= set(module.__all__)

    assert "RUNTIME_RUNNABLE_CONFORMANCE_ACTION_GROUPS" in (
        runtime_runnable_conformance.__all__
    )
    assert "RUNTIME_RUNNABLE_E2E_ACTION_GROUPS" in runtime_runnable_e2e.__all__


def test_runtime_runnable_child_order_is_explicit() -> None:
    assert [
        group.action for group in runtime_runnable_e2e.RUNTIME_RUNNABLE_E2E_ACTION_GROUPS
    ] == EXPECTED_E2E_ACTIONS
    assert [
        group.action
        for group in runtime_runnable_conformance.RUNTIME_RUNNABLE_CONFORMANCE_ACTION_GROUPS
    ] == EXPECTED_CONFORMANCE_ACTIONS


def test_runtime_runnable_catalogs_and_handlers_derive_from_action_groups() -> None:
    assert list(RUNTIME_RUNNABLE_E2E_ACTION_SPECS) == EXPECTED_E2E_ACTIONS
    assert list(RUNTIME_RUNNABLE_E2E_ACTION_HANDLERS) == EXPECTED_E2E_ACTIONS
    assert list(RUNTIME_RUNNABLE_CONFORMANCE_ACTION_SPECS) == (
        EXPECTED_CONFORMANCE_ACTIONS
    )
    assert list(RUNTIME_RUNNABLE_CONFORMANCE_ACTION_HANDLERS) == (
        EXPECTED_CONFORMANCE_ACTIONS
    )

    for action, spec in RUNTIME_RUNNABLE_E2E_ACTION_SPECS.items():
        assert spec.action == action
        assert spec.validation_tier == "full"
        assert spec.backend.startswith("python:scripts/check_objc3c_runnable_")
        assert RUNTIME_RUNNABLE_E2E_ACTION_HANDLERS[action]

    for action, spec in RUNTIME_RUNNABLE_CONFORMANCE_ACTION_SPECS.items():
        assert spec.action == action
        assert spec.validation_tier == "full"
        assert spec.backend.startswith("python:scripts/check_objc3c_runnable_")
        assert RUNTIME_RUNNABLE_CONFORMANCE_ACTION_HANDLERS[action]


def test_runtime_closure_runnable_actions_publish_owner_contract_metadata() -> None:
    groups = {
        group.action: group
        for group in (
            *runtime_runnable_conformance.RUNTIME_RUNNABLE_CONFORMANCE_ACTION_GROUPS,
            *runtime_runnable_e2e.RUNTIME_RUNNABLE_E2E_ACTION_GROUPS,
        )
    }

    for action, (
        owner_contract,
        boundary_inventory,
        executable_proof_contract,
    ) in RUNTIME_CLOSURE_OWNER_METADATA.items():
        group = groups[action]

        assert group.owner_contract == owner_contract
        assert group.boundary_inventory == boundary_inventory
        assert group.executable_proof_contract == executable_proof_contract
        assert group.claim_kind == RUNTIME_CLOSURE_CLAIM_KIND
        assert group.claim_publication_mode == RUNTIME_CLOSURE_PUBLICATION_MODE
        assert set(RUNTIME_CLOSURE_FORBIDDEN_CLAIM_SHAPES).issubset(
            group.forbidden_claim_shapes
        )
        assert set(RUNTIME_CLOSURE_HARD_CUTOVER_REQUIREMENTS).issubset(
            group.hard_cutover_requirements
        )


def test_runtime_closure_forbidden_claim_contracts_are_owner_typed() -> None:
    contracts = runtime_closure_forbidden_claim_contracts()
    by_shape = {contract["shape"]: contract for contract in contracts}

    assert set(by_shape) == set(RUNTIME_CLOSURE_FORBIDDEN_CLAIM_SHAPES)
    assert by_shape["fallback-runtime-behavior"]["policy_field"] == (
        "fallback_runtime_semantics_allowed"
    )
    assert by_shape["report-only-runtime-closure"]["policy_field"] == (
        "report_only_executable_proof_claims_allowed"
    )
    assert by_shape["compatibility-shim-runtime-closure"]["policy_field"] == (
        "compatibility_runtime_semantics_allowed"
    )
    assert by_shape["wrapper-only-runnable-action"]["policy_field"] == (
        "wrapper_only_runnable_actions_allowed"
    )
    assert by_shape["public-runtime-abi-widening-without-source-owner"][
        "policy_field"
    ] == "public_claims_require_executable_proof"
    assert by_shape["generated-report-only-source-truth"]["policy_field"] == (
        "generated_reports_are_source"
    )
    for contract in contracts:
        assert contract["owner"] == "runtime-closure-owner-contract"
        assert contract["failure_mode"] == "fail-closed"
