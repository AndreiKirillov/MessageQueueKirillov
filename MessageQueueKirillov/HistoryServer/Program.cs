using System;
using System.Net;
using System.Net.Sockets;
using Microsoft.Data.Sqlite;
using System.Threading;
using System.Collections.Generic;

namespace HistoryServer
{
    class Program
    {
        private static Socket _history_server;
        private static SqliteConnection _db_connection;

        static void ProcessClient(ref Socket client_sock)
        {
            Message message = Message.Read(client_sock);
            MessageHeader header = message.GetHeader();
            if(header.type == MessageType.GetData)
            {
                string client_name = message.GetSender();
                string query = $"SELECT sender,recipient,data FROM messages WHERE recipient='{client_name}' OR " +
                    $"(recipient='ALL' AND sender <>'{client_name}') OR (sender = '{client_name}') ORDER BY id";
                var command = _db_connection.CreateCommand();
                command.CommandText = query;
                var reader = command.ExecuteReader();
                string message_list = "";
                while (reader.Read())
                {
                    string sender = reader[0].ToString();
                    string recipient = reader[1].ToString();
                    string data = reader[2].ToString();
                    string row = "";
                    if (recipient == client_name || recipient == "ALL")
                    {
                        row = $"Message from {sender}: {data}\n";
                    }
                    if (sender == client_name)
                    {
                        row = $"Message to {recipient}: {data}\n";
                    }
                    message_list += row;
                }
                reader.Close();

                MessageHeader answer_header = new MessageHeader();
                answer_header.type = MessageType.History;
                answer_header.size = message_list.Length;
                answer_header.sender = MessageClient.Broker;
                answer_header.recipient = MessageClient.User;
                answer_header.recipient_id_size = message.GetSender().Length;
                Message answer = new Message(ref answer_header);
                answer.SetData(message_list);
                answer.SetRecipient(message.GetSender());

                Message.Send(client_sock, answer);
            }
        }

        static void WaitConnection()
        {
            int nPort = 12346;
            IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), nPort);
            Socket incoming_sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            incoming_sock.Bind(endPoint);
            while (true)
            {
                incoming_sock.Listen();
                Socket client = incoming_sock.Accept();
                Thread client_thread = new Thread(() => ProcessClient(ref client));
                client_thread.IsBackground = true;
                client_thread.Start();
            }
        }

        static void Main(string[] args)
        {
            _history_server = Message.ConnectToMainServer();

            string db_path = "Data Source=C:/repository/MessageQueueKirillov/MessageQueueKirillov/history.db;";
            _db_connection = new SqliteConnection(db_path);
            _db_connection.Open();

            Thread incloming_clients_thread = new Thread(() => WaitConnection());
            incloming_clients_thread.Start();

            while (true)
            {
                Message message = Message.Read(_history_server);

                MessageHeader header = message.GetHeader();
                switch(header.type)
                {
                    case MessageType.Peer2Peer:
                    {
                            string sender = message.GetSender();
                            string recipient = message.GetRecipient();
                            string data = message.GetData();

                            string query = $"INSERT INTO messages (sender, recipient, data) VALUES ('{sender}', '{recipient}', '{data}')";
                            var command = _db_connection.CreateCommand();
                            command.CommandText = query;
                            command.ExecuteNonQuery();

                            break;
                    }
                    case MessageType.Broadcast:
                    {
                            string sender = message.GetSender();
                            string recipient = "ALL";
                            string data = message.GetData();

                            string query = $"INSERT INTO messages (sender, recipient, data) VALUES ('{sender}', '{recipient}', '{data}')";
                            var command = _db_connection.CreateCommand();
                            command.CommandText = query;
                            command.ExecuteNonQuery();
                            break;
                    }
                }
            }
        }
    }
}
