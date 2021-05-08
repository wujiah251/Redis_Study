/* anet.c -- Basic TCP socket stuff made a bit less boring
 *
 * Copyright (c) 2006-2012, Salvatore Sanfilippo <antirez at gmail dot com>
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

#ifndef ANET_H
#define ANET_H

#define ANET_OK 0
#define ANET_ERR -1
#define ANET_ERR_LEN 256

/* Flags used with certain functions. */
#define ANET_NONE 0
#define ANET_IP_ONLY (1<<0)

#if defined(__sun)
#define AF_LOCAL AF_UNIX
#endif

// 创建一个阻塞TCP已连接套接字
int anetTcpConnect(char *err, char *addr, int port);
// 创建一个非阻塞TCP已连接套接字
int anetTcpNonBlockConnect(char *err, char *addr, int port);
// 创建一个非阻塞已连接套接字并绑定到指定的协议地址
int anetTcpNonBlockBindConnect(char *err, char *addr, int port, char *source_addr);
// 创建一个阻塞本地已连接套接字
int anetUnixConnect(char *err, char *path);
// 创建一个非阻塞本地已连接套接字
int anetUnixNonBlockConnect(char *err, char *path);
// 从套接字中读出指定数量的字符
int anetRead(int fd, char *buf, int count);
// 解释host地址到ipbuf中
int anetResolve(char *err, char *host, char *ipbuf, size_t ipbuf_len);
// 单单解析IP的地址
int anetResolveIP(char *err, char *host, char *ipbuf, size_t ipbuf_len);
// 创建一个监听套接字
int anetTcpServer(char *err, int port, char *bindaddr, int backlog);
// 创建一个监听套接字（IPv6）
int anetTcp6Server(char *err, int port, char *bindaddr, int backlog);
// 创建一个本地监听套接字
int anetUnixServer(char *err, char *path, mode_t perm, int backlog);
// 从已连接队列中获取已连接套接字
int anetTcpAccept(char *err, int serversock, char *ip, size_t ip_len, int *port);
// 从已连接队列中获取已连接套接字
int anetUnixAccept(char *err, int serversock);
// 向套接字写入缓冲区内容
int anetWrite(int fd, char *buf, int count);
// 将套接字描述符设置成非阻塞
int anetNonBlock(char *err, int fd);
// 禁用Nagle算法
int anetEnableTcpNoDelay(char *err, int fd);
// 启用Nagle算法(如果一个包小于MSS会等待其他包一起发送)
int anetDisableTcpNoDelay(char *err, int fd);
// 开启socket的长连接选项
int anetTcpKeepAlive(char *err, int fd);
// 获取客户端的ip和端口号
int anetPeerToString(int fd, char *ip, size_t ip_len, int *port);
// 修改长连接选项
int anetKeepAlive(char *err, int fd, int interval);
// 获取服务器本机的IP和端口号
int anetSockName(int fd, char *ip, size_t ip_len, int *port);

#endif
