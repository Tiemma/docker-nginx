/**
 * @file   ngx_http_hello_world_module.c
 * @author António P. P. Almeida <appa@perusio.net>
 * @date   Wed Aug 17 12:06:52 2011
 *
 * @brief  A hello world module for Nginx.
 *
 * @section LICENSE
 *
 * Copyright (C) 2011 by Dominic Fallows, António P. P. Almeida
 * <appa@perusio.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_errno.h>
#include <ngx_http.h>
#include <stdlib.h>

#define HELLO_WORLD "hello world\r\n"

static char *ngx_http_hello_world(ngx_conf_t *cf, ngx_command_t *cmd,
                                  void *conf);
static ngx_int_t ngx_http_hello_world_handler(ngx_http_request_t *r);

/**
 * This module provided directive: hello world.
 *
 */
static ngx_command_t ngx_http_hello_world_commands[] = {

    {ngx_string("docker_network"),        /* directive */
     NGX_HTTP_LOC_CONF | NGX_CONF_NOARGS, /* location context and takes
                                             no arguments*/
     ngx_http_hello_world,                /* configuration setup function */
     0, /* No offset. Only one context is supported. */
     0, /* No offset when storing the module configuration on struct. */
     NULL},

    ngx_null_command /* command termination */
};

/* The hello world string. */
static u_char ngx_hello_world[] = HELLO_WORLD;

/* The module context. */
static ngx_http_module_t ngx_http_hello_world_module_ctx = {
    NULL, /* preconfiguration */
    NULL, /* postconfiguration */

    NULL, /* create main configuration */
    NULL, /* init main configuration */

    NULL, /* create server configuration */
    NULL, /* merge server configuration */

    NULL, /* create location configuration */
    NULL  /* merge location configuration */
};

/* Module definition. */
ngx_module_t ngx_http_hello_world_module = {
    NGX_MODULE_V1,
    &ngx_http_hello_world_module_ctx, /* module context */
    ngx_http_hello_world_commands,    /* module directives */
    NGX_HTTP_MODULE,                  /* module type */
    NULL,                             /* init master */
    NULL,                             /* init module */
    NULL,                             /* init process */
    NULL,                             /* init thread */
    NULL,                             /* exit thread */
    NULL,                             /* exit process */
    NULL,                             /* exit master */
    NGX_MODULE_V1_PADDING};

/* Docker API integration per network
static ngx_int_t ngx_http_subrequest_done(ngx_http_request_t *r, void *data,
                                          ngx_int_t rc) {
  char *msg = (char *)data;

  ngx_log_error(NGX_LOG_DEBUG, r->connection->log, 0,
                "done subrequest r:%p msg:%s rc:%i", r, msg, rc);

  return rc;
}

static ngx_int_t get_containers_in_network(ngx_http_request_t *r,
                                           char *network) {

  u_char query[400];
  ngx_http_request_t *sr;
  ngx_http_post_subrequest_t *ps;
  ngx_str_t uri = ngx_string("http://v1.40/containers/json");
  ngx_int_t rsp;

  ps = ngx_palloc(r->pool, sizeof(ngx_http_post_subrequest_t));
  if (ps == NULL) {
    return NGX_ERROR;
  }
  ps->handler = ngx_http_subrequest_done;
  ps->data = network;
  ngx_snprintf(query, sizeof(query), "filters={\"network\":[\"%s\"]}", network);

  // Returns an ngx_int_t value which corresponds to a status code
  rsp = ngx_http_subrequest(r, &uri, (ngx_str_t *)query, &sr, ps, 0);
  if (rsp == NGX_OK) {
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                  "Connection to %s was unsuccessful, ", r->uri);
  }
  return rsp;
}*/

/**
 * Content handler.
 *
 * @param r
 *   Pointer to the request structure. See http_request.h.
 * @return
 *   The status of the response generation.
 */
static ngx_int_t ngx_http_hello_world_handler(ngx_http_request_t *r) {
  ngx_buf_t *b;
  ngx_chain_t out;
  /*
    if (get_containers_in_network(r, "nginx") != NGX_OK) {
      ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                    "Error occured on docker API call");
    }*/
  /* Set the Content-Type header. */
  r->headers_out.content_type.len = sizeof("text/plain") - 1;
  r->headers_out.content_type.data = (u_char *)"text/plain";

  /* Allocate a new buffer for sending out the reply. */
  b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));

  /* Insertion in the buffer chain. */
  out.buf = b;
  out.next = NULL; /* just one buffer */

  b->pos = ngx_hello_world; /* first position in memory of the data */
  b->last = ngx_hello_world + sizeof(ngx_hello_world) -
            1;     /* last position in memory of the data */
  b->memory = 1;   /* content is in read-only memory */
  b->last_buf = 1; /* there will be no more buffers in the request */

  /* Sending the headers for the reply. */
  r->headers_out.status = NGX_HTTP_OK; /* 200 status code */
  /* Get the content length of the body. */
  r->headers_out.content_length_n = sizeof(ngx_hello_world) - 1;
  ngx_http_send_header(r); /* Send the headers */

  /* Send the body, and return the status code of the output filter chain. */
  return ngx_http_output_filter(r, &out);
} /* ngx_http_hello_world_handler */

/**
 * Configuration setup function that installs the content handler.
 *
 * @param cf
 *   Module configuration structure pointer.
 * @param cmd
 *   Module directives structure pointer.
 * @param conf
 *   Module configuration structure pointer.
 * @return string
 *   Status of the configuration setup.
 */
static char *ngx_http_hello_world(ngx_conf_t *cf, ngx_command_t *cmd,
                                  void *conf) {
  ngx_http_core_loc_conf_t *clcf; /* pointer to core location configuration */

  /* Install the hello world handler. */
  clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
  clcf->handler = ngx_http_hello_world_handler;

  return NGX_CONF_OK;
} /* ngx_http_hello_world */
