"""Build and qualify the experimental Apple Silicon Objective-C 3 runtime registration."""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import shlex
import shutil
import signal
import subprocess
import sys
import tarfile
import tempfile

from .core import ToolchainError, planned_development_identity, prepare
from . import darwin_compiler_cache
from .darwin_bootstrap import DEPLOYMENT_TARGET, ROOT, require_apple_silicon, verify_macho


PROFILE_DIR = ROOT / 'toolchains/llvm/experimental'
FIXTURES = ROOT / 'tests/clang_foundation/darwin_registration'
RUNTIME = ROOT / 'native/objc3c/src/runtime/platform/darwin_registration.c'
NATIVE = ROOT / 'native/objc3c'
RUNTIME_HEADER = NATIVE / 'src/runtime/public/objc3_darwin.h'
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
TYPED_METHOD_WITNESS = 'owned Objective-C 3 Darwin typed methods: PASS'
TYPED_METHOD_SOURCES = ('typed_method_api.h', 'typed_method_base.m', 'typed_method_child.m', 'typed_method_main.m')
MANAGED_LAYOUT_WITNESS = 'owned Objective-C 3 Darwin managed layout: PASS'
MANAGED_LAYOUT_SOURCES = ('managed_layout_api.h', 'managed_layout_base.m', 'managed_layout_child.m', 'managed_layout_main.m')
MANAGED_LAYOUT_IMPORTS = {
    'lib/libmanaged-layout-child.dylib': {'objc3_darwin_super_dealloc_v1', 'objc3_darwin_arc_store_strong_v1'},
    'bin/managed-layout-dynamic': {'objc3_darwin_allocate_v1', 'objc3_darwin_object_class_v1',
                                   'objc3_darwin_instance_size_v1'},
    'bin/managed-layout-static': set(),
}
BLOCK_SOURCES = ('block_lifetime.m',)
BLOCK_WITNESS = 'owned Objective-C 3 Darwin block lifetime: PASS'
BLOCK_RUNTIME_IMPORTS = {
    '_Block_copy', '_Block_release', '_Block_object_assign', '_Block_object_dispose',
    '_NSConcreteStackBlock', '_NSConcreteGlobalBlock', '_NSConcreteMallocBlock',
}
BLOCK_CAPTURE_IMPORTS = {
    'objc3_darwin_arc_retain_block_v1', 'objc3_darwin_arc_copy_weak_v1',
    'objc3_darwin_arc_destroy_weak_v1', '_Block_object_assign', '_Block_object_dispose',
}
TYPED_ERROR_SOURCES = ('typed_error.m',)
TYPED_ERROR_WITNESS = 'owned Objective-C 3 Darwin typed errors: PASS'
TYPED_ERROR_IMPORTS = {
    'objc3_darwin_lookup_typed_v1', 'objc3_darwin_object_class_v1',
    'objc3_darwin_superclass_v1', 'objc3_darwin_arc_autorelease_v1',
}
CARRIER_DISPATCH_SOURCES = ('carrier_dispatch.m',)
CARRIER_DISPATCH_WITNESS = 'owned Objective-C 3 Darwin carrier dispatch: PASS'
CARRIER_DISPATCH_IMPORTS = {
    'objc3_darwin_lookup_typed_v1', 'objc3_darwin_arc_retain_v1',
    'objc3_darwin_arc_release_v1', 'objc3_darwin_arc_load_weak_retained_v1',
}
CARRIER_METHOD_DIAGNOSTIC = "ObjC3 carrier operation 'by-value call ABI' for type 'Optional(int)' has no qualified lowering"
TYPED_METHOD_IMPORTS = {
    'lib/libtyped-method-child.dylib': 'objc3_darwin_lookup_super_typed_v1',
    'bin/typed-method-dynamic': 'objc3_darwin_lookup_typed_v1',
    'bin/typed-method-arc': 'objc3_darwin_lookup_typed_v1',
    'bin/typed-method-static': None,
}
PROFILE_NAME = 'macos-arm64-registration-experimental'
# The ARC thread witness must complete its condition-variable handshake and join.
ARC_THREAD_WITNESS_TIMEOUT_SECONDS = 60


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
            profile.get('inheritance') != 'stateless-super-v1' or
            profile.get('layout') != 'fixed-root-int-v1' or
            file_digest(patch) != profile.get('patch_sha256')):
        raise ValueError('experimental Darwin registration profile or patch differs from its pin')
    return profile, patch


