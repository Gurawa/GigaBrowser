![Firefox Browser](./docs/readme/readme-banner.svg)

# GigaBrowser

GigaBrowser is a fast, reliable and private web browser based on [Firefox](https://firefox.com/) from the [Mozilla organization](https://mozilla.org/).

## Building for macOS

GigaBrowser supports building for macOS using GitHub Actions with both Intel and Apple Silicon support.

### Quick Start

1. **Simple Build**: The basic workflow builds for Intel Macs and is triggered automatically on pushes to main/develop branches.

2. **Universal Build**: Creates universal binaries for both Intel and Apple Silicon Macs.

3. **Manual Build**: You can trigger builds manually using the workflow dispatch feature.

### Build Workflows

- **macOS Build** (`.github/workflows/macos-build.yml`) - Basic Intel build
- **Universal macOS Build** (`.github/workflows/macos-universal-build.yml`) - Universal binary build
- **Test macOS Build** (`.github/workflows/test-macos-build.yml`) - Environment testing

For detailed build instructions, see [docs/BUILD-MACOS.md](docs/BUILD-MACOS.md).

### Contributing

To learn how to contribute to Firefox read the [Firefox Contributors' Quick Reference document](https://firefox-source-docs.mozilla.org/contributing/contribution_quickref.html).

We use [bugzilla.mozilla.org](https://bugzilla.mozilla.org/) as our issue tracker, please file bugs there.

### Resources

* [Firefox Source Docs](https://firefox-source-docs.mozilla.org/) is our primary documentation repository
* Nightly development builds can be downloaded from [Firefox Nightly page](https://www.mozilla.org/firefox/channel/desktop/#nightly)

If you have a question about developing Firefox, and can't find the solution
on [Firefox Source Docs](https://firefox-source-docs.mozilla.org/), you can try asking your question on Matrix at
chat.mozilla.org in the [Introduction channel](https://chat.mozilla.org/#/room/#introduction:mozilla.org).
