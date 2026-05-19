"""Developer-tooling action specs."""

from __future__ import annotations

from .action_catalog_spec_lint import SPEC_LINT_ACTION_SPECS
from .action_catalog_tooling_inspection import TOOLING_INSPECTION_ACTION_SPECS
from .action_catalog_tooling_materialization import (
    TOOLING_MATERIALIZATION_ACTION_SPECS,
)
from .action_catalog_tooling_parity import TOOLING_PARITY_ACTION_SPECS
from .action_catalog_tooling_validation import TOOLING_VALIDATION_ACTION_SPECS
from .action_spec import ActionSpec

TOOLING_DEVELOPER_ACTION_SPECS: dict[str, ActionSpec] = {
    **TOOLING_INSPECTION_ACTION_SPECS,
    **TOOLING_MATERIALIZATION_ACTION_SPECS,
    **TOOLING_PARITY_ACTION_SPECS,
    **TOOLING_VALIDATION_ACTION_SPECS,
    **SPEC_LINT_ACTION_SPECS,
}

__all__ = ["TOOLING_DEVELOPER_ACTION_SPECS"]
