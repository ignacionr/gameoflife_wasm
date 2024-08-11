FROM emscripten/emsdk

# please map the current directory to /workspace
WORKDIR /workspace

ENTRYPOINT [ "/bin/bash" ]
