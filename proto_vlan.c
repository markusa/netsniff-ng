/*
 * netsniff-ng - the packet sniffing beast
 * Copyright 2009, 2010 Daniel Borkmann.
 * Copyright 2010 Emmanuel Roullit.
 * Subject to the GPL, version 2.
 */

#include <stdio.h>
#include <stdint.h>
#include <netinet/in.h>    /* for ntohs() */

#include "proto.h"
#include "vlan.h"
#include "dissector_eth.h"
#include "pkt_buff.h"

struct vlanhdr {
	uint16_t h_vlan_TCI;
	uint16_t h_vlan_encapsulated_proto;
} __packed;

static void vlan(struct pkt_buff *pkt)
{
	uint16_t tci;
	struct vlanhdr *vlan = (struct vlanhdr *) pkt_pull(pkt, sizeof(*vlan));

	if (vlan == NULL)
		return;

	tci = ntohs(vlan->h_vlan_TCI);

	tprintf(" [ VLAN ");
	tprintf("Prio (%d), ", vlan_tci2prio(tci));
	tprintf("CFI (%d), ", vlan_tci2cfi(tci));
	tprintf("ID (%d), ", vlan_tci2vid(tci));
	tprintf("Proto (0x%.4x)", ntohs(vlan->h_vlan_encapsulated_proto));
	tprintf(" ]\n");

	pkt_set_dissector(pkt, &eth_lay2, ntohs(vlan->h_vlan_encapsulated_proto));
}

static void vlan_less(struct pkt_buff *pkt)
{
	uint16_t tci;
	struct vlanhdr *vlan = (struct vlanhdr *) pkt_pull(pkt, sizeof(*vlan));

	if (vlan == NULL)
		return;

	tci = ntohs(vlan->h_vlan_TCI);

	tprintf(" VLAN%d", (tci & 0x0FFF));

	pkt_set_dissector(pkt, &eth_lay2, ntohs(vlan->h_vlan_encapsulated_proto));
}

struct protocol vlan_ops = {
	.key = 0x8100,
	.print_full = vlan,
	.print_less = vlan_less,
};
