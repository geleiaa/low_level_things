import sys

KEY = "secret_key"

def xor(data, key):

	key = str(key)
	l = len(key)
	output_str = ""

	for i in range(len(data)):
		current = data[i]
		current_key = key[i % len(key)]
		output_str += chr(ord(current) ^ ord(current_key))

	return output_str

def printCipherText(ciphertext):
	print('{ 0x' + ', 0x'.join(hex(ord(x)) [2:] for x in ciphertext) + ' };')


try:
	plaintext = open(sys.argv[i], "rb").read()	
except:
	print("File arg needed! %s [raw payload file]" % sys.argv[0])	
	sys.exit()

ciphertext = xor(plaintext, KEY)
print('{ 0x' + ', 0x'.join(hex(ord(x)) [2:] for x in ciphertext) + ' };')