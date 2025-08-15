#include "../client_include/libcbd.h"
#include <atomic>

std::atomic<int> cbd_aio_counter(0);

void cb(CurveAioContext *ctx){
    if(ctx->ret < 0){
        // handle error
    } else {
        cbd_aio_counter.fetch_add(1, std::memory_order_relaxed);
    }
}

int main(){
    CurveOptions options;
    cbd_lib_init(&options);
    int fd = cbd_libcurve_open("/dev/nbd0");
    CurveAioContext ctx;
    ctx.offset = 0;
    ctx.length = 1024;
    ctx.buf = malloc(1024);
    ctx.cb = cb;
    ctx.op = LIBCURVE_OP_READ;
    cbd_lib_aio_pread(fd, &ctx);
    while (! (cbd_aio_counter.load(std::memory_order_relaxed) == 1)) {
        // all aio requests completed
    }
    cbd_libcurve_close(fd);
}