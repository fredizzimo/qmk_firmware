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

#include "gtest/gtest.h"
#include "gmock/gmock.h"

extern "C" {
#include "api.h"
}

using testing::_;
using testing::Return;
using testing::DoAll;
using testing::Invoke;
using testing::WithArg;
using testing::MatcherCast;
using testing::Pointee;
using testing::Field;
using testing::AllOf;
using testing::PrintToString;
using testing::InSequence;

MATCHER(CommandIsResponse, "The command is a response") {
    return arg->is_response;
}

MATCHER(CommandIsRequest, "The command is a request") {
    return !arg->is_response;
}

MATCHER_P(CommandIs, id, std::string("The command is " + PrintToString(id))) {
    return arg->id == id;
}

class GetDriverMock {
public:
    GetDriverMock() { s_instance = this; }
    ~GetDriverMock() { s_instance = nullptr; }
    MOCK_METHOD1(get_driver, api_driver_t* (uint8_t));

    static GetDriverMock* s_instance;
};

GetDriverMock* GetDriverMock::s_instance = nullptr;

api_driver_t* api_get_driver(uint8_t endpoint) {
    if (GetDriverMock::s_instance) {
        return GetDriverMock::s_instance->get_driver(endpoint);
    }
    else {
        return nullptr;
    }
}


template<size_t id>
class DriverMock {
public:
    DriverMock() {
        m_driver.connect =
        [](uint8_t endpoint) {
            return DriverMock::s_instance->connect(endpoint);
        };
        m_driver.send =
        [](uint8_t endpoint, void* buffer, uint8_t size) {
            return DriverMock::s_instance->send(endpoint, buffer, size);
        };
        m_driver.recv =
        [](uint8_t* endpoint, uint8_t* size) {
            return DriverMock::s_instance->recv(endpoint, size);
        };
        s_instance = this;
    }
    ~DriverMock() {
        s_instance = nullptr;
    }
    api_driver_t* get_driver() { return &m_driver; }


    MOCK_METHOD1(connect, bool (uint8_t endpoint));
    MOCK_METHOD3(send, bool (uint8_t endpoint, void* buffer, uint8_t size));
    MOCK_METHOD2(recv, void* (uint8_t* endpoint, uint8_t* size));
private:
    static DriverMock* s_instance;
    api_driver_t m_driver;
};

template<size_t id>
DriverMock<id>* DriverMock<id>::s_instance = nullptr;

class Api : public testing::Test
{
public:
    Api() {
        api_reset();
    }
};

TEST_F(Api, ConnectingToANonRegisteredEndpointFails) {
    EXPECT_FALSE(api_is_connected(3));
    GetDriverMock mock;
    EXPECT_CALL(mock, get_driver(_)).WillRepeatedly(Return(nullptr));
    EXPECT_FALSE(api_connect(3));
    EXPECT_FALSE(api_is_connected(3));
}

