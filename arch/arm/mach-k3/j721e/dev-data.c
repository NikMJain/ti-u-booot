// SPDX-License-Identifier: GPL-2.0+
/*
 * J721E specific device platform data
 *
 * Copyright (C) 2020 Texas Instruments Incorporated - http://www.ti.com/
 */
#include "k3-dev.h"

static struct ti_psc soc_psc_list[] = {
	[0] = PSC(0, 0x00400000),
	[1] = PSC(1, 0x42000000),
};

static struct ti_pd soc_pd_list[] = {
	[0] = PSC_PD(0, &soc_psc_list[0], NULL),
	[1] = PSC_PD(14, &soc_psc_list[0], NULL),
	[2] = PSC_PD(15, &soc_psc_list[0], &soc_pd_list[1]),
	[3] = PSC_PD(16, &soc_psc_list[0], &soc_pd_list[1]),
	[4] = PSC_PD(0, &soc_psc_list[1], NULL),
};

static struct ti_lpsc soc_lpsc_list[] = {
	[0] = PSC_LPSC(0, &soc_psc_list[0], &soc_pd_list[0], NULL),
	[1] = PSC_LPSC(7, &soc_psc_list[0], &soc_pd_list[0], NULL),
	[2] = PSC_LPSC(14, &soc_psc_list[0], &soc_pd_list[0], &soc_lpsc_list[3]),
	[3] = PSC_LPSC(15, &soc_psc_list[0], &soc_pd_list[0], NULL),
	[4] = PSC_LPSC(23, &soc_psc_list[0], &soc_pd_list[0], NULL),
	[5] = PSC_LPSC(25, &soc_psc_list[0], &soc_pd_list[0], NULL),
	[6] = PSC_LPSC(78, &soc_psc_list[0], &soc_pd_list[1], NULL),
	[7] = PSC_LPSC(80, &soc_psc_list[0], &soc_pd_list[2], &soc_lpsc_list[6]),
	[8] = PSC_LPSC(81, &soc_psc_list[0], &soc_pd_list[3], &soc_lpsc_list[6]),
	[9] = PSC_LPSC(0, &soc_psc_list[1], &soc_pd_list[4], NULL),
	[10] = PSC_LPSC(3, &soc_psc_list[1], &soc_pd_list[4], NULL),
	[11] = PSC_LPSC(10, &soc_psc_list[1], &soc_pd_list[4], NULL),
	[12] = PSC_LPSC(11, &soc_psc_list[1], &soc_pd_list[4], NULL),
};

static struct ti_dev soc_dev_list[] = {
	PSC_DEV(30, &soc_lpsc_list[0]),
	PSC_DEV(61, &soc_lpsc_list[0]),
	PSC_DEV(146, &soc_lpsc_list[1]),
	PSC_DEV(90, &soc_lpsc_list[2]),
	PSC_DEV(47, &soc_lpsc_list[3]),
	PSC_DEV(92, &soc_lpsc_list[4]),
	PSC_DEV(91, &soc_lpsc_list[5]),
	PSC_DEV(4, &soc_lpsc_list[6]),
	PSC_DEV(202, &soc_lpsc_list[7]),
	PSC_DEV(203, &soc_lpsc_list[8]),
	PSC_DEV(103, &soc_lpsc_list[9]),
	PSC_DEV(104, &soc_lpsc_list[9]),
	PSC_DEV(154, &soc_lpsc_list[9]),
	PSC_DEV(149, &soc_lpsc_list[9]),
	PSC_DEV(197, &soc_lpsc_list[10]),
	PSC_DEV(103, &soc_lpsc_list[11]),
	PSC_DEV(104, &soc_lpsc_list[12]),
};

const struct ti_k3_pd_platdata j721e_pd_platdata = {
	.psc = soc_psc_list,
	.pd = soc_pd_list,
	.lpsc = soc_lpsc_list,
	.devs = soc_dev_list,
	.num_psc = 2,
	.num_pd = 5,
	.num_lpsc = 13,
	.num_devs = 17,
};