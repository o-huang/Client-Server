theclient:
			gcc src/client.c -Wall -Werror -pthread -g -o wclient
theserver:
			gcc src/warehouse_db.c -Wall -Werror -pthread -g -o whouse
clean:
			rm wclient
			rm whouse
