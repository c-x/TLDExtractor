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
	TLDNode *Tree = NULL; // Set it to NULL at startup or it will not work :)
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

	ret = initializeTLDTree(&Tree);
	if( ret == EXIT_FAILURE ) {
		printf("Oops! Something went wrong. Call 911.\n");
		return EXIT_FAILURE;
	}

	for(i=0; i<NB_HOSTS; i++)
	{
		ret = get_tld_pos(Tree, hosts[i]);
		printf("%s : %s\n", hosts[i], (ret == -1) ? "NOT FOUND" : (hosts[i]+ret));
	}

	printf("\n\n");

	/*
	// Chargement d'un fichier plat contenant 1 domaine par ligne
	FILE *fp;
	char *buff = calloc(1024, sizeof(char));
	fp = fopen("domains-uniq.log", "r");
	while( fgets(buff, 1024, fp) )
	{
		buff[strlen(buff)-1] = '\0';
		ret = get_tld_pos(Tree, buff);
		printf("%s : %s\n", buff, (ret == -1) ? "NOT FOUND" : (buff+ret));
	}
	fclose(fp);
	*/
	return EXIT_SUCCESS;
}

