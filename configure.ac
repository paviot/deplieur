
	# AUTHOR : Xavier Renault LIP6-SRC, 2005

AC_PREREQ(2.59)

AC_INIT(	col2ordequivDDD,
		1.0,
		alban.linard@lip6.fr)

AC_CONFIG_SRCDIR([col2ordequivDDD.cc])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([sys/time.h sys/resource.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

################

# Check for FrameKit mode :
AC_ARG_WITH(	[framekit],
		AS_HELP_STRING(	[--with-framekit],
				[compile library and tools \
with FrameKit support, give absolute path to FrameKit installation]),
		[AC_SUBST(	[FRAMEKIT_ROOT],
				[$withval])],
		[make_framekit=false])

AM_CONDITIONAL(	FRAMEKIT_MODE,
		test "$make_framekit" != "false")

# Check for Boost include directory :
AC_ARG_WITH(	[boost-incdir],
		AS_HELP_STRING(	[--with-boost-incdir],
				[path to Boost includes, if \
not installed in default search path]),
		[AC_SUBST(	[LIBBOOST_INCDIR],
				[$withval])],
		[use_boost=false])

AM_CONDITIONAL(	BOOST_MODE,
		test "$use_boost" != "false")

# Use libDDD include path :
AC_ARG_WITH(	[libddd-incdir],
		AS_HELP_STRING(	[--with-libddd-incdir],
				[include path for libDDD]),
		[AC_SUBST(	[LIBDDD_INCDIR],
				[$withval])],
		[AC_MSG_ERROR(	[Please use --with-libddd-incdir to \
set path to libDDD include files.])])

# Use libDDD library path :
AC_ARG_WITH(    [libddd-libdir],
		AS_HELP_STRING(	[--with-libddd-libdir],
				[path to libDDD libraries]),
		[AC_SUBST(	[LIBDDD_LIBDIR],
				[$withval])],
		[AC_MSG_ERROR(	[Plese use --with-libddd-libdir \
to set path to libDDD libraries.])])

# Use libPetri include path :
AC_ARG_WITH(	[libpnet-incdir],
		AS_HELP_STRING(	[--with-libpnet-incdir],
				[include path for libPNet]),
		[AC_SUBST(	[LIBPNET_INCDIR],
				[$withval])],
		[AC_MSG_ERROR(	[Please use --with-libpnet-incdir \
to set path to libPNet include files.])])

# Use libPetri library path :
AC_ARG_WITH(    [libpnet-libdir],
		AS_HELP_STRING(	[--with-libpnet-libdir],
				[path to libPNet libraries]),
		[AC_SUBST(	[LIBPNET_LIBDIR],
				[$withval])],
		[AC_MSG_ERROR(	[Please use --with-libpnet-libdir \
to set path to libPNet libraries.])])

# Variable order :
#AC_ARG_WITH(	[variable-order],
#		AS_HELP_STRING( [--with-variable-order],
#				[type of order : \
#  variable_order_same_t \
#  variable_order_reverse_t]),
#		[AC_SUBST(	[VARIABLE_ORDER],
#				[$withval])],
#		[AC_SUBST(	[VARIABLE_ORDER],
#				[variable_order_same_t])])

AC_SUBST([VARIABLE_ORDER],
	       [variable_order_same_t])

AC_PROG_RANLIB

###
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
