NAME	= 9cc
CFLAGS	= -std=c11 -g -fdiagnostics-color=always -static
SRCS	= $(wildcard *.c)
OBJS	= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -o 9cc $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h

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
