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
            int water_level1,
            int water_Level2,
            int degreeOfClogging1,
            int degreeOfClogging2,
            int cloggingHeader1,
            int cloggingHeader2,
            int structuralDeformation,
            string alarmState,
            string errors)
        {
            var builder = new StringBuilder();
            builder.Append($"{{\"id\":{Id}");
            builder.Append(appenedMessageInt(MonitoringParameters.WaterLevel1, water_level1));
            builder.Append(appenedMessageInt(MonitoringParameters.WaterLevel2, water_Level2));
            builder.Append(appenedMessageInt(MonitoringParameters.DegreeOfClogging1, degreeOfClogging1));
            builder.Append(appenedMessageInt(MonitoringParameters.DegreeOfClogging2, degreeOfClogging2));
            builder.Append(appenedMessageInt(MonitoringParameters.CloggingHeader1, cloggingHeader2));
            builder.Append(appenedMessageInt(MonitoringParameters.CloggingHeader2, cloggingHeader2));
            builder.Append(appenedMessageInt(MonitoringParameters.StructuralDeformations, structuralDeformation));
            builder.Append(appenedMessageBool(MonitoringParameters.AlarmState, alarmState != "No alarms"));
            builder.Append(appenedMessageStr(MonitoringParameters.Errors, errors));
            Console.WriteLine(errors);  
            builder.Append('}');

            return builder.ToString();
        }

        private string appenedMessageInt(MonitoringParameters parameter, int value)
        {
            return $",\"name\":\"{parameter.ToString()}\",\"value\":{value}";
        }

        private string appenedMessageStr(MonitoringParameters parameter, string value)
        {
            return $",\"name\":\"{parameter.ToString()}\",\"value\":\"{value}\"";
        }
        private string appenedMessageBool(MonitoringParameters parameter, bool value)
        {
            return $",\"name\":\"{parameter.ToString()}\",\"value\":{value}";
        }
    }
}
