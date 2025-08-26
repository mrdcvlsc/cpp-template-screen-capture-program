debug:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
	echo 'If: false' > ./build/.clangd
	cmake --build build --config Debug

build:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
	echo 'If: false' > ./build/.clangd
	cmake --build build --config Release

clean:
	rm -rf build external

run:
	./build/bin/ScreenShotApp