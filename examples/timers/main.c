// Copyright (c) 2020 Cesanta Software Limited
// All rights reserved
//
// Example Websocket server with timers. This is a simple Websocket echo
// server, which sends a message to all connected clients periodically,
// using timer API.

#include "mongoose.h"

static const char *s_listen_on = "http://0.0.0.0:8000";
static const char *s_web_root = "web_root";
#define MAX 1024



int readfile(int speacial_line,char *filename,char *buf)
{
    char line[MAX];
    int lines = 0;
    FILE *fp1 = fopen(filename, "r"); // 创建文件指针及打开文本文件

    if (fp1 == NULL)
    {
        printf("文件 %s 打开时发生错误", filename);
        return -1;
    }

    while(fgets(line,MAX,fp1) != NULL){
    // 或者，使用whie(!feof(fp1)){fgets(line,MAX，fp1)}
        if(speacial_line == lines)
        {
        //	printf("%s",line);// 打印文本
          strcpy(buf,line);
        	break;
        }
		lines++;
    }
    
    //printf("\n一共 %d 行\n",lines);
    fclose(fp1); // 关闭文件指针
    return 0;
}

int get_file_length(char *filename)
{

	int c,file_row = 0, count = 0;
	FILE *fp1; //打开文件
    fp1 = fopen(filename, "r");
	//打开文件，"r+"为访问模式：打开一个用于更新的文件，可读取也可写入。该文件必须存在。
    if(NULL == fp1)
    	{
        	printf("fopen failed!\n");
        	return -1;
    	}
    //计算行数
	while(!feof(fp1)){
		 c = fgetc(fp1);
    	if(c == '\n')
     	 	count ++;
	}
	file_row = count; //加上最后一行
	//printf("该文件中的行数为：%d\n", file_row);//输出文件中的行数 
    fclose(fp1);//关闭文件
	return file_row;
}

// This RESTful server implements the following endpoints:
static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    if (mg_http_match_uri(hm, "/websocket")) {
      mg_ws_upgrade(c, hm, NULL);  // Upgrade HTTP to Websocket
      c->data[0] = 'W';           // Set some unique mark on a connection
    } else {
      // Serve static files
      struct mg_http_serve_opts opts = {.root_dir = s_web_root};
      mg_http_serve_dir(c, ev_data, &opts);
    }
  } else if (ev == MG_EV_WS_MSG) {
    // Got websocket frame. Received data is wm->data. Echo it back!
    struct mg_ws_message *wm = (struct mg_ws_message *) ev_data;
    mg_ws_send(c, wm->data.ptr, wm->data.len, WEBSOCKET_OP_TEXT);
    mg_iobuf_del(&c->recv, 0, c->recv.len);
  }
  (void) fn_data;
}

static void timer_fn(void *arg) {
  struct mg_mgr *mgr = (struct mg_mgr *) arg;
  static int i = 0;
  int length = 0;
  char buf[MAX]= {0};
  length = get_file_length("呻吟语.txt");
  
  // Broadcast "hi" message to all connected websocket clients.
  // Traverse over all connections
  for (struct mg_connection *c = mgr->conns; c != NULL; c = c->next) {
    // Send only to marked connections
    if(i >=  length )
		i = 0;
	  readfile(i,"呻吟语.txt",buf);
    if (c->data[0] == 'W') mg_ws_send(c, buf, sizeof(buf), WEBSOCKET_OP_TEXT);
    
  }
  i++;
}

int main(void) {
  struct mg_mgr mgr;        // Event manager
  mg_mgr_init(&mgr);        // Initialise event manager
  mg_log_set(MG_LL_DEBUG);  // Set log level
  mg_timer_add(&mgr, 30000, MG_TIMER_REPEAT, timer_fn, &mgr);
  mg_http_listen(&mgr, s_listen_on, fn, NULL);  // Create HTTP listener
  for (;;) mg_mgr_poll(&mgr, 500);              // Infinite event loop
  mg_mgr_free(&mgr);                            // Free manager resources
  return 0;
}
