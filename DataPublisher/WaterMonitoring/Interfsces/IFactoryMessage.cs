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
            int waterlevel1,
            int waterlevel2,
            int DegreeOfClogging1,
            int DegreeOfClogging2,
            int CloggingHeader1,
            int CloggingHeader2,
            int StructuralDeformations,
            string AlarmState,
            string Errors);
    }
}
