

sdk_generate_library(rt582_crypto)

sdk_add_include_directories(
    Inc
)

sdk_library_add_sources(
    rt_aes.c
    rt_crypto_util.c
    rt_crypto.c
    rt_ecjpake.c
    rt_hmac_drbg.c
    rt_sha256.c
)

