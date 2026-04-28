namespace Helper {
	class Program {
		static void Main(string[] args) {
			
			byte[] buf = ...
			
			byte[] encoded = new byte[buf.Length];

			for(int i = 0; i < buf.Length; i++) {
				encoded[i] = (byte)(((uint)buf[i] + 2) & 0xFF);
			}

			StringBuilder hex = new StringBuilder(encoded.Length * 2);
			
			foreach(byte b in encoded) {
				hex.AppendFormat("0x{0:x2}, ", b);
			}

			Console.WriteLine("The payload is: " + hex.ToString());
		}
	}
}

/*
byte[] buf = new byte[752] {0xfe, 0x4a, 0x85, 0xe6, 0xf2...
for(int i = 0; i < buf.Length; i++) {
	buf[i] = (byte)(((uint)buf[i] - 2) & 0xFF);
}
*/