"""Publication artifact command contracts."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
import sys

from ..environment import ROOT

PACKAGE_LOCK_PY = ROOT / "scripts" / "build_objc3c_package_lock.py"
PACKAGE_AUTHORING_WORKFLOW_PY = (
    ROOT / "scripts" / "check_objc3c_package_authoring_workflow.py"
)
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
PLANNING_PUBLICATION_AUDIT_PY = (
    ROOT / "scripts" / "audit_objc3c_planning_publication.py"
)


@dataclass(frozen=True)
class PublicationArtifactContract:
    action_name: str
    script: Path
    fixed_args: tuple[str, ...] = ()
    pass_through_args: bool = False

    def command(self, rest: list[str] | None = None) -> list[str]:
        pass_through = tuple(rest or ()) if self.pass_through_args else ()
        return [sys.executable, str(self.script), *self.fixed_args, *pass_through]


PUBLICATION_ARTIFACT_CONTRACTS: dict[str, PublicationArtifactContract] = {
    "build-package-lock": PublicationArtifactContract(
        "build-package-lock",
        PACKAGE_LOCK_PY,
    ),
    "validate-package-authoring": PublicationArtifactContract(
        "validate-package-authoring",
        PACKAGE_AUTHORING_WORKFLOW_PY,
    ),
    "validate-package-mirror": PublicationArtifactContract(
        "validate-package-mirror",
        PACKAGE_MIRROR_REPRODUCIBILITY_PY,
    ),
    "validate-package-ecosystem": PublicationArtifactContract(
        "validate-package-ecosystem",
        PACKAGE_ECOSYSTEM_INTEGRATION_PY,
    ),
    "validate-runnable-package-ecosystem": PublicationArtifactContract(
        "validate-runnable-package-ecosystem",
        RUNNABLE_PACKAGE_ECOSYSTEM_E2E_PY,
    ),
    "validate-long-horizon-operations": PublicationArtifactContract(
        "validate-long-horizon-operations",
        LONG_HORIZON_OPERATIONS_INTEGRATION_PY,
    ),
    "publish-long-horizon-operations": PublicationArtifactContract(
        "publish-long-horizon-operations",
        LONG_HORIZON_OPERATIONS_PUBLICATION_PY,
    ),
    "validate-adoption-legibility": PublicationArtifactContract(
        "validate-adoption-legibility",
        ADOPTION_LEGIBILITY_INTEGRATION_PY,
    ),
    "publish-adoption-legibility": PublicationArtifactContract(
        "publish-adoption-legibility",
        ADOPTION_LEGIBILITY_PUBLICATION_PY,
    ),
    "validate-governance-sustainability": PublicationArtifactContract(
        "validate-governance-sustainability",
        GOVERNANCE_SUSTAINABILITY_INTEGRATION_PY,
    ),
    "publish-governance-sustainability": PublicationArtifactContract(
        "publish-governance-sustainability",
        GOVERNANCE_SUSTAINABILITY_PUBLICATION_PY,
    ),
    "publish-planning-issues": PublicationArtifactContract(
        "publish-planning-issues",
        PLANNING_ISSUE_PUBLISHER_PY,
        pass_through_args=True,
    ),
    "check-planning-publication-drift": PublicationArtifactContract(
        "check-planning-publication-drift",
        PLANNING_PUBLICATION_AUDIT_PY,
        fixed_args=("--check",),
        pass_through_args=True,
    ),
}
