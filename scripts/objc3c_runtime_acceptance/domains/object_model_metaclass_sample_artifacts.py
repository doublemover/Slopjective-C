"""Compile artifact assertion facade for Object Model metaclass samples."""

from __future__ import annotations

from pathlib import Path


_HELPER_MODULE_DIR = Path(__file__).with_suffix("")
__path__ = [str(_HELPER_MODULE_DIR)]

from .object_model_metaclass_sample_artifacts.assertions import (  # noqa: E402
    assert_canonical_sample_lowering_artifacts,
)
from .object_model_metaclass_sample_artifacts.assertions import (  # noqa: E402
    assert_canonical_sample_registration_manifest,
)
from .object_model_metaclass_sample_artifacts.assertions import (  # noqa: E402
    assert_metaclass_graph_compile_artifacts,
)
from .object_model_metaclass_sample_artifacts.assertions import (  # noqa: E402
    assert_metaclass_graph_negative_diagnostics,
)
from .object_model_metaclass_sample_artifacts.compilation import (  # noqa: E402
    compile_canonical_sample_set_fixture,
)
from .object_model_metaclass_sample_artifacts.compilation import (  # noqa: E402
    compile_metaclass_graph_root_class_fixture,
)
from .object_model_metaclass_sample_artifacts.data import (  # noqa: E402
    CanonicalSampleCompileArtifacts,
)
from .object_model_metaclass_sample_artifacts.data import (  # noqa: E402
    MetaclassGraphCompileArtifacts,
)


__all__ = [
    "CanonicalSampleCompileArtifacts",
    "MetaclassGraphCompileArtifacts",
    "assert_canonical_sample_lowering_artifacts",
    "assert_canonical_sample_registration_manifest",
    "assert_metaclass_graph_compile_artifacts",
    "assert_metaclass_graph_negative_diagnostics",
    "compile_canonical_sample_set_fixture",
    "compile_metaclass_graph_root_class_fixture",
]

for _exported_name in __all__:
    globals()[_exported_name].__module__ = __name__

del _exported_name
