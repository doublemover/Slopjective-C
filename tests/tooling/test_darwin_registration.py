from pathlib import Path
import shutil
import signal
import subprocess
import sys

import pytest

from scripts.objc3c_toolchain import darwin_registration


def test_experimental_profile_binds_separate_patch_and_native_job_budget():
    profile, patch = darwin_registration.load_profile()
    assert profile['profile'] == 'macos-arm64-registration-experimental'
    assert profile['inheritance'] == 'stateless-super-v1'
    assert patch.parent.name == 'experimental'
    assert patch.name not in (darwin_registration.ROOT / 'toolchains/llvm/patches/series').read_text()
    assert darwin_registration.compile_jobs(3, 7 * 1024 ** 3) == 2
    assert darwin_registration.compile_jobs(12, 24 * 1024 ** 3) == 11
    assert darwin_registration.compile_jobs(2, 64 * 1024 ** 3) == 2


def test_experimental_driver_rejects_other_hosts_before_build(monkeypatch, tmp_path):
    monkeypatch.setattr(darwin_registration.platform, 'machine', lambda: 'x86_64')
    with pytest.raises(ValueError, match='native arm64 Python'):
        darwin_registration.qualify(tmp_path / 'missing.json', tmp_path / 'output', fetch=False)
    assert not (tmp_path / 'output').exists()


def test_xcode_cxx_preflight_requires_selected_headers_and_sdk_libcxx(monkeypatch, tmp_path):
    developer = tmp_path / 'Xcode Developer'
    headers = developer / 'Toolchains/Selected/usr/include/c++/v1'
    bundled = tmp_path / 'LLVM Archive/include/c++/v1'
    apple = developer / 'Toolchains/Selected/usr/bin/clang++'
    sdk = developer / 'Platforms/MacOSX/SDKs/MacOSX.sdk'
    library_stub = sdk / 'usr/lib/libc++.tbd'
    for path in (headers / '__config', bundled / '__config', apple, library_stub):
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(path.name)
    with pytest.raises(ValueError, match=r'Apple Clang did not report libc\+\+ headers'):
        darwin_registration.xcode_cxx_headers(str(bundled), developer)

    commands = {}
    emitted = tmp_path / 'xcode-cxx-hash'
    def run(*args, label):
        commands[label] = list(map(str, args))
        if label == 'xcode-libcxx-hash-link':
            emitted.write_text('mock executable')
        return {
            'xcode-clangxx': str(apple),
            'xcode-cxx-search': f'#include <...> search starts here:\n {bundled}\n {headers}\nEnd of search list.',
            'xcode-libcxx-hash-load-paths': (
                f'{emitted}:\n /usr/lib/libc++.1.dylib (compatibility version 1.0.0)\n'
                ' /usr/lib/libSystem.B.dylib (compatibility version 1.0.0)'),
            'xcode-libcxx-hash-native': 'Xcode libc++ 190100',
        }.get(label, '')

    monkeypatch.setattr(darwin_registration, 'verify_macho', lambda path, kind: {'path': str(path), 'kind': kind})
    host = {'platform_identity': {'sdk_path': str(sdk), 'developer_dir': str(developer)},
            'apple_toolchain': {'paths': {'ld': '/xcode/ld'}}}
    result = darwin_registration._qualify_xcode_cxx(run, host, Path('/llvm/clang++'), 'arm64-apple-macos15.0', tmp_path)
    assert result['headers'] == str(headers)
    assert result['header_config_sha256'] == darwin_registration.file_digest(headers / '__config')
    assert result['library_stub_sha256'] == darwin_registration.file_digest(library_stub)
    assert result['flags'] == ['-stdlib=libc++', '-nostdinc++', '-isystem', str(headers)]
    assert all(flag in commands['xcode-libcxx-hash-link'] for flag in result['flags'])
    assert str(bundled) not in commands['xcode-libcxx-hash-link']
    assert commands['xcode-libcxx-hash-native'] == [str(emitted), 'stable-hash-input', 'stable-hash-input']
    assert 'std::hash<std::string>' in (tmp_path / 'xcode-cxx-hash.cpp').read_text()


