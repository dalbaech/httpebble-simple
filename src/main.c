/* Copyright (c) 2013 Daniel Carll, http://www.randomlylost.com/software
 * *
 * *  Portions were created from other people's work and are Copyright
 * *  to their respective authors. There may be snippets of other people's code as
 * *  well, such as from
 * *
 * *  http.h,http.c: Copyright (C) 2013 Katharine Berry
 * *
 * *  The gist that was the codebase that started the code was written by Matthew Tole,
 * *  Copyright (C) 2013, Matthew Tole, https://gist.github.com/matthewtole
 * *
 * *
 * *  Permission is hereby granted, free of charge, to any person obtaining a copy of
 * *  this software and associated documentation files (the "Software"), to deal in
 * *  the Software without restriction, including without limitation the rights to
 * *  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * *  the Software, and to permit persons to whom the Software is furnished to do so,
 * *  subject to the following conditions:
 * *
 * *  The above copyright notice and this permission notice shall be included in all
 * *  copies or substantial portions of the Software.
 * *
 * *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * *  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * *  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * *  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE
 * */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "http.h"
#include <stdlib.h>
#include "resource_ids.auto.h"
#define MY_UUID HTTP_UUID

#define MY_KEY1 1
#define MY_KEY2 2

PBL_APP_INFO(MY_UUID, "External IP", "Daniel Carll", 1, 0,  RESOURCE_ID_IMAGE_MENU_ICON_BLACK, APP_INFO_STANDARD_APP);

int32_t cookie=0000000;

int32_t get_cookie(int32_t);
int32_t random_number();
void request_it(int32_t);

void add_layers();
void deinit(AppContextRef ctx);
void handle_init(AppContextRef ctx);
void httpebble_error(int error_code);
void http_success(int32_t request_id, int http_status, DictionaryIterator* received, void* context);
void http_failure(int32_t request_id, int http_status, void* context);
void reconnect(void* context);

void up_single_click_handler(ClickRecognizerRef recognizer, Window* window);
void select_single_click_handler(ClickRecognizerRef recognizer, Window* window);
void down_single_click_handler(ClickRecognizerRef recognizer, Window* window);
void click_config_provider(ClickConfig** config, Window* window);

Window window;
TextLayer layer_text1;
TextLayer layer_text2;
TextLayer layer_text3;
TextLayer layer_text4;
TextLayer layer_text5;


int32_t get_cookie(int32_t s)
{
	s=random_number(100000,9999999);
	cookie=s;
	return s;
}

int32_t random_number(int32_t min_num,int32_t max_num)
{
	int32_t result=0,low_num=0,hi_num=0;
	if(min_num<max_num)
		{
			low_num=min_num;
			hi_num=max_num+1; // this is done to include max_num in output.
		}
	else
		{
			low_num=max_num+1;// this is done to include max_num in output.
			hi_num=min_num;
		}
	srand(time_ms(NULL,NULL));
	result = (rand()%(hi_num-low_num))+low_num;
	return result;
}

