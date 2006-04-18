AC_PREREQ(2.59)
AC_REVISION($Id: configure.ac 1368 2006-04-18 21:51:48Z markwkm $)

dnl Process this file with autoconf to produce a configure script.
AC_INIT(dbt2, 0.37, osdldbt-general@lists.sourceforge.net)
AM_INIT_AUTOMAKE
AH_TOP([
#ifndef __CONFIG_H__
#define __CONFIG_H__
])
AC_CONFIG_HEADERS(config.h)
AH_BOTTOM([
#endif
])
AC_CONFIG_SRCDIR(src/client.c)


dnl System type
AC_CANONICAL_BUILD
case $build in
*-*-solaris*)
  AC_DEFINE(SOLARIS, 1, [Build for solaris])
  AC_CONFIG_FILES([scripts/SunOS/get_os_info.sh],
      [chmod +x scripts/SunOS/get_os_info.sh])
  ;;
*-*-linux*)
  AC_DEFINE(LINUX, 1, [Build for Linux])
  AC_DEFINE(BROKEN_SEMAPHORES, 1, [Pthread semaphores are broken])
  AC_CONFIG_FILES([scripts/Linux/get_os_info.sh],
      [chmod +x scripts/Linux/get_os_info.sh])
  ;;
*-*-darwin*)
  AC_DEFINE(DARWIN, 1, [Build for DARWIN])
  AC_DEFINE(BROKEN_SEMAPHORES, 1, [Pthread semaphores are broken])
  ;;
*)
  AC_MSG_ERROR([*** Build system not supported])
  ;;
esac


dnl Checks for programs.
AC_PROG_AWK
AC_PROG_CC

dnl Replace below with this in future release AC_PROG_EGREP
AC_PATH_PROGS([EGREP], ['grep -E'], ,
    [$PATH:/usr/local/bin:/usr/bin:/usr/X11R6/bin])
if test -z "$EGREP"
then
  AC_MSG_ERROR([*** grep -E is required])
fi

AC_PATH_PROGS([VMSTAT], [vmstat], ,
    [$PATH:/usr/local/bin:/usr/bin:/usr/X11R6/bin])
if test -z "$VMSTAT"
then
  AC_MSG_WARN([*** vmstat not found some scripts may not be usable])
fi

AC_PATH_PROGS([IOSTAT], [iostat], ,
    [$PATH:/usr/local/bin:/usr/bin:/usr/X11R6/bin])
if test -z "$IOSTAT"
then
  AC_MSG_WARN([*** iostat not found some scripts may not be usable])
fi

AC_PATH_PROGS([GNUPLOT], [gnuplot], ,
    [$PATH:/usr/local/bin:/usr/bin:/usr/X11R6/bin])
if test -z "$GNUPLOT"
then
  AC_MSG_WARN([*** gnuplot not found some scripts may not be usable])
fi

AC_PATH_PROGS([SAR], [sar bsdsar], ,
             [$PATH:/usr/local/bin:/usr/bin])
if test -z "$SAR"
then
  AC_MSG_WARN([*** sar or bsdsar not found some scripts may not be usable])
fi

AC_PATH_PROGS([SYSCTL], [sysctl], ,
    [$PATH:/usr/local/bin:/usr/bin:/sbin:/bin])
if test -z "$SYSCTL"
then
  AC_MSG_WARN([*** sysctl not found some scripts may not be usable])
fi

dnl
dnl Thread Library (required)
dnl check for `pthread_create' in -lpthread
dnl
AC_CHECK_HEADER(semaphore.h, [], [
    AC_MSG_ERROR([could not locate sempahore headers)])
])
AC_CHECK_HEADER(pthread.h, [], [
    AC_MSG_ERROR([could not locate pthread headers)])
])
AC_CHECK_LIB(pthread, pthread_create)
if test ".`echo $LIBS | grep pthread`" != .; then
dnl
dnl check for `sem_init' in -lposix4
dnl
  AC_CHECK_LIB(posix4, sem_init)
else
  AC_CHECK_LIB(pthreads, pthread_create)
  if test ".`echo $LIBS | grep pthread`" != .; then
