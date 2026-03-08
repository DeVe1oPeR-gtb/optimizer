#!/usr/bin/env bash
# Run AllTests (CppUTest). Build first if needed.
set -e
cd "$(dirname "$0")"
if [[ ! -f build/AllTests ]]; then
  make all
fi
./build/AllTests "$@"
