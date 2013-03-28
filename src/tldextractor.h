/*
Put your licence disclaimer here.

Author : Cedric LE ROUX <cedric.lrx@gmail.com>
*/
#ifndef _H_FAUP_TLDINIT_
#define _H_FAUP_TLDINIT_

#define GENERIC_BUFF 2048

typedef struct TLDNode {
	char c;
	bool EoT;      // End of TLD marker : ".uk", ".co.uk" => same shared root (.uk)
	struct TLDNode *sibling;
	struct TLDNode *kid;  
} TLDNode;

int initializeTLDTree(TLDNode **);
int get_tld_pos(TLDNode *, char *);

#endif

