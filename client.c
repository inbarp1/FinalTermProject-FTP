#include "networking.h"
#define _XOPEN_SOURCE       /* See feature_test_macros(7) */
#include <unistd.h>
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <crypt.h>

int check_or_create_account(char * username, char * password, int server_socket, char * protocol);

//main client connection
void client(char * serverIP){
  char username[BUFFER_SIZE];
  char password[BUFFER_SIZE];
  int userId = 0;

  int server_socket;
  char buffer[BUFFER_SIZE];
  char file[BUFFER_SIZE];
  char filePath[BUFFER_SIZE];
  char fileContent[1024];
  int fd;
  //server_socket = client_setup( serverIP); ***

    //login code***
    /*printf("Do you have an account yet?(y/n)\n");
    memset(buffer, 0, sizeof(buffer));
    fgets(buffer, 256, stdin);
    if(buffer[0] == 'y'){
        while(!userId) {
            printf("Username: \n");
            fgets(username, 256, stdin);
            printf("Password: \n"); //make this hidden***
            fgets(password, 256, stdin);
            userId = check_or_create_account(username, password,server_socket,"CHECK");
            if (!userId) {
                printf("Error logging in. Please try again\n");
            }
        }
    }
    else{
        while(!userId) {
            printf("Please create a username: \n");
            fgets(username, 256, stdin);
            printf("Please type in a password: \n"); //make this hidden***
            fgets(password, 256, stdin);
            userId = check_or_create_account(username, password,server_socket,"CREAT");
            if (!userId) {
                printf("Username already exists please try again\n");
            }
        }
    }*/

    while (1) {
        printf("Would you like to push or pull (a file) or view available files in the FTP? (push/pull/view)\n");
        fgets(buffer, sizeof(buffer), stdin);
        *strchr(buffer, '\n') = 0;
        if(!strcmp("push",buffer)){ //push file code
            printf("What is the name of the file you are pushing?\n");
            fgets(file, sizeof(file), stdin);
            *strchr(file, '\n') = 0;
            write(server_socket, "PUSH", sizeof("PUSH")); //push request sent
            read(server_socket, buffer, sizeof(buffer));
            write(server_socket, file, sizeof(file)); //file name sent
            read(server_socket, buffer, sizeof(buffer));

            //file transfer code ***
            printf("What is the path to this file?\n");
            fgets(filePath, sizeof(filePath), stdin);
            *strchr(filePath, '\n') = 0;
            //accessing file contents
            if((fd = open(filePath, O_RDONLY)) < 0) //checks if file exists
                handle_error();
            read(fd, fileContent, sizeof(fileContent));
            //sending file contents
            write(server_socket, fileContent, sizeof(fileContent));
            close(fd);
        }
        else if(!strcmp("pull",buffer)){//pull file code
            printf("What is the name of the file you are pulling?\n");
            fgets(file, sizeof(file), stdin);
            *strchr(file, '\n') = 0;
            write(server_socket, "PULL", sizeof("PULL")); //pull request sent
            read(server_socket, buffer, sizeof(buffer));
            write(server_socket, file, sizeof(file)); //file name sent
            read(server_socket, buffer, sizeof(buffer));

            //file transfer code ***
            printf("Where would you like the file contents to be pulled?(enter a path to file)\n");
            fgets(filePath, sizeof(filePath), stdin);
            *strchr(filePath, '\n') = 0;
            fd = open(filePath, O_CREAT|O_WRONLY);
            //recieving file contents
            read(server_socket, fileContent, sizeof(fileContent));
            //writing into fd
            write(fd, fileContent, sizeof(fileContent));
            close(fd);
        }
        else if(!strcmp("view",buffer)){
            write(server_socket, "VIEW", sizeof("VIEW")); //view request sent

            //VIEWING CODE ***
            read(server_socket, stdout, sizeof(fileContent)); //get this to read to stdout***
        }
        else{
            printf("Please type in 'push', 'pull', or 'view'.");
        }
  }
}

/*=========================
  client_setup
  args: int * to_server

  to_server is a string representing the server address

  create and connect a socket to a server socket that is
  in the listening state

  returns the file descriptor for the socket
  =========================*/
int client_setup(char * server) {
    int sd, i;

    //create the socket
    sd = socket( AF_INET, SOCK_STREAM, 0 );
    error_check( sd, "client socket" );

    //run getaddrinfo
    /* hints->ai_flags not needed because the client
       specifies the desired address. */
    struct addrinfo * hints, * results;
    hints = (struct addrinfo *)calloc(1, sizeof(struct addrinfo));
    hints->ai_family = AF_INET;  //IPv4
    hints->ai_socktype = SOCK_STREAM;  //TCP socket
    getaddrinfo(server, PORT, hints, &results);

    //connect to the server
    //connect will bind the socket for us
    i = connect( sd, results->ai_addr, results->ai_addrlen );
    error_check( i, "client connect" );

    free(hints);
    freeaddrinfo(results);

    return sd;
}

void error_check( int i, char *s ) {
    if ( i < 0 ) {
        printf("[%s] error %d: %s\n", s, errno, strerror(errno) );
        exit(1);
    }
}

//returns userID if everything checks out
//protocol is either CHECK or CREAT
int check_or_create_account(char * username, char * password, int server_socket, char * protocol){
    char * buffer = (char *) malloc(256 * sizeof(char));
    int * retInt = (int *) malloc(sizeof(int));
    char * encrypted = crypt(password, "ab");
    //check file of encrypted passwords***

    write(server_socket, protocol, sizeof(protocol)); //sends either CHECK or CREAT
    read(server_socket, buffer, sizeof(buffer));
    write(server_socket, username, sizeof(username)); //sending username
    read(server_socket, buffer, sizeof(buffer));
    write(server_socket, encrypted, sizeof(encrypted)); //sending encrypted password
    read(server_socket, retInt, sizeof(retInt)); //sending verification in the form of userId
    return *retInt;
}