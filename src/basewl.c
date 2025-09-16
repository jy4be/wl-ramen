#include <bits/time.h>
#include <stdint.h>
#include <sys/types.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <wayland-client.h>
#include <linux/input-event-codes.h>
#include <wayland-util.h>
#include "types.h"
#include "wlr-shell-client-protocol.h"
#include "xdg-shell-client-protocol.h"
#include "basewl.h"

/* Shared memory support code */
static void randname(char *buf)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    long r = ts.tv_nsec;
    for (int i = 0; i < 6; ++i) {
        buf[i] = 'A'+(r&15)+(r&16)*2;
        r >>= 5;
    }
}

static int create_shm_file(void)
{
    int retries = 100;
    do {
        char name[] = "/wl_shm-XXXXXX";
        randname(name + sizeof(name) - 7);
        --retries;
        int fd = shm_open(
                name, O_RDWR | O_CREAT | O_EXCL, 0600);
        if (fd >= 0) {
            shm_unlink(name);
            return fd;
        }
    } while (retries > 0 && errno == EEXIST);
    return -1;
}

static int allocate_shm_file(size_t size)
{
    int fd = create_shm_file();
    if (fd < 0)
        return -1;
    int ret;
    do {
        ret = ftruncate(fd, size);
    } while (ret < 0 && errno == EINTR);
    if (ret < 0) {
        close(fd);
        return -1;
    }
    return fd;
}

/* Wayland code */
struct bwl_state {
    /* Globals */
    struct wl_display *wl_display;
    struct wl_registry *wl_registry;
    struct wl_shm *wl_shm;
    struct wl_compositor *wl_compositor;
    struct wl_seat *wl_seat;
    struct zwlr_layer_shell_v1 *wlr_layer_shell;
    /* Objects */
    struct wl_surface *wl_surface;
    struct wl_buffer *wl_buffer;

    struct wl_pointer *wl_pointer;
    struct zwlr_layer_surface_v1 *wlr_layer_surface;
    uint8_t frame;
    bool shouldRedraw;
    uint16_t width;
    uint16_t height;
    uint32_t *pixels;
    bool shouldClose;

    /* Abstract draw callback*/
    struct bwl_command (*abstr_update)(struct screenData Data, struct bwl_pointer_info);
    struct bwl_pointer_info pointer;
};

static void wl_buffer_release(
        void *data, struct wl_buffer *wl_buffer)
{
    /* Sent by the compositor when it's no longer using this buffer */
    wl_buffer_destroy(wl_buffer);
}

static const struct wl_buffer_listener wl_buffer_listener = {
    .release = wl_buffer_release,
};

static void reset_frame(struct bwl_state *state)
{
    const int width = state->width, height = state->height;
    int stride = width * 4;
    int size = stride * height;

    if (state->pixels)
        munmap(state->pixels, size);

    int fd = allocate_shm_file(size);
    if (fd == -1) {
        return;
    }

    uint32_t *data = mmap(NULL, size,
            PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        close(fd);
        return;
    }

    struct wl_shm_pool *pool = 
        wl_shm_create_pool(state->wl_shm, fd, size);
    struct wl_buffer *buffer = 
        wl_shm_pool_create_buffer(pool, 0,
            width, height, stride, WL_SHM_FORMAT_ARGB8888);
    wl_shm_pool_destroy(pool);
    close(fd);

    memset(data, 0, size);
    state->pixels = data;
    state->frame++;
    state->wl_buffer = buffer;
}

static void draw(struct bwl_state *state){
    struct bwl_command cmd =
        state->abstr_update((struct screenData) {state->width, state->height, state->pixels}, state->pointer);
    state->shouldClose = cmd.shouldClose;
    wl_surface_attach(
                    state->wl_surface, 
                    state->wl_buffer, 
                    0, 
                    0);
    wl_surface_damage_buffer(state->wl_surface, 0, 0, state->width, state->height);
    wl_surface_commit(state->wl_surface);

}

static void registry_global(
        void *data, 
        struct wl_registry *wl_registry,
        uint32_t name, 
        const char *interface, 
        uint32_t version)
{
    struct bwl_state *state = data;
    if 
        (strcmp(interface, wl_shm_interface.name) == 0) 
    {
        state->wl_shm = wl_registry_bind(
                wl_registry, name, &wl_shm_interface, 1);
    } else if 
        (strcmp( interface, wl_compositor_interface.name) == 0) 
    {
        state->wl_compositor = wl_registry_bind(
                wl_registry, name, &wl_compositor_interface, 4);
    } else if 
        (strcmp(interface, zwlr_layer_shell_v1_interface.name) == 0) 
    {
        state->wlr_layer_shell = wl_registry_bind(
                wl_registry, name, &zwlr_layer_shell_v1_interface, 1);
    } else if 
        (strcmp(interface, wl_seat_interface.name) == 0) 
    {
        state->wl_seat = wl_registry_bind(
                wl_registry, name, &wl_seat_interface, 9);
    }
}

