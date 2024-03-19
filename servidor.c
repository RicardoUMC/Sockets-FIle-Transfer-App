#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <dirent.h> // Para trabajar con directorios

#define PORT 8080
#define BUFFER_SIZE 1024

int valread;
int valsend;

void listFiles(int client_socket)
{
    char buffer[BUFFER_SIZE];
    DIR *directory;
    struct dirent *entry;

    directory = opendir(".");
    if (directory == NULL)
    {
        perror("Error al abrir el directorio");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(directory)) != NULL)
    {
        memset(buffer, '\0', BUFFER_SIZE);
        strcpy(buffer, entry->d_name);
        strcat(buffer, "\n");

        valsend = send(client_socket, buffer, strlen(buffer), 0);
        if (valsend < 0)
        {
            perror("Error al enviar datos al cliente");
            exit(EXIT_FAILURE);
        }
    }

    // Enviar marca de finalización
    memset(buffer, '\0', BUFFER_SIZE);
    send(client_socket, buffer, 1, 0);

    closedir(directory);
}

int main(void)
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Crear socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Error al crear el socket del servidor");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Vincular el socket a la dirección
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Error al vincular el socket a la dirección");
        exit(EXIT_FAILURE);
    }

    // Escuchar por conexiones
    if (listen(server_socket, 5) == -1)
    {
        perror("Error al escuchar por conexiones");
        exit(EXIT_FAILURE);
    }

    printf("Servidor escuchando en el puerto %d...\n", PORT);

    while (1) 
    {
        // Aceptar la conexión entrante
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1) 
        {
            perror("Error al aceptar la conexión entrante");
            exit(EXIT_FAILURE);
        }
        
        printf("Cliente conectado\n");
        
        while (1)
        {
            // Manejar la solicitud del cliente
            char buffer[BUFFER_SIZE];
            memset(buffer, '\0', BUFFER_SIZE);

            printf("Esperando siguiente instrucción...\n");
            recv(client_socket, buffer, BUFFER_SIZE, 0);
            printf("recibido:  %s  longitud:%d \n",buffer,(int)strlen(buffer));
            buffer[strlen(buffer)]='\0';
            
            if (strcmp(buffer, "LIST") == 0) 
            {
                printf("Enlistando...\n");
                listFiles(client_socket);
                printf("Terminando...\n");
            }

            else
            {
                close(client_socket);
                printf("Cliente desconectado\n");
            }
        }
    }

    close(server_socket);
    return 0;
}
