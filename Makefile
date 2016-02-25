# ---------------------------------------------------------------------
# Linux Makefile for FreeAdhocUDF
# This file is part of FreeAdhocUDF.
# ---------------------------------------------------------------------
# FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
# Christoph Theuring / Peter Mandrella / Georg Horn
# <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
# based on FreeUDFLibC - copyright (c) 1999 Gregory Deatz
# changes by adhoc dataservice GmbH
# ---------------------------------------------------------------------
#
# ---------------------------------------------------------------------
# InterBase/FireBird Installation Directory
# uncommend to use /include and /lib form installed InterBase/FireBird
# and for make install copy FreeAdhocUDF.so to the $(DBDIR)/UDF/ directory
# ---------------------------------------------------------------------
#DBDIR=/opt/firebird
#DBDIR=/usr/local/firebird
#DBDIR=/opt/interbase
#DBDIR=/usr/local/interbase
DBDIR=/usr/lib/firebird/2.5
# ----------------------------------------------------------------------
# uncomment your choice of the DB-system
# for InterBase you can choose the newest version for all ib versions
# for FireBird 1.x you must choose firebird_1_5
# for FireBird 2.x you can choose the newest firebird 2.x version
# for FireBird 2.x amd64 you can chosse the newest firebird 2.x_64 version
# ----------------------------------------------------------------------
#DBSYSTEM=interbase_6_0_2
#DBSYSTEM=interbase_7_1
#DBSYSTEM=interbase_7_5_1
#DBSYSTEM=interbase_2007
#DBSYSTEM=interbase_2009
#DBSYSTEM=interbase_XE --- actual no XE for Linux
#DBSYSTEM=interbase_XE_amd64 --- actual no XE for Linux
#DBSYSTEM=firebird_1_5_6
#DBSYSTEM=firebird_2_0_5
#DBSYSTEM=firebird_2_0_5_amd64
#DBSYSTEM=firebird_2_1_3
#DBSYSTEM=firebird_2_1_3_amd64
#DBSYSTEM=firebird_2_5_0
DBSYSTEM=firebird_2_5_0_amd64
# ----------------------------------------------------------------------
# check if chosen DBSYSTEM can use icu / contains charset utf8
# ----------------------------------------------------------------------
# icu for interbase since interbase 2009
# icu for firebird since firebird 2.0 
ifeq (firebird, $(findstring firebird, $(DBSYSTEM)))
  ifneq (firebird_1_5, $(findstring firebird_1_5, $(DBSYSTEM)))
    ifeq (_amd64, $(findstring _amd64, $(DBSYSTEM)))
       ICU_INCLUDE=../linux/icu/include64
       ICU_LIB=../linux/icu/lib64
    else
       ICU_INCLUDE=../linux/icu/include32
       ICU_LIB=../linux/icu/lib32
    endif
  endif
else
  ifeq (interbase_2009, $(findstring interbase_2009, $(DBSYSTEM)))
       ICU_INCLUDE=../linux/icu/include32
       ICU_LIB=../linux/icu/lib32
  else
    ifeq (interbase_XE, $(findstring interbase_XE, $(DBSYSTEM)))
      ifeq (_amd64, $(findstring _amd64, $(DBSYSTEM)))
         ICU_INCLUDE=../linux/icu/include64
         ICU_LIB=../linux/icu/lib64
      else
         ICU_INCLUDE=../linux/icu/include32
         ICU_LIB=../linux/icu/lib32
      endif
    endif     
  endif  
endif
# ----------------------------------------------------------------------
# set the original library name
# ----------------------------------------------------------------------
ifeq (interbase, $(findstring interbase, $(DBSYSTEM)))
  FAU=FreeAdhocUDF_IB_i32.so
endif
ifeq (firebird_1_5, $(findstring firebird_1_5, $(DBSYSTEM)))
  FAU=FreeAdhocUDF_FB15_i32.so
endif
ifeq (firebird_2_, $(findstring firebird_2_, $(DBSYSTEM)))
  ifeq (_amd64, $(findstring _amd64, $(DBSYSTEM)))
    FAU=FreeAdhocUDF_FB2x_amd64.so
  else
    FAU=FreeAdhocUDF_FB2x_i32.so
  endif 
endif
# ----------------------------------------------------------------------
# Location of header files and libraries
# ----------------------------------------------------------------------
# set the directory for the associated libraries
ifeq (/, $(findstring /, $(DBDIR)))
  LIB=$(DBDIR)/lib
else
  LIB=../linux/$(DBSYSTEM)/lib
endif   
# set the directory for the associated header files
ifeq (/, $(findstring /, $(DBDIR)))
  INCLUDE=$(DBDIR)/include
else
  ifeq (_amd64, $(findstring _amd64, $(DBSYSTEM)))
     INCLUDE=../linux/$(subst _amd64,,$(DBSYSTEM))_amd64/include  
  else
     INCLUDE=../linux/$(DBSYSTEM)/include
  endif   
endif   

# ----------------------------------------------------------------------
# set compiler and linker options
# ----------------------------------------------------------------------
# set the associated clientlib to the DB-system
# gds for InterBase, fbclient for FireBird   
# ----------------------------------------------------------------------
ifeq (firebird, $(findstring firebird, $(DBSYSTEM)))		
  CLIENTLIB=fbclient
else 
  CLIENTLIB=gds
