#!/usr/bin/env pwsh

# Set the script to fail on error
$ErrorActionPreference = "Stop"

# Define variables
$SOLIDC_GIT_URL = "https://github.com/abiiranathan/solidc.git"
$SOLIDC_GIT_DIR = "solidc"
$BUILD_TESTS = "OFF" # Set to "OFF" to disable tests
$BUILD_EXAMPLES = "OFF" # Set to "OFF" to disable examples
$DELETE_CLONE = "OFF" # Set to "ON" to delete the cloned repositories

# Clone the repositories if they don't exist
if (-Not (Test-Path -Path $SOLIDC_GIT_DIR)) {
    git clone $SOLIDC_GIT_URL $SOLIDC_GIT_DIR
}

# Build the repositories
Push-Location -Path $SOLIDC_GIT_DIR
if (-Not (Test-Path -Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

Push-Location -Path "build"
cmake -DBUILD_TESTING=$BUILD_TESTS ..
make
cmake --install . --prefix "C:\Program Files\solidc"
Pop-Location
Pop-Location
Write-Output "SolidC installed successfully"

# Check if the repositories should be deleted
if ($DELETE_CLONE -eq "ON") {
    # Remove the cloned repositories
    Write-Output "Cleaning up..."
    Remove-Item -Recurse -Force -Path $SOLIDC_GIT_DIR
}

# Build the csvparser library
Remove-Item -Recurse -Force -Path "build"
Remove-Item -Recurse -Force -Path ".cache"
New-Item -ItemType Directory -Path "build" | Out-Null
Push-Location -Path "build"
cmake -DBUILD_TESTING=$BUILD_TESTS -DBUILD_EXAMPLES=$BUILD_EXAMPLES ..
make
cmake --install . --prefix "C:\Program Files\csvparser"
Pop-Location

Write-Output "csvparser installed successfully"
