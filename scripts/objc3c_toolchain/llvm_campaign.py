"""Run the bounded native LLVM IR/debug campaign without packaging an SDK."""
from __future__ import annotations

from .linux_host import native_linux_host

import argparse
import json
from pathlib import Path
import shutil
import subprocess
import sys

from .binary_packages import digest, write_json
from .build import build_foundation
from .llvm_debugger import qualify as qualify_debugger
from .llvm_ir import qualify as qualify_ir
from .llvm_ir_execution import Execution
from .llvm_optimized_debug import qualify as qualify_debug
from .llvm_tools import foundation as admit_foundation, identity, qualify as qualify_tools, selected
from .sanitizer_policy import strict_environment


def verify_results(ir: dict, debugger: dict, optimized: dict) -> list[dict]:
    """Reconcile the inner owners rather than accepting three outer PASS labels."""
    if any(row.get('status') != 'PASS' for row in (ir, debugger, optimized)):
        raise ValueError('LLVM campaign contains an incomplete or failed owner')
    expected = ir.get('expected_cases', [])
    if (not expected or len(expected) != len(set(expected)) or
            len(ir.get('cases', [])) != len(expected) or
            {row['name'] for row in ir['cases'] if row.get('status') == 'PASS'} != set(expected)):
        raise ValueError('LLVM campaign IR case inventory is incomplete')
    if (not ir.get('expected_stages') or
            not set(ir['expected_stages']).issubset({row['name'] for row in ir['commands'] if row.get('status') == 'PASS'})):
        raise ValueError('LLVM campaign IR stage inventory is incomplete')
    controls = debugger.get('shared_sanitizer_controls', [])
    if (set(debugger.get('tools', {})) != {'lldb', 'lldb-server'} or len(controls) != 4 or
            {(row['detector'], row['classification']) for row in controls} !=
            {(kind, result) for kind in ('asan', 'ubsan') for result in ('clean-control', 'expected-detection')}):
        raise ValueError('LLVM campaign debugger/runtime controls are incomplete')
    names = {'O2', 'O2-thinlto', 'async-production', 'async-thinlto'}
    if (len(optimized.get('cases', [])) != len(names) or
            {row['name'] for row in optimized['cases'] if row.get('status') == 'PASS'} != names):
        raise ValueError('LLVM campaign optimized debugger matrix is incomplete')
    return [{'name': row['name'], 'status': 'PASS'} for row in ir['cases']] + optimized['cases']


