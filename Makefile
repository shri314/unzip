all:
	g++ -g3 -o msg-test -std=c++17 Test.cpp 2>&1
	./msg-test; N=$$?; echo $$N; exit $$N


clean:
	rm -f a.out *.o *.gch msg-test a.out
	rm -rf *.dSYM/
