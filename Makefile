all:
	g++ -o msg-test -std=c++17 Test.cpp 2>&1
	./msg-test
	@echo OK


clean:
	rm -f a.out *.o *.gch msg-test a.out
