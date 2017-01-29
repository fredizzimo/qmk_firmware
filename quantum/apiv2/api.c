/* Copyright 2016 Fred Sundvik
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
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

#include "api.h"
#include <stddef.h>

#ifndef API_MAX_CONNECTED_ENDPOINTS
#error Please define API_MAX_CONNECTED_ENDPOINTS
#endif

typedef struct {
    uint8_t endpoint;
    bool is_valid;
}connected_endpoint_t;

static connected_endpoint_t connected_endpoints[API_MAX_CONNECTED_ENDPOINTS] = {
    [0 ... API_MAX_CONNECTED_ENDPOINTS - 1] = {.is_valid = false }
};

static bool s_response_sent = false;

connected_endpoint_t* get_endpoint(uint8_t endpoint) {
    for (int i=0; i < API_MAX_CONNECTED_ENDPOINTS; ++i) {
        connected_endpoint_t* e = &connected_endpoints[i];
        if (e->is_valid && e->endpoint == endpoint) {
            return e;
        }
    }
    return NULL;
}

connected_endpoint_t* get_or_create_endpoint(uint8_t endpoint) {
    connected_endpoint_t* free_endpoint = NULL;
    for (int i=0; i < API_MAX_CONNECTED_ENDPOINTS; ++i) {
        connected_endpoint_t* e = &connected_endpoints[i];
        if (!e->is_valid && free_endpoint == NULL) {
            free_endpoint = e;
        }
        else if(e->is_valid && e->endpoint == endpoint) {
            return e;
        }
    }
    if (free_endpoint) {
        free_endpoint->endpoint = endpoint;
    }
    return free_endpoint;
}


bool api_connect(uint8_t endpoint) {
    connected_endpoint_t* e = get_or_create_endpoint(endpoint);
    if (!e) {
        return false;
    }
    if (e->is_valid) {
        return true;
    }
    api_driver_t* driver = api_get_driver(endpoint);
    if (driver) {
        if (driver->connect(endpoint)) {
            e->is_valid = true;
            req_connect req;
            req.protocol_version = API_PROTOCOL_VERSION;
            API_SEND(endpoint, connect, &req, resp);
            if (resp && resp->successful) {
                return true;
            }
            else {
                e = get_endpoint(endpoint);
                if (e) {
                    e->is_valid = false;
                }
            }
        }
    }
    return false;
}

bool api_is_connected(uint8_t endpoint) {
    return get_endpoint(endpoint) != NULL;
}

void api_reset(void) {
    for (int i=0; i < API_MAX_CONNECTED_ENDPOINTS; ++i) {
        connected_endpoint_t* e = &connected_endpoints[i];
        e->is_valid = false;
    }
    s_response_sent = false;
}

static void process_incoming_connect(uint8_t endpoint, req_connect* req, res_connect* resp) {
    resp->successful = req->protocol_version == API_PROTOCOL_VERSION;
}

void api_add_packet(uint8_t endpoint, void* buffer, uint8_t size) {
    if (size < sizeof(api_packet_t) ) {
        return;
    }
    if (((uintptr_t)(buffer) % API_ALIGN) != 0) {
        return;
    }
    api_packet_t* packet = (api_packet_t*)(buffer);
    // We should not receive responses if we are not waiting for it
    if (packet->is_response) {
        return;
    }
    s_response_sent = false;

    switch(packet->id) {
        API_HANDLE(connect, process_incoming_connect);
    }
    // Need another switch to avoid duplicate entries
    switch (packet->id) {
        case api_qmk_begin ... api_qmk_end:
            api_process_qmk(endpoint, packet, size);
            break;
    }

    if (!s_response_sent) {
        res_unhandled unhandled;
        unhandled.original_request = packet->id;
        api_internal_send_response(endpoint, api_command_unhandled, &unhandled, sizeof(unhandled));
    }
}

void api_internal_send_response(uint8_t endpoint, uint8_t id, void* buffer, uint8_t size) {
    if (s_response_sent) {
        return;
    }
    api_driver_t* driver = api_get_driver(endpoint);
    if (driver) {
        api_packet_t* packet = (api_packet_t*)(buffer);
        packet->id = id;
        packet->is_response = true;
        driver->send(endpoint, packet, size);
    }
    s_response_sent = true;
}

void* api_send(uint8_t endpoint, uint8_t command, void* data, uint8_t size, uint8_t recv_size) {
    if (size < sizeof(api_packet_t)) {
        return NULL;
    }
    api_packet_t* send_packet = (api_packet_t*)(data);
    send_packet->id = command;
    send_packet->is_response = false;
    api_driver_t* driver = api_get_driver(endpoint);
    bool connected = api_is_connected(endpoint);
    if (connected) {
        connected = driver->send(endpoint, data, size);
    }
    else {
        return NULL;
    }
    while (connected) {
        uint8_t recv_endpoint = endpoint;
        uint8_t actual_recv_size;
        api_packet_t* res = (api_packet_t*)driver->recv(&recv_endpoint, &actual_recv_size);
        if (!res)  {
            connected = false;
        }
        else if (actual_recv_size >= sizeof(api_packet_t) && res->is_response == false) {
            // TODO: This need to be unit tested
            // Now partly unit tested for connection packets
            api_add_packet(recv_endpoint, res, actual_recv_size);
            continue;
        }
        else if (recv_endpoint == endpoint) {
            if (actual_recv_size == recv_size && res->id == command) {
                return res;
            }
            else {
                connected = false;
            }
        } else {
            // We got something unexpected from another endpoint, most likely the wrong response, so
            // disconnect if we have an open connection
            connected_endpoint_t* e = get_endpoint(recv_endpoint);
            if (e) {
                e->is_valid = false;
            }
        }
    }
    // Complete the disconnection
    get_endpoint(endpoint)->is_valid = false;
    return NULL;
}
