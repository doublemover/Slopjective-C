"""Showcase, stdlib surface, and getting-started action specs."""

from __future__ import annotations

from .action_catalog_core_getting_started import CORE_GETTING_STARTED_ACTION_SPECS
from .action_catalog_core_showcase_examples import CORE_SHOWCASE_EXAMPLE_ACTION_SPECS
from .action_catalog_core_stdlib_surface import CORE_STDLIB_SURFACE_ACTION_SPECS
from .action_spec import ActionSpec

CORE_SHOWCASE_ACTION_SPECS: dict[str, ActionSpec] = {
    "check-showcase-surface": CORE_SHOWCASE_EXAMPLE_ACTION_SPECS["check-showcase-surface"],
    "check-stdlib-surface": CORE_STDLIB_SURFACE_ACTION_SPECS["check-stdlib-surface"],
    "validate-showcase-runtime": CORE_SHOWCASE_EXAMPLE_ACTION_SPECS[
        "validate-showcase-runtime"
    ],
    "validate-showcase": CORE_SHOWCASE_EXAMPLE_ACTION_SPECS["validate-showcase"],
    "validate-runnable-showcase": CORE_SHOWCASE_EXAMPLE_ACTION_SPECS[
        "validate-runnable-showcase"
    ],
    "validate-getting-started": CORE_GETTING_STARTED_ACTION_SPECS[
        "validate-getting-started"
    ],
}


__all__ = ["CORE_SHOWCASE_ACTION_SPECS"]
