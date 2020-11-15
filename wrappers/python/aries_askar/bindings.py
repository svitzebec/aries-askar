"""Low-level interaction with the aries-askar library."""

import asyncio
import json
import logging
import os
import sys
from ctypes import (
    CDLL,
    CFUNCTYPE,
    byref,
    c_char_p,
    c_int8,
    c_int64,
    c_void_p,
    c_ubyte,
    Structure,
)
from ctypes.util import find_library
from typing import Optional, Sequence, Union

from .error import StoreError, StoreErrorCode
from .types import Entry, EntryOperation, KeyAlg


CALLBACKS = {}
LIB: CDLL = None
LOGGER = logging.getLogger(__name__)


class StoreHandle(c_int64):
    """Index of an active Store instance."""

    def __repr__(self) -> str:
        """Format store handle as a string."""
        return f"{self.__class__.__name__}({self.value})"


class SessionHandle(c_int64):
    """Index of an active Session/Transaction instance."""

    def __repr__(self) -> str:
        """Format session handle as a string."""
        return f"{self.__class__.__name__}({self.value})"


class ScanHandle(c_int64):
    """Index of an active Store scan instance."""

    def __repr__(self) -> str:
        """Format scan handle as a string."""
        return f"{self.__class__.__name__}({self.value})"


class LockHandle(c_int64):
    """Index of an active Lock instance."""

    def __repr__(self) -> str:
        """Format lock handle as a string."""
        return f"{self.__class__.__name__}({self.value})"


class EntrySetHandle(c_int64):
    """Index of an active EntrySet instance."""

    def __repr__(self) -> str:
        """Format entry set handle as a string."""
        return f"{self.__class__.__name__}({self.value})"


class FfiEntry(Structure):
    _fields_ = [
        ("category", c_char_p),
        ("name", c_char_p),
        ("value_len", c_int64),
        ("value", c_void_p),
        ("tags", c_char_p),
    ]

    def decode(self) -> Entry:
        value = memoryview((c_ubyte * self.value_len).from_address(self.value))
        tags = json.loads(decode_str(self.tags)) if self.tags is not None else None
        return Entry(
            decode_str(self.category),
            decode_str(self.name),
            value,
            tags,
        )


class FfiByteBuffer(Structure):
    """A byte buffer allocated by python."""

    _fields_ = [
        ("len", c_int64),
        ("value", c_char_p),
    ]


class lib_buffer(Structure):
    """A byte buffer allocated by the library."""

    _fields_ = [
        ("len", c_int64),
        ("value", c_void_p),
    ]

    def __bytes__(self) -> bytes:
        return bytes((c_ubyte * self.len).from_address(self.value))

    def __repr__(self) -> str:
        """Format byte buffer as a string."""
        return repr(bytes(self))

    def __del__(self):
        """Call the byte buffer destructor when this instance is released."""
        get_library().askar_buffer_free(self)


class lib_string(c_char_p):
    """A string allocated by the library."""

    @classmethod
    def from_param(cls):
        """Returns the type ctypes should use for loading the result."""
        return c_void_p

    def opt_str(self) -> Optional[str]:
        return self.value.decode("utf-8") if self.value is not None else None

    def __bytes__(self):
        """Convert to bytes."""
        return self.value

    def __str__(self):
        """Convert to str."""
        # not allowed to return None
        return self.value.decode("utf-8") if self.value is not None else ""

    def __del__(self):
        """Call the string destructor when this instance is released."""
        get_library().askar_string_free(self)


class lib_unpack_result(Structure):
    _fields_ = [
        ("unpacked", lib_buffer),
        ("recipient", lib_string),
        ("sender", lib_string),
    ]


def get_library() -> CDLL:
    """Return the CDLL instance, loading it if necessary."""
    global LIB
    if LIB is None:
        LIB = _load_library("aries_askar")
        do_call("askar_set_default_logger")
    return LIB


def _load_library(lib_name: str) -> CDLL:
    """Load the CDLL library.
    The python module directory is searched first, followed by the usual
    library resolution for the current system.
    """
    lib_prefix_mapping = {"win32": ""}
    lib_suffix_mapping = {"darwin": ".dylib", "win32": ".dll"}
    try:
        os_name = sys.platform
        lib_prefix = lib_prefix_mapping.get(os_name, "lib")
        lib_suffix = lib_suffix_mapping.get(os_name, ".so")
        lib_path = os.path.join(
            os.path.dirname(__file__), f"{lib_prefix}{lib_name}{lib_suffix}"
        )
        return CDLL(lib_path)
    except KeyError:
        LOGGER.debug("Unknown platform for shared library")
    except OSError:
        LOGGER.warning("Library not loaded from python package")

    lib_path = find_library(lib_name)
    if not lib_path:
        raise StoreError(StoreErrorCode.WRAPPER, f"Error loading library: {lib_name}")
    try:
        return CDLL(lib_path)
    except OSError as e:
        raise StoreError(
            StoreErrorCode.WRAPPER, f"Error loading library: {lib_name}"
        ) from e


