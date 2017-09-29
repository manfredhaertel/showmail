/* showmail.h - include file for showmail.c */

/* includes */

#include "config.h"
#include <signal.h>
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#include <errno.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#include <ctype.h>
#include <time.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_TERMIOS_H
#include <termios.h>
#endif
#ifdef HAVE_WINSOCK_H
#include <winsock.h>
#endif
#ifdef HAVE_CONIO_H
#include <conio.h>
#endif

/* defines */

#define MAXBUF 10000
#define MAXSTRING 256
#define SHOWMAIL_SOCKET_SUCCESS 0
#define SHOWMAIL_SOCKET_ERROR -1
#define POP3_SUCCESS 0
#define POP3_ERROR -1
#define CR '\r'
#define LF '\n'
#define TAB '\t'

#ifndef SSL_CERTS
#define SSL_CERTS "/etc/ssl/certs"
#endif

/* prototypes */

int open_socket ( char *node_name , char *service_name ) ;
int open_socket ( char *node_name , char *service_name ) ;
void read_socket ( int this_socket , char *buffer , int size_of_buffer ) ;
void write_socket ( int this_socket , char *buffer ) ;
int pop3_open ( char *host , char *protocol , char *user , char *password ) ;
void pop3_stat ( int this_socket , int *number_of_mails , int *total_size ) ;
int pop3_top ( int this_socket , int index , char *header ) ;
void scan_header ( char *header , char *item , char *headerline ) ;
void pop3_close ( int this_socket ) ;
int pop3_dele ( int this_socket , int index ) ;
int pop3_rset ( int this_socket ) ;
int pop3_list ( int this_socket , int index , int *size ) ;
void get_parameter ( int argc , char **argv , char *server , char *protocol , char *user , char *password , char *feature ) ;
int open_ssl ( int this_socket , char *server ) ;
void close_ssl ( SSL *ssl_connection , SSL_CTX *ssl_context ) ;
void read_ssl ( SSL *ssl_connection , char *buffer , int size_of_buffer ) ;
void write_ssl ( SSL *ssl_connection , char *buffer ) ;
int open_socks5 ( int this_socket , char *dns_name , char *service_name ) ;
void set_debug ( int debug_value ) ;

/* global variables */

int debug ;
int is_pop3s ;
SSL *this_ssl_connection ;
SSL_CTX *this_ssl_context ;
char homedir[MAXSTRING] ;