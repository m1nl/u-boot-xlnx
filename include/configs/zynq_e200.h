/* SPDX-License-Identifier: GPL-2.0+ */

#ifndef __CONFIG_ZYNQ_E200_H
#define __CONFIG_ZYNQ_E200_H

#define CFG_EXTRA_ENV_SETTINGS \
	"ipaddr=192.168.2.1\0" \
	"ipaddr_host=192.168.2.10\0" \
	"netmask=255.255.255.0\0" \
	"kernel_image=uImage\0" \
	"fit_load_address=0x2080000\0" \
	"fit_config=config@0\0" \
	"extraenv_load_address=0x207E000\0" \
	"ramdisk_image=uramdisk.image.gz\0" \
	"ramdisk_load_address=0x4000000\0" \
	"devicetree_image=devicetree.dtb\0" \
	"devicetree_load_address=0x2000000\0" \
	"fdt_overlay_load_address=0x2008000\0" \
	"bitstream_image=system.bit.bin\0" \
	"boot_image=BOOT.bin\0" \
	"loadbit_addr=0x100000\0" \
	"loadbootenv_addr=0x2000000\0" \
	"fit_size=0x900000\0" \
	"devicetree_size=0x20000\0" \
	"ramdisk_size=0x400000\0" \
	"bitstream_size=0x400000\0" \
	"boot_size=0xF00000\0" \
	"fdt_high=0x20000000\0" \
	"initrd_high=0x20000000\0" \
	"bootenv=uEnv.txt\0" \
	"maxcpus=2\0" \
    "fdt_overlays=\0" \
	"clear_reset_cause=mw f8000008 df0d && mw f8000258 00400000 && mw f8000004 767b\0" \
	"loadbootenv=load mmc 0 ${loadbootenv_addr} ${bootenv}\0" \
	"importbootenv=echo Importing environment from SD ...; " \
		"env import -t ${loadbootenv_addr} $filesize\0" \
	"sd_uEnvtxt_existence_test=test -e mmc 0 /uEnv.txt\0" \
	"preboot=if test $modeboot = sdboot && env run sd_uEnvtxt_existence_test; " \
			"then if env run loadbootenv; " \
				"then env run importbootenv; " \
			"fi; " \
		"fi; \0" \
	"transceiver=ad9361\0" \
	"mode=1r1t\0" \
	"loadvals_e200=if test -n \"${transceiver}\" && test ! \"${transceiver}\" = \"ad9361\" && test ! \"${transceiver}\" = \"ad9363a\"; then " \
			"setenv transceiver ad9361; " \
			"saveenv; " \
		"fi; " \
		"if test -n \"${mode}\" && test ! \"${mode}\" = \"1r1t\" && test ! \"${mode}\" = \"2r2t\"; then " \
			"setenv mode 1r1t; " \
			"saveenv; " \
		"fi; " \
		"if test -n \"${transceiver}\" && test \"${model}\" = \"Microphase AntSDR E200 Rev.C (Z7020/AD936x)\"; then " \
			"fdt set /axi/spi@e0006000/ad9361-phy@0 compatible ${transceiver}; " \
		"fi; " \
		"if test \"${mode}\" = \"1r1t\" && test \"${model}\" = \"Microphase AntSDR E200 Rev.C (Z7020/AD936x)\"; then " \
			"fdt rm /axi/spi@e0006000/ad9361-phy@0 adi,2rx-2tx-mode-enable; " \
			"fdt set /fpga-axi@0/cf-ad9361-dds-core-lpc@79024000 compatible adi,axi-ad9364-dds-6.00.a; " \
		"fi; " \
		"if test -n \"${cs_gpio}\" && test \"${model}\" = \"Microphase AntSDR E200 Rev.C (Z7020/AD936x)\"; then " \
			"fdt set /fpga-axi@0/spi@7C430000/ cs-gpios \"<0x06 ${cs_gpio} 0>\"; " \
		"fi; " \
        "true; \0" \
	"adi_loadvals=fdt addr ${fit_load_address} && fdt get value fdt_choosen /configurations/${fit_config}/ fdt && " \
		"fdt get addr fdtaddr /images/${fdt_choosen} data && fdt addr ${fdtaddr} && " \
		"fdt get value model / model && " \
		"run loadvals_e200; \0" \
	"adi_loadvals_sdcard=fdt addr ${devicetree_load_address}; " \
		"fdt get value model / model && " \
		"run loadvals_e200; \0" \
	"qspiboot_extraenv=sf read ${extraenv_load_address} 0xFF000 0x1000 && " \
		"env import -c ${extraenv_load_address} 0x1000 || true; \0" \
	"read_sf=sf probe 0:0 50000000 0 && run qspiboot_extraenv &&" \
		"sf read ${fit_load_address} 0x200000 ${fit_size} && " \
		"iminfo ${fit_load_address} || " \
		"sf read ${fit_load_address} 0x200000  0x1E00000; \0" \
	"ramboot_verbose=echo Copying Linux from DFU to RAM... && " \
		"run dfu_ram;" \
		"if run adi_loadvals; then " \
		"echo Loaded AD936x model into devicetree; " \
		"fi; " \
		"setenv bootargs console=ttyPS0,115200 maxcpus=${maxcpus} isolcpus=1 rootfstype=ramfs root=/dev/ram0 rw earlyprintk clk_ignore_unused uio_pdrv_genirq.of_id=uio_pdrv_genirq\" && " \
		"bootm ${fit_load_address}#${fit_config}; \0" \
	"qspiboot_verbose=echo Copying Linux from QSPI flash to RAM... && " \
		"run read_sf && " \
		"if run adi_loadvals; then " \
		"echo Loaded AD936x model into devicetree; " \
		"fi; " \
		"setenv bootargs console=ttyPS0,115200 maxcpus=${maxcpus} isolcpus=1 rootfstype=ramfs root=/dev/ram0 rw earlyprintk clk_ignore_unused uio_pdrv_genirq.of_id=uio_pdrv_genirq\" && " \
		"bootm ${fit_load_address}#${fit_config} || echo BOOT failed entering DFU mode ... && run dfu_sf; \0" \
	"qspiboot=set stdout serial@e0000000;" \
		"itest *f8000258 == 480003 && run clear_reset_cause && sf probe && sf protect unlock 0 100000 && run dfu_sf; " \
		"itest *f8000258 == 480007 && run clear_reset_cause && run ramboot_verbose; " \
		"itest *f8000258 == 480006 && run clear_reset_cause && run qspiboot_verbose; " \
		"itest *f8000258 == 480002 && run clear_reset_cause && exit; " \
		"echo Booting silently && set stdout nulldev; " \
		"run read_sf && run adi_loadvals; " \
		"setenv bootargs \"console=ttyPS0,115200 maxcpus=${maxcpus} isolcpus=1 rootfstype=ramfs root=/dev/ram0 rw clk_ignore_unused uio_pdrv_genirq.of_id=uio_pdrv_genirq\" && " \
		"bootm ${fit_load_address}#${fit_config} || set stdout serial@e0000000;echo BOOT failed entering DFU mode ... && sf protect lock 0 100000 && run dfu_sf; \0" \
	"jtagboot=env default -a;sf probe && sf protect unlock 0 100000 && run dfu_sf; \0" \
	"uenvboot=" \
		"if run loadbootenv; then " \
			"echo Loaded environment from ${bootenv}; " \
			"run importbootenv; " \
		"fi; " \
		"if test -n $uenvcmd; then " \
			"echo Running uenvcmd ...; " \
			"run uenvcmd; " \
		"fi; \0" \
	"sdboot=" \
		"if mmcinfo; then " \
			"run uenvboot; " \
			"echo Copying Linux from SD to RAM... && " \
			"load mmc 0 ${fit_load_address} ${kernel_image} && " \
			"load mmc 0 ${devicetree_load_address} ${devicetree_image} &&" \
			"if run adi_loadvals_sdcard; then " \
				"echo Loaded AD936x model into devicetree; " \
			"fi && " \
			"run apply_fdt_overlays_sdcard && " \
			"setenv bootargs \"console=ttyPS0,115200 maxcpus=${maxcpus} root=/dev/mmcblk0p2 rw clk_ignore_unused uio_pdrv_genirq.of_id=uio_pdrv_genirq\" && " \
			"bootm ${fit_load_address} - ${devicetree_load_address}; " \
		"fi; \0" \
	"apply_fdt_overlays_sdcard=" \
		"fdt resize 32768 && " \
		"for fdt_overlay in ${fdt_overlays}; do " \
			"echo \"Applying FDT overlay: ${fdt_overlay}\"; " \
			"load mmc 0 ${fdt_overlay_load_address} ${fdt_overlay} && " \
			"fdt apply ${fdt_overlay_load_address}; " \
		"done; \0"

#include <configs/zynq-common.h>

#endif /* __CONFIG_ZYNQ_E200_H */
