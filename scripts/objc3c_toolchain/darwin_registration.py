"""Build and qualify the experimental Apple Silicon Objective-C 3 runtime registration."""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import shutil
import signal
import subprocess
import sys
import tempfile

from .core import ToolchainError, prepare
from .darwin_bootstrap import DEPLOYMENT_TARGET, ROOT, require_apple_silicon, verify_macho


PROFILE_DIR = ROOT / 'toolchains/llvm/experimental'
FIXTURES = ROOT / 'tests/clang_foundation/darwin_registration'
RUNTIME = ROOT / 'native/objc3c/src/runtime/platform/darwin_registration.c'
NATIVE = ROOT / 'native/objc3c'
PORTABLE_SERVICES = {
    'stdlib-map-count': ('collections_service', 'stdlib_map_count_test'),
    'text-scalar-boundary': ('text_service', 'text_scalar_boundary_test'),
}
PORTABLE_HEADERS = (
    'src/runtime/public/objc3_collections.h', 'src/runtime/public/objc3_stdlib_status.h',
    'src/runtime/public/objc3_text.h', 'src/runtime/stdlib/opaque_service_storage.h',
    'src/runtime/stdlib/stdlib_service_contract.h', 'src/runtime/stdlib/text_utf8_decode.h',
    'tests/test_assertion_policy.h',
)
PROFILE_NAME = 'macos-arm64-registration-experimental'


def file_digest(path: Path) -> str:
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def load_profile() -> tuple[dict, Path]:
    profile = json.loads((PROFILE_DIR / 'darwin-registration.json').read_bytes())
    patch = PROFILE_DIR / 'darwin-registration.patch'
    if (profile.get('schema') != 'objc3c.experimental-darwin-registration.v1' or
            profile.get('profile') != PROFILE_NAME or
            profile.get('target') != 'arm64-apple-macos' + DEPLOYMENT_TARGET or
            profile.get('compiler_runtime_flag') != '-fobjc-runtime=gnustep-2.99' or
            profile.get('runtime_abi') != 'objc3-darwin-registration-v1' or
            file_digest(patch) != profile.get('patch_sha256')):
        raise ValueError('experimental Darwin registration profile or patch differs from its pin')
    return profile, patch


