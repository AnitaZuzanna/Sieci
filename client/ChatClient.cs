using System;

namespace Client
{
    class ChatClient
    {
        static void Main(string[] args)
        {
            foreach(var param in args)
            {
                Console.WriteLine(param);
            }
            Console.WriteLine("Hello World!");
        }
    }
}
