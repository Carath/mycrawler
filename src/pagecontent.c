// Modified from the example found at:
// https://curl.se/libcurl/c/getinmemory.html

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <malloc.h> // For the GNU function malloc_usable_size().

#include "pagecontent.h"
#include "settings.h"


PageContent* initPageContent(void)
{
	PageContent *pageContent = (PageContent*) calloc(1, sizeof(PageContent));

	pageContent -> content = (char*) calloc(1, sizeof(char));
	*(size_t*) &(pageContent -> size) = 1;
	// 'length' must be left to 0!

	return pageContent;
}


void freePageContent(PageContent **pageContentAddress)
{
	if (!pageContentAddress || !*pageContentAddress)
		return;

	free((*pageContentAddress) -> content);
	free(*pageContentAddress);
	*pageContentAddress = NULL;
}


void printPageContent(const PageContent *pageContent)
{
	if (!pageContent)
	{
		printf("\nNULL PageContent.\n");
		return;
	}

	printf("\n>> Page content: (length: %lu, using %ld bytes of memory)\n\n%s\n",
		pageContent -> length, pageContent -> size, pageContent -> content);
}


// Dynamically grows 'pageContent -> content' size if needed, and concatenate it with new data:
static size_t grow_buffer(void *new_content, size_t size, size_t nmemb, void *pageContent)
{
	PageContent *mem = (PageContent*) pageContent;
	size_t size_gain = size * nmemb, new_length = mem -> length + size_gain;

	if (ALWAYS_REALLOC || new_length >= mem -> size) // reallocating!
	{
		size_t new_size = new_length + 1;

		if (!ALWAYS_REALLOC)
		{
			size_t exponential_growth = SIZE_GROWTH_FACTOR * mem -> size;
			new_size = MAX(new_size, exponential_growth);
		}

		// Do not free the previous address nor copy its content, it will be done automatically if needed!
		char *new_address = realloc(mem -> content, new_size);

		if (!new_address)
		{
			printf("\nNot enough memory (realloc returned NULL).\n");
			return 0;
		}

		mem -> content = new_address;
		*(size_t*) &(mem -> size) = new_size;

		// size_t usableSize = malloc_usable_size(mem -> content);
		// printf("\n-> Usable size: %ld\n", usableSize);
	}

	memcpy(mem -> content + mem -> length, new_content, size_gain); // 'new_content' is appended.
	*(size_t*) &(mem -> length) = new_length;
	mem -> content[new_length] = '\0';

	return size_gain;
}


// Resetting 'pageContent', without freeing the allocated memory!
static void resetPageContent(PageContent *pageContent)
{
	if (ALWAYS_REALLOC)
		*(size_t*) &(pageContent -> size) = 1;

	*(size_t*) &(pageContent -> length) = 0;
	pageContent -> content[0] = '\0';
}


// Resets and fills 'pageContent' with the html found at the given url.
// Return values: SUCCESS/FAILURE. 'curl_handle' should be of type CURL*.
int fillPageContent(void *curl_handle, PageContent *pageContent, const char *url)
{
	if (!pageContent || !url)
	{
		printf("\nNULL pageContent or url.\n");
		return FAILURE;
	}

	printf("\n# Getting the content of the page: '%s'\n", url);

	int status = SUCCESS;

	resetPageContent(pageContent);

	/* specify URL to get */
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);

	/* send all data to this function	*/
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, grow_buffer);

	/* we pass our 'pageContent' struct to the callback function */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*) pageContent);

	/* some servers don't like requests that are made without a user-agent
		 field, so we provide one */
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	/* get it! */
	CURLcode res = curl_easy_perform(curl_handle);

	/* check for errors */
	if (res != CURLE_OK)
	{
		printf("\ncurl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		status = FAILURE;
	}
	else if (VERBOSE_MODE >= 1)
		printf("Found %ld bytes.\n", pageContent -> length);

	return status;
}
