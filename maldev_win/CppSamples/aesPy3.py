import sys
import os
from Crypto.Cipher import AES
from Crypto.Hash import SHA256

# Ex usage: python3 aes256_encrypt.py payload_unenc.ico -of payload_enc.ico

def generate_key():
    return os.urandom(16)

def pad(s):
    return s + (AES.block_size - len(s) % AES.block_size) * bytes([AES.block_size - len(s) % AES.block_size])

def aesenc(plaintext, key):
    k = SHA256.new(key).digest()
    print("k =", end=" ")
    print(', '.join(f'0x{byte:02x}' for byte in k))
    iv = 16 * b'\x00'
    plaintext = pad(plaintext)
    cipher = AES.new(k, AES.MODE_CBC, iv)
    return cipher.encrypt(plaintext)

if len(sys.argv) < 2:
    print(f"File argument needed! Usage: {sys.argv[0]} <raw payload file> [-of output_file]")
    sys.exit()

try:
    with open(sys.argv[1], "rb") as file:
        plaintext = file.read()
except FileNotFoundError:
    print(f"File not found: {sys.argv[1]}")
    sys.exit()

key = generate_key()
ciphertext = aesenc(plaintext, key)

key_c_byte_string = ', '.join(f'0x{byte:02x}' for byte in key)
payload_c_byte_string = ', '.join(f'0x{byte:02x}' for byte in ciphertext)

print(f'unsigned char AESkey[] = {{ {key_c_byte_string} }};')
print(f'unsigned char payload[] = {{ {payload_c_byte_string} }};')

if len(sys.argv) == 4 and sys.argv[2] == "-of":
    output_filename = sys.argv[3]
    with open(output_filename, "wb") as output_file:
        output_file.write(ciphertext)
        print("")
        print("[+] Output file argument used")
        print("[+] Wrote contents in binary format to: {0}".format(output_filename))
        print("")

