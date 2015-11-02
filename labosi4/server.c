#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <uv.h>

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 7000
#define DEFAULT_BACKLOG 128


uv_loop_t *loop;
struct sockaddr_in addr;
time_t rawtime;
struct tm * timeinfo;

char* concat(char *s1, char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    buf->base = (char*) malloc(suggested_size);
    buf->len = suggested_size;
}

void callback_write(uv_write_t *req, int status) {
    if (status) {
        fprintf(stderr, "Write error %s\n", uv_strerror(status));
    }
    free(req);
}

void req_read_cb(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
    if (nread < 0) {
        if (nread != UV_EOF)
            fprintf(stderr, "Read error %s\n", uv_err_name(nread));
        uv_close((uv_handle_t*) client, NULL);
    } else if (nread > 0) {
    	printf("\nRequest: %s\n", buf->base);
        uv_write_t *req = (uv_write_t *) malloc(sizeof(uv_write_t));
        time ( &rawtime );
	  	timeinfo = localtime(&rawtime);
	  	int len = strlen(asctime(timeinfo));
	  	char * date_time_now = asctime(timeinfo);
	  	char * response;
        if (strstr(buf->base, "GET") != NULL && strstr(buf->base, "timedate") != NULL) {
        	char * temp = concat("HTTP/1.0 200 OK\nContent-length: 279\nContent-Type: text/html; charset=utf-8\nServer:libuv C++ (Ubuntu)\n\n<html><head><style>h1{margin: 1em 0 0.5em 0;color: #343434;font-weight: normal;font-family: 'Ultra', sans-serif;font-size: 36px;line-height: 42px;text-transform: uppercase;text-shadow: 0 2px white, 0 3px #777;}</style></head><body><h1>", date_time_now);
        	response = concat(temp, "</h1></body></html>");
		} else if(strstr(buf->base,"SIMPLE TIME") != NULL) {
	        response = asctime(timeinfo);
	  	} else {
	  		char * temp = "HTTP/1.0 400 Bad Request\nContent-length: 50\nContent-Type: text/html; charset=utf-8\nServer:libuv C++ (Ubuntu)\n\n<html><body><h1>400 Bad Request</h1></body></html>";
        	response = temp;
	  	}

	  	uv_buf_t wrbuf = uv_buf_init(response, strlen(response));
        
        uv_write(req, client, &wrbuf, 1, callback_write);
    }

    if (buf->base)
        free(buf->base);
}

void on_new_connection(uv_stream_t *server, int status) {
    if (status < 0) {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        return;
    }

    uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
    uv_tcp_init(loop, client);
    if (uv_accept(server, (uv_stream_t*) client) == 0) {
    	printf("TCP accepted...");
        uv_read_start((uv_stream_t*) client, alloc_buffer, req_read_cb);
    }
    else {
        uv_close((uv_handle_t*) client, NULL);
    }
}

int main() {
    loop = uv_default_loop();

    uv_tcp_t server;
    uv_tcp_init(loop, &server);

    uv_ip4_addr(DEFAULT_HOST, DEFAULT_PORT, &addr);

    uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);
    int r = uv_listen((uv_stream_t*) &server, DEFAULT_BACKLOG, on_new_connection);
    if (r) {
        fprintf(stderr, "Listen error %s\n", uv_strerror(r));
        return 1;
    }
    printf("TCP server listening: %s:%d...\n", DEFAULT_HOST, DEFAULT_PORT);
    return uv_run(loop, UV_RUN_DEFAULT);
}