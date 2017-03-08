#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2014 funoverip.net.
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 


# This code is mainly a copy/paste of blks2/packet.py
# header, whitening and CRC were adapted to CC11xx packet format

from binascii import hexlify
from gnuradio import gr, digital
from gnuradio import blocks
from gnuradio.digital import packet_utils
import gnuradio.gr.gr_threading as _threading


##how many messages in a queue
DEFAULT_MSGQ_LIMIT = 2


##################################################
## ISM Packet Encoder
##################################################
class _ism_packet_encoder_thread(_threading.Thread):

	def __init__(self, msgq, send):
		self._msgq = msgq
		self._send = send
		_threading.Thread.__init__(self)
		self.setDaemon(1)
		self.keep_running = True
		self.start()

	def run(self):
		while self.keep_running:
			msg = self._msgq.delete_head() #blocking read of message queue
			sample = msg.to_string() #get the body of the msg as a string
			self._send(sample)