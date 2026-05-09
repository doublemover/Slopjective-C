"""Runtime acceptance case selection and execution helpers."""

from __future__ import annotations

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.case_factory_types import LabeledCaseFactories
from objc3c_runtime_acceptance.progress_state import RuntimeAcceptanceProgress
from objc3c_runtime_acceptance.suite_catalog import RUNTIME_ACCEPTANCE_SUITE_CASES


def filter_case_factories(
    case_factories: LabeledCaseFactories,
    *,
    selected_suite: str,
    selected_cases: list[str],
) -> LabeledCaseFactories:
    available = {label for label, _ in case_factories}
    requested = tuple(selected_cases) or RUNTIME_ACCEPTANCE_SUITE_CASES[selected_suite]
    if selected_suite == "full" and not selected_cases:
        return case_factories
    unknown = [label for label in requested if label not in available]
    if unknown:
        raise RuntimeError("unknown runtime acceptance case(s): " + ", ".join(unknown))
    requested_set = set(requested)
    return [
        (label, factory)
        for label, factory in case_factories
        if label in requested_set
    ]


def run_case_factories(
    case_factories: LabeledCaseFactories,
    *,
    acceptance_progress: RuntimeAcceptanceProgress,
) -> list[CaseResult]:
    results: list[CaseResult] = []
    for index, (label, factory) in enumerate(case_factories, start=1):
        case_started_at = acceptance_progress.start_case(index=index, label=label)
        try:
            result = factory()
        except BaseException as exc:
            acceptance_progress.fail_case(
                index=index,
                label=label,
                started_at=case_started_at,
                error=exc,
            )
            raise
        acceptance_progress.finish_case(
            index=index,
            label=label,
            result=result,
            started_at=case_started_at,
        )
        results.append(result)
    return results


__all__ = [
    "LabeledCaseFactories",
    "filter_case_factories",
    "run_case_factories",
]
