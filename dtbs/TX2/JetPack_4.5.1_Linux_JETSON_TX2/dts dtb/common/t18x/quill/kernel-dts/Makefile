old-dtb := $(dtb-y)
old-dtbo := $(dtbo-y)
dtb-y :=
dtbo-y :=
makefile-path := platform/t18x/quill/kernel-dts

dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3310-1000-a00-00-base.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3310-1000-a00-00-ape-cam.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3310-1000-a00-00-edp.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3310-1000-a00-00-dsi-hdmi-dp.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3310-1000-c03-00-dsi-hdmi-dp.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3310-1000-a00-00-dsi-dp.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-hdmi-primary-p3310-1000-c03-00.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-hdmi-primary-p3310-1000-a00-00.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3310-1000-a00-00-base-mods-display.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3310-1000-a00-00-imx274.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3310-1000-a00-00-imx185_v1.dtb
#dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3310-1000-a00-00-lc898212.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-hdmi-primary-p3310-1000-a00-00-imx274.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-hdmi-primary-p3310-1000-c03-00-imx274.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3310-1000-c03-00-ape-cam.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3310-1000-c03-00-base.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3310-1000-c03-00-imx185_v1.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3310-1000-c03-00-dsi-dp.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3310-1000-c03-00-dsi-hdmi-hdmi.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3310-1000-c03-00-dsi-hdmi-hdmi-imx274-hdmiin.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3310-1000-c00-00-auo-1080p-edp.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-e3313-1000-a00-00-e2598.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3489-1000-a00-00-ucm1.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3489-1000-a00-00-ucm2.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3489-1000-a00-00-e2598.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3489-0888-a00-00-base.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3489-0888-a00-00-e2598.dtb
dtb-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3310-1000-as-0888.dtb
dtbo-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3310-1000-a00-overlay.dtbo
dtbo-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3310-1000-c03-00-base-hdr40.dtbo
dtbo-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3310-1000-c03-00-base-adafruit-sph0645lm4h.dtbo
dtbo-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3310-1000-c03-00-base-fe-pi-audio.dtbo
dtbo-$(CONFIG_ARCH_TEGRA_18x_SOC) += tegra186-quill-p3310-1000-c03-00-base-respeaker-4-mic-array.dtbo

ifneq ($(dtb-y),)
dtb-y := $(addprefix $(makefile-path)/,$(dtb-y))
dts-include += $(makefile-path)
endif
ifneq ($(dtbo-y),)
dtbo-y := $(addprefix $(makefile-path)/,$(dtbo-y))
endif

dtb-y += $(old-dtb)
dtbo-y += $(old-dtbo)
