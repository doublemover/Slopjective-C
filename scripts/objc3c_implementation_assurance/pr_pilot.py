"""Check the executor implementation with the ordinary, already admitted PR compiler."""
from __future__ import annotations

import json
import os
from pathlib import Path
import shlex
import shutil
from urllib.parse import unquote, urlsplit

from scripts.objc3c_assurance.evidence import artifact
from scripts.objc3c_toolchain.binary_packages import digest
from scripts.objc3c_toolchain.compiler_instrumentation import _without_dependency_flags
from scripts.objc3c_toolchain.implementation_analysis import (
    dependency_arguments, discover_variant_dependencies, inventory, textual_pch,
)
from scripts.objc3c_toolchain.linux_host import native_linux_host
from scripts.objc3c_toolchain.llvm_ir_execution import Execution
from scripts.objc3c_toolchain.llvm_tools import identity, verify_installation
from scripts.objc3c_toolchain.package import MANIFEST, SCHEMA as SDK_SCHEMA
from scripts.objc3c_toolchain.sanitizer_policy import strict_environment
from scripts.objc3c_toolchain.sdk_example import dependencies

SCHEMA = 'objc3c.pr-implementation-pilot.v1'
SOURCE = Path('native/objc3c/src/runtime/concurrency/service_executor.cpp')
CONTROL = Path('tests/tooling/fixtures/implementation_analysis/control.cpp')
CHECKER = 'core.NullDereference'
CHECKER_FLAGS = ('--analyze', '--analyzer-no-default-checks', '-Xclang', '-analyzer-checker='+CHECKER)
CHECKER_INVENTORY = ['OVERVIEW: Clang Static Analyzer Enabled Checkers List', '', CHECKER]
UNSELECTED = {'status': 'NOT-SELECTED', 'reason': 'no inputs in the executor analysis pilot'}
PILOT_INPUTS = {CONTROL.as_posix(), '.github/workflows/native-pr.yml', 'scripts/objc3c_assurance/pr_native.py'}


def selected(plan: dict) -> bool:
    paths = plan['changed_paths']
    return plan['classification'] == 'native' and (
        not paths or plan['foundation_policy'] == 'rebuild-required' or any(
            path.startswith(('native/', 'scripts/objc3c_implementation_assurance/')) or path in PILOT_INPUTS
            for path in paths))


def _analyze_selected(plan: dict, headers: list[str]) -> bool:
    paths = plan['changed_paths']
    return (not paths or plan['foundation_policy'] == 'rebuild-required' or
            any(path in headers or path.startswith(('native/cmake/', 'scripts/objc3c_implementation_assurance/')) or
                path in PILOT_INPUTS or
                (path.startswith('native/') and path.endswith(('CMakeLists.txt', '.cmake'))) for path in paths))


def _release_compiler(manifest: dict, admitted: dict) -> str:
    document = dict(manifest)
    seal = document.pop('seal_sha256')
    if (document.get('schema') != SDK_SCHEMA or seal != identity(document) or
            any(manifest.get(field) != value for field, value in admitted.items()) or
            not admitted.get('seal_sha256')):
        raise ValueError('implementation pilot compiler manifest differs from the admitted SDK')
    files = document['files']
    if (files.get('bin/clang++') != {'kind': 'symlink', 'target': 'clang'} or
            files.get('bin/clang') != {'kind': 'symlink', 'target': 'clang-23'} or
            files.get('bin/clang-23', {}).get('kind') != 'file'):
        raise ValueError('implementation pilot requires the SDK compiler executable')
    return files['bin/clang-23']['sha256']


def _analyzer_arguments(original, compiler: Path, output: Path) -> list[str]:
    args = iter(_without_dependency_flags(original)[1:])
    result = [str(compiler), '--no-default-config']
    for argument in args:
        if argument == '-o':
            next(args)
        elif argument != '-c':
            result.append(argument)
    return [*result, *CHECKER_FLAGS, '--analyzer-output', 'sarif', '-o', str(output)]


def _checker_arguments(original, compiler, output):
    return [*_analyzer_arguments(original, compiler, output), '-Xclang', '-analyzer-list-enabled-checkers']


