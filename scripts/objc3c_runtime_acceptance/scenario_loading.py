"""Scenario loading and selection for runtime acceptance."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.cases import RuntimeAcceptanceDomains
from objc3c_runtime_acceptance.cases import build_case_factories
from objc3c_runtime_acceptance.cases import load_runtime_acceptance_domains
from objc3c_runtime_acceptance.case_factory_types import LabeledCaseFactories
from objc3c_runtime_acceptance.execution import filter_case_factories
from objc3c_runtime_acceptance.suite_catalog import available_suite_payload


@dataclass(frozen=True)
class RuntimeAcceptanceScenarios:
    domains: RuntimeAcceptanceDomains
    case_factories: LabeledCaseFactories

    def available_suite_payload(self) -> dict[str, Any]:
        return available_suite_payload(self.case_factories)

    def selected(
        self,
        *,
        selected_suite: str,
        selected_cases: list[str],
    ) -> LabeledCaseFactories:
        return filter_case_factories(
            self.case_factories,
            selected_suite=selected_suite,
            selected_cases=selected_cases,
        )


def load_runtime_acceptance_scenarios(
    *,
    clangxx: str,
    run_dir: Path,
) -> RuntimeAcceptanceScenarios:
    domains = load_runtime_acceptance_domains()
    return RuntimeAcceptanceScenarios(
        domains=domains,
        case_factories=build_case_factories(
            domains,
            clangxx=clangxx,
            run_dir=run_dir,
        ),
    )


__all__ = ["RuntimeAcceptanceScenarios", "load_runtime_acceptance_scenarios"]
