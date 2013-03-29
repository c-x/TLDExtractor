/*
Put your licence disclaimer here.

Author : Cedric LE ROUX <cedric.lrx@gmail.com>
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "tldextractor.h"


/*
 * This file is a demonstrator on how to use the TLD Extractor lib.
 */

int main(int argc, char **argv)
{
	TLDNode *Tree;
	int ret, i;

	int NB_HOSTS = 14;
	char *hosts[] = {
		"www.google.com",
		"www.google.co.uk",
		"www.google.dyndns.org",
		"a.com",
		".com",
		"com",
		"www.erer",
		"www.siemens.om",
		"www.toto.om",
		"cuicui.paragliding.aero",
		"falseparagliding.aero",
		"www.lions.nakahara.kawasaki.jp",
		"pouetpouet.blogspot.co.uk",
		"wonderful.city.kawasaki.jp"
	};

	printf("\n\n");

	Tree = initializeTLDTree();
	if( Tree == NULL ) {
		printf("Oops! Something went wrong. Call 911.\n");
		return EXIT_FAILURE;
	}

	for(i=0; i<NB_HOSTS; i++)
	{
		ret = get_tld_pos(Tree, hosts[i]);
		printf("%s : %s\n", hosts[i], (ret == -1) ? "NOT FOUND" : (hosts[i]+ret));
	}

	printf("\n\n");

	return EXIT_SUCCESS;
}

