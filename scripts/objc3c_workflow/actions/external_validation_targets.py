"""External validation target definitions."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

from ..environment import ROOT
from .external_validation_owner_contracts import (
    EXTERNAL_VALIDATION_OWNER_CONTRACT,
    EXTERNAL_VALIDATION_OWNER_CONTRACT_ID,
    require_external_validation_action,
)

EXTERNAL_VALIDATION_SURFACE_PY = (
    ROOT / "scripts" / "check_external_validation_source_surface.py"
)
EXTERNAL_VALIDATION_REPLAY_PY = (
    ROOT / "scripts" / "run_objc3c_external_validation_replay.py"
)
EXTERNAL_VALIDATION_PUBLICATION_PY = (
    ROOT / "scripts" / "publish_objc3c_external_repro_corpus.py"
)
EXTERNAL_VALIDATION_CLAIM_GATE_PY = (
    ROOT / "scripts" / "check_objc3c_external_support_claim_gate.py"
)
EXTERNAL_VALIDATION_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_external_validation_integration.py"
)


@dataclass(frozen=True)
class ExternalValidationTarget:
    action_name: str
    script: Path
    owner_contract_id: str
    artifact_report: str
    capability_truth_source: str


EXTERNAL_VALIDATION_TARGETS: dict[str, ExternalValidationTarget] = {
    "check-external-validation-surface": ExternalValidationTarget(
        "check-external-validation-surface",
        EXTERNAL_VALIDATION_SURFACE_PY,
        EXTERNAL_VALIDATION_OWNER_CONTRACT_ID,
        "tmp/reports/external-validation/source-surface-summary.json",
        "checked-in source surface contract",
    ),
    "test-external-validation-replay": ExternalValidationTarget(
        "test-external-validation-replay",
        EXTERNAL_VALIDATION_REPLAY_PY,
        EXTERNAL_VALIDATION_OWNER_CONTRACT_ID,
        "tmp/reports/external-validation/intake-replay-summary.json",
        "accepted intake replay proof",
    ),
    "publish-external-repro-corpus": ExternalValidationTarget(
        "publish-external-repro-corpus",
        EXTERNAL_VALIDATION_PUBLICATION_PY,
        EXTERNAL_VALIDATION_OWNER_CONTRACT_ID,
        "tmp/reports/external-validation/publication-summary.json",
        "accepted-fixture publication proof",
    ),
    "check-external-support-claim-gate": ExternalValidationTarget(
        "check-external-support-claim-gate",
        EXTERNAL_VALIDATION_CLAIM_GATE_PY,
        EXTERNAL_VALIDATION_OWNER_CONTRACT_ID,
        "tmp/reports/external-validation/support-claim-gate-summary.json",
        "support and adoption claim gate over accepted external evidence",
    ),
}

VALIDATE_EXTERNAL_VALIDATION_CHILD_ACTIONS = (
    EXTERNAL_VALIDATION_OWNER_CONTRACT.validate_child_actions
)


def external_validation_target(action_name: str) -> ExternalValidationTarget:
    require_external_validation_action(action_name)
    try:
        return EXTERNAL_VALIDATION_TARGETS[action_name]
    except KeyError as exc:
        raise ValueError(
            f"{action_name} has an owner contract but no executable target"
        ) from exc
