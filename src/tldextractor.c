/*
Put your licence disclaimer here.

Author : Cedric LE ROUX <cedric.lrx@gmail.com>
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "tldextractor.h"
#include "tldlist.h"

int _allocateKid(TLDNode **Node, char c, bool EoT, bool move_cursor)
{
	if( (*Node)->kid != NULL )
		return EXIT_FAILURE;
	
	(*Node)->kid = calloc(1, sizeof(TLDNode));
	if( (*Node)->kid == NULL )
		return EXIT_FAILURE;

	((TLDNode *)(*Node)->kid)->c   = c;
	((TLDNode *)(*Node)->kid)->EoT = EoT;

	if( move_cursor )
		*Node = (TLDNode *)(*Node)->kid;
	return EXIT_SUCCESS;
}

int _allocateSibling(TLDNode **Node, char c, bool EoT, bool move_cursor)
{
	if( (*Node)->sibling != NULL )
		return EXIT_FAILURE;
	
	(*Node)->sibling = calloc(1, sizeof(TLDNode));
	if( (*Node)->sibling== NULL )
		return EXIT_FAILURE;

	((TLDNode *)(*Node)->sibling)->c   = c;
	((TLDNode *)(*Node)->sibling)->EoT = EoT;

	if( move_cursor )
		*Node = (TLDNode *)(*Node)->sibling;
	return EXIT_SUCCESS;
}
			
/* 
 * Add a node the the Trie (should be kid or sibling of the root)
 *
 */
int _addNode(TLDNode **Tree, char *TLD, int tLen)
{
	bool lastChar, nextIsDot, nextIsException;
	char *p;
	int ret;
	TLDNode *pNode  = *Tree;


	// Add the TLD to the Trie in reverse order
	p = TLD +tLen -1;
	while( *p )
	{
		lastChar        =  *(p-1)         ? false : true;
		nextIsDot       = (*(p-1) == '.') ? true  : false;
		nextIsException = (*(p-1) == '!') ? true  : false;

		// char does not exist in the Trie at that position
		if( pNode->kid == NULL ) 
		{
			ret = _allocateKid(&pNode, *p, lastChar | nextIsDot | nextIsException, true);
			if( ret == EXIT_FAILURE )
				return EXIT_FAILURE;
		}
		// char may exist in the Trie
		else 
		{
			pNode = pNode->kid;

			while( (pNode->sibling != NULL) && (pNode->c != *p) )
				pNode = pNode->sibling;

			// char does not exist
			if( pNode->c != *p )
			{
				ret = _allocateSibling(&pNode, *p, lastChar | nextIsDot | nextIsException, true);
				if( ret == EXIT_FAILURE )
					return EXIT_FAILURE;
			}
			// char already exist at that position but not necessary as an ended one.
			else if( lastChar || nextIsDot || nextIsException )
				pNode->EoT = true;
		}
		p--;
	}

	return EXIT_SUCCESS;
}


/*
 * Add a node to the Trie
 * Define whether it's an exception (!<domain.tld>) or a regular TLD (including wildcards ones)
 * Exception go under the Tree root's kid part, regular under the root's sibling.
 */
int TreeAddNode(TLDNode **Tree, char *TLD, int tLen)
{
	TLDNode *pNode;

	// regular tld
	if( *TLD != '!' ) 
	{
		// First node
		if( (*Tree)->kid == NULL ) 
		{
			(*Tree)->kid = calloc(1, sizeof(TLDNode));
			if( (*Tree)->kid == NULL )
				return EXIT_FAILURE;

			(*Tree)->kid->c = '\0'; 
		}
		pNode = (*Tree)->kid;
	}
	// exception
	else 
	{
		if( (*Tree)->sibling == NULL ) 
		{
			(*Tree)->sibling = calloc(1, sizeof(TLDNode));
			if( (*Tree)->sibling == NULL )
				return EXIT_FAILURE;

			(*Tree)->sibling->c = '\0'; 
		}
		pNode = (*Tree)->sibling;
	}

	return _addNode(&pNode, TLD, tLen);
}




/* void initializeTLDTree
 *
 * Load TLD from a char array and transform it as a 'reversed' Tree (= Trie).
 * Ex: '.com' is loaded as 'm->o->c->.'
 *
 * There is actually 2 Tree :
 * Tree->siblings contains TLD and wildcards (ex: *.om)
 * Tree->kids     contains exceptions TLD (ex: !siemens.om)
 *
 * See : 
 * - http://en.wikipedia.org/wiki/Trie
 * - http://en.wikipedia.org/wiki/Suffix_tree
 */

int initializeTLDTree(TLDNode **Tree)
{
	char **TLDArray = NULL; // Set it to NULL or it will not work
	int nbTLD, ret, i;

	if( *Tree != NULL )
		return EXIT_FAILURE;

	// Initialize the tree
	*Tree = calloc(1,sizeof(TLDNode));
	if( *Tree == NULL )
		return EXIT_FAILURE;
	(*Tree)->c = '\0'; 

	// Load the raw TLD array
	ret = initializeTLDArray(&TLDArray, &nbTLD);
	if( ret != EXIT_SUCCESS )
		return EXIT_FAILURE;

	for(i=0; i<nbTLD; i++)
	{
		ret = TreeAddNode(Tree, TLDArray[i], strlen(TLDArray[i]));

		if( ret != EXIT_SUCCESS )
			return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
		

/*
 * Return TRUE if the provided tld is found in the provided Tree.
 * or if the provided tld match a wildcard tld in the Tree.
 * FALSE in any other case (no match)
 *
 */
bool inTrie(TLDNode *Tree, char *tld)
{
	TLDNode *pNode = Tree;
	char *p;
	int tLen;
	bool wildcard;

	tLen = strlen(tld);
	p    = tld + tLen-1;
	while( tLen-- )
	{
		wildcard = false;
		pNode    = pNode->kid;

		while( pNode && (pNode->c != *p) )
		{
			if( pNode->c == '*' )
				wildcard = true;
			pNode = pNode->sibling;
		}
		if( ! pNode )
		{
			if( wildcard ) 
			{
				while( tLen-- )
				{
					if( tLen && (*(--p) == '.') )
						return false;
				}
				return true;
			}
			return false;
		}
		p--;
	}

	if( pNode->EoT )
		return true;
	return false;
}

/*
 * Return the starting position of the tld in host or -1 if not found
 * Require a TLD Tree.
 * Ex: 
 * - google.com => 7 (='com')
 * - abc.co.uk  => 4 (='co.uk')
 * - google.nawak => -1 (NOT FOUND)
 *
 */
int get_tld_pos(TLDNode *Tree, char *host)
{
	char *p, *last;
	bool found;
	int step;
 
	last = NULL;
	p    = host + strlen(host) -1;
	while( *p )
	{
		while( *(p-1) && (*p != '.') )
			p--;

		step = ( *p == '.' ) ? 1 : 0;

		found = inTrie(Tree->kid, p+step);
		if( ! found )
			break;
		last = p+step;
		p--;
	}

	if( last == NULL )
		return -1;

	// here we have the longest TLD
	// but is that an exception ? (ex: !siemens.om vs *.om)
	found = inTrie(Tree->sibling, last);

	if( found )
	{
		while( *(last) != '.' )
			last++;
		last++;
	}
	return (strlen(host) - strlen(last));
}

