#include "socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
 #include <sys/wait.h>

void initialiser_signaux ( void ){
	if ( signal ( SIGPIPE , SIG_IGN ) == SIG_ERR ){
		perror ("signal");
	}
	struct sigaction sa ;
		sa . sa_handler = traitement_signal ;
		sigemptyset (& sa . sa_mask );
		sa . sa_flags = SA_RESTART ;
	if ( sigaction ( SIGCHLD , & sa , NULL ) == -1){
		perror("erreur signal handler");
	}
}

void traitement_signal ( int sig ){
	printf ("Signal %d reçu \n",sig);
	wait(NULL);
}

int creer_serveur (int port ) {
	
	int value = port;
	int socket_serveur ;
	int optval = 1;
	int toto;
	char buff[256];
	pid_t pid = 0;

	struct sockaddr_in saddr;
	saddr . sin_family = AF_INET; /* Socket ipv4 */
	saddr . sin_port = htons (8000); /* Port d ’ écoute */
	saddr . sin_addr . s_addr = INADDR_ANY; /* écoute sur toutes les interfaces */

	initialiser_signaux();

	if((socket_serveur = socket (AF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket serveur");
		return -1;
	}

	if ( socket_serveur == -1){
		perror ( " socket_serveur " ); /* traitement de l ’ erreur */
	}
	/* Utilisation de la socket serveur */
	
	if ( setsockopt ( socket_serveur , SOL_SOCKET , SO_REUSEADDR , & optval , sizeof ( int )) == -1){
		perror ( " Can not set SO_REUSEADDR option " );
	}

	if ( bind ( socket_serveur , (struct sockaddr *)& saddr , sizeof( saddr )) == -1){
		perror ( " bind socker_serveur " );
	/* traitement de l ’ erreur */
	}

	if ( listen ( socket_serveur , 10) == -1){
		perror ( " listen socket_serveur " );
	/* traitement d ’ erreur */
	}
	const char * message_bienvenue = "PANZANI SAUCISSE!\n" ;

	while(1){

		int socket_client ;
		socket_client = accept ( socket_serveur , NULL , NULL );
		/* traitement d ’ erreur */
		if ( socket_client == -1){
			perror ("accept");
		}
		/* On peut maintenant dialoguer avec le client */
		sleep(2);
		write (socket_client,message_bienvenue,strlen(message_bienvenue));
		if(fork() != 0){
			close(socket_client);
		}
		else{
			while(1){
				toto = read(socket_client,&buff,256);
				if(toto==-1){
					close(socket_client);
					break;
				}
				toto = write(socket_client,buff,toto);
				if(toto==-1 || toto==0){
		  			close(socket_client);
					break;
				}
	    			else if(pid<0){
	     				 perror("pid");
	    			}
			}
			exit(1);
		}	
	}
	return value;
}
