dnl -------------------------------------------------------------------------------------
dnl
dnl Macro: AC_CHECK_MYSQLR
dnl
dnl 1.    check for root of mysql distro
dnl 2.    check for mysql_config in root
dnl 2.1.  check for binary distro
dnl 2.2.  check for source distro
dnl 2.3.  autodetect of include/lib path based on root dir
dnl 3.    override detected values with customs include/libs
dnl 4.    for include/lib with blank values trying to run mysql_config for autodetection
dnl -------------------------------------------------------------------------------------

AC_DEFUN([AC_CHECK_MYSQLR],[

dnl Default values
inc_type="mysql_config"
lib_type="mysql_config"

dnl Check for custom MySQL root directory
if test [ x$1 != xyes -a x$1 != xno ] 
then
  ac_cv_mysql_root=`echo $1 | sed -e 's+/$++'`

  if test [ -d "$ac_cv_mysql_root/include" ] && \
     test [ -d "$ac_cv_mysql_root/libmysql_r" -o -d "$ac_cv_mysql_root/lib" ]
  then
    mysqlconfig=""
    if test [ -f "$ac_cv_mysql_root/bin/mysql_config" ]
     then
       dnl binary distro
       mysqlconfig="$ac_cv_mysql_root/bin/mysql_config"
     elif test [ -f "$ac_cv_mysql_root/scripts/mysql_config" ]
     then
       dnl source distro. disabled
       dnl mysqlconfig="$ac_cv_mysql_root/scripts/mysql_config"
       mysqlconfig=""
     fi

     if test [ -z "$mysqlconfig" ]
     then
       ac_cv_mysql_includes="$ac_cv_mysql_root/include"

       if test [ -d "$ac_cv_mysql_root/libmysql_r" ]
       then
         ac_cv_mysql_libs="$ac_cv_mysql_root/libmysql_r"
       elif test [ -d "$ac_cv_mysql_root/lib" ]
       then
         ac_cv_mysql_libs="$ac_cv_mysql_root/lib"
       fi
     fi
  else
    AC_MSG_ERROR([invalid MySQL root directory: $ac_cv_mysql_root])
  fi
fi

dnl Check for custom includes path
AC_ARG_WITH([mysql-includes], 
  [AC_HELP_STRING([--with-mysql-includes], 
    [path to MySQL header files])],
  [ac_cv_mysql_includes=$withval]
)

dnl Check for custom library path
AC_ARG_WITH([mysql-libs], 
  [AC_HELP_STRING([--with-mysql-libs], [path to MySQL libraries])],
  [ac_cv_mysql_libs=$withval]
)

if test [ -n "$ac_cv_mysql_includes" ]
then 
  MYSQL_CFLAGS="-I$ac_cv_mysql_includes"
  inc_type="custom"
fi

if test [ -n "$ac_cv_mysql_libs" ]
then
  dnl Trim trailing '.libs' if user passed it in --with-mysql-libs option
  ac_cv_mysql_libs=`echo ${ac_cv_mysql_libs} | sed -e 's/.libs$//' \
                     -e 's+.libs/$++'`
  MYSQL_LIBS="-L$ac_cv_mysql_libs -lmysqlclient_r"
  AC_CHECK_LIB(z,deflate)
  lib_type="custom"
fi


dnl If some path is missing, try to autodetermine with mysql_config
if test [ -z "$ac_cv_mysql_includes" -o -z "$ac_cv_mysql_libs" ]
then
    if test [ -z "$mysqlconfig" ]
    then 
      AC_PATH_PROG(mysqlconfig,mysql_config)
    fi
    if test [ -z "$mysqlconfig" ]
    then
       AC_MSG_ERROR([mysql_config executable not found
********************************************************************************
ERROR: cannot detect MySQL includes/libraries. If you want to compile with MySQL 
       support, you must either specify file locations explicitly using 
       --with-mysql-includes and --with-mysql-libs options, or make sure path to 
       mysql_config is listed in your PATH environment variable.
********************************************************************************
       ])
    else
      if test [ -z "$ac_cv_mysql_includes" ]
      then
        MYSQL_CFLAGS=`${mysqlconfig} --cflags| tr -d \'`
      fi
      if test [ -z "$ac_cv_mysql_libs" ]
      then
        MYSQL_LIBS=`${mysqlconfig} --libs | sed -e \
        's/-lmysqlclient /-lmysqlclient_r /' -e 's/-lmysqlclient$/-lmysqlclient_r/'`
      fi
    fi
fi

AC_MSG_CHECKING([MySQL C flags($inc_type)])
AC_MSG_RESULT($MYSQL_CFLAGS)

AC_MSG_CHECKING([MySQL linker flags($lib_type)])
AC_MSG_RESULT($MYSQL_LIBS)

])

  
