from __future__ import annotations

from pathlib import Path as _Path
from typing import Any

# Keep validation.py as the public facade while focused helpers live below validation/.
__path__ = [str(_Path(__file__).with_suffix(""))]
_spec = globals().get("__spec__")
if _spec is not None:
    _spec.submodule_search_locations = __path__
del _Path, _spec

from .validation.constants import (  # noqa: E402
    EXPECTED_EXAMPLE_IDS,
    GUIDED_WALKTHROUGH_CONTRACT_ID,
    SHOWCASE_SUMMARY_CONTRACT_ID,
    WORKSPACE_CONTRACT_ID,
)
from .validation.examples import (  # noqa: E402
    known_story_capabilities,
    showcase_example_ids,
    validate_requested_capabilities,
    validate_requested_ids,
    validate_showcase_entry,
)
from .validation.guided_walkthrough import validate_guided_walkthrough_contract  # noqa: E402
from .validation.portfolio import validate_portfolio_contract  # noqa: E402
from .validation.workspace import validate_workspace_contract  # noqa: E402