def _diagnostics(sarif: Path) -> list[dict]:
    data = json.loads(sarif.read_text())
    runs = data.get('runs')
    if (data.get('version') != '2.1.0' or not isinstance(runs, list) or len(runs) != 1 or
            runs[0]['tool']['driver']['name'] != 'clang' or not isinstance(runs[0].get('results'), list)):
        raise ValueError('implementation pilot requires a complete Clang SARIF report')
    diagnostics = []
    for result in runs[0]['results']:
        locations = result['locations']
        if result['ruleId'] != CHECKER or len(locations) != 1:
            raise ValueError('implementation pilot reported an unreviewed checker or location')
        location = locations[0]['physicalLocation']
        uri = urlsplit(location['artifactLocation']['uri'])
        if uri.scheme != 'file' or uri.netloc:
            raise ValueError('implementation pilot diagnostic is missing its local source location')
        diagnostics.append({'file': unquote(uri.path), 'line': location['region']['startLine'],
                            'column': location['region']['startColumn'], 'check': CHECKER})
    return diagnostics


def _check_controls(name, diagnostics, workspace):
    expected = [{'file': str(workspace / CONTROL), 'line': 4, 'column': 10, 'check': CHECKER}]
    if diagnostics != (expected if name == 'control-defective' else []):
        raise ValueError('implementation pilot '+name+' checker control did not match its expected source site')


def execute(repo: Path, plan: dict, output: Path, *, sdk: Path | None, admitted: dict | None,
            foundation_report: Path) -> dict:
    if not selected(plan):
        return dict(UNSELECTED)
    repo, output = repo.resolve(strict=True), output.resolve()
    if not output.is_relative_to(repo / 'out') or output.exists():
        raise ValueError('implementation pilot requires fresh owned evidence')
    build = (repo / 'out/toolchains/objc3-facade' / ('build-'+native_linux_host())).resolve(strict=True)
    workspace = repo / 'out/toolchains/objc3-development'
    bootstrap = Path(os.environ['OBJC3C_BOOTSTRAP_LLVM_ROOT']).resolve(strict=True)
    prefix = sdk.resolve(strict=True) if sdk else workspace / ('stage-'+native_linux_host())
    compiler = prefix / 'bin/clang++'
    executor_env = strict_environment(compiler, bootstrap / 'bin/llvm-symbolizer')
    report = {'schema': SCHEMA, 'status': 'running', 'revision': plan['revision'],
              'plan_sha256': plan['sha256'], 'workspace': str(repo), 'compiler': str(compiler),
              'checker': CHECKER, 'scope': SOURCE.as_posix(), 'graph_directory': str(build),
              'compiler_sha256': digest(compiler), 'diagnostics': {}}
    execution = Execution(output, executor_env, report, timeout=None)

    def analyze(name, original, cwd):
        sarif = output / (name+'.sarif')
        execution.run(name, _analyzer_arguments(original, compiler, sarif), cwd=cwd, outputs=[sarif])
        result = _diagnostics(sarif)
        report['diagnostics'][name] = result
        return result

    try:
        if sdk is not None:
            if admitted is None:
                raise ValueError('implementation pilot requires the admitted base SDK identity')
            manifest = output / 'sdk.json'
            shutil.copyfile(prefix / MANIFEST, manifest)
            if report['compiler_sha256'] != _release_compiler(json.loads(manifest.read_text()), admitted):
                raise ValueError('implementation pilot compiler binary differs from the admitted SDK')
            report['producer'] = {'kind': 'verified-release', 'manifest_sha256': digest(manifest)}
        else:
            producer = workspace / ('build-'+native_linux_host())
            owner = producer / 'foundation-build-report.json'
            if digest(owner) != digest(foundation_report) or json.loads(owner.read_text()).get('status') != 'passed':
                raise ValueError('implementation pilot requires the passing current PR foundation owner')
            report['producer'] = {'kind': 'current-source-build', 'receipt_sha256': digest(owner),
                'install_transition': verify_installation(producer / 'bin/clang++', compiler, producer)}
        cache = build / 'CMakeCache.txt'
        cache_entries = dict((line.split(':', 1)[0], line.split('=', 1)[1]) for line in cache.read_text().splitlines()
                             if line.startswith(('CMAKE_HOME_DIRECTORY:', 'CMAKE_MAKE_PROGRAM:',
                                                 'OBJC3C_CLANG_FOUNDATION_ROOT:')))
        if (cache_entries.get('CMAKE_HOME_DIRECTORY') != str(repo / 'native/objc3c') or
                Path(cache_entries.get('OBJC3C_CLANG_FOUNDATION_ROOT', '')).resolve() != prefix):
            raise ValueError('implementation pilot requires the ordinary native PR frontend graph')
        ninja = Path(cache_entries.get('CMAKE_MAKE_PROGRAM', ''))
        if not ninja.is_absolute() or ninja.name != 'ninja' or not os.access(ninja, os.X_OK):
            raise ValueError('implementation pilot requires an owned configured Ninja executable')
        database = build / 'compile_commands.json'
        graph = execution.run('ordinary-ninja-graph', [ninja, '-C', build, '-t', 'commands', 'objc3_runtime_services'])
        row, = inventory(database, graph, {repo / SOURCE})
        _, pch = textual_pch(row)
        if pch or Path(row['arguments'][0]).resolve(strict=True) != (bootstrap / 'bin/clang++').resolve(strict=True):
            raise ValueError('implementation pilot compiler command departed from its reviewed runtime shape')
        # Capture the effective row without importing all unrelated frontend build actions.
        report['variant'] = row
        for index, path in enumerate(sorted(row['response_files'])):
            shutil.copyfile(path, output / f'response-{index}.txt')
        depfile = output / 'executor.d'
        closure = discover_variant_dependencies(row, depfile, name='executor-dependencies', run=execution.run)
        inputs = {path: digest(Path(path)) for path in sorted(set(closure) | set(row['response_files']) |
                  {str(repo / CONTROL), str(ninja), row['arguments'][0], str(database), str(cache)})}
        report['repository_inputs'] = {str(Path(path).relative_to(repo)): value for path, value in inputs.items()
            if Path(path).is_relative_to(repo) and not Path(path).is_relative_to(repo / 'out')}
        witness = ['clang++', '-std=c++20', str(repo / CONTROL)]
        enabled = execution.run('checker-inventory', _checker_arguments(
            row['arguments'], compiler, output / 'checker-inventory.sarif'), cwd=Path(row['directory']))
        if enabled.splitlines() != CHECKER_INVENTORY:
            raise ValueError('implementation pilot configured other than its one reviewed checker')
        for name, args in (('control-defective', [*witness, '-DDEFECTIVE']), ('control-corrected', witness)):
            _check_controls(name, analyze(name, args, repo), repo)
        headers = [str(Path(path).relative_to(repo)) for path in closure if Path(path).is_relative_to(repo)]
        report['dependent'] = _analyze_selected(plan, headers)
        if report['dependent'] and analyze('executor', row['arguments'], Path(row['directory'])):
            raise ValueError('implementation pilot found '+CHECKER+' in '+SOURCE.as_posix())
        if digest(compiler) != report['compiler_sha256'] or any(digest(Path(path)) != value for path, value in inputs.items()):
            raise ValueError('implementation pilot inputs changed during execution')
        report['status'] = 'PASS'
    except BaseException as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        execution.save()
    return {'status': 'PASS', 'dependent': report['dependent'], 'evidence': artifact(output.parent, output / 'qualification.json')}


