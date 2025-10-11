# Simple make file for linux
qur: qur.cpp utils/lexer.cpp
	g++ -std=c++17 qur.cpp utils/lexer.cpp utils/ast.cpp -o qur 

run:
	./qur -c file.qur -o file

clean:
	rm -f qur file
