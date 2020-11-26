#ifndef PAGECONTENT_H
#define PAGECONTENT_H


typedef struct
{
	const size_t size; // *private* contains the current memory size of 'content'.
	const size_t length; // *public* contains the current length of 'content' seen as a string.
	char *content;
} PageContent;


PageContent* initPageContent(void);


void freePageContent(PageContent **pageContentAddress);


void printPageContent(const PageContent *pageContent);


// Resets and fills 'pageContent' with the html found at the given url.
// Return values: SUCCESS/FAILURE. 'curl_handle' should be of type CURL*.
int fillPageContent(void *curl_handle, PageContent *pageContent, const char *url);


#endif
