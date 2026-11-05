"""Measure isolated first-party compiler/runtime source coverage with exact inputs."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import re
import shlex
import subprocess
import tempfile
import time

from .binary_packages import digest, write_json
from .compiler_instrumentation import loaded_graph, _without_dependency_flags
from .llvm_ir_execution import command
from .core import build_plans
from .llvm_tools import identity
from .sanitizer_policy import strict_environment, require_clean_diagnostic
from .sdk_example import dependencies
from scripts.objc3c_assurance.evidence import junit

LIMIT = 64 * 1024**2
FIRST_PARTY_PROFILE_LIMIT = 256 * 1024**2
COMMAND_TIMEOUT = 900
# Clang-facing façade units remain separately bounded from execution/export.
BUILD_TIMEOUT = 3600
MAX_TEST_TIMEOUT = 900


def suite_timeout(policy: dict, component: str, name: str) -> int:
    """Return a reviewed, bounded timeout for an instrumented test shard."""
    values = policy.get('test_timeouts_seconds')
    selected = {f'{owner}:{test}' for owner in ('frontend', 'runtime')
                for test in policy.get(owner+'_tests', [])}
    if (type(values) is not dict or set(values) - ({'default'} | selected) or
            type(values.get('default')) is not int or
            any(type(value) is not int or not 1 <= value <= MAX_TEST_TIMEOUT
                for value in values.values())):
        raise ValueError('source coverage policy has invalid test timeout budgets')
    return values.get(f'{component}:{name}', values['default'])


def profile_runtime(bootstrap: Path, target: str) -> tuple[Path, Path]:
    """Select the exact pinned compiler-rt profile runtime for covered clients."""
    resource = bootstrap / 'lib/clang/23'
    runtime = resource / 'lib' / target / 'libclang_rt.profile.a'
    if (not runtime.is_file() or runtime.is_symlink() or not runtime.stat().st_size or
            not runtime.resolve().is_relative_to(bootstrap.resolve())):
        raise ValueError('pinned bootstrap profile runtime is missing or escaped')
    return resource.resolve(), runtime.resolve()


def compile_inventory(build, graph, *, required_flags):
    """The actual graph, including custom object edges, owns the denominator."""
    database = json.loads((build / 'compile_commands.json').read_text())
    owned = {}
    for row in database:
        argv = row.get('arguments') or shlex.split(row['command'])
        output = (Path(row['directory']) / argv[argv.index('-o')+1]).resolve()
        owned[output] = (Path(row['file']).resolve(), _without_dependency_flags(argv))
    custom = build / 'custom-compile-records.json'
    special = {Path(row['output']).resolve(): Path(row['file']).resolve()
               for row in json.loads(custom.read_text())} if custom.exists() else {}
    result = {}
    for line in graph.splitlines():
        argv = shlex.split(line)
        if '-c' not in argv or '-o' not in argv or Path(argv[0]).name == 'clang-scan-deps':
            continue
        output = (build / argv[argv.index('-o')+1]).resolve(strict=True)
        unit = (build / argv[argv.index('-c')+1]).resolve(strict=True)
        expected = owned.get(output)
        actual = _without_dependency_flags(argv)
        if expected:
            if unit != expected[0] or not any(actual[i:i+len(expected[1])] == expected[1]
                                           for i in range(len(actual)-len(expected[1])+1)):
                raise ValueError('coverage graph differs from its actual compilation database')
        elif special.get(output) != unit:
            raise ValueError('coverage graph omitted an actual compilation owner')
        if (output in result or not output.is_relative_to(build) or
                not set(required_flags) <= set(argv) or
                any(arg.startswith(('-fno-profile-instr-generate', '-fno-coverage-mapping')) for arg in argv)):
            raise ValueError('duplicate, escaped or uninstrumented coverage object')
        result[output] = {'object': str(output), 'sha256': digest(output), 'source': str(unit),
                          'source_sha256': digest(unit), 'argv': argv}
    if not result:
        raise ValueError('empty implementation coverage graph')
    return list(result.values())


def read_export(path, *, repo, build, eligible):
    if path.stat().st_size > LIMIT:
        raise ValueError('coverage export exceeded its bounded decoder size')
    value = json.loads(path.read_text())
    if value.get('type') != 'llvm.coverage.json.export' or not value.get('data'):
        raise ValueError('missing LLVM source coverage export')
    result = {name: {'state': 'no-emitted-mapping', 'summary': None} for name in eligible}
    for data in value['data']:
        for row in data['files']:
            source = Path(row['filename'])
            if not source.is_absolute(): source = build / source
            source = source.resolve()
            if not source.is_relative_to(repo):
                continue
            name = source.relative_to(repo).as_posix()
            if name not in result:
                continue
            if result[name]['summary'] is not None:
                raise ValueError('duplicate source file in coverage export')
            summary = row['summary']
            for key in ('functions', 'regions', 'branches', 'lines'):
                item = summary[key]
                if (type(item['count']) is not int or type(item['covered']) is not int or
                        not 0 <= item['covered'] <= item['count']):
                    raise ValueError('invalid source coverage denominator/count')
            result[name] = {'state': 'executed' if summary['regions']['covered'] else 'zero-hit',
                'summary': summary, 'uncovered_branches': [branch for branch in row.get('branches', [])
                                                         if branch[4] == 0 or branch[5] == 0]}
    return result


def raw_profiles(root, *, limit=LIMIT):
    paths = sorted(root.glob('*.profraw'))
    if (type(limit) is not int or limit <= 0 or not paths or
            sum(path.stat().st_size for path in paths) > limit or
            any(path.is_symlink() or not path.is_file() or not path.stat().st_size for path in paths) or
            any(path.suffix != '.profraw' for path in root.iterdir())):
        raise ValueError('missing, escaped, empty or excessive raw coverage profiles')
    return paths


def reclaim_raw_profiles(paths):
    """Record exact inputs after indexing, then reclaim the transient profiles."""
    records = {}
    for path in paths:
        records[path.name] = {'sha256': digest(path), 'bytes': path.stat().st_size}
        path.unlink()
    return records


def binary_ids(text):
    if text.count('Binary IDs:') != 1:
        raise ValueError('coverage profile omitted its actual binary identity')
    values = [line.strip() for line in text.split('Binary IDs:', 1)[1].splitlines() if line.strip()]
    if not values or any(not re.fullmatch('[0-9a-f]{40}', value) for value in values):
        raise ValueError('coverage profile has missing or malformed binary identities')
    return set(values)


def elf_build_id(text: str) -> str:
    """Extract one canonical SHA-1 GNU build identity from an ELF note view."""
    values = re.findall(r'Build ID: ([0-9a-f]{40})', text)
    if len(values) != 1:
        raise ValueError('covered ELF lacks one build identity')
    return values[0]


def qualify(repo, output, *, jobs=2, reuse_build_receipt=None):
    repo, output = repo.resolve(strict=True), output.resolve()
    if output.exists() or not output.is_relative_to(repo / 'out') or jobs not in (1, 2, 3, 4):
        raise ValueError('source coverage requires fresh owned output and bounded jobs')
    plan = build_plans(repo, host='linux-x86_64',
                       instrumentation_profile='compiler-source-coverage')['configure']
    bootstrap, stage = Path(plan['host_environment']['bootstrap_root']), Path(plan['paths']['stage'])
    resource, runtime = profile_runtime(bootstrap, plan['host_environment']['target_triple'])
    completed = json.loads((Path(plan['paths']['build']) / 'foundation-build-report.json').read_text())
    if (completed.get('status') != 'passed' or completed.get('source_identity') != plan['source_build_id'] or
            {row['phase'] for row in completed.get('phases', [])} != {'configure', 'build', 'test', 'stage'} or
            any(row.get('exit_code') != 0 for row in completed['phases'])):
        raise ValueError('source coverage requires the complete matching compiler coverage profile')
    policy_path = repo / 'toolchains/llvm/source-coverage.json'
    policy = json.loads(policy_path.read_text())
    if policy.get('schema') != 'objc3c.native-source-coverage-policy.v1' or policy.get('host') != 'linux-x86_64':
        raise ValueError('unreviewed source coverage policy')
    output.mkdir(parents=True);(output / 'logs').mkdir()
    # Linked frontend witnesses embed large instrumented Clang archives. Keep
    # one executable resident and all ephemeral build products on /tmp.
    reused = None
    if reuse_build_receipt:
        reuse_build_receipt = reuse_build_receipt.resolve(strict=True)
        reused = json.loads(reuse_build_receipt.read_text())
        if (not reuse_build_receipt.is_relative_to(repo / 'out') or reused.get('status') != 'FAIL' or
                reused.get('schema') != 'objc3c.native-source-coverage.v1' or reused.get('policy') != policy or
                reused.get('source_build_id') != plan['source_build_id'] or not reused.get('inputs')):
            raise ValueError('coverage build reuse requires a matching failed source-owned attempt')
        for path, expected in reused['inputs'].items():
            # Runner repairs rerun every test/profile. No execution evidence is
            # reused; only compiled inputs and object products may be retained.
            if Path(path) != Path(__file__) and digest(Path(path)) != expected:
                raise ValueError('coverage compiled input changed before build reuse: '+path)
        build_root = Path(reused['temporary_build_root']).resolve(strict=True)
        if build_root.parent != Path('/tmp') or not build_root.name.startswith('objc3-source-coverage-'):
            raise ValueError('coverage build reuse escaped its temporary namespace')
    else:
        build_root = Path(tempfile.mkdtemp(prefix='objc3-source-coverage-'))
    if __import__('shutil').disk_usage(build_root).free < 1536 * 1024**2:
        raise ValueError('source coverage needs at least 1.5 GiB free temporary build space')
    started = time.monotonic()
    compiler = bootstrap / 'bin/clang++'
    env = strict_environment(compiler, bootstrap / 'bin/llvm-symbolizer',
                             library_dirs=(stage / 'lib',))
    env['LLVM_PROFILE_FILE'] = os.devnull
    env['PATH'] = os.pathsep.join([str(Path(plan['host_environment'][key]).parent)
        for key in ('cmake_path', 'ninja_path')]+[env['PATH']])
    env['OBJC3C_BOOTSTRAP_LLVM_ROOT'] = str(bootstrap)
    tools = {name: stage / 'bin' / name for name in ('llvm-cov', 'llvm-profdata', 'llvm-readobj')}
    report = {'schema': 'objc3c.native-source-coverage.v1', 'status': 'running',
        'source_build_id': plan['source_build_id'], 'build_config_digest': plan['build_config_digest'],
        'revision': subprocess.check_output(['git', '-C', repo, 'rev-parse', 'HEAD'], text=True).strip(),
        'scope': 'First-party façade and standalone-runtime denominator linked to the exact isolated patched-compiler source-coverage profile.',
        'policy': policy, 'commands': [], 'suites': [], 'objects': {}, 'inputs': {}, 'eligible': {},
        'temporary_build_root': str(build_root), 'build_storage': 'One linked witness resident; temporary build products reclaimed after successful admission.',
        'raw_profile_limit_bytes_per_suite': FIRST_PARTY_PROFILE_LIMIT,
        'profile_runtime': {'path': str(runtime), 'sha256': digest(runtime),
            'resource_dir': str(resource), 'provenance': 'pinned-bootstrap-llvm-23.1.3'},
        'measurement_compiler': {'path': str(compiler), 'sha256': digest(compiler),
            'provenance': 'pinned-bootstrap-llvm-23.1.3',
            'subject': 'measurement-tool-not-covered-implementation'},
        'compiled_product_reuse': {'receipt': str(reuse_build_receipt), 'sha256': digest(reuse_build_receipt),
            'execution_reused': False} if reused else None,
        'exclusions': {'patched-clang-llvm': 'Measured by compiler_source_coverage.py from the same profile and reported as a separate denominator.',
            'upstream-system-headers': 'Reported separately from first-party ownership, never added to its denominator.',
            'language-and-fuzzer-coverage': 'Separate claims; not inferred from implementation percentages.'}}
    frozen = report['inputs']
    for path in [Path(__file__), policy_path, *sorted((repo / 'native/objc3c').rglob('*.cmake')),
                 *sorted((repo / 'native/objc3c').rglob('CMakeLists.txt')), compiler, *tools.values(), runtime]:
        frozen[str(path)] = digest(path)
    def save():
        report['seconds'] = round(time.monotonic()-started, 6)
        write_json(output / 'qualification.json', report)
    def run(name, argv, *, environment=env, expected=0, cwd=repo, timeout=COMMAND_TIMEOUT):
        if any(row['name'] == name for row in report['commands']): raise ValueError('duplicate coverage command')
        row = {'name': name, 'environment': environment, **command(argv, cwd=cwd, environment=environment,
            log=output / 'logs' / (name+'.log'), timeout=timeout)}
        report['commands'].append(row);save()
        text = Path(row['log']).read_text(errors='replace')
        if row['timed_out'] or row['exit_code'] != expected:
            raise ValueError('coverage command failed: '+name+'; see '+row['log'])
        if expected == 0: require_clean_diagnostic(text)
        row['status'] = 'PASS';save()
        return text
    def export(name, binary, objects, profile, *, build):
        args = [tools['llvm-cov'], 'export', binary, '--check-binary-ids', '--skip-expansions', '--compilation-dir='+str(build)]
        args += ['--instr-profile='+str(profile)] if profile else ['--empty-profile']
        # Live counters belong to the linked binary's selected COMDAT/inline
        # variants. Relocatable objects have a separate zero-profile view;
        # mixing them can attach counters to different function hashes.
        if profile is None:
            for obj in objects: args += ['--object='+obj['object']]
        run(name, args)
        return read_export(output / 'logs' / (name+'.log'), repo=repo, build=build, eligible=report['eligible'])
    try:
        for name, path in {'clang++': compiler, **tools}.items():
            version = run(name+'-version', [path, '--version'])
            if '23.1.3' not in version: raise ValueError('coverage tool version differs from the admitted bootstrap')
        libraries = [(stage / 'lib' / name).resolve(strict=True) for name in ('libLLVM.so', 'libclang-cpp.so')]
        frozen.update({str(path): digest(path) for path in libraries})
        library_identity = {str(library): elf_build_id(run('coverage-library-notes-'+str(index),
            [tools['llvm-readobj'], '--notes', library])) for index, library in enumerate(libraries)}
        library_ids = set(library_identity.values())
        report['coverage_library_build_ids'] = library_identity
        # Keep uninstantiated headers and unused compiled files visible; only
        # LLVM's emitted maps determine executable-region denominators.
        for path in sorted((repo / 'native/objc3c/src').rglob('*')):
            if path.suffix in ('.cpp', '.h', '.hpp'):
                name = path.relative_to(repo).as_posix()
                report['eligible'][name] = digest(path);frozen[str(path)] = digest(path)
        report['eligible_digest'] = identity(report['eligible'])
        for component in ('frontend', 'runtime'):
            build = build_root / component
            source = repo / ('native/objc3c' if component == 'frontend' else 'native/objc3c/src/runtime')
            configure = [plan['host_environment']['cmake_path'], '-S', source, '-B', build, '-G', 'Ninja',
                '-DCMAKE_MAKE_PROGRAM='+plan['host_environment']['ninja_path'],
                '-DCMAKE_CXX_COMPILER='+str(compiler), '-DCMAKE_BUILD_TYPE=Debug',
                '-DCMAKE_CXX_FLAGS_DEBUG=-O0 -g0 -resource-dir='+str(resource),
                '-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld',
                '-DOBJC3C_SOURCE_COVERAGE=ON', '-DOBJC3C_LINK_CLANG_CPP=ON',
                '-DBUILD_TESTING=ON', '-DPython3_EXECUTABLE='+__import__('sys').executable]
            if component == 'frontend':
                configure += ['-DOBJC3C_CLANG_FOUNDATION_ROOT='+str(stage),
                              '-DOBJC3C_COMPILER_INSTRUMENTATION_PROFILE=compiler-source-coverage']
            run(component+'-configure', configure)
            discovery = json.loads(run(component+'-discovery', ['ctest', '--test-dir', build, '--show-only=json-v1']))
            cases = {row['name']: row for row in discovery['tests']}
            selected = policy[component+'_tests']
            if not selected or len(selected) != len(set(selected)) or not set(selected) <= set(cases):
                raise ValueError('coverage selected suite is missing, empty or duplicated')
            report['exclusions'][component+'-unselected-tests'] = sorted(set(cases)-set(selected))
            targets = [name+'_test' for name in selected]
            records = {}
            report['objects'][component] = []
            for name in selected:
                target = name+'_test'
                common = ['objc3c_frontend_v2'] if component == 'frontend' else []
                run(component+'-'+name+'-build', [plan['host_environment']['cmake_path'], '--build', build,
                    '--target', target, *common, '--parallel', str(jobs)], timeout=BUILD_TIMEOUT)
                graph = run(component+'-'+name+'-graph', [plan['host_environment']['ninja_path'], '-C', build,
                    '-t', 'commands', target, *common])
                objects = compile_inventory(build, graph, required_flags=policy['compile_flags'])
                for obj in objects:
                    if obj['object'] in records:
                        if records[obj['object']] != obj: raise ValueError('coverage object changed between test shards')
                        continue
                    sections = run(component+'-sections-'+str(len(records)), [tools['llvm-readobj'], '--sections', obj['object']])
                    if not all(marker in sections for marker in ('__llvm_covmap', '__llvm_prf_cnts', '__llvm_prf_data')):
                        raise ValueError('actual object has no coverage counters/mapping')
                    records[obj['object']] = obj
                    frozen[obj['object']] = obj['sha256']
                report['objects'][component] = list(records.values())
                deps = run(component+'-'+name+'-dependencies', [plan['host_environment']['ninja_path'], '-C', build, '-t', 'deps'])
                from .implementation_analysis import dependencies as ninja_dependencies
                closure = ninja_dependencies(deps, build=build, selected_outputs={obj['object'] for obj in objects})
                for obj in objects:
                    if not closure.get(obj['object']): raise ValueError('coverage object lacks actual dependency closure')
                    frozen.update({path: digest(Path(path)) for path in closure[obj['object']]})
                current = json.loads(run(component+'-'+name+'-effective-discovery',
                    ['ctest', '--test-dir', build, '--show-only=json-v1', '-R', '^'+re.escape(name)+'$']))['tests']
                if len(current) != 1 or current[0]['name'] != name or not current[0].get('command'):
                    raise ValueError('built coverage test has no exact executable command')
                binary = Path(current[0]['command'][0]).resolve(strict=True)
                binary_hash = digest(binary)
                loader_graph = None
                if component == 'frontend':
                    loader_graph = loaded_graph(binary, Path('/lib64/ld-linux-x86-64.so.2').resolve(strict=True),
                        libraries, required=libraries,
                        run=lambda suffix, argv, name=name: run('frontend-'+name+'-'+suffix, argv))
                    frozen.update(loader_graph['libraries'])
                work = output / 'suites' / (component+'-'+name);work.mkdir(parents=True)
                raw = work / 'raw';raw.mkdir()
                case_env = {**env, 'LLVM_PROFILE_FILE': str(raw / '%p-%m.profraw')}
                result = work / 'tests.xml'
                timeout = suite_timeout(policy, component, name)
                run(component+'-'+name, ['ctest', '--test-dir', build, '--no-tests=error', '--output-on-failure',
                    '-R', '^'+re.escape(name)+'$', '--timeout', str(timeout), '--output-junit', result],
                    environment=case_env, timeout=timeout+30)
                executions = junit(result, expected={name})
                profiles = raw_profiles(raw, limit=FIRST_PARTY_PROFILE_LIMIT)
                profile = work / 'merged.profdata'
                run(component+'-'+name+'-merge', [tools['llvm-profdata'], 'merge', '--sparse', *profiles, '-o', profile])
                profile_ids = run(component+'-'+name+'-binary-ids', [tools['llvm-profdata'], 'show', '--binary-ids', profile])
                notes = run(component+'-'+name+'-binary-notes', [tools['llvm-readobj'], '--notes', binary])
                expected_ids = {elf_build_id(notes), *(library_ids if component == 'frontend' else set())}
                if binary_ids(profile_ids) != expected_ids:
                    raise ValueError('coverage profile differs from the actual selected test binary')
                baseline = export(component+'-'+name+'-mapping-baseline', binary, objects, None, build=build)
                rows = export(component+'-'+name+'-export', binary, (), profile, build=build)
                for filename, row in rows.items():
                    mapping = baseline[filename]
                    row['compiled_mapping_summary'] = mapping['summary']
                    if row['summary'] is None and mapping['summary'] is not None:
                        row.update(state='mapped-but-not-linked', summary=mapping['summary'])
                if digest(binary) != binary_hash: raise ValueError('coverage test binary changed during execution/export')
                if not any(row['state'] == 'executed' for row in rows.values()):
                    raise ValueError('coverage export has no executed first-party source mappings')
                raw_records = reclaim_raw_profiles(profiles)
                report['suites'].append({'name': name, 'component': component, 'status': 'PASS',
                    'executions': executions, 'binary_sha256': digest(binary), 'binary_ids': profile_ids,
                    'raw_profiles': raw_records, 'profile_sha256': digest(profile),
                    'raw_profiles_reclaimed': True,
                    'loader_graph': loader_graph,
                    'source_coverage': rows})
                save()
                binary.unlink()
                # The indexed profile and compressed exact raw inputs remain;
                # transient linked test binaries are reclaimed per shard.
        if len(report['suites']) != sum(len(policy[key]) for key in ('frontend_tests', 'runtime_tests')):
            raise ValueError('source coverage is missing a required suite/shard')
        if any(digest(Path(path)) != value for path, value in frozen.items()):
            raise ValueError('source coverage input/object/tool changed during execution')
        report['status'] = 'PASS'
        __import__('shutil').rmtree(build_root)
        report['temporary_build_reclaimed'] = True
    except Exception as error:
        report.update(status='FAIL', error=str(error));raise
    finally:
        save()
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--jobs', type=int, choices=(1, 2, 3, 4), default=2)
    parser.add_argument('--reuse-build-receipt', type=Path)
    result = qualify(**vars(parser.parse_args()))
    print(json.dumps({'status': result['status'], 'suites': len(result['suites'])}))


if __name__ == '__main__':
    main()
