FLAGS	 = -Wall -Wextra -Werror

all: game_of_life

game_of_life: game_of_life.c
	gcc $(FLAGS) game_of_life.c -o game_of_life

rebuild: clean all

clean:
	rm game_of_life