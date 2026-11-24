palindrome: palindrome.o
	g++ -o palindrome palindrome.o
	./palindrome

palindrome.o: palindrome.cpp stack.h
	g++ -c palindrome.cpp

clean:
	rm *.o palindrome

