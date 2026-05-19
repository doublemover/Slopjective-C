"""Authoritative runtime acceptance case factory ordering."""

from __future__ import annotations

from objc3c_runtime_acceptance.case_factory_block_arc import (
    build_block_arc_case_factories,
)
from objc3c_runtime_acceptance.case_factory_concurrency_errors import (
    build_concurrency_case_factories,
)
from objc3c_runtime_acceptance.case_factory_concurrency_errors import (
    build_error_case_factories,
)
from objc3c_runtime_acceptance.case_factory_foundation import (
    build_core_case_factories,
)
from objc3c_runtime_acceptance.case_factory_foundation import (
    build_cross_module_case_factories,
)
from objc3c_runtime_acceptance.case_factory_foundation import (
    build_registration_case_factories,
)
from objc3c_runtime_acceptance.case_factory_interop import (
    build_interop_packaging_case_factories,
)
from objc3c_runtime_acceptance.case_factory_metaprogramming import (
    build_metaprogramming_case_factories,
)
from objc3c_runtime_acceptance.case_factory_object_model import (
    build_object_model_case_factories,
)
from objc3c_runtime_acceptance.case_factory_release_claims import (
    build_release_claim_case_factories,
)
from objc3c_runtime_acceptance.case_factory_storage_reflection import (
    build_storage_reflection_case_factories,
)
from objc3c_runtime_acceptance.case_factory_types import CaseFactoryContext
from objc3c_runtime_acceptance.case_factory_types import LabeledCaseFactories


def build_all_case_factories(context: CaseFactoryContext) -> LabeledCaseFactories:
    interop_packaging = build_interop_packaging_case_factories(context)
    concurrency = build_concurrency_case_factories(context)
    return [
        *build_core_case_factories(context),
        *build_registration_case_factories(context),
        *build_metaprogramming_case_factories(context),
        *interop_packaging[:2],
        *build_release_claim_case_factories(context),
        *interop_packaging[2:10],
        *concurrency[:5],
        *build_error_case_factories(context),
        *concurrency[5:],
        *build_cross_module_case_factories(context),
        *interop_packaging[10:],
        *build_object_model_case_factories(context),
        *build_storage_reflection_case_factories(context),
        *build_block_arc_case_factories(context),
    ]


__all__ = ["build_all_case_factories"]
