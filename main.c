#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <uv.h>

#define SRC_FILE "./main.c"

uv_loop_t *main_loop;
char buffer[1000];
char *file_buffer;

uv_fs_t open_req;
uv_fs_t fstat_req;
uv_fs_t read_req;
uv_fs_t write_req;
uv_fs_t close_req;
uv_buf_t iov;
void (*get_file_buf_callback)(int err, uv_buf_t *data);

void on_read(uv_fs_t *req);

#define vv_log_err(req)\
  fprintf(stderr, "ERROR: %s\n", uv_strerror(req->result));

#define vv_req_is_evil(req) (req->result < 0)
#define vv_req_is_empty(req) (req->result == 0)

void z_callback(int err, uv_buf_t *buffer){
  puts("hit z callback");
  if(err) {
    puts("fuck");
    return;
  }

  printf("buffer length %lu\n", buffer->len);

}

void on_close(uv_fs_t *req){
  if(vv_req_is_evil(req)){
    printf("error closing file");
    return;
  }

  get_file_buf_callback(0, &iov);
  printf("%s file closed", SRC_FILE);
}

void on_read(uv_fs_t *req){
  puts("hit on_read");
  // print
  if(vv_req_is_evil(req)){
    vv_log_err(req);
    printf("gllo\n");
    return;
  }

  if(vv_req_is_empty(req)){
    fprintf(stderr, "file empty\n");
    uv_fs_close(main_loop, &close_req, open_req.result, on_close);
    return;
  }

  uv_fs_close(main_loop, &close_req, open_req.result, on_close);
}

void on_fstat(uv_fs_t *req){
  puts("hit on_fstat");
  if(vv_req_is_evil(req)){
    vv_log_err(req);
    return;
  }

  file_buffer = calloc(sizeof(char), req->statbuf.st_size);
  iov = uv_buf_init(file_buffer, req->statbuf.st_size);
  uv_fs_read(main_loop, &read_req, open_req.result, &iov, 1, 0, on_read);
  printf("file size %llu\n", req->statbuf.st_size);

}

void on_open(uv_fs_t *req){
  puts("hit on_open");
  // check for error
  if(vv_req_is_evil(req)){
    vv_log_err(req);
    return;
  }

  // register read callback
  uv_fs_fstat(main_loop, &fstat_req, req->result, on_fstat);
};

void get_file_buf(char *path, void (*callback)(int err, uv_buf_t *data)){
  uv_fs_open(main_loop, &open_req, SRC_FILE, O_RDONLY, 0, on_open);
  get_file_buf_callback = callback;
};

int main(){
  puts("hit main");
  // get lib_uv
  main_loop = malloc(sizeof(uv_loop_t));
  uv_loop_init(main_loop);
  get_file_buf(SRC_FILE, z_callback);

  uv_run(main_loop, UV_RUN_DEFAULT);
  uv_loop_close(main_loop);

  free(main_loop);
}

/*void on_write(uv_fs_t *req){*/
/*if(vv_req_is_evil(req)){*/
/*printf("error writing file");*/
/*uv_fs_close(main_loop, &close_req, open_req.result, on_close);*/
/*return;*/
/*}*/

/*if(vv_req_is_empty(req)){*/
/*fprintf(stderr, "file empty\n");*/
/*uv_fs_close(main_loop, &close_req, open_req.result, on_close);*/
/*return;*/
/*}*/

/*[>uv_fs_read(main_loop, &read_req, req->result, &iov, 1, -1, on_read);<]*/
/*}*/

