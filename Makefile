gameoflife/index.html: src/gameoflife.cpp
	mkdir -p gameoflife
	em++ -sUSE_SDL=2 -sUSE_SDL_TTF=2 -sALLOW_MEMORY_GROWTH -std=c++20 src/gameoflife.cpp -o gameoflife/index.html -s WASM=1

clean:
	rm -rf gameoflife
