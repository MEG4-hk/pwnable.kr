#!/usr/bin/python3


from pwn import *


# Connect to the game
con = remote('localhost', 9007)

# Catch the game's instructions.
con.recv(2048)
sleep(3)

# Repeat 100 times
for run_num in range(100):
    # Extract N and C
    values = con.recvline()[:-1].decode()
    N = int(values.split(' ')[0].split('=')[1])
    C = int(values.split('C=')[1])

    min_index = 0
    max_index = N - 1

    # Try C times
    for _ in range(0, C):
        half = (max_index - min_index)//2 + min_index

        # Send indexes to weigh.
        con.sendline(' '.join([str(x) for x in range(min_index, half)]))
        weight = int(con.recvline()[:-1].decode())

        if weight % 10 is 0:
            min_index = half
        else:
            max_index = half

    # Send final index - the index of the counterfeit coin.
    con.sendline(str(min_index))
    print(con.recvline()[:-1].decode())

    print('[+] Finished run {} -> counterfeit={}'.format(run_num, min_index))

print(con.recvline())
print('[+] flag = {}'.format(con.recvline().decode()))