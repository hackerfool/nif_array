#include "erl_nif.h"
#include <stdio.h>
#include <string.h>

static ErlNifResourceType* test_RESOURCE = NULL;

// Prototypes
static ERL_NIF_TERM get_bin_address(ErlNifEnv* env, int argc,
                                          const ERL_NIF_TERM argv[]);

static ERL_NIF_TERM new_array(ErlNifEnv* env, int argc,
                                          const ERL_NIF_TERM argv[]);

static ERL_NIF_TERM size_array(ErlNifEnv* env, int argc,
                                          const ERL_NIF_TERM argv[]);

static ERL_NIF_TERM put_array(ErlNifEnv* env, int argc,
                                          const ERL_NIF_TERM argv[]);

static ERL_NIF_TERM get_array(ErlNifEnv* env, int argc,
                                          const ERL_NIF_TERM argv[]);


static ErlNifFunc nif_funcs[] =
{
    {"get_bin_address", 1, get_bin_address},
    {"new_array", 1, new_array},
    {"size_array", 1, size_array},
    {"put_array", 3, put_array},
    {"get_array", 2, get_array}
};

static ERL_NIF_TERM get_bin_address(ErlNifEnv* env, int argc,
                                          const ERL_NIF_TERM argv[])
{
    ErlNifBinary bin;
    enif_inspect_binary(env, argv[0], &bin);
    char buf[256];
    sprintf(buf, "bin: size=%zu, ptr=%p", bin.size, bin.data);
    return enif_make_string(env, buf, ERL_NIF_LATIN1);
}

static ERL_NIF_TERM new_array(ErlNifEnv* env, int argc,
                                          const ERL_NIF_TERM argv[])
{
    ErlNifBinary bin;
    unsigned long size;
    // unsigned char* data;
    enif_get_ulong(env, argv[0], &size);
    // enif_inspect_binary(env, argv[1], &bin);
    enif_alloc_binary(size * sizeof(long), &bin);

    return enif_make_binary(env, &bin);
}

static ERL_NIF_TERM size_array(ErlNifEnv* env, int argc,
                                          const ERL_NIF_TERM argv[])
{
    ErlNifBinary bin;
    enif_inspect_binary(env, argv[0], &bin);
    return enif_make_int64(env, bin.size);
}

static ERL_NIF_TERM put_array(ErlNifEnv* env, int argc,
                                          const ERL_NIF_TERM argv[])
{
    ErlNifBinary bin;
    unsigned long* array;
    unsigned long pos, value;
    enif_get_ulong(env, argv[0], &pos);
    enif_get_ulong(env, argv[1], &value);
    enif_inspect_binary(env, argv[2], &bin);
    array = (unsigned long*)bin.data;
    array[pos] = value;
    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM get_array(ErlNifEnv* env, int argc,
                                          const ERL_NIF_TERM argv[])
{
    ErlNifBinary bin;
    unsigned long* array;
    unsigned long pos;
    enif_get_ulong(env, argv[0], &pos);
    enif_inspect_binary(env, argv[1], &bin);
    array = (unsigned long*)bin.data;
    return enif_make_int64(env, *(array + pos));
}

static void test_resource_cleanup(ErlNifEnv* env, void* arg)
{
    /* Delete any dynamically allocated memory stored in test_handle */
    /* test_handle* handle = (test_handle*)arg; */
}

static int on_load(ErlNifEnv* env, void** priv_data, ERL_NIF_TERM load_info)
{
    ErlNifResourceFlags flags = ERL_NIF_RT_CREATE | ERL_NIF_RT_TAKEOVER;
    ErlNifResourceType* rt = enif_open_resource_type(env, NULL,
                                                     "test_resource",
                                                     &test_resource_cleanup,
                                                     flags, NULL);
    if (rt == NULL)
        return -1;

    test_RESOURCE = rt;

    return 0;
}

ERL_NIF_INIT(binary_tools, nif_funcs, &on_load, NULL, NULL, NULL);
