#!/bin/bash

# Install and setup the rust tool cbindgen to automate the generation of the c header file through
# parsing the aries-askar library code.

set -e

rustup install nightly
rustup default nightly
cargo install cargo-expand
cargo install --force cbindgen
cbindgen --config cbindgen.toml --crate aries-askar --output include/aries-askar.h
rustup default stable