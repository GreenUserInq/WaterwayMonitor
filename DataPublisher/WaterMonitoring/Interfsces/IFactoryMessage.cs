using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataPublisher.WaterMonitoring.Interfsces
{
    interface IFactoryMessage
    {
        string GetPostMessage(
            int ID,
            int Water_level,
            int Degree_of_clogging,
            int Structural_deformations,
            int Ambient_temperature,
            int Water_flow_rate,
            int Humidity_inside_the_pipe);
    }
}
