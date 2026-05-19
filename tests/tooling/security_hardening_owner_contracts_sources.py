from __future__ import annotations

import json
from pathlib import Path

from scripts.objc3c_workflow.actions.release_governance_security_targets import (
    CHECK_SECURITY_RESPONSE_DRILL,
    CHECK_SECURITY_RUNTIME_HARDENING,
    SECURITY_HARDENING_ACTION_OWNER_CONTRACT_IDS,
    SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS,
    SECURITY_HARDENING_PUBLIC_TARGETS,
    security_hardening_domain_owner_contracts,
    security_hardening_hard_cutover_guardrails,
    security_hardening_target,
)

ROOT = Path(__file__).resolve().parents[2]
FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "security_hardening"


def load_fixture(name: str) -> dict[str, object]:
    return json.loads((FIXTURE_ROOT / name).read_text(encoding="utf-8"))


def owner_contracts_fixture() -> dict[str, object]:
    return load_fixture("owner_contracts.json")


def source_and_workflow_surface_fixtures() -> tuple[dict[str, object], dict[str, object]]:
    return load_fixture("source_surface.json"), load_fixture("workflow_surface.json")


def boundary_inventory_fixture() -> dict[str, object]:
    return load_fixture("boundary_inventory.json")


def domain_claim_owner_fixtures() -> tuple[
    dict[str, object],
    dict[str, object],
    dict[str, object],
    dict[str, object],
]:
    return (
        load_fixture("macro_package_provenance_trust_policy.json"),
        load_fixture("response_drill_contract.json"),
        load_fixture("runtime_hardening_contract.json"),
        load_fixture("installer_update_release_key_hardening_policy.json"),
    )
