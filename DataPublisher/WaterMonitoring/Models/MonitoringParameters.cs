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
            int waterLevel,
            int waterLevel2,
            int degreeOfClogging1,
            int degreeOfClogging2,
            int cloggingHeader1,
            int cloggingHeader2,
            int structuralDeformation,
            string alarmState,
            string errors)
        {
            ID = id;
            WaterLevel1 = waterLevel;
            WaterLevel2 = waterLevel2;
            DegreeOfClogging1 = degreeOfClogging1;
            DegreeOfClogging2 = degreeOfClogging2;
            CloggingHeader1 = cloggingHeader1;
            CloggingHeader2 = cloggingHeader2;
            StructuralDeformations = structuralDeformation;
            Errors = errors;
            AlarmState = alarmState;
        }
    }
}
