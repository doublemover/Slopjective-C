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
from .sanitizer_policy import DETECTORS, validate_detection


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
    sanitizer_library = (bootstrap / 'lib/clang/23/lib/x86_64-unknown-linux-gnu/libclang_rt.asan.so').resolve(strict=True)
    link_flags = '-shared-libasan -L'+str(build / 'lib')+' -Wl,-rpath,'+str(sanitizer_library.parent)
    argv = [foundation['host_environment']['cmake_path'], '-S', str(source / 'lldb'), '-B', str(root), '-G', 'Ninja',
        '-DCMAKE_C_COMPILER='+str(bootstrap / 'bin/clang'), '-DCMAKE_CXX_COMPILER='+str(bootstrap / 'bin/clang++'),
        '-DCMAKE_MAKE_PROGRAM='+foundation['host_environment']['ninja_path'],
        '-DLLVM_DIR='+str(stage / 'lib/cmake/llvm'), '-DClang_DIR='+str(stage / 'lib/cmake/clang'),
        '-DLLVM_TABLEGEN='+str(tablegen), '-DPython3_EXECUTABLE='+foundation['host_environment']['python_executable'],
        # LLVM's installed distribution omits the TableGen development archive.
        # LLDB's own matched tablegen helper needs that canonical build archive.
        '-DCMAKE_EXE_LINKER_FLAGS='+link_flags,
        '-DCMAKE_SHARED_LINKER_FLAGS='+link_flags,
        '-DCMAKE_INSTALL_PREFIX='+str(root / 'stage'),
        *('-D'+key+'='+value for key, value in document['cmake'].items())]
    return {'schema': 'objc3c.llvm-development-debugger-plan.v1', 'identity': key,
            'binding': binding, 'foundation': foundation, 'build': str(root), 'configure': argv,
            'tablegen_sha256': digest(tablegen), 'tablegen_library_sha256': digest(tablegen_library),
            'sanitizer_library': str(sanitizer_library), 'sanitizer_library_sha256': digest(sanitizer_library),
            'boundary': document['boundary'], 'targets': document['targets']}


def adopt(configuration: dict, build: Path, previous: dict) -> dict:
    """Reuse compatible compilation; reconfigure and rebuild changed link edges."""
    def binding(plan):
        return {key: value for key, value in plan['binding'].items() if key != 'owner_sha256'}
    def recipe(plan):
        # Relinking with newly admitted foundation development archives need
        # not discard compatible compiled objects. All compilation options,
        # generated headers and foundation identities still match exactly;
        # CMake reruns affected link edges and the new graph is checked below.
        return [arg.replace(plan['build'], '{build}') for arg in plan['configure']
                if not arg.startswith(('-DCMAKE_EXE_LINKER_FLAGS=', '-DCMAKE_SHARED_LINKER_FLAGS='))]
    if (binding(previous) != binding(configuration) or recipe(previous) != recipe(configuration) or
            previous['tablegen_sha256'] != configuration['tablegen_sha256'] or
            previous['targets'] != configuration['targets'] or str(build) != previous['build']):
        raise ValueError('reused debugger build differs in source, bootstrap or compilation recipe')
    old = configuration['build']
    return {**configuration, 'build': str(build),
            'configure': [arg.replace(old, str(build)) for arg in configuration['configure']],
            'reused_build_identity': previous['identity']}


