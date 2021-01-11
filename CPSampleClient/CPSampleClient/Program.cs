using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;

namespace CPSampleClient
{
    class Program
    {
        static void Main(string[] args)
        {
            IPEndPoint ip = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 1983);

            Socket server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            Console.WriteLine("♠ ♥ ♣ ♦ ♡ ♢ ♤ ♧");

            try
            {
                server.Connect(ip);
            }
            catch (SocketException e)
            {
                Console.WriteLine("Unable to connect to server.");
                return;
            }

            string input = "14*1;20.0;10;20;Kyocera Poker Club;1;1;0.0;0.0;0;0.0;0.0;|";    //Create room
            byte[] data = Encoding.ASCII.GetBytes(input);
            server.Send(data);
/*
            //fixed limit texas
            input = "14*1;20.0;10;60;pacific poker;1;3;0.0;0.0;0;0.0;0.0;|";    //Create room fixed limit
            data = Encoding.ASCII.GetBytes(input);
            server.Send(data);

            input = "14*2;20.0;7;3600;full tilt poker(normal);1;3;1.0;5.0;0;0.0;0.0;|";    //Create room fixed limit - 7 card stud
            data = Encoding.ASCII.GetBytes(input);
            server.Send(data);

            input = "14*4;20.0;7;3600;Stud/8;1;3;1.0;5.0;0;0.0;0.0;|";    //Create room fixed limit - 7 card stud hilo
            data = Encoding.ASCII.GetBytes(input);
            server.Send(data);

            input = "14*5;20.0;7;3600;Lowball-Stud;1;3;1.0;5.0;0;0.0;0.0;|";    //Create room fixed limit - Razz
            data = Encoding.ASCII.GetBytes(input);
            server.Send(data);

            input = "14*3;2.0;10;3600;titan poker(normal);1;2;0.0;0.0;0;0.0;0.0;|";    //Create room pot limit - omaha
            data = Encoding.ASCII.GetBytes(input);
            server.Send(data);

            input = "14*6;2.0;10;3600;Omaha/8;1;2;0.0;0.0;0;0.0;0.0;|";    //Create room pot limit - omaha hilo
            data = Encoding.ASCII.GetBytes(input);
            server.Send(data);

            //create texas holdem no limit SNG
            input = "14*1;10.0;2;3600;SNG texas;3;1;0.0;0.0;60;1.0;100.0;|";    
            data = Encoding.ASCII.GetBytes(input);
            server.Send(data);
            */
            server.Shutdown(SocketShutdown.Both);
            server.Close();
        }
    }
}

/*
♠ U+2660 Black Spade Suit
♡ U+2661 White Heart Suit
♢ U+2662 White Diamond Suit
♣ U+2663 Black Club Suit
♤ U+2664 White Spade Suit
♥ U+2665 Black Heart Suit
♦ U+2666 Black Diamond Suit
♧ U+2667 White Club Suit
 */