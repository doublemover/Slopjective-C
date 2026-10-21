from pathlib import Path
import shutil

import pytest

from scripts.objc3c_toolchain.check_onboarding import check

ROOT = Path(__file__).resolve().parents[2]


@pytest.fixture
def reader_tree(tmp_path):
    for directory in ('docs/tutorials', 'showcase', 'site'):
        shutil.copytree(ROOT / directory, tmp_path / directory,
                        ignore=shutil.ignore_patterns('_site'))
    # External source owners are read-only; mutate only the copied reader pages.
    for directory in ('schemas', 'tests', 'native', 'toolchains', 'spec'):
        (tmp_path / directory).symlink_to(ROOT / directory, target_is_directory=True)
    for path in (ROOT / 'docs').iterdir():
        if path.name != 'tutorials':
            (tmp_path / 'docs' / path.name).symlink_to(path, target_is_directory=path.is_dir())
    (tmp_path / 'README.md').symlink_to(ROOT / 'README.md')
    return tmp_path


def test_current_routes_and_installed_example_owners():
    report = check(ROOT)
    assert report['status'] == 'PASS'
    assert report['examples'] == ['managed-modules', 'native-application', 'package-graph']


@pytest.mark.parametrize('addition,diagnostic', [
    ('\n```sh\nnpm run objc3c -- compile-objc3c app.objc3\n```\n', 'retired compiler workflow'),
    ('\n```sh\npython -m scripts.objc3c_toolchain imaginary-command\n```\n', 'unknown repository command'),
    ('\n[Next step](missing-reader-page.md)\n', 'broken navigation'),
])
def test_rejects_removed_commands_and_broken_navigation(reader_tree, addition, diagnostic):
    page = reader_tree / 'docs/tutorials/getting_started.md'
    page.write_text(page.read_text() + addition)
    with pytest.raises(ValueError, match=diagnostic):
        check(reader_tree)


def test_rejects_public_concurrency_promotion(reader_tree):
    page = reader_tree / 'docs/tutorials/getting_started.md'
    page.write_text(page.read_text().replace('actors remain reserved', 'actors are implemented'))
    with pytest.raises(ValueError, match='reserved concurrency boundary'):
        check(reader_tree)


def test_rejects_generated_site_drift(reader_tree):
    page = reader_tree / 'site/index.md'
    page.write_text(page.read_text() + '\nManual edit\n')
    with pytest.raises(ValueError, match='generated output drift'):
        check(reader_tree)


def test_rejects_disconnected_example_qualification(reader_tree):
    page = reader_tree / 'showcase/portfolio.json'
    page.write_text(page.read_text().replace('installed_source_graph', 'unexecuted_example'))
    with pytest.raises(ValueError, match='installed SDK qualification owners'):
        check(reader_tree)