dnl
dnl check for `sem_init' in -lposix4
dnl
    AC_CHECK_LIB(posix4, sem_init)
  else
    AC_CHECK_LIB(c_r, pthread_create)
    if test ".`echo $LIBS | grep c_r`" == .; then
      threaded=no;
    fi
  fi
fi
if test "$threaded" == "no" ; then
  AC_MSG_ERROR([Multithread support not available.])
fi


dnl Checks for libraries.
dnl Replace `main' with a function in -lm:
AC_CHECK_LIB(m, main)

dnl search for socket libraries
AC_SEARCH_LIBS(socket, socket, [], AC_MSG_ERROR([socket required]))
AC_SEARCH_LIBS(gethostbyname, nsl, [], AC_MSG_ERROR([gethostbyname required]))

dnl I like warnings.
CFLAGS="-Wall $CFLAGS"

dnl Argument for GNU getopt
AC_ARG_WITH(getopt,
    [AC_HELP_STRING([--with-getopt=DIR],
        [Set this to GNU getopt's installation directory, or leave it unset if
            it is in compiler's default search paths.])],
    [default_getopt="$withval"],
    [default_getopt="default"]
)
if test "$default_getopt" != default; then
  CPPFLAGS="-I${default_getopt}/include $CPPFLAGS"
  CFLAGS="-I${default_getopt}/include $CFLAGS"
  LDFLAGS="-L${default_getopt}/lib -Wl,-R${default_getopt}/lib $LDFLAGS"
fi

dnl GNU Getopt
AC_CHECK_HEADER([getopt.h],[],AC_MSG_ERROR([GNU Getopt required]))
AC_SEARCH_LIBS(getopt_long, [getopt gnugetopt], [],
  AC_MSG_ERROR([getopt libs required])
)


dnl Configuration options for different databases, PostgreSQL is default.

dnl PostgreSQL
AC_ARG_WITH(postgresql,
  [AC_HELP_STRING([--with-postgresql=DIR],
      [Default on. Set to the path of the PostgreSQL's installation, or leave
          unset if the path is already in the compiler search path.])],
  [pgsql_path=$withval database_to_use="pgsql"]
)

dnl SAP DB
AC_ARG_WITH(sapdb,
  [AC_HELP_STRING([--with-sapdb], [using ODBC])],
  [database_to_use="sapdb"]
)

dnl MySQL
AC_ARG_WITH(mysql,
  [AC_HELP_STRING([--with-mysql=DIR],
  [Build C based version of dbt2 test. Set to the path of the MySQL's 
      installation, or leave unset if the path is already in the search 
      path])],
  [ ac_cv_use_mysql=$withval database_to_use="mysql"]
)

dnl ODBC
dnl AC_ARG_WITH(odbc,
dnl   [AC_HELP_STRING([--with-odbc], [Default off, unless using SAP DB.])],
dnl   [use_odbc="yes"]
dnl )

AC_ARG_ENABLE(odbc,
  [AC_HELP_STRING([--enable-odbc],
      [Use ODBC (default is no)]
      )], 
  [use_odbc="yes"], [use_odbc="no"]
)

AC_ARG_ENABLE(nonsp,
  [AC_HELP_STRING([--enable-nonsp],
      [Force to build pure C based version of dbt2 test (default is no)]
      )], 
  [NONSP="yes"], [NONSP="no"]
)

AC_ARG_ENABLE(debug_query,
  [AC_HELP_STRING([--enable-debug-query],
      [Turn on debug info for nonsp version of test dbt2 test (default is no)]
      )], 
  [CFLAGS="$CFLAGS -DDEBUG_QUERY"]
)

if test "$use_odbc" == "yes"; then
  AC_DEFINE(ODBC, 1, [using ODBC to SAPDB])

  AC_ARG_WITH([unixodbc],
    [AC_HELP_STRING([--with-unixodbc=DIR],
        [path to UnixODBC installation root])],
    [unixodbc_root=$withval]
  )

  AC_ARG_WITH([iodbc],
    [AC_HELP_STRING([--with-iodbc=DIR],
      [path to iODBC installation root])],
    [iodbc_root=$withval]
  )

  if test "$unixodbc_root" != ""; then 
    if test "$unixodbc_root" != "yes" -a "$unixodbc_root" != "no"; then 
      if test -d "$unixodbc_root/include"; then     
        CFLAGS="$CFLAGS -I$unixodbc_root/include"
      else
        AC_MSG_ERROR([Detection of ODBC components: Directory $unixodbc_root/include not exist])
      fi
      if test -d "$unixodbc_root/lib"; then     
        LDFLAGS="$LDFLAGS -L$unixodbc_root/lib"
      else
        AC_MSG_ERROR([Detection of ODBC components: Directory $unixodbc_root/lib not exist])
      fi
      LIBS="$LIBS -lodbc"
    else
      AC_CHECK_HEADERS([sql.h sqlext.h sqltypes.h], [],
        [AC_MSG_ERROR([unixODBC required])]
      )
      AC_SEARCH_LIBS(SQLDriverConnect, [odbc], [],
        AC_MSG_ERROR([unixODBC required])
      )
    fi        
  elif test "$iodbc_root" != ""; then 
    CPPFLAGS="$CPPFLAGS -DIODBC"

    if test "$iodbc_root" != "yes" -a "$iodbc_root" != "no"; then 

      if test -d "$iodbc_root/include"; then     
        CFLAGS="$CFLAGS -I$iodbc_root/include"
      else
        AC_MSG_ERROR([Detection of ODBC components: Directory $iodbc_root/include not exist])
      fi
      if test -d "$iodbc_root/lib"; then     
        LDFLAGS="$LDFLAGS -L$iodbc_root/lib"
      else
        AC_MSG_ERROR([Detection of ODBC components: Directory $iodbc_root/lib not exist])
      fi

      LIBS="$LIBS -liodbc"
    else
      AC_CHECK_HEADERS([isql.h isqlext.h isqltypes.h], [],
        [AC_MSG_ERROR([iODBC required])]
      )
      AC_SEARCH_LIBS(SQLDriverConnect, [iodbc], [],
        AC_MSG_ERROR([iODBC required])
      )
    fi        
  else
    AC_CHECK_HEADERS(sql.h sqlext.h sqltypes.h, [],
      [AC_MSG_ERROR([unixODBC or iODBC required])]
    )
    AC_SEARCH_LIBS(SQLDriverConnect, [odbc iodbc], [],
      AC_MSG_ERROR([unixODBC or iODBC required])
    )
  fi
fi

dnl Set up rules depending on what database is selected.
if test "$database_to_use" == "pgsql"; then
  if test "$use_odbc" == "yes"; then
    DBC_CORE="odbc"
    DBC_FUNCTIONS="odbc"
    SP_DIR=""
  else
    DBC_FUNCTIONS="libpq"
    DBC_CORE="libpq"
    SP_DIR="pgsql"
    AC_DEFINE(LIBPQ, 1, [using PostgreSQL])

    dnl check if the user has specified path for pgsql
    if test "$pgsql_path" != ""; then
      CPPFLAGS="-I${pgsql_path}/include -I${pgsql_path}/include/server -I${pgsql_path}/include/postgresql/server -I${pgsql_path}/include/pgsql/server $CPPFLAGS"
      CFLAGS="-I${pgsql_path}/include -I${pgsql_path}/include/server -I${pgsql_path}/include/postgresql/server -I${pgsql_path}/include/pgsql/server $CFLAGS"
      LDFLAGS="-L${pgsql_path}/lib -Wl,-R${pgsql_path}/lib $LDFLAGS"
    fi

    AC_CHECK_HEADERS(
        [${pgsql_path}/include/libpq-fe.h libpq-fe.h],
        [libpq_ok=yes; break], [libpq_ok=no]
    )
    if test "x$libpq_ok" == "xno"; then
        AC_MSG_ERROR([could not locate libpq-fe.h])
    fi

    AC_CHECK_HEADERS(
        [${pgsql_path}/include/server/funcapi.h ${pgsql_path}/include/postgresql/server/funcapi.h ${pgsql_path}/include/pgsql/server/funcapi.h server/funcapi.h postgresql/server/funcapi.h pgsql/server/funcapi.h],
        [funcapi_ok=yes; break],
        [funcapi_ok=no]
    )
    if test "x$funcapi_ok" == "xno"; then
        AC_MSG_ERROR([could not locate funcapi.h])
    fi

    AC_CHECK_HEADERS(
        [${pgsql_path}/include/server/executor/spi.h ${pgsql_path}/include/postgresql/server/executor/spi.h ${pgsql_path}/include/pgsql/server/executor/spi.h server/executor/spi.h postgresql/server/executor/spi.h pgsql/server/executor/spi.h],
        [spi_ok=yes; break],
        [spi_ok=no]
    )
    if test "x$spi_ok" == "xno"; then
        AC_MSG_ERROR([could not locate spi.h])
    fi

    AC_CHECK_HEADERS(
        [${pgsql_path}/include/server/executor/executor.h ${pgsql_path}/include/postgresql/server/executor/executor.h ${pgsql_path}/include/pgsql/server/executor/executor.h server/executor/executor.h postgresql/server/executor/executor.h pgsql/server/executor/executor.h],
        [executor_ok=yes; break],
        [executor_ok=no]
    )
    if test "x$spi_ok" == "xno"; then
        AC_MSG_ERROR([could not locate executor.h])
    fi

    AC_CHECK_HEADERS(
        [${pgsql_path}/include/server/fmgr.h ${pgsql_path}/include/postgresql/server/fmgr.h ${pgsql_path}/include/pgsql/server/fmgr.h server/fmgr.h postgresql/server/fmgr.h pgsql/server/fmgr.h],
        [fmgr_ok=yes; break],
        [fmgr_ok=no]
    )
    if test "x$fmgr_ok" == "xno"; then
        AC_MSG_ERROR([could not locate fmgr.h])
    fi

    AC_CHECK_HEADERS(
        [${pgsql_path}/include/server/postgres.h ${pgsql_path}/include/postgresql/server/postgres.h ${pgsql_path}/include/pgsql/server/postgres.h server/postgres.h postgresql/server/postgres.h pgsql/server/postgres.h],
        [postgres_ok=yes; break],
        [postgres_ok=no]
    )
    if test "x$postgres_ok" == "xno"; then
        AC_MSG_ERROR([could not locate postgres.h])
    fi

    AC_SEARCH_LIBS(PQexec, pq, [], AC_MSG_ERROR([libpq libs required]))
  fi

  dnl check for initdb
  AC_PATH_PROGS([INITDB], [initdb], ,
    [$PATH:$pgsql_path/bin:/bin:/usr/bin:/usr/local/bin]
  )
  if test -z "$INITDB" ; then
    AC_MSG_ERROR([*** initdb is required])
  fi

  dnl check for createdb
  AC_PATH_PROGS([CREATEDB], [createdb], ,
    [$PATH:$pgsql_path/bin:/bin:/usr/bin:/usr/local/bin]
  )
  if test -z "$CREATEDB" ; then
    AC_MSG_ERROR([*** createdb is required])
  fi

  dnl check for dropdb
  AC_PATH_PROGS([DROPDB], [dropdb], ,
    [$PATH:$pgsql_path/bin:/bin:/usr/bin:/usr/local/bin]
  )
  if test -z "$DROPDB" ; then
    AC_MSG_ERROR([*** dropdb is required])
  fi

  dnl check for pg_restore
  AC_PATH_PROGS([PGRESTORE], [pg_restore], ,
    [$PATH:$pgsql_path/bin:/bin:/usr/bin:/usr/local/bin]
  )
  if test -z "$PGRESTORE" ; then
    AC_MSG_ERROR([*** pg_restore is required])
  fi

  dnl check for createlang
  AC_PATH_PROGS([CREATELANG], [createlang], ,
    [$PATH:$pgsql_path/bin:/bin:/usr/bin:/usr/local/bin]
  )
  if test -z "$CREATELANG" ; then
    AC_MSG_ERROR([*** createlang is required])
  fi

  dnl check for pg_ctl
  AC_PATH_PROGS([PG_CTL], [pg_ctl], ,
    [$PATH:$pgsql_path/bin:/bin:/usr/bin:/usr/local/bin]
  )
  if test -z "$PG_CTL" ; then
    AC_MSG_ERROR([*** pg_ctl is required])
  fi

  dnl check for psql
  AC_PATH_PROGS([PSQL], [psql], ,
    [$PATH:$pgsql_path/bin:/bin:/usr/bin:/usr/local/bin]
  )
  if test -z "$PSQL" ; then
    AC_MSG_ERROR([*** psql is required])
  fi

  dnl check for vacuumdb
  AC_PATH_PROGS([VACUUMDB], [vacuumdb], ,
    [$PATH:$pgsql_path/bin:/bin:/usr/bin:/usr/local/bin]
  )
  if test -z "$VACUUMDB" ; then
    AC_MSG_ERROR([*** vacuumdb is required])
  fi

  dnl check for pg_autovacuum
  AC_PATH_PROGS([PG_AUTOVACUUM], [pg_autovacuum], ,
    [$PATH:$pgsql_path/bin:/bin:/usr/bin:/usr/local/bin]
  )
  if test -z "$PG_AUTOVACUUM" ; then
    AC_MSG_WARN([*** pg_autovacuum is recommended from 7.4 and 8.0])
  fi

  AC_CONFIG_FILES([scripts/pgsql/db_stat.sh],
      [chmod +x scripts/pgsql/db_stat.sh])
  AC_CONFIG_FILES([scripts/pgsql/pgsql_profile])
  AC_CONFIG_FILES([storedproc/pgsql/Makefile])
  AC_CONFIG_FILES([storedproc/pgsql/c/Makefile])
  AC_CONFIG_FILES([storedproc/pgsql/c/delivery.sql])
  AC_CONFIG_FILES([storedproc/pgsql/c/new_order.sql])
  AC_CONFIG_FILES([storedproc/pgsql/c/order_status.sql])
  AC_CONFIG_FILES([storedproc/pgsql/c/payment.sql])
  AC_CONFIG_FILES([storedproc/pgsql/c/stock_level.sql])
elif test "$database_to_use" == "sapdb"; then
  dnl ODBC Library (required)
  dnl check for `odbcinit' in -lodbc, -liodbc

  if test "$use_odbc" == "yes" -a -z "$NONSP"; then 
    AC_MSG_ERROR(["It is possible to build only nonSP version of test for ODBC mode. Please 
                   use --enable-nonsp option"])
  fi
  
  DBC_CORE="odbc"
  DBC_FUNCTIONS="odbc"

  SP_DIR=""
elif test "$database_to_use" == "mysql"; then
  AC_CHECK_MYSQL([$ac_cv_use_mysql], ["libmysql_r"])

  CFLAGS="$CFLAGS $MYSQL_CFLAGS"
  LIBS="$LIBS $MYSQL_LIBS" 

  if test -z "$NONSP" ; then
    if test $MYSQL_VER -ne 0 && test $MYSQL_VER -lt 50000 ; then
      AC_MSG_ERROR(["To build SP based test you should use includes and 
                   libraries at least from MySQL 5.0 but you use them from $MYSQL_VER.
                    It is possible to build nonSP version of test. Please use --enable-nonsp 
                    option"])
    fi 
    AC_DEFINE(MYSQL_SP, 1, [SP based version of test])
  fi

  if test "$use_odbc" == "yes"; then
    DBC_CORE="odbc"
    DBC_FUNCTIONS="odbc"
    SP_DIR=""
  else
    DBC_CORE="mysql"
    DBC_FUNCTIONS="mysql"
    SP_DIR=""        
    AC_DEFINE(LIBMYSQL, 1, [using MySQL])
  fi

  AC_CONFIG_FILES([scripts/run_mysql.sh], [chmod +x scripts/run_mysql.sh])
else
  AC_MSG_ERROR('Please choose DBMS. ./configure --with-(postgresql|sapdb|mysql)!')
fi

dnl Check for test type(SP/NONSP)
if test -n "$NONSP" && test "$database_to_use" != "pgsql"; then
  DBC_FUNCTIONS="nonsp"  
fi

AM_CONDITIONAL(NONSP, test -n "$NONSP" && test "$database_to_use" != "pgsql")

dnl Checks for header files.
AC_HEADER_STDC

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_TIME
AC_STRUCT_TM

dnl Checks for library functions.
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(gettimeofday socket)

dnl Large file support?
AC_SYS_LARGEFILE

AC_SUBST(DBC_CORE)
AC_SUBST(DBC_FUNCTIONS)

dnl stored procedure subdir
AC_SUBST(SP_DIR)
AC_SUBST(database_to_use)

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([storedproc/Makefile])
AC_CONFIG_FILES([scripts/sysstats.sh], [chmod +x scripts/sysstats.sh])
AC_CONFIG_FILES([scripts/transform_sar.sh], [chmod +x scripts/transform_sar.sh])
AC_CONFIG_FILES([scripts/run_workload.sh], [chmod +x scripts/run_workload.sh])

AC_OUTPUT
