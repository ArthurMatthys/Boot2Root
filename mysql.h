/* Copyright (c) 2000, 2020, Oracle and/or its affiliates. All rights reserved.
    2 
    3    This program is free software; you can redistribute it and/or modify
    4    it under the terms of the GNU General Public License, version 2.0,
    5    as published by the Free Software Foundation.
    6 
    7    This program is also distributed with certain software (including
    8    but not limited to OpenSSL) that is licensed under separate terms,
    9    as designated in a particular file or component or in included license
   10    documentation.  The authors of MySQL hereby grant you an additional
   11    permission to link the program and your derivative works with the
   12    separately licensed software that they have included with MySQL.
   13 
   14    Without limiting anything contained in the foregoing, this file,
   15    which is part of C Driver for MySQL (Connector/C), is also subject to the
   16    Universal FOSS Exception, version 1.0, a copy of which can be found at
   17    http://oss.oracle.com/licenses/universal-foss-exception.
   18 
   19    This program is distributed in the hope that it will be useful,
   20    but WITHOUT ANY WARRANTY; without even the implied warranty of
   21    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   22    GNU General Public License, version 2.0, for more details.
   23 
   24    You should have received a copy of the GNU General Public License
   25    along with this program; if not, write to the Free Software
   26    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */
   27 
   28 /**
   29   @file include/mysql.h
   30   This file defines the client API to MySQL and also the ABI of the
   31   dynamically linked libmysqlclient.
   32 
   33   The ABI should never be changed in a released product of MySQL,
   34   thus you need to take great care when changing the file. In case
   35   the file is changed so the ABI is broken, you must also update
   36   the SHARED_LIB_MAJOR_VERSION in cmake/mysql_version.cmake
   37 */
   38 
   39 #ifndef _mysql_h
   40 #define _mysql_h
   41 
   42 #ifndef MYSQL_ABI_CHECK
   43 #include <stdbool.h>
   44 #include <stddef.h>
   45 #include <stdint.h>
   46 #include <sys/types.h>
   47 #endif
   48 
   49 // Legacy definition for the benefit of old code. Use uint64_t in new code.
   50 // If you get warnings from printf, use the PRIu64 macro, or, if you need
   51 // compatibility with older versions of the client library, cast
   52 // before printing.
   53 typedef uint64_t my_ulonglong;
   54 
   55 #ifndef my_socket_defined
   56 #define my_socket_defined
   57 #ifdef _WIN32
   58 #include <windows.h>
   59 #ifdef WIN32_LEAN_AND_MEAN
   60 #include <winsock2.h>
   61 #endif
   62 #define my_socket SOCKET
   63 #else
   64 typedef int my_socket;
   65 #endif /* _WIN32 */
   66 #endif /* my_socket_defined */
   67 
   68 // Small extra definition to avoid pulling in my_compiler.h in client code.
   69 // IWYU pragma: no_include "my_compiler.h"
   70 #ifndef MY_COMPILER_INCLUDED
   71 #if !defined(_WIN32)
   72 #define STDCALL
   73 #else
   74 #define STDCALL __stdcall
   75 #endif
   76 #endif /* MY_COMPILER_INCLUDED */
   77 
   78 #include "field_types.h"
   79 #include "my_list.h"
   80 #include "mysql_com.h"
   81 
   82 /* Include declarations of plug-in API */
   83 #include "mysql/client_plugin.h"  // IWYU pragma: keep
   84 
   85 /*
   86   The client should be able to know which version it is compiled against,
   87   even if mysql.h doesn't use this information directly.
   88 */
   89 #include "mysql_version.h"  // IWYU pragma: keep
   90 
   91 // MYSQL_TIME is part of our public API.
   92 #include "mysql_time.h"  // IWYU pragma: keep
   93 
   94 // The error messages are part of our public API.
   95 #include "errmsg.h"  // IWYU pragma: keep
   96 
   97 #ifdef __cplusplus
   98 extern "C" {
   99 #endif
  100 
  101 extern unsigned int mysql_port;
  102 extern char *mysql_unix_port;
  103 
  104 #define CLIENT_NET_RETRY_COUNT 1                 /* Retry count */
  105 #define CLIENT_NET_READ_TIMEOUT 365 * 24 * 3600  /* Timeout on read */
  106 #define CLIENT_NET_WRITE_TIMEOUT 365 * 24 * 3600 /* Timeout on write */
  107 
  108 #define IS_PRI_KEY(n) ((n)&PRI_KEY_FLAG)
  109 #define IS_NOT_NULL(n) ((n)&NOT_NULL_FLAG)
  110 #define IS_BLOB(n) ((n)&BLOB_FLAG)
  111 /**
  112    Returns true if the value is a number which does not need quotes for
  113    the sql_lex.cc parser to parse correctly.
  114 */
  115 #define IS_NUM(t)                                              \
  116   (((t) <= MYSQL_TYPE_INT24 && (t) != MYSQL_TYPE_TIMESTAMP) || \
  117    (t) == MYSQL_TYPE_YEAR || (t) == MYSQL_TYPE_NEWDECIMAL)
  118 #define IS_LONGDATA(t) ((t) >= MYSQL_TYPE_TINY_BLOB && (t) <= MYSQL_TYPE_STRING)
  119 
  120 typedef struct MYSQL_FIELD {
  121   char *name;               /* Name of column */
  122   char *org_name;           /* Original column name, if an alias */
  123   char *table;              /* Table of column if column was a field */
  124   char *org_table;          /* Org table name, if table was an alias */
  125   char *db;                 /* Database for table */
  126   char *catalog;            /* Catalog for table */
  127   char *def;                /* Default value (set by mysql_list_fields) */
  128   unsigned long length;     /* Width of column (create length) */
  129   unsigned long max_length; /* Max width for selected set */
  130   unsigned int name_length;
  131   unsigned int org_name_length;
  132   unsigned int table_length;
  133   unsigned int org_table_length;
  134   unsigned int db_length;
  135   unsigned int catalog_length;
  136   unsigned int def_length;
  137   unsigned int flags;         /* Div flags */
  138   unsigned int decimals;      /* Number of decimals in field */
  139   unsigned int charsetnr;     /* Character set */
  140   enum enum_field_types type; /* Type of field. See mysql_com.h for types */
  141   void *extension;
  142 } MYSQL_FIELD;
  143 
  144 typedef char **MYSQL_ROW;                /* return data as array of strings */
  145 typedef unsigned int MYSQL_FIELD_OFFSET; /* offset to current field */
  146 
  147 #define MYSQL_COUNT_ERROR (~(uint64_t)0)
  148 
  149 /* backward compatibility define - to be removed eventually */
  150 #define ER_WARN_DATA_TRUNCATED WARN_DATA_TRUNCATED
  151 
  152 typedef struct MYSQL_ROWS {
  153   struct MYSQL_ROWS *next; /* list of rows */
  154   MYSQL_ROW data;
  155   unsigned long length;
  156 } MYSQL_ROWS;
  157 
  158 typedef MYSQL_ROWS *MYSQL_ROW_OFFSET; /* offset to current row */
  159 
  160 struct MEM_ROOT;
  161 
  162 typedef struct MYSQL_DATA {
  163   MYSQL_ROWS *data;
  164   struct MEM_ROOT *alloc;
  165   uint64_t rows;
  166   unsigned int fields;
  167 } MYSQL_DATA;
  168 
  169 enum mysql_option {
  170   MYSQL_OPT_CONNECT_TIMEOUT,
  171   MYSQL_OPT_COMPRESS,
  172   MYSQL_OPT_NAMED_PIPE,
  173   MYSQL_INIT_COMMAND,
  174   MYSQL_READ_DEFAULT_FILE,
  175   MYSQL_READ_DEFAULT_GROUP,
  176   MYSQL_SET_CHARSET_DIR,
  177   MYSQL_SET_CHARSET_NAME,
  178   MYSQL_OPT_LOCAL_INFILE,
  179   MYSQL_OPT_PROTOCOL,
  180   MYSQL_SHARED_MEMORY_BASE_NAME,
  181   MYSQL_OPT_READ_TIMEOUT,
  182   MYSQL_OPT_WRITE_TIMEOUT,
  183   MYSQL_OPT_USE_RESULT,
  184   MYSQL_REPORT_DATA_TRUNCATION,
  185   MYSQL_OPT_RECONNECT,
  186   MYSQL_PLUGIN_DIR,
  187   MYSQL_DEFAULT_AUTH,
  188   MYSQL_OPT_BIND,
  189   MYSQL_OPT_SSL_KEY,
  190   MYSQL_OPT_SSL_CERT,
  191   MYSQL_OPT_SSL_CA,
  192   MYSQL_OPT_SSL_CAPATH,
  193   MYSQL_OPT_SSL_CIPHER,
  194   MYSQL_OPT_SSL_CRL,
  195   MYSQL_OPT_SSL_CRLPATH,
  196   MYSQL_OPT_CONNECT_ATTR_RESET,
  197   MYSQL_OPT_CONNECT_ATTR_ADD,
  198   MYSQL_OPT_CONNECT_ATTR_DELETE,
  199   MYSQL_SERVER_PUBLIC_KEY,
  200   MYSQL_ENABLE_CLEARTEXT_PLUGIN,
  201   MYSQL_OPT_CAN_HANDLE_EXPIRED_PASSWORDS,
  202   MYSQL_OPT_MAX_ALLOWED_PACKET,
  203   MYSQL_OPT_NET_BUFFER_LENGTH,
  204   MYSQL_OPT_TLS_VERSION,
  205   MYSQL_OPT_SSL_MODE,
  206   MYSQL_OPT_GET_SERVER_PUBLIC_KEY,
  207   MYSQL_OPT_RETRY_COUNT,
  208   MYSQL_OPT_OPTIONAL_RESULTSET_METADATA,
  209   MYSQL_OPT_SSL_FIPS_MODE,
  210   MYSQL_OPT_TLS_CIPHERSUITES,
  211   MYSQL_OPT_COMPRESSION_ALGORITHMS,
  212   MYSQL_OPT_ZSTD_COMPRESSION_LEVEL,
  213   MYSQL_OPT_LOAD_DATA_LOCAL_DIR
  214 };
  215 
  216 /**
  217   @todo remove the "extension", move st_mysql_options completely
  218   out of mysql.h
  219 */
  220 struct st_mysql_options_extention;
  221 
  222 struct st_mysql_options {
  223   unsigned int connect_timeout, read_timeout, write_timeout;
  224   unsigned int port, protocol;
  225   unsigned long client_flag;
  226   char *host, *user, *password, *unix_socket, *db;
  227   struct Init_commands_array *init_commands;
  228   char *my_cnf_file, *my_cnf_group, *charset_dir, *charset_name;
  229   char *ssl_key;    /* PEM key file */
  230   char *ssl_cert;   /* PEM cert file */
  231   char *ssl_ca;     /* PEM CA file */
  232   char *ssl_capath; /* PEM directory of CA-s? */
  233   char *ssl_cipher; /* cipher to use */
  234   char *shared_memory_base_name;
  235   unsigned long max_allowed_packet;
  236   bool compress, named_pipe;
  237   /**
  238     The local address to bind when connecting to remote server.
  239   */
  240   char *bind_address;
  241   /* 0 - never report, 1 - always report (default) */
  242   bool report_data_truncation;
  243 
  244   /* function pointers for local infile support */
  245   int (*local_infile_init)(void **, const char *, void *);
  246   int (*local_infile_read)(void *, char *, unsigned int);
  247   void (*local_infile_end)(void *);
  248   int (*local_infile_error)(void *, char *, unsigned int);
  249   void *local_infile_userdata;
  250   struct st_mysql_options_extention *extension;
  251 };
  252 
  253 enum mysql_status {
  254   MYSQL_STATUS_READY,
  255   MYSQL_STATUS_GET_RESULT,
  256   MYSQL_STATUS_USE_RESULT,
  257   MYSQL_STATUS_STATEMENT_GET_RESULT
  258 };
  259 
  260 enum mysql_protocol_type {
  261   MYSQL_PROTOCOL_DEFAULT,
  262   MYSQL_PROTOCOL_TCP,
  263   MYSQL_PROTOCOL_SOCKET,
  264   MYSQL_PROTOCOL_PIPE,
  265   MYSQL_PROTOCOL_MEMORY
  266 };
  267 
  268 enum mysql_ssl_mode {
  269   SSL_MODE_DISABLED = 1,
  270   SSL_MODE_PREFERRED,
  271   SSL_MODE_REQUIRED,
  272   SSL_MODE_VERIFY_CA,
  273   SSL_MODE_VERIFY_IDENTITY
  274 };
  275 
  276 enum mysql_ssl_fips_mode {
  277   SSL_FIPS_MODE_OFF = 0,
  278   SSL_FIPS_MODE_ON = 1,
  279   SSL_FIPS_MODE_STRICT
  280 };
  281 
  282 typedef struct character_set {
  283   unsigned int number;   /* character set number              */
  284   unsigned int state;    /* character set state               */
  285   const char *csname;    /* collation name                    */
  286   const char *name;      /* character set name                */
  287   const char *comment;   /* comment                           */
  288   const char *dir;       /* character set directory           */
  289   unsigned int mbminlen; /* min. length for multibyte strings */
  290   unsigned int mbmaxlen; /* max. length for multibyte strings */
  291 } MY_CHARSET_INFO;
  292 
  293 struct MYSQL_METHODS;
  294 struct MYSQL_STMT;
  295 
  296 typedef struct MYSQL {
  297   NET net;                     /* Communication parameters */
  298   unsigned char *connector_fd; /* ConnectorFd for SSL */
  299   char *host, *user, *passwd, *unix_socket, *server_version, *host_info;
  300   char *info, *db;
  301   struct CHARSET_INFO *charset;
  302   MYSQL_FIELD *fields;
  303   struct MEM_ROOT *field_alloc;
  304   uint64_t affected_rows;
  305   uint64_t insert_id;      /* id if insert on table with NEXTNR */
  306   uint64_t extra_info;     /* Not used */
  307   unsigned long thread_id; /* Id for connection in server */
  308   unsigned long packet_length;
  309   unsigned int port;
  310   unsigned long client_flag, server_capabilities;
  311   unsigned int protocol_version;
  312   unsigned int field_count;
  313   unsigned int server_status;
  314   unsigned int server_language;
  315   unsigned int warning_count;
  316   struct st_mysql_options options;
  317   enum mysql_status status;
  318   enum enum_resultset_metadata resultset_metadata;
  319   bool free_me;   /* If free in mysql_close */
  320   bool reconnect; /* set to 1 if automatic reconnect */
  321 
  322   /* session-wide random string */
  323   char scramble[SCRAMBLE_LENGTH + 1];
  324 
  325   LIST *stmts; /* list of all statements */
  326   const struct MYSQL_METHODS *methods;
  327   void *thd;
  328   /*
  329     Points to boolean flag in MYSQL_RES  or MYSQL_STMT. We set this flag
  330     from mysql_stmt_close if close had to cancel result set of this object.
  331   */
  332   bool *unbuffered_fetch_owner;
  333   void *extension;
  334 } MYSQL;
  335 
  336 typedef struct MYSQL_RES {
  337   uint64_t row_count;
  338   MYSQL_FIELD *fields;
  339   struct MYSQL_DATA *data;
  340   MYSQL_ROWS *data_cursor;
  341   unsigned long *lengths; /* column lengths of current row */
  342   MYSQL *handle;          /* for unbuffered reads */
  343   const struct MYSQL_METHODS *methods;
  344   MYSQL_ROW row;         /* If unbuffered read */
  345   MYSQL_ROW current_row; /* buffer to current row */
  346   struct MEM_ROOT *field_alloc;
  347   unsigned int field_count, current_field;
  348   bool eof; /* Used by mysql_fetch_row */
  349   /* mysql_stmt_close() had to cancel this result */
  350   bool unbuffered_fetch_cancelled;
  351   enum enum_resultset_metadata metadata;
  352   void *extension;
  353 } MYSQL_RES;
  354 
  355 /**
  356   Flag to indicate that COM_BINLOG_DUMP_GTID should
  357   be used rather than COM_BINLOG_DUMP in the @sa mysql_binlog_open().
  358 */
  359 #define MYSQL_RPL_GTID (1 << 16)
  360 /**
  361   Skip HEARBEAT events in the @sa mysql_binlog_fetch().
  362 */
  363 #define MYSQL_RPL_SKIP_HEARTBEAT (1 << 17)
  364 
  365 /**
  366   Struct for information about a replication stream.
  367 
  368   @sa mysql_binlog_open()
  369   @sa mysql_binlog_fetch()
  370   @sa mysql_binlog_close()
  371 */
  372 typedef struct MYSQL_RPL {
  373   size_t file_name_length; /** Length of the 'file_name' or 0     */
  374   const char *file_name;   /** Filename of the binary log to read */
  375   uint64_t start_position; /** Position in the binary log to      */
  376                            /*  start reading from                 */
  377   unsigned int server_id;  /** Server ID to use when identifying  */
  378                            /*  with the master                    */
  379   unsigned int flags;      /** Flags, e.g. MYSQL_RPL_GTID         */
  380 
  381   /** Size of gtid set data              */
  382   size_t gtid_set_encoded_size;
  383   /** Callback function which is called  */
  384   /*  from @sa mysql_binlog_open() to    */
  385   /*  fill command packet gtid set       */
  386   void (*fix_gtid_set)(struct MYSQL_RPL *rpl, unsigned char *packet_gtid_set);
  387   void *gtid_set_arg; /** GTID set data or an argument for   */
  388                       /*  fix_gtid_set() callback function   */
  389 
  390   unsigned long size;          /** Size of the packet returned by     */
  391                                /*  mysql_binlog_fetch()               */
  392   const unsigned char *buffer; /** Pointer to returned data           */
  393 } MYSQL_RPL;
  394 
  395 /*
  396   Set up and bring down the server; to ensure that applications will
  397   work when linked against either the standard client library or the
  398   embedded server library, these functions should be called.
  399 */
  400 int STDCALL mysql_server_init(int argc, char **argv, char **groups);
  401 void STDCALL mysql_server_end(void);
  402 
  403 /*
  404   mysql_server_init/end need to be called when using libmysqld or
  405   libmysqlclient (exactly, mysql_server_init() is called by mysql_init() so
  406   you don't need to call it explicitely; but you need to call
  407   mysql_server_end() to free memory). The names are a bit misleading
  408   (mysql_SERVER* to be used when using libmysqlCLIENT). So we add more general
  409   names which suit well whether you're using libmysqld or libmysqlclient. We
  410   intend to promote these aliases over the mysql_server* ones.
  411 */
  412 #define mysql_library_init mysql_server_init
  413 #define mysql_library_end mysql_server_end
  414 
  415 /*
  416   Set up and bring down a thread; these function should be called
  417   for each thread in an application which opens at least one MySQL
  418   connection.  All uses of the connection(s) should be between these
  419   function calls.
  420 */
  421 bool STDCALL mysql_thread_init(void);
  422 void STDCALL mysql_thread_end(void);
  423 
  424 /*
  425   Functions to get information from the MYSQL and MYSQL_RES structures
  426   Should definitely be used if one uses shared libraries.
  427 */
  428 
  429 uint64_t STDCALL mysql_num_rows(MYSQL_RES *res);
  430 unsigned int STDCALL mysql_num_fields(MYSQL_RES *res);
  431 bool STDCALL mysql_eof(MYSQL_RES *res);
  432 MYSQL_FIELD *STDCALL mysql_fetch_field_direct(MYSQL_RES *res,
  433                                               unsigned int fieldnr);
  434 MYSQL_FIELD *STDCALL mysql_fetch_fields(MYSQL_RES *res);
  435 MYSQL_ROW_OFFSET STDCALL mysql_row_tell(MYSQL_RES *res);
  436 MYSQL_FIELD_OFFSET STDCALL mysql_field_tell(MYSQL_RES *res);
  437 enum enum_resultset_metadata STDCALL mysql_result_metadata(MYSQL_RES *result);
  438 
  439 unsigned int STDCALL mysql_field_count(MYSQL *mysql);
  440 uint64_t STDCALL mysql_affected_rows(MYSQL *mysql);
  441 uint64_t STDCALL mysql_insert_id(MYSQL *mysql);
  442 unsigned int STDCALL mysql_errno(MYSQL *mysql);
  443 const char *STDCALL mysql_error(MYSQL *mysql);
  444 const char *STDCALL mysql_sqlstate(MYSQL *mysql);
  445 unsigned int STDCALL mysql_warning_count(MYSQL *mysql);
  446 const char *STDCALL mysql_info(MYSQL *mysql);
  447 unsigned long STDCALL mysql_thread_id(MYSQL *mysql);
  448 const char *STDCALL mysql_character_set_name(MYSQL *mysql);
  449 int STDCALL mysql_set_character_set(MYSQL *mysql, const char *csname);
  450 
  451 MYSQL *STDCALL mysql_init(MYSQL *mysql);
  452 bool STDCALL mysql_ssl_set(MYSQL *mysql, const char *key, const char *cert,
  453                            const char *ca, const char *capath,
  454                            const char *cipher);
  455 const char *STDCALL mysql_get_ssl_cipher(MYSQL *mysql);
  456 bool STDCALL mysql_change_user(MYSQL *mysql, const char *user,
  457                                const char *passwd, const char *db);
  458 MYSQL *STDCALL mysql_real_connect(MYSQL *mysql, const char *host,
  459                                   const char *user, const char *passwd,
  460                                   const char *db, unsigned int port,
  461                                   const char *unix_socket,
  462                                   unsigned long clientflag);
  463 int STDCALL mysql_select_db(MYSQL *mysql, const char *db);
  464 int STDCALL mysql_query(MYSQL *mysql, const char *q);
  465 int STDCALL mysql_send_query(MYSQL *mysql, const char *q, unsigned long length);
  466 int STDCALL mysql_real_query(MYSQL *mysql, const char *q, unsigned long length);
  467 MYSQL_RES *STDCALL mysql_store_result(MYSQL *mysql);
  468 MYSQL_RES *STDCALL mysql_use_result(MYSQL *mysql);
  469 
  470 enum net_async_status STDCALL mysql_real_connect_nonblocking(
  471     MYSQL *mysql, const char *host, const char *user, const char *passwd,
  472     const char *db, unsigned int port, const char *unix_socket,
  473     unsigned long clientflag);
  474 enum net_async_status STDCALL mysql_send_query_nonblocking(
  475     MYSQL *mysql, const char *query, unsigned long length);
  476 enum net_async_status STDCALL mysql_real_query_nonblocking(
  477     MYSQL *mysql, const char *query, unsigned long length);
  478 enum net_async_status STDCALL
  479 mysql_store_result_nonblocking(MYSQL *mysql, MYSQL_RES **result);
  480 enum net_async_status STDCALL mysql_next_result_nonblocking(MYSQL *mysql);
  481 enum net_async_status STDCALL mysql_select_db_nonblocking(MYSQL *mysql,
  482                                                           const char *db,
  483                                                           bool *error);
  484 void STDCALL mysql_get_character_set_info(MYSQL *mysql,
  485                                           MY_CHARSET_INFO *charset);
  486 
  487 int STDCALL mysql_session_track_get_first(MYSQL *mysql,
  488                                           enum enum_session_state_type type,
  489                                           const char **data, size_t *length);
  490 int STDCALL mysql_session_track_get_next(MYSQL *mysql,
  491                                          enum enum_session_state_type type,
  492                                          const char **data, size_t *length);
  493 /* local infile support */
  494 
  495 #define LOCAL_INFILE_ERROR_LEN 512
  496 
  497 void mysql_set_local_infile_handler(
  498     MYSQL *mysql, int (*local_infile_init)(void **, const char *, void *),
  499     int (*local_infile_read)(void *, char *, unsigned int),
  500     void (*local_infile_end)(void *),
  501     int (*local_infile_error)(void *, char *, unsigned int), void *);
  502 
  503 void mysql_set_local_infile_default(MYSQL *mysql);
  504 int STDCALL mysql_shutdown(MYSQL *mysql,
  505                            enum mysql_enum_shutdown_level shutdown_level);
  506 int STDCALL mysql_dump_debug_info(MYSQL *mysql);
  507 int STDCALL mysql_refresh(MYSQL *mysql, unsigned int refresh_options);
  508 int STDCALL mysql_kill(MYSQL *mysql, unsigned long pid);
  509 int STDCALL mysql_set_server_option(MYSQL *mysql,
  510                                     enum enum_mysql_set_option option);
  511 int STDCALL mysql_ping(MYSQL *mysql);
  512 const char *STDCALL mysql_stat(MYSQL *mysql);
  513 const char *STDCALL mysql_get_server_info(MYSQL *mysql);
  514 const char *STDCALL mysql_get_client_info(void);
  515 unsigned long STDCALL mysql_get_client_version(void);
  516 const char *STDCALL mysql_get_host_info(MYSQL *mysql);
  517 unsigned long STDCALL mysql_get_server_version(MYSQL *mysql);
  518 unsigned int STDCALL mysql_get_proto_info(MYSQL *mysql);
  519 MYSQL_RES *STDCALL mysql_list_dbs(MYSQL *mysql, const char *wild);
  520 MYSQL_RES *STDCALL mysql_list_tables(MYSQL *mysql, const char *wild);
  521 MYSQL_RES *STDCALL mysql_list_processes(MYSQL *mysql);
  522 int STDCALL mysql_options(MYSQL *mysql, enum mysql_option option,
  523                           const void *arg);
  524 int STDCALL mysql_options4(MYSQL *mysql, enum mysql_option option,
  525                            const void *arg1, const void *arg2);
  526 int STDCALL mysql_get_option(MYSQL *mysql, enum mysql_option option,
  527                              const void *arg);
  528 void STDCALL mysql_free_result(MYSQL_RES *result);
  529 enum net_async_status STDCALL mysql_free_result_nonblocking(MYSQL_RES *result);
  530 void STDCALL mysql_data_seek(MYSQL_RES *result, uint64_t offset);
  531 MYSQL_ROW_OFFSET STDCALL mysql_row_seek(MYSQL_RES *result,
  532                                         MYSQL_ROW_OFFSET offset);
  533 MYSQL_FIELD_OFFSET STDCALL mysql_field_seek(MYSQL_RES *result,
  534                                             MYSQL_FIELD_OFFSET offset);
  535 MYSQL_ROW STDCALL mysql_fetch_row(MYSQL_RES *result);
  536 enum net_async_status STDCALL mysql_fetch_row_nonblocking(MYSQL_RES *res,
  537                                                           MYSQL_ROW *row);
  538 
  539 unsigned long *STDCALL mysql_fetch_lengths(MYSQL_RES *result);
  540 MYSQL_FIELD *STDCALL mysql_fetch_field(MYSQL_RES *result);
  541 MYSQL_RES *STDCALL mysql_list_fields(MYSQL *mysql, const char *table,
  542                                      const char *wild);
  543 unsigned long STDCALL mysql_escape_string(char *to, const char *from,
  544                                           unsigned long from_length);
  545 unsigned long STDCALL mysql_hex_string(char *to, const char *from,
  546                                        unsigned long from_length);
  547 unsigned long STDCALL mysql_real_escape_string(MYSQL *mysql, char *to,
  548                                                const char *from,
  549                                                unsigned long length);
  550 unsigned long STDCALL mysql_real_escape_string_quote(MYSQL *mysql, char *to,
  551                                                      const char *from,
  552                                                      unsigned long length,
  553                                                      char quote);
  554 void STDCALL mysql_debug(const char *debug);
  555 void STDCALL myodbc_remove_escape(MYSQL *mysql, char *name);
  556 unsigned int STDCALL mysql_thread_safe(void);
  557 bool STDCALL mysql_read_query_result(MYSQL *mysql);
  558 int STDCALL mysql_reset_connection(MYSQL *mysql);
  559 
  560 int STDCALL mysql_binlog_open(MYSQL *mysql, MYSQL_RPL *rpl);
  561 int STDCALL mysql_binlog_fetch(MYSQL *mysql, MYSQL_RPL *rpl);
  562 void STDCALL mysql_binlog_close(MYSQL *mysql, MYSQL_RPL *rpl);
  563 
  564 /*
  565   The following definitions are added for the enhanced
  566   client-server protocol
  567 */
  568 
  569 /* statement state */
  570 enum enum_mysql_stmt_state {
  571   MYSQL_STMT_INIT_DONE = 1,
  572   MYSQL_STMT_PREPARE_DONE,
  573   MYSQL_STMT_EXECUTE_DONE,
  574   MYSQL_STMT_FETCH_DONE
  575 };
  576 
  577 /*
  578   This structure is used to define bind information, and
  579   internally by the client library.
  580   Public members with their descriptions are listed below
  581   (conventionally `On input' refers to the binds given to
  582   mysql_stmt_bind_param, `On output' refers to the binds given
  583   to mysql_stmt_bind_result):
  584 
  585   buffer_type    - One of the MYSQL_* types, used to describe
  586                    the host language type of buffer.
  587                    On output: if column type is different from
  588                    buffer_type, column value is automatically converted
  589                    to buffer_type before it is stored in the buffer.
  590   buffer         - On input: points to the buffer with input data.
  591                    On output: points to the buffer capable to store
  592                    output data.
  593                    The type of memory pointed by buffer must correspond
  594                    to buffer_type. See the correspondence table in
  595                    the comment to mysql_stmt_bind_param.
  596 
  597   The two above members are mandatory for any kind of bind.
  598 
  599   buffer_length  - the length of the buffer. You don't have to set
  600                    it for any fixed length buffer: float, double,
  601                    int, etc. It must be set however for variable-length
  602                    types, such as BLOBs or STRINGs.
  603 
  604   length         - On input: in case when lengths of input values
  605                    are different for each execute, you can set this to
  606                    point at a variable containining value length. This
  607                    way the value length can be different in each execute.
  608                    If length is not NULL, buffer_length is not used.
  609                    Note, length can even point at buffer_length if
  610                    you keep bind structures around while fetching:
  611                    this way you can change buffer_length before
  612                    each execution, everything will work ok.
  613                    On output: if length is set, mysql_stmt_fetch will
  614                    write column length into it.
  615 
  616   is_null        - On input: points to a boolean variable that should
  617                    be set to TRUE for NULL values.
  618                    This member is useful only if your data may be
  619                    NULL in some but not all cases.
  620                    If your data is never NULL, is_null should be set to 0.
  621                    If your data is always NULL, set buffer_type
  622                    to MYSQL_TYPE_NULL, and is_null will not be used.
  623 
  624   is_unsigned    - On input: used to signify that values provided for one
  625                    of numeric types are unsigned.
  626                    On output describes signedness of the output buffer.
  627                    If, taking into account is_unsigned flag, column data
  628                    is out of range of the output buffer, data for this column
  629                    is regarded truncated. Note that this has no correspondence
  630                    to the sign of result set column, if you need to find it out
  631                    use mysql_stmt_result_metadata.
  632   error          - where to write a truncation error if it is present.
  633                    possible error value is:
  634                    0  no truncation
  635                    1  value is out of range or buffer is too small
  636 
  637   Please note that MYSQL_BIND also has internals members.
  638 */
  639 
  640 typedef struct MYSQL_BIND {
  641   unsigned long *length; /* output length pointer */
  642   bool *is_null;         /* Pointer to null indicator */
  643   void *buffer;          /* buffer to get/put data */
  644   /* set this if you want to track data truncations happened during fetch */
  645   bool *error;
  646   unsigned char *row_ptr; /* for the current data position */
  647   void (*store_param_func)(NET *net, struct MYSQL_BIND *param);
  648   void (*fetch_result)(struct MYSQL_BIND *, MYSQL_FIELD *, unsigned char **row);
  649   void (*skip_result)(struct MYSQL_BIND *, MYSQL_FIELD *, unsigned char **row);
  650   /* output buffer length, must be set when fetching str/binary */
  651   unsigned long buffer_length;
  652   unsigned long offset;              /* offset position for char/binary fetch */
  653   unsigned long length_value;        /* Used if length is 0 */
  654   unsigned int param_number;         /* For null count and error messages */
  655   unsigned int pack_length;          /* Internal length for packed data */
  656   enum enum_field_types buffer_type; /* buffer type */
  657   bool error_value;                  /* used if error is 0 */
  658   bool is_unsigned;                  /* set if integer type is unsigned */
  659   bool long_data_used;               /* If used with mysql_send_long_data */
  660   bool is_null_value;                /* Used if is_null is 0 */
  661   void *extension;
  662 } MYSQL_BIND;
  663 
  664 struct MYSQL_STMT_EXT;
  665 
  666 /* statement handler */
  667 typedef struct MYSQL_STMT {
  668   struct MEM_ROOT *mem_root; /* root allocations */
  669   LIST list;                 /* list to keep track of all stmts */
  670   MYSQL *mysql;              /* connection handle */
  671   MYSQL_BIND *params;        /* input parameters */
  672   MYSQL_BIND *bind;          /* output parameters */
  673   MYSQL_FIELD *fields;       /* result set metadata */
  674   MYSQL_DATA result;         /* cached result set */
  675   MYSQL_ROWS *data_cursor;   /* current row in cached result */
  676   /*
  677     mysql_stmt_fetch() calls this function to fetch one row (it's different
  678     for buffered, unbuffered and cursor fetch).
  679   */
  680   int (*read_row_func)(struct MYSQL_STMT *stmt, unsigned char **row);
  681   /* copy of mysql->affected_rows after statement execution */
  682   uint64_t affected_rows;
  683   uint64_t insert_id;          /* copy of mysql->insert_id */
  684   unsigned long stmt_id;       /* Id for prepared statement */
  685   unsigned long flags;         /* i.e. type of cursor to open */
  686   unsigned long prefetch_rows; /* number of rows per one COM_FETCH */
  687   /*
  688     Copied from mysql->server_status after execute/fetch to know
  689     server-side cursor status for this statement.
  690   */
  691   unsigned int server_status;
  692   unsigned int last_errno;            /* error code */
  693   unsigned int param_count;           /* input parameter count */
  694   unsigned int field_count;           /* number of columns in result set */
  695   enum enum_mysql_stmt_state state;   /* statement state */
  696   char last_error[MYSQL_ERRMSG_SIZE]; /* error message */
  697   char sqlstate[SQLSTATE_LENGTH + 1];
  698   /* Types of input parameters should be sent to server */
  699   bool send_types_to_server;
  700   bool bind_param_done;           /* input buffers were supplied */
  701   unsigned char bind_result_done; /* output buffers were supplied */
  702   /* mysql_stmt_close() had to cancel this result */
  703   bool unbuffered_fetch_cancelled;
  704   /*
  705     Is set to true if we need to calculate field->max_length for
  706     metadata fields when doing mysql_stmt_store_result.
  707   */
  708   bool update_max_length;
  709   struct MYSQL_STMT_EXT *extension;
  710 } MYSQL_STMT;
  711 
  712 enum enum_stmt_attr_type {
  713   /*
  714     When doing mysql_stmt_store_result calculate max_length attribute
  715     of statement metadata. This is to be consistent with the old API,
  716     where this was done automatically.
  717     In the new API we do that only by request because it slows down
  718     mysql_stmt_store_result sufficiently.
  719   */
  720   STMT_ATTR_UPDATE_MAX_LENGTH,
  721   /*
  722     unsigned long with combination of cursor flags (read only, for update,
  723     etc)
  724   */
  725   STMT_ATTR_CURSOR_TYPE,
  726   /*
  727     Amount of rows to retrieve from server per one fetch if using cursors.
  728     Accepts unsigned long attribute in the range 1 - ulong_max
  729   */
  730   STMT_ATTR_PREFETCH_ROWS
  731 };
  732 
  733 MYSQL_STMT *STDCALL mysql_stmt_init(MYSQL *mysql);
  734 int STDCALL mysql_stmt_prepare(MYSQL_STMT *stmt, const char *query,
  735                                unsigned long length);
  736 int STDCALL mysql_stmt_execute(MYSQL_STMT *stmt);
  737 int STDCALL mysql_stmt_fetch(MYSQL_STMT *stmt);
  738 int STDCALL mysql_stmt_fetch_column(MYSQL_STMT *stmt, MYSQL_BIND *bind_arg,
  739                                     unsigned int column, unsigned long offset);
  740 int STDCALL mysql_stmt_store_result(MYSQL_STMT *stmt);
  741 unsigned long STDCALL mysql_stmt_param_count(MYSQL_STMT *stmt);
  742 bool STDCALL mysql_stmt_attr_set(MYSQL_STMT *stmt,
  743                                  enum enum_stmt_attr_type attr_type,
  744                                  const void *attr);
  745 bool STDCALL mysql_stmt_attr_get(MYSQL_STMT *stmt,
  746                                  enum enum_stmt_attr_type attr_type,
  747                                  void *attr);
  748 bool STDCALL mysql_stmt_bind_param(MYSQL_STMT *stmt, MYSQL_BIND *bnd);
  749 bool STDCALL mysql_stmt_bind_result(MYSQL_STMT *stmt, MYSQL_BIND *bnd);
  750 bool STDCALL mysql_stmt_close(MYSQL_STMT *stmt);
  751 bool STDCALL mysql_stmt_reset(MYSQL_STMT *stmt);
  752 bool STDCALL mysql_stmt_free_result(MYSQL_STMT *stmt);
  753 bool STDCALL mysql_stmt_send_long_data(MYSQL_STMT *stmt,
  754                                        unsigned int param_number,
  755                                        const char *data, unsigned long length);
  756 MYSQL_RES *STDCALL mysql_stmt_result_metadata(MYSQL_STMT *stmt);
  757 MYSQL_RES *STDCALL mysql_stmt_param_metadata(MYSQL_STMT *stmt);
  758 unsigned int STDCALL mysql_stmt_errno(MYSQL_STMT *stmt);
  759 const char *STDCALL mysql_stmt_error(MYSQL_STMT *stmt);
  760 const char *STDCALL mysql_stmt_sqlstate(MYSQL_STMT *stmt);
  761 MYSQL_ROW_OFFSET STDCALL mysql_stmt_row_seek(MYSQL_STMT *stmt,
  762                                              MYSQL_ROW_OFFSET offset);
  763 MYSQL_ROW_OFFSET STDCALL mysql_stmt_row_tell(MYSQL_STMT *stmt);
  764 void STDCALL mysql_stmt_data_seek(MYSQL_STMT *stmt, uint64_t offset);
  765 uint64_t STDCALL mysql_stmt_num_rows(MYSQL_STMT *stmt);
  766 uint64_t STDCALL mysql_stmt_affected_rows(MYSQL_STMT *stmt);
  767 uint64_t STDCALL mysql_stmt_insert_id(MYSQL_STMT *stmt);
  768 unsigned int STDCALL mysql_stmt_field_count(MYSQL_STMT *stmt);
  769 
  770 bool STDCALL mysql_commit(MYSQL *mysql);
  771 bool STDCALL mysql_rollback(MYSQL *mysql);
  772 bool STDCALL mysql_autocommit(MYSQL *mysql, bool auto_mode);
  773 bool STDCALL mysql_more_results(MYSQL *mysql);
  774 int STDCALL mysql_next_result(MYSQL *mysql);
  775 int STDCALL mysql_stmt_next_result(MYSQL_STMT *stmt);
  776 void STDCALL mysql_close(MYSQL *sock);
  777 
  778 /* Public key reset */
  779 void STDCALL mysql_reset_server_public_key(void);
  780 
  781 /* status return codes */
  782 #define MYSQL_NO_DATA 100
  783 #define MYSQL_DATA_TRUNCATED 101
  784 
  785 #define mysql_reload(mysql) mysql_refresh((mysql), REFRESH_GRANT)
  786 
  787 #define HAVE_MYSQL_REAL_CONNECT
  788 
  789 MYSQL *STDCALL mysql_real_connect_dns_srv(MYSQL *mysql,
  790                                           const char *dns_srv_name,
  791                                           const char *user, const char *passwd,
  792                                           const char *db,
  793                                           unsigned long client_flag);
  794 
  795 #ifdef __cplusplus
  796 }
  797 #endif
  798 
  799 #endif /* _mysql_h */