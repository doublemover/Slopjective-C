"""Runtime ownership payload and compile-surface assertion facade."""

from __future__ import annotations

from pathlib import Path


_HELPER_MODULE_DIR = Path(__file__).with_suffix("")
__path__ = [str(_HELPER_MODULE_DIR)]

from .storage_reflection_runtime_ownership_payload_assertions.assertions import (  # noqa: E402
    assert_storage_ownership_runtime_payload,
)
from .storage_reflection_runtime_ownership_payload_assertions.data import (  # noqa: E402
    StorageOwnershipReflectionFacts,
)
from .storage_reflection_runtime_ownership_payload_assertions.payloads import (  # noqa: E402
    capture_storage_ownership_reflection_facts,
)


__all__ = [
    "StorageOwnershipReflectionFacts",
    "assert_storage_ownership_runtime_payload",
    "capture_storage_ownership_reflection_facts",
]

for _exported_name in __all__:
    globals()[_exported_name].__module__ = __name__

del _exported_name
