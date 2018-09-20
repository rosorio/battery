/*
 *  Copyright (c) 2008, Rodrigo Osorio <rodrigo@osorio.me>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of  nor the names of its contributors may be used to
 *    endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/user.h>
#include <unistd.h>
#include <dev/acpica/acpiio.h>

#define COLOR_RED    "\033[31m"
#define COLOR_GREEN  "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_NONE   "\033[0m"

/*
 * get_battery_status()
 * This function returns the battery current status
 */
int
get_battery_state()
{
    int mib[4];
    size_t len;
    int bst;

    /* Fill out the first three components of the mib */
    len = 4;
    sysctlnametomib("hw.acpi.battery.state", mib, &len);
    len = sizeof(int);
    if (sysctl(mib, 4, &bst, &len, NULL, 0) == -1)
    {
    	/* No acpi informations found */
    	fprintf(stderr,"No battery informations found, maybe you don't turn the acpi on\n");
	    return(-1);
    }

    return(bst);
}

/*
 * get_battery_percent()
 * This function returns the battery usage in percent
 */
int
get_battery_percent()
{
	int mib[4];
    size_t len;
    int bpc;

    /* Fill out the first three components of the mib */
    len = 4;
    sysctlnametomib("hw.acpi.battery.life", mib, &len);
    len = sizeof(int);
    if (sysctl(mib, 4, &bpc, &len, NULL, 0) == -1)
    {
    	/* No acpi informations found */
    	fprintf(stderr,"No battery informations found, maybe you don't turn the acpi on\n");
	    return(-1);
    }

    return(bpc);
}


/*
 * get_battery_time()
 * This function returns the battery remaining time in min
 */
int
get_battery_time()
{
	int mib[4];
    size_t len;
    int bpc;

    /* Fill out the first three components of the mib */
    len = 4;
    sysctlnametomib("hw.acpi.battery.time", mib, &len);
    len = sizeof(int);
    if (sysctl(mib, 4, &bpc, &len, NULL, 0) == -1)
    {
    	/* No acpi informations found */
    	fprintf(stderr,"No battery informations found, maybe you don't turn the acpi on\n");
	    return(-1);
    }

    return(bpc);
}


int main(int argc, char * argv[])
{
	int state;
	int charge,charge2;
	int i;
    int btime;
    int flag_color = 0;
    int flag_short = 0;
    int ch;


    while ((ch = getopt(argc, argv, "cs")) != -1) {
        switch (ch) {
            case 'c':
                    flag_color = 1;
                break;
            case 's':
                    flag_short++;
                break;
            default:
                /* Unknown command */
                break;
        }
    }

	btime = get_battery_time();
	/*
	 * Get the battery state
	 */
	state = get_battery_state();
	if(state == -1)
	{
		/*
		 * This us is performed to exit when the battery ACPI
		 * informations aren't available on the computer
		 */
		return(1);
	}

	if(state & ACPI_BATT_STAT_DISCHARG)
	{
		if(state & ACPI_BATT_STAT_CRITICAL)
		{
            if (flag_color) printf(COLOR_RED);
            printf("%s",flag_short ? "C" : "CRIT");
		} else {
            if (flag_color) printf(COLOR_GREEN);
            printf("%s",flag_short ? "B" : "BAT");
		}


	} else if((state & ACPI_BATT_STAT_CHARGING)
	       || (state == 0) || (state & ACPI_BATT_STAT_NOT_PRESENT))
	{
        if (flag_color) printf(COLOR_GREEN);
        printf("%s",flag_short ? "P" : "A/C");
	} else {
        printf("%s",flag_short ? "?" : "UNKNOWN");
	}

	printf(" [");
	charge = get_battery_percent();
	charge2 = charge/10;
	for (i = 0; i < 10; i++)
	{
		if(i<(charge2))
		{
			printf("#");
		}
		else
		{
			printf("_");
		}
	}

	printf("] ");
	if(btime < 0 ) {
        if (flag_color)
		    printf(COLOR_NONE);
		return 0;
	}

	if(btime > 60)
		printf("%d:", btime / 60);
	printf("%02d", btime % 60);
    if (flag_color)
	    printf(COLOR_NONE);

	return(0);
}
