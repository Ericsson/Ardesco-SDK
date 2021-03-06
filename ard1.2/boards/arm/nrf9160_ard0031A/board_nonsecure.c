/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 * Copyright (c) Ericsson AB 2020, all rights reserved
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#include <init.h>

#if defined(CONFIG_BSD_LIBRARY) && defined(CONFIG_NET_SOCKETS_OFFLOAD)
#include <net/socket.h>
#endif

#include <logging/log.h>
LOG_MODULE_REGISTER(board_nonsecure, CONFIG_BOARD_LOG_LEVEL);

#define AT_CMD_MAX_READ_LENGTH	128
#define AT_CMD_LEN(cmd)		(sizeof (cmd) - 1)
#define AT_CMD_MAGPIO		"AT%XMAGPIO=1,1,1,7,4,698,770,0,770,825,"\
				"3,825,870,7,1574,1577,2,870,925,"\
				"1,925,960,2,1710,2200"
#define AT_CMD_TRACE		"AT%XMODEMTRACE=0"
// Used to configure modem for GPS use.
#define AT_CMD_COEX0		"AT%XCOEX0=1,1,1570,1580"

static int ard0031A_magpio_configure(void)
{
#if defined(CONFIG_BSD_LIBRARY) && defined(CONFIG_NET_SOCKETS_OFFLOAD)
	int at_socket_fd;
	int buffer;
	u8_t read_buffer[AT_CMD_MAX_READ_LENGTH];

	at_socket_fd = socket(AF_LTE, 0, NPROTO_AT);
	if (at_socket_fd == -1) {
		LOG_ERR("AT socket could not be opened");
		return -EFAULT;
	}

	LOG_DBG("AT CMD: %s", log_strdup(AT_CMD_TRACE));
	buffer = send(at_socket_fd, AT_CMD_TRACE, AT_CMD_LEN(AT_CMD_TRACE), 0);
	if (buffer != AT_CMD_LEN(AT_CMD_TRACE)) {
		LOG_ERR("XMODEMTRACE command failed");
		close(at_socket_fd);
		__ASSERT_NO_MSG(false);
		return -EIO;
	}

	buffer = recv(at_socket_fd, read_buffer, AT_CMD_MAX_READ_LENGTH, 0);
	LOG_DBG("AT RESP: %s", log_strdup(read_buffer));
	if ((buffer < 2) ||
	    (memcmp("OK", read_buffer, 2 != 0))) {
		LOG_ERR("XMODEMTRACE received unexpected response");
		close(at_socket_fd);
		__ASSERT_NO_MSG(false);
		return -EIO;
	}

	LOG_DBG("AT CMD: %s", log_strdup(AT_CMD_MAGPIO));
	buffer = send(at_socket_fd, AT_CMD_MAGPIO,
		      AT_CMD_LEN(AT_CMD_MAGPIO), 0);
	if (buffer != AT_CMD_LEN(AT_CMD_MAGPIO)) {
		LOG_ERR("MAGPIO command failed");
		close(at_socket_fd);
		return -EIO;
	}

	LOG_DBG("AT CMD: %s", log_strdup(AT_CMD_COEX0));
	buffer = send(at_socket_fd, AT_CMD_COEX0,
		      AT_CMD_LEN(AT_CMD_COEX0), 0);
	if (buffer != AT_CMD_LEN(AT_CMD_COEX0)) {
		LOG_ERR("COEX0 command failed");
		close(at_socket_fd);
		return -EIO;
	}

	buffer = recv(at_socket_fd, read_buffer, AT_CMD_MAX_READ_LENGTH, 0);
	LOG_DBG("AT RESP: %s", log_strdup(read_buffer));
	if ((buffer < 2) ||
	    (memcmp("OK", read_buffer, 2 != 0))) {
		LOG_ERR("MAGPIO command failed");
		close(at_socket_fd);
		return -EIO;
	}

	LOG_DBG("MAGPIO successfully configured");

	close(at_socket_fd);
#endif
	return 0;
}

static int ard0031A_board_init(struct device *dev)
{
	int err;

	err = ard0031A_magpio_configure();
	if (err) {
		LOG_ERR("ard0031A_magpio_configure failed with error: %d", err);
		return err;
	}

	return 0;
}

SYS_INIT(ard0031A_board_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
