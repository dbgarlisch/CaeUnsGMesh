########################################################################
# Pointwise - Proprietary software product of Pointwise, Inc.
#             Copyright (c) 1995-2014 Pointwise, Inc.
#             All rights reserved.
#
# module.mk for src\plugins\CaeUnsGMesh plugin
########################################################################

########################################################################
########################################################################
#
#                   DO NOT EDIT THIS FILE
#
# To simplify SDK upgrades, the standard module.mk file should NOT be edited.
#
# If you want to modify a plugin's build process, you should rename
# modulelocal-sample.mk to modulelocal.mk and edit its settings.
#
# See the comments in modulelocal-sample.mk for more information.
#
#                   DO NOT EDIT THIS FILE
#
########################################################################
########################################################################

CaeUnsGMesh_LOC := $(PLUGINS_LOC)/CaeUnsGMesh
CaeUnsGMesh_LIB := CaeUnsGMesh$(DBG_SUFFIX)
CaeUnsGMesh_CXX_LOC := $(CaeUnsGMesh_LOC)
CaeUnsGMesh_OBJ_LOC := $(PLUGINS_OBJ_LOC)/CaeUnsGMesh

CaeUnsGMesh_FULLNAME := lib$(CaeUnsGMesh_LIB).$(SHLIB_SUFFIX)
CaeUnsGMesh_FULLLIB := $(PLUGINS_DIST_DIR)/$(CaeUnsGMesh_FULLNAME)

CaeUnsGMesh_DEPS = \
    $(NULL)

MODCXXFILES := \
    runtimeWrite.cxx CaeUnsGMesh.cxx \
    $(NULL)

# IMPORTANT:
# Must recompile the shared/XXX/.cxx files for each plugin. These .cxx files
# include the plugin specific settings defined in the ./CaeUnsGMesh/*.h
# files.
CaeUnsGMesh_SRC := \
    $(PLUGINS_RT_PWPSRC) \
    $(PLUGINS_RT_PWGMSRC) \
    $(PLUGINS_RT_CAEPSRC) \
    $(patsubst %,$(CaeUnsGMesh_CXX_LOC)/%,$(MODCXXFILES))

CaeUnsGMesh_SRC_CXX := $(filter %.cxx,$(MODCXXFILES))

# place the .o files generated from shared sources in the plugin's
# OBJ folder.
CaeUnsGMesh_OBJ := \
    $(patsubst %.cxx,$(CaeUnsGMesh_OBJ_LOC)/%.o,$(PLUGINS_RT_PWPFILES)) \
    $(patsubst %.cxx,$(CaeUnsGMesh_OBJ_LOC)/%.o,$(PLUGINS_RT_PWGMFILES)) \
    $(patsubst %.cxx,$(CaeUnsGMesh_OBJ_LOC)/%.o,$(PLUGINS_RT_CAEPFILES)) \
    $(patsubst %.cxx,$(CaeUnsGMesh_OBJ_LOC)/%.o,$(CaeUnsGMesh_SRC_CXX)) \
    $(NULL)

# To allow over-rides, search FIRST for headers in the local module's folder.
# For example, a site.h file in the local module's folder will preempt the
# file .../src/plugins/site.h
CaeUnsGMesh_INCL = \
    -I$(CaeUnsGMesh_LOC) \
    $(PLUGINS_RT_INCL) \
    $(NULL)

CaeUnsGMesh_LIBS = \
    $(NULL)

CaeUnsGMesh_MAINT_TARGETS := \
    CaeUnsGMesh_info \
    CaeUnsGMesh_install \
    CaeUnsGMesh_installnow \
    CaeUnsGMesh_uninstall


