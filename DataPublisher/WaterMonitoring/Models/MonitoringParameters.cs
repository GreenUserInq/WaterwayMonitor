using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataPublisher.WaterMonitoring.Models
{
    public class MonitoringParameters
    {
        public int ID { get; set; }
        public int WaterLevel1 { get; set; }
        public int WaterLevel2 { get; set; }
        public int DegreeOfClogging1 { get; set; }
        public int DegreeOfClogging2 { get; set; }
        public int CloggingHeader1 { get; set; }
        public int CloggingHeader2 { get; set; }
        public int StructuralDeformations { get; set; }
        public string AlarmState { get; set; }
        public string Errors { get; set; }

        public MonitoringParameters(
            int id,
            int waterLevel1,
            int waterLevel2,
            int degreeOfClogging1,
            int degreeOfClogging2,
            int header1,
            int header2,
            int structuralDeformations,
            string alarmState,
            string errors)
        {
            ID = id;
            WaterLevel1 = waterLevel1;
            WaterLevel2 = waterLevel2;
            DegreeOfClogging1 = degreeOfClogging1;
            DegreeOfClogging2 = degreeOfClogging2;
            CloggingHeader1 = header1;
            CloggingHeader2 = header2;
            StructuralDeformations = structuralDeformations;
            AlarmState = alarmState;
            Errors = errors;
        }
    }
}
