using DataPublisher.WaterMonitoring.Enums;
using DataPublisher.WaterMonitoring.Interfsces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataPublisher.WaterMonitoring.Factories
{
    class FactoryMessage : IFactoryMessage
    {
        public string GetPostMessage(
            int Id,
            int Water_level,
            int Degree_of_clogging,
            int Structural_deformations,
            int Ambient_temperature,
            int Water_flow_rate,
            int Humidity_inside_the_pipe)
        {
            var builder = new StringBuilder();
            builder.Append($"{{\"id\":{Id}");
            builder.Append(appenedMessage(MonitoringParameters.Water_level, Water_level));
            builder.Append(appenedMessage(MonitoringParameters.Degree_of_clogging, Degree_of_clogging));
            builder.Append(appenedMessage(MonitoringParameters.Structural_deformations, Structural_deformations));
            builder.Append(appenedMessage(MonitoringParameters.Ambient_temperature, Ambient_temperature));
            builder.Append(appenedMessage(MonitoringParameters.Water_flow_rate, Water_flow_rate));
            builder.Append(appenedMessage(MonitoringParameters.Humidity_inside_the_pipe, Humidity_inside_the_pipe));
            builder.Append('}');

            return builder.ToString();
        }

        private string appenedMessage(MonitoringParameters parameter, int value)
        {
            return $",\"name\":\"{parameter.ToString()}\",\"value\":{value}";
        }
    }
}