def _fulfill_future(fut: asyncio.Future, result, err: Exception = None):
    """Resolve a callback future given the result and exception, if any."""
    if fut.cancelled():
        LOGGER.debug("callback previously cancelled")
    elif err:
        fut.set_exception(err)
    else:
        fut.set_result(result)


def _create_callback(cb_type: CFUNCTYPE, fut: asyncio.Future, post_process=None):
    """Create a callback to handle the response from an async library method."""

    def _cb(id: int, err: int, result=None):
        """Callback function passed to the CFUNCTYPE for invocation."""
        if post_process:
            result = post_process(result)
        exc = get_current_error() if err else None
        try:
            (loop, _cb) = CALLBACKS.pop(fut)
        except KeyError:
            LOGGER.debug("callback already fulfilled")
            return
        loop.call_soon_threadsafe(lambda: _fulfill_future(fut, result, exc))

    res = cb_type(_cb)
    return res


def do_call(fn_name, *args):
    """Perform a synchronous library function call."""
    lib_fn = getattr(get_library(), fn_name)
    result = lib_fn(*args)
    if result:
        raise get_current_error(True)


def do_call_async(
    fn_name, *args, return_type=None, post_process=None
) -> asyncio.Future:
    """Perform an asynchronous library function call."""
    lib_fn = getattr(get_library(), fn_name)
    loop = asyncio.get_event_loop()
    fut = loop.create_future()
    cf_args = [None, c_int64, c_int64]
    if return_type:
        cf_args.append(return_type)
    cb_type = CFUNCTYPE(*cf_args)  # could be cached
    cb_res = _create_callback(cb_type, fut, post_process)
    # keep a reference to the callback function to avoid it being freed
    CALLBACKS[fut] = (loop, cb_res)
    result = lib_fn(*args, cb_res, c_int64(0))  # not making use of callback ID
    if result:
        # callback will not be executed
        if CALLBACKS.pop(fut):
            fut.set_exception(get_current_error())
    return fut


def decode_str(value: c_char_p) -> str:
    return value.decode("utf-8")


def encode_str(arg: Optional[Union[str, bytes, memoryview]]) -> c_char_p:
    """Encode an optional input argument as a string.
    Returns: None if the argument is None, otherwise the value encoded utf-8.
    """
    if arg is None:
        return None
    if isinstance(arg, str):
        return c_char_p(arg.encode("utf-8"))
    return c_char_p(arg)


def encode_bytes(arg: Optional[Union[str, bytes, memoryview]]) -> FfiByteBuffer:
    buf = FfiByteBuffer()
    if arg is not None:
        if isinstance(arg, str):
            arg = arg.encode("utf-8")
        buf.len = len(arg)
        buf.value = c_char_p(arg)
    return buf


def get_current_error(expect: bool = False) -> StoreError:
    """Get the error result from the previous failed API method.
    Args:
        expect: Return a default error message if none is found
    """
    err_json = lib_string()
    if not get_library().askar_get_current_error(byref(err_json)):
        try:
            msg = json.loads(err_json.value)
        except json.JSONDecodeError:
            LOGGER.warning("JSON decode error for askar_get_current_error")
            msg = None
        if msg and "message" in msg and "code" in msg:
            return StoreError(
                StoreErrorCode(msg["code"]), msg["message"], msg.get("extra")
            )
        if not expect:
            return None
    return StoreError(StoreErrorCode.WRAPPER, "Unknown error")


def derive_verkey(key_alg: KeyAlg, seed: Union[str, bytes, memoryview]) -> str:
    verkey = lib_string()
    do_call(
        "askar_derive_verkey",
        encode_str(key_alg.value),
        encode_bytes(seed),
        byref(verkey),
    )
    return str(verkey)


def generate_raw_key(seed: str = None) -> str:
    """Generate a new raw store wrapping key."""
    result = lib_string()
    do_call("askar_generate_raw_key", encode_str(seed), byref(result))
    return str(result)


def verify_signature(
    signer_vk: str,
    message: Union[str, bytes, memoryview],
    signature: Union[str, bytes, memoryview],
) -> bool:
    result = c_int8()
    do_call(
        "askar_verify_signature",
        encode_str(signer_vk),
        encode_bytes(message),
        encode_bytes(signature),
        byref(result),
    )
    return result.value != 0