def verify(repo: Path, plan: dict, root: Path, summary: dict, *, admitted: dict | None,
           foundation_report: Path) -> None:
    output = root / 'implementation-pilot'
    if not selected(plan):
        if summary != UNSELECTED or output.exists():
            raise ValueError('implementation pilot missing its exact unselected disposition')
        return
    qualification = output / 'qualification.json'
    if summary.get('evidence') != artifact(root, qualification) or summary.get('status') != 'PASS':
        raise ValueError('implementation pilot missing its exact passing qualification')
    report = json.loads(qualification.read_text())
    expected = {'status': 'PASS', 'schema': SCHEMA, 'revision': plan['revision'], 'plan_sha256': plan['sha256'],
                'scope': SOURCE.as_posix(), 'checker': CHECKER}
    if any(report.get(field) != value for field, value in expected.items()):
        raise ValueError('implementation pilot qualification describes different inputs')
    producer = report['producer']
    if admitted is not None:
        manifest = output / 'sdk.json'
        if (producer != {'kind': 'verified-release', 'manifest_sha256': artifact(output, manifest)['sha256']} or
                report['compiler_sha256'] != _release_compiler(json.loads(manifest.read_text()), admitted)):
            raise ValueError('implementation pilot omitted the matching release compiler proof')
    elif (producer.get('kind') != 'current-source-build' or
            producer.get('receipt_sha256') != digest(foundation_report) or
            producer.get('install_transition', {}).get('status') != 'PASS' or
            producer['install_transition'].get('installed_sha256') != report['compiler_sha256']):
        raise ValueError('implementation pilot omitted the matching current compiler proof')

    original = Path(report['workspace'])
    row = report['variant']
    if not original.is_absolute() or row['file'] != str(original / SOURCE):
        raise ValueError('implementation pilot command did not examine the reviewed executor source')
    for index, (_, signature) in enumerate(sorted(row['response_files'].items())):
        if artifact(output, output / f'response-{index}.txt')['sha256'] != signature:
            raise ValueError('implementation pilot compile response-file input changed')
    depfile = output / 'executor.d'
    dep_signature = artifact(output, depfile)['sha256']
    closure = set(dependencies(depfile, Path(row['directory']), preserve_spelling=True))
    headers = {str(path.relative_to(original)) for path in closure if path.is_relative_to(original)}
    owned = {header for header in headers if not header.startswith('out/')} | {CONTROL.as_posix()}
    recorded = report['repository_inputs']
    if (SOURCE.as_posix() not in headers or not owned <= recorded.keys() or
            any(not (repo / name).resolve(strict=True).is_relative_to(repo) or
                digest(repo / name) != value for name, value in recorded.items())):
        raise ValueError('implementation pilot retained stale or incomplete repository dependencies')
    dependent = _analyze_selected(plan, list(headers))
    if report.get('dependent') is not dependent or summary.get('dependent') is not dependent:
        raise ValueError('implementation pilot omitted a dependency-selected executor analysis')
    names = ['ordinary-ninja-graph', 'executor-dependencies', 'checker-inventory', 'control-defective', 'control-corrected']
    if dependent:
        names.append('executor')
    commands = report['commands']
    if [command['name'] for command in commands] != names:
        raise ValueError('implementation pilot native action inventory is incomplete')
    retained_output = Path(commands[1]['log']).parents[1]
    compiler = Path(report['compiler'])
    for command in commands:
        name = command['name']
        log = output / 'logs' / (name+'.log')
        if (command.get('status') != 'PASS' or command['exit_code'] != 0 or command['timed_out'] or
                command['log_sha256'] != artifact(output, log)['sha256']):
            raise ValueError('implementation pilot native action evidence is incomplete or failed')
        if name == 'executor-dependencies':
            retained = retained_output / 'executor.d'
            if (command['outputs'] != {str(retained): dep_signature} or command['cwd'] != row['directory'] or
                    command['argv'] != dependency_arguments(row, retained)):
                raise ValueError('implementation pilot dependency proof differs from the native compile flags')
        elif name == 'checker-inventory':
            if (log.read_text().splitlines() != CHECKER_INVENTORY or command['cwd'] != row['directory'] or
                    command['argv'] != _checker_arguments(row['arguments'], compiler,
                                                           retained_output / 'checker-inventory.sarif')):
                raise ValueError('implementation pilot retained a different checker inventory')
        elif name == 'ordinary-ninja-graph':
            argv = command['argv']
            if (Path(argv[0]).name != 'ninja' or argv[1:] != ['-C', report['graph_directory'], '-t', 'commands',
                    'objc3_runtime_services'] or not _variant_in_graph(log.read_text(), row)):
                raise ValueError('implementation pilot native command differs from its retained graph')
        elif name.startswith('control-') or name == 'executor':
            sarif = output / (name+'.sarif')
            retained = retained_output / sarif.name
            original_args = row['arguments'] if name == 'executor' else [
                'clang++', '-std=c++20', str(original / CONTROL), *(['-DDEFECTIVE'] if name == 'control-defective' else [])]
            if (command['outputs'] != {str(retained): artifact(output, sarif)['sha256']} or
                    command['argv'] != _analyzer_arguments(original_args, compiler, retained) or
                    command['cwd'] != (row['directory'] if name == 'executor' else str(original))):
                raise ValueError('implementation pilot analyzed different compiler inputs')
            diagnostic = _diagnostics(sarif)
            if diagnostic != report['diagnostics'].get(name) or (name == 'executor' and diagnostic):
                raise ValueError('implementation pilot reported a blocker or changed diagnostics')
            if name.startswith('control-'):
                _check_controls(name, diagnostic, original)


def _variant_in_graph(graph, row):
    expected = _without_dependency_flags(row['arguments'])
    matching = 0
    for line in graph.splitlines():
        command = shlex.split(line)
        if command and Path(command[0]).name == 'ccache':
            command = command[1:]
        if _without_dependency_flags(command) == expected:
            matching += 1
    return matching == 1
