"""Qualify the isolated compiler implementation, independently of client checks."""
from __future__ import annotations

import argparse
from datetime import datetime, timezone
import json
import os
from pathlib import Path
import shlex
import subprocess
import time

from .binary_packages import digest, write_json
from .core import build_plans
from .frontend_tests import qualify as frontend_tests
from .sanitizer_policy import SanitizerQualification, require_clean_diagnostic

PROFILE = 'compiler-asan-ubsan'
# Assembly has no C/C++ sanitizer instrumentation; these exact upstream kernels
# are still linked and hashed. Do not widen this to arbitrary uninstrumented TUs.
ASSEMBLY = {f'blake3_{name}_x86-64_unix.S' for name in ('sse2', 'sse41', 'avx2', 'avx512')}


def compile_record(row: dict, build: Path, source: Path, *, foundation: bool) -> dict:
    argv = row.get('arguments') or shlex.split(row['command'])
    directory = Path(row['directory']).resolve(strict=True)
    unit = Path(row['file']).resolve(strict=True)
    output = Path(row.get('output') or argv[argv.index('-o') + 1])
    output = (directory / output).resolve(strict=True)
    if not output.is_relative_to(build) or not (unit.is_relative_to(source) or unit.is_relative_to(build)):
        raise ValueError('compiler instrumentation source/object escaped its declared graph')
    record = {'source': str(unit), 'object': str(output), 'argv': argv,
              'sha256': digest(output), 'source_sha256': digest(unit)}
    if unit.suffix == '.S':
        if not foundation or unit.name not in ASSEMBLY or unit.parent.name != 'BLAKE3':
            raise ValueError('unapproved compiler assembly exclusion')
        return {**record, 'classification': 'upstream-assembly-kernel'}
    enabled = {value for arg in argv if arg.startswith('-fsanitize=')
               for value in arg.split('=', 1)[1].split(',')}
    disabled = {value for arg in argv if arg.startswith('-fno-sanitize=')
                for value in arg.split('=', 1)[1].split(',')}
    if (not {'address', 'undefined'}.issubset(enabled) or
            disabled - {'function', 'vptr'} or '-fno-sanitize-recover=undefined' not in argv or
            any(arg.startswith('-fsanitize-recover') for arg in argv) or
            '-fno-omit-frame-pointer' not in argv):
        raise ValueError('compiler object lacks owned nonrecovering instrumentation: '+str(unit))
    if '-gline-tables-only' not in argv and '-g' not in argv:
        raise ValueError('compiler instrumentation lacks source locations')
    for arg in argv:
        if arg.startswith(('-fsanitize-blacklist=', '-fsanitize-ignorelist=')):
            approved = source / 'llvm/utils/sanitizers/ubsan_ignorelist.txt'
            if not foundation or Path(arg.split('=', 1)[1]).resolve(strict=True) != approved:
                raise ValueError('unapproved compiler instrumentation ignorelist')
    return {**record, 'classification': 'precompiled-header' if output.suffix == '.pch' else 'instrumented-object'}


def _without_dependency_flags(argv):
    result = []
    values = iter(argv)
    for arg in values:
        if arg in ('-MD', '-MMD', '-MP'):
            continue
        if arg in ('-MT', '-MQ', '-MF'):
            next(values)
            continue
        result.append(arg)
    return result


