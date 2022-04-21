/******************************************************************/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/******************************************************************/
#include <curl/curl.h>
/******************************************************************/
#define SKIP_LIST_C          1
/******************************************************************/
#define DEFAULT_USERNAME     "binRick"
#define CACHE_DIRECTORY      "./.cache"
#define STAR_URL_TEMPLATE    "https://github.com/stars/binRick/lists/%s"
#define C_REPO_STAR_TPL      "https://github.com/stars/%s/lists/%s"
/******************************************************************/
#define DEFAULT_LOG_LEVEL    LOG_TRACE
#include "../../log/log.c"
/******************************************************************/
#include "../deps/tiny-regex-c/re.h"
/******************************************************************/
#include <gumbo.h>
/******************************************************************/
#include "../include/get-element-by-id.h"
#include "../src/get-element-by-id.c"
/******************************************************************/
#include "../deps/b64/buffer.c"
#include "../deps/b64/decode.c"
#include "../deps/b64/encode.c"
#include "../deps/commander/commander.c"
#include "../deps/fs/fs.c"
#include "../deps/http-get/http-get.c"
#include "../deps/list/list.c"
#include "../deps/list/list_iterator.c"
#include "../deps/list/list_node.c"
#include "../deps/rhash_md5/md5.c"
/******************************************************************/
struct args_t args_t;
struct args_t {
  char   *username;
  bool   verbose;
  int    additional_args_qty;
  list_t *additional_args;
};
/******************************************************************/
struct command *cmd;
struct args_t  *args;


/******************************************************************/


void parse_html(const char *html) {
  log_debug("parsing:%db", strlen(html));
  GumboOutput    *output = gumbo_parse(html);
  list_t         *pkgs   = list_new();

  GumboAttribute *class   = NULL;
  GumboNode      *element = NULL;

  element = gumbo_get_element_by_id("jump-to-results", output->root);
  log_debug("children qty:%d", element->v.element.children.length);
  GumboNode *title_text = element->v.element.children.data[0];

  log_debug("title:%s", title_text->v.text.text);

  class = gumbo_get_attribute(&element->v.element.attributes, "role");
  assert(class->value);
  log_debug("role:%s", class->value);
}


/******************************************************************/
char *encoded_md5(const char *to_encode){
  const char    *s = strdup(to_encode);
  md5_ctx       ctx;
  unsigned char hash[16];

  rhash_md5_init(&ctx);
  rhash_md5_update(&ctx, (const unsigned char *)s, strlen(s));
  rhash_md5_final(&ctx, hash);
  return(b64_encode(hash, strlen(hash)));
}


/******************************************************************/
char * url_cached_file(const char *url){
  char *p = malloc(1024);

  sprintf(p, "%s/%s.txt", CACHE_DIRECTORY, encoded_md5(url));
  return(strdup(p));
}


bool url_cached_file_exists(const char *url){
  return((fs_exists(url_cached_file(url)) == -1) ? false : true);
}


char * get_url_content(const char *url) {
  log_set_level(DEFAULT_LOG_LEVEL);
  log_trace("Fetching url "AC_RESETALL AC_YELLOW AC_REVERSED "%s"AC_RESETALL, url);
  tq_start("");
  http_get_response_t *res;
  bool                dofree = false;
  char                *html;

  log_debug("%s -> %s | %s |", url, encoded_md5(url), url_cached_file(url));
  log_debug("Cached File Exists: %s", url_cached_file_exists(url) ? "Yes" : "No");
  if (!url_cached_file_exists(url)) {
    log_trace("fetching remote content");
    log_trace("caching remote content");
    res = http_get(url);
    assert(res->ok);
    assert(strlen(res->data) > 1024);
    html = strdup(res->data);
    assert(fs_write(url_cached_file(url), res->data));
    assert(url_cached_file_exists(url));
    html = strdup(fs_read(url_cached_file(url)));
    http_get_free(res);
    log_trace("cached remote content");
  }else{
    log_trace("using cached file");
    html = fs_read(url_cached_file(url));
  }
  log_trace(
    AC_RESETALL AC_REVERSED AC_GREEN "working with" AC_RESETALL " " AC_BLUE AC_BOLD "%d" AC_RESETALL " " AC_RESETALL "bytes" AC_RESETALL " :: " AC_YELLOW "%s" AC_RESETALL,
    strlen(html),
    url
    );
  return(strdup(html));
}


/******************************************************************/


static void debug_args(){
  log_debug("Github Username: %s", args->username);
  log_debug("Verbose Mode: %s", args->verbose ? "Enabled" : "Disabled");
  for (int i = 0; i < cmd->argc; ++i) {
    args->additional_args_qty++;
  }
  log_debug("Additional Args Qty: %d", args->additional_args_qty);

  if (args->additional_args_qty > 0) {
    for (int i = 0; i < cmd->argc; ++i) {
      list_rpush(args->additional_args, list_node_new(strdup(cmd->argv[i])));
    }
    int             ii = 0;
    list_node_t     *node;
    list_iterator_t *it = list_iterator_new(args->additional_args, LIST_HEAD);
    while ((node = list_iterator_next(it))) {
      log_debug("- #%d: '%s'", ii + 1, node->val);
    }
  }
}


static void set_username(command_t *self) {
  args->username = strdup(self->arg);
}


static void verbose(command_t *self) {
  args->verbose = true;
}


void setup_args(const int argc, const char **argv){
  args                  = malloc(sizeof(args_t));
  cmd                   = malloc(sizeof(command_t));
  args->username        = strdup(DEFAULT_USERNAME);
  args->additional_args = list_new();

  command_init(cmd, argv[0], "0.0.1");
  command_option(cmd, "-v", "--verbose", "Enable Verbose Mode", verbose);
  command_option(cmd, "-u", "--username <username>", "Github Username", set_username);
  command_parse(cmd, argc, argv);
}


int main(const int argc, const char **argv) {
  if (!fs_exists(CACHE_DIRECTORY)) {
    assert(fs_mkdir(CACHE_DIRECTORY, 0777));
  }
  setup_args(argc, argv);
  debug_args();
  char *u  = "https://github.com/binRick?tab=stars";
  char *u1 = "https://github.com/stars/binRick/lists/c";
//  get_url_content(u);
  char *html = get_url_content(u);

  parse_html(html);
  // get_url_content(u1);
}
