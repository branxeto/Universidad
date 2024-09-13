#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <cstdlib>
#include <iostream>
#include <sys/stat.h>

using namespace std;

int main() {
    printf("Proceso Origen: %d\n", getpid());
    printf("Ingrese cantidad de jugadores: ");
    fflush(stdout);

    int cant_jugadores;
    scanf("%d", &cant_jugadores);
    int id_jugadores[cant_jugadores];

    //<-------------- Creación de los named pipes -------------->
    int fd;
    const char* fifo_path = "./mensajes";
    if (mkfifo(fifo_path, 0666) == -1) {
        perror("mkfifo");
        exit(-1);
    }

    //<-------- Creación jerárquica de los procesos jugadores ------------>
    pid_t pid = fork();
    if(pid == 0){ //Procesos jugadores
        for (int i = 0; i < cant_jugadores; i++) {
            pid = fork();
            if (pid == 0) {
                // Proceso hijo (jugador)
                printf("Proceso hijo %d: %d\n", i, getpid());
                fflush(stdout);

                int id = i;
                //<------- Creación de pipes con proceso observador ---------->
                fd = open(fifo_path, O_RDWR);
                if (fd == -1) {
                    perror("fd");
                    exit(-1);
                }

                //<---------- Proceso de votación ------------->

                //<-- variables iniciales -->
                int jugadores[cant_jugadores]; // Arreglo de jugadores, 0 si esta jugando y 1
                int voto; // Voto del jugador
                int finalizado = -1; // Jugador finalizado
                int tiempo; // iteraciones para generar votos



                while (finalizado != id) {
                    sleep(3);
                    //recivir cantidad de votos por realizar (paso 1)
                    read(fd, &tiempo, sizeof(tiempo));

                    //generar votos y mandarlos al observador (paso 2)
                    for(int i = 0; i < tiempo; i++){
                        voto = rand() % cant_jugadores;
                        if (voto == id) {
                            voto = rand() % cant_jugadores;
                        }
                        write(fd, &voto, sizeof(voto)); // Mandar voto
                        printf("Jugador %d votó por: %d\n", id, voto);
                        fflush(stdout);
                    }

                    sleep(5);//esperar a que el observador recopile los votos

                    read(fd, &finalizado, sizeof(finalizado)); // Recibir resultado (paso 3)
                    sleep(3);
                    jugadores[finalizado] = 1;
                }

                printf("Jugador %d amurrado\n", i);
                fflush(stdout);
                close(fd);
                exit(0);
            }
        }
    }else if(pid > 0){
        //<---------- Creación del proceso observador --------------->
        pid = fork();
        if (pid == 0) { //proceso observador
            printf("Proceso observador: %d\n", getpid());
            fflush(stdout);

            // <---------- Creación de pipes con procesos jugadores -------->
            fd = open(fifo_path, O_RDWR);
            if (fd == -1) {
                perror("fd");
                exit(-1);
            }

            int id_jugadores[cant_jugadores];
            int jugador_sel;
            int contador = 1;
            sleep(2);
            while (cant_jugadores != 1) {
                
                printf("Juego numero: %d\n", contador);

                //enviar cantidad de iteraciones a los jugadores (paso 1)
                int tiempo;
                tiempo = rand() % 10;
                printf("Tiempo de la canción: %d\n", tiempo);
                for(int i = 0; i < cant_jugadores; i++){
                    write(fd, &tiempo, sizeof(tiempo));
                }
                sleep(5);

                // Recopilar los votos de los jugadores (paso 2)
                for (int i = 0; i < tiempo; i++) {
                    read(fd, &jugador_sel, sizeof(jugador_sel));
                    id_jugadores[jugador_sel] += 1;
                }
                sleep(2);

                // Conseguir el id más grande
                int mayor = 0;
                int id_mayor;
                for (int i = 0; i < cant_jugadores; i++) {
                    if (id_jugadores[i] > mayor) {
                        mayor = id_jugadores[i];
                        id_mayor = i;
                    }
                }

                // Enviar el id más grande a los jugadores (paso 3)
                for(int i = 0; i < cant_jugadores; i++){
                    write(fd, &id_mayor, sizeof(id_mayor));
                }
                printf("Jugador más votado: %d\n", id_mayor);
                fflush(stdout);

                // Limpiar el arreglo de votos
                for (int i = 0; i < cant_jugadores; i++) {
                    id_jugadores[i] = 0;
                }

                cant_jugadores -= 1;
                contador += 1;
            }



            printf("Juego terminado.\n");
            fflush(stdout);
            close(fd);
            unlink(fifo_path);
            exit(0);
        }
    }
    return 0;
}
