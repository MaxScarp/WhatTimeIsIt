import socket
import struct
import random

identification = random.randint(0, 65535)

print('identification' , hex(identification ), identification )

query = 0
opcode = 0
aa = 0 # not relevant for client
tc = 0 # not relevant for client
rd = 1 # recursion desired
ra = 0 # not relevant for client
rcode = 0 # not relevant for client

packet_header = struct.pack('>HBBHHHH' ,
                            identification ,
                            query << 7 | opcode << 3 | aa << 2 | tc << 1 | rd,
                            rcode,
                            1, # 1 question
                            0, 0, 0 # answers
                            )

def dns_string (string):
    parts = string.split('.')
    result = b''
    for part in parts:
        bpart = part.encode( 'ascii')
        result += struct.pack('B', len(bpart)) + bpart
    return result + b'\x00'

packet_question = dns_string ('aiv01.it' ) + struct.pack('>HH',
                                                        1, # A type
                                                        1 # class IN/Internet
                                                        )

print(packet_header + packet_question )

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM )
s.settimeout (3)

s.sendto(packet_header + packet_question , ('8.8.8.8' , 53)) # send to google

response , sender = s.recvfrom (4096)
print('response from' , sender)

print(struct.unpack('>HBBHHHH' , response [0:12]))

print(response [12:])

# last 4 bytes for ip address (naive/dumb approach, do not follow it)
print(tuple(response [-4:]))