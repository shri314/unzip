all:
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

clean:
	rm -f a.out *.o *.gch unzip-test a.out
	rm -rf *.dSYM/ tmp_stage/
