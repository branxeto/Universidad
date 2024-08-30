#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <cstlib>
#include <iostream>

using namespace std;

int main(){
    cout<<"proceso Origen:" << getpid() << endl;
    cout<<"Ingrese cantidad de jugadores: ";
    int cant_jugadores;
    cin >> cant_jugadores;
    int id_jugadores [cant_jugadores];

    //<---------- Creacion de la variable compartida -------------->
    //esta variable count va a ser la misma en cualquier parte del codigo independientemente de que proceso este.
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, sizeof(int), 0666|IPC_CREAT);
    int *count = (int*) shmat(shmid, (void*)0, 0);
    *count = 0;

    //<-------------- Creacion de los named pipes -------------->
    int fd;
    if(mkfifo("/temp/mi_fifo", 0666) == -1){
        perror("mkfifo");
        exit(-1);
    }

    //<--------creacion de los hijos------------>
    for(int i = 0; i < cant_jugadores; i++){
        if(fork() == 0){
            int id = i;
            cout << "proceso hijo" << id << ": " << getpid() << endl;
            fd = open("/temp/mi_fifo", O_RDWR);
            if(fd == -1){
                perror("fd");
                exit(-1);
            }

            int voto;
            int finalizado;

            do {
                voto = rand()%10;
            }while(voto == id);
            ssize_t bytes_written = write(fd, &voto, sizeof(voto));
            sleep(2);
            ssize_t bytes_read = read(fd, &finalizado, sizeof(finalizado));


            exit(0);
        }
    }

    //<-------Espera a que los hijos se creen para que el observador se cree ------------>
    for(int i = 0; i < cant_jugadores + 1; i++){
        int status;
        wait(&status);
    }

    //<----------proceso de creacion del proceso observador ---------------_>
    pid_t pid = fork();
    if(pid > 0){
        pid_t observador = fork();
        if(observador == 0){
            cout << "proceso observador: " << getpid() << endl;
            fd = open("/temp/mi_fifo", O_RDWR);
            if(fd == -1){
                perror("fd");
                exit(-1);
            }
            

            exit(0);
        }
    }
    return 0;
}