"""Imported-runtime packaging post-replay probe assertions."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.domains.interop_packaging_imported_replay_startup_probe import (
    ImportedRuntimeStartupDispatchValues,
)


_HELPER_MODULE_DIR = Path(__file__).with_suffix("")
__path__ = [str(_HELPER_MODULE_DIR)]

from .interop_packaging_imported_replay_runtime_probe.assertions import (  # noqa: E402
    assert_imported_runtime_replay_probe_payload,
)


assert_imported_runtime_replay_probe_payload.__module__ = __name__


__all__ = ["assert_imported_runtime_replay_probe_payload"]
