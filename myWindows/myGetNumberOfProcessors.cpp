#include <stdio.h>
#include <stdlib.h>

#if defined (__NetBSD__) || defined(__OpenBSD__) || defined (__FreeBSD__) || defined (__APPLE__)
#include <sys/param.h>
#include <sys/sysctl.h>
#elif defined(__linux__) || defined(__CYGWIN__) || defined(sun)
#include <unistd.h>
#endif

namespace NWindows
{
	namespace NSystem
	{
		#if defined (__NetBSD__) || defined(__OpenBSD__)
		int GetNumberOfProcessors() {
			int mib[2];
			size_t value[2];
        		int nbcpu = 1;

        		mib[0] = CTL_HW;
        		mib[1] = HW_NCPU;
        		value[1] = sizeof(size_t);
        		if (sysctl(mib, 2, value, value+1, NULL, 0) >= 0)
           		if (value[0] > nbcpu)
                    		nbcpu = value[0];
			return nbcpu;
		}
		#elif defined (__FreeBSD__)
		int GetNumberOfProcessors() {
        		int nbcpu = 1;
			size_t value;
			size_t len = sizeof(value);
			if (sysctlbyname("hw.ncpu", &value, &len, NULL, 0) == 0)
				nbcpu = value;
			return nbcpu;
		}
		#elif defined (__APPLE__)
		int GetNumberOfProcessors() {
        		int nbcpu = 1,value;
			size_t valSize = sizeof(value);
			if (sysctlbyname ("hw.ncpu", &value, &valSize, NULL, 0) == 0)
				nbcpu = value;
			return nbcpu;
		}

		#elif defined(__linux__) || defined(__CYGWIN__) || defined(sun)
		int GetNumberOfProcessors() {
        		int nbcpu = sysconf (_SC_NPROCESSORS_CONF);
			if (nbcpu < 1) nbcpu = 1;
			return nbcpu;
		}
		#else
		#warning Generic GetNumberOfProcessors
		int GetNumberOfProcessors() {
			return 1;
		}
		#endif
	}
}