void add_layers()
{
	text_layer_init(&layer_text1, GRect(0, 0, 144, 26));
	text_layer_set_text_color(&layer_text1, GColorBlack);
	text_layer_set_background_color(&layer_text1, GColorClear);
	text_layer_set_font(&layer_text1, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(&layer_text1, GTextAlignmentCenter);
	text_layer_set_overflow_mode(&layer_text1, GTextOverflowModeWordWrap);
	layer_add_child(&window.layer, &layer_text1.layer);
	text_layer_init(&layer_text2, GRect(0, 18, 144, 30));
	text_layer_set_text_color(&layer_text2, GColorBlack);
	text_layer_set_background_color(&layer_text2, GColorClear);
	text_layer_set_font(&layer_text2, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_overflow_mode(&layer_text2, GTextOverflowModeWordWrap);
	text_layer_set_text_alignment(&layer_text2, GTextAlignmentCenter);
	layer_add_child(&window.layer, &layer_text2.layer);
	text_layer_init(&layer_text3, GRect(0, 40, 144, 30));
	text_layer_set_text_color(&layer_text3, GColorBlack);
	text_layer_set_background_color(&layer_text3, GColorClear);
	text_layer_set_font(&layer_text3, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text_alignment(&layer_text3, GTextAlignmentCenter);
	text_layer_set_overflow_mode(&layer_text3, GTextOverflowModeWordWrap);
	layer_add_child(&window.layer, &layer_text3.layer);
	text_layer_init(&layer_text4, GRect(0, 64, 144, 75));
	text_layer_set_text_color(&layer_text4, GColorWhite);
	text_layer_set_background_color(&layer_text4, GColorBlack);
	text_layer_set_font(&layer_text4, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text_alignment(&layer_text4, GTextAlignmentLeft);
	text_layer_set_overflow_mode(&layer_text4, GTextOverflowModeWordWrap);
	layer_add_child(&window.layer, &layer_text4.layer);
	text_layer_set_text(&layer_text4, "This will listen for httpebble requests in the background. Refresh: UP (lighton), DOWN (lightoff), MIDDLE clears seen count.");
	text_layer_init(&layer_text5, GRect(0, 140, 144, 30));
	text_layer_set_text_color(&layer_text5, GColorBlack);
	text_layer_set_background_color(&layer_text5, GColorClear);
	text_layer_set_font(&layer_text5, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text_alignment(&layer_text5, GTextAlignmentCenter);
	text_layer_set_overflow_mode(&layer_text5, GTextOverflowModeWordWrap);
	layer_add_child(&window.layer, &layer_text5.layer);
}

void deinit(AppContextRef ctx)
{
	layer_remove_from_parent(&layer_text1.layer);
	layer_remove_from_parent(&layer_text2.layer);
	layer_remove_from_parent(&layer_text3.layer);
	layer_remove_from_parent(&layer_text4.layer);
	layer_remove_from_parent(&layer_text5.layer);
	light_enable(false);
}

void handle_init(AppContextRef ctx)
{
	resource_init_current_app(&APP_RESOURCES);
	http_register_callbacks((HTTPCallbacks)
	{
		.success = http_success,
		 .failure = http_failure,
		  .reconnect=reconnect
	}, (void*)ctx);
	window_init(&window, "External IP");
	window_stack_push(&window, true);
	window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);
	window_set_fullscreen(&window, true);
	add_layers();
	int32_t clear=0;
	request_it(clear);
}

void httpebble_error(int error_code)
{
	static char error_message[] = "UNKNOWN_HTTP_ERRROR_CODE_GENERATED";
	switch(error_code)
		{
			case HTTP_SEND_TIMEOUT:
				strcpy(error_message, "HTTP_SEND_TIMEOUT");
				break;
			case HTTP_SEND_REJECTED:
				strcpy(error_message, "HTTP_SEND_REJECTED");
				break;
			case HTTP_NOT_CONNECTED:
				strcpy(error_message, "HTTP_NOT_CONNECTED");
				break;
			case HTTP_BRIDGE_NOT_RUNNING:
				strcpy(error_message, "HTTP_BRIDGE_NOT_RUNNING");
				break;
			case HTTP_INVALID_ARGS:
				strcpy(error_message, "HTTP_INVALID_ARGS");
				break;
			case HTTP_BUSY:
				strcpy(error_message, "HTTP_BUSY");
				break;
			case HTTP_BUFFER_OVERFLOW:
				strcpy(error_message, "HTTP_BUFFER_OVERFLOW");
				break;
			case HTTP_ALREADY_RELEASED:
				strcpy(error_message, "HTTP_ALREADY_RELEASED");
				break;
			case HTTP_CALLBACK_ALREADY_REGISTERED:
				strcpy(error_message, "HTTP_CALLBACK_ALREADY_REGISTERED");
				break;
			case HTTP_CALLBACK_NOT_REGISTERED:
				strcpy(error_message, "HTTP_CALLBACK_NOT_REGISTERED");
				break;
			case HTTP_NOT_ENOUGH_STORAGE:
				strcpy(error_message, "HTTP_NOT_ENOUGH_STORAGE");
				break;
			case HTTP_INVALID_DICT_ARGS:
				strcpy(error_message, "HTTP_INVALID_DICT_ARGS");
				break;
			case HTTP_INTERNAL_INCONSISTENCY:
				strcpy(error_message, "HTTP_INTERNAL_INCONSISTENCY");
				break;
			case HTTP_INVALID_BRIDGE_RESPONSE:
				strcpy(error_message, "HTTP_INVALID_BRIDGE_RESPONSE");
				break;
			default:
			{
				strcpy(error_message, "HTTP_ERROR_UNKNOWN");
			}
		}
	text_layer_set_text(&layer_text1, error_message);
}

void http_success(int32_t request_id, int http_status, DictionaryIterator* received, void* context)
{
	char* printed="YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY";
	char* cookie2="XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
	if(request_id != cookie)
		{
			return;
		}
	text_layer_set_text(&layer_text1, "IP detected:");
	Tuple* tuple1 = dict_find(received, 0);
	Tuple* tuple2 = dict_find(received, 1);
	text_layer_set_text(&layer_text2, tuple1->value->cstring);
	snprintf(printed, 30, "Device seen: %s times", tuple2->value->cstring);
	snprintf(cookie2, 30, "HTTP COOKIE: %ld", cookie);
	text_layer_set_text(&layer_text3, printed);
	text_layer_set_text(&layer_text5, cookie2);
}

void http_failure(int32_t request_id, int http_status, void* context)
{
	httpebble_error(http_status >= 1000 ? http_status - 1000 : http_status);
}

void reconnect(void* context)
{
	int32_t clear=0;
	request_it(clear);
}

void request_it(int32_t clear)
{
	static char* url="http://mypeb.pw/pebip.php";
	DictionaryIterator* dict;
	HTTPResult  result = http_out_get(url, cookie, &dict);
	if(result != HTTP_OK)
		{
			httpebble_error(result);
			return;
		}
	dict_write_int32(dict, MY_KEY1, cookie);
	dict_write_int32(dict, MY_KEY2, clear);
	result = http_out_send();
	if(result != HTTP_OK)
		{
			httpebble_error(result);
			return;
		}
}

void up_single_click_handler(ClickRecognizerRef recognizer, Window* window)
{
	int32_t clear=0;
	request_it(clear);
	light_enable(true);
}

void select_single_click_handler(ClickRecognizerRef recognizer, Window* window)
{
	int32_t clear=1;
	request_it(clear);
}
void down_single_click_handler(ClickRecognizerRef recognizer, Window* window)
{
	int32_t clear=0;
	request_it(clear);
	light_enable(false);
}

void click_config_provider(ClickConfig** config, Window* window)
{
	config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
	config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;
	config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;
	config[BUTTON_ID_SELECT]->click.repeat_interval_ms = 100;
	config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
	config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
}

void pbl_main(void* params)
{
	get_cookie(cookie);
	PebbleAppHandlers handlers =
	{
		.init_handler = &handle_init,
		.deinit_handler = &deinit,
		.messaging_info = {
			.buffer_sizes = {
				.inbound = 124,
				.outbound = 256,
			}
		}
	};
	app_event_loop(params, &handlers);
}