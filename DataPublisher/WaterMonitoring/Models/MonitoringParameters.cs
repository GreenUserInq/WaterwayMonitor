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
        public int WaterLevel { get; set; }
        public int DegreeOfClogging { get; set; }
        public int StructuralDeformations { get; set; }
        public int AmbientTemperature { get; set; }
        public int WaterFlowRate { get; set; }
        public int HumidityInsideThePipe { get; set; }

        public MonitoringParameters(
            int id,
            int waterLevel,
            int degreeOfClogging,
            int structuralDeformations,
            int ambientTemperature,
            int waterFlowRate,
            int humidityInsideThePipe)
        {
            ID = id;
            WaterLevel = waterLevel;
            DegreeOfClogging = degreeOfClogging;
            StructuralDeformations = structuralDeformations;
            AmbientTemperature = ambientTemperature;
            WaterFlowRate = waterFlowRate;
            HumidityInsideThePipe = humidityInsideThePipe;
        }
    }
}
