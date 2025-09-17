import socket
import json

def main():
    # Configurazione server
    SERVER_HOST = "127.0.0.1"  # localhost (per test iniziale)
    SERVER_PORT = 12345        # stessa porta che useremo sul server

    # Creazione socket
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        # Connessione al server
        client_socket.connect((SERVER_HOST, SERVER_PORT))
        print(f"✅ Connesso al server {SERVER_HOST}:{SERVER_PORT}")

        # Messaggio di test (in formato JSON)
        message = {
            "user_id": "u1",
            "message": "Ciao dal client Python!"
        }
        json_message = json.dumps(message)

        # Invio messaggio
        client_socket.sendall(json_message.encode("utf-8"))
        print("📤 Messaggio inviato al server.")

        # Attesa risposta
        response = client_socket.recv(1024).decode("utf-8")
        print(f"📥 Risposta dal server: {response}")

    except Exception as e:
        print(f"❌ Errore: {e}")

    finally:
        client_socket.close()
        print("🔒 Connessione chiusa.")

if __name__ == "__main__":
    main()
