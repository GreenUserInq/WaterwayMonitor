using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataPublisher.Common.Models
{
    class TimerConfig
    {
        public TimerConfig(int interval, bool isEnabled)
        {
            IntervalMilliseconds = interval;
            IsEnabled = isEnabled;
        }

        public TimerConfig() { }
        public int IntervalMilliseconds { get; set; }
        public bool IsEnabled { get; set; }
    }
}
