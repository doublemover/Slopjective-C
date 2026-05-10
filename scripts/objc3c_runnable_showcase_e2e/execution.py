from __future__ import annotations

from pathlib import Path

from .execution_runner import run_showcase_example
from .models import ShowcaseExecutionResult, ShowcasePackageSurface
from .tooling import find_clangxx


def run_showcase_examples(
    *,
    package_root: Path,
    artifacts_root: Path,
    surface: ShowcasePackageSurface,
) -> ShowcaseExecutionResult:
    clangxx = find_clangxx()
    return ShowcaseExecutionResult(
        clangxx=clangxx,
        examples=[
            run_showcase_example(
                package_root=package_root,
                artifacts_root=artifacts_root,
                surface=surface,
                example=example,
                clangxx=clangxx,
            )
            for example in surface.showcase_examples
        ],
    )


__all__ = [
    "run_showcase_example",
    "run_showcase_examples",
]
