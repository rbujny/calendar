# System Współdzielonego Kalendarza Sieciowego

## Temat Zadania
**System Współdzielonego Kalendarza Sieciowego** to aplikacja pozwalająca na tworzenie użytkowników, dodawanie wydarzeń do ich kalendarzy oraz wyświetlanie zaplanowanych wydarzeń w tygodniu. System działa w modelu klient-serwer, gdzie serwer zarządza danymi, a klient zapewnia interfejs GUI dla użytkownika.

---

## Opis Protokołu Komunikacyjnego
Komunikacja między klientem a serwerem odbywa się za pomocą gniazd TCP. Protokół używa prostych poleceń tekstowych:

- `create_user <username>`  
  Tworzy nowego użytkownika.
- `add_event <username> <day> <event>`  
  Dodaje wydarzenie do kalendarza użytkownika na wskazany dzień tygodnia.
- `list_events <username>`  
  Wyświetla wszystkie wydarzenia użytkownika posortowane według dni tygodnia.

Odpowiedzi serwera są tekstowe i zawierają potwierdzenia operacji lub komunikaty o błędach.

---

## Opis Implementacji

### Serwer (C++)
- **Plik**: `server.cpp`
  - Inicjalizuje serwer nasłuchujący na porcie 1337.
  - Obsługuje wiele klientów jednocześnie za pomocą wątków.
  - Używa mutexów do synchronizacji dostępu do wspólnej struktury danych: kalendarzy użytkowników.
  - Kluczowe funkcje:
    - `createUser`: Tworzy nowego użytkownika.
    - `addEvent`: Dodaje wydarzenie do kalendarza użytkownika.
    - `listEvents`: Wyświetla wydarzenia użytkownika.

### Klient (Python)
- **Plik**: `client.py`
  - Implementuje graficzny interfejs użytkownika (GUI) w Tkinter.
  - Zapewnia funkcjonalność:
    - Tworzenia użytkowników.
    - Dodawania wydarzeń do kalendarza.
    - Wyświetlania zaplanowanych wydarzeń w formacie tekstowym.
  - Komunikuje się z serwerem za pomocą protokołu TCP.

---

## Sposób Kompilacji i Uruchamiania

### Serwer
* **Kompilacja**:
   ```bash
   g++ --std=c++20 -Wall -O0 -g -pthread -o server server.cpp
   ```
* **Uruchomienie**
   ```bash
   ./server
   ```
### Klient
* **Uruchomienie**
   ```bash
   python3 client.py
   ```
---
## Obsługa Projektu
1. **Uruchomienie serwera**
2. **Uruchomienie klienta**
3. **Działanie aplikacji**:
    * Tworzenie użytkowników: Wpisz nazwę w polu i kliknij Create User.
    * Dodawanie wydarzenia: Podaj nazwę użytkownika, dzień tygodnia i szczegóły wydarzenia, a następnie kliknij Add Event.
    * Wyświetlanie wydarzenia: Wpisz nazwę użytkownika w odpowiednie pole i kliknij List Events.
4. **Zamknięcie klienta**: Klient automatycznie rozłącza się z serwerem podczas zamykania.