# ***********************************************************************
#
# EMPIRE Makefile include for FORTRAN 90
#
# Usage: make DEPF90
#        make
#
# Author: Michael Andre, Chair for Structural Analysis, TUM
#
# ***********************************************************************

Q ?= @
MAKEDEPF90 := makedepf90
DEPFILEF90 := depend.mk
export MAKEDEPF90

SEARCH_PATHS := $(SRC_DIR_MAIN) $(patsubst %, $(SRC_DIR_MAIN)/%, $(SUB_DIR))
SRCSF90 := $(foreach search_path, $(SEARCH_PATHS), $(wildcard $(search_path)/*.f90))

DEPF90: $(BUILD_DIR)
	@echo "===>  MAKEDEPF90"
	$(Q)$(MAKEDEPF90) \
	--multi-target --warn-parsing --headers \
	--module-path $(MOD_DIR) \
	--object-path $(BUILD_DIR) \
	--glob-rule '*.f90' '$(Q)$(FC) $(FCFLAGS) -c @source@ -o @object@' \
	$(SRCSF90) > $(DEPFILEF90)

-include $(DEPFILEF90)
