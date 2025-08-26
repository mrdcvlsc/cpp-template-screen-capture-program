debug:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
	cmake --build build --config Debug

build:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
	cmake --build build --config Release

clean:
	rm -rf build external

run:
	./build/bin/ScreenShotApp