all: tests examples

create_build_dir:
	mkdir -p build

tests: create_build_dir
	cc -o build/test_c test/test.c -Wall -Wextra -Wshadow -g -I./
	build/test_c

examples: create_build_dir
	cc -o build/example_c example/main.c -Wall -Wextra -Wshadow -g -I./
	c++ -o build/example_cpp example/main.cpp -Wall -Wextra -Wshadow -g -I./

