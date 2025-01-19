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
            int Id,
            int WaterLevel1,
            int WaterLevel2,
            int Degree_of_clogging1,
            int Degree_of_clogging2,
            int Header1,
            int Header2,
            string alarm,
            string errors,
            int Structural_deformations);
    }
}
