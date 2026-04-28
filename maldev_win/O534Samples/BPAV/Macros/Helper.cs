namespace Helper {
        class Program {
                static void Main(string[] args) {

                        byte[] buf = ...

                        byte[] encoded = new byte[buf.Length];

                        for(int i = 0; i < buf.Length; i++) {
                                encoded[i] = (byte)(((uint)buf[i] + 2) & 0xFF);
                        }

                        uint conter = 0;

                        StringBuilder hex = new StringBuilder(encoded.Length * 2);

                        foreach(byte b in encoded) {
                                hex.AppendFormat("{0:D}, ", b);
                                counter++;
                                if(counter % 50 == 0){
                                        hex.AppendFormat("_{0}", Environment.NewLine);
                                }
                        }

                        Console.WriteLine("The payload is: " + hex.ToString());
                }
        }
}