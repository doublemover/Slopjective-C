"""Compile and probe operations for live metaprogramming host-cache cases."""

from __future__ import annotations

import inspect
from pathlib import Path


_HELPER_MODULE_DIR = Path(__file__).with_suffix("")
__path__ = [str(_HELPER_MODULE_DIR)]

from .metaprogramming_live_cache_compilation.config import (  # noqa: E402
    HOST_CACHE_PROBE_PATH,
)
from .metaprogramming_live_cache_compilation.models import (  # noqa: E402
    LiveMetaprogrammingCacheCompile,
    LiveMetaprogrammingCacheConsumerLink,
    LiveMetaprogrammingCacheProbeRun,
    LiveMetaprogrammingCacheProvider,
)
from .metaprogramming_live_cache_compilation.orchestration import (  # noqa: E402
    compile_live_metaprogramming_cache_consumer,
    compile_live_metaprogramming_cache_replay,
    compile_live_metaprogramming_cache_tampered_replay_expect_failure,
    live_metaprogramming_cache_case_dir,
    materialize_live_metaprogramming_cache_provider,
    prepare_live_metaprogramming_cache_provider,
    run_live_metaprogramming_cache_probe,
)


__all__ = [
    "HOST_CACHE_PROBE_PATH",
    "LiveMetaprogrammingCacheCompile",
    "LiveMetaprogrammingCacheConsumerLink",
    "LiveMetaprogrammingCacheProbeRun",
    "LiveMetaprogrammingCacheProvider",
    "compile_live_metaprogramming_cache_consumer",
    "compile_live_metaprogramming_cache_replay",
    "compile_live_metaprogramming_cache_tampered_replay_expect_failure",
    "live_metaprogramming_cache_case_dir",
    "materialize_live_metaprogramming_cache_provider",
    "prepare_live_metaprogramming_cache_provider",
    "run_live_metaprogramming_cache_probe",
]

for _exported_name in __all__:
    _exported = globals()[_exported_name]
    if inspect.isclass(_exported) or inspect.isfunction(_exported):
        _exported.__module__ = __name__

del _exported
del _exported_name
