"""Analyze actual native/maintained-Clang compilation variants with matched tools."""
from __future__ import annotations

import argparse
from concurrent.futures import ProcessPoolExecutor, as_completed
import json
import multiprocessing
from pathlib import Path
import re
import shlex

from .binary_packages import digest, installed_shared_library, write_json
from .compiler_instrumentation import _without_dependency_flags, graph_records, loaded_graph, verify_symbols
from .llvm_ir_execution import Execution, StageFailure
from .llvm_tools import foundation, identity, verify_installation
from .sanitizer_policy import strict_environment
from .sdk_example import dependencies as header_dependencies
from .analysis_checkpoint import run_unit, successful_action

SUFFIXES = {'.cpp', '.cc', '.c', '.mm', '.m'}


def patch_units(repo):
    return {value for patch in (repo / 'toolchains/llvm/patches').glob('*.patch')
        for value in re.findall(r'^\+\+\+ b/(clang/lib/.*\.cpp)$', patch.read_text(), re.M)}


def arguments(row):
    result = row.get('arguments') or shlex.split(row['command'])
    if not isinstance(result, list) or not all(isinstance(arg, str) for arg in result):
        raise ValueError('invalid analysis compilation command')
    return result


def output_path(row):
    argv = arguments(row)
    if '-c' not in argv or '-o' not in argv:
        raise ValueError('analysis requires a concrete compilation command/output')
    return (Path(row['directory']) / (row.get('output') or argv[argv.index('-o')+1])).resolve()


def inventory(database, graph, selected_sources):
    """Require every selected source/configuration in the actual default graph."""
    selected_sources = {Path(path).resolve(strict=True) for path in selected_sources}
    expected = {}
    for line in graph.splitlines():
        argv = shlex.split(line)
        if not argv or Path(argv[0]).name == 'clang-scan-deps' or '-c' not in argv or '-o' not in argv:
            continue
        unit = (database.parent / argv[argv.index('-c')+1]).resolve()
        if unit in selected_sources:
            output = (database.parent / argv[argv.index('-o')+1]).resolve()
            if output in expected:
                raise ValueError('duplicate conflicting analysis graph output')
            expected[output] = (unit, argv)
    if {value[0] for value in expected.values()} != selected_sources:
        raise ValueError('analysis graph omitted selected implementation translation units')
    rows, outputs = [], set()
    for row in json.loads(database.read_text()):
        unit = (Path(row['directory']) / row['file']).resolve(strict=True)
        if unit not in selected_sources:
            continue
        output = output_path(row)
        if output not in expected or output in outputs or expected[output][0] != unit:
            raise ValueError('duplicate, conflicting or unowned analysis compilation variant')
        argv = arguments(row)
        if (Path(row['directory']) / argv[argv.index('-c')+1]).resolve() != unit:
            raise ValueError('analysis compilation command names a different translation unit')
        actual, normalized = _without_dependency_flags(expected[output][1]), _without_dependency_flags(argv)
        launcher = {}
        if Path(actual[0]).name == 'ccache' and actual[1:] == normalized:
            launcher[str(Path(actual[0]).resolve(strict=True))] = digest(Path(actual[0]))
        elif actual != normalized:
            raise ValueError('analysis flags differ from the actual compilation graph')
        if not output.is_relative_to(database.parent):
            raise ValueError('analysis object escaped its compilation database')
        responses = {}
        for arg in argv:
            if arg.startswith('@'):
                response = (Path(row['directory']) / arg[1:]).resolve(strict=True)
                responses[str(response)] = digest(response)
        outputs.add(output)
        rows.append({'file': str(unit), 'directory': row['directory'], 'arguments': argv,
                     'output': str(output), 'response_files': responses, 'build_launcher': launcher})
    if outputs != set(expected) or not rows:
        raise ValueError('analysis compilation database is incomplete')
    return rows