TEST_F(Api, ASuccessfulConnection) {
    EXPECT_FALSE(api_is_connected(3));
    GetDriverMock mock;
    DriverMock<1> driver;
    res_connect resp;
    resp.is_response = 1;
    resp.id = api_command_connect;
    resp.successful = 1;
    EXPECT_CALL(mock, get_driver(3)).WillRepeatedly(Return(driver.get_driver()));
    EXPECT_CALL(driver, connect(3)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(driver,
        send(3,
            MatcherCast<void*>(MatcherCast<req_connect*>(AllOf(
                Field(&req_connect::protocol_version, API_PROTOCOL_VERSION),
                CommandIsRequest(),
                CommandIs(api_command_connect)
            ))),
            sizeof(req_connect))
        )
        .Times(1).WillOnce(Return(true));
    EXPECT_CALL(driver, recv(Pointee(3), _)).Times(1).WillOnce(Invoke(
        [&resp](uint8_t* endpoint, uint8_t* size) {
            *endpoint = 3;
            *size = sizeof(resp);
            return &resp;
        }
    ));
    EXPECT_TRUE(api_connect(3));
    EXPECT_TRUE(api_is_connected(3));
    // Another endpoint should not be connected
    EXPECT_FALSE(api_is_connected(2));
}

TEST_F(Api, AFailedConnection) {
    EXPECT_FALSE(api_is_connected(3));
    GetDriverMock mock;
    DriverMock<1> driver;
    EXPECT_CALL(mock, get_driver(3)).WillRepeatedly(Return(driver.get_driver()));
    EXPECT_CALL(driver, connect(3)).Times(1).WillOnce(Return(false));
    EXPECT_FALSE(api_connect(3));
    EXPECT_FALSE(api_is_connected(3));
}

TEST_F(Api, AFailedConnectionThroughFailureDuringConnectionPacketSend) {
    EXPECT_FALSE(api_is_connected(3));
    GetDriverMock mock;
    DriverMock<1> driver;
    EXPECT_CALL(mock, get_driver(3)).WillRepeatedly(Return(driver.get_driver()));
    EXPECT_CALL(driver, connect(3)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(driver, send(3, _, _)).Times(1).WillOnce(Return(false));
    EXPECT_FALSE(api_connect(3));
    EXPECT_FALSE(api_is_connected(3));
}

TEST_F(Api, AFailedConnectionDueToRemoteNotAccepting) {
    EXPECT_FALSE(api_is_connected(3));
    GetDriverMock mock;
    DriverMock<1> driver;
    res_connect resp;
    resp.is_response = 1;
    resp.id = api_command_connect;
    resp.successful = 0;
    EXPECT_CALL(mock, get_driver(3)).WillRepeatedly(Return(driver.get_driver()));
    EXPECT_CALL(driver, connect(3)).WillOnce(Return(true));
    EXPECT_CALL(driver, send(3, _, _)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(driver, recv(_, _)).Times(1).WillOnce(Invoke(
        [&resp](uint8_t* endpoint, uint8_t* size) {
            *endpoint = 3;
            *size = sizeof(resp);
            return &resp;
        }
    ));
    EXPECT_FALSE(api_connect(3));
    EXPECT_FALSE(api_is_connected(3));
}

TEST_F(Api, AConnectionFailsWhenTheWrongTypeOfResponseIsReceived) {
    GetDriverMock mock;
    DriverMock<1> driver;
    // Use the same type of packet so that we know if the id really is checked
    res_connect resp;
    resp.is_response = 1;
    resp.id = 0xDEAD;
    resp.is_response = 1;
    resp.successful = 1;
    EXPECT_CALL(mock, get_driver(3)).WillRepeatedly(Return(driver.get_driver()));
    EXPECT_CALL(driver, connect(3)).WillOnce(Return(true));
    EXPECT_CALL(driver, send(3, _, _)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(driver, recv(_, _)).Times(1).WillOnce(Invoke(
        [&resp](uint8_t* endpoint, uint8_t* size) {
            *endpoint = 3;
            *size = sizeof(resp);
            return &resp;
        }
    ));
    EXPECT_FALSE(api_connect(3));
    EXPECT_FALSE(api_is_connected(3));
}

TEST_F(Api, AConnectionFailsWhenTheResponseHasTheWrongSize) {
    GetDriverMock mock;
    DriverMock<1> driver;
    // Use the same type of packet so that we know if the id really is checked
    res_connect resp;
    resp.is_response = 1;
    resp.id = api_command_connect;
    resp.is_response = 1;
    resp.successful = 1;
    EXPECT_CALL(mock, get_driver(3)).WillRepeatedly(Return(driver.get_driver()));
    EXPECT_CALL(driver, connect(3)).WillOnce(Return(true));
    EXPECT_CALL(driver, send(3, _, _)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(driver, recv(_, _)).Times(1).WillOnce(Invoke(
        [&resp](uint8_t* endpoint, uint8_t* size) {
            *endpoint = 3;
            *size = sizeof(resp) - 1;
            return &resp;
        }
    ));
    EXPECT_FALSE(api_connect(3));
    EXPECT_FALSE(api_is_connected(3));
}

TEST_F(Api, AConnectionFailsWhenTheresADisconnectWaitingForResponse) {
    GetDriverMock mock;
    DriverMock<1> driver;
    EXPECT_CALL(mock, get_driver(3)).WillRepeatedly(Return(driver.get_driver()));
    EXPECT_CALL(driver, connect(3)).WillOnce(Return(true));
    EXPECT_CALL(driver, send(3, _, _)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(driver, recv(_, _)).Times(1).WillOnce(Return(nullptr));
    EXPECT_FALSE(api_connect(3));
    EXPECT_FALSE(api_is_connected(3));
}

TEST_F(Api, ReceivingAResponseFromAnUnrelatedEndpointDoesNothing) {
    GetDriverMock mock;
    DriverMock<1> driver;
    res_connect resp;
    resp.is_response = 1;
    resp.id = api_command_connect;
    resp.successful = 1;
    EXPECT_CALL(mock, get_driver(3)).WillRepeatedly(Return(driver.get_driver()));
    EXPECT_CALL(driver, connect(3)).WillOnce(Return(true));
    EXPECT_CALL(driver, send(3, _, _)).Times(1).WillOnce(Return(true));

    EXPECT_CALL(driver, recv(_, _)).Times(2)
        .WillOnce(Invoke(
            [&resp](uint8_t* endpoint, uint8_t* size) {
                *size = sizeof(resp);
                *endpoint = 1;
                return &resp;
            }
        ))
        .WillOnce(Invoke(
            [&resp](uint8_t* endpoint, uint8_t* size) {
                *size = sizeof(resp);
                *endpoint = 3;
                return &resp;
            }
        ));
    EXPECT_TRUE(api_connect(3));
    EXPECT_TRUE(api_is_connected(3));
}

TEST_F(Api, AnotherEndpointIsDisconnectedWhenRecievingUnexpectedConnectionResponseFromItDuringConnect) {
    GetDriverMock mock;
    DriverMock<1> driver;
    res_connect resp;
    resp.is_response = 1;
    resp.id = api_command_connect;
    resp.successful = 1;
    EXPECT_CALL(mock, get_driver(_)).WillRepeatedly(Return(driver.get_driver()));
    EXPECT_CALL(driver, connect(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(driver, send(_, _, _)).WillRepeatedly(Return(true));
    EXPECT_CALL(driver, recv(Pointee(1), _)).Times(1).WillOnce(Invoke(
        [&resp](uint8_t* endpoint, uint8_t* size) {
            *endpoint = 1;
            *size = sizeof(resp);
            return &resp;
        }
    ));
    EXPECT_TRUE(api_connect(1));
    EXPECT_TRUE(api_is_connected(1));

    EXPECT_CALL(driver, recv( _, _)).Times(2)
        .WillOnce(Invoke(
            [&resp](uint8_t* endpoint, uint8_t* size) {
                *size = sizeof(resp);
                *endpoint = 1;
                return &resp;
            }
        ))
        .WillOnce(Invoke(
            [&resp](uint8_t* endpoint, uint8_t* size) {
                *endpoint = 2;
                return &resp;
            }
        ));
    EXPECT_TRUE(api_connect(2));
    EXPECT_TRUE(api_is_connected(2));
    // The original endpoint should be disconnected
    EXPECT_FALSE(api_is_connected(1));
}

TEST_F(Api, AnotherEndpointIsDisconnectedWhenRecievingUnexpectedGeneralResponseFromItDuringConnect) {
    GetDriverMock mock;
    DriverMock<1> driver;
    res_connect resp;
    resp.is_response = 1;
    resp.id = api_command_connect;
    resp.successful = 1;

    res_connect general_response;
    general_response.is_response = 1;
    general_response.id = 0xDEAD;
    general_response.successful = 1;
    EXPECT_CALL(mock, get_driver(_)).WillRepeatedly(Return(driver.get_driver()));
    EXPECT_CALL(driver, connect(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(driver, send(_, _, _)).WillRepeatedly(Return(true));
    EXPECT_CALL(driver, recv(Pointee(1), _)).Times(1).WillOnce(Invoke(
        [&resp](uint8_t* endpoint, uint8_t* size) {
            *endpoint = 1;
            *size = sizeof(resp);
            return &resp;
        }
    ));
    EXPECT_TRUE(api_connect(1));
    EXPECT_TRUE(api_is_connected(1));

    EXPECT_CALL(driver, recv( _, _)).Times(2)
        .WillOnce(Invoke(
            [&general_response](uint8_t* endpoint, uint8_t* size) {
                *size = sizeof(general_response);
                *endpoint = 1;
                return &general_response;
            }
        ))
        .WillOnce(Invoke(
            [&resp](uint8_t* endpoint, uint8_t* size) {
                *size = sizeof(resp);
                *endpoint = 2;
                return &resp;
            }
        ));
    EXPECT_TRUE(api_connect(2));
    EXPECT_TRUE(api_is_connected(2));
    // The original endpoint should be disconnected
    EXPECT_FALSE(api_is_connected(1));
}

TEST_F(Api, TryingToConnectWhenAlreadyConnectedDoesNothing) {
    GetDriverMock mock;
    DriverMock<1> driver;
    res_connect resp;
    resp.is_response = 1;
    resp.id = api_command_connect;
    resp.successful = 1;
    EXPECT_CALL(mock, get_driver(1)).WillOnce(Return(driver.get_driver()));
    EXPECT_CALL(driver, connect(1)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(driver, send(1, _, _)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(driver, recv(_, _)).Times(1).WillOnce(Invoke(
        [&resp](uint8_t* endpoint, uint8_t* size) {
            *endpoint = 1;
            *size = sizeof(resp);
            return &resp;
        }
    ));
    EXPECT_TRUE(api_connect(1));
    EXPECT_TRUE(api_connect(1));
}

TEST_F(Api, ItsPossibleToConnectAfterAFailedTry) {
    GetDriverMock mock;
    DriverMock<1> driver;
    res_connect resp;
    resp.is_response = 1;
    resp.id = api_command_connect;
    resp.successful = 1;
    EXPECT_CALL(mock, get_driver(1)).WillRepeatedly(Return(driver.get_driver()));
    EXPECT_CALL(driver, connect(1)).Times(2).WillOnce(Return(false)).WillOnce(Return(true));
    EXPECT_FALSE(api_connect(1));
    EXPECT_CALL(driver, send(1, _, _)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(driver, recv(_, _)).Times(1).WillOnce(Invoke(
        [&resp](uint8_t* endpoint, uint8_t* size) {
            *endpoint = 1;
            *size = sizeof(resp);
            return &resp;
        }
    ));
    EXPECT_TRUE(api_connect(1));
}

TEST_F(Api, ConnectionFailsWhenTooManyConcurrentConnectionsAreOpened) {
    GetDriverMock mock;
    DriverMock<1> driver;
    res_connect resp;
    resp.is_response = 1;
    resp.id = api_command_connect;
    resp.successful = 1;
    EXPECT_CALL(mock, get_driver(_)).WillRepeatedly(Return(driver.get_driver()));
    EXPECT_CALL(driver, connect(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(driver, send(_, _, _)).WillRepeatedly(Return(true));
    EXPECT_CALL(driver, recv(_, _)).WillRepeatedly(Invoke(
        [&resp](uint8_t* endpoint, uint8_t* size) {
            *size = sizeof(resp);
            return &resp;
        }
    ));
    for (int i = 0; i < API_MAX_CONNECTED_ENDPOINTS; i++) {
        EXPECT_TRUE(api_connect(i));
    }
    EXPECT_FALSE(api_connect(API_MAX_CONNECTED_ENDPOINTS));
    for (int i = 0; i < API_MAX_CONNECTED_ENDPOINTS; i++) {
        EXPECT_TRUE(api_is_connected(i));
    }
    EXPECT_FALSE(api_is_connected(API_MAX_CONNECTED_ENDPOINTS));
}

TEST_F(Api, AnIncommingConnectionWithTheCorrectVersionIsAccepted) {
    GetDriverMock mock;
    DriverMock<1> driver;
    req_connect req;
    req.id = api_command_connect;
    req.is_response = 0;
    req.protocol_version = API_PROTOCOL_VERSION;
    EXPECT_CALL(mock, get_driver(5)).WillRepeatedly(Return(driver.get_driver()));
    EXPECT_CALL(driver,
        send(5,
            MatcherCast<void*>(MatcherCast<res_connect*>(AllOf(
                Field(&res_connect::successful, 1),
                CommandIsResponse(),
                CommandIs(api_command_connect)
            ))),
            sizeof(res_connect)))
    .Times(1).WillOnce(Return(true));
    api_add_packet(5, &req, sizeof(req));
}


TEST_F(Api, AnIncommingConnectionWithTheWrongVersionIsNotAccepted) {
    GetDriverMock mock;
    DriverMock<1> driver;
    req_connect req;
    req.id = api_command_connect;
    req.is_response = 0;
    req.protocol_version = 0xDEAD;
    EXPECT_CALL(mock, get_driver(5)).WillRepeatedly(Return(driver.get_driver()));
    EXPECT_CALL(driver,
        send(5,
            MatcherCast<void*>(MatcherCast<res_connect*>(AllOf(
                Field(&res_connect::successful, 0),
                CommandIsResponse(),
                CommandIs(api_command_connect)
            ))),
            sizeof(res_connect)))
    .Times(1).WillOnce(Return(true));
    api_add_packet(5, &req, sizeof(req));
}

TEST_F(Api, AnIncommingConnectionFromTheSameEndpointCanBeAcceptedDuringConnect) {
    GetDriverMock mock;
    DriverMock<1> driver;
    res_connect resp;
    resp.is_response = true;
    resp.id = api_command_connect;
    resp.successful = 1;

    req_connect req;
    req.is_response = false;
    req.id = api_command_connect;
    req.protocol_version = API_PROTOCOL_VERSION;
    EXPECT_CALL(mock, get_driver(1)).WillRepeatedly(Return(driver.get_driver()));
    InSequence s;
    // Connect the endpoint
    EXPECT_CALL(driver, connect(1)).Times(1).WillOnce(Return(true));
    // Send the request
    EXPECT_CALL(driver,
        send(1,
            MatcherCast<void*>(MatcherCast<req_connect*>(AllOf(
                CommandIsRequest(),
                CommandIs(api_command_connect)
            ))),
            sizeof(req_connect))
    ).Times(1).WillOnce(Return(true));
    // Receive the connection request from the remote, before we have received our response
    EXPECT_CALL(driver, recv(_, _)).Times(1).WillOnce(Invoke(
        [&req](uint8_t* endpoint, uint8_t* size) {
            *endpoint = 1;
            *size = sizeof(req);
            return &req;
        }
    ));
    // Send the response
    EXPECT_CALL(driver,
        send(1,
            MatcherCast<void*>(MatcherCast<res_connect*>(AllOf(
                CommandIsResponse(),
                CommandIs(api_command_connect)
            ))),
            sizeof(res_connect))
    ).Times(1);
    // Receive the response for our outgoing request
    EXPECT_CALL(driver, recv(_, _)).Times(1).WillOnce(Invoke(
        [&resp](uint8_t* endpoint, uint8_t* size) {
            *endpoint = 1;
            *size = sizeof(resp);
            return &resp;
        }
    ));
    EXPECT_TRUE(api_connect(1));
}

TEST_F(Api, AnIncommingConnectionFromAnotherEndpointCanBeAcceptedDuringConnect) {
    GetDriverMock mock;
    DriverMock<1> driver;
    res_connect resp;
    resp.is_response = true;
    resp.id = api_command_connect;
    resp.successful = 1;

    req_connect req;
    req.is_response = false;
    req.id = api_command_connect;
    req.protocol_version = API_PROTOCOL_VERSION;
    EXPECT_CALL(mock, get_driver(_)).WillRepeatedly(Return(driver.get_driver()));
    InSequence s;
    // Connect the endpoint
    EXPECT_CALL(driver, connect(1)).Times(1).WillOnce(Return(true));
    // Send the request
    EXPECT_CALL(driver,
        send(1,
            MatcherCast<void*>(MatcherCast<req_connect*>(AllOf(
                CommandIsRequest(),
                CommandIs(api_command_connect)
            ))),
            sizeof(req_connect))
    ).Times(1).WillOnce(Return(true));
    // Receive the connection request from the remote, before we have received our response
    EXPECT_CALL(driver, recv(_, _)).Times(1).WillOnce(Invoke(
        [&req](uint8_t* endpoint, uint8_t* size) {
            *endpoint = 2;
            *size = sizeof(req);
            return &req;
        }
    ));
    // Send the response
    EXPECT_CALL(driver,
        send(2,
            MatcherCast<void*>(MatcherCast<res_connect*>(AllOf(
                CommandIsResponse(),
                CommandIs(api_command_connect)
            ))),
            sizeof(res_connect))
    ).Times(1);
    // Receive the response for our outgoing request
    EXPECT_CALL(driver, recv(_, _)).Times(1).WillOnce(Invoke(
        [&resp](uint8_t* endpoint, uint8_t* size) {
            *endpoint = 1;
            *size = sizeof(resp);
            return &resp;
        }
    ));
    EXPECT_TRUE(api_connect(1));
}

// TODO: Add tests for other requests during connect
