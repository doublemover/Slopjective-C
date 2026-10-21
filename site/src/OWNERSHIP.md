# Site source ownership

| Source                | Responsibility                 | Owner            |
| --------------------- | ------------------------------ | ---------------- |
| `index.body.md`       | Public overview and navigation | compiler/docs    |
| `index.contract.json` | Paths and front matter         | compiler/tooling |
| `README.md`           | Generation policy              | compiler/docs    |

Edit source, run `python -m scripts.build_site_index`, and verify with
`python -m scripts.build_site_index --check`. Commit `site/index.md` alongside
its owner changes. Capability claims follow
`schemas/objc3-language-capabilities.json`; the onboarding guard checks current
reader routes. Generation does not deploy the website.
