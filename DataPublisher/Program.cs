using DataPublisher.Common.Services;
using Microsoft.Extensions.Configuration;
using DataPublisher.Common.Models;
using DataPublisher.WaterMonitoring.Factories;

class Program
{
    static async Task Main(string[] args)
    {
        var configuration = new ConfigurationBuilder()
            .SetBasePath(Directory.GetCurrentDirectory())
            .AddJsonFile("appsettings.json", optional: false, reloadOnChange: true)
            .Build();

        var mqttConfig = new MqttConfig();
        configuration.GetSection("MqttSettings").Bind(mqttConfig);

        ////  конфиг для подключения к WiFi если понадобится
        //var internetSettings = new InternetConfig();
        //configuration.GetSection("InternetSettings").Bind(internetSettings);

        var mqttClient = new MqttService(
            mqttConfig.BrokerAddress,
            mqttConfig.Port,
            mqttConfig.Username,
            mqttConfig.Password,
            mqttConfig.ClientId
            );

        await mqttClient.ConnectAsync();
        await mqttClient.SubscribeAndReceiveAsync("devices/update");

        var timerSettings = new TimerConfig();
        configuration.GetSection("TimerSettings").Bind(timerSettings);

        while (timerSettings.IsEnabled)
        {
            var data = JsonService.LoadFromJsonFile<DataPublisher.WaterMonitoring.Models.MonitoringParameters>("../MonitoringData.json");

            var factory = new FactoryMessage();
            var mqttStr = factory.GetPostMessage(
                data.ID,
                data.WaterLevel,
                data.DegreeOfClogging,
                data.StructuralDeformations,
                data.AmbientTemperature,
                data.WaterFlowRate,
                data.HumidityInsideThePipe);

            await mqttClient.PublishDataAsync(mqttStr);

            await Task.Delay(timerSettings.IntervalMilliseconds);
            configuration.GetSection("TimerSettings").Bind(timerSettings);
        }

        await mqttClient.Disconect();
    }
}