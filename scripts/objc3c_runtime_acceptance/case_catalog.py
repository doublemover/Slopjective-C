"""Runtime acceptance domain loading and case catalog facade."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.case_factories import CaseFactory
from objc3c_runtime_acceptance.case_factories import CaseFactoryContext
from objc3c_runtime_acceptance.case_factories import build_all_case_factories


@dataclass(frozen=True)
class RuntimeAcceptanceDomains:
    release_claims: Any
    block_arc: Any
    compiler_artifacts: Any
    concurrency: Any
    errors: Any
    interop_packaging: Any
    metaprogramming: Any
    object_model: Any
    registration: Any
    storage_reflection: Any


def load_runtime_acceptance_domains() -> RuntimeAcceptanceDomains:
    from objc3c_runtime_acceptance.domains import block_arc
    from objc3c_runtime_acceptance.domains import compiler_artifacts
    from objc3c_runtime_acceptance.domains import concurrency
    from objc3c_runtime_acceptance.domains import errors
    from objc3c_runtime_acceptance.domains import interop_packaging
    from objc3c_runtime_acceptance.domains import metaprogramming
    from objc3c_runtime_acceptance.domains import object_model
    from objc3c_runtime_acceptance.domains import registration
    from objc3c_runtime_acceptance.domains import release_claims
    from objc3c_runtime_acceptance.domains import storage_reflection

    return RuntimeAcceptanceDomains(
        release_claims=release_claims,
        block_arc=block_arc,
        compiler_artifacts=compiler_artifacts,
        concurrency=concurrency,
        errors=errors,
        interop_packaging=interop_packaging,
        metaprogramming=metaprogramming,
        object_model=object_model,
        registration=registration,
        storage_reflection=storage_reflection,
    )


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


__all__ = [
    "RuntimeAcceptanceDomains",
    "build_case_factories",
    "load_runtime_acceptance_domains",
]
