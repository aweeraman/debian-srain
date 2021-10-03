/* Copyright (C) 2016-2017 Shengyu Zhang <i@silverrainz.me>
 *
 * This file is part of Srain.
 *
 * Srain is free software: you can redistribute it and/or modify
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


#include "core/core.h"

#include "log.h"
#include "utils.h"

static void srn_server_user_update_chat_user(SrnServerUser *self);

SrnServerUser *srn_server_user_new(SrnServer *srv, const char *nick){
    SrnServerUser *self;

    g_return_val_if_fail(nick, NULL);

    self = g_malloc0(sizeof(SrnServerUser));
    self->srv = srv;
    self->is_ignored = FALSE;
    str_assign(&self->nick, nick);
    self->extra_data = srn_extra_data_new();

    return self;
}

void srn_server_user_free(SrnServerUser *self){
    g_return_if_fail(g_list_length(self->chat_user_list) == 0);

    str_assign(&self->nick, NULL);
    str_assign(&self->username, NULL);
    str_assign(&self->hostname, NULL);
    str_assign(&self->realname, NULL);
    srn_extra_data_free(self->extra_data);
    g_free(self);
}

SrnRet srn_server_user_attach_chat_user(SrnServerUser *self, SrnChatUser *chat_user){
    GList *lst;

    lst = self->chat_user_list;
    while (lst){
        SrnChatUser *chat_user2;

        chat_user2 = lst->data;
        if (chat_user2->chat == chat_user->chat){
            return SRN_ERR;
        }
        lst = g_list_next(lst);
    }
    self->chat_user_list = g_list_append(self->chat_user_list, chat_user);

    return SRN_OK;
}

SrnRet srn_server_user_detach_chat_user(SrnServerUser *self, SrnChatUser *chat_user){
    GList *lst;

    lst = g_list_find(self->chat_user_list, chat_user);
    if (lst){
        self->chat_user_list = g_list_delete_link(self->chat_user_list, lst);
        return SRN_OK;
    }

    return SRN_ERR;
}

void srn_server_user_set_nick(SrnServerUser *self, const char *nick){
    str_assign(&self->nick, nick);
    srn_server_user_update_chat_user(self);
}

void srn_server_user_set_username(SrnServerUser *self, const char *username){
    str_assign(&self->username, username);
    srn_server_user_update_chat_user(self);
}

void srn_server_user_set_hostname(SrnServerUser *self, const char *hostname){
    str_assign(&self->hostname, hostname);
    srn_server_user_update_chat_user(self);
}

void srn_server_user_set_realname(SrnServerUser *self, const char *realname){
    str_assign(&self->realname, realname);
}

void srn_server_user_set_is_me(SrnServerUser *self, bool me){
    if (self->is_me == me){
        return;
    }
    self->is_me = me;
    srn_server_user_update_chat_user(self);
}

void srn_server_user_set_is_online(SrnServerUser *self, bool online){
    self->is_online = online;

    if (!self->is_online){
        GList *lst;

        lst = self->chat_user_list;
        while (lst) {
            SrnChatUser *chat_user;

            chat_user = lst->data;
            srn_chat_user_set_is_joined(chat_user, FALSE);
            lst = g_list_next(lst);
        }
    }
}

void srn_server_user_set_is_ignored(SrnServerUser *self, bool is_ignored){
    if (self->is_ignored == is_ignored) {
        return;
    }
    self->is_ignored = is_ignored;
}

static void srn_server_user_update_chat_user(SrnServerUser *self){
    GList *lst;

    lst = self->chat_user_list;
    while (lst) {
        SrnChatUser *chat_user;

        chat_user = lst->data;
        srn_chat_user_update(chat_user);
        lst = g_list_next(lst);
    }
}
