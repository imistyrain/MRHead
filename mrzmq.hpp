#ifndef _MRZMQ_H_
#define _MRZMQ_H_
#include "zmq.hpp"
#define ZMQ_AUX_STR_EXP(__A) #__A
#define ZMQ_AUX_STR(__A) ZMQ_AUX_STR_EXP(__A)
#define _MR_ZMQ_LIBNAME1_ "libzmq-"
#if _MSC_VER < 1300
#define _MR_ZMQ_LIBNAME2_	_MR_ZMQ_LIBNAME1_##"v"
#elif _MSC_VER == 1300
#define _MR_ZMQ_LIBNAME2_	_MR_ZMQ_LIBNAME1_"v70"
#elif _MSC_VER < 1400
#define _MR_ZMQ_LIBNAME2_	_MR_ZMQ_LIBNAME1_"v71"
#elif _MSC_VER < 1500
#define _MR_ZMQ_LIBNAME2_	_MR_ZMQ_LIBNAME1_##"v80"
#elif _MSC_VER == 1500
#define _MR_ZMQ_LIBNAME2_	_MR_ZMQ_LIBNAME1_##"v90"
#elif _MSC_VER == 1600
#define _MR_ZMQ_LIBNAME2_	_MR_ZMQ_LIBNAME1_##"v100"
#elif _MSC_VER == 1700
#define _MR_ZMQ_LIBNAME2_	_MR_ZMQ_LIBNAME1_##"v110"
#elif _MSC_VER == 1800
#define _MR_ZMQ_LIBNAME2_	"libzmq-"##"v120"
#else
#error "Unsupported VC++ version"
#endif
#define _MR_ZMQ_LIBNAME3_ _MR_ZMQ_LIBNAME2_##"-mt-"
#ifdef _DEBUG
#define _MR_ZMQ_LIBNAME4_ _MR_ZMQ_LIBNAME3_##"d"
#else
#define _MR_ZMQ_LIBNAME4_ _MR_ZMQ_LIBNAME3_
#endif
#define _ZMQV1_ ZMQ_AUX_STR(ZMQ_VERSION_MAJOR)##"_"
#define _ZMQV2_ _ZMQV1_ ZMQ_AUX_STR(ZMQ_VERSION_MINOR)
#define  _ZMQV3_ _ZMQV2_##"_"
#define _ZMQV4_ _ZMQV3_ ZMQ_AUX_STR(ZMQ_VERSION_PATCH)
#define  _MR_ZMQ_LIBNAME_ _MR_ZMQ_LIBNAME4_ _ZMQV4_
#pragma  comment(lib,_MR_ZMQ_LIBNAME_)
#endif