using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Net;
using System.Net.Sockets;

namespace HistoryServer
{
    public enum MessageType
    { Registration, Exit, Peer2Peer, Broadcast, GetData, Empty, Confirm, Error, HistoryServerInit};

    public enum MessageClient
    { Broker, User, All };

    [StructLayout(LayoutKind.Sequential)]
    struct MessageHeader
    {
        [MarshalAs(UnmanagedType.I4)]
        public MessageClient recipient;
        [MarshalAs(UnmanagedType.I4)]
        public int recipient_id_size;

        [MarshalAs(UnmanagedType.I4)]
        public MessageClient sender;
        [MarshalAs(UnmanagedType.I4)]
        public int sender_id_size;

        [MarshalAs(UnmanagedType.I4)]
        public MessageType type;
        [MarshalAs(UnmanagedType.I4)]
        public int size;
    };

    internal class Message
    {
        private MessageHeader _header;
        private string _recipient_id;
        private string _sender_id;
        private string _data;
        static Encoding? cp866 = null;

        public Message()
        {
            _header = new MessageHeader();
            _recipient_id = string.Empty;
            _sender_id = string.Empty;
            _data = string.Empty;
        }

        public Message(ref MessageHeader header, string data = "")
        {
            _header = header;
            _recipient_id = string.Empty;
            _sender_id = string.Empty;
            _data = data;
        }

        Encoding get866()
        {
            if (cp866 is null)
            {
                Encoding.RegisterProvider(CodePagesEncodingProvider.Instance);
                cp866 = Encoding.GetEncoding("CP866");
            }
            return cp866;
        }

        static byte[] toBytes(object obj)     // Функция конвертации объекта в набор байтов
        {
            int size = Marshal.SizeOf(obj);
            byte[] buff = new byte[size];
            IntPtr ptr = Marshal.AllocHGlobal(size);
            Marshal.StructureToPtr(obj, ptr, true);
            Marshal.Copy(ptr, buff, 0, size);
            Marshal.FreeHGlobal(ptr);
            return buff;
        }

        // Функция конструирования объекта типа Т из набора байт
        static T fromBytes<T>(byte[] buff) where T : struct
        {
            T data = default(T);
            int size = Marshal.SizeOf(data);
            IntPtr ptr = Marshal.AllocHGlobal(size);
            Marshal.Copy(buff, 0, ptr, size);
            var d = Marshal.PtrToStructure(ptr, data.GetType());
            if (d is not null)
            {
                data = (T)d;
            }
            Marshal.FreeHGlobal(ptr);
            return data;
        }

        public static bool Send(Socket destination, Message message)    // Функция отправки сообщения через сокет
        {
            try
            {
                // отправляем заголовок
                destination.Send(toBytes(message._header), Marshal.SizeOf(message._header), SocketFlags.None);

                if (message._header.type == MessageType.Empty || message._header.type == MessageType.HistoryServerInit)
                    return true;

                if (message._header.recipient == MessageClient.User && message._header.recipient_id_size > 0)
                {
                    // отправляем имя отправителя
                    destination.Send(message.get866().GetBytes(message._recipient_id), message._header.recipient_id_size, SocketFlags.None);
                }

                if (message._header.sender == MessageClient.User && message._header.sender_id_size > 0)
                {
                    // отправляем имя получателя
                    destination.Send(message.get866().GetBytes(message._sender_id), message._header.sender_id_size, SocketFlags.None);
                }

                if (message._header.size != 0)
                {
                    destination.Send(message.get866().GetBytes(message._data), message._data.Length, SocketFlags.None);
                }
            }
            catch (SocketException ex)
            {
                Console.WriteLine("{0} Error code: {1}.", ex.Message, ex.ErrorCode);
                return false;
            }
            return true;
        }

        public static Message Read(Socket source)    // Функция чтения сообщения из сокета
        {
            Message received_message = new Message();
            MessageHeader header = new MessageHeader();
            byte[] buff_for_header = new byte[Marshal.SizeOf(header)];

            try
            {
                source.Receive(buff_for_header, Marshal.SizeOf(header), SocketFlags.None);
                header = fromBytes<MessageHeader>(buff_for_header);

                if (header.type == MessageType.Confirm || header.type == MessageType.Error || header.type == MessageType.Empty)
                {
                    received_message._header = header;
                    return received_message;
                }

                if (header.recipient == MessageClient.User)
                {
                    byte[] recipient_buff = new byte[header.recipient_id_size];
                    source.Receive(recipient_buff, header.recipient_id_size, SocketFlags.None);
                    received_message._recipient_id = received_message.get866().GetString(recipient_buff, 0, header.recipient_id_size);
                }

                if (header.sender == MessageClient.User)
                {
                    byte[] sender_buff = new byte[header.sender_id_size];
                    source.Receive(sender_buff, header.sender_id_size, SocketFlags.None);
                    received_message._sender_id = received_message.get866().GetString(sender_buff, 0, header.sender_id_size);
                }

                received_message._header = header;
                if (header.size > 0)
                {
                    byte[] data_buff = new byte[header.size];
                    source.Receive(data_buff, header.size, SocketFlags.None);
                    received_message._data = received_message.get866().GetString(data_buff, 0, header.size);
                }
                return received_message;
            }
            catch (SocketException ex)
            {
                Console.WriteLine("{0} Error code: {1}.", ex.Message, ex.ErrorCode);
                return received_message;
            }
        }

        public static Socket ConnectToMainServer()
        {
            MessageHeader header = new MessageHeader();

            header.recipient = MessageClient.Broker;
            header.recipient_id_size = 0;
            header.sender = MessageClient.Broker;
            header.sender_id_size = 0;

            header.type = MessageType.HistoryServerInit;
            header.size = 0;
            Message request = new Message(ref header);

            int nPort = 12345;
            IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), nPort);
            Socket server_sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            server_sock.Connect(endPoint);
            if (!server_sock.Connected)
            {
                throw new Exception("Connection error");
            }

            Message.Send(server_sock, request); // Отправляем запрос регистрации
            Message.WaitConfirm(server_sock);
            return server_sock;
        }

        public static bool WaitConfirm(Socket confirm_source)
        {
            Message confirm_message = Read(confirm_source);

            if (confirm_message.IsConfirm())
                return true;
            else
                return false;
        }

        public void SetSender(string username)
        {
            _sender_id = username;
            _header.sender = MessageClient.User;
            _header.sender_id_size = username.Length;
        }

        public void SetRecipient(string username)
        {
            _recipient_id = username;
            _header.recipient = MessageClient.User;
            _header.recipient_id_size = username.Length;
        }

        public void SetData(string data)
        {
            _data = data;
            _header.size = data.Length;
        }

        public void SetType(MessageType type)
        {
            _header.type = type;
        }

        public MessageHeader GetHeader()
        {
            return _header;
        }
        public string GetData()
        {
            return _data;
        }

        public string GetSender()
        {
            return _sender_id;
        }

        public string GetRecipient()
        {
            return _recipient_id;
        }

        public bool IsConfirm()
        {
            return _header.type == MessageType.Confirm;
        }
        public bool IsError()
        {
            return _header.type == MessageType.Error;
        }
    }
}
