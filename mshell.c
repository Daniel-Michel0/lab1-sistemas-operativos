#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <sys/wait.h>
#include <signal.h>

/*
	Cosas que arreglar:
	*arreglado*-Hay que evitar el segmentation fault al solamente usar \n
	 esto *probablemente* se debe al strcmp del exit, pero son las 2 am y mi cerebro no da para pensar la solucion xd
	*arreglado*-Exit no funciona siempre, ni idea porque
	*arreglado*-El manejador de ctrlc bugea feo el terminal tras usar ctrlc despues de algun comando no valido
	-El manejador de ctrlc a veces se bugea teniendo que ser usado 2 veces
	-Se supone que usar printf en un manejador de señales es mala practica y deberia cambiarse

	Cosas que estan listas:
	-parte 1 1,2 (hay que cambiar el malloc para que se use de acuerdo a la cantidad de entradas que hay, se podria ver los " " (espacios) 
	 y reservar memoria de acuerdo a eso), 3, 4, 5 (no he probado extensamente), 6 , 7
	-parte 2 2
*/


char **getInput(char *input){
	char **comando = malloc(8 * sizeof(char*));				// Alocamos memoria (deberia ser muy grande, cambiar el 8)
	char *aux;												// para cumplir la parte a) 2)
	int indice = 0;

	aux = strtok(input," ");								
	while(aux != NULL){										// Separamos el input en un arreglo
		comando[indice] = aux;
		indice++;
		aux = strtok(NULL," ");
	}

	comando[indice] = NULL;
	return comando;											// Limpiamos la cola del arreglo(ultima posicion + 1) y lo devolvemos
}

void ctrlc(){
	char aux;
	printf("Presione x si desea cerrar la aplicacion\n");
	aux = getchar();
	if(aux == 'x' || aux == 'X') exit(0);
}

int main(){
	char **comando;
	char *input;
	pid_t child_pid;
	int stat_loc;
	
	signal(SIGINT, ctrlc); 								// Ignora ctrl-c

	while(1){
		input = readline("bencinera :) ");					// funny joke
		comando = getInput(input);							// Obtenemos el input
		if(comando[0] != NULL){								// Evita segmentation fault de strcmp
			if(strcmp(comando[0], "exit") == 0){
				exit(0);
			}
		}

		child_pid = fork();									// Creamos el hijo
		if(child_pid == -1){
			perror("error de fork.");
		}else if(child_pid == 0){
			if(execvp(comando[0],comando) == -1){
				// perror(comando[0]);
				printf("%s no es un comando o archivo valido.\n", comando[0]);
				kill(getpid(), SIGKILL);
			}
		}else{
			waitpid(child_pid, &stat_loc, WUNTRACED);
		}
		free(input);
		free(comando);

	}

	return 0;
}