def compile_jobs(cpu_count: int, memory_bytes: int) -> int:
    gib = 1024 ** 3
    return max(1, min(cpu_count, (memory_bytes - 2 * gib) // (2 * gib)))


def qualify(bootstrap_report: Path, output: Path, *, fetch: bool) -> dict:
    require_apple_silicon()
    profile, patch = load_profile()
    output = output.resolve()
    output.mkdir(parents=True)
    report = {'schema': 'objc3c.darwin-registration-qualification.v1', 'status': 'FAIL',
              'support_truth': False, 'profile': profile, 'commands': []}
    env = {key: value for key, value in os.environ.items()
           if not key.startswith('DYLD_') and key not in ('SDKROOT', 'MACOSX_DEPLOYMENT_TARGET')}
    env['LC_ALL'] = 'C'

    def run(*argv, label=None, rejects=None, rejection_status=(1, 2), timeout=60) -> str:
        command = list(map(str, argv))
        index = len(report['commands'])
        log = output / f'{index:03d}-{label or Path(command[0]).name}.log'
        row = {'argv': command, 'log': log.name}
        report['commands'].append(row)
        with log.open('w') as stream:
            result = subprocess.run(command, env=env, stdout=stream, stderr=subprocess.STDOUT, timeout=timeout)
        row.update(exit_status=result.returncode, log_sha256=file_digest(log))
        transcript = log.read_text(errors='replace')
        if rejects is None and result.returncode:
            print('\n'.join(transcript.splitlines()[-30:]), file=sys.stderr)
            raise ValueError(f'{label or command[0]} exited {result.returncode}; see {log}')
        if rejects is not None:
            if result.returncode not in rejection_status or not all(marker in transcript for marker in rejects):
                raise ValueError(f'{label or command[0]} lacked the expected rejection; see {log}')
            row['expected_diagnostic'] = rejects
        return transcript.strip()

    try:
        host = json.loads(bootstrap_report.read_bytes())
        revision = run('/usr/bin/git', '-C', ROOT, 'rev-parse', 'HEAD')
        if (host.get('schema') != 'objc3c.darwin-bootstrap-probe.v1' or
                host.get('status') != 'bootstrap-verified' or host.get('support_truth') is not False or
                host.get('checkout_revision') != revision or host['platform_identity']['arch'] != 'arm64' or
                host['platform_identity']['version'] != platform.mac_ver()[0] or
                host['platform_identity']['developer_dir'] != run('/usr/bin/xcode-select', '-p') or
                host['platform_identity']['sdk_path'] != run('/usr/bin/xcrun', '--sdk', 'macosx', '--show-sdk-path')):
            raise ValueError('experimental Darwin runtime requires a bootstrap receipt from this checkout and host')
        sdk = host['platform_identity']['sdk_path']
        linker = host['apple_toolchain']['paths']['ld']
        llvm_root = Path(host['llvm_identity']['root'])
        stage0_c = llvm_root / 'bin/clang'
        stage0_cxx = llvm_root / 'bin/clang++'
        pin = json.loads((ROOT / 'toolchains/llvm/manifest.json').read_bytes())['upstream']
        for driver in (stage0_c, stage0_cxx):
            if not driver.resolve(strict=True).is_relative_to(llvm_root):
                raise ValueError(f'Darwin bootstrap compiler escaped its toolchain root: {driver}')
            if 'arm64' not in run(host['apple_toolchain']['paths']['lipo'], '-archs', driver).split():
                raise ValueError(f'Darwin bootstrap compiler lacks a native arm64 slice: {driver}')
            if pin['commit'] not in run(driver, '--version'):
                raise ValueError(f'Darwin bootstrap compiler differs from the pinned revision: {driver}')
        report.update(revision=revision, bootstrap_sha256=file_digest(bootstrap_report),
                      platform_identity=host['platform_identity'], llvm_identity=host['llvm_identity'],
                      runtime_source_sha256=file_digest(RUNTIME),
                      fixtures={path.name: file_digest(path) for path in sorted(FIXTURES.iterdir())})
        memory = int(run('/usr/sbin/sysctl', '-n', 'hw.memsize'))
        cpus = os.cpu_count() or 1
        jobs = compile_jobs(cpus, memory)
        report['build_resources'] = {'logical_cpus': cpus, 'physical_memory_bytes': memory,
                                     'compile_jobs': jobs, 'link_jobs': 1}
        cmake, ninja = (shutil.which(name) for name in ('cmake', 'ninja'))
        if not cmake or not ninja:
            raise ValueError('the experimental Darwin compiler build requires CMake and Ninja on PATH')
        run(cmake, '--version')
        run(ninja, '--version')
        build_root = ROOT / 'out/toolchains'
        build_root.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(prefix='darwin-registration-', dir=build_root) as scratch:
            source = Path(scratch) / 'src'
            build = Path(scratch) / 'build'
            print('Preparing the pinned Darwin LLVM source', flush=True)
            base = prepare(ROOT, source_dir=source, fetch=fetch)
            run('/usr/bin/git', '-C', source, 'apply', '--check', patch, label='check-patch')
            run('/usr/bin/git', '-C', source, 'apply', patch, label='apply-patch')
            patch_digest = hashlib.sha256(bytes.fromhex(base['patch_digest']) +
                                         bytes.fromhex(profile['patch_sha256'])).hexdigest()
            configure = [
                '-G', 'Ninja', f'-DCMAKE_MAKE_PROGRAM={ninja}', '-DCMAKE_BUILD_TYPE=Release',
                f'-DCMAKE_C_COMPILER={stage0_c}', f'-DCMAKE_CXX_COMPILER={stage0_cxx}',
                '-DCMAKE_CXX_FLAGS_RELEASE=-O1 -DNDEBUG', f'-DCMAKE_OSX_SYSROOT={sdk}',
                '-DCMAKE_OSX_ARCHITECTURES=arm64', f'-DCMAKE_OSX_DEPLOYMENT_TARGET={DEPLOYMENT_TARGET}',
                '-DLLVM_TARGETS_TO_BUILD=AArch64', '-DLLVM_ENABLE_PROJECTS=clang',
                f'-DLLVM_DEFAULT_TARGET_TRIPLE={profile["target"]}',
                f'-DLLVM_PARALLEL_COMPILE_JOBS={jobs}', '-DLLVM_PARALLEL_LINK_JOBS=1',
                '-DLLVM_INCLUDE_TESTS=OFF', '-DLLVM_INCLUDE_BENCHMARKS=OFF',
                '-DLLVM_ENABLE_ASSERTIONS=ON', '-DLLVM_ENABLE_ZSTD=OFF', '-DLLVM_ENABLE_LIBXML2=OFF',
                f'-DPython3_EXECUTABLE={sys.executable}', f'-DCLANG_OBJC3_PATCH_DIGEST={patch_digest}',
            ]
            config_digest = hashlib.sha256(json.dumps(configure).encode()).hexdigest()
            report['compiler_build'] = {'base_build_id': base['build_id'], 'patch_digest': patch_digest,
                                        'configuration_sha256': config_digest}
            run(cmake, '-S', source / 'llvm', '-B', build, *configure,
                f'-DCLANG_OBJC3_CONFIG_DIGEST={config_digest}', label='configure', timeout=300)
            print(f'Building a native arm64 Clang with {jobs} compile job(s)', flush=True)
            run(cmake, '--build', build, '--parallel', jobs, '--target', 'clang', 'clang-resource-headers',
                label='build', timeout=180 * 60)
            clang = build / 'bin/clang'
            report['compiler_build']['artifact'] = verify_macho(clang, 'executable')
            version = run(clang, '--version', label='patched-clang')
            if (f'ObjC3-patches={patch_digest}' not in version or f'ObjC3-config={config_digest}' not in version or
                    pin['commit'] not in version):
                raise ValueError('native patched Clang did not report the Darwin compiler source/configuration identity')
            _exercise(run, clang, llvm_root, sdk, linker, output, report)
        report['status'] = 'registration-verified'
    except (OSError, KeyError, ValueError, ToolchainError, subprocess.SubprocessError) as error:
        report['error'] = str(error)
        raise
    finally:
        (output / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    return report


def _exercise(run, clang: Path, llvm_root: Path, sdk: str, linker: str, output: Path, report: dict) -> None:
    profile = report['profile']
    target = '--target=' + profile['target']
    resource = Path(run(llvm_root / 'bin/clang', '-print-resource-dir')).resolve(strict=True)
    builtins = resource / 'lib/darwin/libclang_rt.osx.a'
    if not resource.is_relative_to(llvm_root) or not builtins.is_file():
        raise ValueError('Darwin bootstrap is missing its compiler-rt builtins archive')
    report['compiler_builtins'] = {'path': str(builtins), 'sha256': file_digest(builtins)}
    defaults = [clang, '--no-default-config', target, '-isysroot', sdk, '-resource-dir', resource]
    objective_c = [*defaults, '-fobjc-version=3', profile['compiler_runtime_flag'],
                   '-fno-objc-arc', '-fno-objc-exceptions']
    link = [*defaults, '--ld-path=' + linker, '-Wl,-dead_strip']
    otool = '/usr/bin/otool'
    ar = llvm_root / 'bin/llvm-ar'
    dwarf = llvm_root / 'bin/llvm-dwarfdump'
    stages = output / 'build'
    stages.mkdir()
    layout = output / 'Install candidate'
    (layout / 'bin').mkdir(parents=True)
    (layout / 'lib').mkdir()
    runtime_object = stages / 'runtime.o'
    runtime_archive = layout / 'lib/libobjc3darwin.a'
    runtime_dylib = layout / 'lib/libobjc3darwin.dylib'
    fixture_objects = {name: stages / (name + '.o') for name in ('provider', 'main', 'late')}
    fixture_archive = layout / 'lib/libprovider.a'
    late_dylib = layout / 'lib/liblate.dylib'
    static = layout / 'bin/static'
    dynamic = layout / 'bin/dynamic'
    run(*defaults, '-std=c17', '-Wall', '-Wextra', '-Werror', '-O2', '-g', '-c', RUNTIME, '-o', runtime_object)
    for name, obj in fixture_objects.items():
        run(*objective_c, '-Wall', '-Wextra', '-Werror', '-O2', '-g', '-c', FIXTURES / (name + '.m'), '-o', obj)
    run(ar, '--format=darwin', 'rcs', fixture_archive, fixture_objects['provider'])
    run(ar, '--format=darwin', 'rcs', runtime_archive, runtime_object)
    run(*link, '-dynamiclib', runtime_object, '-Wl,-install_name,@rpath/libobjc3darwin.dylib', '-o', runtime_dylib)
    run(*link, '-dynamiclib', fixture_objects['late'], runtime_dylib,
        '-Wl,-install_name,@rpath/liblate.dylib', '-Wl,-rpath,@loader_path', '-o', late_dylib)
    run(*link, fixture_objects['main'], fixture_archive, runtime_archive, '-o', static)
    run(*link, fixture_objects['main'], fixture_archive, runtime_dylib, '-Wl,-rpath,@loader_path/../lib', '-o', dynamic)
    run(*link, fixture_objects['main'], fixture_archive, '-o', stages / 'no-runtime',
        rejects=('objc3_darwin_lookup_v1', 'objc3_darwin_register_v1',
                 'objc3_darwin_allocate_v1', 'objc3_darwin_deallocate_v1'), label='missing-runtime-at-link')
    unsupported = FIXTURES / 'unsupported.m'
    run(*objective_c, '-c', unsupported, '-o', stages / 'unsupported.o',
        rejects=('experimental Darwin registration requires a stateless root class',), label='unsupported-subclass')
    for name, diagnostic in (('layout', 'a stateless root class'), ('signature', 'ordinary no-argument int methods')):
        fixture = FIXTURES / f'unsupported_instance_{name}.m'
        run(*objective_c, '-c', fixture, '-o', stages / f'unsupported-instance-{name}.o',
            rejects=(f'experimental Darwin registration requires {diagnostic}',), label=f'unsupported-instance-{name}')
    run(*objective_c, '-fobjc-arc', '-c', FIXTURES / 'provider.m', '-o', stages / 'arc.o',
        rejects=(profile['compiler_runtime_flag'], '-fobjc-arc'), label='unsupported-arc')
    run(*objective_c, '-fobjc-link-runtime', '-c', FIXTURES / 'provider.m', '-o', stages / 'apple-runtime.o',
        rejects=(profile['compiler_runtime_flag'], '-fobjc-link-runtime'), label='unsupported-apple-runtime')
    run(*objective_c, '--target=x86_64-apple-macos15.0', '-c', FIXTURES / 'provider.m', '-o', stages / 'x86.o',
        rejects=(profile['compiler_runtime_flag'], 'x86_64'), label='unsupported-x86')
    run(*objective_c, '-fobjc-runtime=gnustep-2.2', '-c', FIXTURES / 'provider.m', '-o', stages / 'legacy.o',
        rejects=('GNUstep Objective-C runtime version 2 incompatible with target binary format',), label='legacy-runtime')
    run(dwarf, '--verify', fixture_objects['provider'])
    ir = stages / 'provider.ll'
    run(*objective_c, '-O0', '-S', '-emit-llvm', FIXTURES / 'provider.m', '-o', ir)
    metadata = ir.read_text()
    if (not all(name in metadata for name in ('@llvm.global_ctors', '@objc3_darwin_register_v1', '@objc3_darwin_lookup_v1')) or
            re.search(r'@__objc_load\b|@__start___objc_|\bcomdat\b', metadata)):
        raise ValueError('the provider uses an unexpected registration lowering')

    layout = layout.rename(output / 'Relocated install')
    artifacts = {name: verify_macho(obj, 'object') for name, obj in fixture_objects.items()}
    for name, members in (('libprovider.a', ['provider.o']), ('libobjc3darwin.a', ['runtime.o'])):
        archive = layout / 'lib' / name
        if run(ar, 't', archive).splitlines() != members:
            raise ValueError(f'unexpected members in Darwin archive {archive}')
        artifacts[name] = {'path': str(archive), 'format': 'Darwin ar', 'members': members, 'sha256': file_digest(archive)}
    required_libraries = {'bin/static': [], 'bin/dynamic': ['@rpath/libobjc3darwin.dylib'],
                          'lib/libobjc3darwin.dylib': ['@rpath/libobjc3darwin.dylib'],
                          'lib/liblate.dylib': ['@rpath/liblate.dylib', '@rpath/libobjc3darwin.dylib']}
    for relative, library_paths in required_libraries.items():
        path = layout / relative
        kind = 'executable' if relative.startswith('bin/') else 'dynamic_library'
        artifacts[relative] = verify_macho(path, kind)
        dependencies = [line.strip().split(' (', 1)[0] for line in run(otool, '-L', path).splitlines()[1:]]
        if sorted(dependencies) != sorted([*library_paths, '/usr/lib/libSystem.B.dylib']):
            raise ValueError(f'unexpected owned-runtime link dependencies of {path}: {dependencies}')
        artifacts[relative]['load_paths'] = dependencies
    expected = 'owned Objective-C 3 Darwin registration: PASS\nowned Objective-C 3 Darwin instances: PASS'
    if run(layout / 'bin/static') != expected or run(layout / 'bin/dynamic', layout / 'lib/liblate.dylib') != expected:
        raise ValueError('the registered native consumers did not return the runtime success witness')
    library = layout / 'lib/libobjc3darwin.dylib'
    hidden = library.with_suffix('.absent')
    library.rename(hidden)
    try:
        run(layout / 'bin/dynamic', rejects=('Library not loaded', 'libobjc3darwin.dylib'),
            rejection_status=(1, -signal.SIGABRT), label='missing-runtime-at-load')
    finally:
        hidden.rename(library)
    report.update(artifacts=artifacts, installation_root=str(layout))
    report['portable_services'] = _exercise_portable_services(run, defaults, linker, stages, layout)


def _exercise_portable_services(run, defaults, linker: str, stages: Path, layout: Path) -> dict:
    pairs = {label: (NATIVE / 'src/runtime/stdlib' / (service+'.cpp'),
                     NATIVE / 'tests' / (fixture+'.cpp'))
             for label, (service, fixture) in PORTABLE_SERVICES.items()}
    sources = {path for pair in pairs.values() for path in pair}
    sources.update(NATIVE / header for header in PORTABLE_HEADERS)
    inputs = {str(path.relative_to(ROOT)): file_digest(path) for path in sorted(sources)}
    cxx = [*defaults, '--driver-mode=g++', '-stdlib=libc++']
    compile_options = ['-std=c++20', '-O2', '-Wall', '-Wextra', '-Werror', '-I', NATIVE / 'src']
    portable = {}
    for label, (service, fixture) in pairs.items():
        runtime_object = stages / (service.stem+'.o')
        fixture_object = stages / (fixture.stem+'.o')
        binary = stages / label
        run(*cxx, *compile_options, '-c', service, '-o', runtime_object, label=label+'-service')
        run(*cxx, *compile_options, '-UNDEBUG', '-include', NATIVE / 'tests/test_assertion_policy.h',
            '-c', fixture, '-o', fixture_object, label=label+'-fixture')
        objects = {path.name: verify_macho(path, 'object') for path in (runtime_object, fixture_object)}
        run(*cxx, '--ld-path='+linker, '-Wl,-dead_strip', runtime_object, fixture_object,
            '-o', binary, label=label+'-link')
        installed = binary.rename(layout / 'bin' / label)
        artifact = verify_macho(installed, 'executable')
        libraries = [line.strip().split(' (', 1)[0]
                     for line in run('/usr/bin/otool', '-L', installed, label=label+'-load-paths').splitlines()[1:]]
        if sorted(libraries) != ['/usr/lib/libSystem.B.dylib', '/usr/lib/libc++.1.dylib']:
            raise ValueError(f'unexpected portable Darwin service dependencies of {installed}: {libraries}')
        run(installed, label=label+'-native')
        portable[label] = {'objects': objects, 'executable': {**artifact, 'load_paths': libraries}}
    if inputs != {str(path.relative_to(ROOT)): file_digest(path) for path in sorted(sources)}:
        raise ValueError('portable Darwin service inputs changed during qualification')
    return {'status': 'PASS', 'scope': 'Portable C++ services and public API regressions on Mach-O arm64.',
            'inputs': inputs, 'suites': portable}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--profile', choices=(PROFILE_NAME,), required=True)
    parser.add_argument('--bootstrap-report', type=Path, required=True)
    parser.add_argument('--output-dir', type=Path, required=True)
    parser.add_argument('--fetch', action='store_true', help='fetch the pinned upstream LLVM Git source')
    args = parser.parse_args()
    try:
        qualify(args.bootstrap_report, args.output_dir, fetch=args.fetch)
    except (OSError, ValueError, KeyError, ToolchainError, subprocess.SubprocessError) as error:
        parser.exit(2, f'objc3c Darwin registration: {error}\n')
    print(args.output_dir.resolve() / 'report.json')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
