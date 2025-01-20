all:
	g++ -Wall -Werror -Wextra -fsanitize=address -g3 -o tests/test-expected -std=c++17 tests/TestExpected.cpp -I . 2>&1
	tests/test-expected
	g++ -Wall -Werror -Wextra -fsanitize=address -g3 -o unzip-test -std=c++17 Test.cpp utils/MemoryMappedFile.cpp zip/Inflate.cpp -I . -I ./thirdparty/include -L ./thirdparty/lib -lz 2>&1
	@for f in ./assets/test*-*.zip; \
	do \
		if (set -x; ./unzip-test $$f); then \
			echo OK; \
		else \
			N=$$?; \
			echo "FAIL (exit: $$N)"; \
			exit 1; \
		fi; \
	done

format:
	@if ! which clang-format-20 1>/dev/null; then echo "Need clang-fomat-20, see https://apt.llvm.org/"; exit 1; fi
	find . -name \*.hpp -o -name \*.cpp \
		| grep -v -e thridparty -e tmp_stage \
		| xargs clang-format-20 -i --style=file

clean:
	rm -f a.out *.o *.gch unzip-test a.out tests/test-expected
	rm -rf *.dSYM/ tmp_stage/
