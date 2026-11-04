"""Execute all current Linux sanitizer product owners with one runtime profile set."""
from __future__ import annotations

import argparse
from concurrent.futures import ThreadPoolExecutor, as_completed
from functools import partial
import json
from pathlib import Path
import time

from .binary_packages import digest, write_json
from .package import verify_sdk
from .runtime_instrumentation import RuntimeProfiles
from . import qualify_native_async_sanitizers as native_async
from . import qualify_task_coroutine_sanitizers as coroutines
from . import qualify_structured_sanitizers as structured
from . import qualify_async_c_abi_sanitizers as c_bridge
from . import run_linux_feature_batch as features
from . import run_linux_factory_batch as factories
from scripts import qualify_linux_ownership_sanitizers as ownership


def execute_owners(tasks, output, report, jobs):
    if not tasks or len({name for name, _ in tasks}) != len(tasks) or jobs not in (1, 2, 3, 4):
        raise ValueError('product campaign requires unique nonempty owners and 1..4 jobs')
    def run(name, callback):
        tick = time.monotonic()
        try:
            result = callback(output / name)
            if result.get('status') not in ('PASS', 'pass'):
                raise ValueError('sanitizer product owner did not pass: '+name)
            return {'name': name, 'status': 'PASS', 'seconds': round(time.monotonic()-tick, 6)}
        except Exception as error:
            return {'name': name, 'status': 'FAIL', 'seconds': round(time.monotonic()-tick, 6), 'error': str(error)}
    with ThreadPoolExecutor(max_workers=jobs) as workers:
        futures = [workers.submit(run, name, callback) for name, callback in tasks]
        for future in as_completed(futures):
            report['owners'].append(future.result())
            write_json(output / 'qualification.json', report)
    if any(row['status'] != 'PASS' for row in report['owners']):
        raise ValueError('sanitizer product campaign has failed owners')


def qualify(repo: Path, sdk: Path, compiler_root: Path, facade: Path, resource: Path, output: Path, *, jobs=4):
    repo, sdk, compiler_root, facade, resource = (path.resolve(strict=True)
        for path in (repo, sdk, compiler_root, facade, resource))
    output = output.resolve()
    if output.exists() or not output.is_relative_to(repo / 'out'):
        raise ValueError('sanitizer product campaign requires a fresh owned output')
    # Admit the complete installed-tool surface before expensive runtime builds.
    verify_sdk(sdk)
    for name in ('clang', 'clang++', 'ld.lld', 'llvm-readobj', 'llvm-dwarfdump'):
        if not (sdk / 'bin' / name).is_file():
            raise ValueError('sanitizer product campaign requires installed tool: '+name)
    if not (sdk / 'share/objc3c/examples/ownership-projection').is_dir():
        raise ValueError('sanitizer product campaign requires installed ownership fixtures')
    if jobs not in (1, 2, 3, 4):
        raise ValueError('sanitizer product campaign requires 1..4 jobs')
    output.mkdir(parents=True)
    report = {'schema': 'objc3c.sanitizer-products-campaign.v1', 'status': 'running',
              'owners': [], 'runtime_profile_policy': 'Shared only within this immutable process; every get revalidates bytes/inputs.',
              'parallel_owners': jobs,
              'sdk_scope': json.loads((sdk / 'objc3-sdk.json').read_text()).get('purpose', 'selected sealed SDK'),
              'source_inputs': {}, 'started_monotonic': time.monotonic()}
    sources = [Path(__file__), Path(features.__file__), Path(factories.__file__),
        Path(native_async.__file__), Path(coroutines.__file__), Path(structured.__file__),
        Path(c_bridge.__file__), Path(ownership.__file__),
        *(Path(__file__).with_name(name+'.py') for name in
          ('product_evidence', 'sanitizer_policy', 'runtime_instrumentation', 'qualify_async_c_abi', 'sdk_example'))]
    report['source_inputs'] = {str(path): digest(path) for path in sources}
    profiles = RuntimeProfiles(repo, compiler_root, resource, output / 'runtime-profiles')

    try:
        # Complete all writers before sharing profile objects across readers.
        for sanitizer in ('address,undefined', 'thread'):
            for optimization in ('O0', 'O1', 'O2'):
                profiles.get(sanitizer, optimization)
        tasks = []
        for name, owner in (('structured', structured), ('native-async', native_async), ('coroutines', coroutines)):
            tasks.append((name, partial(owner.qualify, repo, compiler_root, resource, runtime_profiles=profiles)))
        tasks.append(('c-frame-bridge', partial(c_bridge.qualify, repo, sdk, resource,
            compiler=compiler_root / 'bin/clang', fixture=repo / 'tests/clang_foundation/async_c_abi',
            headers=repo / 'native/objc3c/src/runtime/public', runtime_profiles=profiles)))
        for sanitizer in ('address,undefined', 'thread'):
            for optimization in ('O0', 'O2'):
                profile = profiles.get(sanitizer, optimization)
                label = sanitizer.replace(',', '-')+'-'+optimization
                tasks.append(('features-'+label, partial(features.run_batch, sdk,
                    compiler=compiler_root / 'bin/clang', headers=repo / 'native/objc3c/src/runtime/public',
                    include_root=repo / 'stdlib/include', fixtures=repo / 'tests/clang_foundation',
                    runtime_library=profile.archive, objc_runtime=profile.objc_runtime,
                    sanitizer=sanitizer, resource_root=resource, optimizations=(optimization,))))
                tasks.append(('factories-'+label, partial(factories.qualify, sdk, facade, compiler_root,
                    profile.archive, profile.objc_runtime,
                    sanitizer=sanitizer, resource_root=resource, optimizations=(optimization,))))
        tasks.append(('installed-ownership', partial(ownership.qualify, sdk, resource)))
        report['expected_owners'] = [name for name, _ in tasks]
        execute_owners(tasks, output, report, jobs)
        if any(digest(Path(path)) != value for path, value in report['source_inputs'].items()):
            raise ValueError('sanitizer product campaign owner changed during execution')
        report.update(status='PASS', runtime_profiles=profiles.receipts(),
            seconds=round(time.monotonic()-report['started_monotonic'], 6))
    except BaseException as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        write_json(output / 'qualification.json', report)
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    for name in ('sdk', 'compiler-root', 'facade', 'resource', 'output'):
        parser.add_argument('--'+name, type=Path, required=True)
    parser.add_argument('--jobs', type=int, choices=range(1, 5), default=4)
    args = parser.parse_args()
    result = qualify(args.repo, args.sdk, args.compiler_root, args.facade, args.resource, args.output, jobs=args.jobs)
    print(json.dumps({'status': result['status'], 'owners': len(result['owners'])}))


if __name__ == '__main__':
    main()
