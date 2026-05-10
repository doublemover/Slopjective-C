"""Activation preflight payload and markdown consistency validation."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any, Sequence

from objc3c_tooling.paths import display_path

from scripts.activation_preflight.contracts import CommandResult
from scripts.activation_preflight.payload_validation_constants import (
    EXIT_GATE_CLOSED,
    EXIT_GATE_OPEN,
    EXIT_RUNNER_ERROR,
    OPEN_BLOCKERS_TRIGGER_ID,
)
from scripts.activation_preflight.payload_validation_markdown import (
    bool_text,
    check_markdown_gate_consistency,
    find_markdown_table_row,
    markdown_freshness_cell,
    validate_markdown_freshness_row,
)
from scripts.activation_preflight.payload_validation_payload import (
    parse_activation_payload,
    validate_freshness_entry,
)
