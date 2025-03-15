
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <mts/extensibility/string.h>
#include <mts/runtime/string.h>

mts_error_t mts_runtime_c_string_init(const mts_memory_allocator_t *allocator, mts_runtime_c_string_t *str, mts_error_details_handle_t *error_details)
{
    mts_check_argument_for_null(allocator, error_details);
    mts_check_argument_for_null(str, error_details);
    str->data = allocator->allocate(1, allocator->state);
    if (str->data == NULL)
        return mts_error_details_create(error_details, mts_error_fail, "failed to allocate 'mts_runtime_c_string_t' object");
    str->data[0] = '\0';
    str->size = 0;
    str->capacity = 1;
    return mts_error_success;
}

mts_error_t mts_runtime_c_string_fini(const mts_memory_allocator_t *allocator, mts_runtime_c_string_t *str, mts_error_details_handle_t *error_details)
{
    mts_check_argument_for_null(allocator, error_details);
    mts_check_argument_for_null(str, error_details);
    if (str->data != NULL)
    {
        /* Ensure that data and capacity values are consistent */
        if (str->capacity <= 0)
            return mts_error_details_create(error_details, mts_error_fail, "string capacity was zero for allocated data");
        allocator->deallocate(str->data, allocator->state);
        str->data = NULL;
        str->size = 0;
        str->capacity = 0;
    }
    else
    {
        /* Ensure that data, size, and capacity values are consistent */
        if (0 != str->size)
            return mts_error_details_create(error_details, mts_error_fail, "string size was non-zero for deallocated data");
        if (0 != str->capacity)
            return mts_error_details_create(error_details, mts_error_fail, "string capacity was non-zero for deallocated data");
    }
    return mts_error_success;
}

mts_error_t mts_runtime_c_string_assignn(const mts_memory_allocator_t *allocator, mts_runtime_c_string_t *str, const char *value, size_t n, mts_error_details_handle_t *error_details)
{
    mts_check_argument_for_null(allocator, error_details);
    mts_check_argument_for_null(str, error_details);
    /* A NULL value is not valid */
    if (value == NULL)
    {
        return mts_error_false;
    }
    /* Since n + 1 bytes are being allocated n can't be the maximum value */
    if (n == SIZE_MAX)
    {
        return mts_error_false;
    }
    char *data = allocator->reallocate(str->data, n + 1, allocator->state);
    if (data == NULL)
        return mts_error_details_create(error_details, mts_error_fail, "failed to allocate 'mts_runtime_c_string_t' data");
    memcpy(data, value, n);
    data[n] = '\0';
    str->data = data;
    str->size = n;
    str->capacity = n + 1;
    return mts_error_success;
}

mts_error_t mts_runtime_c_string_assign(const mts_memory_allocator_t *allocator, mts_runtime_c_string_t *str, const char *value, mts_error_details_handle_t *error_details)
{
    mts_check_argument_for_null(allocator, error_details);
    if (value == NULL)
    {
        /* strlen is not defined for nullptr, let assignn take care of other bad values */
        return mts_error_false;
    }
    return mts_runtime_c_string_assignn(allocator, str, value, strlen(value), error_details);
}

mts_error_t mts_runtime_c_string_sequence_init(const mts_memory_allocator_t *allocator, mts_runtime_c_string_sequence_t *sequence, size_t size, mts_error_details_handle_t *error_details)
{
    mts_check_argument_for_null(allocator, error_details);
    mts_check_argument_for_null(sequence, error_details);
    mts_runtime_c_string_t *data = NULL;
    mts_error_t error = mts_error_success;
    if (size > 0)
    {
        data = (mts_runtime_c_string_t *)allocator->zero_allocate(size, sizeof(mts_runtime_c_string_t), allocator->state);
        if (data == NULL)
            return mts_error_details_create(error_details, mts_error_fail, "failed to allocate 'mts_runtime_c_string_t' data");
        /* Initialize all sequence elements */
        for (size_t i = 0; i < size; ++i)
        {
            error = mts_runtime_c_string_init(allocator, &data[i], error_details);
            if (error != mts_error_success)
            {
                /* free currently allocated and return false */
                for (; i-- > 0;)
                {
                    mts_runtime_c_string_fini(allocator, &data[i], NULL);
                }
                allocator->deallocate(data, allocator->state);
                return mts_error_details_create(error_details, mts_error_fail, "failed to initialize 'mts_runtime_c_string_t' data");
            }
        }
    }
    sequence->data = data;
    sequence->size = size;
    sequence->capacity = size;
    return error;
}

