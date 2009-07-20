/*
 * Copyright (C) 2009  Paul Mundt
 *
 * This file prints out all of the supported C-states for platforms using
 * a generic cpuidle topology. (OMAP3, SuperH, etc.). Derived from
 * intelcstates.c.
 *
 * This program file is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program in a file named COPYING; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>

#include "powertop.h"

#if !defined(__i386__)
/**
 * print_cstates() - Prints the list of supported C-states.
 *
 * This functions uses standard sysfs interface of the cpuidle framework
 * to extract the information of the C-states supported by the Linux
 * kernel.
 */
void print_cstates(void)
{
	unsigned long cstate_bitmap = 0;
	DIR *cpudir;
	DIR *dir;
	FILE *file = NULL;
	char line[4];
	char filename[128], *f;
	struct dirent *entry;
	int len, i;

	cpudir = opendir("/sys/devices/system/cpu");
	if (!cpudir)
		return;

	/* Loop over cpuN entries */
	while ((entry = readdir(cpudir))) {
		if (strlen(entry->d_name) < 3)
			continue;

		if (!isdigit(entry->d_name[3]))
			continue;

		len = sprintf(filename, "/sys/devices/system/cpu/%s/cpuidle",
			      entry->d_name);

		dir = opendir(filename);
		if (!dir)
			return;

		/*
		 * For each C-state, there is a stateX directory which
		 * contains a 'name' file (amongst others). Iterate over
		 * all supported states and cache the supported C-states
		 * in a bitmap
		 */
		while ((entry = readdir(dir))) {
			if (strlen(entry->d_name) < 3)
				continue;

			sprintf(filename + len, "/%s/name", entry->d_name);

			file = fopen(filename, "r");
			if (file) {
				memset(line, 0, sizeof(line));
				f = fgets(line, sizeof(line), file);
				fclose(file);
				if (f == NULL)
					break;

				cstate_bitmap |= (1 << strtoul(f+1, NULL, 10));
			}
		}

		closedir(dir);
	}

	closedir(cpudir);

	if (!cstate_bitmap)
		return;

	printf(_("Your CPU supports the following C-states : "));

	/* Now iterate over the C-state bitmap */
	for (i = 0; cstate_bitmap; i++)
		if (cstate_bitmap & (1 << i)) {
			printf("C%i ", i);
			cstate_bitmap &= ~(1 << i);
		}

	printf("\n");
}
#endif
