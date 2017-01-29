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

#ifndef QUANTUM_APIV2_API_H_
#define QUANTUM_APIV2_API_H_

#include <stdint.h>
#include <stdbool.h>

#define API_ALIGN 4

// The first two bytes of this structure can never change
// But you could potentially add more fields if the protocol version is bumped
typedef struct __attribute__((packed, aligned(API_ALIGN))) {
    uint16_t id : 15;
    uint16_t is_response : 1;
} api_packet_t;

// Note, this define has to match the api_packet_t above
#define BEGIN_MSG typedef struct __attribute__((packed, aligned(API_ALIGN))) { \
    uint16_t id : 15; \
    uint16_t is_response : 1;

#define END_MSG(name) } name;

#define API_HANDLE(id, function) \
case api_command_##id: \
    if (sizeof(req_##id) == size) { \
        req_##id* request = (req_##id*)(buffer); \
        res_##id response; \
        function(endpoint, request, &response); \
        api_internal_send_response(endpoint, api_command_##id, &response, sizeof(response)); \
        break; \
    }


#include "api_commands.h"
#include "api_requests.h"
#include "api_responses.h"

bool api_connect(uint8_t endpoint);
bool api_is_connected(uint8_t endpoint);
void api_reset(void);


typedef struct {
    // * This should start initiating a physical and transport level connection to the the endpoint.
    // * The call should be non-blocking, and can be called multiple times until it succeeds.
    // * The driver can perform the connecting in the background and return true when it succeeds.
    // * Either physical or transport level connection can be skipped if the driver don't need to do that
    // * Once the connection has succeeded, a reliable communication channel should be established.
    // * Note, you are only allowed to connect to endpoints with a lower id than yourself, this to prevent
    //   some race conditions in the connection protocol
    bool (*connect)(uint8_t endpoint);

    // * Should perform a blocking send, but doesn't need to wait for the confirmation of the other side.
    // * If it can't physically send yet, it could just save the buffer for later use
    // * Should return false only if the link is or gets disconnected during the send
    bool (*send)(uint8_t endpoint, void* buffer, uint8_t size);

    // * Should return a buffer if data was received, it then also sets the endpoint and the size
    // * If endpoint is specified, should perform a blocking read, that is ended by the following condtions
    //   1. Data is received from the specified endpoint
    //   2. Data is received from another endpoint on the same driver
    //   3. The specified endpoint is disconnected
    // * When no endpoint is specified, it should perform a non-blocking read
    // * Returns NULL if there's no data to be read, or if the specified endpoint is disconnected
    // * The returned buffer should be valid at least until the next send or recv is performed on the driver
    // * Note the returned data has to be aligned to 4 bytes
    void* (*recv)(uint8_t* endpoint, uint8_t* size);
}api_driver_t;

// The keyboard should implement this, you can return NULL for unsupported endpoints
api_driver_t* api_get_driver(uint8_t endpoint);

// * The keyboard should call this when an incoming packet is received
// * Typically this would be implemented by iterating all the drivers onece every scan loop and call recv with
//   a NULL endpoint. If data is received, then you call api_add_packet.
// * The lifetime for the buffer pointer is the same as the driver, which means that it doesn't necessarily
//   even have to be valid until the function returns
// * NOTE: You have to be careful about race conditions, so don't call this directly from an interrupt or
//   another thread as soon as a packet is received.
// * Also don't call this function as a response to another driver request, like recv
void api_add_packet(uint8_t endpoint, void* buffer, uint8_t size);


void api_internal_send_response(uint8_t endpoint, uint8_t id, void* buffer, uint8_t size);


#endif /* QUANTUM_APIV2_API_H_ */