def version() -> str:
    """Set the version of the installed aries-askar library."""
    lib = get_library()
    lib.askar_version.restype = c_void_p
    return str(lib_string(lib.askar_version()))


async def store_open(
    uri: str, wrap_method: str = None, pass_key: str = None, profile: str = None
) -> StoreHandle:
    """Open an existing Store and return the open handle."""
    return await do_call_async(
        "askar_store_open",
        encode_str(uri),
        encode_str(wrap_method and wrap_method.lower()),
        encode_str(pass_key),
        encode_str(profile),
        return_type=StoreHandle,
    )


async def store_provision(
    uri: str,
    wrap_method: str = None,
    pass_key: str = None,
    profile: str = None,
    recreate: bool = False,
) -> StoreHandle:
    """Provision a new Store and return the open handle."""
    return await do_call_async(
        "askar_store_provision",
        encode_str(uri),
        encode_str(wrap_method and wrap_method.lower()),
        encode_str(pass_key),
        encode_str(profile),
        c_int8(recreate),
        return_type=StoreHandle,
    )


async def store_create_profile(handle: StoreHandle, name: str = None) -> str:
    """Create a new profile in a Store."""
    return str(
        await do_call_async(
            "askar_store_create_profile",
            handle,
            encode_str(name),
            return_type=lib_string,
        )
    )


async def store_remove_profile(handle: StoreHandle, name: str) -> bool:
    """Remove an existing profile from a Store."""
    return (
        await do_call_async(
            "askar_store_remove_profile",
            handle,
            encode_str(name),
            return_type=c_int8,
        )
        != 0
    )


async def store_rekey(
    handle: StoreHandle,
    wrap_method: str = None,
    pass_key: str = None,
) -> StoreHandle:
    """Replace the wrap key on a Store."""
    return await do_call_async(
        "askar_store_rekey",
        handle,
        encode_str(wrap_method and wrap_method.lower()),
        encode_str(pass_key),
    )


async def store_remove(uri: str) -> bool:
    """Remove an existing Store, if any."""
    return (
        await do_call_async(
            "askar_store_remove",
            encode_str(uri),
            return_type=c_int8,
        )
        != 0
    )


async def store_close(handle: StoreHandle):
    """Close an opened store instance."""
    return await do_call_async("askar_store_close", handle)


def store_close_immed(handle: StoreHandle):
    """Close an opened store instance."""
    do_call("askar_store_close", handle, c_void_p(0), c_int64(0))


async def session_start(
    handle: StoreHandle, profile: Optional[str] = None, as_transaction: bool = False
) -> SessionHandle:
    """Start a new session with an open Store."""
    return await do_call_async(
        "askar_session_start",
        handle,
        encode_str(profile),
        c_int8(as_transaction),
        return_type=SessionHandle,
    )


async def session_close(handle: SessionHandle, commit: bool = False):
    """Close an existing Session."""
    return await do_call_async(
        "askar_session_close",
        handle,
        c_int8(commit),
    )


def session_close_immed(handle: SessionHandle):
    """Close an existing Session."""
    do_call("askar_session_close", handle, c_int8(0), c_int64(0))


async def session_count(
    handle: SessionHandle, category: str, tag_filter: Union[str, dict] = None
) -> int:
    """Count rows in the Store."""
    category = encode_str(category)
    if isinstance(tag_filter, dict):
        tag_filter = json.dumps(tag_filter)
    tag_filter = encode_str(tag_filter)
    return int(
        await do_call_async(
            "askar_session_count", handle, category, tag_filter, return_type=c_int64
        )
    )


async def session_fetch(
    handle: SessionHandle, category: str, name: str, for_update: bool = False
) -> EntrySetHandle:
    """Fetch a row from the Store."""
    category = encode_str(category)
    name = encode_str(name)
    return await do_call_async(
        "askar_session_fetch",
        handle,
        category,
        name,
        c_int8(for_update),
        return_type=EntrySetHandle,
    )


async def session_fetch_all(
    handle: SessionHandle,
    category: str,
    tag_filter: Union[str, dict] = None,
    limit: int = None,
    for_update: bool = False,
) -> EntrySetHandle:
    """Fetch all matching rows in the Store."""
    category = encode_str(category)
    if isinstance(tag_filter, dict):
        tag_filter = json.dumps(tag_filter)
    tag_filter = encode_str(tag_filter)
    return await do_call_async(
        "askar_session_fetch_all",
        handle,
        category,
        tag_filter,
        c_int64(limit if limit is not None else -1),
        c_int8(for_update),
        return_type=EntrySetHandle,
    )


