all:
	g++ -fsanitize=address -g3 -o msg-test -std=c++17 Test.cpp 2>&1
	@if (set -x; ./msg-test); then echo OK; else N=$$?; echo "FAIL (exit: $$N)"; exit 1; fi


clean:
	rm -f a.out *.o *.gch msg-test a.out
	rm -rf *.dSYM/
