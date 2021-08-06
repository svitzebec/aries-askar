#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define PAGE_SIZE 32

//enum ErrorCode {
//  Success = 0,
//  Backend = 1,
//  Busy = 2,
//  Duplicate = 3,
//  Encryption = 4,
//  Input = 5,
//  NotFound = 6,
//  Unexpected = 7,
//  Unsupported = 8,
//};

typedef uintptr_t ErrorCode;

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

ErrorCode askar_key_get_secret_bytes(LocalKeyHandle handle, struct SecretBuffer *out);

void askar_key_free(LocalKeyHandle handle);

ErrorCode askar_key_get_algorithm(LocalKeyHandle handle, const char **out);

ErrorCode askar_key_get_ephemeral(LocalKeyHandle handle, int8_t *out);

ErrorCode askar_key_aead_random_nonce(LocalKeyHandle handle, struct SecretBuffer *out);

ErrorCode askar_key_aead_get_params(LocalKeyHandle handle, struct AeadParams *out);

ErrorCode askar_key_aead_get_padding(LocalKeyHandle handle, int64_t msg_len, int32_t *out);

ErrorCode askar_key_crypto_box_random_nonce(struct SecretBuffer *out);

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

// TODO: - We need to find a way to generate this headers methods with FfiStr and ByteBuffer which are included in the rust standard library and are not custom implemented.

//ErrorCode askar_session_fetch(SessionHandle handle,
//                              struct FfiStr category,
//                              struct FfiStr name,
//                              int8_t for_update,
//                              void (*cb)(CallbackId cb_id, ErrorCode err, EntryListHandle results),
//                              CallbackId cb_id);
//
//ErrorCode askar_session_update(SessionHandle handle,
//                               int8_t operation,
//                               struct FfiStr category,
//                               struct FfiStr name,
//                               struct ByteBuffer value,
//                               struct FfiStr tags,
//                               int64_t expiry_ms,
//                               void (*cb)(CallbackId cb_id, ErrorCode err),
//                               CallbackId cb_id);