########################################################################
# Get (OPTIONAL) locally defined make targets. If a plugin developer wants
# to extend a plugin's make scheme, they should create a modulelocal.mk file
# in the plugin's base folder. To provide for future SDK upgrades, the standard
# module.mk file should NOT be directly edited.
#
ifneq ($(wildcard $(CaeUnsGMesh_LOC)/modulelocal.mk),)
    CaeUnsGMesh_DEPS += $(CaeUnsGMesh_LOC)/modulelocal.mk

    include $(CaeUnsGMesh_LOC)/modulelocal.mk

    # merge in plugin private settings
    CaeUnsGMesh_OBJ += \
        $(patsubst %.cxx,$(CaeUnsGMesh_OBJ_LOC)/%.o,$(filter %.cxx,$(CaeUnsGMesh_CXXFILES_PRIVATE))) \
        $(NULL)

    CaeUnsGMesh_SRC += \
        $(patsubst %,$(CaeUnsGMesh_CXX_LOC)/%,$(CaeUnsGMesh_CXXFILES_PRIVATE)) \
        $(NULL)

    CaeUnsGMesh_INCL += $(CaeUnsGMesh_INCL_PRIVATE)
    CaeUnsGMesh_LIBS += $(CaeUnsGMesh_LIBS_PRIVATE)
    CaeUnsGMesh_CXXFLAGS += $(CaeUnsGMesh_CXXFLAGS_PRIVATE)
    CaeUnsGMesh_LDFLAGS += $(CaeUnsGMesh_LDFLAGS_PRIVATE)
    CaeUnsGMesh_MAINT_TARGETS += $(CaeUnsGMesh_MAINT_TARGETS_PRIVATE)
    CaeUnsGMesh_DEPS += $(CaeUnsGMesh_DEPS_PRIVATE)
endif

PLUGIN_MAINT_TARGETS += $(CaeUnsGMesh_MAINT_TARGETS)
PLUGIN_OBJ += $(CaeUnsGMesh_OBJ)

# add to plugin maint targets to the global .PHONY target
.PHONY: \
    $(CaeUnsGMesh_MAINT_TARGETS) \
    $(NULL)


########################################################################
# Set the final build macros
CaeUnsGMesh_CXXFLAGS += $(CXXFLAGS) $(PLUGINS_STDDEFS) $(CaeUnsGMesh_INCL) \
    -DPWGM_HIDE_STRUCTURED_API


ifeq ($(machine),macosx)
CaeUnsGMesh_LDFLAGS += -install_name "@rpath/Plugins/$(CaeUnsGMesh_FULLNAME)"
else
CaeUnsGMesh_LDFLAGS +=
endif


########################################################################
# list of plugin's build targets
#
CaeUnsGMesh: $(CaeUnsGMesh_FULLLIB)

$(CaeUnsGMesh_FULLLIB): $(CaeUnsGMesh_OBJ) $(CaeUnsGMesh_DEPS)
	@echo "***"
	@echo "*** $@"
	@echo "***"
	@mkdir -p $(dir $@)
	$(SHLIB_LD) $(ARCH_FLAGS) $(CaeUnsGMesh_LDFLAGS) -o $(CaeUnsGMesh_FULLLIB) $(CaeUnsGMesh_OBJ) $(CaeUnsGMesh_LIBS) $(SYS_LIBS)

CaeUnsGMesh_info:
	@echo ""
	@echo "--------------------------------------------------------------"
ifeq ($(machine),macosx)
	otool -L -arch all $(CaeUnsGMesh_FULLLIB)
	@echo ""
endif
	file $(CaeUnsGMesh_FULLLIB)
	@echo "--------------------------------------------------------------"
	@echo ""


########################################################################
# list of plugin's intermediate targets
#
$(CaeUnsGMesh_OBJ_LOC):
	mkdir -p $(CaeUnsGMesh_OBJ_LOC)

#.......................................................................
# build .d files for the plugin and each of the shared runtime sources
# the .d files will be placed in the plugins OBJ folder CaeUnsGMesh_OBJ_LOC
$(CaeUnsGMesh_OBJ_LOC)/%.d: $(CaeUnsGMesh_CXX_LOC)/%.cxx
	@echo "Updating dependencies $(CaeUnsGMesh_OBJ_LOC)/$*.d"
	@mkdir -p $(dir $@)
	@./depend.sh $(CaeUnsGMesh_OBJ_LOC) $(CaeUnsGMesh_CXXFLAGS) $< > $@

