/*
 * Soft:        Keepalived is a failover program for the LVS project
 *              <www.linuxvirtualserver.org>. It monitor & manipulate
 *              a loadbalanced server pool using multi-layer checks.
 *
 * Part:        vrrp_track.c include file.
 *
 * Author:      Alexandre Cassen, <acassen@linux-vs.org>
 *
 *              This program is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *              See the GNU General Public License for more details.
 *
 *              This program is free software; you can redistribute it and/or
 *              modify it under the terms of the GNU General Public License
 *              as published by the Free Software Foundation; either version
 *              2 of the License, or (at your option) any later version.
 *
 * Copyright (C) 2001-2012 Alexandre Cassen, <acassen@gmail.com>
 */

#ifndef _VRRP_TRACK_H
#define _VRRP_TRACK_H

/* global includes */
#include <stdbool.h>
#include <sys/types.h>

/* local includes */
#include "vector.h"
#include "list.h"
#include "vrrp_if.h"
#include "vrrp.h"

/* VRRP script tracking defaults */
#define VRRP_SCRIPT_DI 1	/* external script track interval (in sec) */
#define VRRP_SCRIPT_DT 0	/* external script track timeout (in sec) */
#define VRRP_SCRIPT_DW 0	/* external script default weight */

/* VRRP script tracking results.
 * The result is an integer between 0 and rise-1 to indicate a DOWN state,
 * or between rise-1 and rise+fall-1 to indicate an UP state. Upon failure,
 * we decrease result and set it to zero when we pass below rise. Upon
 * success, we increase result and set it to rise+fall-1 when we pass above
 * rise-1.
 */
#define VRRP_SCRIPT_STATUS_TIMEDOUT	-5
#define VRRP_SCRIPT_STATUS_NOT_SET	-4
#define VRRP_SCRIPT_STATUS_DISABLED	-3
#define VRRP_SCRIPT_STATUS_INIT_FAILED	-2
#define VRRP_SCRIPT_STATUS_INIT		-1

/* If a VRRP instance doesn't track it's own interface, we still
 * want the interface to have a reference to the VRRP instance,
 * but it needs to know the instance isn't tracking it. */
#define	VRRP_NOT_TRACK_IF	255

/* external script we call to track local processes */
typedef struct _vrrp_script {
	char			*sname;		/* instance name */
	notify_script_t		script;		/* The script details */
	unsigned long		interval;	/* interval between script calls */
	unsigned long		timeout;	/* microseconds before script timeout */
	int			weight;		/* weight associated to this script */
	int			result;		/* result of last call to this script: 0..R-1 = KO, R..R+F-1 = OK */
	int			rise;		/* R: how many successes before OK */
	int			fall;		/* F: how many failures before KO */
	list			vrrp;		/* List of vrrp instances using this script */
	int			last_status;	/* Last status returned by script. Used to report changes */
	bool			forcing_termination;	/* Set if script didn't respond and we sent it SIGTERM */
	bool			insecure;	/* Set if script is run by root, but is non-root modifiable */
} vrrp_script_t;

/* Tracked script structure definition */
typedef struct _tracked_sc {
	int			weight;		/* tracking weight when non-zero */
	vrrp_script_t		*scr;		/* script pointer, cannot be NULL */
} tracked_sc_t;

/* Forward references */
struct _vrrp_t;

/* Tracked interface structure definition */
typedef struct _tracking_vrrp {
	int			weight;		/* Tracking weight, or zero for down instance */
	struct _vrrp_t		*vrrp;		/* The vrrp instance */
} tracking_vrrp_t;

/* prototypes */
extern void dump_track(void *);
extern void free_track(void *);
extern void alloc_track(struct _vrrp_t *, vector_t *);
extern void dump_track_script(void *);
extern void free_track_script(void *);
extern void alloc_track_script(struct _vrrp_t *, vector_t *);
extern vrrp_script_t *find_script_by_name(char *);
extern void update_script_priorities(vrrp_script_t *, bool);
extern void down_instance(struct _vrrp_t *);
extern void initialise_tracking_priorities(struct _vrrp_t *);

#endif
