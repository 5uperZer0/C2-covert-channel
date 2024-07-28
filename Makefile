ssmake: server_send.c helper.c
	gcc -g -o server_send server_send.c helper.c -lm

srmake: server_rec.c helper.c
	gcc -g -o server_rec server_rec.c helper.c -lm

crmake: client_rec.c helper.c
	gcc -g -o client_rec client_rec.c helper.c -lm

csmake: client_send.c helper.c
	gcc -g -o client_send client_send.c helper.c -lm

all: csmake crmake ssmake srmake