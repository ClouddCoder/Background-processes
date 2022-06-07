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
        if (strcmp(command, "salir") == 0)
            break;

        int length = sizeof(child_pids) / sizeof(child_pids[0]);

        if (strcmp(command, "tareas") == 0) {
            for (int i = 0; i < length; i++)
                printf("%d\n", child_pids[i]);
        }

        comando = de_cadena_a_vector(command);

        // Determina el tamaño de char **comando
        while (comando[posicion] != NULL) {
            posicion++;
        }

        if (posicion > 0) {
            // Si se ingresa un ampersand, bg (background) se convierte en true
            if (strcmp(comando[posicion - 1], "&") == 0)
                bg = true;

            // Tamaño del array de procesos en background
            //int length = sizeof(child_pids) / sizeof(child_pids[0]);

            // Si el usuario escribe "tareas" muestra los procesos que se encuentran en background

            /*
            if (strcmp(comando[0], "tareas") == 0) {
                for (int i = 0; i < length; i++)
                    printf("%d\n", child_pids[i]);
            }
            */

            // Detiene de manera definitiva un proceso por medio de su identificador
            if (strcmp(comando[0], "detener") == 0) {
                int borrarElemento;
                for (int i = 0; i < length; i++) {
                    char pid[256];
                    sprintf(pid, "%d", child_pids[i]);
                    // Revisa si el proceso ingresado existe para detenerlo
                    if (strcmp(comando[1], pid) == 0) {
                        kill(child_pids[i], SIGKILL);
                        borrarElemento = i;
                        eliminar = 1;
                        printf("El proceso ha sido finalizado\n");
                        // printf("%d\n", b);
                    } else {
                        printf("El proceso no existe\n");
                        eliminar = 0;
                    }
                    break;
                }

                // Borra el pid del array de procesos en background
                if (eliminar == 1) {
                    for (int pos = borrarElemento; pos < length; pos++) {
                        if (pos == length - 1)
                            child_pids[pos] = 0;
                        else
                            child_pids[pos] = child_pids[pos + 1];
                    }
                }
            }
        }

        int rc = fork();
        assert(rc >= 0);
        if (rc == 0) {
            // Si el argumento es un ampersand, se reemplaza por un NULL
            if (strcmp(comando[posicion - 1], "&") == 0 && posicion > 0) {
                comando[posicion - 1] = NULL;
            }
            execvp(comando[0], comando);
        } else {
            if (!bg) {
                // Si no es un proceso en background, el padre debe esperar la finalizacion del hijo
                waitpid(rc, NULL, 0);
                // printf("primero %d\n", rc);
            } else {
                // Si es un proceso en background, el padre no espera la finalizacion del hijo
                waitpid(rc, &status, WNOHANG);
                // printf("segundo %d\n", rc);
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
