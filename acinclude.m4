dnl -------------------------------------------------------------------------------------
dnl
dnl Macro: AC_CHECK_MYSQL [ param1. param2 ] 
dnl
dnl param1 - value of --with-mysql option
dnl param2 - type of library. Possible values (libmysql|libmysql_r) 
dnl
dnl Return values:
dnl               $MYSQL_CFLAGS 
dnl               $MYSQL_LIBS
dnl               $MYSQL_VER           
dnl
dnl Example of usage:
dnl
dnl configure.in
dnl   ...
dnl   # MySQL
dnl   AC_ARG_WITH(mysql,
dnl     [AC_HELP_STRING([--with-mysql=DIR],
dnl       [Build C based version of dbt2 test. Set to the path of the MySQL's
dnl        installation, or leave unset if the path is already in the search
dnl        path])],
dnl     [ac_cv_use_mysql=$withval],
dnl     [],
dnl   )
dnl   ...
dnl   AC_CHECK_MYSQL([$ac_cv_use_mysql],["libmysql_r"])
dnl   CFLAGS="$CFLAGS $MYSQL_CFLAGS"
dnl   LIBS="$LIBS $MYSQL_LIBS"
dnl   ...
dnl 
dnl -------------------------------------------------------------------------------------

AC_DEFUN([AC_CHECK_MYSQL],[

if test [ "$2" == "libmysql_r" ]
then
  MYSQL_LIBS_DIR="libmysql_r"
  MYSQL_LIBS_TYPE="mysqlclient_r"
else
  MYSQL_LIBS_DIR="lib"
  MYSQL_LIBS_TYPE="mysqlclient"
fi

dnl Default values
inc_type="mysql_config"
lib_type="mysql_config"

dnl Check for custom MySQL root directory
if test [ x$1 != xyes -a x$1 != xno ] 
then
  ac_cv_mysql_root=`echo $1 | sed -e 's+/$++'`

  if test [ -d "$ac_cv_mysql_root/include" ] && \
     test [ -d "$ac_cv_mysql_root/$MYSQL_LIBS_DIR" ]
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

       if test [ -d "$ac_cv_mysql_root/$MYSQL_LIBS_DIR" ]
       then
         ac_cv_mysql_libs="$ac_cv_mysql_root/$MYSQL_LIBS_DIR"
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

  MYSQL_LIBS="-L$ac_cv_mysql_libs -l$MYSQL_LIBS_TYPE"
  AC_CHECK_LIB(z,deflate)
  lib_type="custom"
fi

dnl If some path is missing, try to autodetermine with mysql_config
if test [ -z "$ac_cv_mysql_includes" -o -z "$ac_cv_mysql_libs" ]
then

    if test [ -z "$mysqlconfig" ] 
    then 
      dnl Check for custom path for mysql_config
      AC_ARG_WITH([mysql-config],
      [AC_HELP_STRING([--with-mysql-config], [path to mysql-config])],
          [ if test [ "$withval" != "yes" -a "$withval" != "no" ]
            then 
              if test [ -x "$withval" ]
              then
                mysqlconfig="$withval"
              else
                AC_MSG_ERROR([$withval not exist
**********************************************************************************
ERROR: Please check that path $withval is correct and that mysql_config executable exist
**********************************************************************************
                ])
              fi
            fi
          ]
      )
    fi
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
        res=`${mysqlconfig} --libs_r|grep -c 'mysqlclient_r'`
        if test [ "$MYSQL_LIBS_TYPE" == "mysqlclient_r" -a "$res" == "1" ]
        then
           MYSQL_LIBS=`${mysqlconfig} --libs_r`
        else
           MYSQL_LIBS=`${mysqlconfig} --libs | sed -e \
                      "s/-lmysqlclient /-l$MYSQL_LIBS_TYPE /" -e \
                      "s/-lmysqlclient$/-l$MYSQL_LIBS_TYPE/"`
        fi
      fi
    fi
fi


AC_MSG_CHECKING(Version of MySQL headers)

MYSQL_VER=`(echo '#include <mysql_version.h>'; echo MYSQL_VERSION_ID ) \
    | cpp -P $MYSQL_CFLAGS`

AC_MSG_RESULT($MYSQL_VER)

if test $MYSQL_VER -eq 0 ; then
  AC_MSG_WARN([Unable to detect version of headers. Please check that include
                 directory was correctly specifed.])
fi

AC_MSG_CHECKING([MySQL C flags($inc_type)])
AC_MSG_RESULT($MYSQL_CFLAGS)

AC_MSG_CHECKING([MySQL linker flags($lib_type)])
AC_MSG_RESULT($MYSQL_LIBS)

])

  
