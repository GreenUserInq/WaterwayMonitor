using Microsoft.Extensions.Configuration;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataPublisher.Common.Models
{
    public class MqttConfig
    {
        public MqttConfig(
            string brokerAddress,
            int port,
            string userName,
            string pass,
            string id)
        {
            BrokerAddress = brokerAddress;
            Port = port;
            Username = userName;
            Password = pass;
            ClientId = id;
        }

        public MqttConfig()
        {
            var configuration = new ConfigurationBuilder()
            .SetBasePath(Directory.GetCurrentDirectory())
            .AddJsonFile("appsettings.json")
            .Build();

            var settings = configuration.GetSection("MqttSettings");
            BrokerAddress = settings["BrokerAddress"];
            Port = int.Parse(settings["Port"]);
            Username = settings["Username"];
            Password = settings["Password"];
            ClientId = settings["ClientId"];
        }

        public string BrokerAddress { get; }
        public int Port { get; }
        public string Username { get; }
        public string Password { get; }
        public string ClientId { get; }
    }
}
