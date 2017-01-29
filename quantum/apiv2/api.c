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

connected_endpoint_t* get_endpoint(uint8_t endpoint) {
    for (int i=0; i < API_MAX_CONNECTED_ENDPOINTS; ++i) {
        connected_endpoint_t* e = &connected_endpoints[i];
        if (e->is_valid) {
            return e;
        }
    }
    return NULL;
}

connected_endpoint_t* add_endpoint(uint8_t endpoint) {
    for (int i=0; i < API_MAX_CONNECTED_ENDPOINTS; ++i) {
        connected_endpoint_t* e = &connected_endpoints[i];
        if (!e->is_valid) {
            e->is_valid = true;
            return e;
        }
    }
    return NULL;
}


bool api_connect(uint8_t endpoint) {
    api_driver_t* driver = api_get_driver(endpoint);
    if (driver) {
        connected_endpoint_t* e = add_endpoint(endpoint);
        // TODO: handle out of endpoints
        bool connected =  driver->connect(endpoint);
        if (!connected) {
            e->is_valid = false;
        }
        return connected;
    }
    return driver != NULL;
}

bool api_is_connected(uint8_t endpoint) {
    return get_endpoint(endpoint) != NULL;
}

void api_reset(void) {
    for (int i=0; i < API_MAX_CONNECTED_ENDPOINTS; ++i) {
        connected_endpoint_t* e = &connected_endpoints[i];
        e->is_valid = false;
    }
}
