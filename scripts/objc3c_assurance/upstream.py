"""Build the exact-fork utilities required by the selected upstream lit suites."""

from scripts.objc3c_toolchain.linux_host import native_linux_host
import os
import sys
from pathlib import Path
from .evidence import digest, lit
from scripts.objc3c_toolchain.binary_packages import write_json

UPSTREAM_TOOLS = ('split-file', 'opt', 'llvm-profdata', 'llvm-lto', 'llvm-lto2',
                  'yaml2obj', 'clang-scan-deps')


def prepare_upstream_tools(repo: Path, command) -> dict:
    """Build and identify the exact-fork utilities required by selected lit suites."""
    build = repo / ('out/toolchains/objc3-development/build-' + native_linux_host())
    command('upstream-tools-build', ['cmake', '--build', build, '--target',
            *UPSTREAM_TOOLS, '--parallel', '4'])
    missing = [tool for tool in UPSTREAM_TOOLS
               if not (build / 'bin' / tool).is_file() or
               not os.access(build / 'bin' / tool, os.X_OK)]
    if missing:
        raise ValueError('upstream test tools missing after build: '+', '.join(missing))
    return {tool: digest(build / 'bin' / tool) for tool in UPSTREAM_TOOLS}


# Existing Clang lit taxonomy, confined to touched compiler subsystems. Whole
# check-clang/check-llvm promotion awaits measured runner cost, not a new cron.
UPSTREAM = ('clang/test/AST', 'clang/test/SemaObjC', 'clang/test/SemaObjCXX',
            'clang/test/CodeGenObjC', 'clang/test/CodeGenObjCXX', 'clang/test/PCH',
            'clang/test/Modules', 'clang/test/Driver')


def execute_upstream(repo: Path, directory: Path, command) -> list[dict]:
    """Publish a passing selection only after validated native lit execution."""
    foundation = repo / 'out/toolchains/objc3-development'
    selection = {'status': 'RUNNING', 'selected': list(UPSTREAM),
        'reason': 'unchanged-language regressions across AST/Sema/CodeGen/serialization/driver',
        'excluded': {'check-clang/check-llvm':
            'whole-suite promotion requires measured runner cost; #8283'}}
    receipt = directory / 'selection.json'
    write_json(receipt, selection)
    try:
        selection['tools'] = prepare_upstream_tools(repo, command)
        write_json(receipt, selection)
        result_file = directory / 'lit.json'
        command('upstream-lit', [sys.executable, '-B',
            foundation / ('build-' + native_linux_host() + '/bin/llvm-lit'), '-sv',
            '--show-unsupported', '-j', '2', '--output', result_file,
            *(foundation / 'src' / name for name in UPSTREAM)])
        results = lit(result_file)
    except BaseException:
        write_json(receipt, {**selection, 'status': 'FAIL'})
        raise
    write_json(receipt, {**selection, 'status': 'PASS'})
    return results
