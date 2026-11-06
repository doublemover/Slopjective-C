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
SWIFT = FOUNDATION + ('toolchains/swift/', 'tests/foreign/swift_consumer/',
    'scripts/objc3c_toolchain/swift_', 'scripts/objc3c_assurance/',
    'native/objc3c/src/clang_foundation/interface_', 'native/objc3c/src/clang_foundation/module_',
    'native/objc3c/src/clang_foundation/objc3_cli_', '.github/workflows/native-assurance.yml')
BENCHMARK = FOUNDATION + ('tests/benchmarks/', 'scripts/objc3c_toolchain/semantic_benchmarks.py',
    'scripts/objc3c_toolchain/semantic_import_benchmarks.py',
    'scripts/objc3c_toolchain/semantic_benchmark_calibration.py',
    'scripts/objc3c_toolchain/semantic_runtime_oracles.py',
    'scripts/objc3c_assurance/', 'native/objc3c/', 'stdlib/', '.github/workflows/native-assurance.yml')
SANITIZER_PRODUCTS = RUNTIME + PACKAGE + ('scripts/objc3c_toolchain/sanitizer_',
    'scripts/objc3c_toolchain/runtime_instrumentation.py',
    'scripts/objc3c_toolchain/run_linux_feature_batch.py',
    'scripts/objc3c_toolchain/run_linux_factory_batch.py',
    'scripts/qualify_linux_ownership_sanitizers.py')
RUNTIME_SCHEDULES = RUNTIME + ('scripts/objc3c_toolchain/runtime_schedule_',
    'scripts/objc3c_toolchain/runtime_schedules.py',
    'tests/clang_foundation/runtime_schedules/',
    'tests/clang_foundation/async_handle_joins/')
FUZZ = FOUNDATION + ('native/fuzz/', 'scripts/objc3c_toolchain/codec_fuzz.py',
    'native/objc3c/src/clang_foundation/compile_session.cpp',
    'native/objc3c/src/clang_foundation/frontend_api.',
    'native/objc3c/src/clang_foundation/interface_')
COVERAGE = FOUNDATION + ('scripts/objc3c_toolchain/source_coverage.py',
    'scripts/objc3c_toolchain/compiler_source_coverage.py',
    'scripts/objc3c_toolchain/coverage_controls.py', 'toolchains/llvm/source-coverage.json')


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
    complete_owners = periodic or not paths or any(path.startswith((
        'scripts/objc3c_assurance/', '.github/workflows/native-assurance.yml',
        '.github/workflows/adaptive-closeout.yml')) for path in paths)

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
        sanitizer = complete_owners or any(
            path.startswith(RUNTIME + FOUNDATION + ('tests/clang_foundation/', 'stdlib/', 'native/objc3c/', 'scripts/objc3c_assurance/')) or
            path.startswith('scripts/objc3c_toolchain/qualify_') for path in paths)
        if sanitizer:
            add('sanitizers', ['sanitizer-detector-control', 'native-execute'],
                'scripts/objc3c_toolchain/qualify_structured_sanitizers.py')
        if periodic:
            add('upstream', ['native-compile', 'expected-rejection'], 'LLVM lit')
        if complete_owners or any(path.startswith(SWIFT) for path in paths):
            add('swift-consumer', ['foreign-import', 'native-compile', 'native-execute', 'expected-rejection'],
                'scripts/objc3c_toolchain/swift_consumer.py')
        if complete_owners or any(path.startswith(BENCHMARK) for path in paths):
            add('semantic-workloads', ['native-compile', 'native-execute', 'semantic-work-count', 'advisory-timing'],
                'scripts/objc3c_toolchain/semantic_benchmarks.py')
        if (complete_owners or any(path.startswith(PACKAGE) for path in paths or []) or
                any(path.startswith(SANITIZER_PRODUCTS) for path in paths or [])):
            add('package', ['installed-package-qualification'], 'scripts/objc3c_toolchain/linux_sdk.py')
        if complete_owners or any(path.startswith(SANITIZER_PRODUCTS) for path in paths or []):
            add('sanitizer-products', ['paired-detector-controls', 'native-product-execute'],
                'scripts/objc3c_toolchain/sanitizer_products.py')
        if complete_owners or any(path.startswith(RUNTIME_SCHEDULES) for path in paths or []):
            add('runtime-schedules', ['generated-history', 'manual-replay', 'worker-replay',
                'semantic-mutant-rejection', 'asan-ubsan', 'tsan'],
                'scripts/objc3c_toolchain/runtime_schedules.py')
        if complete_owners or any(path.startswith(FUZZ) for path in paths or []):
            add('frontend-fuzz', ['exact-corpus-replay', 'coverage-guided-discovery',
                'source-branch-feedback', 'fault-minimization'],
                'scripts/objc3c_toolchain/codec_fuzz.py')
        if complete_owners or any(path.startswith(COVERAGE) for path in paths or []):
            add('source-coverage', ['first-party-source-coverage', 'patched-compiler-source-coverage',
                'reader-fault-controls'], 'scripts/objc3c_toolchain/compiler_source_coverage.py')
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


# Keep independent instrumented compiler builds out of the ordinary source job.
# Every selected suite still has exactly one owner in the terminal aggregate.
SHARDS = ('core', 'frontend-fuzz', 'source-coverage')
SUITE_SHARDS = {name: 'core' for name in (
    'foundation', 'runtime', 'frontend', 'installed', 'sanitizers', 'upstream',
    'swift-consumer', 'semantic-workloads', 'package', 'sanitizer-products',
    'runtime-schedules')}
SUITE_SHARDS.update({'frontend-fuzz': 'frontend-fuzz', 'source-coverage': 'source-coverage'})
PROFILE_BUILDS = {'sanitizer-products': 'compiler-asan-ubsan',
    'runtime-schedules': 'compiler-asan-ubsan', 'frontend-fuzz': 'compiler-fuzz-asan-ubsan',
    'source-coverage': 'compiler-source-coverage'}


def execution_shards(plan: dict) -> list[str]:
    validate(plan)
    if any(row['id'] not in SUITE_SHARDS for row in plan['suites']):
        raise ValueError('native suite has no execution shard')
    selected = {SUITE_SHARDS[row['id']] for row in plan['suites']}
    return [name for name in SHARDS if name in selected]


def selected_suites(plan: dict, shard: str | None = None) -> list[dict]:
    available = execution_shards(plan)
    if shard is None:
        return plan['suites']
    if shard not in available:
        raise ValueError('execution shard is unknown or has no selected native suites')
    return [row for row in plan['suites'] if SUITE_SHARDS[row['id']] == shard]


def foundation_receipts(root: Path, suites: list[dict]) -> list[Path]:
    """Each profile is built once, by its first selected consumer in this job."""
    result, profiles = [], set()
    for suite in suites:
        name = suite['id']
        if name == 'foundation':
            result.append(root / name / 'foundation.json')
        profile = PROFILE_BUILDS.get(name)
        if profile and profile not in profiles:
            result.append(root / name / 'foundations' / profile / 'foundation.json')
            profiles.add(profile)
    return result
