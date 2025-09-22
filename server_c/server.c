// server.c
// Compilare: gcc server.c -o server -pthread
// Esegui: ./server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>

#define PORT 12345
#define BACKLOG 8
#define BUF_SIZE 4096

int server_fd = -1;
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

// Funzione per stampa thread-safe
void safe_print(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    pthread_mutex_lock(&print_mutex);
    vprintf(fmt, args);
    printf("\n");
    pthread_mutex_unlock(&print_mutex);
    va_end(args);
}

// Chiusura server pulita
void handle_exit(int sig) {
    if (server_fd != -1) {
        close(server_fd);
    }
    safe_print("Server terminato (signal %d). Socket chiuso.", sig);
    exit(0);
}

// Funzione semplice per estrarre un valore float da un buffer per una chiave
float extract_trait(const char *buffer, const char *trait) {
    char *pos = strstr(buffer, trait);
    if (!pos) return -1;
    float val = -1;
    sscanf(pos, "%*[^:]: %f", &val);
    return val;
}

// Funzione per decidere il comportamento
void decide_behavior(float estro, float stabile, float amical, float coscienz, float apertura,
                     char *behavior, size_t beh_size, char *speech, size_t speech_size) {
    // Logica semplice: comportamento basato su estroversione e stabilità emotiva
    if (estro >= 5) strcpy(behavior, "estroverso");
    else if (estro <= 3) strcpy(behavior, "introverso");
    else strcpy(behavior, "neutrale");

    if (stabile <= 3) strcat(behavior, "_calmo");
    
    snprintf(speech, speech_size,
             "Estroversione: %.1f, Stabilità: %.1f, Amicalità: %.1f, Coscienziosità: %.1f, Apertura: %.1f",
             estro, stabile, amical, coscienz, apertura);
}

// Thread per ogni client
void *client_thread(void *arg) {
    int client_sock = *(int *)arg;
    free(arg);

    char buffer[BUF_SIZE];
    ssize_t read_bytes;

    while ((read_bytes = recv(client_sock, buffer, sizeof(buffer)-1, 0)) > 0) {
        buffer[read_bytes] = '\0';
        safe_print("📥 Messaggio ricevuto (%ld bytes): %s", (long)read_bytes, buffer);

        char response[1024];
        char behavior[64] = "neutrale";
        char speech[512] = "Ricevuto.";

        if (strstr(buffer, "'traits'") != NULL) {
            // Estrai i 5 tratti
            float estro = extract_trait(buffer, "Estroversione");
            float stabile = extract_trait(buffer, "Stabilità emotiva");
            float amical = extract_trait(buffer, "Amicalità");
            float coscienz = extract_trait(buffer, "Coscienziosità");
            float apertura = extract_trait(buffer, "Apertura");

            decide_behavior(estro, stabile, amical, coscienz, apertura,
                            behavior, sizeof(behavior), speech, sizeof(speech));
        }

        snprintf(response, sizeof(response),
                 "{ \"status\": \"ok\", \"behavior\": \"%s\", \"speech\": \"%s\" }",
                 behavior, speech);

        ssize_t sent = send(client_sock, response, strlen(response), 0);
        if (sent < 0) {
            safe_print("Errore nell'invio della risposta.");
        } else {
            safe_print("📤 Risposta inviata (%ld bytes).", (long)sent);
        }
    }

    if (read_bytes == 0) {
        safe_print("Connessione client chiusa dal client.");
    } else {
        safe_print("Errore nella recv.");
    }

    close(client_sock);
    return NULL;
}

int main() {
    signal(SIGINT, handle_exit);
    signal(SIGTERM, handle_exit);

    struct sockaddr_in address;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    safe_print("✅ Server in ascolto sulla porta %d ...", PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);
        int *client_sock = malloc(sizeof(int));
        if (!client_sock) {
            safe_print("Memoria insufficiente per accept");
            continue;
        }

        *client_sock = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
        if (*client_sock < 0) {
            safe_print("accept fallita");
            free(client_sock);
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        safe_print("Connessione accettata da %s:%d", client_ip, ntohs(client_addr.sin_port));

        pthread_t tid;
        if (pthread_create(&tid, NULL, client_thread, client_sock) != 0) {
            safe_print("Impossibile creare thread per client");
            close(*client_sock);
            free(client_sock);
            continue;
        }
        pthread_detach(tid);
    }

    close(server_fd);
    return 0;
}
