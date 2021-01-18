#include <stdio.h>
#include <stdlib.h>

#include <curl/curl.h>

#include "webcrawler.h"


int main(void)
{
	////////////////////////////////////////////////////////////
	// Stating url choice:

	// const char the_url[] = "";
	const char the_url[] = "https://www.example.com/";
	// const char the_url[] = "https://ent-toulon.isen.fr/faces/Login.xhtml";
	// const char the_url[] = "http://www.esreality.com/";
	// const char the_url[] = "http://www.esreality.com/test"; // -> 404

	////////////////////////////////////////////////////////////
	// Initializing some libcurl resources:

	curl_global_init(CURL_GLOBAL_ALL);

	/* init the curl session */
	CURL *curl_handle = curl_easy_init();

	////////////////////////////////////////////////////////////
	// Printing a page content (html only):

	PageContent *pageContent = initPageContent();
	CrawlingResult *found_entries = initCrawlingResult(50);

	fillPageContent(curl_handle, pageContent, the_url);

	searchEntries(found_entries, pageContent, "<a href='https://www.twitch.tv/", ">", "", "");

	printCrawlingResult(found_entries);

	freeCrawlingResult(&found_entries);
	freePageContent(&pageContent);

	////////////////////////////////////////////////////////////
	// Crawling phase:

	// int max_url_number = 50;
	// int max_webpages_tovisit = 10;

	// crawl(curl_handle, the_url, max_url_number, max_webpages_tovisit);

	////////////////////////////////////////////////////////////
	// Cleaning:

	/* cleanup curl stuff */
	curl_easy_cleanup(curl_handle);

	/* we're done with libcurl, so clean it up */
	curl_global_cleanup();

	return 0;
}

// UTF8 topic:
// https://stackoverflow.com/questions/21216307/how-to-decode-an-uri-with-utf-8-characters-in-c

// Trailing '/' at url end: to be removed for avoiding redundencies!