@pytest.mark.parametrize('extra_library', ['', '/usr/lib/libobjc.A.dylib'])
def test_portable_services_link_current_sources_and_run_after_relocation(monkeypatch, tmp_path, extra_library):
    stages = tmp_path / 'build'
    layout = tmp_path / 'Relocated install'
    stages.mkdir()
    (layout / 'bin').mkdir(parents=True)
    commands = {}

    def run(*args, label):
        argv = list(map(str, args))
        commands[label] = argv
        if '-o' in argv:
            Path(argv[argv.index('-o')+1]).write_bytes(b'linked by mock compiler')
        if label.endswith('-load-paths'):
            paths = ['/usr/lib/libc++.1.dylib', '/usr/lib/libSystem.B.dylib']
            if extra_library:
                paths.append(extra_library)
            return argv[-1]+'\n'+'\n'.join(f'  {path} (compatibility version 1.0.0)' for path in paths)
        return ''

    def verify_macho(path, kind):
        assert path.is_file()
        return {'path': str(path), 'format': 'Mach-O arm64', 'kind': kind}

    monkeypatch.setattr(darwin_registration, 'verify_macho', verify_macho)
    defaults = ['/owned/clang', '--target=arm64-apple-macos15.0', '-isysroot', '/owned/SDK']
    cxx_flags = ['-stdlib=libc++', '-nostdinc++', '-isystem', '/xcode/include/c++/v1']
    if extra_library:
        with pytest.raises(ValueError, match='unexpected portable Darwin service dependencies'):
            darwin_registration._exercise_portable_services(run, defaults, '/owned/ld', stages, layout, cxx_flags=cxx_flags)
        assert 'stdlib-map-count-native' not in commands
        return
    report = darwin_registration._exercise_portable_services(run, defaults, '/owned/ld', stages, layout, cxx_flags=cxx_flags)
    assert report['status'] == 'PASS'
    assert set(report['suites']) == {'stdlib-map-count', 'text-scalar-boundary'}
    for label, unit, fixture in (
        ('stdlib-map-count', 'collections_service', 'stdlib_map_count_test'),
        ('text-scalar-boundary', 'text_service', 'text_scalar_boundary_test'),
    ):
        service = f'native/objc3c/src/runtime/stdlib/{unit}.cpp'
        test = f'native/objc3c/tests/{fixture}.cpp'
        for source in (service, test):
            assert report['inputs'][source] == darwin_registration.file_digest(darwin_registration.ROOT / source)
        assert str(darwin_registration.ROOT / service) in commands[label+'-service']
        assert str(darwin_registration.ROOT / test) in commands[label+'-fixture']
        assert '-UNDEBUG' in commands[label+'-fixture']
        assert str(stages / (unit+'.o')) in commands[label+'-link']
        assert str(stages / (fixture+'.o')) in commands[label+'-link']
        assert all('--driver-mode=g++' in commands[label+'-'+step] for step in ('service', 'fixture', 'link'))
        assert all(all(flag in commands[label+'-'+step] for flag in cxx_flags) for step in ('service', 'fixture', 'link'))
        assert commands[label+'-native'] == [str(layout / 'bin' / label)]


