/*
 * Copyright (c) 2009-2012, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __REDIS_RDB_H
#define __REDIS_RDB_H

#include <stdio.h>
#include "rio.h"

/* TBD: include only necessary headers. */
#include "redis.h"

/* The current RDB version. When the format changes in a way that is no longer
 * backward compatible this number gets incremented.
 * RDB 的版本，当新版本不向就版本兼容时，增一
 */
#define REDIS_RDB_VERSION 6

/* Defines related to the dump file format. To store 32 bits lengths for short
 * keys requires a lot of space, so we check the most significant 2 bits of
 * the first byte to interpreter the length:
 * 通过读取第一字节的最高 2 位来判断长度
 * 00|000000 => if the two MSB are 00 the len is the 6 bits of this byte
 *              长度编码在这一字节的其余 6 位中
 * 01|000000 00000000 =>  01, the len is 14 byes, 6 bits + 8 bits of next byte
 *                        长度为 14 位，当前字节 6 位，加上下个字节 8 位
 * 10|000000 [32 bit integer] => if it's 01, a full 32 bit len will follow
 *                               长度由后跟的 32 位保存
 * 11|000000 this means: specially encoded object will follow. The six bits
 *           number specify the kind of object that follows.
 *           See the REDIS_RDB_ENC_* defines.
 *           后跟一个特殊编码的对象。字节中的 6 位指定对象的类型。
 *           查看 REDIS_RDB_ENC_* 定义获得更多消息
 * Lenghts up to 63 are stored using a single byte, most DB keys, and may
 * values, will fit inside. 
 *
 * 一个字节（的其中 6 个字节）可以保存的最大长度是 63 （包括在内），
 * 对于大多数键和值来说，都已经足够了。
 */
#define REDIS_RDB_6BITLEN 0
#define REDIS_RDB_14BITLEN 1
#define REDIS_RDB_32BITLEN 2
#define REDIS_RDB_ENCVAL 3
// 表示读取/写入错误
#define REDIS_RDB_LENERR UINT_MAX

/* 
 * 当对象是一个字符串对象时，
 * 最高两个位之后的两个位（第 3 个位和第 4 个位）指定了对象的特殊编码
 */
#define REDIS_RDB_ENC_INT8 0  /* 8 bit signed integer */
#define REDIS_RDB_ENC_INT16 1 /* 16 bit signed integer */
#define REDIS_RDB_ENC_INT32 2 /* 32 bit signed integer */
#define REDIS_RDB_ENC_LZF 3   /* string compressed with FASTLZ */

/*
 * 对象类型在 RDB 文件中的类型
 */
#define REDIS_RDB_TYPE_STRING 0
#define REDIS_RDB_TYPE_LIST 1
#define REDIS_RDB_TYPE_SET 2
#define REDIS_RDB_TYPE_ZSET 3
#define REDIS_RDB_TYPE_HASH 4

/* 
 * 对象的编码方式
 */
#define REDIS_RDB_TYPE_HASH_ZIPMAP 9
#define REDIS_RDB_TYPE_LIST_ZIPLIST 10
#define REDIS_RDB_TYPE_SET_INTSET 11
#define REDIS_RDB_TYPE_ZSET_ZIPLIST 12
#define REDIS_RDB_TYPE_HASH_ZIPLIST 13

/* 
 * 检查给定类型是否对象
 */
#define rdbIsObjectType(t) ((t >= 0 && t <= 4) || (t >= 9 && t <= 13))

/* 
 * 数据库特殊操作标识符
 */
// 以 MS 计算的过期时间
#define REDIS_RDB_OPCODE_EXPIRETIME_MS 252
// 以秒计算的过期时间
#define REDIS_RDB_OPCODE_EXPIRETIME 253
// 选择数据库
#define REDIS_RDB_OPCODE_SELECTDB 254
// 数据库的结尾（但不是 RDB 文件的结尾）
#define REDIS_RDB_OPCODE_EOF 255

// 保存类型
int rdbSaveType(rio *rdb, unsigned char type);
// 载入类型
int rdbLoadType(rio *rdb);
// 保存时间
int rdbSaveTime(rio *rdb, time_t t);
// 载入时间
time_t rdbLoadTime(rio *rdb);
// 保存长度信息
int rdbSaveLen(rio *rdb, uint32_t len);
// 载入长度信息
uint32_t rdbLoadLen(rio *rdb, int *isencoded);
// 保存对象类型，根据对象类型给rdbSaveType传入不同的参数
int rdbSaveObjectType(rio *rdb, robj *o);
// 载入类型
int rdbLoadObjectType(rio *rdb);
// 将给定的rdb文件载入到数据库当中
int rdbLoad(char *filename);
// BGSAVE命令的底层实现（调用rdbSave），创建一个子进程写入rdb文件
int rdbSaveBackground(char *filename);
// 移除BGSAVE所产生的临时文件
void rdbRemoveTempFile(pid_t childpid);
// 将数据写入rdb文件
int rdbSave(char *filename);
// 根据类型保存对象
int rdbSaveObject(rio *rdb, robj *o);
// 未使用，已经被废弃
off_t rdbSavedObjectLen(robj *o);
off_t rdbSavedObjectPages(robj *o);
// 从rdb文件中载入指定类型的对象。
robj *rdbLoadObject(int type, rio *rdb);
// 处理BGSAVE完成时发送的信号
void backgroundSaveDoneHandler(int exitcode, int bysignal);
// 将类型、键、值和过期时间写入到rdb文件中
int rdbSaveKeyValuePair(rio *rdb, robj *key, robj *val, long long expiretime, long long now);
// 载入一个字符串对象
robj *rdbLoadStringObject(rio *rdb);

#endif
