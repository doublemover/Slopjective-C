"""Input loading for bonus-tool integration inspection."""

from __future__ import annotations

import json
from dataclasses import dataclass

from ..environment import ROOT
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


def missing_bonus_source_message() -> str:
    return (
        "missing source-of-truth artifact: "
        f"{REPO_SUPERCLEAN_SOURCE_OF_TRUTH.relative_to(ROOT).as_posix()}"
    )


def load_bonus_surface_inputs() -> BonusSurfaceInputs:
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
