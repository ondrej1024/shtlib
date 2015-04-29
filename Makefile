# ;
# Makefile:
###############################################################################
#
#  SHT Temperature & Humidity Sensor library for use on 
#  the RaspberryPi Single Board Computer
#
###############################################################################

DYN_VERS_MAJ=0
DYN_VERS_MIN=1

VERSION=$(DYN_VERS_MAJ).$(DYN_VERS_MIN)
DESTDIR=/usr
PREFIX=/local

STATIC=libsht.a
DYNAMIC=libsht.so.$(VERSION)

#DEBUG	= -g -O0
DEBUG	= -O2
CC	= gcc
INCLUDE	= -I.
DEFS	= -D_GNU_SOURCE
CFLAGS	= $(DEBUG) $(DEFS) -Wformat=2 -Wall -Winline $(INCLUDE) -pipe -fPIC
#CFLAGS  = -mtune=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard -marm -O3

LIBS    =

# Should not alter anything below this line
###############################################################################

SRC	=	bcm2835.c i2c.c sht21.c

OBJ	=	$(SRC:.c=.o)

all:		$(DYNAMIC)

static:		$(STATIC)

$(STATIC):	$(OBJ)
	@echo "[Link (Static)]"
	@ar rcs $(STATIC) $(OBJ)
	@ranlib $(STATIC)
#	@size   $(STATIC)

$(DYNAMIC):	$(OBJ)
	@echo "[Link (Dynamic)]"
	@$(CC) -shared -Wl,-soname,libsht.so -o libsht.so.$(VERSION) -lrt $(OBJ)

.c.o:
	@echo [Compile] $<
	@$(CC) -c $(CFLAGS) $< -o $@

.PHONEY:	clean
clean:
	@echo "[Clean]"
	@rm -f $(OBJ) $(OBJ_I2C) *~ core tags Makefile.bak libsht.*

.PHONEY:	tags
tags:	$(SRC)
	@echo [ctags]
	@ctags $(SRC)


.PHONEY:	install-headers
install-headers:
	@echo "[Install Headers]"
	@install -m 0755 -d		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 sht21.h	$(DESTDIR)$(PREFIX)/include

.PHONEY:	install
install:	$(DYNAMIC) install-headers
	@echo "[Install Dynamic Lib]"
	@install -m 0755 -d					$(DESTDIR)$(PREFIX)/lib
	@install -m 0755 libsht.so.$(VERSION)			$(DESTDIR)$(PREFIX)/lib/libsht.so.$(VERSION)
	@ln -sf $(DESTDIR)$(PREFIX)/lib/libsht.so.$(VERSION)	$(DESTDIR)/lib/libsht.so
	@ldconfig

.PHONEY:	install-static
install-static:	$(STATIC) install-headers
	@echo "[Install Static Lib]"
	@install -m 0755 -d		$(DESTDIR)$(PREFIX)/lib
	@install -m 0755 libsht.a	$(DESTDIR)$(PREFIX)/lib

.PHONEY:	uninstall
uninstall:
	@echo "[UnInstall]"
	@rm -f $(DESTDIR)$(PREFIX)/include/sht21.h
	@rm -f $(DESTDIR)$(PREFIX)/lib/libsht.*
	@ldconfig


# DO NOT DELETE

sht.o: sht21.h
 