def qualify(repo: Path, output: Path, *, fetch=False, prepared=False, jobs=3,
            tools_receipt: Path | None = None, verify_compiler_graph=False) -> dict:
    repo, output = repo.resolve(strict=True), output.resolve()
    if output.exists() or not output.is_relative_to(repo / 'out') or jobs < 1:
        raise ValueError('LLVM campaign requires fresh repository out evidence and positive jobs')
    if tools_receipt is not None and not prepared:
        raise ValueError('LLVM tool receipt reuse requires the matching prepared foundation')
    output.mkdir(parents=True)
    revision = subprocess.check_output(['git', '-C', str(repo), 'rev-parse', 'HEAD'], text=True).strip()
    report = {'schema': 'objc3c.llvm-assurance-campaign.v1', 'status': 'running',
              'revision': revision, 'owner_sha256': digest(Path(__file__))}
    try:
        write_json(output / 'qualification.json', report)
        if not prepared:
            try:
                build_foundation(repo, host=native_linux_host(), fetch=fetch, stage=True,
                                 instrumentation_profile='compiler-asan-ubsan')
            finally:
                receipt = repo / ('out/toolchains/objc3-development/build-' + native_linux_host() + '-compiler-asan-ubsan/foundation-build-report.json')
                if receipt.is_file():
                    saved = json.loads(receipt.read_text())
                    evidence = output / 'foundation'
                    evidence.mkdir()
                    write_json(evidence / 'build-report.json', saved)
                    for index, phase in enumerate(saved.get('phases', [])):
                        shutil.copyfile(phase['log'], evidence / (str(index)+'-'+phase['phase']+'.log'))
        foundation, _ = admit_foundation(repo)
        host = foundation['host_environment']
        bootstrap = Path(host['bootstrap_root'])
        environment = strict_environment(bootstrap / 'bin/clang', bootstrap / 'bin/llvm-symbolizer')
        # Configure/build commands need pinned CMake/Ninja as well as Clang.
        environment['PATH'] = ':'.join([str(Path(host[key]).parent) for key in ('cmake_path', 'ninja_path')]
                                      + [environment['PATH']])
        executor = Execution(output, environment, report, timeout=1800)
        run = executor.run
        stage, build = (Path(foundation['paths'][key]) for key in ('stage', 'build'))
        report['foundation'] = json.loads((build / 'foundation-build-report.json').read_text())
        report['foundation_reused'] = prepared
        report['source_build_id'] = foundation['source_build_id']
        root = repo / 'out/toolchains' / ('objc3-assurance-campaign-'+identity({
            'source': foundation['source_build_id'], 'config': foundation['build_config_digest'],
            'host': host})[:16])
        report['build_root'] = str(root)
        runtime, facade = root / 'runtime', root / 'facade'
        tools_to_build = ['opt', 'llvm-dis', 'llvm-reduce']
        if tools_receipt is not None:
            tools_receipt = tools_receipt.resolve(strict=True)
            receipt_hash = digest(tools_receipt)
            admitted_tools, _, _ = qualify_tools(repo, run=run, receipt=tools_receipt)
            report['reused_development_tools'] = {'sha256': receipt_hash, 'identity': admitted_tools['identity']}
            tools_to_build.clear()
        run('development-tools', [host['cmake_path'], '--build', build, '--target', *tools_to_build,
            'llvm-tblgen', 'LLVMTableGen', 'LLVMDebuginfod', 'LLVMHTTP', 'SupportTests', '--parallel', str(jobs)])
        # A clean host has no opt/dis/reduce until the preceding build. Admit
        # the complete tool bundle only now; prepared hosts follow this too.
        selected(repo)
        run('md5-regressions', [build / 'unittests/Support/SupportTests', '--gtest_filter=MD5*'])
        run('runtime-configure', [host['cmake_path'], '-S', repo / 'native/objc3c/src/runtime',
            '-B', runtime, '-G', 'Ninja', '-DCMAKE_CXX_COMPILER='+str(bootstrap / 'bin/clang++'),
            '-DCMAKE_BUILD_TYPE=Release', '-DBUILD_TESTING=OFF'])
        run('runtime-build', [host['cmake_path'], '--build', runtime, '--parallel', str(jobs)])
        runtime_report = output / 'runtime.json'
        run('objc-runtime', [sys.executable, '-B', '-m', 'scripts.objc3c_toolchain_runtime.build_linux',
            '--repo', repo, '--compiler-root', bootstrap, '--jobs', str(jobs),
            *(['--fetch'] if fetch else []), '--report-file', runtime_report], outputs=[runtime_report], cwd=repo)
        dependency = json.loads(runtime_report.read_text())
        if dependency.get('status') != 'passed':
            raise ValueError('LLVM campaign runtime dependency did not pass')
        run('facade-configure', [host['cmake_path'], '-S', repo / 'native/objc3c', '-B', facade, '-G', 'Ninja',
            '-DCMAKE_CXX_COMPILER='+str(bootstrap / 'bin/clang++'), '-DCMAKE_BUILD_TYPE=Release',
            '-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld -Wl,--disable-new-dtags',
            '-DCMAKE_SHARED_LINKER_FLAGS=-fuse-ld=lld -Wl,--disable-new-dtags',
            '-DCMAKE_MODULE_LINKER_FLAGS=-fuse-ld=lld -Wl,--disable-new-dtags',
            '-DOBJC3C_CLANG_FOUNDATION_ROOT='+str(stage),
            '-DBUILD_TESTING=OFF',
            '-DOBJC3C_COMPILER_INSTRUMENTATION_PROFILE=compiler-asan-ubsan'])
        run('facade-build', [host['cmake_path'], '--build', facade, '--target', 'objc3c', '--parallel', str(jobs)])
        ir = qualify_ir(repo, output / 'ir', runtime=runtime / 'libobjc3_runtime_services.a',
            objc_runtime=Path(dependency['stage']) / 'lib/libobjc.so',
            facade=facade / 'src/clang_foundation/objc3c', tools_receipt=tools_receipt)
        debugger = qualify_debugger(repo, output / 'debugger', jobs=jobs,
                                    tools_receipt=output / 'ir/tools.json')
        optimized = qualify_debug(repo, output / 'optimized-debug',
            debugger_receipt=output / 'debugger/qualification.json', ir_receipt=output / 'ir/qualification.json')
        report['executions'] = verify_results(ir, debugger, optimized)
        if tools_receipt is not None and digest(tools_receipt) != receipt_hash:
            raise ValueError('LLVM development tools receipt changed during the campaign')
        if verify_compiler_graph:
            from .compiler_instrumentation import qualify as qualify_compiler_instrumentation
            output_graph = output / 'instrumentation'
            graph = qualify_compiler_instrumentation(repo, output_graph, jobs=jobs)
            if (graph.get('status') != 'PASS' or graph.get('source_build_id') != foundation['source_build_id'] or
                    graph.get('build_config_digest') != foundation['build_config_digest']):
                raise ValueError('compiler instrumentation differs from the LLVM campaign foundation')
            report['compiler_instrumentation'] = {'status': graph['status'],
                'sha256': digest(output_graph / 'qualification.json')}
        if digest(Path(__file__)) != report['owner_sha256']:
            raise ValueError('LLVM campaign owner changed during execution')
        if subprocess.check_output(['git', '-C', str(repo), 'rev-parse', 'HEAD'], text=True).strip() != revision:
            raise ValueError('LLVM campaign source revision changed during execution')
        report['status'] = 'PASS'
    except BaseException as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        write_json(output / 'qualification.json', report)
    return report


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--jobs', type=int, default=3)
    parser.add_argument('--fetch', action='store_true')
    parser.add_argument('--prepared', action='store_true', help='admit an already built/tested/staged matching foundation')
    parser.add_argument('--tools-receipt', type=Path, help='reuse matching opt, llvm-dis and llvm-reduce objects')
    parser.add_argument('--verify-compiler-graph', action='store_true',
                        help='qualify compiler instrumentation using the same foundation build')
    args = parser.parse_args(argv)
    report = qualify(**vars(args))
    print(json.dumps({'status': report['status'], 'executions': len(report['executions'])}))


if __name__ == '__main__':
    main()
