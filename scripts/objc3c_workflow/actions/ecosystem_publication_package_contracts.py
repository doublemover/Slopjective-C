"""Package-owned publication action command contracts."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
import sys

from scripts.objc3c_workflow.action_catalog_package_integration_claims import (
    PACKAGE_INTEGRATION_PUBLIC_ACTIONS,
)
from scripts.objc3c_workflow.action_catalog_package_lock_contracts import (
    PACKAGE_LOCK_PUBLIC_ACTIONS,
)
from scripts.objc3c_workflow.action_catalog_package_registry_publication import (
    PACKAGE_REGISTRY_PUBLIC_ACTIONS,
)
from scripts.objc3c_workflow.environment import ROOT


@dataclass(frozen=True)
class PackagePublicationActionContract:
    action_name: str
    script: Path

    def command(self) -> list[str]:
        return [sys.executable, str(self.script)]


def _contract_from_action(
    action_name: str,
    script_path: str,
) -> PackagePublicationActionContract:
    return PackagePublicationActionContract(action_name, ROOT / script_path)


PACKAGE_PUBLICATION_ACTION_CONTRACTS = {
    action.action: _contract_from_action(action.action, action.script_path)
    for action in (
        *PACKAGE_LOCK_PUBLIC_ACTIONS,
        *PACKAGE_REGISTRY_PUBLIC_ACTIONS,
        *PACKAGE_INTEGRATION_PUBLIC_ACTIONS,
    )
}

PACKAGE_LOCK_PY = PACKAGE_PUBLICATION_ACTION_CONTRACTS["build-package-lock"].script
PACKAGE_SIGN_PY = PACKAGE_PUBLICATION_ACTION_CONTRACTS["package-sign"].script
PACKAGE_VERIFY_PY = PACKAGE_PUBLICATION_ACTION_CONTRACTS["package-verify"].script
PACKAGE_MANAGER_MODEL_PY = PACKAGE_PUBLICATION_ACTION_CONTRACTS[
    "validate-package-manager-model"
].script
PACKAGE_AUTHORING_WORKFLOW_PY = PACKAGE_PUBLICATION_ACTION_CONTRACTS[
    "validate-package-authoring"
].script
PACKAGE_MIRROR_REPRODUCIBILITY_PY = PACKAGE_PUBLICATION_ACTION_CONTRACTS[
    "validate-package-mirror"
].script
PACKAGE_REGISTRY_MODEL_PY = PACKAGE_PUBLICATION_ACTION_CONTRACTS[
    "validate-package-registry-model"
].script
PACKAGE_ECOSYSTEM_INTEGRATION_PY = PACKAGE_PUBLICATION_ACTION_CONTRACTS[
    "validate-package-ecosystem"
].script
PACKAGE_INSTALL_DISTRIBUTION_PY = PACKAGE_PUBLICATION_ACTION_CONTRACTS[
    "validate-package-install-distribution"
].script
RUNNABLE_PACKAGE_ECOSYSTEM_E2E_PY = PACKAGE_PUBLICATION_ACTION_CONTRACTS[
    "validate-runnable-package-ecosystem"
].script


__all__ = [
    "PACKAGE_AUTHORING_WORKFLOW_PY",
    "PACKAGE_ECOSYSTEM_INTEGRATION_PY",
    "PACKAGE_LOCK_PY",
    "PACKAGE_MANAGER_MODEL_PY",
    "PACKAGE_MIRROR_REPRODUCIBILITY_PY",
    "PACKAGE_REGISTRY_MODEL_PY",
    "PACKAGE_SIGN_PY",
    "PACKAGE_INSTALL_DISTRIBUTION_PY",
    "PACKAGE_PUBLICATION_ACTION_CONTRACTS",
    "PACKAGE_VERIFY_PY",
    "RUNNABLE_PACKAGE_ECOSYSTEM_E2E_PY",
    "PackagePublicationActionContract",
]
