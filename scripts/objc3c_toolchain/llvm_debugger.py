"""Build/admit an isolated LLDB consumer of the qualified compiler foundation."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from .binary_packages import digest, write_json
from .compiler_instrumentation import (archive_owners, graph_records, loaded_graph,
    verify_owned_archives, verify_symbols)
from .llvm_ir_execution import Execution
from .llvm_tools import identity, selected, qualify as qualify_tools


def plan(repo: Path) -> dict:
    foundation, _, _ = selected(repo)
    inventory = repo / 'toolchains/llvm/assurance-debugger.json'
    document = json.loads(inventory.read_text())
    if (document.get('schema') != 'objc3c.llvm-development-debugger.v1' or
            document.get('foundation_profile') != 'compiler-asan-ubsan' or
            document.get('targets') != ['lldb', 'lldb-server'] or
            document['cmake'].get('LLVM_USE_SANITIZER') != 'Address;Undefined' or
            document['cmake'].get('LLDB_ENABLE_PYTHON') != 'OFF'):
        raise ValueError('unreviewed development debugger inventory')
    binding = {'foundation_id': foundation['source_build_id'],
               'foundation_configuration': foundation['build_config_digest'],
               'inventory_sha256': digest(inventory), 'owner_sha256': digest(Path(__file__)),
               'bootstrap': foundation['host_environment']}
    key = identity(binding)
    root = repo / 'out/toolchains' / ('objc3-assurance-lldb-'+key[:16])
    stage, build, source = (Path(foundation['paths'][name]) for name in ('stage', 'build', 'source'))
    bootstrap = Path(foundation['host_environment']['bootstrap_root'])
    tablegen = (build / 'bin/llvm-tblgen').resolve(strict=True)
    tablegen_library = (build / 'lib/libLLVMTableGen.a').resolve(strict=True)
    argv = [foundation['host_environment']['cmake_path'], '-S', str(source / 'lldb'), '-B', str(root), '-G', 'Ninja',
        '-DCMAKE_C_COMPILER='+str(bootstrap / 'bin/clang'), '-DCMAKE_CXX_COMPILER='+str(bootstrap / 'bin/clang++'),
        '-DCMAKE_MAKE_PROGRAM='+foundation['host_environment']['ninja_path'],
        '-DLLVM_DIR='+str(stage / 'lib/cmake/llvm'), '-DClang_DIR='+str(stage / 'lib/cmake/clang'),
        '-DLLVM_TABLEGEN='+str(tablegen), '-DPython3_EXECUTABLE='+foundation['host_environment']['python_executable'],
        # LLVM's installed distribution omits the TableGen development archive.
        # LLDB's own matched tablegen helper needs that canonical build archive.
        '-DCMAKE_EXE_LINKER_FLAGS=-L'+str(build / 'lib'),
        '-DCMAKE_INSTALL_PREFIX='+str(root / 'stage'),
        *('-D'+key+'='+value for key, value in document['cmake'].items())]
    return {'schema': 'objc3c.llvm-development-debugger-plan.v1', 'identity': key,
            'binding': binding, 'foundation': foundation, 'build': str(root), 'configure': argv,
            'tablegen_sha256': digest(tablegen), 'tablegen_library_sha256': digest(tablegen_library),
            'boundary': document['boundary'], 'targets': document['targets']}


def adopt(configuration: dict, build: Path, previous: dict) -> dict:
    """Reuse compilation after runner-only edits, never after a recipe change."""
    def binding(plan):
        return {key: value for key, value in plan['binding'].items() if key != 'owner_sha256'}
    def recipe(plan):
        return [arg.replace(plan['build'], '{build}') for arg in plan['configure']]
    if (binding(previous) != binding(configuration) or recipe(previous) != recipe(configuration) or
            previous['tablegen_sha256'] != configuration['tablegen_sha256'] or
            previous['targets'] != configuration['targets'] or str(build) != previous['build']):
        raise ValueError('reused debugger build differs in source, bootstrap or compilation recipe')
    old = configuration['build']
    return {**configuration, 'build': str(build),
            'configure': [arg.replace(old, str(build)) for arg in configuration['configure']],
            'reused_build_identity': previous['identity']}


def qualify(repo: Path, output: Path, *, jobs=3, tools_receipt: Path | None = None,
            reuse_build: Path | None = None) -> dict:
    repo, output = repo.resolve(strict=True), output.resolve()
    if output.exists() or not output.is_relative_to(repo / 'out') or jobs < 1:
        raise ValueError('debugger qualification requires a fresh repository out directory and positive jobs')
    configuration = plan(repo)
    if reuse_build is not None:
        reuse_build = reuse_build.resolve(strict=True)
        if (not reuse_build.is_relative_to(repo / 'out/toolchains') or
                not reuse_build.name.startswith('objc3-assurance-lldb-')):
            raise ValueError('reused debugger must be a canonical owned development build')
        configuration = adopt(configuration, reuse_build,
            json.loads((reuse_build / 'development-debugger-plan.json').read_text()))
    foundation = configuration['foundation']
    build = Path(configuration['build'])
    if build.exists():
        previous = json.loads((build / 'development-debugger-plan.json').read_text())
        if previous['identity'] != configuration['identity'] and 'reused_build_identity' not in configuration:
            raise ValueError('debugger build has a different source/configuration owner')
    build.mkdir(parents=True, exist_ok=True)
    write_json(build / 'development-debugger-plan.json', configuration)
    output.mkdir(parents=True)
    _, _, environment = selected(repo)
    report = {'schema': 'objc3c.llvm-development-debugger-receipt.v1', 'status': 'running',
              'identity': configuration['identity'], 'configuration': configuration}
    executor = Execution(output, environment, report, timeout=1800)
    run = executor.run
    try:
        foundation_tools, _, _ = qualify_tools(repo, run=run, receipt=tools_receipt)
        report['foundation_tools_identity'] = foundation_tools['identity']
        tablegen_archive = Path(foundation['paths']['build']) / 'lib/libLLVMTableGen.a'
        if not any(row['path'] == str(tablegen_archive) and row['sha256'] == digest(tablegen_archive)
                   for row in foundation_tools['archives']):
            raise ValueError('LLDB TableGen archive lacks canonical foundation object provenance')
        run('configure', configuration['configure'])
        host = foundation['host_environment']
        run('build', [host['cmake_path'], '--build', build, '--target', *configuration['targets'],
                      '--parallel', str(min(jobs, host['resource_limits']['compile_jobs']))])
        source = Path(foundation['paths']['source'])
        report['objects'] = graph_records(build, source, configuration['targets'], Path(host['ninja_path']),
            environment, foundation=True, run=lambda name, cmd: run('debugger-'+name, cmd))
        bootstrap = Path(host['bootstrap_root'])
        verify_symbols(report['objects'], bootstrap / 'bin/llvm-nm',
                       run=lambda name, cmd: run('debugger-'+name, cmd))
        archives = sorted((build / 'lib').glob('liblldb*.a'))
        report['archives'] = verify_owned_archives(archives, report['objects'],
            owners=archive_owners(build, archives, Path(host['ninja_path']),
                run=lambda name, cmd: run('debugger-'+name, cmd)))
        stage = Path(foundation['paths']['stage'])
        libraries = [(stage / 'lib' / name).resolve() for name in ('libLLVM.so', 'libclang-cpp.so')]
        libraries += [path.resolve() for path in (build / 'lib').glob('liblldb.so*') if path.is_file()]
        report['tools'] = {}
        for name in configuration['targets']:
            tool = build / 'bin' / name
            graph = loaded_graph(tool, Path('/lib64/ld-linux-x86-64.so.2').resolve(), libraries,
                required=(libraries[0],), run=lambda suffix, cmd, name=name: run(name+'-'+suffix, cmd))
            version = run(name+'-version', [tool, 'version' if name == 'lldb-server' else '--version'])
            report['tools'][name] = {'path': str(tool), 'sha256': digest(tool), 'version': version, 'loaded_graph': graph}
        report['status'] = 'PASS'
    except (OSError, ValueError) as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        executor.save()
    return report


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--jobs', type=int, default=3)
    parser.add_argument('--tools-receipt', type=Path)
    parser.add_argument('--reuse-build', type=Path)
    args = parser.parse_args(argv)
    report = qualify(args.repo, args.output, jobs=args.jobs, tools_receipt=args.tools_receipt,
                     reuse_build=args.reuse_build)
    print(json.dumps({'status': report['status'], 'identity': report['identity']}))


if __name__ == '__main__':
    main()
