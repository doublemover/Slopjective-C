"""Check the generated or published Pages index at its public project root."""
from __future__ import annotations

import argparse
from html.parser import HTMLParser
from pathlib import Path
import time
from urllib.parse import urljoin, urlsplit
from urllib.request import urlopen


class Navigation(HTMLParser):
    def __init__(self):
        super().__init__()
        self.links = set()

    def handle_starttag(self, tag, attrs):
        href = dict(attrs).get('href')
        if tag == 'a' and href:
            self.links.add(href)


def check(html: str, project: str) -> set[str]:
    owner, repo = project.split('/')
    origin = f'https://{owner.lower()}.github.io'
    root = f'{origin}/{repo}/'
    site = Navigation()
    site.feed(html)
    links = {urljoin(root, link).split('#', 1)[0] for link in site.links}
    required = {f'https://github.com/{project}/blob/main/docs/releases/{name}.md'
                for name in ('linux', 'status')}
    if not required <= links:
        raise ValueError('Pages index lacks the current Linux guide or release status')
    local = {link for link in links if urlsplit(link).netloc == urlsplit(origin).netloc}
    outside = sorted(link for link in local if not urlsplit(link).path.startswith(f'/{repo}/'))
    if outside:
        raise ValueError(f'Pages navigation leaves the deployed project root: {outside}')
    return local | required


def published(url: str, project: str) -> None:
    for attempt in range(12):
        try:
            with urlopen(url, timeout=10) as page:
                links = check(page.read().decode(), project)
            for target in sorted(links - {url}):
                with urlopen(target, timeout=10):
                    pass
            return
        except (OSError, ValueError):
            if attempt == 11:
                raise
            time.sleep(5)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', required=True)
    source = parser.add_mutually_exclusive_group(required=True)
    source.add_argument('--html', type=Path)
    source.add_argument('--published')
    args = parser.parse_args()
    if args.html:
        check(args.html.read_text(), args.repo)
    else:
        published(args.published, args.repo)


if __name__ == '__main__':
    main()
