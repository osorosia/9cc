all: run

# host
run:
	@docker-compose up -d
	@docker-compose exec cc bash

down:
	@docker-compose down

# guest
