from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_workflow.action_catalog_package_ecosystem import (
    PACKAGE_ECOSYSTEM_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_package_integration import (
    PACKAGE_INTEGRATION_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_package_lock import (
    PACKAGE_LOCK_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_package_registry import (
    PACKAGE_REGISTRY_ACTION_SPECS,
)
from scripts.objc3c_workflow.actions.ecosystem_publication_adoption import (
    ADOPTION_LEGIBILITY_PUBLIC_ACTIONS,
)
from scripts.objc3c_workflow.actions.ecosystem_publication_owner_contracts import (
    ECOSYSTEM_PUBLICATION_OWNER_CONTRACTS,
    ecosystem_publication_owner_contracts_by_feed,
    require_ecosystem_publication_owner_contract,
)
from scripts.objc3c_workflow.actions.ecosystem_publication_package_contracts import (
    PACKAGE_PUBLICATION_ACTION_CONTRACTS,
)

ROOT = Path(__file__).resolve().parents[2]
WORKFLOW_ROOT = ROOT / "scripts" / "objc3c_workflow"


PACKAGE_OWNER_MODULES = (
    "action_catalog_package_lock",
    "action_catalog_package_registry",
    "action_catalog_package_integration",
)


def test_package_ecosystem_catalog_is_owner_facade() -> None:
    facade_text = (WORKFLOW_ROOT / "action_catalog_package_ecosystem.py").read_text(
        encoding="utf-8"
    )

    for module_name in PACKAGE_OWNER_MODULES:
        assert importlib.import_module(f"scripts.objc3c_workflow.{module_name}")
        assert f"from .{module_name} import" in facade_text
    assert "ActionSpec(" not in facade_text


def test_package_ecosystem_catalog_preserves_owner_aggregation() -> None:
    assert PACKAGE_ECOSYSTEM_ACTION_SPECS == {
        **PACKAGE_LOCK_ACTION_SPECS,
        **PACKAGE_REGISTRY_ACTION_SPECS,
        **PACKAGE_INTEGRATION_ACTION_SPECS,
    }
    assert "build-package-lock" in PACKAGE_ECOSYSTEM_ACTION_SPECS
    assert "validate-runnable-package-ecosystem" in PACKAGE_ECOSYSTEM_ACTION_SPECS


def test_package_ecosystem_public_actions_have_hard_cutover_owner_contracts() -> None:
    assert set(PACKAGE_PUBLICATION_ACTION_CONTRACTS) <= set(ECOSYSTEM_PUBLICATION_OWNER_CONTRACTS)

    package_owner_roles = {
        contract.owner_role
        for contract in ecosystem_publication_owner_contracts_by_feed("package-ecosystem")
    }
    assert {
        "package-ecosystem-lock-owner",
        "package-ecosystem-authoring-owner",
        "package-ecosystem-mirror-owner",
        "package-ecosystem-registry-owner",
        "package-ecosystem-runnable-owner",
    } <= package_owner_roles

    for action_name in PACKAGE_PUBLICATION_ACTION_CONTRACTS:
        contract = require_ecosystem_publication_owner_contract(action_name)
        assert contract.feed_name == "package-ecosystem"
        assert contract.source_contracts
        assert contract.evidence_contracts
        assert not contract.report_only_allowed
        assert not contract.wrapper_only_allowed
        assert not contract.source_compatibility_claim_allowed
        assert not contract.migration_fallback_claim_allowed
        assert any("hosted registry" in claim for claim in contract.forbidden_claims)
        assert "wrapper-only" in " ".join(contract.forbidden_claims)


def test_adoption_public_actions_have_truthful_owner_contracts() -> None:
    assert set(ADOPTION_LEGIBILITY_PUBLIC_ACTIONS) <= set(ECOSYSTEM_PUBLICATION_OWNER_CONTRACTS)

    for action_name in ADOPTION_LEGIBILITY_PUBLIC_ACTIONS:
        contract = require_ecosystem_publication_owner_contract(action_name)
        assert contract.feed_name == "adoption-legibility"
        assert contract.owner_role == "adoption-legibility-owner"
        assert contract.source_contracts
        assert contract.evidence_contracts
        assert not contract.report_only_allowed
        assert not contract.wrapper_only_allowed
        assert not contract.source_compatibility_claim_allowed
        assert not contract.migration_fallback_claim_allowed
        forbidden = " ".join(contract.forbidden_claims)
        assert "source compatibility" in forbidden
        assert "fallback support" in forbidden
