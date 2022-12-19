using System;
using System.Net.Sockets;
using Microsoft.Data.Sqlite;

namespace HistoryServer
{
    class Program
    {
        static void Main(string[] args)
        {
            Socket connection_to_main = Message.ConnectToMainServer();

            string db_path = "Data Source=C:/repository/MessageQueueKirillov/MessageQueueKirillov/history.db;";
            var db_connection = new SqliteConnection(db_path);
            db_connection.Open();

            while(true)
            {
                Message message = Message.Read(connection_to_main);

                MessageHeader header = message.GetHeader();
                switch(header.type)
                {
                    case MessageType.Peer2Peer:
                    {
                            string sender = message.GetSender();
                            string recipient = message.GetRecipient();
                            string data = message.GetData();

                            string query = $"INSERT INTO messages (sender, recipient, data) VALUES ('{sender}', '{recipient}', '{data}')";
                            var command = db_connection.CreateCommand();
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
                            var command = db_connection.CreateCommand();
                            command.CommandText = query;
                            command.ExecuteNonQuery();
                            break;
                    }
                }
            }
        }
    }
}
