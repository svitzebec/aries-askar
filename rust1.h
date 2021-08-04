#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define PAGE_SIZE 32

enum ErrorCode {
  Success = 0,
  Backend = 1,
  Busy = 2,
  Duplicate = 3,
  Encryption = 4,
  Input = 5,
  NotFound = 6,
  Unexpected = 7,
  Unsupported = 8,
};
typedef uintptr_t ErrorCode;

typedef struct ByteBuffer ByteBuffer;

typedef struct FfiStr FfiStr;

typedef struct Option_EnabledCallback Option_EnabledCallback;

typedef struct Option_FlushCallback Option_FlushCallback;

typedef uintptr_t ArcHandle_LocalKey;

typedef ArcHandle_LocalKey LocalKeyHandle;

typedef struct SecretBuffer {
  int64_t len;
  uint8_t *data;
} SecretBuffer;

typedef struct AeadParams {
  int32_t nonce_length;
  int32_t tag_length;
} AeadParams;

typedef struct EncryptedBuffer {
  struct SecretBuffer buffer;
  int64_t tag_pos;
  int64_t nonce_pos;
} EncryptedBuffer;

typedef void (*LogCallback)(const void *context, int32_t level, const char *target, const char *message, const char *module_path, const char *file, int32_t line);

typedef uintptr_t ArcHandle_FfiEntryList;

typedef ArcHandle_FfiEntryList EntryListHandle;

typedef uintptr_t ArcHandle_FfiKeyEntryList;

typedef ArcHandle_FfiKeyEntryList KeyEntryListHandle;

typedef int64_t CallbackId;

char *askar_version(void);

ErrorCode askar_get_current_error(const char **error_json_p);

ErrorCode askar_key_generate(struct FfiStr alg, int8_t ephemeral, LocalKeyHandle *out);

ErrorCode askar_key_from_seed(struct FfiStr alg,
                              struct ByteBuffer seed,
                              struct FfiStr method,
                              LocalKeyHandle *out);

ErrorCode askar_key_from_jwk(struct ByteBuffer jwk, LocalKeyHandle *out);

ErrorCode askar_key_from_public_bytes(struct FfiStr alg,
                                      struct ByteBuffer public_,
                                      LocalKeyHandle *out);

ErrorCode askar_key_get_public_bytes(LocalKeyHandle handle, struct SecretBuffer *out);

ErrorCode askar_key_from_secret_bytes(struct FfiStr alg,
                                      struct ByteBuffer secret,
                                      LocalKeyHandle *out);

ErrorCode askar_key_get_secret_bytes(LocalKeyHandle handle, struct SecretBuffer *out);

ErrorCode askar_key_convert(LocalKeyHandle handle, struct FfiStr alg, LocalKeyHandle *out);

ErrorCode askar_key_from_key_exchange(struct FfiStr alg,
                                      LocalKeyHandle sk_handle,
                                      LocalKeyHandle pk_handle,
                                      LocalKeyHandle *out);

void askar_key_free(LocalKeyHandle handle);

ErrorCode askar_key_get_algorithm(LocalKeyHandle handle, const char **out);

ErrorCode askar_key_get_ephemeral(LocalKeyHandle handle, int8_t *out);

ErrorCode askar_key_get_jwk_public(LocalKeyHandle handle, struct FfiStr alg, const char **out);

ErrorCode askar_key_get_jwk_secret(LocalKeyHandle handle, struct SecretBuffer *out);

ErrorCode askar_key_get_jwk_thumbprint(LocalKeyHandle handle, struct FfiStr alg, const char **out);

ErrorCode askar_key_aead_random_nonce(LocalKeyHandle handle, struct SecretBuffer *out);

ErrorCode askar_key_aead_get_params(LocalKeyHandle handle, struct AeadParams *out);

ErrorCode askar_key_aead_get_padding(LocalKeyHandle handle, int64_t msg_len, int32_t *out);

ErrorCode askar_key_aead_encrypt(LocalKeyHandle handle,
                                 struct ByteBuffer message,
                                 struct ByteBuffer nonce,
                                 struct ByteBuffer aad,
                                 struct EncryptedBuffer *out);

ErrorCode askar_key_aead_decrypt(LocalKeyHandle handle,
                                 struct ByteBuffer ciphertext,
                                 struct ByteBuffer nonce,
                                 struct ByteBuffer tag,
                                 struct ByteBuffer aad,
                                 struct SecretBuffer *out);

ErrorCode askar_key_sign_message(LocalKeyHandle handle,
                                 struct ByteBuffer message,
                                 struct FfiStr sig_type,
                                 struct SecretBuffer *out);

ErrorCode askar_key_verify_signature(LocalKeyHandle handle,
                                     struct ByteBuffer message,
                                     struct ByteBuffer signature,
                                     struct FfiStr sig_type,
                                     int8_t *out);

ErrorCode askar_key_wrap_key(LocalKeyHandle handle,
                             LocalKeyHandle other,
                             struct ByteBuffer nonce,
                             struct EncryptedBuffer *out);