def graph_records(build: Path, source: Path, targets: list[str], ninja: Path, environment: dict,
                  *, foundation: bool, run) -> list[dict]:
    """Intersect the actual transitive Ninja graph with compilation ownership.

    This includes build helpers used to generate compiler inputs. It cannot
    silently shrink to whichever object files happen to exist on disk.
    """
    text = run('graph-commands', [ninja, '-C', build, '-t', 'commands', *targets])
    graph = {}
    for line in text.splitlines():
        argv = shlex.split(line)
        if '-c' not in argv or '-o' not in argv:
            continue
        output = (build / argv[argv.index('-o') + 1]).resolve()
        if output in graph:
            raise ValueError('ambiguous compiler graph output')
        graph[output] = argv
    if not graph:
        raise ValueError('compiler graph has no compilation work')
    commands = json.loads((build / 'compile_commands.json').read_text())
    records = {}
    for row in commands:
        argv = row.get('arguments') or shlex.split(row['command'])
        output = (Path(row['directory']) / (row.get('output') or argv[argv.index('-o') + 1])).resolve()
        if output not in graph:
            continue
        actual = _without_dependency_flags(graph[output])
        argv = _without_dependency_flags(argv)
        if (output in records or
                (build / actual[actual.index('-c') + 1]).resolve() != Path(row['file']).resolve() or
                not any(actual[index:index+len(argv)] == argv
                        for index in range(len(actual)-len(argv)+1))):
            raise ValueError('compiler graph differs from compilation ownership')
        records[output] = compile_record(row, build, source, foundation=foundation)
    custom = build / 'custom-compile-records.json'
    if custom.exists():
        if foundation:
            raise ValueError('foundation cannot import facade custom compilation records')
        for row in json.loads(custom.read_text()):
            output = (build / row['output']).resolve(strict=True)
            if output not in graph:
                continue
            actual = graph[output]
            if output in records or (build / actual[actual.index('-c') + 1]).resolve() != Path(row['file']).resolve():
                raise ValueError('custom compiler object differs from its canonical CMake source')
            records[output] = compile_record({**row, 'directory': str(build), 'arguments': actual},
                                             build, source, foundation=False)
    if set(records) != set(graph):
        raise ValueError('compiler graph is missing compilation records')
    return list(records.values())


def verify_symbols(records: list[dict], nm: Path, *, run):
    objects = {row['object']: row for row in records if row['classification'] == 'instrumented-object'}
    if not objects:
        raise ValueError('no compiler implementation objects were verified')
    names = list(objects)
    for start in range(0, len(names), 100):
        batch = names[start:start+100]
        symbols = run('object-symbols-'+str(start), [nm, '-A', '--undefined-only', *batch])
        found = {path: set() for path in batch}
        for line in symbols.splitlines():
            path, separator, value = line.partition(':')
            if separator and path in found and ('__asan_' in value or '__ubsan_' in value):
                found[path].add(value.split()[-1])
        for path, values in found.items():
            if not any('__asan_' in value for value in values):
                raise ValueError('compiler object has no ASan symbol evidence: '+path)
            objects[path]['instrumentation_symbols'] = sorted(values)
    if not any('__ubsan_' in symbol for row in objects.values() for symbol in row['instrumentation_symbols']):
        raise ValueError('compiler graph has no UBSan symbol evidence')


def borrowed_base_pch_matrix(compiler: Path, source: Path, output: Path, *, run):
    """Reuse the authored mandatory-diagnostic oracle across both PCH standards."""
    fixture = source / 'clang/test/ObjC3/Sema/borrowed-base-subobjects.mm'
    header = fixture.parent / 'Inputs/borrowed-base-subobjects.h'
    for standard in ('c++20', 'c++23'):
        for arc in (False, True):
            name = 'borrowed-base-' + standard + ('-arc' if arc else '-mrc')
            pch = output / (name+'.pch')
            flags = ['-cc1', '-std='+standard, '-fblocks', '-fobjc-version=3',
                     '-fobjc-runtime=gnustep-2.2', '-fobjc3-profile=strict-system',
                     *(['-fobjc-arc'] if arc else [])]
            run(name+'-producer', [compiler, *flags, '-x', 'objective-c++-header',
                                   '-emit-pch', header, '-o', pch])
            run(name+'-consumer', [compiler, *flags, '-x', 'objective-c++',
                '-include-pch', pch, '-DFROM_PCH', '-fsyntax-only', '-verify', fixture])
            pch.unlink()


