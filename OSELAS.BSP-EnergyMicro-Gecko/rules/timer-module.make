# -*-makefile-*-
#
# Copyright (C) 2017 by <TL>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_TIMER_MODULE) += timer-module

#
# Paths and names and versions
#
TIMER_MODULE_VERSION	:= 1
TIMER_MODULE		:= timer-module-$(TIMER_MODULE_VERSION)
TIMER_MODULE_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/$(TIMER_MODULE)
TIMER_MODULE_DIR		:= $(BUILDDIR)/$(TIMER_MODULE)
TIMER_MODULE_LICENSE	:= unknown

ifdef PTXCONF_TIMER_MODULE
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/timer-module.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/timer-module.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/timer-module.compile:
	@$(call targetinfo)
	$(KERNEL_PATH) $(KERNEL_ENV) $(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(TIMER_MODULE_DIR) \
		modules
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/timer-module.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/timer-module.targetinstall:
	@$(call targetinfo)
	$(KERNEL_PATH) $(KERNEL_ENV) $(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(TIMER_MODULE_DIR) \
		modules_install
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/timer-module.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, TIMER_MODULE)

# vim: syntax=make
