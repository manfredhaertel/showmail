s/read_socket ( /read_ssl ( /g
s/write_socket ( /write_ssl ( /g
s/DO_READ_SOCKET ( this_socket/SSL_read ( this_ssl_connection/g
s/DO_WRITE_SOCKET ( this_socket/SSL_write ( this_ssl_connection/g
s/int this_socket/SSL *ssl_connection/g
s/this_socket/ssl_connection/g
