#!/bin/bash

# Build the WASM module with Docker
docker run --rm -t wasm -v $(pwd):/workspace
