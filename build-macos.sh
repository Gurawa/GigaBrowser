#!/bin/bash
# Build script for GigaBrowser on macOS using GitHub Actions
set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Check required dependencies
check_dependencies() {
    log_info "Checking dependencies..."
    
    local missing_deps=()
    
    if ! command_exists python3; then
        missing_deps+=("python3")
    fi
    
    if ! command_exists clang; then
        missing_deps+=("clang")
    fi
    
    if ! command_exists rustc; then
        missing_deps+=("rust")
    fi
    
    if ! command_exists node; then
        missing_deps+=("node")
    fi
    
    if [ ${#missing_deps[@]} -ne 0 ]; then
        log_error "Missing dependencies: ${missing_deps[*]}"
        log_info "Please install missing dependencies and try again"
        exit 1
    fi
    
    log_success "All dependencies found"
}

# Setup build environment
setup_environment() {
    log_info "Setting up build environment..."
    
    # Set environment variables
    export MOZILLA_OFFICIAL=1
    export MOZ_AUTOMATION=1
    export MACH_BUILD_PYTHON_NATIVE_PACKAGE_SOURCE=system
    
    # Use GitHub Actions specific mozconfig if available
    if [ -f "mozconfig.github-actions" ]; then
        export MOZCONFIG="$PWD/mozconfig.github-actions"
        log_info "Using GitHub Actions mozconfig: $MOZCONFIG"
    elif [ -f "mozconfig" ]; then
        export MOZCONFIG="$PWD/mozconfig"
        log_info "Using existing mozconfig: $MOZCONFIG"
    else
        log_error "No mozconfig found!"
        exit 1
    fi
    
    log_success "Environment setup complete"
}

# Bootstrap the build
bootstrap_build() {
    log_info "Bootstrapping build environment..."
    
    python3 ./mach bootstrap --no-interactive --application-choice=browser
    
    log_success "Bootstrap complete"
}

# Configure the build
configure_build() {
    log_info "Configuring build..."
    
    python3 ./mach configure
    
    log_success "Configuration complete"
}

# Build the application
build_application() {
    log_info "Building application..."
    
    # Get start time
    local start_time=$(date +%s)
    
    python3 ./mach build
    
    # Calculate build time
    local end_time=$(date +%s)
    local build_time=$((end_time - start_time))
    local build_time_min=$((build_time / 60))
    local build_time_sec=$((build_time % 60))
    
    log_success "Build complete in ${build_time_min}m ${build_time_sec}s"
}

# Package the application
package_application() {
    log_info "Packaging application..."
    
    python3 ./mach package
    
    log_success "Packaging complete"
}

# Create installer (optional)
create_installer() {
    log_info "Creating installer..."
    
    if python3 ./mach build installer; then
        log_success "Installer created successfully"
    else
        log_warning "Installer creation failed, but continuing..."
    fi
}

# Show build artifacts
show_artifacts() {
    log_info "Build artifacts:"
    
    local obj_dir="obj-ff"
    if [ -n "$MOZCONFIG" ]; then
        obj_dir=$(grep "MOZ_OBJDIR" "$MOZCONFIG" | cut -d'=' -f2 | sed 's/@TOPSRCDIR@\///')
    fi
    
    if [ -d "$obj_dir/dist" ]; then
        find "$obj_dir/dist" -name "*.dmg" -o -name "*.tar.bz2" -o -name "*.app" | while read -r artifact; do
            local size=$(du -h "$artifact" | cut -f1)
            log_info "  $artifact ($size)"
        done
    else
        log_warning "No dist directory found at $obj_dir/dist"
    fi
}

# Cleanup function
cleanup() {
    log_info "Cleaning up temporary files..."
    # Add any cleanup tasks here
}

# Main execution
main() {
    log_info "Starting GigaBrowser macOS build process..."
    
    # Set up trap for cleanup
    trap cleanup EXIT
    
    check_dependencies
    setup_environment
    
    # Install Node.js dependencies if package.json exists
    if [ -f "package.json" ]; then
        log_info "Installing Node.js dependencies..."
        npm install
    fi
    
    bootstrap_build
    configure_build
    build_application
    package_application
    
    # Try to create installer (optional)
    create_installer
    
    show_artifacts
    
    log_success "Build process completed successfully!"
}

# Run main function
main "$@"