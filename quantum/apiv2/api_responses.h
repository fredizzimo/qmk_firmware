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

#ifndef QUANTUM_APIV2_API_RESPONSES_H_
#define QUANTUM_APIV2_API_RESPONSES_H_

// See api_requests.h for documentation of how to add new responses

BEGIN_MSG
    uint8_t successful;
END_MSG(res_connect);

BEGIN_MSG
    uint8_t original_request; // The command id for the request that was unhandled
END_MSG(res_unhandled);

enum ProtocolError {
    PROTOCOL_ERROR_NOT_CONNECTED,
    PROTOCOL_ERROR_INCOMING_TOO_SMALL,
    PROTOCOL_ERROR_UNEXPECTED_RESPONSE,
};
BEGIN_MSG
    uint8_t error;
END_MSG(res_protocol_error);

#endif /* QUANTUM_APIV2_API_RESPONSES_H_ */
