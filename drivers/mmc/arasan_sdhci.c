/*
 * (C) Copyright 2013 - 2015 Xilinx, Inc.
 *
 * Xilinx Zynq SD Host Controller Interface
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <clk.h>
#include <common.h>
#include <dm.h>
#include <fdtdec.h>
#include <generic-phy.h>
#include <libfdt.h>
#include <malloc.h>
#include <power-domain.h>
#include <sdhci.h>

DECLARE_GLOBAL_DATA_PTR;

struct arasan_sdhci_plat {
	struct mmc_config cfg;
	struct mmc mmc;
	unsigned int f_max;
	struct phy phy;
};

static int arasan_sdhci_probe(struct udevice *dev)
{
	struct arasan_sdhci_plat *plat = dev_get_platdata(dev);
	struct mmc_uclass_priv *upriv = dev_get_uclass_priv(dev);
	struct sdhci_host *host = dev_get_priv(dev);
	struct power_domain sdhci_pwrdmn;
	struct clk clk;
	unsigned long clock;
	int ret;

	ret = power_domain_get_by_index(dev, &sdhci_pwrdmn, 0);
	if (!ret) {
		power_domain_on(&sdhci_pwrdmn);
	} else if (ret != -ENOENT && ret != -ENODEV && ret != -ENOSYS) {
		dev_err(dev, "power_domain_get() failed: %d\n", ret);
		return ret;
	}

	ret = clk_get_by_name(dev, "clk_xin", &clk);
	if (ret < 0) {
		dev_err(dev, "failed to get clock\n");
		return ret;
	}

	clock = clk_get_rate(&clk);
	if (IS_ERR_VALUE(clock)) {
		dev_err(dev, "failed to get rate\n");
		return clock;
	}
	debug("%s: CLK %ld\n", __func__, clock);

	ret = clk_enable(&clk);
	if (ret && ret != -ENOSYS) {
		dev_err(dev, "failed to enable clock\n");
		return ret;
	}

	if (device_is_compatible(dev, "ti,am654-sdhci-5.1")) {
		/* Get and init phy */
		ret = generic_phy_get_by_name(dev, "phy_arasan", &plat->phy);
		if (ret) {
			pr_err("can't get phy from DT\n");
			return ret;
		}

		ret = generic_phy_init(&plat->phy);
		if (ret) {
			pr_err("couldn't initialize MMC PHY\n");
			return ret;
		}
	}

	host->quirks = SDHCI_QUIRK_WAIT_SEND_CMD |
		       SDHCI_QUIRK_BROKEN_R1B;

#ifdef CONFIG_ZYNQ_HISPD_BROKEN
	host->quirks |= SDHCI_QUIRK_NO_HISPD_BIT;
#endif
	if (fdtdec_get_bool(gd->fdt_blob, dev_of_offset(dev),
			    "xlnx,fails-without-test-cd"))
		host->quirks |= SDHCI_QUIRK_FORCE_CD_TEST;

	switch (fdtdec_get_int(gd->fdt_blob, dev_of_offset(dev),
		"bus-width", 4)) {
	case 8:
		host->host_caps |= MMC_MODE_8BIT;
		break;
	case 4:
		host->host_caps |= MMC_MODE_4BIT;
		break;
	case 1:
		break;
	default:
		printf("Invalid \"bus-width\" value\n");
		return -EINVAL;
	}

	host->max_clk = clock;

	ret = sdhci_setup_cfg(&plat->cfg, host, plat->f_max,
			      CONFIG_ARASAN_SDHCI_MIN_FREQ);
	host->mmc = &plat->mmc;
	if (ret)
		return ret;
	host->mmc->priv = host;
	host->mmc->dev = dev;
	upriv->mmc = host->mmc;

	return sdhci_probe(dev);
}

static int arasan_sdhci_ofdata_to_platdata(struct udevice *dev)
{
	struct arasan_sdhci_plat *plat = dev_get_platdata(dev);
	struct sdhci_host *host = dev_get_priv(dev);

	host->name = dev->name;
	host->ioaddr = (void *)devfdt_get_addr(dev);

	plat->f_max = fdtdec_get_int(gd->fdt_blob, dev_of_offset(dev),
				"max-frequency", CONFIG_ARASAN_SDHCI_MAX_FREQ);

	return 0;
}

static int arasan_sdhci_bind(struct udevice *dev)
{
	struct arasan_sdhci_plat *plat = dev_get_platdata(dev);

	return sdhci_bind(dev, &plat->mmc, &plat->cfg);
}

static const struct udevice_id arasan_sdhci_ids[] = {
	{ .compatible = "ti,am654-sdhci-5.1"},
	{ .compatible = "arasan,sdhci-8.9a" },
	{ .compatible = "arasan,sdhci-5.1"  },
	{ }
};

U_BOOT_DRIVER(arasan_sdhci_drv) = {
	.name		= "arasan_sdhci",
	.id		= UCLASS_MMC,
	.of_match	= arasan_sdhci_ids,
	.ofdata_to_platdata = arasan_sdhci_ofdata_to_platdata,
	.ops		= &sdhci_ops,
	.bind		= arasan_sdhci_bind,
	.probe		= arasan_sdhci_probe,
	.priv_auto_alloc_size = sizeof(struct sdhci_host),
	.platdata_auto_alloc_size = sizeof(struct arasan_sdhci_plat),
};