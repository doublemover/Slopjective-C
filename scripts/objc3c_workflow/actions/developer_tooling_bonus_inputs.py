"""Input loading for bonus-tool integration inspection."""

from __future__ import annotations

import json
from dataclasses import dataclass

from scripts.check_repo_superclean_surface_model import write_surface_payload

from .developer_tooling_paths import (
    REPO_SUPERCLEAN_SOURCE_OF_TRUTH,
    SHOWCASE_PORTFOLIO_JSON,
    SHOWCASE_TUTORIAL_WALKTHROUGH_JSON,
)


@dataclass(frozen=True)
class BonusSurfaceInputs:
    source_of_truth: dict[str, object]
    portfolio: dict[str, object]
    walkthrough: dict[str, object]


def ensure_bonus_source_of_truth() -> None:
    if not REPO_SUPERCLEAN_SOURCE_OF_TRUTH.is_file():
        write_surface_payload(REPO_SUPERCLEAN_SOURCE_OF_TRUTH)


def load_bonus_surface_inputs() -> BonusSurfaceInputs:
    ensure_bonus_source_of_truth()
    source_of_truth = json.loads(
        REPO_SUPERCLEAN_SOURCE_OF_TRUTH.read_text(encoding="utf-8")
    )
    portfolio = json.loads(SHOWCASE_PORTFOLIO_JSON.read_text(encoding="utf-8"))
    walkthrough = json.loads(
        SHOWCASE_TUTORIAL_WALKTHROUGH_JSON.read_text(encoding="utf-8")
    )
    return BonusSurfaceInputs(
        source_of_truth=source_of_truth,
        portfolio=portfolio,
        walkthrough=walkthrough,
    )
