CXXFLAGS = --std=c++11 -O2 -Wall

messcpp: messcpp.cpp lexer.hpp
	$(CXX) $(CXXFLAGS) messcpp.cpp -o messcpp

mess_list.txt: gen_mess
	./gen_mess 2000

gen: mess_list.txt

test: messcpp test.cpp mess_list.txt
	./messcpp test.cpp test_out.cpp

test_lexer: test.cpp lexer_test
	./lexer_test test.cpp

gen_mess: gen_mess.cpp
	$(CXX) $(CXXFLAGS) gen_mess.cpp -o gen_mess

lexer.hpp: queue.hpp

lexer_test: lexer.hpp lexer_test.cpp
	$(CXX) $(CXXFLAGS) lexer_test.cpp -o lexer_test

clean:
	rm -f lexer_test messcpp test_out.cpp gen_mess

