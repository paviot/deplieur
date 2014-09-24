# AUTHOR : Xavier Renault LIP6-SRC, 2005


# Library paths used in both FrameKit or normal modes :
ALL_VERSIONS_LIBS = \
		-L$(LIBPNET_LIBDIR) \
		-L$(LIBDDD_LIBDIR) \
		-L./

if FRAMEKIT_MODE

lib_LIBRARIES =	libcol2ordequivDDD_FrameKit.a

bin_PROGRAMS =	col2ordequivDDD_reference_FrameKit \
		extract_classes_FrameKit

AM_LDFLAGS =	$(ALL_VERSIONS_LIBS) \
		-L$(FRAMEKIT_ROOT)/TOOLS_C/COMMON/fk_data/ \
		-L$(FRAMEKIT_ROOT)/TOOLS_C/COMMON/fk_env/ \
		-L$(FRAMEKIT_ROOT)/TOOLS_C/COMMON/fk_user/ \
		-L$(FRAMEKIT_ROOT)/TOOLS_C/COMMON/tools/ 


LDADD =		libcol2ordequivDDD_FrameKit.a \
		-lDDD \
		-lPNet_FrameKit \
		-lFKData  \
		-lFKEnv \
		-lFKUser \
		-lTools

FK_INC =	-I$(FRAMEKIT_ROOT)/TOOLS_C/COMMON/interfaces/C \
		-I$(FRAMEKIT_ROOT)/TOOLS_C/COMMON/tools/interfaces 

FK_FLAGS =	-DFRAMEKIT_SUPPORT

else

lib_LIBRARIES = libcol2ordequivDDD.a 

bin_PROGRAMS =	col2ordequivDDD_reference

AM_LDFLAGS =	$(ALL_VERSIONS_LIBS)

LDADD =		libcol2ordequivDDD.a \
		-lPNet \
		-lDDD

FK_INC =	# Nothing

FK_FLAGS =	# Nothing

endif

if BOOST_MODE
BOOST_INC =	-I$(LIBBOOST_INCDIR)
else
BOOST_INC =
endif

CCFILES =	fk_buffer.cc \
		variable_order.cc \
		variable_order_same.cc \
		variable_order_reverse.cc \
		ddd_common.cc \
		ddd_unfold.cc \
		ddd_operation.cc \
		ddd_guard.cc \
		ddd_count.cc \
		ddd_compare_reference.cc \
		ddd_compare_add.cc \
		ddd_compare_firstopt.cc \
		ddd_compare_bubble.cc \
		ddd_syphon_reference.cc \
		ddd_syphon_add.cc \
		ddd_syphon_transitions.cc \
		ddd_syphon_firstopt.cc \
		ddd_syphon_bubble.cc \
		ddd_marked_orphan.cc \
		cpn_to_ddd.cc \
		ddd_to_cpn.cc \
		usage.cc

HHFILES =	fk_buffer.hh \
		variable_order.hh \
		variable_order_same.hh \
		variable_order_reverse.hh \
		ddd_common.hh \
		ddd_unfold.hh \
		ddd_operation.hh \
		ddd_guard.hh \
		ddd_count.hh \
		ddd_compare_reference.hh \
		ddd_compare_add.hh \
		ddd_compare_firstopt.hh \
		ddd_compare_bubble.hh \
		ddd_syphon_reference.hh \
		ddd_syphon_add.hh \
		ddd_syphon_transitions.hh \
		ddd_syphon_firstopt.hh \
		ddd_syphon_buuble.hh \
		ddd_marked_orphan.hh \
		cpn_to_ddd.hh \
		ddd_to_cpn.hh \
		usage.hh

# construction des librairies..
AM_CPPFLAGS =	-I$(LIBDDD_INCDIR) \
		-I$(LIBPNET_INCDIR) \
		$(BOOST_INC) \
		$(FK_INC) \
		$(FK_FLAGS) \
		-DVARIABLE_ORDER=$(VARIABLE_ORDER)

AM_CXXFLAGS =	-Wall

libcol2ordequivDDD_a_SOURCES = $(CCFILES) $(HHFILES)

libcol2ordequivDDD_FrameKit_a_SOURCES = $(CCFILES) $(HHFILES)


######################################
# les quatre premiers
# libcol2ordequivDDD_a_LIBS = \
# 	-lPNet \
# 	-lDDD

col2ordequivDDD_reference_SOURCES = \
	col2ordequivDDD.cc exec_reference.cc \
	$(HHFILES)

#col2ordequivDDD_reference_iter_SOURCES = \
#	col2ordequivDDD.cc exec_reference_iter.cc \
#	$(HHFILES)
#
#col2ordequivDDD_firstopt_SOURCES = \
#	col2ordequivDDD.cc exec_firstopt.cc \
#	$(HHFILES)
#
#col2ordequivDDD_firstopt_iter_SOURCES = \
#	col2ordequivDDD.cc exec_firstopt_iter.cc \
#	$(HHFILES)


########################################
# rajouter les lignes -lFramekit
# les quatres en bas
# libcol2ordequivDDD_FrameKit_a_LIBS = \
# 	-lPNet_FrameKit -lFKEnv -lFKData -lFKUser -lTools \
# 	-lDDD

col2ordequivDDD_reference_FrameKit_SOURCES = \
	col2ordequivDDD.cc exec_reference.cc \
	$(HHFILES)
#col2ordequivDDD_reference_iter_FrameKit_SOURCES = \
#	col2ordequivDDD.cc exec_reference_iter.cc \
#	$(HHFILES)
#col2ordequivDDD_firstopt_FrameKit_SOURCES = \
#	col2ordequivDDD.cc exec_firstopt.cc \
#	$(HHFILES)
#col2ordequivDDD_firstopt_iter_FrameKit_SOURCES = \
#	col2ordequivDDD.cc exec_firstopt_iter.cc \
#	$(HHFILES)
extract_classes_FrameKit_SOURCES = \
	extract_classes.cc
