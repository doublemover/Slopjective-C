from __future__ import annotations

from pathlib import Path as _Path

from objc3c_library_cli_parity.subprocesses import CommandResult
from objc3c_library_cli_parity.subprocesses import format_command
from objc3c_library_cli_parity.subprocesses import run_command

# Keep this legacy module import target while loading focused helper modules from
# the sibling source_mode/ directory.
__path__ = [str(_Path(__file__).with_suffix(""))]

from objc3c_library_cli_parity.source_mode.artifacts import (  # noqa: E402
    assert_no_stale_source_mode_outputs,
)
from objc3c_library_cli_parity.source_mode.artifacts import (  # noqa: E402
    build_source_mode_artifacts,
)
from objc3c_library_cli_parity.source_mode.preparation import (  # noqa: E402
    prepare_source_mode,
)
from objc3c_library_cli_parity.source_mode.work_key import (  # noqa: E402
    default_source_mode_work_key,
)

__all__ = [
    "CommandResult",
    "assert_no_stale_source_mode_outputs",
    "build_source_mode_artifacts",
    "default_source_mode_work_key",
    "format_command",
    "prepare_source_mode",
    "run_command",
]
