#ifndef WEBCRAWLER_H
#define WEBCRAWLER_H

#include "pagecontent.h"


typedef struct
{
	const int entryNumber;
	const int currentIndex;
	int warningPrinted;
	char **entryArray;
} CrawlingResult;


CrawlingResult* initCrawlingResult(int entryNumber);


void freeCrawlingResult(CrawlingResult **result_address);


void printCrawlingResult(const CrawlingResult *result);


// Returns the index of the given string in the CrawlingResult entryArray
// if it exists, or 'currentIndex' otherwise.
int getStringIndex(const CrawlingResult *result, const char *string);


// Search entries (e.g urls) in the given html string, and add it to 'found_entries'.
void searchEntries(CrawlingResult *found_entries, const PageContent *pageContent,
	const char *start_tag, const char *end_tag, const char *prefix, const char *suffix);


// Crawls the web starting from the given url, and retrieve all urls in that page.
void crawl(void *curl_handle, const char *url, int max_url_number, int max_webpages_tovisit);


#endif
