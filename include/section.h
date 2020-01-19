/**
 * @file section.h
 * @brief Modify sections.
 * @author Chen Zhenshuo (chenzs108@outlook.com)
 * @version 1.0
 * @date 2020-01-12
 * 
 * @par GitHub
 * https://github.com/czs108/
 */

#pragma once

#include "image.h"
#include "utility/encrypt.h"

#include <windows.h>

/**
 * @brief The encryption information of a section.
 */
typedef struct _ENCRY_INFO
{
    //! The relative virtual address of the section.
    DWORD rva;

    //! The size of the encrypted data.
    DWORD size;

} ENCRY_INFO;


/**
 * @brief Append a new section to the PE image.
 * 
 * @public @memberof _PE_IMAGE_INFO
 * 
 * @param image_info    The PE image.
 * @param name          The name of the new section.
 * @param size          The size of the new section.
 * @param[out] header   The `IMAGE_SECTION_HEADER` structure of the new section.
 * @return `true` if the method succeeds, otherwise `false`.
 */
bool AppendNewSection(
    PE_IMAGE_INFO *const image_info,
    const CHAR *const name,
    const DWORD size,
    IMAGE_SECTION_HEADER *const header);


/**
 * @brief Get the number of sections that can be encrypted.
 * 
 * @public @memberof _PE_IMAGE_INFO
 * 
 * @param image_info    The PE image.
 * @return The number of sections that can be encrypted.
 */
WORD GetEncryptableSectionNumber(
    const PE_IMAGE_INFO *const image_info);


/**
 * @brief Encrypt sections.
 * 
 * @public @memberof _PE_IMAGE_INFO
 * 
 * @param image_info    The PE image.
 * @param encry_info
 * The array where the encryption information will be saved,
 * and its length must be larger than the value returned by `GetNumOfSectionsCanBeEncrypted()` method.
 * Set this to `NULL` to get the required length.
 * @return The number of sections can be encrypted.
 */
WORD EncryptSections(
    const PE_IMAGE_INFO *const image_info,
    ENCRY_INFO *const encry_info);


/**
 * @brief Clear the section name.
 * 
 * @public @memberof _PE_IMAGE_INFO
 * 
 * @param image_info    The PE image.
 */
void ClearSectionNames(
    const PE_IMAGE_INFO *const image_info);