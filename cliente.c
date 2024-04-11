#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <netdb.h>
#include <libgen.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define EXIT_VALUE 7

int valread;
int valsend;

struct FileMetadata
{
    char filename[256];
    long filesize;
};

void listFiles(int client_socket)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    printf("Ingrese donde desea realizar la operación (cliente/servidor): ");
    scanf("%s", buffer);

    if (strcmp(buffer, "cliente") == 0)
    {
        struct dirent *entry;
        DIR *directory = opendir(".");
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
        printf("%s", buffer);
        closedir(directory);
        return;
    }

    memset(buffer, '\0', BUFFER_SIZE);
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
    char folder_name[BUFFER_SIZE-strlen("CREATE_FOLDER")];
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    printf("Ingrese el nombre de la carpeta que desea crear (cliente/servidor): ");
    scanf("%s %[^\n]", buffer, folder_name);

    if (strcmp(buffer, "cliente") == 0)
    {
        // Crear la carpeta en el cliente
        if (mkdir(folder_name, 0777) == 0)
            printf("Carpeta creada exitosamente en el cliente.\n");
        else
            perror("Error al crear la carpeta en el cliente.\n");
        return;
    }

    printf("Ingrese el nombre de la carpeta que desea crear: ");
    scanf("%s", folder_name);

    sprintf(buffer, "CREATE_FOLDER %s", folder_name);
    valsend = send(client_socket, buffer, strlen(buffer), 0);
    if (valsend < 0)
    {
        perror("No se pudo enviar el comando CREATE_FOLDER.\n");
        exit(EXIT_FAILURE);
    }

    memset(buffer, '\0', BUFFER_SIZE);
    valread = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (valread < 0)
    {
        perror("Error al recibir los datos del servidor.");
        exit(EXIT_FAILURE);
    }

    printf("Respuesta del servidor: %s\n", buffer);
}

void deleteFolderFile(int client_socket)
{
    char name[BUFFER_SIZE-strlen("DELETE")];
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    printf("Ingrese el nombre de la carpeta/archivo que desea eliminar: ");
    scanf("%s", name);

    sprintf(buffer, "DELETE %s", name);
    valsend = send(client_socket, buffer, strlen(buffer), 0);
    if (valsend < 0)
    {
        perror("No se pudo enviar el comando DELETE.\n");
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

void changeDirectory(int client_socket)
{
    char new_directory[BUFFER_SIZE-strlen("CHANGE_DIR")];
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    printf("Ingrese la nueva ruta de directorio: ");
    scanf("%s", new_directory);

    // Enviar la solicitud al servidor
    sprintf(buffer, "CHANGE_DIR %s", new_directory);
    int valsend = send(client_socket, buffer, strlen(buffer), 0);
    if (valsend < 0)
    {
        perror("No se pudo enviar el comando CHANGE_DIR.\n");
        exit(EXIT_FAILURE);
    }

    // Recibir respuesta del servidor
    memset(buffer, '\0', BUFFER_SIZE);
    int valread = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (valread < 0)
    {
        perror("Error al recibir los datos del servidor.");
        exit(EXIT_FAILURE);
    }

    printf("Respuesta del servidor: %s\n", buffer);
}

void uploadFileToServer(int client_socket)
{
    char file_path[BUFFER_SIZE-strlen("UPLOAD")];
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    printf("Ingrese el nombre (ruta relativa o absoluta) del archivo/carpeta a enviar: ");
    scanf("%s", file_path);

    struct FileMetadata metadata;
    FILE *file = fopen(file_path, "r");
    if (file == NULL)
    {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    valsend = send(client_socket, "UPLOAD", strlen("UPLOAD"), 0);
    if (valsend < 0)
    {
        perror("Error al enviar instruccion al servidoor");
        exit(EXIT_FAILURE);
    }

    // Obtener el nombre y tamaño del archivo
    strcpy(metadata.filename, basename(file_path));
    fseek(file, 0, SEEK_END);
    metadata.filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Enviar metainformación al servidor
    valsend = write(client_socket, &metadata, sizeof(struct FileMetadata));
    if (valsend < 0)
    {
        perror("Error al enviar metadatos al servidor");
        exit(EXIT_FAILURE);
    }

    // Enviar el archivo al servidor
    while (!feof(file))
    {
        size_t bytes_read = fread(buffer, 1, BUFFER_SIZE, file);
        if (bytes_read < 0)
        {
            perror("Error al leer el archivo");
            exit(EXIT_FAILURE);
        }

        int valsend = send(client_socket, buffer, bytes_read, 0);
        if (valsend < 0)
        {
            perror("Error al enviar el archivo al servidor.");
            exit(EXIT_FAILURE);
        }
    }

    fclose(file);

    // Recibir respuesta del servidor
    memset(buffer, '\0', BUFFER_SIZE);
    int valread = recv(client_socket, buffer, BUFFER_SIZE, 0);
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

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("Error al crear el socket del cliente");
        exit(EXIT_FAILURE);
    }

    server = gethostbyname("localhost");
    if (server == NULL)
    {
        perror("Error al obtener la dirección IP del servidor");
        exit(EXIT_FAILURE);
    }

    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(PORT);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Error al conectar al servidor");
        exit(EXIT_FAILURE);
    }

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
                listFiles(client_socket);
                break;
            case 2:
                printf("\n");
                createFolder(client_socket);
                break;
            case 3:
                printf("\n");
                deleteFolderFile(client_socket);
                break;
            case 4:
                printf("\n");
                changeDirectory(client_socket);
                break;
            case 5:
                printf("\n");
                uploadFileToServer(client_socket);
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