@pytest.mark.skipif(sys.platform != 'linux' or not shutil.which('cc'), reason='LP64 C registry control uses Linux cc')
def test_owned_registry_lookup_and_negative_metadata_controls(tmp_path):
    harness = tmp_path / 'harness.c'
    harness.write_text('''
#include "darwin_registration.c"
#include <sys/resource.h>

static int class_answer(const void *receiver, const struct selector_record *selector) {
  return receiver && selector ? 42 : -1;
}

static int instance_answer(const void *receiver, const struct selector_record *selector) {
  return receiver && selector ? 17 : -1;
}

static int child_answer(const void *receiver, const struct selector_record *selector) {
  return receiver && selector ? 19 : -1;
}

static const struct selector_record selector = {"answer", "i16@0:8"};
static const struct selector_record same_signature = {"answer", "i16@0:8"};
static const struct selector_record wrong_signature = {"answer", "i20@0:8i16"};
static const struct method_list class_methods = {
  NULL, 1, sizeof(struct method_record), {{class_answer, &selector, "i16@0:8"}}
};
static const struct method_list instance_methods = {
  NULL, 1, sizeof(struct method_record), {{instance_answer, &selector, "i16@0:8"}}
};
static const struct method_list child_methods = {
  NULL, 1, sizeof(struct method_record), {{child_answer, &selector, "i16@0:8"}}
};
static const struct method_list wrong_methods = {
  NULL, 1, sizeof(struct method_record), {{instance_answer, &wrong_signature, "i20@0:8i16"}}
};
static const struct class_record meta = {.flags = 1, .methods = &class_methods};
static const struct class_record root = {.isa = &meta, .name = "Root", .methods = &instance_methods};
static const struct class_record child_meta = {.flags = 1};
static const struct class_record child = {
  .isa = &child_meta, .superclass = &root, .name = "Child", .methods = &child_methods
};
static const struct class_record cyclic = {.isa = &meta, .superclass = &cyclic, .name = "Cyclic"};
static const struct class_record stateful = {.isa = &meta, .name = "Stateful", .instance_size = 8};
static const struct class_record wrong_methods_class = {.isa = &meta, .name = "Wrong", .methods = &wrong_methods};
int objc3_darwin_check_arc_threads(const void *cls);

int main(int argc, char **argv) {
  struct rlimit no_core = {0, 0};
  setrlimit(RLIMIT_CORE, &no_core);
  const char *control = argc > 1 ? argv[1] : "";
  if (strcmp(control, "unregistered") == 0)
    objc3_darwin_lookup_v1(&root, &selector);
  if (strcmp(control, "allocate-unregistered") == 0)
    objc3_darwin_allocate_v1(&root);
  const struct class_record *child_batch[] = {&child};
  objc3_darwin_register_v1(child_batch, 1);
  if (strcmp(control, "missing-superclass") == 0)
    objc3_darwin_class_v1("Child");
  if (strcmp(control, "cyclic-superclass") == 0) {
    const struct class_record *bad[] = {&cyclic};
    objc3_darwin_register_v1(bad, 1);
    objc3_darwin_allocate_v1(&cyclic);
  }
  const struct class_record *batch[] = {&root};
  objc3_darwin_register_v1(batch, 1);
  if (strcmp(control, "duplicate") == 0)
    objc3_darwin_register_v1(batch, 1);
  if (strcmp(control, "deallocate-class") == 0)
    objc3_darwin_deallocate_v1((void *)&root);
  if (strcmp(control, "stateful") == 0) {
    const struct class_record *bad[] = {&stateful};
    objc3_darwin_register_v1(bad, 1);
  }
  if (strcmp(control, "wrong-method-table") == 0) {
    const struct class_record *bad[] = {&wrong_methods_class};
    objc3_darwin_register_v1(bad, 1);
  }
  if (objc3_darwin_class_v1("Root") != &root || objc3_darwin_class_v1("Missing"))
    return 1;
  if (objc3_darwin_lookup_v1(&root, &selector)(&root, &selector) != 42)
    return 2;
  if (objc3_darwin_lookup_v1(NULL, &selector)(NULL, &selector) != 0)
    return 3;
  const struct class_record **first = objc3_darwin_allocate_v1(&root);
  const struct class_record **second = objc3_darwin_allocate_v1(&root);
  if (!first || !second || first == second || *first != &root || *second != &root)
    return 4;
  if (strcmp(control, "wrong-selector-type") == 0)
    objc3_darwin_lookup_v1(second, &wrong_signature);
  if (objc3_darwin_lookup_v1(first, &same_signature)(first, &same_signature) != 17)
    return 5;
  objc3_darwin_deallocate_v1(first);
  if (objc3_darwin_lookup_v1(second, &selector)(second, &selector) != 17)
    return 6;
  objc3_darwin_deallocate_v1(second);
  objc3_darwin_deallocate_v1(NULL);
  if (instances || objc3_darwin_allocate_v1(NULL))
    return 7;
  if (objc3_darwin_class_v1("Child") != &child ||
      objc3_darwin_lookup_v1(&child, &selector)(&child, &selector) != 42)
    return 8;
  void *derived = objc3_darwin_allocate_v1(&child);
  if (!derived || objc3_darwin_lookup_v1(derived, &selector)(derived, &selector) != 19)
    return 9;
  const struct super_record instance_super = {derived, &root};
  const struct super_record class_super = {&child, &meta};
  if (strcmp(control, "wrong-super-start") == 0) {
    const struct super_record wrong = {derived, &meta};
    objc3_darwin_lookup_super_v1(&wrong, &selector);
  }
  if (objc3_darwin_lookup_super_v1(&instance_super, &selector)(derived, &selector) != 17 ||
      objc3_darwin_lookup_super_v1(&class_super, &selector)(&child, &selector) != 42)
    return 10;
  objc3_darwin_deallocate_v1(derived);
  if (!objc3_darwin_check_arc_threads(&root) || instances)
    return 11;
  return 0;
}
''')
    binary = tmp_path / 'registry'
    subprocess.run(['cc', '-O2', '-Wall', '-Wextra', '-Werror', '-pthread', '-I',
                    darwin_registration.RUNTIME.parent, harness,
                    darwin_registration.FIXTURES / 'arc_threads.c', '-o', binary], check=True)
    subprocess.run([binary], check=True)
    for name, diagnostic in (
        ('duplicate', 'duplicate class name'),
        ('unregistered', 'receiver has no registered class or live instance'),
        ('allocate-unregistered', 'allocation requires a registered class'),
        ('deallocate-class', 'deallocation requires a live instance'),
        ('stateful', 'invalid stateless class metadata'),
        ('wrong-method-table', 'invalid no-argument int method'),
        ('wrong-selector-type', 'method selector is missing'),
        ('missing-superclass', 'class has an unregistered superclass'),
        ('cyclic-superclass', 'class hierarchy has a cycle'),
        ('wrong-super-start', 'super dispatch start is not an ancestor of the receiver'),
    ):
        result = subprocess.run([binary, name], capture_output=True, text=True)
        assert result.returncode == -signal.SIGABRT
        assert diagnostic in result.stderr
