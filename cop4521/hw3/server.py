"""
Cristian Victoria
cjv22c
Due date: March 30 2026
The program in this file is the individual work of Cristian Victoria
"""

import argparse
import socket
import threading

def put(store: dict, message_parts: list[str]) -> str:
    if len(message_parts) < 3:
        return "ERROR: PUT requires a key and a value"
    
    key: str = message_parts[1]
    value: str = message_parts[2]
    store[key] = value
    
    return "OK"

def get(store: dict, message_parts: list[str]) -> str:
    if len(message_parts) < 2:
        return "ERROR: GET requires a key"
    
    key: str = message_parts[1]

    return store.get(key, "ERROR: Key not found")

def handle_command(store: dict, message: str) -> str:
    message_parts: list[str] = message.strip().split(" ", 2)
    # message_parts[0] = command
    # message_parts[1] = key
    # message_parts[2] = value

    command: str = message_parts[0].upper()

    if command == "PUT":
        return put(store, message_parts)
    elif command == "GET":
        return get(store, message_parts)
    else:
        return "ERROR: Invalid Command"

def handle_thread(conn, addr, store, lock) -> None:
    print(f"[LOG] Accepted connection from addr: {addr}")
    data = conn.recv(1024).decode("utf-8")
    if data:
        with lock:
           response = handle_command(store, data) # Protects shared dictionary access

        conn.sendall(response.encode("utf-8"))

    conn.close()
    print(f"Thread client {addr} done")

"""
MAIN
"""
def main():
    parser = argparse.ArgumentParser(description="Socket Programming script")
    parser.add_argument("port", type=int, help="The port number to connect to")
    args = parser.parse_args()
    port = args.port

    concurrent_store: dict = {}
    lock = threading.Lock()       # Protects store from race conditions
    thread_list = []

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1) 
    server_socket.bind(("127.0.0.1", port))
    server_socket.listen()
    print(f"[START] Server listening on port {port}")

    while True:
        conn, addr = server_socket.accept()
        thread = threading.Thread(target=handle_thread, args=(conn, addr, concurrent_store, lock))
        thread.daemon = True      # Thread dies automatically when main program exits
        thread.start()
        thread_list.append(thread) # append to list of threads
        print(f"[Main] Spawned thread for {addr}, back to accept()")
        

if __name__ == "__main__":
    main()