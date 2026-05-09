"""External validation action specs."""

from __future__ import annotations

from .action_catalog_external_validation_publication import (
    EXTERNAL_VALIDATION_PUBLICATION_ACTION_SPECS,
)
from .action_catalog_external_validation_replay import (
    EXTERNAL_VALIDATION_REPLAY_ACTION_SPECS,
)
from .action_catalog_external_validation_source import (
    EXTERNAL_VALIDATION_SOURCE_ACTION_SPECS,
)
from .action_catalog_external_validation_workflow import (
    EXTERNAL_VALIDATION_WORKFLOW_ACTION_SPECS,
)
from .action_spec import ActionSpec

EXTERNAL_VALIDATION_ACTION_SPECS: dict[str, ActionSpec] = {
    "check-external-validation-surface": EXTERNAL_VALIDATION_SOURCE_ACTION_SPECS[
        "check-external-validation-surface"
    ],
    "test-external-validation-replay": EXTERNAL_VALIDATION_REPLAY_ACTION_SPECS[
        "test-external-validation-replay"
    ],
    "publish-external-repro-corpus": EXTERNAL_VALIDATION_PUBLICATION_ACTION_SPECS[
        "publish-external-repro-corpus"
    ],
    "validate-external-validation": EXTERNAL_VALIDATION_WORKFLOW_ACTION_SPECS[
        "validate-external-validation"
    ],
    "validate-external-validation-integration": EXTERNAL_VALIDATION_WORKFLOW_ACTION_SPECS[
        "validate-external-validation-integration"
    ],
}

__all__ = ["EXTERNAL_VALIDATION_ACTION_SPECS"]
