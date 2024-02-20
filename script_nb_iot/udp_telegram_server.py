import socket
import requests

TOKEN = "6435809290:AAEMM1PzOaueYUpCjWHy65JuWr1up50XauY"
url = f"https://api.telegram.org/bot{TOKEN}/getUpdates"
print(requests.get(url).json())


chat_id = "1979230463"
message = "UDP telegram bot started"
url = f"https://api.telegram.org/bot{TOKEN}/sendMessage?chat_id={chat_id}&text={message}"
#print(requests.get(url).json()) # this sends the message

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

server_address = '0.0.0.0'
server_port = 20001

server = (server_address, server_port)
sock.bind(server)
print("Listening on " + server_address + ":" + str(server_port))

while True:
    payload, client_address = sock.recvfrom(1024)
    chat_id = "1979230463"
    message = str(payload,'utf-8')
    url = f"https://api.telegram.org/bot{TOKEN}/sendMessage?chat_id={chat_id}&text={message}"
    print(requests.get(url).json()) # this sends the message
    print("Echoing data back to " + str(client_address))
    sent = sock.sendto(payload, client_address)