static void registry_global_remove(
        void *data, 
        struct wl_registry *wl_registry, 
        uint32_t name)
{
    /* This space deliberately left blank */
}

static const struct wl_registry_listener wl_registry_listener = {
    .global = registry_global,
    .global_remove = registry_global_remove,
};

static void zwlr_layer_surface_v1_configure(
        void* data, 
        struct zwlr_layer_surface_v1 *zwlr_layer_surface_v1, 
        uint serial,
        uint w, 
        uint h) 
{
    struct bwl_state *state = data;
    zwlr_layer_surface_v1_ack_configure(
            zwlr_layer_surface_v1, serial);

    reset_frame(state);
    draw(state);
}

struct zwlr_layer_surface_v1_listener wlr_layer_surface_listener = {
    .configure = zwlr_layer_surface_v1_configure,
};

static void wl_pointer_enter(
        void *data, 
        struct wl_pointer *wl_pointer, 
        uint serial, 
        struct wl_surface *wl_surface, 
        wl_fixed_t surface_x, 
        wl_fixed_t surface_y) 
{
}

static void wl_pointer_leave(
        void *data, 
        struct wl_pointer *wl_pointer, 
        uint serial, 
        struct wl_surface *wl_surface) 
{

}

static void wl_pointer_motion(
        void *data, 
        struct wl_pointer *wl_pointer, 
        uint time, 
        wl_fixed_t surface_x, 
        wl_fixed_t surface_y) 
{
    ((struct bwl_state*) data)->pointer.position = 
        (struct vector) {wl_fixed_to_int(surface_x), wl_fixed_to_int(surface_y)};
}

static void wl_pointer_frame(
        void *data, 
        struct wl_pointer *wl_pointer)
{
    ((struct bwl_state*) data)->shouldRedraw = true;
}

static void wl_pointer_button(
        void *data,
        struct wl_pointer *wl_pointer,
        uint serial,
        uint time,
        uint button,
        uint state)
{
    if (button == BTN_LEFT)
        ((struct bwl_state*) data)->pointer.isLeftPressed = 
            (state == WL_POINTER_BUTTON_STATE_PRESSED);
    if (button == BTN_RIGHT) 
        ((struct bwl_state*) data)->pointer.isRightPressed = 
            (state == WL_POINTER_BUTTON_STATE_PRESSED);


}

struct wl_pointer_listener wl_pointer_listener = {
    .enter = wl_pointer_enter,
    .leave = wl_pointer_leave,
    .motion = wl_pointer_motion,
    .frame = wl_pointer_frame,
    .button = wl_pointer_button
};

struct bwl_pointer_info bwl_getPointerInfo(struct bwl_state * state){
    return state->pointer;
}

struct bwl_state *bwl_init(struct bwl_settings settings)
{
    struct bwl_state state = { 0 };
    state.abstr_update = settings.update;
    state.width = settings.width;
    state.height = settings.height;
    state.wl_display = wl_display_connect(NULL);
    state.wl_registry = 
        wl_display_get_registry(state.wl_display);
    wl_registry_add_listener(
            state.wl_registry, &wl_registry_listener, &state);
    wl_display_roundtrip(state.wl_display);

    state.wl_pointer = wl_seat_get_pointer(state.wl_seat);
    wl_pointer_add_listener(
            state.wl_pointer, &wl_pointer_listener, &state);

    state.wl_surface = 
        wl_compositor_create_surface( state.wl_compositor);

    state.wlr_layer_surface = 
        zwlr_layer_shell_v1_get_layer_surface(
                state.wlr_layer_shell, 
                state.wl_surface, 
                NULL, 
                ZWLR_LAYER_SHELL_V1_LAYER_TOP, 
                "qSel");
    zwlr_layer_surface_v1_set_size(
            state.wlr_layer_surface, 
            settings.width, 
            settings.height);
    
    zwlr_layer_surface_v1_add_listener(
            state.wlr_layer_surface, 
            &wlr_layer_surface_listener, 
            &state);
    wl_surface_commit(state.wl_surface);
    
    while (wl_display_dispatch(state.wl_display) && !state.shouldClose) {
        /* This space deliberately left blank */
        if(state.shouldRedraw){
            draw(&state);
            state.shouldRedraw = false;
        }
    }
    wl_seat_release(state.wl_seat);
    munmap(state.pixels, state.width * state.height * 4);
    wl_buffer_destroy(state.wl_buffer);
    zwlr_layer_surface_v1_destroy(state.wlr_layer_surface);
    zwlr_layer_shell_v1_destroy(state.wlr_layer_shell);
    wl_surface_destroy(state.wl_surface);
    wl_display_disconnect(state.wl_display);
    return 0;
}
