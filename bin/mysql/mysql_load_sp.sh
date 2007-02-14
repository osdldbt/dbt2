#!/bin/bash

usage() {

  if [ "$1" != "" ]; then
    echo ''
    echo "error: $1"
  fi

  echo ''
  echo 'usage: mysql_load_sp.sh [options]'
  echo 'options:'
  echo '       -d <database name>'
  echo '       -c <path to mysql client binary. (default: /usr/bin/mysql)>'
  echo '       -f <path to SPs>'
  echo '       -h <database host (default: localhost>'
  echo '       -s <database socket>'
  echo '       -u <database user'
  echo '       -p <database port>'
  echo '       -t <database port>'
  echo '       -v <verbose>'
  echo ''
  echo 'Example: sh mysql_load_sp.sh -d dbt2'
  echo ''
}

command_exec()
{
  if [ -n "$VERBOSE" ]; then
    echo "Executed command: $1"
  fi

  eval "$1"

  rc=$?
  if [ $rc -ne 0 ]; then
   echo "ERROR: rc=$rc"
   case $rc in
     127) echo "COMMAND NOT FOUND"
          ;;
       *) echo "SCRIPT INTERRUPTED"
          ;;
    esac
    exit 255
  fi
}

load_sp()
{
  PROCEDURES="delivery new_order new_order_2 order_status payment stock_level"

  for PROCEDURE in $PROCEDURES ; do

  echo "Load SP: $PROCEDURE"
  command_exec "$MYSQL < $PATH_SP/$PROCEDURE.sql"

  done
}

#DEFAULTs

VERBOSE=""
DB_PASSWORD=""
PATH_SP=""
DB_NAME=""

MYSQL="/usr/bin/mysql"
DB_HOST="localhost"
DB_SOCKET="/tmp/mysql.sock"
DB_USER=$USER
DB_PORT="3306"
PATH_SP=../../storedproc/mysql

while getopts "t:f:d:c:s:h:u:p:v" opt; do
        case $opt in
        p)
                DB_PORT=$OPTARG
                ;;
        d)
                DB_NAME=$OPTARG
                ;;
        c)
                MYSQL=$OPTARG
                ;;
        s)
                DB_SOCKET=$OPTARG
                ;;
        h)
                DB_HOST=$OPTARG
                ;;
        f)
                PATH_SP=$OPTARG
                ;;
        u)
                DB_USER=$OPTARG
                ;;
        p)
                DB_PASSWORD=$OPTARG
                ;;
        v)
                VERBOSE=1
                ;;
        ?)
                usage
                exit 1
                ;;
        esac
done

if [ "$DB_NAME" == "" ]; then
  usage "specify database name using -d #"
  exit 1
fi

if [ ! -d "$PATH_SP" ]; then 
  usage "Directory '$PATH_SP' not exists. Please specify 
       correct path to SPs using -f #"
  exit 1
fi

if [ ! -f "$MYSQL" ]; then
  usage "MySQL client binary '$MYSQL' not exists. 
       Please specify correct one using -c #"
  exit 1
fi

MYSQL_VER=`$MYSQL --version | sed -e "s/.* \([0-9]*\).*,.*/\1/"`

if [ $MYSQL_VER -lt 5 ]; then
  usage "In order to load stored procedures you have to use mysql client binary from MySQL 
       distribution 5.0 or higher. Please specify correct binary using -c #"
  exit 1
fi

if [ "$DB_PASSWORD" != "" ]; then
  MYSQL_ARGS="-p $DB_PASSWORD"
fi

MYSQL_ARGS="$MYSQL_ARGS $DB_NAME -h $DB_HOST -u $DB_USER --socket=$DB_SOCKET"
MYSQL="$MYSQL $MYSQL_ARGS"

echo ""
echo "Loading of DBT2 SPs located in $PATH_SP to database $DB_NAME."
echo ""

load_sp



