#!/usr/bin/env sh

# Set the script to fail on error
set -eux

SOLIDC_GIT_URL="https://github.com/abiiranathan/solidc.git"
SOLIDC_GIT_DIR="solidc"
BUILD_TESTS="OFF" # Set to "OFF" to disable tests
BUILD_EXAMPLES="OFF" # Set to "OFF" to disable examples
DELETE_CLONE="OFF" # Set to "ON" to delete the cloned repositories

# Clone the repositories if they don't exist
[ -d $SOLIDC_GIT_DIR ] || git clone $SOLIDC_GIT_URL $SOLIDC_GIT_DIR

# Build the repositories
cd $SOLIDC_GIT_DIR
mkdir -p build && cd build
cmake -DBUILD_TESTING=$BUILD_TESTS ..
make
sudo cmake --install .
cd ../..
echo "SolidC installed successfully"

# Check if the repositories should be deleted
if [ $DELETE_CLONE = "ON" ]; then
    # Remove the cloned repositories
    echo "Cleaning up..."
    rm -rf $SOLIDC_GIT_DIR
fi

# Build the csvparser library
rm -rf build .cache && mkdir -p build && cd build
cmake -DBUILD_TESTING=$BUILD_TESTS -DBUILD_EXAMPLES=$BUILD_EXAMPLES ..
make
sudo cmake --install .
cd ../
