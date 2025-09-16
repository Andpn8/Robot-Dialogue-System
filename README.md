# 🤖 Robot Dialogue System

Progetto universitario per il corso **Laboratorio di Sistemi Operativi** – Università degli Studi di Napoli Federico II.  
Il sistema permette al robot **Furhat** di dialogare con una persona, adattando il comportamento in base alla personalità dell’interlocutore rilevata tramite il questionario TIPI.

---

## 📌 Specifiche del progetto
- **Server (C)**  
  - Gestisce le connessioni socket.  
  - Riceve i dati di personalità dal client.  
  - Applica la logica decisionale per determinare il comportamento del robot.  

- **Client (Python/Kotlin)**  
  - Somministra il questionario TIPI (Ten Item Personality Inventory).  
  - Calcola i punteggi dei tratti di personalità (Big Five).  
  - Comunica col server tramite socket.  
  - Interagisce con il robot **Furhat** tramite SDK o emulatore.  

- **Robot (Furhat)**  
  - Esegue i comportamenti ricevuti dal server: tono di voce, linguaggio, movimenti ed espressioni facciali.  

---

## 🛠️ Tecnologie utilizzate
- **C** → sviluppo del server.  
- **Python** → sviluppo del client.  
- **Furhat SDK** → simulazione.  
- **Git + GitHub** → version control e collaborazione.    

---

## 📂 Struttura della repository
/client_python # codice client in Python
/server_c # codice server in C
/docs # documentazione, relazioni, specifiche

---

## 👥 Team
- Andrea Pinto  
- Sebastiano Sannino  
- Ilaria Russo  

