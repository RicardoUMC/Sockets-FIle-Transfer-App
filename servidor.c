#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <dirent.h> // Para trabajar con directorios
#include <sys/stat.h> // Para la función mkdir

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

    memset(buffer, '\0', BUFFER_SIZE);
    while ((entry = readdir(directory)) != NULL)
    {
        strcat(buffer, entry->d_name);
        strcat(buffer, "\n");

    }

    // Enviar marca de finalización
    valsend = send(client_socket, buffer, strlen(buffer), 0);
    if (valsend < 0)
    {
        perror("Error al enviar datos al cliente");
        exit(EXIT_FAILURE);
    }

    /*memset(buffer, '\0', BUFFER_SIZE);
    strcpy(buffer, "Directorios y archivos enlistados exitosamente.");
    // Enviar respuesta al cliente
    valsend = send(client_socket, buffer, strlen(buffer), 0);
    if (valsend < 0)
    {
        perror("Error al enviar datos al cliente");
        exit(EXIT_FAILURE);
    }*/

    closedir(directory);
}

void createFolder(int client_socket, char *folder_name)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    // Intentar crear la carpeta
    if (mkdir(folder_name, 0777) == 0)
    {
        strcpy(buffer, "Carpeta creada exitosamente.");
    }
    else
    {
        strcpy(buffer, "Error al crear la carpeta.");
    }

    // Enviar respuesta al cliente
    valsend = send(client_socket, buffer, strlen(buffer), 0);
    if (valsend < 0)
    {
        perror("Error al enviar datos al cliente");
        exit(EXIT_FAILURE);
    }
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
                listFiles(client_socket);
                printf("Directorios y archivos enlistados exitosamente.\n");
            }
            else if (strncmp(buffer, "CREATE_FOLDER", strlen("CREATE_FOLDER")) == 0)
            {
                char *folder_name = strtok(buffer, " ");
                folder_name = strtok(NULL, " ");
                createFolder(client_socket, folder_name);
                printf("Carpeta creada exitosamente.\n");
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
