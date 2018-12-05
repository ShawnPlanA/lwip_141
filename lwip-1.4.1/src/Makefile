#
# (C) Copyright 2003
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk

LIB	= liblwip.a

CORE_OBJ := \
	core/def.o \
	core/dhcp.o \
	core/dns.o \
	core/stats.o \
	core/init.o \
	core/mem.o \
	core/memp.o \
	core/netif.o \
	core/pbuf.o \
	core/raw.o \
	core/sys.o \
	core/tcp.o \
	core/tcp_in.o \
	core/tcp_out.o \
	core/timers.o \
	core/udp.o \
	core/ipv4/autoip.o \
	core/ipv4/icmp.o \
	core/ipv4/igmp.o \
	core/ipv4/inet.o \
	core/ipv4/inet_chksum.o \
	core/ipv4/ip_addr.o \
	core/ipv4/ip.o \
	core/ipv4/ip_frag.o

API_OBJ := \
	api/api_lib.o \
	api/api_msg.o \
	api/err.o \
	api/netbuf.o \
	api/netdb.o \
	api/netifapi.o \
	api/sockets.o \
	api/tcpip.o


NETIF_OBJ := \
	netif/etharp.o \
	netif/ethernetif.o \
	netif/slipif.o

ARCH_OBJ := \
	arch/sys_arch.o \
	arch/perf.o 

OBJS := $(CORE_OBJ)  $(API_OBJ) $(NETIF_OBJ)  $(ARCH_OBJ)



#		-fno-zero-initialized-in-bss 
CFLAGS += \
		-I./include/ \
		-I./include/ipv4/ \
		-I./include/lwip/ \
		-I./include/netif/ \
		-I$(CONFIG_CROSS_COMPILER_PATH)/../include/ 
#	-I../include \
#	-I../include/linux \
#####################################################

all: $(LIB)

$(LIB):	$(OBJS)
	$(AR) crv $@ $(OBJS)

$(OBJS): %.o:%.c
	$(CC) $(CFLAGS) $< -c -o $@

