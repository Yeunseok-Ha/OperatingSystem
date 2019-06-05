#include <stdio.h>
#include "smalloc.h"

int 
main()
{
	void *p1, *p2, *p3, *p4 ;

	print_sm_containers() ;
	print_sm_uses();

	p1 = smalloc(2000) ; 
	printf("smalloc(2000)\n") ; 
	print_sm_containers() ;
	print_sm_uses();

	p2 = smalloc(2500) ; 
	printf("smalloc(2500)\n") ; 
	print_sm_containers() ;
	print_sm_uses();

	sfree(p1) ; 
	printf("sfree(%p)\n", p1) ; 
	print_sm_containers() ;
	print_sm_uses();

	p3 = smalloc(1000) ; 
	printf("smalloc(1000)\n") ; 
	print_sm_containers() ;
	print_sm_uses();

	p4 = smalloc(1000) ; 
	printf("smalloc(1000)\n") ; 
	print_sm_containers() ;
	print_sm_uses();
}
