#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "webcrawler.h"
#include "settings.h"
#include "sleep.h"
#include "get_time.h"


// Returns SUCCESS when no issue arose, FAILURE else. Define as a macro,
// in order to get nice debugging  data from PRINT_FILE_LOCATION().
#define checkMaxEntryNumberReached(result)														\
({																								\
	int return_code = SUCCESS;																	\
																								\
	if (result -> currentIndex >= result -> entryNumber)										\
	{																							\
		return_code = FAILURE;																	\
																								\
		if (result -> warningPrinted == 0)														\
		{																						\
			result -> warningPrinted = !REPEAT_WARNINGS;										\
			printf("\n>> Maximum number of entries (%d) reached!\n", result -> entryNumber);	\
			PRINT_FILE_LOCATION();																\
		}																						\
	}																							\
	return_code;																				\
})


CrawlingResult* initCrawlingResult(int entryNumber, int entryMaxLength)
{
	CrawlingResult *result = (CrawlingResult*) calloc(1, sizeof(CrawlingResult));

	*(int*) &(result -> entryNumber) = entryNumber;
	*(int*) &(result -> entryMaxLength) = entryMaxLength;
	// currentIndex left to 0.

	result -> entryArray = (char**) calloc(entryNumber, sizeof(char*));

	if (!result -> entryArray)
	{
		printf("\nOut of memory!\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < result -> entryNumber; ++i)
	{
		result -> entryArray[i] = (char*) calloc(entryMaxLength + 1, sizeof(char));

		if (!result -> entryArray[i])
		{
			printf("\nOut of memory!\n");
			exit(EXIT_FAILURE);
		}
	}

	return result;
}


void freeCrawlingResult(CrawlingResult **result_address)
{
	if (!result_address || !*result_address)
		return;

	if ((*result_address) -> entryArray)
	{
		for (int i = 0; i < (*result_address) -> entryNumber; ++i)
		{
			if ((*result_address) -> entryArray[i])
				free((*result_address) -> entryArray[i]);
		}
	}

	free((*result_address) -> entryArray);
	free(*result_address);
	*result_address = NULL;
}


void printCrawlingResult(const CrawlingResult *result)
{
	if (!result)
	{
		printf("\nNULL CrawlingResult.\n");
		return;
	}

	printf("\n>> Crawling result: entryNumber: %d, entryMaxLength: %d, currentIndex: %d (private)\n",
		result -> entryNumber, result -> entryMaxLength, result -> currentIndex);

	printf("Found entries:\n\n");

	for (int i = 0; i < result -> currentIndex; ++i)
	{
		printf("%s\n", result -> entryArray[i]);
	}
}


// Returns the index of the given string in the CrawlingResult entryArray
// if it exists, or 'currentIndex' otherwise.
inline int getStringIndex(const CrawlingResult *result, const char *string)
{
	int i = 0;

	while (i < result -> currentIndex && strcmp(result -> entryArray[i], string) != 0)
		++i;

	return i;
}


// Returns FAILURE on... failure, SUCCESS on an entry being
// successfully added, and NO_ISSUE when the entry already exists.
int addEntry(CrawlingResult *result, const char *entry)
{
	if (checkMaxEntryNumberReached(result) == FAILURE)
		return FAILURE;

	if (strlen(entry) > result -> entryMaxLength)
	{
		printf("\nMaximum entry length (%d) is too small! Entry: '%s'\n", result -> entryMaxLength, entry);
		return FAILURE;
	}

	int index = getStringIndex(result, entry);

	if (index == result -> currentIndex) // entry not existing yet, it can thus be added.
	{
		strncpy(result -> entryArray[result -> currentIndex], entry, result -> entryMaxLength);
		++*(int*) &(result -> currentIndex);
		return SUCCESS;
	}

	return NO_ISSUE;
}


// Search entries (e.g urls) in the given html string, and add it to 'found_entries'.
void searchEntries(CrawlingResult *found_entries, const PageContent *pageContent,
	const char *start_tag, const char *end_tag, const char *prefix, const char *sufix)
{
	if (VERBOSE_MODE >= 2)
		printf("\n-> Search for the tags: '%s' and '%s':\n\n", start_tag, end_tag);

	if (checkMaxEntryNumberReached(found_entries) == FAILURE)
		return;

	const char *content = pageContent -> content;
	char *new_entry_buffer = (char*) calloc(found_entries -> entryMaxLength, sizeof(char));

	const size_t start_tag_length = strlen(start_tag), end_tag_length = strlen(end_tag);
	size_t start_index = 0, content_length = pageContent -> length;

	while (start_index < content_length)
	{
		// No overflow possible here, since 'content' must have an ending '\0' char:
		if (strncmp(start_tag, content + start_index, start_tag_length) != 0)
		{
			++start_index;
			continue;
		}

		size_t end_index = start_index + start_tag_length, end_bound = content_length - end_tag_length;

		// Seeking the ending delimiting char:
		while (end_index <= end_bound && strncmp(end_tag, content + end_index, end_tag_length) != 0)
			++end_index;

		if (end_index > end_bound)
			printf("\nEnding tag '%s' not found!\n", end_tag);

		else // Adding the new found entry to the entry array!
		{
			size_t offset = start_index + start_tag_length;

			snprintf(new_entry_buffer, found_entries -> entryMaxLength, "%s%.*s%s",
				prefix, (int) (end_index - offset), content + offset, sufix);

			if (VERBOSE_MODE >= 1)
				printf("Found: %s\n", new_entry_buffer);

			if (addEntry(found_entries, new_entry_buffer) == FAILURE)
				goto end; // stopping the search here.
		}

		start_index = end_index + end_tag_length;
	}

	end:
		free(new_entry_buffer);
}


// Experimental: replacing every '\'' by a '\"'.
void cleanup(char *string)
{
	while (*string != '\0')
	{
		if (*string == '\'')
			*string = '\"';

		++string;
	}
}


// Crawls the web starting from the given url, and retrieve all urls in that page.
void crawl(void *curl_handle, const char *url, int max_url_number, int max_url_length, int max_webpages_tovisit)
{
	printf("\n-> Crawling starting from the webpage: '%s'\n\n", url);

	PageContent *pageContent = initPageContent();
	CrawlingResult *found_urls = initCrawlingResult(max_url_number, max_url_length);

	if (addEntry(found_urls, url) == FAILURE) // to be sure the starting url isn't parsed again.
	{
		printf("\nCould not add the starting url!\n");
		return;
	}

	for (int page_index = 0; page_index < found_urls -> currentIndex && page_index < max_webpages_tovisit; ++page_index)
	{
		const char *current_url = found_urls -> entryArray[page_index];

		double time_start = get_time();

		if (fillPageContent(curl_handle, pageContent, current_url) == FAILURE)
			continue; // wrong url.

		if (VERBOSE_MODE >= 2)
			printPageContent(pageContent);

		printf("\n# Searching the page (rank %d): '%s'\n\n", page_index, current_url);

		cleanup(pageContent -> content);

		// Should work for https too. Needs a cleanup pass in order to replace each \' with \".
		// Doesn't support yet relative links.
		searchEntries(found_urls, pageContent, "\"http", "\"", "http", "");

		if (checkMaxEntryNumberReached(found_urls) == FAILURE)
			break;

		printf("\nFalling asleep...\n");

		double time_waited = adaptive_sleep(QUERY_COOLDOWN, get_time() - time_start);

		printf("Waking up! (after %.3f s)\n", time_waited);
	}

	if (VERBOSE_MODE >= 1)
		printCrawlingResult(found_urls);

	freeCrawlingResult(&found_urls);
	freePageContent(&pageContent);
}