endif
# ----------------------------------------------------------------------
# potential set flag for a 32 bit FreeAdhocUDF compile on a 64 bit system
# ----------------------------------------------------------------------
# point to the 32bit libraries on a 64bit system
LIB32=/usr/lib32
ifeq (x86_64, $(findstring x86_64, $(shell uname -a)))
	SYSTEM='the system is 64bit'
  ifneq (_amd64, $(findstring _amd64, $(DBSYSTEM)))
    CF=-m32
    LF=-mi386linux -L$(LIB32)
	  FREEADHOCUDF='the FreeAdhocUDF is 32bit'
  else
    CF=-m64
    LF=-L/usr/lib/x86_64-linux-gnu
	  FREEADHOCUDF='the FreeAdhocUDF is 64bit'
  endif  
else
	SYSTEM='the system is 32bit'
  ifneq (_amd64, $(findstring _amd64, $(DBSYSTEM)))
	  FREEADHOCUDF='the FreeAdhocUDF is 32bit'
    CF=
    LF=    
  else
    $(error no 64bit compiling on a 32bit system!)
  endif
endif
# ---------------------------------------------------------------------
# Compiler and linker Defines
# ---------------------------------------------------------------------
CC=gcc
ifneq (icu, $(findstring icu, $(ICU_INCLUDE)))
  CFLAGS=-Wall -fpic -O -c $(CF) -I$(INCLUDE) -U_FORTIFY_SOURCE
  LDFLAGS=-shared $(LF) -L$(LIB) -lm -lc -lib_util -lpcre
else
  CFLAGS=-Wall -fpic -O -c $(CF) -I$(INCLUDE) -I$(ICU_INCLUDE) -U_FORTIFY_SOURCE
  LDFLAGS=-shared $(LF) -L$(LIB) -L$(ICU_LIB) -Wl,-rpath,$(ICU_LIB) \
    -lm -lc -lib_util -licudata -licui18n -licuuc -lpcre
endif  
# ---------------------------------------------------------------------
# Generic Compilation Rules
# ---------------------------------------------------------------------
.c.o:
	$(CC) $< $(CFLAGS) -o $@
# ---------------------------------------------------------------------
# The UDF objects
# ---------------------------------------------------------------------
UDF_OBJ = date_functions.o math_functions.o  string_functions.o  \
  blob_functions.o  md5.o  sysdep.o  uuid.o  uuid_functions.o  \
  const_functions.o conv_functions.o  diverse_functions.o \
  checksum_functions.o gto_functions.o pcrs.o
ifeq (icu, $(findstring icu, $(ICU_INCLUDE)))
  UDF_OBJs = $(UDF_OBJ) utf8_functions.o
else
  UDF_OBJs = $(UDF_OBJ)  
endif  

all: $(FAU)

# /usr/lib/x86_64-linux-gnu/libpcre.a
$(FAU): $(UDF_OBJs)
	$(CC) $(UDF_OBJs) -o $@ $(LDFLAGS) -l$(CLIENTLIB)
  
clean:
	rm -f *.o *.so *.bak *.~* core
	@echo ----------
	@echo clean done
	@echo ----------

new:	clean depend all
	@echo -------------------------------
	@echo chosen $(DBSYSTEM)
	@echo libs are located $(LIB)
	@echo headers are located $(INCLUDE)
ifeq (icu, $(findstring icu, $(ICU_INCLUDE)))
	@echo icu_headers are located $(ICU_INCLUDE)
	@echo icu_libs are located $(ICU_LIB)	
endif
	@echo clientlib is $(CLIENTLIB)
	@echo $(SYSTEM)
	@echo $(FREEADHOCUDF)
	@echo libname is $(FAU)
	@echo ------------------------------

install: all
	cp $(FAU) $(DBDIR)/UDF/FreeAdhocUDF.so

depend:
	cat Makefile | sed '/^# DO NOT DELETE/,$$d' > Makefile.tmp
	mv Makefile.tmp Makefile
	echo "# DO NOT DELETE THIS LINE - MAKE DEPEND DEPENDS ON IT" >> Makefile
	echo "" >> Makefile
	for i in *.c; do gcc $(CFLAGS) -E -MM $$i >> Makefile; done
	@echo -----------
	@echo depend done
	@echo -----------

# DO NOT DELETE THIS LINE - MAKE DEPEND DEPENDS ON IT

blob_functions.o: blob_functions.c global.h string_functions.h \
 blob_functions.h
checksum_functions.o: checksum_functions.c global.h md5.h \
 checksum_functions.h
const_functions.o: const_functions.c global.h const_functions.h
conv_functions.o: conv_functions.c global.h conv_functions.h \
 math_functions.h
date_functions.o: date_functions.c global.h math_functions.h \
 date_functions.h string_functions.h
diverse_functions.o: diverse_functions.c global.h date_functions.h \
 diverse_functions.h
file_functions.o: file_functions.c global.h blob_functions.h \
 file_functions.h
gto_functions.o: gto_functions.c global.h gto_functions.h pcrs.h
math_functions.o: math_functions.c global.h math_functions.h \
 date_functions.h
md5.o: md5.c md5.h
pcrs.o: pcrs.c pcrs.h
string_functions.o: string_functions.c global.h string_functions.h
sysdep.o: sysdep.c sysdep.h md5.h
test.o: test.c pcrs.h
utf8_functions.o: utf8_functions.c global.h blob_functions.h \
 utf8_functions.h
uuid.o: uuid.c sysdep.h md5.h uuid.h
uuid_functions.o: uuid_functions.c global.h sysdep.h md5.h uuid.h \
 date_functions.h uuid_functions.h
