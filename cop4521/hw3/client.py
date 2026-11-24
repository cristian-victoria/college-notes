import socket

def send_command(command: str, port: int) -> str:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect(("127.0.0.1", port))
        s.sendall(command.encode("utf-8"))
        response = s.recv(1024).decode("utf-8")

        return response

def main():
    port = int(input("Please specify port number: "))
    quit = False
    while quit != True:
        command = input("Please insert command (-1 to exit): ")
        if command == "-1":
            quit = True
        else:
            response = send_command(command, port)
            print (response)

    return "Program terminated."

if __name__ == "__main__":
    main()