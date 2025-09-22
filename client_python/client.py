#!/usr/bin/env python3
# client.py
# Esegui: python3 client.py

import socket

SERVER_HOST = "127.0.0.1"
SERVER_PORT = 12345
BUFFER_SIZE = 4096

# Domande TIPI in italiano
TIPI_QUESTIONS = [
    ("Sono socievole, estroverso", False),           # Estroversione positiva
    ("Mi sento spesso preoccupato o ansioso", True), # Stabilità emotiva invertita
    ("Mi affido agli altri e mi fido facilmente", False), # Amicalità positiva
    ("Mi organizzo e rispetto le regole", False),    # Coscienziosità positiva
    ("Mi piace provare cose nuove", False),          # Apertura positiva
    ("Cerco di essere attivo e vivace", False),      # Estroversione positiva
    ("A volte sono egoista o poco disponibile", True), # Amicalità invertita
    ("Tendo a rimandare e procrastinare", True),    # Coscienziosità invertita
    ("Mi sento nervoso e teso spesso", True),       # Stabilità emotiva invertita
    ("Sono curioso e interessato alle novità", False) # Apertura positiva
]

TRAITS_NAMES = ["Estroversione", "Stabilità emotiva", "Amicalità", "Coscienziosità", "Apertura"]

def send_payload(payload: dict):
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((SERVER_HOST, SERVER_PORT))
        s.sendall(str(payload).encode("utf-8"))
        resp = s.recv(BUFFER_SIZE).decode("utf-8")
        print("📥 Risposta dal server:", resp)
    except Exception as e:
        print("❌ Errore di connessione:", e)
    finally:
        try:
            s.close()
        except:
            pass

def menu():
    print("\n=== Client Python ===")
    print("1) Invia messaggio di test")
    print("2) Compila il questionario TIPI")
    print("0) Esci")
    choice = input("Scegli opzione: ").strip()
    return choice

def do_test_message():
    user_id = input("user_id (es. u1): ").strip() or "u1"
    msg = input("Messaggio da inviare: ").strip() or "Ciao dal client Python!"
    payload = {
        "user_id": user_id,
        "message": msg
    }
    print("Invio payload:", payload)
    send_payload(payload)

def invert_score(value: int) -> float:
    """Trasforma punteggio invertito: 8 - value"""
    return 8 - value

def do_tipi_questionnaire():
    print("\nCompila il questionario TIPI (scala 1-7, 1=completamente in disaccordo, 7=completamente d'accordo)")
    answers = []
    for i, (q, invert) in enumerate(TIPI_QUESTIONS, start=1):
        while True:
            try:
                val = int(input(f"{i}. {q}: ").strip())
                if 1 <= val <= 7:
                    if invert:
                        val = invert_score(val)
                    answers.append(val)
                    break
                else:
                    print("Inserisci un numero tra 1 e 7.")
            except ValueError:
                print("Valore non valido. Inserisci un numero tra 1 e 7.")

    # Calcola media dei 2 item per ciascun tratto
    traits = {
        "Estroversione": round((answers[0] + answers[5]) / 2, 2),
        "Stabilità emotiva": round((answers[1] + answers[8]) / 2, 2),
        "Amicalità": round((answers[2] + answers[6]) / 2, 2),
        "Coscienziosità": round((answers[3] + answers[7]) / 2, 2),
        "Apertura": round((answers[4] + answers[9]) / 2, 2)
    }

    user_id = input("user_id (es. u1): ").strip() or "u1"
    payload = {
        "user_id": user_id,
        "traits": traits
    }
    print("\nInvio i tratti calcolati:", payload)
    send_payload(payload)

def main():
    print("Client pronto. Server:", SERVER_HOST, "Porta:", SERVER_PORT)
    while True:
        choice = menu()
        if choice == "1":
            do_test_message()
        elif choice == "2":
            do_tipi_questionnaire()
        elif choice == "0":
            print("Esco.")
            break
        else:
            print("Scelta non valida.")

if __name__ == "__main__":
    main()
