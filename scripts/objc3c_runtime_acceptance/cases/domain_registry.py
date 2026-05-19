"""Runtime acceptance domain module registry."""

from __future__ import annotations

from dataclasses import dataclass
from types import ModuleType


@dataclass(frozen=True)
class RuntimeAcceptanceDomains:
    release_claims: ModuleType
    block_arc: ModuleType
    compiler_artifacts: ModuleType
    concurrency: ModuleType
    errors: ModuleType
    interop_packaging: ModuleType
    metaprogramming: ModuleType
    object_model: ModuleType
    registration: ModuleType
    storage_reflection: ModuleType


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


__all__ = ["RuntimeAcceptanceDomains", "load_runtime_acceptance_domains"]