ErrorCode askar_key_unwrap_key(LocalKeyHandle handle,
                               struct FfiStr alg,
                               struct ByteBuffer ciphertext,
                               struct ByteBuffer nonce,
                               struct ByteBuffer tag,
                               LocalKeyHandle *out);

ErrorCode askar_key_crypto_box_random_nonce(struct SecretBuffer *out);

ErrorCode askar_key_crypto_box(LocalKeyHandle recip_key,
                               LocalKeyHandle sender_key,
                               struct ByteBuffer message,
                               struct ByteBuffer nonce,
                               struct SecretBuffer *out);

ErrorCode askar_key_crypto_box_open(LocalKeyHandle recip_key,
                                    LocalKeyHandle sender_key,
                                    struct ByteBuffer message,
                                    struct ByteBuffer nonce,
                                    struct SecretBuffer *out);

ErrorCode askar_key_crypto_box_seal(LocalKeyHandle handle,
                                    struct ByteBuffer message,
                                    struct SecretBuffer *out);

ErrorCode askar_key_crypto_box_seal_open(LocalKeyHandle handle,
                                         struct ByteBuffer ciphertext,
                                         struct SecretBuffer *out);

ErrorCode askar_key_derive_ecdh_es(struct FfiStr alg,
                                   LocalKeyHandle ephem_key,
                                   LocalKeyHandle recip_key,
                                   struct ByteBuffer alg_id,
                                   struct ByteBuffer apu,
                                   struct ByteBuffer apv,
                                   int8_t receive,
                                   LocalKeyHandle *out);

ErrorCode askar_key_derive_ecdh_1pu(struct FfiStr alg,
                                    LocalKeyHandle ephem_key,
                                    LocalKeyHandle sender_key,
                                    LocalKeyHandle recip_key,
                                    struct ByteBuffer alg_id,
                                    struct ByteBuffer apu,
                                    struct ByteBuffer apv,
                                    struct ByteBuffer cc_tag,
                                    int8_t receive,
                                    LocalKeyHandle *out);

ErrorCode askar_set_custom_logger(const void *context,
                                  LogCallback log,
                                  struct Option_EnabledCallback enabled,
                                  struct Option_FlushCallback flush,
                                  int32_t max_level);

ErrorCode askar_set_default_logger(void);

ErrorCode askar_set_max_log_level(int32_t max_level);

ErrorCode askar_entry_list_count(EntryListHandle handle, int32_t *count);

ErrorCode askar_entry_list_get_category(EntryListHandle handle,
                                        int32_t index,
                                        const char **category);

ErrorCode askar_entry_list_get_name(EntryListHandle handle, int32_t index, const char **name);

ErrorCode askar_entry_list_get_value(EntryListHandle handle,
                                     int32_t index,
                                     struct SecretBuffer *value);

ErrorCode askar_entry_list_get_tags(EntryListHandle handle, int32_t index, const char **tags);

void askar_entry_list_free(EntryListHandle handle);

ErrorCode askar_key_entry_list_count(KeyEntryListHandle handle, int32_t *count);

void askar_key_entry_list_free(KeyEntryListHandle handle);

ErrorCode askar_key_entry_list_get_algorithm(KeyEntryListHandle handle,
                                             int32_t index,
                                             const char **alg);

ErrorCode askar_key_entry_list_get_name(KeyEntryListHandle handle,
                                        int32_t index,
                                        const char **name);

ErrorCode askar_key_entry_list_get_metadata(KeyEntryListHandle handle,
                                            int32_t index,
                                            const char **metadata);

ErrorCode askar_key_entry_list_get_tags(KeyEntryListHandle handle,
                                        int32_t index,
                                        const char **tags);

ErrorCode askar_key_entry_list_load_local(KeyEntryListHandle handle,
                                          int32_t index,
                                          LocalKeyHandle *out);

void askar_buffer_free(struct SecretBuffer buffer);

ErrorCode askar_store_generate_raw_key(struct ByteBuffer seed, const char **out);

ErrorCode askar_store_provision(struct FfiStr spec_uri,
                                struct FfiStr key_method,
                                struct FfiStr pass_key,
                                struct FfiStr profile,
                                int8_t recreate,
                                void (*cb)(CallbackId cb_id, ErrorCode err, StoreHandle handle),
                                CallbackId cb_id);

ErrorCode askar_store_open(struct FfiStr spec_uri,
                           struct FfiStr key_method,
                           struct FfiStr pass_key,
                           struct FfiStr profile,
                           void (*cb)(CallbackId cb_id, ErrorCode err, StoreHandle handle),
                           CallbackId cb_id);

ErrorCode askar_store_remove(struct FfiStr spec_uri,
                             void (*cb)(CallbackId cb_id, ErrorCode err, int8_t),
                             CallbackId cb_id);

