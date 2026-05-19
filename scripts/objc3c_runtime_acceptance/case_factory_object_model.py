"""Object-model case factories."""

from __future__ import annotations

from objc3c_runtime_acceptance.case_factory_types import CaseFactoryContext
from objc3c_runtime_acceptance.case_factory_types import LabeledCaseFactories


def build_object_model_case_factories(
    context: CaseFactoryContext,
) -> LabeledCaseFactories:
    domains = context.domains
    clangxx = context.clangxx
    run_dir = context.run_dir
    return [
        (
            "canonical-dispatch",
            lambda: domains.object_model.check_canonical_dispatch_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "metaclass-graph-root-class",
            lambda: domains.object_model.check_metaclass_graph_root_class_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "canonical-sample-set",
            lambda: domains.object_model.check_canonical_sample_set_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "realization-lookup-reflection-runtime",
            lambda: domains.object_model.check_realization_lookup_reflection_runtime_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "live-dispatch-fast-path",
            lambda: domains.object_model.check_live_dispatch_fast_path_case(
                clangxx,
                run_dir,
            ),
        ),
    ]


__all__ = ["build_object_model_case_factories"]
