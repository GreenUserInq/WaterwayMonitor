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
            int WaterLevel1,
            int WaterLevel2,
            int Degree_of_clogging1,
            int Degree_of_clogging2,
            int Header1,
            int Header2,
            string alarm,
            string errors,
            int Structural_deformations)
        {
            var builder = new StringBuilder();
            builder.Append($"{{\"id\":{Id}");
            builder.Append(appenedMessage(MonitoringParameters.WaterLevel1, WaterLevel1));
            builder.Append(appenedMessage(MonitoringParameters.WaterLevel2 , WaterLevel2));
            builder.Append(appenedMessage(MonitoringParameters.DegreeOfClogging1, Degree_of_clogging1));
            builder.Append(appenedMessage(MonitoringParameters.DegreeOfClogging2 , Degree_of_clogging2));
            builder.Append(appenedMessage(MonitoringParameters.CloggingHeader1 , Header1));
            builder.Append(appenedMessage(MonitoringParameters.CloggingHeader2 , Header2));
            builder.Append(appenedMessage(MonitoringParameters.StructuralDeformations, Structural_deformations));
            builder.Append(appenedMessage(MonitoringParameters.AlarmState, alarm != "No alarms"));
            builder.Append(appenedMessage(MonitoringParameters.Errors, errors));
            builder.Append('}');

            return builder.ToString();
        }

        private string appenedMessage(MonitoringParameters parameter, object value)
        {
            return $",\"{parameter.ToString()}\":{value.ToString()}";
        }
    }
}
