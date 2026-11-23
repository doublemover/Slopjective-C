from pathlib import Path
import shutil
import subprocess
import sys

import pytest

from scripts.objc3c_toolchain import darwin_registration


def test_experimental_profile_binds_separate_patch_and_native_job_budget():
    profile, patch = darwin_registration.load_profile()
    assert profile['profile'] == 'macos-arm64-registration-experimental'
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
    if extra_library:
        with pytest.raises(ValueError, match='unexpected portable Darwin service dependencies'):
            darwin_registration._exercise_portable_services(run, defaults, '/owned/ld', stages, layout)
        assert 'stdlib-map-count-native' not in commands
        return
    report = darwin_registration._exercise_portable_services(run, defaults, '/owned/ld', stages, layout)
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
        assert '-stdlib=libc++' in commands[label+'-link']
        assert commands[label+'-native'] == [str(layout / 'bin' / label)]


@pytest.mark.skipif(sys.platform != 'linux' or not shutil.which('cc'), reason='LP64 C registry control uses Linux cc')
def test_owned_registry_lookup_and_negative_metadata_controls(tmp_path):
    harness = tmp_path / 'harness.c'
    harness.write_text('''
#include "darwin_registration.c"
#include <sys/resource.h>

static int answer(const struct class_record *receiver, const struct selector_record *selector) {
  return receiver && selector ? 42 : -1;
}

static const struct selector_record selector = {"answer", "i16@0:8"};
static const struct {
  const struct method_list *next;
  int count;
  size_t stride;
  struct method_record methods[1];
} methods = {NULL, 1, sizeof(struct method_record), {{answer, &selector, "i16@0:8"}}};
static const struct class_record meta = {.flags = 1, .methods = (const struct method_list *)&methods};
static const struct class_record root = {.isa = &meta, .name = "Root"};

int main(int argc, char **argv) {
  struct rlimit no_core = {0, 0};
  setrlimit(RLIMIT_CORE, &no_core);
  if (argc > 1 && strcmp(argv[1], "unregistered") == 0)
    objc3_darwin_lookup_v1(&root, &selector);
  const struct class_record *batch[] = {&root};
  objc3_darwin_register_v1(batch, 1);
  if (argc > 1 && strcmp(argv[1], "duplicate") == 0)
    objc3_darwin_register_v1(batch, 1);
  if (objc3_darwin_class_v1("Root") != &root || objc3_darwin_class_v1("Missing"))
    return 1;
  if (objc3_darwin_lookup_v1(&root, &selector)(&root, &selector) != 42)
    return 2;
  if (objc3_darwin_lookup_v1(NULL, &selector)(NULL, &selector) != 0)
    return 3;
  return 0;
}
''')
    binary = tmp_path / 'registry'
    subprocess.run(['cc', '-O2', '-Wall', '-Wextra', '-Werror', '-pthread', '-I',
                    darwin_registration.RUNTIME.parent, harness, '-o', binary], check=True)
    subprocess.run([binary], check=True)
    for name, diagnostic in (('duplicate', 'duplicate class name'), ('unregistered', 'receiver is not a registered class')):
        result = subprocess.run([binary, name], capture_output=True, text=True)
        assert result.returncode != 0
        assert diagnostic in result.stderr