def qualify_shared_sanitizer_route(*, output: Path, run, compiler: Path,
        sanitizer: Path, libraries: list[Path]) -> list[dict]:
    """Prove the shared-ASan link route with clean and detecting controls."""
    fixtures = Path(__file__).resolve().parents[2] / 'tests/tooling/fixtures/native_sanitizer_controls'
    flags = ['--no-default-config', '--driver-mode=g++', '--target=x86_64-unknown-linux-gnu',
             '-std=c++20', '-O1', '-g', '-fno-omit-frame-pointer',
             '-fno-sanitize-recover=undefined', '-fsanitize=address,undefined',
             '-shared-libasan', '-fuse-ld=lld', '-pthread',
             '-Wl,-rpath,'+str(sanitizer.parent),
             '-Wl,-rpath,'+str(libraries[0].parent)]
    results = []
    for detector in ('asan', 'ubsan'):
        source = (fixtures / (detector+'.cpp')).resolve(strict=True)
        for bad in (False, True):
            kind = 'bad' if bad else 'good'
            name = 'shared-runtime-'+detector+'-'+kind
            binary = output / name
            run(name+'-build', [compiler, *flags,
                *(['-DOBJC3_DETECTOR_BAD=1'] if bad else []), source,
                '-Wl,--no-as-needed', *libraries, '-Wl,--as-needed', '-o', binary],
                outputs=[binary])
            graph = loaded_graph(binary, Path('/lib64/ld-linux-x86-64.so.2').resolve(),
                [*libraries, sanitizer], required=(*libraries, sanitizer),
                run=lambda suffix, cmd, name=name: run(name+'-'+suffix, cmd))
            diagnostic = run(name+'-execute', [binary, *DETECTORS[detector][3]],
                expected=86 if bad else 0)
            validate_detection(detector, source, 86 if bad else 0, diagnostic, bad=bad)
            results.append({'detector': detector,
                'classification': 'expected-detection' if bad else 'clean-control',
                'source': str(source), 'source_sha256': digest(source),
                'binary': str(binary), 'binary_sha256': digest(binary),
                'loaded_graph': graph})
    return results


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
        source = Path(foundation['paths']['source'])
        foundation_build = Path(foundation['paths']['build'])
        ninja = Path(foundation['host_environment']['ninja_path'])
        extra_targets = ['LLVMDebuginfod', 'LLVMHTTP']
        report['additional_foundation_objects'] = graph_records(foundation_build, source, extra_targets, ninja,
            environment, foundation=True, run=lambda name, cmd: run('foundation-development-'+name, cmd))
        verify_symbols(report['additional_foundation_objects'],
            Path(foundation['host_environment']['bootstrap_root']) / 'bin/llvm-nm',
            run=lambda name, cmd: run('foundation-development-'+name, cmd))
        extra_archives = [foundation_build / 'lib' / ('lib'+name+'.a') for name in extra_targets]
        report['additional_foundation_archives'] = verify_owned_archives(extra_archives,
            report['additional_foundation_objects'], owners=archive_owners(foundation_build, extra_archives, ninja,
                run=lambda name, cmd: run('foundation-development-'+name, cmd)))
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
        sanitizer = Path(configuration['sanitizer_library'])
        report['shared_sanitizer_controls'] = qualify_shared_sanitizer_route(output=output, run=run,
            compiler=bootstrap / 'bin/clang++', sanitizer=sanitizer, libraries=libraries)
        libraries += [path.resolve() for path in (build / 'lib').glob('liblldb.so*') if path.is_file()]
        libraries.append(sanitizer)
        report['tools'] = {}
        for name in configuration['targets']:
            tool = build / 'bin' / name
            graph = loaded_graph(tool, Path('/lib64/ld-linux-x86-64.so.2').resolve(), libraries,
                required=(libraries[0], libraries[-1]), run=lambda suffix, cmd, name=name: run(name+'-'+suffix, cmd))
            version = run(name+'-version', [tool, 'version' if name == 'lldb-server' else '--version'])
            report['tools'][name] = {'path': str(tool), 'sha256': digest(tool), 'version': version, 'loaded_graph': graph}
        immutable_inputs = {
            Path(__file__): configuration['binding']['owner_sha256'],
            repo / 'toolchains/llvm/assurance-debugger.json': configuration['binding']['inventory_sha256'],
            sanitizer: configuration['sanitizer_library_sha256'],
        }
        if any(digest(path) != expected for path, expected in immutable_inputs.items()):
            raise ValueError('debugger qualification input changed during execution')
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
