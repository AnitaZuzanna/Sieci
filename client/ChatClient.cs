using System;  
using System.Net;  
using System.Net.Sockets;  
using System.Text;  
using System.Threading;
using System.IO;

namespace Client
{
    class ChatClient
    {
        public static void StartClient(string address)
        {

            
           
            int port = 9000;
             
            IPAddress ipAddress = IPAddress.Parse(address);
            IPEndPoint remoteEP = new IPEndPoint(ipAddress,port); 

            Socket clientSocket = new Socket( ipAddress.AddressFamily, SocketType.Stream, ProtocolType.Tcp);

            Thread thread = new Thread(o => ReceiveData((Socket)o));

            clientSocket.Connect(remoteEP);

            thread.Start(clientSocket);

            string input;

            while((input = Console.ReadLine()) != "Exit")
            {
                if(input[0] == 'M')
                {
                    sendMessage(clientSocket,input);
                }          

                if(input[0] == 'U')
                {
                    sendFile(clientSocket, input);
                } 
            }

            clientSocket.Shutdown(SocketShutdown.Both);
            clientSocket.Close();

        }

        static void sendFile(Socket clSocket, string msg)
        {   
            string fileName = msg.Substring(2);
            FileInfo fi = new FileInfo(fileName);

            if(fi.Exists == false)
            {
                Console.WriteLine("File does not exist");
                return;
            }
            Console.WriteLine($"file: {fi.Length}");
            string messageString = "U " + fileName + " " + fi.Length;
            sendMessage(clSocket, messageString);
            //odebrac odp serwera
        }
         static void sendMessage(Socket clSocket, string msg)
         {
            byte[] sdMessage = new byte[1024];
            sdMessage = Encoding.ASCII.GetBytes(msg);
            int sendMessage = clSocket.Send(sdMessage);
         }


         static void ReceiveData(Socket clientSocket)
         {
             byte[] rcMessage = new byte[1024];
              
            while(true)
            {
                int msgLen = clientSocket.Receive(rcMessage);
                if (msgLen == 0){
                    break;
                }
                Console.WriteLine(Encoding.ASCII.GetString(rcMessage,0,msgLen));
            }
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
