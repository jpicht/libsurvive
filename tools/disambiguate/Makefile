disambiguate: disambiguate.c dis.c
	gcc -I../../src -Wall -Wextra -Werror -g -o $@ $^ -lm

clean:
	rm disambiguate

test: disambiguate
	./disambiguate | head -n 100

disambiguate2: disambiguate2.c
	gcc -Wall -Wextra -Werror -g -o $@ $^

test2: disambiguate2
	./disambiguate2 | head -n 100