async def session_remove_all(
    handle: SessionHandle,
    category: str,
    tag_filter: Union[str, dict] = None,
) -> int:
    """Remove all matching rows in the Store."""
    category = encode_str(category)
    if isinstance(tag_filter, dict):
        tag_filter = json.dumps(tag_filter)
    tag_filter = encode_str(tag_filter)
    return int(
        await do_call_async(
            "askar_session_remove_all",
            handle,
            category,
            tag_filter,
            return_type=c_int64,
        )
    )


async def session_update(
    handle: SessionHandle,
    operation: EntryOperation,
    category: str,
    name: str,
    value: Union[str, bytes, memoryview] = None,
    tags: dict = None,
    expiry_ms: Optional[int] = None,
):
    """Update a Store by inserting, updating, or removing a record."""

    return await do_call_async(
        "askar_session_update",
        handle,
        c_int8(operation.value),
        encode_str(category),
        encode_str(name),
        encode_bytes(value),
        encode_str(None if tags is None else json.dumps(tags)),
        c_int64(-1 if expiry_ms is None else expiry_ms),
    )


async def session_create_keypair(
    handle: SessionHandle,
    alg: str,
    metadata: str = None,
    tags: dict = None,
    seed: Union[str, bytes, memoryview] = None,
) -> str:
    return str(
        await do_call_async(
            "askar_session_create_keypair",
            handle,
            encode_str(alg),
            encode_str(metadata),
            encode_str(None if tags is None else json.dumps(tags)),
            encode_bytes(seed),
            return_type=lib_string,
        )
    )


async def session_fetch_keypair(
    handle: SessionHandle, ident: str, for_update: bool = False
) -> Optional[EntrySetHandle]:
    ptr = await do_call_async(
        "askar_session_fetch_keypair",
        handle,
        encode_str(ident),
        c_int8(for_update),
        return_type=c_void_p,
    )
    if ptr:
        return EntrySetHandle(ptr)


async def session_update_keypair(
    handle: SessionHandle, ident: str, metadata: str = None, tags: dict = None
):
    await do_call_async(
        "askar_session_update_keypair",
        handle,
        encode_str(ident),
        encode_str(metadata),
        encode_str(None if tags is None else json.dumps(tags)),
    )


async def session_sign_message(
    handle: SessionHandle,
    key_ident: str,
    message: Union[str, bytes, memoryview],
) -> lib_buffer:
    return await do_call_async(
        "askar_session_sign_message",
        handle,
        encode_str(key_ident),
        encode_bytes(message),
        return_type=lib_buffer,
    )


async def session_pack_message(
    handle: SessionHandle,
    recipient_vks: Sequence[str],
    from_key_ident: Optional[str],
    message: Union[str, bytes, memoryview],
) -> lib_buffer:
    recipient_vks = encode_str(",".join(recipient_vks))
    from_key_ident = encode_str(from_key_ident)
    message = encode_bytes(message)
    return await do_call_async(
        "askar_session_pack_message",
        handle,
        recipient_vks,
        from_key_ident,
        message,
        return_type=lib_buffer,
    )


async def session_unpack_message(
    handle: SessionHandle,
    message: Union[str, bytes, memoryview],
) -> (lib_buffer, str, Optional[str]):
    message = encode_bytes(message)
    result = await do_call_async(
        "askar_session_unpack_message", handle, message, return_type=lib_unpack_result
    )
    return (result.unpacked, str(result.recipient), result.sender.opt_str())


async def scan_start(
    handle: StoreHandle,
    profile: Optional[str],
    category: str,
    tag_filter: Union[str, dict] = None,
    offset: int = None,
    limit: int = None,
) -> ScanHandle:
    """Create a new Scan against the Store."""
    if isinstance(tag_filter, dict):
        tag_filter = json.dumps(tag_filter)
    tag_filter = encode_str(tag_filter)
    return await do_call_async(
        "askar_scan_start",
        handle,
        encode_str(profile),
        encode_str(category),
        tag_filter,
        c_int64(offset or 0),
        c_int64(limit if limit is not None else -1),
        return_type=ScanHandle,
    )


async def scan_next(handle: StoreHandle) -> Optional[EntrySetHandle]:
    handle = await do_call_async("askar_scan_next", handle, return_type=EntrySetHandle)
    return handle or None


def scan_free(handle: ScanHandle):
    """Free an active store scan."""
    do_call("askar_scan_free", handle)


def entry_set_next(handle: EntrySetHandle) -> Optional[Entry]:
    ffi_entry = FfiEntry()
    found = c_int8(0)
    do_call("askar_entry_set_next", handle, byref(ffi_entry), byref(found))
    if found:
        return ffi_entry.decode()
    return None


def entry_set_free(handle: EntrySetHandle):
    get_library().askar_entry_set_free(handle)
