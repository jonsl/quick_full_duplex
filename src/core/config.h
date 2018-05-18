//
// Created by jslater on 10/03/18.
//

#ifndef CONFIG_H
#define CONFIG_H

#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif

#include <sys/types.h>
#include <sys/stat.h>

#ifdef __APPLE__
#include <sys/event.h>
#else
#include <sys/epoll.h>
#include <netinet/in.h>
#endif

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef __cplusplus
#include <cerrno>
#include <atomic>
#include <csignal>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <cassert>
#include <condition_variable>
#include <thread>
#include <future>
#include <mutex>
#include <memory>
#include <scoped_allocator>
#include <limits>
#include <algorithm>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <unordered_map>
#include <deque>
#endif

/// POD types.
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef float float32;
typedef double float64;

/// types
typedef int fd_t;
typedef long time_msec_t;
typedef int pid_t;

/// constants
static const fd_t INVALID_FD = -1;
static const time_msec_t INVALID_TIME_MSEC = -1;
static const int MAX_PROCESSESS = 1024;
#ifdef _LEVEL1_DCACHE_LINESIZE
static const int LEVEL1_DCACHE_LINESIZE = _LEVEL1_DCACHE_LINESIZE;
#endif
#ifdef _NPROCESSORS_ONLINE
static const int NPROCESSORS_ONLINE = _NPROCESSORS_ONLINE;
#endif

#if __GNUC__ >= 4
#   define _VIA_PUBLIC __attribute__ ((visibility ("default")))
#   define _VIA_LOCAL __attribute__ ((visibility ("hidden")))
#else
#   define _VIA_PUBLIC
#   define _VIA_LOCAL
#endif

#ifndef _VIA_INLINE
#define _VIA_INLINE inline
#endif

#ifndef _VIA_NOEXCEPT
#ifdef __APPLE__
#define _VIA_NOEXCEPT _NOEXCEPT
#else
#define _VIA_NOEXCEPT _GLIBCXX_NOEXCEPT
#endif
#endif

#endif //CONFIG_H
