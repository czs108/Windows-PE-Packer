/**
 * @file extra_data.h
 * @brief Process the extra data behind the PE image.
 * @author Chen Zhenshuo (chenzs108@outlook.com)
 * @version 1.0
 * @date 2020-01-12
 * 
 * @par GitHub
 * https://github.com/czs108/
 */

#pragma once

#include <windows.h>

#include <stdbool.h>

/**
 * @brief The view of the extra data.
 * 
 * @see `LoadPeImage()`
 */
typedef struct _EXTRA_DATA_VIEW
{
    //! The base address of the data.
    const BYTE* base;

    //! The size of the data.
    DWORD size;

} EXTRA_DATA_VIEW;


/**
 * @brief Write the extra data to the file.
 * 
 * @public @memberof _EXTRA_DATA_VIEW
 * 
 * @param data  The extra data.
 * @param file  The file.
 * @return `true` if the method succeeds, otherwise `false`.
 */
bool WriteExtraDataToFile(
    const EXTRA_DATA_VIEW *const data,
    const HANDLE file);