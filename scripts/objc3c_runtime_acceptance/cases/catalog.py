"""Runtime acceptance case catalog assembly."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.case_factory_types import CaseFactory
from objc3c_runtime_acceptance.case_factory_types import CaseFactoryContext

from .domain_registry import RuntimeAcceptanceDomains
from .factory_ordering import build_all_case_factories


def build_case_factories(
    domains: RuntimeAcceptanceDomains,
    *,
    clangxx: str,
    run_dir: Path,
) -> list[tuple[str, CaseFactory]]:
    return build_all_case_factories(
        CaseFactoryContext(
            domains=domains,
            clangxx=clangxx,
            run_dir=run_dir,
        )
    )


__all__ = ["build_case_factories"]
