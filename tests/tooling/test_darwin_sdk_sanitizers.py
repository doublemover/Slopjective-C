"""Check the optional Darwin sanitizer package and witness policy on any host."""

import copy

import pytest

from scripts.objc3c_toolchain import darwin_sdk as sdk
from scripts.objc3c_toolchain import darwin_sdk_sanitizers as sanitizers
from scripts.objc3c_toolchain.darwin_registration import qualify


RESOURCE = 'lib/clang/23'


def manifest():
    variant = sanitizers.descriptor(RESOURCE)
    files = {row['path']: {'kind': 'file', 'sha256': row['sha256']} for row in variant['runtimes'].values()}
    files[sanitizers.FIXTURE] = {'kind': 'file', 'sha256': '0' * 64}
    return {sanitizers.KEY: variant, 'files': files,
            'bootstrap_llvm': {key: sanitizers.UPSTREAM[key] for key in ('version', 'revision')}}


def test_optional_descriptor_admits_exactly_the_two_upstream_runtime_files_and_witness():
    data = manifest()
    assert sanitizers.manifest_files({}, RESOURCE) == set()
    assert sanitizers.manifest_files(data, RESOURCE) == data['files'].keys()
    assert not any(path.startswith('lib/libobjc3darwin') for path in data['files'])
    assert 'MRC consumer' in data[sanitizers.KEY]['scope']


@pytest.mark.parametrize('fault', ('wrong-source', 'wrong-target-resource', 'changed-runtime',
                                   'incomplete-variant', 'missing-runtime', 'null-variant'))
def test_claimed_variant_rejects_missing_stale_or_unpinned_assets(fault):
    data, resource = manifest(), RESOURCE
    variant = data[sanitizers.KEY]
    runtime = variant['runtimes']['asan']['path']
    if fault == 'wrong-source':
        data['bootstrap_llvm']['revision'] = '0' * 40
    elif fault == 'wrong-target-resource':
        resource = 'lib/clang/24'
    elif fault == 'changed-runtime':
        data['files'][runtime]['sha256'] = variant['runtimes']['ubsan']['sha256']
    elif fault == 'incomplete-variant':
        variant['runtimes'].pop('ubsan')
    elif fault == 'missing-runtime':
        data['files'].pop(runtime)
    else:
        data[sanitizers.KEY] = None
    with pytest.raises(ValueError, match='pinned upstream'):
        sanitizers.manifest_files(data, resource)


def test_stage_refuses_an_ambient_runtime_prefix_before_copying(tmp_path):
    with pytest.raises(ValueError, match='pinned Darwin compiler-rt dylib is absent'):
        sanitizers.stage_payload(tmp_path / 'package', tmp_path / 'ambient-resources', RESOURCE)
    assert not (tmp_path / 'package').exists()


def test_registration_requires_explicit_sdk_package_selection(tmp_path):
    output = tmp_path / 'output'
    with pytest.raises(ValueError, match='--sdk-sanitizers requires --package-sdk'):
        qualify(tmp_path / 'bootstrap.json', output, fetch=False, sdk_sanitizers=True)
    assert not output.exists()


def test_base_package_inventory_requires_opt_in_and_the_live_witness():
    host = {'xcode': sdk.XCODE, 'sdk_version': sdk.SDK_VERSION, 'sdk_settings_sha256': '1' * 64,
            'libcxx': {'config_relative': 'config', 'config_sha256': '2' * 64, 'stub_sha256': '3' * 64}, 'tools': {}}
    data = manifest()
    data.update(payload_schema=sdk.PAYLOAD_SCHEMA, profile=sdk.PROFILE, target=sdk.TARGET,
                runtime_abi=sdk.RUNTIME_ABI, compiler_runtime_flag=sdk.RUNTIME_FLAG, support_truth=False,
                host_requirements=sdk._host_requirements(host), resource_dir=RESOURCE)
    for path in (sdk.DRIVER, 'bin/clang', 'bin/llvm-ar', 'bin/llvm-dwarfdump', f'lib/{sdk.LIBRARY}.a', sdk.API_HEADER,
                 f'lib/{sdk.LIBRARY}.dylib', RESOURCE + '/include/stddef.h',
                 RESOURCE + '/lib/darwin/libclang_rt.osx.a', 'share/licenses/slopjective-c/LICENSE',
                 *(sdk.DEMO + '/' + name for name in sdk.DEMO_FILES),
                 *(f'share/licenses/{name}/LICENSE.TXT' for name in ('llvm', 'clang', 'compiler-rt'))):
        data['files'][path] = {'kind': 'file'}
    assert sdk._validate_manifest(data, host) == RESOURCE
    missing = copy.deepcopy(data)
    missing['files'].pop(sanitizers.FIXTURE)
    with pytest.raises(ValueError, match='payload inventory'):
        sdk._validate_manifest(missing, host)
    data.pop(sanitizers.KEY)
    with pytest.raises(ValueError, match='payload inventory'):
        sdk._validate_manifest(data, host)


@pytest.mark.parametrize('mode', tuple(sanitizers.FLAGS))
def test_requires_native_dispatch_and_the_compiler_injected_detector_references(mode):
    graph = sanitizers.MACHINE_IMPORTS | sanitizers.SANITIZER_IMPORTS[mode] | {'_puts'}
    assert sanitizers.validate_imports(mode, '\n'.join(graph)) == sorted(graph)
    with pytest.raises(ValueError, match='owned dispatch instrumentation'):
        sanitizers.validate_imports(mode, '\n'.join(graph - {'_objc3_darwin_lookup_v1'}))
    if mode != 'plain':
        with pytest.raises(ValueError, match='sanitizer'):
            sanitizers.validate_imports(mode, '\n'.join(sanitizers.MACHINE_IMPORTS))
    conflicting = 'asan' if mode == 'ubsan' else 'ubsan'
    with pytest.raises(ValueError, match='sanitizer'):
        sanitizers.validate_imports(mode, '\n'.join(graph | sanitizers.SANITIZER_IMPORTS[conflicting]))


def test_native_execution_must_be_clean_and_load_the_installed_path(tmp_path):
    runtime = tmp_path / 'Relocated SDK' / sanitizers.RUNTIMES['asan'][0]
    runtime.parent.mkdir()
    runtime.touch()
    outcome = sanitizers.WITNESS + '\n' + str(runtime) + '\n'
    assert sanitizers.validate_execution(outcome, '', 0, runtime) == str(runtime.resolve())
    assert sanitizers.validate_execution(sanitizers.WITNESS, '', 0, None) is None
    with pytest.raises(ValueError, match='outside the admitted SDK'):
        sanitizers.validate_execution(outcome.replace('Relocated SDK', 'Other SDK'), '', 0, runtime)
    for stdout, stderr, code in ((outcome, '', 1), (outcome, 'unexpected stderr', 0), (sanitizers.WITNESS, '', 0)):
        with pytest.raises(ValueError, match='execute cleanly'):
            sanitizers.validate_execution(stdout, stderr, code, runtime)
