/* -*- c++ -*- */

#define ISM_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "ism_swig_doc.i"

%{
#include "ism/ism_packet_decoder.h"
%}
%include "ism/ism_packet_decoder.h"
GR_SWIG_BLOCK_MAGIC2(ism, ism_packet_decoder);