def compile_jobs(cpu_count: int, memory_bytes: int) -> int:
    gib = 1024 ** 3
    return max(1, min(cpu_count, (memory_bytes - 2 * gib) // (2 * gib)))


def xcode_cxx_headers(transcript: str, developer_dir: Path) -> Path:
    """Select the default libc++ include directory reported by Apple Clang."""
    developer_dir = developer_dir.resolve(strict=True)
    for line in transcript.splitlines():
        value = line.strip()
        if value.startswith('/') and value.endswith('/include/c++/v1'):
            candidate = Path(value).resolve(strict=True)
            if candidate.is_relative_to(developer_dir) and (candidate / '__config').is_file():
                return candidate
    raise ValueError('Apple Clang did not report libc++ headers from the selected Xcode or Command Line Tools')


def _qualify_xcode_cxx(run, host: dict, stage0_cxx: Path, target: str, output: Path) -> dict:
    sdk = Path(host['platform_identity']['sdk_path'])
    developer_dir = Path(host['platform_identity']['developer_dir'])
    linker = host['apple_toolchain']['paths']['ld']
    apple = Path(run('/usr/bin/xcrun', '--sdk', 'macosx', '--find', 'clang++', label='xcode-clangxx'))
    if not apple.resolve(strict=True).is_relative_to(developer_dir.resolve(strict=True)):
        raise ValueError(f'Apple Clang++ escaped the selected developer directory: {apple}')
    target_flags = ['--target=' + target, '-isysroot', str(sdk)]
    search = run(apple, *target_flags, '-stdlib=libc++', '-E', '-v', '-x', 'c++', '/dev/null', '-o', '/dev/null',
                 label='xcode-cxx-search')
    headers = xcode_cxx_headers(search, developer_dir)
    library_stub = sdk / 'usr/lib/libc++.tbd'
    if not library_stub.is_file():
        raise ValueError(f'the selected macOS SDK has no libc++ link stub: {library_stub}')
    # LLVM's archive also includes libc++ headers. The native runtime resolves
    # the Apple SDK dylib, so both native build stages must use Xcode headers.
    flags = ['-stdlib=libc++', '-nostdinc++', '-isystem', str(headers)]
    source = output / 'xcode-cxx-hash.cpp'
    source.write_text('''#include <cstdio>
#include <functional>
#include <string>
int main(int argc, char **argv) {
  if (argc != 3 || std::hash<std::string>{}(argv[1]) != std::hash<std::string>{}(argv[2]))
    return 1;
  std::printf("Xcode libc++ %d\\n", _LIBCPP_VERSION);
  return 0;
}
''')
    binary = output / 'xcode-cxx-hash'
    run(stage0_cxx, *target_flags, *flags, '--ld-path=' + linker, '-std=c++17', '-O1', source, '-o', binary,
        label='xcode-libcxx-hash-link')
    artifact = verify_macho(binary, 'executable')
    libraries = [line.strip().split(' (', 1)[0]
                 for line in run('/usr/bin/otool', '-L', binary, label='xcode-libcxx-hash-load-paths').splitlines()[1:]]
    if sorted(libraries) != ['/usr/lib/libSystem.B.dylib', '/usr/lib/libc++.1.dylib']:
        raise ValueError(f'unexpected Xcode libc++ probe dependencies: {libraries}')
    version = run(binary, 'stable-hash-input', 'stable-hash-input', label='xcode-libcxx-hash-native')
    if not re.fullmatch(r'Xcode libc\+\+ [0-9]+', version):
        raise ValueError('the native libc++ hash probe did not report its header identity')
    return {'headers': str(headers), 'header_config_sha256': file_digest(headers / '__config'), 'flags': flags,
            'library_stub': str(library_stub), 'library_stub_sha256': file_digest(library_stub),
            'reported_version': version, 'hash_probe': {**artifact, 'load_paths': libraries}}


def registration_declaration_command(compiler: Path | str, *target_options: str) -> list[str | Path]:
    # This entry point uses ordinary Objective-C syntax. It can check the SDK
    # declarations with stock Clang before building the experimental frontend.
    return [compiler, '--no-default-config', *target_options, '-std=c17',
            '-Wall', '-Wextra', '-Werror', '-fsyntax-only', '-I', RUNTIME_HEADER.parent,
            FIXTURES / 'main.m']


def qualify(bootstrap_report: Path, output: Path, *, fetch: bool, package_sdk: bool = False,
            sdk_sanitizers: bool = False, compiler_cache: Path | None = None) -> dict:
    if sdk_sanitizers and not package_sdk:
        raise ValueError('--sdk-sanitizers requires --package-sdk')
    require_apple_silicon()
    profile, patch = load_profile()
    output = output.resolve()
    output.mkdir(parents=True)
    report = {'schema': 'objc3c.darwin-registration-qualification.v1', 'status': 'FAIL',
              'support_truth': False, 'profile': profile, 'commands': []}
    compiler_overrides = ('SDKROOT', 'MACOSX_DEPLOYMENT_TARGET', 'CPATH', 'C_INCLUDE_PATH',
                          'CPLUS_INCLUDE_PATH', 'OBJC_INCLUDE_PATH', 'LIBRARY_PATH',
                          'CPPFLAGS', 'CFLAGS', 'CXXFLAGS', 'LDFLAGS')
    env = {key: value for key, value in os.environ.items()
           if not key.startswith('DYLD_') and key not in compiler_overrides}
    env['LC_ALL'] = 'C'

    def run(*argv, label=None, rejects=None, rejection_status=(1, 2), timeout=None) -> str:
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
                      runtime_source_sha256=file_digest(RUNTIME), runtime_header_sha256=file_digest(RUNTIME_HEADER),
                      fixtures={path.name: file_digest(path) for path in sorted(FIXTURES.iterdir())})
        run(*registration_declaration_command(stage0_c, '--target='+profile['target'], '-isysroot', sdk),
            label='registration-api-declarations')
        report['cxx_toolchain'] = _qualify_xcode_cxx(run, host, stage0_cxx, profile['target'], output)
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
            base = planned_development_identity(ROOT)
            patch_digest = hashlib.sha256(bytes.fromhex(base['patch_digest']) +
                                         bytes.fromhex(profile['patch_sha256'])).hexdigest()
            cxx_flags = shlex.join(report['cxx_toolchain']['flags'])
            linker_flag = shlex.quote('--ld-path=' + linker)
            configure = [
                '-G', 'Ninja', f'-DCMAKE_MAKE_PROGRAM={ninja}', '-DCMAKE_BUILD_TYPE=Release',
                f'-DCMAKE_C_COMPILER={stage0_c}', f'-DCMAKE_CXX_COMPILER={stage0_cxx}',
                f'-DCMAKE_CXX_FLAGS={cxx_flags}', f'-DCMAKE_EXE_LINKER_FLAGS={linker_flag}',
                f'-DCMAKE_SHARED_LINKER_FLAGS={linker_flag}', f'-DCMAKE_MODULE_LINKER_FLAGS={linker_flag}',
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
            inputs = {
                **report['compiler_build'], 'configure': configure,
                'upstream': pin['commit'], 'xcode': host['apple_toolchain']['developer_tools_version'],
                'sdk_settings_sha256': file_digest(Path(sdk) / 'SDKSettings.json'),
                'tool_files': {str(tool): file_digest(Path(tool))
                               for tool in (cmake, ninja, stage0_c, stage0_cxx, linker, sys.executable)},
                'libcxx': {key: report['cxx_toolchain'][key] for key in
                           ('header_config_sha256', 'library_stub_sha256', 'reported_version')},
            }
            cached = darwin_compiler_cache.restore(compiler_cache, inputs)
            report['compiler_build']['cache_hit'] = cached is not None
            if cached:
                print('Reusing the verified native Darwin compiler products', flush=True)
                clang, source = cached / 'bin/clang', cached
            else:
                print('Preparing the pinned Darwin LLVM source', flush=True)
                actual = prepare(ROOT, source_dir=source, fetch=fetch)
                if actual['build_id'] != base['build_id'] or actual['patch_digest'] != base['patch_digest']:
                    raise ValueError('prepared Darwin compiler inputs differ from the build plan')
                run('/usr/bin/git', '-C', source, 'apply', '--check', patch, label='check-patch')
                run('/usr/bin/git', '-C', source, 'apply', patch, label='apply-patch')
                run(cmake, '-S', source / 'llvm', '-B', build, *configure,
                    f'-DCLANG_OBJC3_CONFIG_DIGEST={config_digest}', label='configure')
                print(f'Building a native arm64 Clang with {jobs} compile job(s)', flush=True)
                run(cmake, '--build', build, '--parallel', jobs, '--target', 'clang', 'clang-resource-headers',
                    label='build')
                clang = build / 'bin/clang'
            report['compiler_build']['artifact'] = verify_macho(clang, 'executable')
            version = run(clang, '--version', label='patched-clang')
            if (f'ObjC3-patches={patch_digest}' not in version or f'ObjC3-config={config_digest}' not in version or
                    pin['commit'] not in version):
                raise ValueError('native patched Clang did not report the Darwin compiler source/configuration identity')
            if compiler_cache is not None and cached is None:
                darwin_compiler_cache.save(compiler_cache, inputs, clang, source)
            _exercise(run, clang, llvm_root, sdk, linker, output, report)
            if package_sdk:
                from . import darwin_sdk
                package_output = output / 'sdk/package'
                registration = {**report, 'status': 'registration-verified'}
                report['sdk_package'] = darwin_sdk.package_registration(
                    clang, source, bootstrap_report, registration, package_output, sdk_sanitizers=sdk_sanitizers)
        if package_sdk:
            report['sdk_qualification'] = darwin_sdk.qualify(
                package_output / darwin_sdk.ARCHIVE_NAME, report['sdk_package']['archive_sha256'],
                output / 'sdk/qualification')
        report['status'] = 'registration-verified'
    except (OSError, KeyError, ValueError, ToolchainError, subprocess.SubprocessError, tarfile.TarError) as error:
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
                   '-fno-objc-arc', '-fno-objc-exceptions', '-I', RUNTIME_HEADER.parent]
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
    hierarchy_witnesses = {
        'inheritance': 'owned Objective-C 3 Darwin inheritance: PASS',
        'fixed-layout': 'owned Objective-C 3 Darwin fixed root layout: PASS',
        'int-method': 'owned Objective-C 3 Darwin int-argument methods: PASS',
        'two-int-method': 'owned Objective-C 3 Darwin two-int-argument methods: PASS',
        'typed-method': TYPED_METHOD_WITNESS,
        'managed-layout': MANAGED_LAYOUT_WITNESS,
    }
    method_families = (
        ('int-method', 1, 'int_arg', 'int_argument_methods'),
        ('two-int-method', 2, 'two_int_args', 'two_int_argument_methods'),
    )
    names = ['provider', 'main', 'late', 'late_parent']
    for family in hierarchy_witnesses:
        prefix = family.replace('-', '_')
        names.extend(prefix + '_' + unit for unit in ('base', 'child', 'main'))
    fixture_objects = {name: stages / (name + '.o') for name in names}
    fixture_archive = layout / 'lib/libprovider.a'
    late_dylib = layout / 'lib/liblate.dylib'
    late_parent = layout / 'lib/liblate-parent.dylib'
    static = layout / 'bin/static'
    dynamic = layout / 'bin/dynamic'
    run(*defaults, '-std=c17', '-Wall', '-Wextra', '-Werror', '-O2', '-g', '-c', RUNTIME, '-o', runtime_object)
    for name, obj in fixture_objects.items():
        managed = name.startswith('managed_layout_')
        flags = ['-fobjc-arc'] if managed else []
        label = {'label': name.replace('_', '-') + '-object'} if managed or name.startswith('two_int_method_') else {}
        run(*objective_c, *flags, '-Wall', '-Wextra', '-Werror', '-O2', '-g',
            '-c', FIXTURES / (name + '.m'), '-o', obj, **label)
    layout_arc = stages / 'fixed_layout_arc.o'
    run(*objective_c, '-fobjc-arc', '-Wall', '-Wextra', '-Werror', '-O2', '-g',
        '-c', FIXTURES / 'fixed_layout_arc.m', '-o', layout_arc, label='fixed-layout-arc-object')
    fixture_objects['fixed_layout_arc'] = layout_arc
    run(ar, '--format=darwin', 'rcs', fixture_archive, fixture_objects['provider'])
    run(ar, '--format=darwin', 'rcs', runtime_archive, runtime_object)
    run(*link, '-dynamiclib', runtime_object, '-Wl,-install_name,@rpath/libobjc3darwin.dylib', '-o', runtime_dylib)
    run(*link, '-dynamiclib', fixture_objects['late_parent'], runtime_dylib,
        '-Wl,-install_name,@rpath/liblate-parent.dylib', '-Wl,-rpath,@loader_path', '-o', late_parent)
    run(*link, '-dynamiclib', fixture_objects['late'], late_parent, runtime_dylib,
        '-Wl,-install_name,@rpath/liblate.dylib', '-Wl,-rpath,@loader_path', '-o', late_dylib)
    run(*link, fixture_objects['main'], fixture_archive, runtime_archive, '-o', static)
    run(*link, fixture_objects['main'], fixture_archive, runtime_dylib, '-Wl,-rpath,@loader_path/../lib', '-o', dynamic)
    runtime_id = '@rpath/libobjc3darwin.dylib'
    required_libraries = {'bin/static': [], 'bin/dynamic': [runtime_id],
                          'lib/libobjc3darwin.dylib': [runtime_id],
                          'lib/liblate-parent.dylib': ['@rpath/liblate-parent.dylib', runtime_id],
                          'lib/liblate.dylib': ['@rpath/liblate.dylib', '@rpath/liblate-parent.dylib', runtime_id]}
    for family in hierarchy_witnesses:
        prefix = family.replace('-', '_')
        base = layout / f'lib/lib{family}-base.dylib'
        child = layout / f'lib/lib{family}-child.dylib'
        base_id = '@rpath/' + base.name
        child_id = '@rpath/' + child.name
        run(*link, '-dynamiclib', fixture_objects[prefix + '_base'], runtime_dylib,
            '-Wl,-install_name,' + base_id, '-Wl,-rpath,@loader_path', '-o', base)
        run(*link, '-dynamiclib', fixture_objects[prefix + '_child'], base, runtime_dylib,
            '-Wl,-install_name,' + child_id, '-Wl,-rpath,@loader_path', '-o', child)
        # Static links admit the child's constructor before the base's.
        run(*link, fixture_objects[prefix + '_main'], fixture_objects[prefix + '_child'],
            fixture_objects[prefix + '_base'], runtime_archive, '-o', layout / f'bin/{family}-static')
        extra_link = [base] if family == 'managed-layout' else []
        run(*link, fixture_objects[prefix + '_main'], child, *extra_link, runtime_dylib,
            '-Wl,-rpath,@loader_path/../lib', '-o', layout / f'bin/{family}-dynamic')
        executable_dependencies = [child_id, *([base_id] if extra_link else []), runtime_id]
        required_libraries.update({f'lib/{base.name}': [base_id, runtime_id],
                                   f'lib/{child.name}': [child_id, base_id, runtime_id],
                                   f'bin/{family}-static': [], f'bin/{family}-dynamic': executable_dependencies})
    fixed_child = layout / 'lib/libfixed-layout-child.dylib'
    run(*link, layout_arc, fixed_child, runtime_dylib,
        '-Wl,-rpath,@loader_path/../lib', '-o', layout / 'bin/fixed-layout-arc')
    required_libraries['bin/fixed-layout-arc'] = ['@rpath/' + fixed_child.name, runtime_id]
    arc_families = [family for family, _, _, _ in method_families] + ['typed-method']
    for family in arc_families:
        prefix = family.replace('-', '_') + '_main'
        main_arc = stages / (prefix + '_arc.o')
        child = layout / f'lib/lib{family}-child.dylib'
        run(*objective_c, '-fobjc-arc', '-Wall', '-Wextra', '-Werror', '-O2', '-g',
            '-c', FIXTURES / (prefix + '.m'), '-o', main_arc, label=family + '-arc-object')
        fixture_objects[prefix + '_arc'] = main_arc
        binary = f'bin/{family}-arc'
        run(*link, main_arc, child, runtime_dylib, '-Wl,-rpath,@loader_path/../lib', '-o', layout / binary)
        required_libraries[binary] = ['@rpath/' + child.name, runtime_id]
    run(*link, fixture_objects['main'], fixture_archive, '-o', stages / 'no-runtime',
        rejects=('objc3_darwin_lookup_v1', 'objc3_darwin_register_v1',
                 'objc3_darwin_allocate_v1', 'objc3_darwin_deallocate_v1'), label='missing-runtime-at-link')
    for fixture, label, diagnostic in (
        ('unsupported.m', 'unsupported-subclass-layout', 'a single plain int root ivar'),
        ('unsupported_instance_layout.m', 'unsupported-instance-layout', 'a single plain int root ivar'),
        ('unsupported_instance_signature.m', 'unsupported-instance-signature',
         'supported typed methods'),
    ):
        artifact = stages / (label + '.o')
        run(*objective_c, '-c', FIXTURES / fixture, '-o', artifact,
            rejects=(f'experimental Darwin registration requires {diagnostic}',), label=label)
        if artifact.exists():
            raise ValueError(f'rejected Darwin fixture emitted an object: {fixture}')
    run(*objective_c, '-DDARWIN_UNSUPPORTED_SEND=1',
        '-c', FIXTURES / 'unsupported_instance_signature.m',
        '-o', stages / 'unsupported-instance-send.o', label='unsupported-instance-send',
        rejects=('experimental Darwin registration requires a supported typed message',))
    for label, options in (
        ('unsupported-carrier-method', []),
        ('unsupported-carrier-send', ['-DDARWIN_UNSUPPORTED_SEND=1']),
    ):
        artifact = stages / (label + '.o')
        run(*objective_c, *options, '-c', FIXTURES / 'unsupported_carrier_method.m',
            '-o', artifact, label=label,
            rejects=(CARRIER_METHOD_DIAGNOSTIC,))
        if artifact.exists():
            raise ValueError(f'rejected Darwin carrier method emitted an object: {label}')
    run(*objective_c, '-fobjc-link-runtime', '-c', FIXTURES / 'provider.m', '-o', stages / 'apple-runtime.o',
        rejects=(profile['compiler_runtime_flag'], '-fobjc-link-runtime'), label='unsupported-apple-runtime')
    run(*objective_c, '--target=x86_64-apple-macos15.0', '-c', FIXTURES / 'provider.m', '-o', stages / 'x86.o',
        rejects=(profile['compiler_runtime_flag'], 'x86_64'), label='unsupported-x86')
    run(*objective_c, '-fobjc-runtime=gnustep-2.2', '-c', FIXTURES / 'provider.m', '-o', stages / 'legacy.o',
        rejects=('GNUstep Objective-C runtime version 2 incompatible with target binary format',), label='legacy-runtime')
    run(dwarf, '--verify', fixture_objects['provider'])
    run(dwarf, '--verify', fixture_objects['inheritance_child'])
    for kind in ('base', 'child'):
        obj = fixture_objects['fixed_layout_' + kind]
        class_name = 'DarwinFixed' + kind.title()
        run(dwarf, '--verify', obj, label=f'fixed-layout-{kind}-verify')
        transcript = run(dwarf, '--name=' + class_name, '--show-children', obj,
                         label=f'fixed-layout-{kind}-dwarf')
        verify_fixed_layout_dwarf(transcript, class_name)
    ir = stages / 'provider.ll'
    run(*objective_c, '-O0', '-S', '-emit-llvm', FIXTURES / 'provider.m', '-o', ir)
    metadata = ir.read_text()
    if (not all(name in metadata for name in ('@llvm.global_ctors', '@objc3_darwin_register_v1', '@objc3_darwin_lookup_v1')) or
            re.search(r'@__objc_load\b|@__start___objc_|\bcomdat\b', metadata)):
        raise ValueError('the provider uses an unexpected registration lowering')
    methods = {}
    for family, arity, abi_suffix, _ in method_families:
        prefix = family.replace('-', '_')
        method_ir = {}
        for optimization in ('O0', 'O2'):
            for unit, dispatch in (('child', 'lookup_super'), ('main', 'lookup')):
                label = f'{family}-{unit}-{optimization.lower()}-ir'
                artifact = stages / (label + '.ll')
                run(*objective_c, '-' + optimization, '-S', '-emit-llvm',
                    FIXTURES / f'{prefix}_{unit}.m', '-o', artifact, label=label)
                verify_int_method_ir(artifact.read_text(), f'objc3_darwin_{dispatch}_{abi_suffix}_v1', arity=arity)
                method_ir[label] = {'path': str(artifact), 'sha256': file_digest(artifact)}
        methods[family] = method_ir

    layout = layout.rename(output / 'Relocated install')
    artifacts = {name: verify_macho(obj, 'object') for name, obj in fixture_objects.items()}
    for name, members in (('libprovider.a', ['provider.o']), ('libobjc3darwin.a', ['runtime.o'])):
        archive = layout / 'lib' / name
        if run(ar, 't', archive).splitlines() != members:
            raise ValueError(f'unexpected members in Darwin archive {archive}')
        artifacts[name] = {'path': str(archive), 'format': 'Darwin ar', 'members': members, 'sha256': file_digest(archive)}
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
    for family, witness in hierarchy_witnesses.items():
        for route in ('static', 'dynamic'):
            label = f'{family}-{route}'
            if run(layout / 'bin' / label, label=label + '-native') != witness:
                raise ValueError(f'the native {label} consumer failed')
    if (run(layout / 'bin/fixed-layout-arc', label='fixed-layout-arc-native') !=
            'owned Objective-C 3 Darwin fixed root layout ARC lifetime: PASS'):
        raise ValueError('the native fixed-layout ARC consumer failed')
    for family, _, abi_suffix, receipt in method_families:
        if run(layout / f'bin/{family}-arc', label=family + '-arc-native') != hierarchy_witnesses[family]:
            raise ValueError(f'the native {family} ARC consumer failed')
        method_imports = {}
        lookup = f'objc3_darwin_lookup_{abi_suffix}_v1'
        for relative, required in (
            (f'lib/lib{family}-child.dylib', f'objc3_darwin_lookup_super_{abi_suffix}_v1'),
            (f'bin/{family}-dynamic', lookup),
            (f'bin/{family}-arc', lookup),
            (f'bin/{family}-static', None),
        ):
            names = _owned_objc_imports(run, layout / relative, Path(relative).name + '-imports')
            if (required and required not in names) or (required is None and any(
                    name.startswith('objc3_darwin_') for name in names)):
                raise ValueError(f'{relative} has inconsistent int-argument method linkage')
            method_imports[relative] = names
        report[receipt] = {'status': 'PASS', 'ir': methods[family], 'imports': method_imports}
    if run(layout / 'bin/typed-method-arc', label='typed-method-arc-native') != hierarchy_witnesses['typed-method']:
        raise ValueError('the native typed method ARC consumer failed')
    typed_imports = {}
    for relative, required in TYPED_METHOD_IMPORTS.items():
        names = _owned_objc_imports(run, layout / relative, Path(relative).name + '-imports')
        if (required is not None and required not in names) or (required is None and any(
                name.startswith('objc3_darwin_') for name in names)):
            raise ValueError(f'{relative} has inconsistent typed method linkage')
        typed_imports[relative] = names
    report['typed_methods'] = {'status': 'PASS', 'imports': typed_imports}
    managed_imports = {}
    for relative, required in MANAGED_LAYOUT_IMPORTS.items():
        names = _owned_objc_imports(run, layout / relative, Path(relative).name + '-imports')
        if not required <= set(names) or (not required and any(name.startswith('objc3_darwin_') for name in names)):
            raise ValueError(f'{relative} has inconsistent managed-layout runtime imports')
        managed_imports[relative] = names
    report['managed_layout'] = {'status': 'PASS', 'imports': managed_imports}
    for binary, filename, label in (
        ('dynamic', 'libobjc3darwin.dylib', 'missing-runtime-at-load'),
        ('inheritance-dynamic', 'libinheritance-base.dylib', 'missing-superclass-at-load'),
    ):
        library = layout / 'lib' / filename
        hidden = library.with_suffix('.absent')
        library.rename(hidden)
        try:
            run(layout / 'bin' / binary, rejects=('Library not loaded', filename),
                rejection_status=(1, -signal.SIGABRT), label=label)
        finally:
            hidden.rename(library)
    report.update(artifacts=artifacts, installation_root=str(layout))
    report['arc'] = _exercise_arc(run, defaults, profile['compiler_runtime_flag'],
                                  link, stages, layout, dwarf)
    report['blocks'] = _exercise_owned_program(
        run, defaults, profile['compiler_runtime_flag'], link, stages, layout,
        'blocks', BLOCK_SOURCES[0], BLOCK_WITNESS, BLOCK_CAPTURE_IMPORTS, extra_flags=['-fblocks'])
    report['typed_errors'] = _exercise_owned_program(
        run, defaults, profile['compiler_runtime_flag'], link, stages, layout,
        'typed-errors', TYPED_ERROR_SOURCES[0], TYPED_ERROR_WITNESS, TYPED_ERROR_IMPORTS)
    report['carrier_dispatch'] = _exercise_owned_program(
        run, defaults, profile['compiler_runtime_flag'], link, stages, layout,
        'carrier-dispatch', CARRIER_DISPATCH_SOURCES[0], CARRIER_DISPATCH_WITNESS, CARRIER_DISPATCH_IMPORTS)
    report['portable_services'] = _exercise_portable_services(
        run, defaults, linker, stages, layout, cxx_flags=report['cxx_toolchain']['flags'])


def verify_fixed_layout_dwarf(transcript: str, class_name: str) -> None:
    """Verify the class extent and the exact ivar or inherited DWARF offset."""
    records = re.split(r'(?m)^0x[0-9a-f]+:\s*', transcript)[1:]

    def attr(record, name, value):
        return re.search(rf'(?m)^\s*DW_AT_{name}\s*\({value}\)$', record)

    classes = [record for record in records if record.startswith('DW_TAG_structure_type\n') and
               attr(record, 'name', re.escape(f'"{class_name}"'))]
    if len(classes) != 1 or not attr(classes[0], 'byte_size', r'0x0*10'):
        raise ValueError(f'fixed Darwin layout DWARF has the wrong size for {class_name}')
    if class_name == 'DarwinFixedBase':
        members = [record for record in records if record.startswith('DW_TAG_member\n') and
                   attr(record, 'name', '"value"')]
        if (len(members) != 1 or not attr(members[0], 'type', r'0x[0-9a-f]+ "int"') or
                not attr(members[0], 'data_member_location', r'0x0*8')):
            raise ValueError('fixed Darwin layout DWARF has the wrong int ivar offset')
    else:
        inherited = [record for record in records if record.startswith('DW_TAG_inheritance\n')]
        if (len(inherited) != 1 or not attr(inherited[0], 'type', r'0x[0-9a-f]+ "DarwinFixedBase"') or
                not attr(inherited[0], 'data_member_location', r'0x0*')):
            raise ValueError('fixed Darwin layout DWARF lost the inherited base')


def verify_int_method_ir(ir: str, lookup: str, *, arity: int = 1) -> None:
    """Check that each versioned lookup result is called with the typed ABI."""
    encodings = {1: 'i20@0:8i16', 2: 'i24@0:8i16i20'}
    if arity not in encodings or encodings[arity] not in ir:
        raise ValueError('Darwin int argument method is missing its exact selector encoding')
    arguments = r'ptr [^,\n]+, ptr [^,\n]+' + r', i32 [^,)\n]+' * arity
    seen = 0
    for function in re.findall(r'(?ms)^define[^\n]*\n.*?^\}', ir):
        for name in re.findall(rf'(?m)^\s*(%[-\w.]+) = .*\bcall ptr @{re.escape(lookup)}\(', function):
            seen += 1
            if not re.search(rf'\bcall i32 {re.escape(name)}\({arguments}\)', function):
                raise ValueError('Darwin int argument method lookup lacks a matching indirect call')
    if seen < 2:
        raise ValueError(f'Darwin int argument method fixture did not exercise {lookup} twice')


def _owned_objc_imports(run, path: Path, label: str) -> list[str]:
    result = run('/usr/bin/nm', '-u', '-j', path, label=label)
    names = sorted({name.removeprefix('_') for name in result.splitlines()})
    if any(name.startswith(('objc_', '__objc_', '_Block_', '_NSConcrete')) and name not in BLOCK_RUNTIME_IMPORTS
           for name in names):
        raise ValueError(f'Darwin artifact has a foreign Objective-C runtime import: {path}')
    return names


def _exercise_arc(run, defaults, runtime_flag: str, link, stages: Path,
                  layout: Path, dwarf: Path) -> dict:
    objective_c = [*defaults, '-std=c17', '-fobjc-version=3', runtime_flag, '-fobjc-arc']
    source = FIXTURES / 'arc.m'
    threads = stages / 'arc-threads.o'
    run(*defaults, '-std=c17', '-Wall', '-Wextra', '-Werror', '-O2', '-g',
        '-c', FIXTURES / 'arc_threads.c', '-o', threads, label='arc-threads-object')
    for option in ('-fobjc-exceptions', '-fexceptions', '-fobjc-link-runtime'):
        diagnostic = runtime_flag if option == '-fobjc-link-runtime' else 'experimental Darwin ARC'
        run(*objective_c, option, '-c', source, '-o', stages / 'unsupported-arc.o',
            rejects=(diagnostic, option), label='unsupported-arc' + option)

    owned = {'objc3_darwin_arc_' + operation + '_v1' for operation in (
        'retain', 'release', 'init_weak', 'store_weak', 'load_weak_retained',
        'copy_weak', 'destroy_weak', 'pool_push', 'pool_pop', 'autorelease')}
    routes = {}
    for optimization in ('O0', 'O2'):
        label = 'arc-' + optimization.lower()
        required = owned | ({'objc3_darwin_arc_store_strong_v1'} if optimization == 'O0' else set())
        obj = stages / (label + '.o')
        ir = stages / (label + '.ll')
        run(*objective_c, '-' + optimization, '-Wall', '-Wextra', '-Werror',
            '-g', '-c', source, '-o', obj, label=label+'-object')
        run(*objective_c, '-' + optimization, '-S', '-emit-llvm', source,
            '-o', ir, label=label+'-ir')
        ir_text = ir.read_text()
        called = set(re.findall(r'\b(?:call|invoke)\b[^\n]*?@(objc3_darwin_arc_\w+)\(', ir_text))
        if (not required <= called or
                re.search(r'@(?:llvm\.objc\.(?!clang\.arc\.)|objc_)|clang\.arc\.attachedcall', ir_text)):
            raise ValueError(f'{label} did not lower directly to its owned ARC entry points')
        object_imports = _owned_objc_imports(run, obj, label+'-object-imports')
        if not required <= set(object_imports):
            raise ValueError(f'{label} Mach-O object is missing owned ARC imports')
        run(dwarf, '--verify', obj, label=label+'-dwarf')
        routes[optimization] = {'object': {**verify_macho(obj, 'object'), 'imports': object_imports},
                                'ir_sha256': file_digest(ir), 'canonical_calls': sorted(called)}
        for linkage, library in (('static', 'libobjc3darwin.a'), ('dynamic', 'libobjc3darwin.dylib')):
            label_route = label + '-' + linkage
            executable = layout / 'bin' / label_route
            rpath = ['-Wl,-rpath,@loader_path/../lib'] if linkage == 'dynamic' else []
            run(*link, obj, threads, layout / 'lib' / library, *rpath,
                '-o', executable, label=label_route+'-link')
            dependencies = [line.strip().split(' (', 1)[0] for line in
                            run('/usr/bin/otool', '-L', executable, label=label_route+'-load-paths')
                            .splitlines()[1:]]
            expected = ['/usr/lib/libSystem.B.dylib']
            if linkage == 'dynamic':
                expected.append('@rpath/libobjc3darwin.dylib')
            if sorted(dependencies) != sorted(expected):
                raise ValueError(f'{label_route} has an unexpected runtime dependency')
            undefined = _owned_objc_imports(run, executable, label_route+'-executable-imports')
            owned_imports = {name for name in undefined if name.startswith('objc3_darwin_')}
            if ((linkage == 'dynamic' and not required <= owned_imports) or
                    (linkage == 'static' and owned_imports)):
                raise ValueError(f'{label_route} has inconsistent owned runtime linkage')
            if run(executable, label=label_route+'-native',
                   timeout=ARC_THREAD_WITNESS_TIMEOUT_SECONDS) != 'owned Objective-C 3 Darwin ARC: PASS':
                raise ValueError(f'{label_route} failed the ARC lifetime witness')
            routes[optimization][linkage] = {**verify_macho(executable, 'executable'),
                                            'load_paths': dependencies, 'imports': undefined}
    return {'status': 'PASS', 'scope': 'Stateless ARC objects and thread-local autorelease pools.',
            'threads_object': verify_macho(threads, 'object'), 'routes': routes}


def _exercise_owned_program(run, defaults, runtime_flag, link, stages, layout,
                            name, source, witness, required, *, extra_flags=()) -> dict:
    flags = [*defaults, '-std=c17', '-fobjc-version=3', runtime_flag, '-fobjc-arc', *extra_flags,
             '-I', RUNTIME_HEADER.parent, '-Wall', '-Wextra', '-Werror']
    artifacts = {}
    for optimization in ('O0', 'O2'):
        label = f'{name}-{optimization.lower()}'
        obj = stages / (label + '.o')
        run(*flags, '-' + optimization, '-c', FIXTURES / source, '-o', obj, label=label + '-object')
        imports = _owned_objc_imports(run, obj, label + '-object-imports')
        if not required <= set(imports):
            raise ValueError(f'{label} omitted required owned ABI imports')
        artifacts[label] = {**verify_macho(obj, 'object'), 'imports': imports}
        for linkage, suffix in (('static', '.a'), ('dynamic', '.dylib')):
            route = label + '-' + linkage
            binary = layout / 'bin' / route
            library = layout / 'lib' / ('libobjc3darwin' + suffix)
            rpaths = ['-Wl,-rpath,@loader_path/../lib'] if linkage == 'dynamic' else []
            run(*link, obj, library, *rpaths, '-o', binary, label=route + '-link')
            imports = _owned_objc_imports(run, binary, route + '-imports')
            paths = [line.strip().split(' (', 1)[0] for line in
                     run('/usr/bin/otool', '-L', binary, label=route + '-load-paths').splitlines()[1:]]
            expected = ['/usr/lib/libSystem.B.dylib']
            if linkage == 'dynamic':
                expected.append('@rpath/' + library.name)
            if sorted(paths) != sorted(expected) or run(binary, label=route + '-native') != witness:
                raise ValueError(f'{route} failed its native execution witness')
            artifacts[route] = {**verify_macho(binary, 'executable'), 'imports': imports, 'load_paths': paths}
    return {'status': 'PASS', 'artifacts': artifacts}


def _exercise_portable_services(run, defaults, linker: str, stages: Path, layout: Path, *, cxx_flags: list[str]) -> dict:
    pairs = {label: (NATIVE / 'src/runtime/stdlib' / (service+'.cpp'),
                     NATIVE / 'tests' / (fixture+'.cpp'))
             for label, (service, fixture) in PORTABLE_SERVICES.items()}
    sources = {path for pair in pairs.values() for path in pair}
    sources.update(NATIVE / header for header in PORTABLE_HEADERS)
    inputs = {str(path.relative_to(ROOT)): file_digest(path) for path in sorted(sources)}
    cxx = [*defaults, '--driver-mode=g++', *cxx_flags]
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


def main(argv=None, *, prog=None, package=False) -> int:
    parser = argparse.ArgumentParser(description=__doc__, prog=prog)
    if package:
        parser.set_defaults(package_sdk=True)
    else:
        parser.add_argument('--profile', choices=(PROFILE_NAME,), required=True)
        parser.add_argument('--package-sdk', action='store_true', help='package the admitted build and qualify a relocated SDK')
    parser.add_argument('--bootstrap-report', type=Path, required=True)
    parser.add_argument('--output-dir', type=Path, required=True)
    parser.add_argument('--fetch', action='store_true', help='fetch the pinned upstream LLVM Git source')
    parser.add_argument('--sdk-sanitizers', action='store_true', help='qualify the optional SDK ASan and UBSan consumers')
    parser.add_argument('--compiler-cache', type=Path, help='reuse exact verified patched Clang products')
    args = parser.parse_args(argv)
    try:
        qualify(args.bootstrap_report, args.output_dir, fetch=args.fetch, package_sdk=args.package_sdk,
                sdk_sanitizers=args.sdk_sanitizers,
                **({'compiler_cache': args.compiler_cache} if args.compiler_cache is not None else {}))
    except (OSError, ValueError, KeyError, ToolchainError, subprocess.SubprocessError, tarfile.TarError) as error:
        parser.exit(2, f'objc3c Darwin registration: {error}\n')
    print(args.output_dir.resolve() / 'report.json')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
