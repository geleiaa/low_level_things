import sys

def xor_with_key(string, key):
    # Convert string and key to bytes
    string_bytes = string.encode()
    key_bytes = key.encode()

    # Perform XOR operation byte by byte
    result_bytes = bytearray()
    for i in range(len(string_bytes)):
        result_bytes.append(string_bytes[i] ^ key_bytes[i % len(key_bytes)])

    # Convert to C-style hexadecimal format
    result_c_format = ", ".join([f"0x{byte:02X}" for byte in result_bytes])
    return result_c_format

def main():
    if len(sys.argv) < 2:
        print("Please provide a key as an argument.")
        return

    key = sys.argv[1]

    try:
        with open("sc.bin", "r") as file:
            for line in file:
                string = line.strip()
                xor_value = xor_with_key(string, key)
                print("unsigned char os_{0}[] = {{ {1} }};".format(string, xor_value))
    except FileNotFoundError:
        print("File not found.")

if __name__ == "__main__":
    main()
