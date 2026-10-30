"""Conservative source selection for the guaranteed Linux assurance gate."""
from __future__ import annotations

import hashlib
import json
from pathlib import Path, PurePosixPath
import re
import subprocess

SCHEMA = 'objc3c.native-assurance.plan.v1'
SHA = re.compile(r'[0-9a-f]{40}')
PROTECTED_DOCS = ('docs/contracts/', 'docs/support/', 'docs/specs/', 'docs/fixtures/')
RUNTIME = ('native/objc3c/src/runtime/', 'scripts/objc3c_toolchain_runtime/',
           'tests/clang_foundation/structured_runtime/', 'tests/clang_foundation/native_async/')
PACKAGE = ('scripts/install_objc3c_linux.sh', 'scripts/qualify_linux_archive.sh',
           'scripts/objc3c_toolchain/install_sdk.py', 'scripts/objc3c_toolchain/package.py',
           'scripts/objc3c_toolchain/linux_sdk.py', 'tests/clang_foundation/installed_sdk/')
FOUNDATION = ('toolchains/llvm/', 'scripts/objc3c_toolchain/core.py',
              'scripts/objc3c_toolchain/build.py', 'scripts/objc3c_toolchain/host.py',
              'scripts/objc3c_toolchain/instrumentation_profiles.py')


def signature(value: dict) -> str:
    return hashlib.sha256(json.dumps(value, sort_keys=True, separators=(',', ':')).encode()).hexdigest()


def sha(value: str) -> str:
    if not isinstance(value, str) or not SHA.fullmatch(value) or value == '0' * 40:
        raise ValueError('an exact nonzero Git commit SHA is required')
    return value


def prose(path: str) -> bool:
    return path in ('README.md', 'CHANGELOG.md', 'LICENSE') or (
        path.startswith('docs/') and path.endswith('.md') and not path.startswith(PROTECTED_DOCS))


def select(paths: list[str] | None, *, head: str, base: str, tested: str,
           periodic: bool = False) -> dict:
    revision = {key: sha(value) for key, value in dict(head=head, base=base, tested=tested).items()}
    if paths is not None:
        for name in paths:
            if (not isinstance(name, str) or not name or '\\' in name or
                    PurePosixPath(name).is_absolute() or '..' in PurePosixPath(name).parts):
                raise ValueError('changed paths must be repository-relative names')
        paths = sorted(set(paths))
    # An empty/unknown comparison is never proof of a prose-only change.
    docs_only = bool(paths) and all(prose(path) for path in paths) and not periodic
    reasons = ['prose-only changes outside machine-consumed documents'] if docs_only else [
        'periodic complete native surface' if periodic else
        'unknown/empty comparison requires native execution' if not paths else
        'non-prose changes require the stable cross-layer native surface']
    suites = []

    def add(name, execution, owner):
        suites.append({'id': name, 'execution': execution, 'owner': owner,
                       'host': 'linux-x86_64', 'artifacts': 'content-hashed native results and command logs'})

    if not docs_only:
        add('foundation', ['native-compile', 'native-execute', 'expected-rejection'],
            'scripts/objc3c_toolchain/build.py')
        add('runtime', ['native-compile', 'native-execute'],
            'tests/clang_foundation/native_runtime/CMakeLists.txt')
        add('frontend', ['native-compile', 'native-execute', 'expected-rejection'],
            'scripts/objc3c_toolchain/frontend_tests.py')
        add('installed', ['native-compile', 'native-execute'],
            'scripts/objc3c_toolchain/qualify_installed.py')
        # Unknown sources, compiler changes and fixture changes can affect runtime
        # code generation even when no runtime implementation file changed.
        sanitizer = periodic or not paths or any(
            path.startswith(RUNTIME + FOUNDATION + ('tests/clang_foundation/', 'stdlib/', 'native/objc3c/', 'scripts/objc3c_assurance/')) or
            path.startswith('scripts/objc3c_toolchain/qualify_') for path in paths)
        if sanitizer:
            add('sanitizers', ['sanitizer-detector-control', 'native-execute'],
                'scripts/objc3c_toolchain/qualify_structured_sanitizers.py')
        if periodic:
            add('upstream', ['native-compile', 'expected-rejection'], 'LLVM lit')
        if (not paths and not periodic) or any(path.startswith(PACKAGE) for path in paths or []):
            add('package', ['installed-package-qualification'], 'scripts/objc3c_toolchain/linux_sdk.py')
    foundation_changes = not paths or any(path.startswith(FOUNDATION) for path in paths)
    plan = {'schema': SCHEMA, 'revision': revision, 'changed_paths': paths, 'periodic': periodic,
            'classification': 'prose-only' if docs_only else 'native', 'reasons': reasons,
            'foundation_policy': 'rebuild-required' if foundation_changes else 'verify-and-incrementally-build',
            'cache_policy': 'no downloaded build products; canonical source seal and build identity required',
            'suites': suites}
    return {**plan, 'sha256': signature(plan)}


def git(repo: Path, *arguments: str) -> str:
    return subprocess.check_output(['git', '-C', str(repo), *arguments], text=True).strip()


def from_checkout(repo: Path, *, head: str, base: str, tested: str, periodic=False) -> dict:
    for value in (head, base, tested):
        sha(value)
    if git(repo, 'rev-parse', 'HEAD') != tested:
        raise ValueError('checked-out revision differs from the tested SHA')
    if git(repo, 'status', '--porcelain', '--untracked-files=no'):
        raise ValueError('tracked source changes are not represented by the tested commit')
    for value in (base, head):
        subprocess.run(['git', '-C', str(repo), 'merge-base', '--is-ancestor', value, tested], check=True)
    # --no-renames includes both deleted and added names; moving code into a
    # prose path cannot hide the removed implementation from classification.
    changed = subprocess.check_output(['git', '-C', str(repo), 'diff', '--name-only',
                                      '--no-renames', '-z', base, tested]).decode().split('\0')
    return select([name for name in changed if name], head=head, base=base, tested=tested, periodic=periodic)


def validate(plan: dict) -> None:
    expected = select(plan['changed_paths'], **plan['revision'], periodic=plan['periodic'])
    if expected != plan:
        raise ValueError('native assurance plan differs from its canonical selection')