def dependencies(text, *, build, selected_outputs=None):
    """Read Ninja's recorded actual header closure, rejecting stale/missing deps."""
    records, current = {}, None
    for line in text.splitlines():
        if line and not line.startswith(' '):
            if line.endswith(': deps not found'):
                current = None
                continue
            match = re.fullmatch(r'(.*): #deps (\d+), deps mtime \d+ \((VALID|STALE)\)', line)
            if not match:
                raise ValueError('unrecognized implementation dependency record')
            current = str((build / match[1]).resolve())
            if selected_outputs is not None and current not in selected_outputs:
                current = None
                continue
            if current in records or match[3] != 'VALID':
                raise ValueError('duplicate or stale implementation dependency record')
            records[current] = []
        elif line.strip() and current is not None:
            records[current].append(str((build / line.strip()).resolve(strict=True)))
    return records


def selected_variants(rows, headers, changed):
    """No changed-line filter: header edits select their complete dependent TUs."""
    if not changed:
        return rows
    paths = {str(Path(path).resolve()) for path in changed}
    known = {row['file'] for row in rows} | {path for values in headers.values() for path in values}
    # Configuration/unknown/new/deleted inputs conservatively select everything.
    if not paths <= known:
        return rows
    return [row for row in rows if row['file'] in paths or paths.intersection(headers.get(row['output'], []))]


def textual_pch(row):
    """Reprocess exact CMake wrapper text; never admit bootstrap serialized AST."""
    original, result, projections = row['arguments'], [], []
    index = 0
    while index < len(original):
        if original[index:index+3] == ['-Xclang', '-include-pch', '-Xclang']:
            pch = (Path(row['directory']) / original[index+3]).resolve(strict=True)
            wrapper = pch.with_suffix('')
            if (pch.name != 'cmake_pch.hxx.pch' or not wrapper.is_file() or
                    original[index+4:index+7] != ['-Xclang', '-include', '-Xclang'] or
                    index+7 >= len(original) or
                    (Path(row['directory']) / original[index+7]).resolve() != wrapper):
                raise ValueError('analysis PCH lacks its exact CMake textual include counterpart')
            projections.append({'bootstrap_pch': str(pch), 'pch_sha256': digest(pch),
                'textual_wrapper': str(wrapper), 'wrapper_sha256': digest(wrapper),
                'reason': 'The matched fork cannot deserialize bootstrap PCH. Preserve the existing forced textual include, all TU defines and include order.'})
            index += 4
        elif original[index] == '-include-pch':
            raise ValueError('unreviewed analysis PCH invocation')
        else:
            result.append(original[index]);index += 1
    return result, projections