mts_error_t
mts_runtime_c_string_sequence_init_from_array(const mts_memory_allocator_t *allocator, mts_runtime_c_string_sequence_t *sequence, const char **src, size_t size, mts_error_details_handle_t *error_details)
{
    mts_check_argument_for_null(allocator, error_details);
    mts_check_argument_for_null(sequence, error_details);
    mts_runtime_c_string_t *data = NULL;
    mts_error_t error = mts_error_success;
    if (size > 0)
    {
        data = (mts_runtime_c_string_t *)allocator->zero_allocate(size, sizeof(mts_runtime_c_string_t), allocator->state);
        if (data == NULL)
            return mts_error_details_create(error_details, mts_error_fail, "failed to allocate 'mts_runtime_c_string_t' data");
        /* Initialize all sequence elements */
        for (size_t i = 0; i < size; ++i)
        {
            error = mts_runtime_c_string_assign(allocator, &data[i], src[i], error_details);
            if (error != mts_error_success)
            {
                /* free currently allocated and return false */
                for (; i-- > 0;)
                {
                    mts_runtime_c_string_fini(allocator, &data[i], NULL);
                }
                allocator->deallocate(data, allocator->state);
                return mts_error_details_create(error_details, mts_error_fail, "failed to initialize 'mts_runtime_c_string_t' data");
            }
        }
    }
    sequence->data = data;
    sequence->size = size;
    sequence->capacity = size;
    return error;
}

mts_error_t mts_runtime_c_string_sequence_fini(const mts_memory_allocator_t *allocator, mts_runtime_c_string_sequence_t *sequence, mts_error_details_handle_t *error_details)
{
    mts_check_argument_for_null(allocator, error_details);
    mts_check_argument_for_null(sequence, error_details);
    mts_error_t error = mts_error_success;
    if (sequence->data != NULL)
    {
        /* Ensure that data and capacity values are consistent */
        assert(sequence->capacity > 0);
        /* Finalize all sequence elements */
        for (size_t i = 0; i < sequence->capacity; ++i)
        {
            error = mts_runtime_c_string_fini(allocator, &sequence->data[i], error_details);
        }
        allocator->deallocate(sequence->data, allocator->state);
        sequence->data = NULL;
        sequence->size = 0;
        sequence->capacity = 0;
    }
    else
    {
        /* Ensure that data, size, and capacity values are consistent */
        assert(0 == sequence->size);
        assert(0 == sequence->capacity);
    }
    return error;
}

mts_error_t mts_runtime_c_string_sequence_create(const mts_memory_allocator_t *allocator, size_t size, mts_runtime_c_string_sequence_t **sequence, mts_error_details_handle_t *error_details)
{
    mts_check_argument_for_null(allocator, error_details);
    mts_check_argument_for_null(sequence, error_details);
    *sequence = (mts_runtime_c_string_sequence_t *)allocator->allocate(sizeof(mts_runtime_c_string_sequence_t), allocator->state);
    if (sequence == NULL)
        return mts_error_details_create(error_details, mts_error_fail, "failed to allocate 'mts_runtime_c_string_sequence_t' object");
    mts_error_t error = mts_runtime_c_string_sequence_init(allocator, *sequence, size, error_details);
    if (error != mts_error_success)
    {
        allocator->deallocate(*sequence, allocator->state);
        return error;
    }
    return error;
}

mts_error_t
mts_runtime_c_string_sequence_create_from_array(const mts_memory_allocator_t *allocator, const char **src, size_t size, mts_runtime_c_string_sequence_t **sequence, mts_error_details_handle_t *error_details)
{
    mts_check_argument_for_null(allocator, error_details);
    mts_check_argument_for_null(sequence, error_details);
    *sequence = (mts_runtime_c_string_sequence_t *)allocator->allocate(sizeof(mts_runtime_c_string_sequence_t), allocator->state);
    if (sequence == NULL)
        return mts_error_details_create(error_details, mts_error_fail, "failed to allocate 'mts_runtime_c_string_sequence_t' object");
    mts_error_t error = mts_runtime_c_string_sequence_init_from_array(allocator, *sequence, src, size, error_details);
    if (error != mts_error_success)
    {
        allocator->deallocate(*sequence, allocator->state);
        return error;
    }
    return error;
}

mts_error_t mts_runtime_c_string_sequence_destroy(const mts_memory_allocator_t *allocator, mts_runtime_c_string_sequence_t *sequence, mts_error_details_handle_t *error_details)
{
    mts_check_argument_for_null(allocator, error_details);
    mts_error_t error = mts_error_success;
    if (sequence != NULL)
    {
        error = mts_runtime_c_string_sequence_fini(allocator, sequence, error_details);
    }
    allocator->deallocate(sequence, allocator->state);
    return error;
}

char *
mts_strndup(const char *str, size_t string_length, mts_memory_allocator_t allocator)
{
    if (NULL == str)
    {
        return NULL;
    }
    char *new_string = allocator.allocate(string_length + 1, allocator.state);
    if (NULL == new_string)
    {
        return NULL;
    }
    memcpy(new_string, str, string_length + 1);
    new_string[string_length] = '\0';
    return new_string;
}

char *
mts_strdup(const char *str, mts_memory_allocator_t allocator)
{
    if (NULL == str)
    {
        return NULL;
    }
    return mts_strndup(str, strlen(str), allocator);
}
