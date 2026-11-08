"""Select the fork-matched implementation-analysis lane from exact source changes."""
from __future__ import annotations

import hashlib
import json
from pathlib import Path, PurePosixPath
import subprocess

from scripts.objc3c_assurance.plan import sha
from scripts.objc3c_toolchain.analysis_partition import FULL_PARTITIONS

SCHEMA = 'objc3c.implementation-assurance.plan.v1'

# These owners can change either the analyzed compilation graph, the admitted
# fork/tool, the checker policy or the evidence boundary. Other product changes
# do not need to repeat this expensive compiler-implementation scan.
OWNERS = (
    '.github/workflows/implementation-analysis.yml',
    'native/analysis/',
    'native/cmake/',
    'native/objc3c/',
    'scripts/objc3c_implementation_assurance/',
    'tests/tooling/fixtures/implementation_analysis/',
    'toolchains/llvm/',
)
TOOLCHAIN_OWNERS = {
    'scripts/objc3c_toolchain/analysis_checkpoint.py',
    'scripts/objc3c_toolchain/analysis_scheduling.py',
    'scripts/objc3c_toolchain/analysis_partition.py',
    'scripts/objc3c_toolchain/binary_packages.py',
    'scripts/objc3c_toolchain/build.py',
    'scripts/objc3c_toolchain/compiler_instrumentation.py',
    'scripts/objc3c_toolchain/core.py',
    'scripts/objc3c_toolchain/host.py',
    'scripts/objc3c_toolchain/implementation_analysis.py',
    'scripts/objc3c_toolchain/instrumentation_profiles.py',
    'scripts/objc3c_toolchain/llvm_ir_execution.py',
    'scripts/objc3c_toolchain/llvm_tools.py',
    'scripts/objc3c_toolchain/sanitizer_policy.py',
    'scripts/objc3c_toolchain/sdk_example.py',
}


def signature(value: dict) -> str:
    return hashlib.sha256(json.dumps(value, sort_keys=True, separators=(',', ':')).encode()).hexdigest()


def owned(path: str) -> bool:
    return path in TOOLCHAIN_OWNERS or path.startswith(OWNERS)


def select(paths: list[str] | None, *, head: str, base: str, tested: str,
           periodic: bool = False) -> dict:
    revision = {key: sha(value) for key, value in dict(head=head, base=base, tested=tested).items()}
    if paths is not None:
        for path in paths:
            if (not isinstance(path, str) or not path or '\\' in path or
                    PurePosixPath(path).is_absolute() or '..' in PurePosixPath(path).parts):
                raise ValueError('changed paths must be repository-relative names')
        paths = sorted(set(paths))
    selected = sorted(path for path in paths or [] if owned(path))
    # Unknown and empty comparisons are deliberately complete. Scheduled and
    # manually dispatched runs are the full owned-TU lane required by #8285.
    required = periodic or not paths or bool(selected)
    analysis_paths = [] if periodic or not paths else selected
    plan = {
        'schema': SCHEMA,
        'revision': revision,
        'changed_paths': paths,
        'periodic': periodic,
        'required': required,
        'analysis_paths': analysis_paths,
        'analysis_partitions': FULL_PARTITIONS if required and not analysis_paths else 1,
        'selection': ('complete-owned-tu-inventory' if not analysis_paths and required else
                      'dependency-aware-owned-changes' if required else 'unaffected'),
        'reason': ('scheduled or manually dispatched complete analysis' if periodic else
                   'unknown/empty comparison requires complete analysis' if not paths else
                   'implementation-analysis owner changed' if required else
                   'no implementation-analysis owner changed'),
    }
    return {**plan, 'sha256': signature(plan)}


def git(repo: Path, *arguments: str) -> str:
    return subprocess.check_output(['git', '-C', str(repo), *arguments], text=True).strip()


def from_checkout(repo: Path, *, head: str, base: str, tested: str,
                  periodic: bool = False) -> dict:
    for value in (head, base, tested):
        sha(value)
    if git(repo, 'rev-parse', 'HEAD') != tested:
        raise ValueError('checked-out revision differs from the tested SHA')
    if git(repo, 'status', '--porcelain', '--untracked-files=no'):
        raise ValueError('tracked source changes are not represented by the tested commit')
    for value in (base, head):
        subprocess.run(['git', '-C', str(repo), 'merge-base', '--is-ancestor', value, tested], check=True)
    changed = subprocess.check_output(['git', '-C', str(repo), 'diff', '--name-only',
                                       '--no-renames', '-z', base, tested]).decode().split('\0')
    return select([path for path in changed if path], head=head, base=base, tested=tested,
                  periodic=periodic)


def validate(plan: dict) -> None:
    expected = select(plan['changed_paths'], **plan['revision'], periodic=plan['periodic'])
    if expected != plan:
        raise ValueError('implementation assurance plan differs from its canonical selection')
