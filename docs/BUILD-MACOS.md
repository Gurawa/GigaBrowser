# Building GigaBrowser for macOS

This document describes how to build GigaBrowser for macOS using GitHub Actions.

## Overview

GigaBrowser can be built for macOS using GitHub's macOS runners. We provide several workflows for different use cases:

1. **Simple macOS Build** (`macos-build.yml`) - Basic build for Intel Macs
2. **Universal macOS Build** (`macos-universal-build.yml`) - Builds universal binaries for both Intel and Apple Silicon
3. **Test Build** (`test-macos-build.yml`) - Tests the build environment setup

## Workflows

### Simple macOS Build

The basic workflow builds GigaBrowser for Intel-based Macs:

- Runs on `macos-13` (Intel runner)
- Builds optimized release version
- Creates application package and DMG installer
- Uploads build artifacts

**Triggers:**
- Push to `main` or `develop` branches
- Pull requests to `main` or `develop` branches
- Manual workflow dispatch

### Universal macOS Build

The universal build workflow creates binaries that run on both Intel and Apple Silicon Macs:

- Builds separately on Intel (`macos-13`) and Apple Silicon (`macos-14`) runners
- Combines the binaries using `lipo` to create universal binaries
- Creates a universal DMG installer
- Supports release automation for tagged versions

**Triggers:**
- Push to `main` or `develop` branches
- Push of version tags (`v*`)
- Pull requests to `main` or `develop` branches
- Manual workflow dispatch with build type selection

### Test Build

A lightweight workflow to test the build environment setup:

- Validates dependencies and configuration
- Tests `mach` command accessibility
- Checks system information
- Validates mozconfig files

## Configuration Files

### `mozconfig.github-actions`

This is the build configuration file optimized for GitHub Actions:

```bash
# Key settings
mk_add_options MOZ_OBJDIR=@TOPSRCDIR@/obj-ff
mk_add_options MOZ_MAKE_FLAGS="-j$(sysctl -n hw.ncpu)"
export CC=clang
export CXX=clang++
ac_add_options --target=x86_64-apple-darwin
ac_add_options --enable-application=browser
ac_add_options --enable-optimize
ac_add_options --enable-release
```

### `build-macos.sh`

A helper script that automates the build process:

```bash
./build-macos.sh
```

Features:
- Dependency checking
- Environment setup
- Automated build process
- Build time tracking
- Artifact listing

## Build Process

The build process follows these steps:

1. **Environment Setup**
   - Install system dependencies via Homebrew
   - Set up Python 3.11
   - Install Rust toolchain
   - Install Node.js dependencies

2. **Bootstrap**
   - Run `./mach bootstrap` to set up the build environment

3. **Configure**
   - Run `./mach configure` with the mozconfig

4. **Build**
   - Run `./mach build` to compile the browser

5. **Package**
   - Run `./mach package` to create the application bundle
   - Optionally create DMG installer

## Artifacts

The workflows produce the following artifacts:

- **Application Bundle**: `GigaBrowser.app` - The macOS application
- **Compressed Archive**: `*.tar.bz2` - Compressed application bundle
- **DMG Installer**: `*.dmg` - macOS disk image installer (when successful)

## Requirements

### System Requirements

- macOS 10.12 (Sierra) or later for running the built application
- Xcode Command Line Tools
- At least 8GB RAM for building
- At least 30GB free disk space

### GitHub Actions Requirements

- Repository with GitHub Actions enabled
- No additional secrets required for basic builds
- `GITHUB_TOKEN` is automatically provided for release uploads

## Customization

### Build Types

You can customize the build by modifying the mozconfig:

- **Debug Build**: Remove `--enable-optimize` and `--disable-debug`
- **PGO Build**: Add `--enable-profile-guided-optimization`
- **LTO Build**: Add `--enable-lto=thin`

### Branding

To use custom branding:

1. Create your branding directory in `browser/branding/`
2. Update the mozconfig: `ac_add_options --with-branding=browser/branding/yourbrand`

### Distribution ID

Update the distribution ID in mozconfig:
```bash
ac_add_options --with-distribution-id=com.yourcompany.yourbrowser
```

## Troubleshooting

### Common Issues

1. **Build Timeout**: Increase the `timeout-minutes` value in the workflow
2. **Disk Space**: The build requires significant disk space
3. **Memory Issues**: Consider using fewer parallel jobs in mozconfig
4. **Dependency Issues**: Ensure all required packages are installed via Homebrew

### Debug Builds

For debugging build issues:

1. Enable the test workflow to check environment setup
2. Add debug output to the build steps
3. Upload build logs as artifacts for analysis

### Performance Optimization

To speed up builds:

1. Use fewer parallel jobs if memory is limited: `MOZ_MAKE_FLAGS="-j2"`
2. Disable unnecessary features in mozconfig
3. Use ccache (requires additional setup)

## Contributing

When modifying the build system:

1. Test changes with the test workflow first
2. Update documentation for any configuration changes
3. Ensure compatibility with both Intel and Apple Silicon runners
4. Test the universal build process if making architecture-specific changes

## Support

For build issues:

1. Check the GitHub Actions logs
2. Review the uploaded artifacts
3. Compare with successful builds
4. Check Mozilla's documentation for Firefox builds