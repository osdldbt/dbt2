CC = gcc

SRCDIR = /usr/local/src/osdldbt/dbt2
INCLUDEDIR = $(SRCDIR)/include
SAPDBROOT = /opt/sapdb
ODBCDIR = $(SAPDBROOT)/interfaces/odbc
ODBCINCLUDEDIR = $(ODBCDIR)/incl
ODBCLIBDIR = $(ODBCDIR)/lib
DBTXNDIR = $(SRCDIR)/interfaces/odbc
COMMONDIR = $(SRCDIR)/common

DB_INTERFACE = ODBC
CFLAGS = -g -I$(INCLUDEDIR) -D$(DB_INTERFACE)

COMMONOBJS = $(COMMONDIR)/common.o
COMMONPRGS = $(COMMONDIR)/common.c $(INCLUDEDIR)/common.h
