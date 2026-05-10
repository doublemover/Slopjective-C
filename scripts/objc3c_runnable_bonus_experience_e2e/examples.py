from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import require_json_object as load_json
from objc3c_tooling.probe_compile import find_clangxx

from .example_runner import materialize_and_run_example
from .models import BonusPackageSurface, ExampleRunResult


def materialize_and_run_examples(
    *,
    package_root: Path,
    artifacts_root: Path,
    surface: BonusPackageSurface,
) -> list[ExampleRunResult]:
    clangxx = find_clangxx()
    portfolio_payload = load_json(surface.showcase_portfolio)
    portfolio_examples: dict[str, Any] = {
        str(entry.get("id")): entry
        for entry in portfolio_payload.get("examples", [])
        if isinstance(entry, dict)
    }

    return [
        materialize_and_run_example(
            package_root=package_root,
            artifacts_root=artifacts_root,
            surface=surface,
            example=example,
            portfolio_examples=portfolio_examples,
            clangxx=clangxx,
        )
        for example in surface.showcase_examples
    ]


__all__ = ("materialize_and_run_examples",)
