/*
 * Copyright (C) 2001  Sistina Software
 *
 * LVM is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * LVM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LVM; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#include "tools.h"

static int vgscan_single(const char *vg_name);

int vgscan(int argc, char **argv)
{
	if (argc) {
		log_error("Too many parameters on command line");
		return EINVALID_CMD_LINE;
	}

	log_print("Reading all physical volumes (this may take a while...)");

	return process_each_vg(argc, argv, &vgscan_single);
}

static int vgscan_single(const char *vg_name)
{
	struct volume_group *vg;

	log_verbose("Checking for volume group %s", vg_name);
	if (!(vg = ios->vg_read(ios, vg_name))) {
		log_error("Volume group %s not found", vg_name);
		return ECMD_FAILED;
	}

	log_print("Found %sactive volume group %s",
		  (vg->status & ACTIVE) ? "" : "in", vg_name);

	/* FIXME: Now try to activate any inactive logical volumes */
	/* activate_lvs(vg) ? */

	if (!(vg->status & ACTIVE)) {
		vg->status |= ACTIVE;
		if (!(ios->vg_write(ios, vg))) {
			log_error("Failed to activate volume group %s",
				  vg_name);
			return ECMD_FAILED;
		}

		/* FIXME: Create /dev/vg entries? */
		log_print("Volume Group %s activated", vg_name);
	}

	return 0;
}
