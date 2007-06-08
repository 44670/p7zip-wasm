#include <stdio.h>
#include <stdlib.h>

#if defined (__NetBSD__) || defined(__OpenBSD__) || defined (__FreeBSD__) || defined (__APPLE__)
#include <sys/param.h>
#include <sys/sysctl.h>
#elif defined(__linux__) || defined(__CYGWIN__) || defined(sun)
#include <unistd.h>
#endif

#include "Common/Types.h"

namespace NWindows
{
	namespace NSystem
	{
		/************************ GetNumberOfProcessors ************************/

		#if defined (__NetBSD__) || defined(__OpenBSD__)
		int GetNumberOfProcessors() {
			int mib[2], value;
        		int nbcpu = 1;

        		mib[0] = CTL_HW;
        		mib[1] = HW_NCPU;
        		size_t len = sizeof(size_t);
        		if (sysctl(mib, 2, &value, &len, NULL, 0) >= 0)
           		if (value > nbcpu)
                    		nbcpu = value;
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

		/************************ GetRamSize ************************/
UInt64 GetRamSize() {
    UInt64 ullTotalPhys = 128 * 1024 * 1024; // FIXME 128MB

#ifdef linux
    FILE * f = fopen( "/proc/meminfo", "r" );
    if (f)
    {
        char buffer[256];
        unsigned long total;

	ullTotalPhys = 0;

        while (fgets( buffer, sizeof(buffer), f ))
        {
	    /* old style /proc/meminfo ... */
            if (sscanf( buffer, "Mem: %lu", &total))
            {
                ullTotalPhys += total;
            }

            /* new style /proc/meminfo ... */
            if (sscanf(buffer, "MemTotal: %lu", &total))
                ullTotalPhys = ((UInt64)total)*1024;
        }
        fclose( f );
    }
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__NetBSD__) || defined(__APPLE__)
    unsigned long val;
    int mib[2];

    mib[0] = CTL_HW;
    mib[1] = HW_PHYSMEM;
    size_t size_sys = sizeof(val);
    sysctl(mib, 2, &val, &size_sys, NULL, 0);
    if (val) ullTotalPhys = val;
#elif defined(__CYGWIN__)
    unsigned long pagesize=4096; // FIXME - sysconf(_SC_PAGESIZE) returns 65536 !?
                                 // see http://readlist.com/lists/cygwin.com/cygwin/0/3313.html
    unsigned long maxpages=sysconf(_SC_PHYS_PAGES);
    ullTotalPhys = ((UInt64)pagesize)*maxpages;
#elif defined ( sun )
    unsigned long pagesize=sysconf(_SC_PAGESIZE);
    unsigned long maxpages=sysconf(_SC_PHYS_PAGES);
    ullTotalPhys = ((UInt64)pagesize)*maxpages;
#else
#warning Generic GetRamSize
#endif

    return ullTotalPhys;
}

	}
}

