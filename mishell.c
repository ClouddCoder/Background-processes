/*
 * Este programa gestiona los procesos que se ejecutan en background.
 * Codigo tomado de: https://www.programiz.com/c-programming/c-strings
 *
 * Modificado por: Brayan Sanchez - brayan.sanchez.leon@correounivalle.edu.co / Mayra Sanchez - mayra.alejandra.sanchez@correounivalle.edu.co
 * Fecha: 2022-06-06
 */
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "leercadena.h"

/**
 * @brief Gestiona los procesos en background.
 *
 * @param argc comando que ingresa el usuario
 * @param argv argumento del comando ingresado
 * @return int
 */
int main(int argc, char *argv[]) {
    char command[256];
    char **comando;
    int posicion = 0, status, child_pids[3], child_nb = 0, eliminar;
    bool bg;

    while (1) {
        printf("> ");
        leer_de_teclado(256, command);
        if (strcmp(command, "salir") == 0) {
            break;
        } else {
            // Si el usuario escribe "tareas" muestra los procesos que se encuentran en background
            if (strcmp(command, "tareas") == 0) {
                for (int i = 0; i < child_nb; i++)
                    printf("%d\n", child_pids[i]);
            }
        }

        comando = de_cadena_a_vector(command);

        // Determina el tamaño de char **comando
        while (comando[posicion] != NULL) {
            posicion++;
        }

        if (posicion > 1) {
            // Si se ingresa un ampersand, bg (background) se convierte en true
            if (strcmp(comando[posicion - 1], "&") == 0)
                bg = true;

            // Detiene de manera definitiva un proceso por medio de su identificador
            if (strcmp(comando[0], "detener") == 0) {
                char pid[256];
                int borrarElemento;
                for (int i = 0; i < child_nb; i++) {
                    // Convierte en string el pid del proceso para hacer la comparacion con el argumento ingresado por el usuario
                    sprintf(pid, "%d", child_pids[i]);
                    // Revisa si el proceso ingresado existe para detenerlo
                    if (strcmp(comando[1], pid) == 0) {
                        kill(child_pids[i], SIGKILL);
                        borrarElemento = i;
                        eliminar = 1;
                        printf("El proceso %d ha sido finalizado\n", child_pids[i]);
                        break;
                    } else
                        eliminar = 0;
                }

                // Borra el pid del array de procesos en background
                if (eliminar == 1) {
                    for (int pos = borrarElemento; pos < child_nb; pos++) {
                        // Muestra un 0 en la posicion que no tiene elemento
                        if (pos == child_nb - 1)
                            child_pids[pos] = 0;
                        else
                            child_pids[pos] = child_pids[pos + 1];
                    }
                } else
                    printf("El proceso %s no existe\n", comando[1]);
            }
        }

        int rc = fork();
        assert(rc >= 0);
        if (rc == 0) {
            // Si el argumento es un ampersand, se reemplaza por un NULL
            if (strcmp(comando[posicion - 1], "&") == 0 && posicion > 1) {
                comando[posicion - 1] = NULL;
            }

            execvp(comando[0], comando);
            // Captura el error en caso de que no se ejecute el execvp y se lo comunica al padre
            _exit(EXIT_FAILURE);
        } else {
            if (!bg) {
                // Si no es un proceso en background, el padre debe esperar la finalizacion del hijo
                waitpid(rc, &status, 0);
            } else {
                // Si es un proceso en background, el padre no espera la finalizacion del hijo
                waitpid(rc, &status, WNOHANG);
                // Se agregan los procesos en background al array child_pids[]
                child_pids[child_nb] = rc;
                child_nb++;
            }
        }
        posicion = 0;
        bg = false;
    }
    return 0;
}
