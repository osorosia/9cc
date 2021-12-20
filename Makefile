all:

# guest

# host
up:
	@docker-compose up -d
	@docker-compose exec cc bash

down:
	@docker-compose down