def qualify(repo: Path, output: Path, *, jobs=2) -> dict:
    if jobs < 1:
        raise ValueError('compiler qualification requires positive build parallelism')
    repo = repo.resolve(strict=True)
    output = output.resolve()
    if output.exists():
        raise ValueError('compiler instrumentation requires a fresh evidence directory')
    plans = build_plans(repo, host='linux-x86_64', instrumentation_profile=PROFILE)
    plan = plans['configure']
    build, stage, source = (Path(plan['paths'][key]).resolve(strict=True) for key in ('build', 'stage', 'source'))
    # Refuse output locations that can overwrite inputs or the ordinary SDK.
    if output == repo or any(output.is_relative_to(path) or path.is_relative_to(output)
                             for path in (build, stage, source, repo / 'native', repo / 'toolchains')):
        raise ValueError('compiler evidence must be disjoint from implementation inputs')
    foundation_report = build / 'foundation-build-report.json'
    completed = json.loads(foundation_report.read_text())
    if (completed.get('status') != 'passed' or completed.get('source_identity') != plan['source_build_id'] or
            {phase['phase'] for phase in completed['phases']} != {'configure', 'build', 'test', 'stage'} or
            any(phase['exit_code'] for phase in completed['phases'])):
        raise ValueError('compiler instrumentation requires a current complete foundation build/test/stage')
    bootstrap = Path(plan['host_environment']['bootstrap_root'])
    libraries = [stage / 'lib/libclang-cpp.so', stage / 'lib/libLLVM.so']
    policy = SanitizerQualification(bootstrap / 'bin/clang++', bootstrap / 'lib/clang/23',
        output / 'detectors', 'address,undefined', libraries=libraries,
        additional_inputs=[foundation_report, Path(__file__),
            *(repo / name for name in plan['configuration_inputs']),
            *(path for tree in ('native/objc3c', 'tests/clang_foundation', 'tests/tooling/fixtures/clang_foundation')
              for path in (repo / tree).rglob('*')
              if path.is_file() and (path.suffix in ('.cpp', '.h', '.hpp', '.cmake', '.in') or path.name == 'CMakeLists.txt'))])
    environment = dict(policy.environment)
    for key in ('cmake_path', 'ninja_path'):
        environment['PATH'] = str(Path(plan['host_environment'][key]).parent)+os.pathsep+environment['PATH']
    output.mkdir(parents=True)
    receipt = output / 'qualification.json'
    report = {'schema': 'objc3c.compiler-implementation-instrumentation.v1', 'status': 'running',
              'subject': 'compiler-implementation', 'profile': PROFILE,
              'source_build_id': plan['source_build_id'], 'build_config_digest': plan['build_config_digest'],
              'foundation_receipt_sha256': digest(foundation_report),
              'boundaries': plan['instrumentation']['boundaries'],
              'compile_time_exclusions': {
                  'upstream_ubsan_ignorelist': {
                      'path': str(source / 'llvm/utils/sanitizers/ubsan_ignorelist.txt'),
                      'sha256': digest(source / 'llvm/utils/sanitizers/ubsan_ignorelist.txt'),
                      'owner': 'LLVM upstream sanitizer configuration',
                      'scope': 'libstdc++ tree/iterator/vector/shared-pointer header operations',
                      'references': ['GCC #59829', 'GCC #85930', 'LLVM HandleLLVMOptions.cmake'],
                      'revalidate': 'Every upstream/compiler/standard-library upgrade; #8284'},
                  'function_and_vptr': {
                      'owner': 'LLVM upstream Address;Undefined recipe',
                      'reason': 'Erased callback and native type representations use the supported LLVM profile.',
                      'revalidate': 'Every upstream/compiler upgrade; #8284'},
                  'assembly': {
                      'owner': 'LLVM upstream BLAKE3 kernels',
                      'reason': 'Four named assembly kernels have no C/C++ sanitizer instrumentation.',
                      'revalidate': 'Every source/toolchain upgrade; #8284'}},
              'commands': [], 'foundation_objects': [], 'embedding_objects': []}

    def run(name, command, *, expected=0):
        log = output / (name+'.log')
        started = time.monotonic()
        argv = list(map(str, command))
        try:
            with log.open('w') as stream:
                result = subprocess.run(argv, cwd=repo, env=environment,
                    stdout=stream, stderr=subprocess.STDOUT, timeout=900)
            code = result.returncode
        except subprocess.TimeoutExpired:
            code = 124
        report['commands'].append({'name': name, 'argv': argv, 'exit_code': code,
            'seconds': round(time.monotonic()-started, 6), 'log': str(log), 'log_sha256': digest(log)})
        write_json(receipt, report)
        diagnostic = log.read_text(errors='replace')
        if code != expected:
            raise ValueError('compiler instrumentation command failed: '+name)
        require_clean_diagnostic(diagnostic)
        return diagnostic

    try:
        policy.ensure_controls('O1')
        borrowed_base_pch_matrix(stage / 'bin/clang', source, output, run=run)
        ninja = Path(plan['host_environment']['ninja_path'])
        targets = [target for group in plan['instrumentation']['build_target_groups'] for target in group['targets']]
        report['foundation_objects'] = graph_records(build, source, targets, ninja, environment,
            foundation=True, run=lambda name, cmd: run('foundation-'+name, cmd))
        verify_symbols(report['foundation_objects'], bootstrap / 'bin/llvm-nm',
            run=lambda name, cmd: run('foundation-'+name, cmd))
        facade = output / 'facade'
        run('embedding-configure', ['cmake', '-S', repo / 'native/objc3c', '-B', facade, '-G', 'Ninja',
            '-DCMAKE_CXX_COMPILER='+str(bootstrap / 'bin/clang++'), '-DCMAKE_BUILD_TYPE=Release',
            '-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld', '-DCMAKE_SHARED_LINKER_FLAGS=-fuse-ld=lld',
            '-DOBJC3C_CLANG_FOUNDATION_ROOT='+str(stage),
            '-DOBJC3C_COMPILER_INSTRUMENTATION_PROFILE='+PROFILE])
        embedding_tools = ['objc3c', 'objc3c_frontend_v2', 'clang-scan-deps']
        run('embedding-tools-build', ['cmake', '--build', facade, '--target', *embedding_tools,
                                     '--parallel', str(jobs)])
        report['frontends'] = frontend_tests(facade, output / 'frontends', run, jobs=jobs)
        # Serial qualification reclaims passing test executables, but retains
        # their object files and exact binary identities in the nested receipt.
        targets = [row['target'] for row in report['frontends']['tests']] + embedding_tools
        report['embedding_objects'] = graph_records(facade, repo, targets, ninja, environment,
            foundation=False, run=lambda name, cmd: run('embedding-'+name, cmd))
        verify_symbols(report['embedding_objects'], bootstrap / 'bin/llvm-nm',
            run=lambda name, cmd: run('embedding-'+name, cmd))
        for record in report['foundation_objects'] + report['embedding_objects']:
            if (digest(Path(record['source'])) != record['source_sha256'] or
                    digest(Path(record['object'])) != record['sha256']):
                raise ValueError('compiler graph changed during implementation qualification')
        final = build_plans(repo, host='linux-x86_64', instrumentation_profile=PROFILE)['configure']
        if (final['source_build_id'] != plan['source_build_id'] or
                final['host_environment'] != plan['host_environment']):
            raise ValueError('compiler source/profile/bootstrap changed during qualification')
        report['detectors'] = policy.finalize()
        report['loaded_compiler_libraries'] = {str(path.resolve(strict=True)): digest(path)
                                               for path in libraries}
        report['status'] = 'PASS'
    except BaseException as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        write_json(receipt, report)
    return report


def build_and_qualify(repo: Path, *, profile=PROFILE, fetch=False) -> dict:
    """Opt-in SDK pipeline prerequisite; never package an instrumented SDK."""
    if profile != PROFILE:
        raise ValueError('unknown compiler implementation instrumentation profile')
    from .build import build_foundation
    build_foundation(repo, host='linux-x86_64', fetch=fetch, stage=True,
                     instrumentation_profile=profile)
    name = datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%S.%fZ') + '-' + str(os.getpid())
    output = repo / 'out/compiler-instrumentation' / name
    report = qualify(repo, output)
    receipt = output / 'qualification.json'
    return {'profile': profile, 'subject': report['subject'], 'status': report['status'],
            'receipt': str(receipt), 'sha256': digest(receipt)}


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--jobs', type=int, default=2)
    args = parser.parse_args(argv)
    if args.jobs < 1:
        parser.error('--jobs must be positive')
    qualify(args.repo, args.output, jobs=args.jobs)
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