ErrorCode askar_store_create_profile(StoreHandle handle,
                                     struct FfiStr profile,
                                     void (*cb)(CallbackId cb_id, ErrorCode err, const char *result_p),
                                     CallbackId cb_id);

ErrorCode askar_store_get_profile_name(StoreHandle handle,
                                       void (*cb)(CallbackId cb_id, ErrorCode err, const char *name),
                                       CallbackId cb_id);

ErrorCode askar_store_remove_profile(StoreHandle handle,
                                     struct FfiStr profile,
                                     void (*cb)(CallbackId cb_id, ErrorCode err, int8_t removed),
                                     CallbackId cb_id);

ErrorCode askar_store_rekey(StoreHandle handle,
                            struct FfiStr key_method,
                            struct FfiStr pass_key,
                            void (*cb)(CallbackId cb_id, ErrorCode err),
                            CallbackId cb_id);

ErrorCode askar_store_close(StoreHandle handle,
                            void (*cb)(CallbackId cb_id, ErrorCode err),
                            CallbackId cb_id);

ErrorCode askar_scan_start(StoreHandle handle,
                           struct FfiStr profile,
                           struct FfiStr category,
                           struct FfiStr tag_filter,
                           int64_t offset,
                           int64_t limit,
                           void (*cb)(CallbackId cb_id, ErrorCode err, ScanHandle handle),
                           CallbackId cb_id);

ErrorCode askar_scan_next(ScanHandle handle,
                          void (*cb)(CallbackId cb_id, ErrorCode err, EntryListHandle results),
                          CallbackId cb_id);

ErrorCode askar_scan_free(ScanHandle handle);

ErrorCode askar_session_start(StoreHandle handle,
                              struct FfiStr profile,
                              int8_t as_transaction,
                              void (*cb)(CallbackId cb_id, ErrorCode err, SessionHandle handle),
                              CallbackId cb_id);

ErrorCode askar_session_count(SessionHandle handle,
                              struct FfiStr category,
                              struct FfiStr tag_filter,
                              void (*cb)(CallbackId cb_id, ErrorCode err, int64_t count),
                              CallbackId cb_id);

ErrorCode askar_session_fetch(SessionHandle handle,
                              struct FfiStr category,
                              struct FfiStr name,
                              int8_t for_update,
                              void (*cb)(CallbackId cb_id, ErrorCode err, EntryListHandle results),
                              CallbackId cb_id);

ErrorCode askar_session_fetch_all(SessionHandle handle,
                                  struct FfiStr category,
                                  struct FfiStr tag_filter,
                                  int64_t limit,
                                  int8_t for_update,
                                  void (*cb)(CallbackId cb_id, ErrorCode err, EntryListHandle results),
                                  CallbackId cb_id);

ErrorCode askar_session_remove_all(SessionHandle handle,
                                   struct FfiStr category,
                                   struct FfiStr tag_filter,
                                   void (*cb)(CallbackId cb_id, ErrorCode err, int64_t removed),
                                   CallbackId cb_id);

ErrorCode askar_session_update(SessionHandle handle,
                               int8_t operation,
                               struct FfiStr category,
                               struct FfiStr name,
                               struct ByteBuffer value,
                               struct FfiStr tags,
                               int64_t expiry_ms,
                               void (*cb)(CallbackId cb_id, ErrorCode err),
                               CallbackId cb_id);

ErrorCode askar_session_insert_key(SessionHandle handle,
                                   LocalKeyHandle key_handle,
                                   struct FfiStr name,
                                   struct FfiStr metadata,
                                   struct FfiStr tags,
                                   int64_t expiry_ms,
                                   void (*cb)(CallbackId cb_id, ErrorCode err),
                                   CallbackId cb_id);

ErrorCode askar_session_fetch_key(SessionHandle handle,
                                  struct FfiStr name,
                                  int8_t for_update,
                                  void (*cb)(CallbackId cb_id, ErrorCode err, KeyEntryListHandle results),
                                  CallbackId cb_id);

ErrorCode askar_session_fetch_all_keys(SessionHandle handle,
                                       struct FfiStr alg,
                                       struct FfiStr thumbprint,
                                       struct FfiStr tag_filter,
                                       int64_t limit,
                                       int8_t for_update,
                                       void (*cb)(CallbackId cb_id, ErrorCode err, KeyEntryListHandle results),
                                       CallbackId cb_id);

ErrorCode askar_session_update_key(SessionHandle handle,
                                   struct FfiStr name,
                                   struct FfiStr metadata,
                                   struct FfiStr tags,
                                   int64_t expiry_ms,
                                   void (*cb)(CallbackId cb_id, ErrorCode err),
                                   CallbackId cb_id);

ErrorCode askar_session_remove_key(SessionHandle handle,
                                   struct FfiStr name,
                                   void (*cb)(CallbackId cb_id, ErrorCode err),
                                   CallbackId cb_id);

ErrorCode askar_session_close(SessionHandle handle,
                              int8_t commit,
                              void (*cb)(CallbackId cb_id, ErrorCode err),
                              CallbackId cb_id);
