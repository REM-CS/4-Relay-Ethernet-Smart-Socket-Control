/**
 * @file	httpUtil.c
 * @brief	HTTP Server Utilities	
 * @version 1.0
 * @date	2014/07/15
 * @par Revision
 *			2014/07/15 - 1.0 Release
 * @author	
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "httpUtil.h"

// ===== Control-system API key (hardcoded as requested) =====
#define CONTROL_SYSTEM_API_KEY "CHANGE_ME_TO_A_LONG_RANDOM_VALUE"

// Implemented in your application (control-system.c)
extern uint8_t control_system_http_handle(const char *endpoint, uint8_t *buf, uint16_t *len);

uint8_t http_get_cgi_handler(uint8_t * uri_name, uint8_t * buf, uint32_t * file_len)
{
	uint8_t ret = HTTP_OK;
	uint16_t len = 0;

	if(predefined_get_cgi_processor(uri_name, buf, &len))
	{
		;
	}
	else if(strcmp((const char *)uri_name, "example.cgi") == 0)
	{
		// To do
		;
	}
	else
	{
		// CGI file not found
		ret = HTTP_FAILED;
	}

	if(ret)	*file_len = len;
	return ret;
}

uint8_t http_post_cgi_handler(uint8_t * uri_name, st_http_request * p_http_request, uint8_t * buf, uint32_t * file_len)
{
	uint8_t ret = HTTP_OK;
	uint16_t len = 0;
	uint8_t val = 0;

	if(predefined_set_cgi_processor(uri_name, p_http_request->URI, buf, &len))
	{
		;
	}
	else if(strcmp((const char *)uri_name, "example.cgi") == 0)
	{
		// To do
		val = 1;
		len = sprintf((char *)buf, "%d", val);
	}
	else
	{
		// CGI file not found
		ret = HTTP_FAILED;
	}

	if(ret)	*file_len = len;
	return ret;
}

uint8_t predefined_get_cgi_processor(uint8_t *uri_name, uint8_t *buf, uint16_t *len)
{
    // uri_name is produced by get_http_uri_name():
    // - leading '/' removed (unless URI was "/")
    // - query string removed
    // Example: "<APIKEY>/relay1/on"

    if (!uri_name || !buf || !len) return 0;

    const char *u = (const char *)uri_name;


    // Must have at least "<key>/<endpoint>"
    const char *slash = strchr(u, '/');
    if (!slash || slash == u || *(slash + 1) == '\0')
        return 0; // not ours

    // Extract key segment
    size_t key_len = (size_t)(slash - u);
    if (strlen(CONTROL_SYSTEM_API_KEY) != key_len || strncmp(u, CONTROL_SYSTEM_API_KEY, key_len) != 0)
    {
        // Invalid API key -> do not execute anything
        *len = (uint16_t)sprintf((char *)buf, "401 Unauthorized\n");
        return 1; // handled (returns body; httpServer will still reply 200 for CGI)
    }

    // Endpoint after "<key>/"
    const char *endpoint = slash + 1;

    // Delegate actual command execution to application logic
    if (control_system_http_handle(endpoint, buf, len))
        return 1;

    // Not recognized by app handler
    return 0;
}

uint8_t predefined_set_cgi_processor(uint8_t *uri_name, uint8_t *uri, uint8_t *buf, uint16_t *len)
{
    (void)uri_name;
    (void)uri;

    // POST not supported for control-system endpoints (intentional)
    if (buf && len)
    {
        *len = (uint16_t)sprintf((char *)buf, "405 Method Not Allowed\n");
        return 1;
    }
    return 0;
}
