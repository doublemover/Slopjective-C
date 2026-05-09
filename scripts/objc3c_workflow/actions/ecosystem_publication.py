"""Package ecosystem and planning publication workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from ..environment import ROOT

PACKAGE_LOCK_PY = ROOT / "scripts" / "build_objc3c_package_lock.py"
PACKAGE_AUTHORING_WORKFLOW_PY = ROOT / "scripts" / "check_objc3c_package_authoring_workflow.py"
PACKAGE_MIRROR_REPRODUCIBILITY_PY = (
    ROOT / "scripts" / "check_objc3c_package_registry_mirror_reproducibility.py"
)
PACKAGE_ECOSYSTEM_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_package_ecosystem_integration.py"
)
RUNNABLE_PACKAGE_ECOSYSTEM_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_package_ecosystem_end_to_end.py"
)
LONG_HORIZON_OPERATIONS_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_long_horizon_operations_integration.py"
)
LONG_HORIZON_OPERATIONS_PUBLICATION_PY = (
    ROOT / "scripts" / "publish_objc3c_long_horizon_operations_metadata.py"
)
ADOPTION_LEGIBILITY_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_adoption_legibility_integration.py"
)
ADOPTION_LEGIBILITY_PUBLICATION_PY = (
    ROOT / "scripts" / "publish_objc3c_adoption_legibility_metadata.py"
)
GOVERNANCE_SUSTAINABILITY_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_governance_sustainability_integration.py"
)
GOVERNANCE_SUSTAINABILITY_PUBLICATION_PY = (
    ROOT / "scripts" / "publish_objc3c_governance_sustainability_metadata.py"
)
PLANNING_ISSUE_PUBLISHER_PY = ROOT / "scripts" / "publish_objc3c_planning_issues.py"
PLANNING_PUBLICATION_AUDIT_PY = ROOT / "scripts" / "audit_objc3c_planning_publication.py"


def action_build_package_lock(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGE_LOCK_PY)])


def action_validate_package_authoring(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGE_AUTHORING_WORKFLOW_PY)])


def action_validate_package_mirror(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGE_MIRROR_REPRODUCIBILITY_PY)])


def action_validate_package_ecosystem(_: list[str]) -> int:
    return run([sys.executable, str(PACKAGE_ECOSYSTEM_INTEGRATION_PY)])


def action_validate_runnable_package_ecosystem(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_PACKAGE_ECOSYSTEM_E2E_PY)])


def action_validate_long_horizon_operations(_: list[str]) -> int:
    return run([sys.executable, str(LONG_HORIZON_OPERATIONS_INTEGRATION_PY)])


def action_publish_long_horizon_operations(_: list[str]) -> int:
    return run([sys.executable, str(LONG_HORIZON_OPERATIONS_PUBLICATION_PY)])


def action_validate_adoption_legibility(_: list[str]) -> int:
    return run([sys.executable, str(ADOPTION_LEGIBILITY_INTEGRATION_PY)])


def action_publish_adoption_legibility(_: list[str]) -> int:
    return run([sys.executable, str(ADOPTION_LEGIBILITY_PUBLICATION_PY)])


def action_validate_governance_sustainability(_: list[str]) -> int:
    return run([sys.executable, str(GOVERNANCE_SUSTAINABILITY_INTEGRATION_PY)])


def action_publish_governance_sustainability(_: list[str]) -> int:
    return run([sys.executable, str(GOVERNANCE_SUSTAINABILITY_PUBLICATION_PY)])


def action_publish_planning_issues(rest: list[str]) -> int:
    return run([sys.executable, str(PLANNING_ISSUE_PUBLISHER_PY), *rest])


def action_check_planning_publication_drift(rest: list[str]) -> int:
    return run([sys.executable, str(PLANNING_PUBLICATION_AUDIT_PY), "--check", *rest])
