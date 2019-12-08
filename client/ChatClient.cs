using System;  
using System.Net;  
using System.Net.Sockets;  
using System.Text;  
  

namespace Client
{
    class ChatClient
    {
        public static void StartClient(string address)
        {

            byte[] rcMessage = new byte[1024];
            byte[] sdMessage = new byte[1024];
            int port = 9000;
             
            IPAddress ipAddress = IPAddress.Parse(address);
            IPEndPoint remoteEP = new IPEndPoint(ipAddress,port); 

            Socket clientSocket = new Socket( ipAddress.AddressFamily, SocketType.Stream, ProtocolType.Tcp);

            clientSocket.Connect(remoteEP);

            while(true)
            {
                string input = Console.ReadLine();
                sdMessage = Encoding.ASCII.GetBytes(input);
                int sendMessage = clientSocket.Send(sdMessage);
            }

            clientSocket.Shutdown(SocketShutdown.Both);
                clientSocket.Close();

        }
        
        static int Main(string[] args)
        {
            if(args.Length > 0)
            {
                StartClient(args[0]);
            }
            else
            {
                Console.WriteLine("Usage: ./Client [ip address]");
                return 1;
            }

            return 0;
        }
    }
}
