NAME	= 9cc
CFLAGS	= -std=c11 -g -static

all: $(NAME)
$(NAME): 9cc.c

test: $(NAME)
	bash test.sh

clean:
	rm -f $(NAME) *.o *~ tmp*

.PHONY: test clean

# host---------------
up:
	@docker-compose up -d
	@docker-compose exec cc bash

down:
	@docker-compose down

.PHONY: up down