$(CaeUnsGMesh_OBJ_LOC)/%.d: $(PLUGINS_RT_PWPLOC)/%.cxx
	@echo "Updating dependencies $(CaeUnsGMesh_OBJ_LOC)/$*.d"
	@mkdir -p $(dir $@)
	@./depend.sh $(CaeUnsGMesh_OBJ_LOC) $(CaeUnsGMesh_CXXFLAGS) $< > $@

$(CaeUnsGMesh_OBJ_LOC)/%.d: $(PLUGINS_RT_PWGMLOC)/%.cxx
	@echo "Updating dependencies $(CaeUnsGMesh_OBJ_LOC)/$*.d"
	@mkdir -p $(dir $@)
	@./depend.sh $(CaeUnsGMesh_OBJ_LOC) $(CaeUnsGMesh_CXXFLAGS) $< > $@

$(CaeUnsGMesh_OBJ_LOC)/%.d: $(PLUGINS_RT_CAEPLOC)/%.cxx
	@echo "Updating dependencies $(CaeUnsGMesh_OBJ_LOC)/$*.d"
	@mkdir -p $(dir $@)
	@./depend.sh $(CaeUnsGMesh_OBJ_LOC) $(CaeUnsGMesh_CXXFLAGS) $< > $@

#.......................................................................
# build .o files for the plugin and each of the shared runtime sources.
# the .o files will be placed in the plugins OBJ folder CaeUnsGMesh_OBJ_LOC
$(CaeUnsGMesh_OBJ_LOC)/%.o: $(CaeUnsGMesh_CXX_LOC)/%.cxx
	@echo "***"
	@echo "*** $*"
	@echo "***"
	@mkdir -p $(dir $@)
	$(CXX) $(CaeUnsGMesh_CXXFLAGS) $(ARCH_FLAGS) -o $@ -c $<

$(CaeUnsGMesh_OBJ_LOC)/%.o: $(PLUGINS_RT_PWPLOC)/%.cxx
	@echo "***"
	@echo "*** $*"
	@echo "***"
	@mkdir -p $(dir $@)
	$(CXX) $(CaeUnsGMesh_CXXFLAGS) $(ARCH_FLAGS) -o $@ -c $<

$(CaeUnsGMesh_OBJ_LOC)/%.o: $(PLUGINS_RT_PWGMLOC)/%.cxx
	@echo "***"
	@echo "*** $*"
	@echo "***"
	@mkdir -p $(dir $@)
	$(CXX) $(CaeUnsGMesh_CXXFLAGS) $(ARCH_FLAGS) -o $@ -c $<

$(CaeUnsGMesh_OBJ_LOC)/%.o: $(PLUGINS_RT_CAEPLOC)/%.cxx
	@echo "***"
	@echo "*** $*"
	@echo "***"
	@mkdir -p $(dir $@)
	$(CXX) $(CaeUnsGMesh_CXXFLAGS) $(ARCH_FLAGS) -o $@ -c $<


########################################################################
# list of plugin's clean targets
#
CaeUnsGMesh_cleandep:
	-$(RMR) $(CaeUnsGMesh_OBJ_LOC)/*.d

CaeUnsGMesh_clean:
	-$(RMR) $(CaeUnsGMesh_OBJ_LOC)/*.{d,o}

CaeUnsGMesh_distclean: CaeUnsGMesh_clean
	-$(RMF) $(CaeUnsGMesh_FULLLIB) > /dev/null 2>&1

CaeUnsGMesh_clobber:
	-$(RMR) $(CaeUnsGMesh_OBJ_LOC)


########################################################################
# list of plugin's clean targets
#
CaeUnsGMesh_install: install_validate CaeUnsGMesh_installnow
	@echo "CaeUnsGMesh Installed to '$(PLUGIN_INSTALL_FULLPATH)'"

CaeUnsGMesh_installnow:
	-@$(CP) $(CaeUnsGMesh_FULLLIB) "$(PLUGIN_INSTALL_FULLPATH)/libCaeUnsGMesh.$(SHLIB_SUFFIX)"

CaeUnsGMesh_uninstall:
	@echo "CaeUnsGMesh Uninstalled from '$(PLUGIN_INSTALL_FULLPATH)'"
	-@$(RMF) "$(PLUGIN_INSTALL_FULLPATH)/libCaeUnsGMesh.$(SHLIB_SUFFIX)"
