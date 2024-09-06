#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <cstdlib>
#include <iostream>
#include <sys/stat.h>

using namespace std;

int main(){
    printf("Proceso Origen: %d\n", getpid());
    printf("Ingrese cantidad de jugadores: ");
    fflush(stdout);

    int cant_jugadores;
    scanf("%d", &cant_jugadores);
    int id_jugadores [cant_jugadores];

    //<-------------- Creacion de los named pipes -------------->
    int fd;

    if(mkfifo("./mi_fifo", 0666) == -1){
        perror("./mkfifo");
        exit(-1);
    }

    //<--------creacion de los hijos------------>
    for(int i = 0; i < cant_jugadores; i++){
        if(fork() == 0){
            printf("Proceso hijo %d: %d\n", i, getpid());
            fflush(stdout);
            int id = i;
            //<------- creacion de pipes con proceso observador ---------->
            fd = open("./mi_fifo", O_RDWR);
            if(fd == -1){
                perror("fd");
                exit(-1);
            }
            //<---------- procesos de votacion ------------->
            int jugadores[cant_jugadores];

            int voto;
            int finalizado = -1;
            while(finalizado != id){
                do {
                    voto = rand()%cant_jugadores;
                }while(voto == id && jugadores[voto] == 1);

                printf("Jugador %d votó por: %d\n", id, voto);
                fflush(stdout);

                write(fd, &voto, sizeof(voto)); //mandar voto
                sleep(2);
                read(fd, &finalizado, sizeof(finalizado));//recivir resultado
                jugadores[finalizado] = 1;
            }
            printf("Jugador %d amurrado\n", i);
            fflush(stdout);
            close(fd);
            exit(0);
        }
    }

    //<----------proceso de creacion del proceso observador ---------------_>
    pid_t pid = fork();
    if(pid > 0){
        pid_t observador = fork();
        if(observador == 0){
            printf("Proceso observador: %d\n", getpid());
            fflush(stdout);
            // <---------- creacion de pipes con procesos jugadores -------->
            fd = open("./mi_fifo", O_RDWR);
            if(fd == -1){
                perror("fd");
                exit(-1);
            }

            int id_jugadores[cant_jugadores];
            int jugador_sel;
            
            while(cant_jugadores != 1){
                //recopilar los votos de los jugadores
                for(int i = 0; i < cant_jugadores; i++){
                    read(fd, &jugador_sel, sizeof(jugador_sel));
                    id_jugadores[jugador_sel] += 1;
                }

                //conseguir el id mas grande
                int mayor = 0;
                int id_mayor;
                for(int i = 0; i < cant_jugadores; i++){
                    if(id_jugadores[i] > mayor){
                        mayor = id_jugadores[i];
                        id_mayor = i;
                    }
                }

                //enviar el id mas grande al observador
                write(fd, &id_mayor, sizeof(id_mayor));
                printf("Jugador más votado: %d\n", id_mayor);
                fflush(stdout);

                //limpiar el arreglo de votos
                for(int i = 0; i < cant_jugadores; i++){
                    id_jugadores[i] = 0;
                }

                cant_jugadores -= 1;
            }


            printf("Juego terminado.\n");
            fflush(stdout);
            close(fd);
            exit(0);
        }
    }

    unlink("./mi_fifo");
    return 0;
}