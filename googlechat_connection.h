/*
 * GoogleChat Plugin for libpurple/Pidgin
 * Copyright (c) 2015-2016 Eion Robb, Mike Ruprecht
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef _GOOGLECHAT_CONNECTION_H_
#define _GOOGLECHAT_CONNECTION_H_

#include <glib.h>

#include "http.h"

#include "libgooglechat.h"
#include "googlechat_pblite.h"
#include "googlechat.pb-c.h"

#define GOOGLECHAT_PBLITE_XORIGIN_URL "https://chat.google.com"
#define GOOGLECHAT_PBLITE_API_URL "https://chat.google.com"
#define GOOGLECHAT_CHANNEL_URL_PREFIX "https://chat.google.com/webchannel/"

void googlechat_process_data_chunks(GoogleChatAccount *ha, const gchar *data, gsize len);

void googlechat_process_channel(int fd);


void googlechat_longpoll_request(GoogleChatAccount *ha);
void googlechat_fetch_channel_sid(GoogleChatAccount *ha);
void googlechat_send_maps(GoogleChatAccount *ha, JsonArray *map_list, PurpleHttpCallback send_maps_callback);
void googlechat_add_channel_services(GoogleChatAccount *ha);

void googlechat_default_response_dump(GoogleChatAccount *ha, ProtobufCMessage *response, gpointer user_data);
gboolean googlechat_set_active_client(PurpleConnection *pc);
void googlechat_search_users(PurpleProtocolAction *action);
void googlechat_search_users_text(GoogleChatAccount *ha, const gchar *text);

typedef enum {
	GOOGLECHAT_CONTENT_TYPE_NONE = 0,
	GOOGLECHAT_CONTENT_TYPE_JSON,
	GOOGLECHAT_CONTENT_TYPE_PBLITE,
	GOOGLECHAT_CONTENT_TYPE_PROTOBUF
} GoogleChatContentType;
PurpleHttpConnection *googlechat_raw_request(GoogleChatAccount *ha, const gchar *path, GoogleChatContentType request_type, const gchar *request_data, gssize request_len, GoogleChatContentType response_type, PurpleHttpCallback callback, gpointer user_data);

typedef void(* GoogleChatApiResponseFunc)(GoogleChatAccount *ha, ProtobufCMessage *response, gpointer user_data);
void googlechat_api_request(GoogleChatAccount *ha, const gchar *endpoint, ProtobufCMessage *request, GoogleChatApiResponseFunc callback, ProtobufCMessage *response_message, gpointer user_data);


#define GOOGLECHAT_DEFINE_API_REQUEST_FUNC(name, type, url) \
typedef void(* GoogleChatApi##type##ResponseFunc)(GoogleChatAccount *ha, type##Response *response, gpointer user_data);\
static inline void \
googlechat_api_##name(GoogleChatAccount *ha, type##Request *request, GoogleChatApi##type##ResponseFunc callback, gpointer user_data)\
{\
	type##Response *response = g_new0(type##Response, 1);\
	\
	name##_response__init(response);\
	googlechat_api_request(ha, "/api/" url "?rt=b", (ProtobufCMessage *)request, (GoogleChatApiResponseFunc)callback, (ProtobufCMessage *)response, user_data);\
}

GOOGLECHAT_DEFINE_API_REQUEST_FUNC(create_topic, CreateTopic, "create_topic");
GOOGLECHAT_DEFINE_API_REQUEST_FUNC(get_user_status, GetUserStatus, "get_user_status");
GOOGLECHAT_DEFINE_API_REQUEST_FUNC(get_members, GetMembers, "get_members");
GOOGLECHAT_DEFINE_API_REQUEST_FUNC(set_typing_state, SetTypingState, "set_typing_state");
GOOGLECHAT_DEFINE_API_REQUEST_FUNC(list_topics, ListTopics, "list_topics");
GOOGLECHAT_DEFINE_API_REQUEST_FUNC(get_user_presence, GetUserPresence, "get_user_presence");

#endif /*_GOOGLECHAT_CONNECTION_H_*/