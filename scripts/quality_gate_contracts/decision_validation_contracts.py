"""Row-level baseline contract validation for quality-gate decisions."""

from __future__ import annotations

from pathlib import Path

# Keep this public module as the facade while focused helpers live below it.
__path__ = [str(Path(__file__).with_suffix(""))]
if __spec__ is not None:
    __spec__.submodule_search_locations = __path__

from .decision_validation_contracts.artifact_mapping import (  # noqa: E402
    validate_ev_artifact_mapping_contract,
)
from .decision_validation_contracts.consumers import validate_consumer_contract  # noqa: E402
from .decision_validation_contracts.evidence import validate_evidence_contract  # noqa: E402
from .decision_validation_contracts.gate_results import (  # noqa: E402
    validate_gate_results_contract,
)

__all__ = [
    "validate_consumer_contract",
    "validate_ev_artifact_mapping_contract",
    "validate_evidence_contract",
    "validate_gate_results_contract",
]
