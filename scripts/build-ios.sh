#!/bin/bash

set -e

OUTPUT_LOCATION=$1

if [ -z "$OUTPUT_LOCATION" ]
then
  echo "ERROR: OUTPUT_LOCATION argument must be supplied and be a valid directory"
  exit 1
fi

# Create the root directory for the IOS release binaries
mkdir -p $OUTPUT_LOCATION/ios

# Create the directories at the output location for the release binaries
mkdir -p $OUTPUT_LOCATION/ios/x86_64
mkdir -p $OUTPUT_LOCATION/ios/aarch64
mkdir -p $OUTPUT_LOCATION/ios/universal

# Install cargo-lipo
# see https://github.com/TimNN/cargo-lipo
cargo install cargo-lipo
rustup target install x86_64-apple-ios aarch64-apple-ios
cargo lipo --release
cp "./target/x86_64-apple-ios/release/libaries_askar.a" $OUTPUT_LOCATION/ios/x86_64
cp "./target/aarch64-apple-ios/release/libaries_askar.a" $OUTPUT_LOCATION/ios/aarch64
cp "./target/universal/release/libaries_askar.a" $OUTPUT_LOCATION/ios/universal