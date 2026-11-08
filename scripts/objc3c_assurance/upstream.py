"""Build the exact-fork utilities required by the selected upstream lit suites."""
import os
from pathlib import Path
from .evidence import digest

UPSTREAM_TOOLS = ('split-file', 'llvm-profdata', 'llvm-lto', 'llvm-lto2',
                  'yaml2obj', 'clang-scan-deps')


def prepare_upstream_tools(repo: Path, command) -> dict:
    """Build and identify the exact-fork utilities required by selected lit suites."""
    build = repo / 'out/toolchains/objc3-development/build-linux-x86_64'
    command('upstream-tools-build', ['cmake', '--build', build, '--target',
            *UPSTREAM_TOOLS, '--parallel', '4'])
    missing = [tool for tool in UPSTREAM_TOOLS
               if not (build / 'bin' / tool).is_file() or
               not os.access(build / 'bin' / tool, os.X_OK)]
    if missing:
        raise ValueError('upstream test tools missing after build: '+', '.join(missing))
    return {tool: digest(build / 'bin' / tool) for tool in UPSTREAM_TOOLS}
