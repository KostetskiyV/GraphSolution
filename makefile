CC := gcc
CFLAGS := -std=c99 -Werror -Wall -Wextra -Wpedantic -g

app.exe: main.o graph.o matrix_manager.o ./math_funcs.o
	$(CC) -o $@ $^ -lm

./main.o : ./main.c ./graph.h ./math_funcs.h ./error_codes.h
	$(CC) $(CFLAGS) -c $< -o $@

./graph.o : ./graph.c ./graph.h ./matrix_manager.h ./edge_variations.h ./error_codes.h
	$(CC) $(CFLAGS) -c $< -o $@

./matrix_manager.o : ./matrix_manager.c ./matrix_manager.h ./error_codes.h
	$(CC) $(CFLAGS) -c $< -o $@

./math_funcs.o : ./math_funcs.c ./math_funcs.h
	$(CC) $(CFLAGS) -c $< -o $@

./data/map.png : ./data/graph.gv
	dot -T png -o $@ $^

.PHONY: clean graph
clean:
	rm -f *.o *.exe

graph: ./data/map.png
	open ./data/map.png