def qualify(repo, output, *, frontend_build, runtime_build, tool_build=None, changed=(), jobs=2, resume=None):
    repo, output = repo.resolve(strict=True), output.resolve()
    if output.exists() or not output.is_relative_to(repo / 'out'):
        raise ValueError('implementation analysis requires fresh owned evidence')
    if jobs not in (1, 2, 3, 4): raise ValueError('implementation analysis requires one to four worker processes')
    previous = None
    if resume:
        resume = resume.resolve(strict=True)
        if not resume.is_relative_to(repo / 'out') or resume.stat().st_size > 64*1024**2:
            raise ValueError('implementation analysis checkpoint is outside bounded owned output')
        previous = json.loads(resume.read_text())
    plan, _ = foundation(repo)
    source, stage, build = (Path(plan['paths'][key]).resolve(strict=True)
                            for key in ('source', 'stage', 'build'))
    bootstrap = Path(plan['host_environment']['bootstrap_root'])
    tool_build = (tool_build or repo / 'out/implementation-analysis/build').resolve()
    if not tool_build.is_relative_to(repo / 'out') or tool_build.is_relative_to(output):
        raise ValueError('analysis tool build requires its own owned repository output')
    policy_path = repo / 'toolchains/llvm/implementation-analysis.json'
    policy = json.loads(policy_path.read_text())
    if (policy.get('schema') != 'objc3c.implementation-analysis-policy.v1' or
            policy.get('host') != 'linux-x86_64' or policy.get('tool') != 'objc3-clang-tidy' or
            policy.get('warnings_as_errors') != '*' or policy.get('baseline') != [] or
            policy.get('translation_unit_timeout_seconds') != 600 or
            not policy.get('checks') or len(set(policy['checks'])) != len(policy['checks']) or
            any(not re.fullmatch(r'[a-zA-Z0-9_.-]+', check) for check in policy['checks'])):
        raise ValueError('unreviewed implementation analysis policy/baseline')
    output.mkdir(parents=True)
    environment = strict_environment(bootstrap / 'bin/clang++', bootstrap / 'bin/llvm-symbolizer')
    report = {'schema': 'objc3c.implementation-analysis.v1', 'status': 'running',
        'subject': 'native-and-maintained-clang-implementation', 'policy': policy,
        'source_build_id': plan['source_build_id'], 'build_config_digest': plan['build_config_digest'],
        'environment': environment, 'inventory': [], 'results': [], 'exclusions': [],
        'repository_output': str(repo / 'out'),
        'parallel_workers': jobs,
        'inputs': {str(path): digest(path) for path in (Path(__file__), policy_path,
            repo / 'native/analysis/CMakeLists.txt', repo / 'native/analysis/SelectedChecks.cpp',
            repo / 'tests/tooling/fixtures/implementation_analysis/control.cpp')}}
    executor = Execution(output, environment, report, timeout=600)
    run = executor.run
    try:
        run('configure-tool', [plan['host_environment']['cmake_path'], '-S', repo / 'native/analysis',
            '-B', tool_build, '-G', 'Ninja', '-DCMAKE_CXX_COMPILER='+str(bootstrap / 'bin/clang++'),
            '-DOBJC3_ANALYSIS_FOUNDATION='+str(stage), '-DOBJC3_ANALYSIS_SOURCE='+str(source)])
        run('build-tool', [plan['host_environment']['cmake_path'], '--build', tool_build, '--parallel', '2'])
        tool = tool_build / policy['tool']
        ninja = Path(plan['host_environment']['ninja_path'])
        records = graph_records(tool_build, repo, [policy['tool']], ninja, environment,
                                foundation=False, run=lambda name, argv: run('tool-'+name, argv))
        verify_symbols(records, bootstrap / 'bin/llvm-nm', run=lambda name, argv: run('tool-'+name, argv))
        report['tool_objects'] = records
        tool_headers = dependencies(run('tool-headers', [ninja, '-C', tool_build, '-t', 'deps',
            *(str(Path(row['object']).relative_to(tool_build)) for row in records)]),
            build=tool_build, selected_outputs={row['object'] for row in records})
        if any(not tool_headers.get(row['object']) for row in records):
            raise ValueError('implementation analyzer lacks its build header closure')
        report['inputs'].update({path: digest(Path(path)) for values in tool_headers.values() for path in values})
        for name in ('compiler_instrumentation', 'llvm_tools', 'llvm_ir_execution', 'sanitizer_policy', 'binary_packages', 'analysis_checkpoint'):
            path = Path(__file__).with_name(name+'.py')
            report['inputs'][str(path)] = digest(path)
        libraries = [installed_shared_library(stage / 'lib', name)
                     for name in ('libLLVM.so', 'libclang-cpp.so')]
        report['install_transitions'] = {name: verify_installation(
                                        build / 'lib' / name,
                                        installed_shared_library(stage / 'lib', name), build)
                                        for name in ('libLLVM.so', 'libclang-cpp.so')}
        report['tool_graph'] = loaded_graph(tool, Path('/lib64/ld-linux-x86-64.so.2').resolve(), libraries,
            required=tuple(libraries), run=lambda name, argv: run('tool-'+name, argv))
        report['tool'] = {'path': str(tool), 'sha256': digest(tool), 'version': run('tool-version', [tool, '--version'])}
        checker_text = run('checker-inventory', [tool, '--list-checks', '--checks=*'])
        available = {line.strip() for line in checker_text.splitlines()}
        if not set(policy['checks']) <= available:
            raise ValueError('selected check is absent from the pinned implementation analyzer')
        config = output / 'checks.json'
        write_json(config, {'Checks': '-*,'+','.join(policy['checks']), 'WarningsAsErrors': '*',
            'HeaderFilterRegex': '('+'|'.join(re.escape(str(path)) for path in
                (repo / 'native', source / 'clang', repo / 'tests/tooling/fixtures/implementation_analysis'))+')/.*',
            'SystemHeaders': False, 'FormatStyle': 'none'})
        report['inputs'].update({str(tool): digest(tool), str(config): digest(config),
                                  **report['tool_graph']['libraries']})
        report['configuration'] = str(config)
        report['resource_directory'] = str(bootstrap / 'lib/clang/23')
        for row in records:
            report['inputs'][row['source']] = row['source_sha256']
            report['inputs'][row['object']] = row['sha256']
        def prepare_analysis(name, row):
            directory = output / name
            directory.mkdir()
            args, projection = textual_pch(row)
            row['analysis_arguments'], row['pch_projection'] = args, projection
            for item in projection:
                report['inputs'][item['bootstrap_pch']] = item['pch_sha256']
                report['inputs'][item['textual_wrapper']] = item['wrapper_sha256']
            write_json(directory / 'compile_commands.json', [{'file': row['file'],
                'directory': row['directory'], 'arguments': args}])
            return list(map(str, [tool, '-p', directory, row['file'], '--config-file='+str(config),
                '--warnings-as-errors=*', '--extra-arg=-resource-dir='+str(bootstrap / 'lib/clang/23'),
                '--export-fixes='+str(directory / 'diagnostics.yaml')]))
        def analyze(name, row, *, expected=0, contains=()):
            run(name, prepare_analysis(name, row), expected=expected, contains=contains)
        control = repo / 'tests/tooling/fixtures/implementation_analysis/control.cpp'
        for bad in (False, True):
            analyze('control-'+('defective' if bad else 'corrected'), {'file': str(control), 'directory': str(repo),
                'arguments': [str(bootstrap / 'bin/clang++'), '-std=c++20', *(['-DDEFECTIVE'] if bad else []),
                    '-c', str(control), '-o', str(output / 'unused-control.o')]},
                expected=int(bad), contains=('clang-analyzer-core.NullDereference',) if bad else ())
        owners = [('frontend', frontend_build.resolve(strict=True), None),
                  ('standalone-runtime', runtime_build.resolve(strict=True), None),
                  ('maintained-clang', build, {source / name for name in patch_units(repo)})]
        for label, root, units in owners:
            database = root / 'compile_commands.json'
            graph = run(label+'-graph', [ninja, '-C', root, '-t', 'commands'])
            if units is None:
                prefix = repo / 'native/objc3c/src'
                units = set()
                for line in graph.splitlines():
                    args = shlex.split(line)
                    if args and Path(args[0]).name != 'clang-scan-deps' and '-c' in args and '-o' in args:
                        unit = (root / args[args.index('-c')+1]).resolve()
                        if unit.is_relative_to(prefix) and unit.suffix in SUFFIXES:
                            units.add(unit)
            # CMake module-map response files may belong to a configured variant
            # that has not been compiled. Materialize its owned scan output;
            # analysis must not invent or silently omit those flags.
            for entry in json.loads(database.read_text()):
                if Path(entry['file']).resolve() not in units:
                    continue
                for arg in arguments(entry):
                    if arg.startswith('@'):
                        path = (Path(entry['directory']) / arg[1:]).resolve()
                        if not path.exists() and path.suffix == '.modmap' and path.is_relative_to(root):
                            run(label+'-module-map-'+identity(str(path))[:16],
                                [ninja, '-C', root, str(path.relative_to(root))])
            # A CMake regeneration can update commands while generating maps.
            graph = run(label+'-effective-graph', [ninja, '-C', root, '-t', 'commands'])
            rows = inventory(database, graph, units)
            deps = dependencies(run(label+'-headers', [ninja, '-C', root, '-t', 'deps',
                *(str(Path(row['output']).relative_to(root)) for row in rows)]),
                build=root, selected_outputs={row['output'] for row in rows})
            for row in rows:
                if not deps.get(row['output']):
                    depfile = output / (label+'-'+identity(row['output'])[:16]+'.d')
                    args, index = _without_dependency_flags(row['arguments']), 1
                    query = [args[0]]
                    while index < len(args):
                        if args[index] == '-o':
                            index += 2
                            continue
                        if args[index] != '-c':
                            query.append(args[index])
                        index += 1
                    run(label+'-discover-headers-'+identity(row['output'])[:16],
                        [*query, '-M', '-MF', depfile, '-MT', 'implementation-analysis'],
                        cwd=Path(row['directory']))
                    deps[row['output']] = [str(path) for path in header_dependencies(depfile, Path(row['directory']))]
                    row['dependency_origin'] = 'fresh-original-compiler-preprocessing'
                else:
                    row['dependency_origin'] = 'actual-ninja-compilation'
                if row['file'] not in deps[row['output']]:
                    raise ValueError('implementation variant lacks its actual header dependency closure')
            chosen = selected_variants(rows, deps, changed)
            report['inputs'][str(database)] = digest(database)
            for row in rows:
                row.update(owner=label, identity=identity({'owner': label, **row}), dependencies=deps[row['output']])
                report['inputs'].update({path: digest(Path(path)) for path in row['dependencies']})
                report['inputs'].update(row['response_files'])
                report['inputs'].update(row['build_launcher'])
            report['inventory'].extend(rows)
            report['exclusions'].append({'owner': label, 'database_entries': len(json.loads(database.read_text())),
                'selected_variants': len(rows), 'analyzed_variants': len(chosen),
                'reason': 'Owned implementation TUs and every maintained Clang patch TU; vendor/test/generated-only TUs excluded. Unaffected variants omitted only for explicit dependency-based incremental selection.'})
            executor.timeout = policy['translation_unit_timeout_seconds']
            with ProcessPoolExecutor(max_workers=jobs, mp_context=multiprocessing.get_context('spawn')) as workers:
                futures = []
                for row in chosen:
                    name = label+'-'+row['identity'][:16]
                    argv = prepare_analysis(name, row)
                    cached = successful_action(resume, previous, report, row,
                        arguments=row['analysis_arguments']) if previous else None
                    if cached:
                        report['results'].append(cached)
                    else:
                        futures.append(workers.submit(run_unit, {'name': name, 'identity': row['identity'],
                            'directory': str(output / name), 'environment': environment, 'argv': argv,
                            'timeout': policy['translation_unit_timeout_seconds']}))
                    executor.save()
                for future in as_completed(futures):
                    result, commands = future.result()
                    report['results'].append(result);report['commands'].extend(commands);executor.save()
        if not report['results'] or any(row['status'] != 'PASS' for row in report['results']):
            raise ValueError('implementation analysis contains findings or incomplete translation units')
        if any(digest(Path(path)) != value for path, value in report['inputs'].items()):
            raise ValueError('implementation analysis inputs changed')
        report.update(status='PASS', analyzed_variants=len(report['results']), inventory_variants=len(report['inventory']))
        report['fresh_native_executions'] = sum(not row.get('execution_reused', False) for row in report['results'])
        report['verified_checkpoint_actions'] = sum(row.get('execution_reused', False) for row in report['results'])
    except (OSError, ValueError) as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        executor.save()
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    for name in ('output', 'frontend-build', 'runtime-build'):
        parser.add_argument('--'+name, type=Path, required=True)
    parser.add_argument('--tool-build', type=Path)
    parser.add_argument('--changed', nargs='*', type=Path, default=[])
    parser.add_argument('--jobs', type=int, choices=(1, 2, 3, 4), default=2)
    parser.add_argument('--resume', type=Path)
    result = qualify(**vars(parser.parse_args()))
    print(json.dumps({'status': result['status'], 'analyzed_variants': result['analyzed_variants']}))


if __name__ == '__main__':
    main()
