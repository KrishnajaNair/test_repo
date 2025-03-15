#include <mts/extensibility/string.h>
#include <mts/runtime/error.h>

#ifdef MTS_SDK_WINDOWS
#include <Windows.h>
#include <winerror.h>
#include <strsafe.h>
#endif

#include <stdio.h>
#include <string.h>

typedef struct mts_error_detail_t
{
    mts_runtime_c_string_t description;
    mts_error_t error_code;
} mts_error_detail_t;

void mts_error_detail_fini(mts_error_detail_t *obj)
{
    if (obj == NULL)
    {
        return;
    }
    mts_runtime_c_string_fini(mts_get_default_allocator(), &obj->description, NULL);
}

bool mts_error_detail_init(mts_error_detail_t *obj)
{
    if (obj == NULL)
    {
        return false;
    }
    obj->error_code = 0;
    /* name */
    if (mts_runtime_c_string_init(mts_get_default_allocator(), &obj->description, NULL) != mts_error_success)
    {
        mts_error_detail_fini(obj);
        return false;
    }
    return true;
}

mts_error_t mts_error_details_create(mts_error_details_handle_t *handle, mts_error_t error_code, const char *description)
{
    if (handle != NULL && (*handle) == NULL)
    {
        mts_memory_allocator_t *allocator = mts_get_default_allocator();
        mts_error_detail_t *obj = (mts_error_detail_t *)allocator->allocate(sizeof(mts_error_detail_t), allocator->state);
        if (obj == NULL)
        {
            return mts_error_fail;
        }
        memset(obj, 0, sizeof(mts_error_detail_t));
        bool success = mts_error_detail_init(obj);
        if (!success)
        {
            allocator->deallocate(obj, allocator->state);
            return mts_error_fail;
        }
        obj->error_code = error_code;
        mts_runtime_c_string_assign(allocator, &obj->description, description, NULL);
        *handle = (mts_error_details_handle_t)obj;
        return error_code;
    }
    return error_code;
}

#ifdef MTS_SDK_WINDOWS

/**
 * @brief Converts a WIN32 error code obtained by calling a Win32 API or GetLastError into an MTS error code.
 *
 * @param hr Win32 error code
 * @return MTS error code.
 */
/* NOLINTNEXTLINE(readability-magic-numbers) */
inline mts_error_t error_t_from_win32(uint32_t hr)
{
    return (mts_error_t)(hr) <= 0 ? (mts_error_t)(hr) : (mts_error_t)(((hr)&mts_error_code_mask) | (FACILITY_WIN32 << 16) | mts_error_code_facility_mask);
}

mts_error_t mts_error_details_create_from_win32(mts_error_details_handle_t *handle, /*DWORD*/ uint32_t error_code)
{
    mts_error_t result = error_t_from_win32(error_code);
    if (handle != NULL && (*handle) == NULL)
    {
        LPVOID lpMsgBuf = NULL;

        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            error_code,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf,
            0, NULL);
        if (lpMsgBuf != NULL)
        {
            char *ptr = strchr(lpMsgBuf, '\r');
            if (ptr != NULL)
            { /* Right trim (newline) */
                *ptr = '\0';
            }
            result = mts_error_details_create(handle, result, lpMsgBuf);
            LocalFree(lpMsgBuf);
        }
    }
    return result;
}

#endif /* MTS_SDK_WINDOWS */

mts_error_t mts_error_details_create_from_posix(mts_error_details_handle_t *handle, int posix_errno)
{
    return mts_error_details_create(handle, mts_make_error_code_form_posix_errno(posix_errno), strerror(posix_errno));
}

void mts_error_details_destroy(mts_error_details_handle_t handle)
{
    if (handle != NULL)
    {
        mts_memory_allocator_t *allocator = mts_get_default_allocator();
        mts_error_detail_t *obj = (mts_error_detail_t *)handle;
        /* Finalize members */
        mts_error_detail_fini(obj);
        allocator->deallocate((mts_error_detail_t *)handle, allocator->state);
    }
}

mts_error_t mts_error_details_get_code(mts_error_details_handle_t handle)
{
    if (handle != NULL)
        return ((mts_error_detail_t *)handle)->error_code;
    return mts_error_success;
}

const char *mts_error_details_get_description(mts_error_details_handle_t handle)
{
    if (handle != NULL)
    {
        mts_error_detail_t *obj = (mts_error_detail_t *)handle;
        if (obj->description.size > 1UL)
            return obj->description.data;

        if (obj->error_code == mts_error_success)
            return "the operation completed successfully";

        mts_memory_allocator_t *allocator = mts_get_default_allocator();

#ifdef MTS_SDK_WINDOWS
        if (mts_error_is_hresult(obj->error_code))
        {
            LPVOID lpMsgBuf = NULL;

            FormatMessageA(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                obj->error_code,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR)&lpMsgBuf,
                0, NULL);
            if (lpMsgBuf != NULL)
            {
                char *ptr = strchr(lpMsgBuf, '\r');
                if (ptr != NULL)
                { /* Right trim (newline) */
                    *ptr = '\0';
                }

                mts_runtime_c_string_assign(allocator, &obj->description, lpMsgBuf, NULL);
                LocalFree(lpMsgBuf);
                return obj->description.data;
            }
        }
#endif /* MTS_SDK_WINDOWS */

        if (mts_error_get_domain(obj->error_code) == mts_domain_std_generic_error || mts_error_get_domain(obj->error_code) == mts_domain_std_system_error)
        {
            mts_runtime_c_string_assign(allocator, &obj->description, strerror((int)mts_error_get_error_code(obj->error_code)), NULL);
            return obj->description.data;
        }

        if (mts_error_get_domain(obj->error_code) == mts_domain_std_io_error)
        {
            /* std::io_errc codes do not map to POSIX error codes */
            mts_runtime_c_string_assign(allocator, &obj->description, "iostream error", NULL);
            return obj->description.data;
        }

        char description[256];
        sprintf(description, "unknown error 0x%x", obj->error_code);
        mts_runtime_c_string_assign(allocator, &obj->description, description, NULL);
        return obj->description.data;
    }
    return NULL;
}
