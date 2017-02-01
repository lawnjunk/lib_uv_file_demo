#include <stdlib.h>
#include <stdio.h>
#include <uv.h>

static int count = 0;

void count_to_ten(uv_idle_t *handle){
  if(count >= 10) 
    uv_idle_stop(handle);
  printf("on count [%d]\n", count);
  count++;
}

int main(){
  // get lib_uv
  uv_loop_t *main_loop = malloc(sizeof(uv_loop_t));
  uv_loop_init(main_loop);  

  // setup idle callback
  uv_idle_t chill;
  uv_idle_init(main_loop, &chill);
  uv_idle_start(&chill, count_to_ten);
  puts("start");
  puts("middle");
  uv_run(main_loop, UV_RUN_DEFAULT);
  puts("stop");
  uv_loop_close(main_loop);

  free(main_loop);
}
