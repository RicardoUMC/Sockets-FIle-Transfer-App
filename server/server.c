#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#define PORT 8080
#define BUFFER_SIZE 1024

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

    valsend = send(client_socket, buffer, strlen(buffer), 0);
    if (valsend < 0)
    {
        perror("Error al enviar datos al cliente");
        exit(EXIT_FAILURE);
    }

    closedir(directory);
}

void createFolder(int client_socket, char *folder_name)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    if (mkdir(folder_name, 0777) == 0)
    {
        strcpy(buffer, "Carpeta creada exitosamente.");
    }
    else
    {
        strcpy(buffer, "Error al crear la carpeta.");
    }

    valsend = send(client_socket, buffer, strlen(buffer), 0);
    if (valsend < 0)
    {
        perror("Error al enviar datos al cliente");
        exit(EXIT_FAILURE);
    }
}

void deleteFolderFile(int client_socket, char *name)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    // Eliminar carpeta/archivo
    if (remove(name) == 0)
    {
        sprintf(buffer, "Eliminación exitosa de %s.", name);
    }
    else
    {
        sprintf(buffer, "Error al eliminar %s.", name);
    }

    // Enviar respuesta al cliente
    valsend = send(client_socket, buffer, strlen(buffer), 0);
    if (valsend < 0)
    {
        perror("Error al enviar datos al cliente");
        exit(EXIT_FAILURE);
    }
}

void changeDirectory(int client_socket, char *new_directory)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    // Intentar cambiar el directorio
    if (chdir(new_directory) == 0)
    {
        strcpy(buffer, "Directorio cambiado exitosamente.");
    }
    else
    {
        strcpy(buffer, "Error al cambiar el directorio.");
    }

    // Enviar respuesta al cliente
    int valsend = send(client_socket, buffer, strlen(buffer), 0);
    if (valsend < 0)
    {
        perror("Error al enviar datos al cliente");
        exit(EXIT_FAILURE);
    }

    sleep(1);
    char *cwd;
    memset(buffer, '\0', BUFFER_SIZE);
    cwd = getcwd(buffer, sizeof(buffer));
    send(client_socket, cwd, strlen(cwd), 0);
}

void receiveFileFromClient(int client_socket)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    struct FileMetadata metadata;

    // Recibir metainformación del cliente
    int valread = read(client_socket, &metadata, sizeof(struct FileMetadata));
    if (valread < 0)
    {
        perror("Error al recibir metadatos del servidor");
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(metadata.filename, "w");
    if (file == NULL)
    {
        perror("Error al abrir el archivo para escritura");
        exit(EXIT_FAILURE);
    }

    // Recibir el archivo del cliente
    long bytes_received = 0;
    while (bytes_received < metadata.filesize)
    {
        memset(buffer, '\0', BUFFER_SIZE);
        valread = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (valread < 0)
        {
            perror("Error al recibir el archivo del cliente");
            exit(EXIT_FAILURE);
        }

        fwrite(buffer, 1, valread, file);
        bytes_received += valread;
    }

    // Enviar respuesta al cliente
    memset(buffer, '\0', BUFFER_SIZE);
    strcpy(buffer, "Archivo recibido exitosamente.");
    int valsend = send(client_socket, buffer, strlen(buffer), 0);
    if (valsend < 0)
    {
        perror("Error al enviar datos al cliente");
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

void sendFileToClient(int client_socket, char *file_name)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    struct FileMetadata metadata;
    FILE *file = fopen(file_name, "r");
    if (file == NULL)
    {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    // Obtener el nombre y tamaño del archivo
    strcpy(metadata.filename, file_name);
    fseek(file, 0, SEEK_END);
    metadata.filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Enviar metainformación al cliente
    printf("[Enviando metainformación al cliente]\n");
    valsend = write(client_socket, &metadata, sizeof(struct FileMetadata));
    if (valsend < 0)
    {
        perror("Error al enviar metadatos al cliente");
        exit(EXIT_FAILURE);
    }

    // Enviar el archivo al cliente
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
            perror("Error al enviar el archivo al cliente.");
            exit(EXIT_FAILURE);
        }
    }

    fclose(file);

}

int main(void)
{
    char buffer[BUFFER_SIZE];
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Error al crear el socket del servidor");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Error al vincular el socket a la dirección");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) == -1)
    {
        perror("Error al escuchar por conexiones");
        exit(EXIT_FAILURE);
    }

    printf("Servidor escuchando en el puerto %d...\n", PORT);

    while (1) 
    {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1) 
        {
            perror("Error al aceptar la conexión entrante");
            exit(EXIT_FAILURE);
        }
        
        memset(buffer, '\0', BUFFER_SIZE);
        char *cwd = getcwd(buffer, sizeof(buffer));
        send(client_socket, cwd, strlen(cwd), 0);
        
        printf("\n  -> Cliente conectado\n");
        
        while (1)
        {
            memset(buffer, '\0', BUFFER_SIZE);

            printf("Esperando siguiente instrucción...\n");
            recv(client_socket, buffer, BUFFER_SIZE, 0);
            buffer[strlen(buffer)]='\0';
            
            if (strcmp(buffer, "LIST") == 0) 
            {
                listFiles(client_socket);
                printf("  -> Directorios y archivos enlistados exitosamente.\n");
            }
            else if (strncmp(buffer, "CREATE_FOLDER", strlen("CREATE_FOLDER")) == 0)
            {
                char *folder_name = strtok(buffer, " ");
                folder_name = strtok(NULL, " ");
                createFolder(client_socket, folder_name);
                printf("  -> Carpeta creada exitosamente.\n");
            }
            else if (strncmp(buffer, "DELETE", strlen("DELETE")) == 0)
            {
                char *file_name = strtok(buffer, " ");
                file_name = strtok(NULL, " ");
                deleteFolderFile(client_socket, file_name);
                printf("  -> Carpeta/archivo eliminado exitosamente.\n");
            }
            else if (strncmp(buffer, "CHANGE_DIR", strlen("CHANGE_DIR")) == 0)
            {
                char *dir_name = strtok(buffer, " ");
                dir_name = strtok(NULL, " ");
                changeDirectory(client_socket, dir_name);
                printf("  -> Cambio de directorio exitosamente.\n");
            }
            else if (strncmp(buffer, "UPLOAD", strlen("UPLOAD")) == 0)
            {
                receiveFileFromClient(client_socket);
                printf("  -> Carpeta/archivo cargado exitosamente.\n");
            }
            else if (strncmp(buffer, "DOWNLOAD", strlen("DOWNLOAD")) == 0)
            {
                char *file_name = strtok(buffer, " ");
                file_name = strtok(NULL, " ");
                sendFileToClient(client_socket, file_name);
                printf("  -> Carpeta/archivo descargado exitosamente.\n");
            }
            else if (strcmp(buffer, "DISCONNECT") == 0)
            {
                close(client_socket);
                printf("  -> Cliente desconectado\n");
                break;
            }
            else
            {
                printf("  -> OPERACIÓN NO RECONOCIDA\n");
                break;
            }
        }
    }

    close(server_socket);
    return 0;
}
