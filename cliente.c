#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define EXIT_VALUE 7

int valread;
int valsend;

void listServerFiles(int client_socket)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    // Enviar solicitud para listar archivos
    strcpy(buffer, "LIST");
    valsend = send(client_socket, buffer, strlen(buffer), 0);
    if (valsend < 0)
    {
        perror("No se pudo enviar el comando LIST.\n");
        exit(EXIT_FAILURE);
    }

    memset(buffer, '\0', strlen(buffer));

    valread = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (valread < 0)
    {
        perror("Error al recibir los datos del servidor.");
        exit(EXIT_FAILURE);
    }

    printf("%s", buffer);
}

void createFolder(int client_socket)
{
    char folder_name[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    printf("Ingrese el nombre de la carpeta que desea crear: ");
    scanf("%s", folder_name);

    strcpy(buffer, "CREATE_FOLDER ");
    strcat(buffer, folder_name);
    strcat(buffer, " ");
    valsend = send(client_socket, buffer, strlen(buffer), 0);
    if (valsend < 0)
    {
        perror("No se pudo enviar el comando CREATE_FOLDER.\n");
        exit(EXIT_FAILURE);
    }

    // Recibir respuesta del servidor
    memset(buffer, '\0', BUFFER_SIZE);
    valread = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (valread < 0)
    {
        perror("Error al recibir los datos del servidor.");
        exit(EXIT_FAILURE);
    }

    printf("Respuesta del servidor: %s\n", buffer);
}

int main(void)
{
    int client_socket;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char buffer[BUFFER_SIZE];
    int choice;

    // Crear socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("Error al crear el socket del cliente");
        exit(EXIT_FAILURE);
    }

    // Obtener la dirección IP del servidor
    server = gethostbyname("localhost");
    if (server == NULL)
    {
        perror("Error al obtener la dirección IP del servidor");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(PORT);

    // Conectar al servidor
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Error al conectar al servidor");
        exit(EXIT_FAILURE);
    }

    // Menú de opciones
    do
    {
        printf("\n=== Menú ===\n");
        printf("1. Listar archivos en el servidor.\n");
        printf("2. Crear carpeta.\n");
        printf("3. Eliminar carpeta(s)/archivo(s).\n");
        printf("4. Cambiar la ruta de directorio.\n");
        printf("5. Cargar carpeta(s)/archivo(s).\n");
        printf("6. Descargar carpeta(s)/archivo(s).\n");
        printf("7. Salir\n");
        printf("Ingrese su elección: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                printf("\n");
                listServerFiles(client_socket);
                break;
            case 2:
                printf("\n");
                createFolder(client_socket);
                break;
            case 3:
                printf("Por implementar...\n");
                break;
            case 4:
                printf("Por implementar...\n");
                break;
            case 5:
                printf("Por implementar...\n");
                break;
            case 6:
                printf("Por implementar...\n");
                break;
            case EXIT_VALUE:
                printf("Saliendo, terminando programa...\n");
                break;
            default:
                printf("Opción no válida. Por favor, ingrese una opción válida.\n");
        }
    } while (choice != EXIT_VALUE);

    close(client_socket);
    return 0;
}
