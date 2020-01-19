/**
 * @file section.c
 * @brief Modify sections.
 * @author Chen Zhenshuo (chenzs108@outlook.com)
 * @version 1.0
 * @date 2020-01-12
 * 
 * @par GitHub
 * https://github.com/czs108/
 */

#include "section.h"
#include "utility/error_handling.h"

#include <assert.h>
#include <string.h>
#include <stdbool.h>

/**
 * @brief The callback method, called when enumerating sections.
 * 
 * @private @memberof _PE_IMAGE_INFO
 * 
 * @param image_info    The PE image.
 * @param header        The `IMAGE_SECTION_HEADER` structure.
 * @param arg           The optional custom argument.
 * @return The optional custom value.
 * 
 * @see `EnumSection()`
 * 
 */
typedef DWORD(*LPFN_ENUM_SECTION_CALLBACK)(
    const PE_IMAGE_INFO *const image_info,
    IMAGE_SECTION_HEADER *const header,
    void *const arg);


/******************************************************************************/
// Callback methods
/******************************************************************************/

/**
 * @brief
 * The `::LPFN_ENUM_SECTION_CALLBACK` method,
 * used to clear the section name.
 * 
 * @param image_info    The PE image.
 * @param header        The `IMAGE_SECTION_HEADER` structure.
 * @param arg           Useless.
 * @return Useless.
 */
static DWORD ClearNameCallBack(
    const PE_IMAGE_INFO *const image_info,
    IMAGE_SECTION_HEADER *const header,
    void *const arg);


/**
 * @brief
 * The `::LPFN_ENUM_SECTION_CALLBACK` method,
 * used to get the number of sections that can be encrypted.
 * 
 * @param image_info        The PE image.
 * @param header            The `IMAGE_SECTION_HEADER` structure.
 * @param[in, out] count    The number of sections that can be encrypted.
 * @return Useless.
 */
static DWORD CheckEncryptableCallBack(
    const PE_IMAGE_INFO *const image_info,
    IMAGE_SECTION_HEADER *const header,
    void *const count);


/**
 * @brief
 * The `::LPFN_ENUM_SECTION_CALLBACK` method,
 * used to Encrypt the section.
 * 
 * @param image_info            The PE image.
 * @param header                The `IMAGE_SECTION_HEADER` structure.
 * @param[in, out] encry_info   The space where the encryption information will be saved.
 * @return Useless.
 */
static DWORD EncryCallBack(
    const PE_IMAGE_INFO *const image_info,
    IMAGE_SECTION_HEADER *const header,
    void *const encry_info);

/******************************************************************************/

/**
 * @brief Enumerate sections.
 * 
 * @private @memberof _PE_IMAGE_INFO
 * 
 * @param image_info    The PE image.
 * @param callback
 * The `::LPFN_ENUM_SECTION_CALLBACK` method,
 * it will be called on each `IMAGE_SECTION_HEADER` structure.
 * @param arg           The optional custom argument, it will be passed to `callback`.
 */
static void EnumSections(
    const PE_IMAGE_INFO *const image_info,
    const LPFN_ENUM_SECTION_CALLBACK callback,
    void *const arg);


/**
 * @brief Check if the section can be encrypted.
 * 
 * @param header    The `IMAGE_SECTION_HEADER` structure.
 * @return `true` if the section can be encrypted, otherwise `false`.
 */
static bool IsEncryptable(
    const IMAGE_SECTION_HEADER *const header);


/**
 * @brief Calculate the size of the section, not including the zero at the end.
 * 
 * @private @memberof _PE_IMAGE_INFO
 * 
 * @param image_info    The PE image.
 * @param header        The `IMAGE_SECTION_HEADER` structure.
 * @return The minimum size of the section.
 */
static DWORD CalcMinSize(
    const PE_IMAGE_INFO *const image_info,
    const IMAGE_SECTION_HEADER *const header);


/**
 * @brief Save the encryption information.
 * 
 * @param buffer    The space where the information will be saved.
 * @param rva       The relative virtual address of the section.
 * @param size      The size of the section.
 * @return The new space to save the information.
 */
static ENCRY_INFO *SaveEncryInfo(
    ENCRY_INFO *const buffer,
    const DWORD rva,
    const DWORD size);


bool AppendNewSection(
    PE_IMAGE_INFO *const image_info,
    const CHAR *const name,
    const DWORD size,
    IMAGE_SECTION_HEADER *const header)
{
    assert(image_info != NULL);
    assert(header != NULL);

    ZeroMemory(header, sizeof(IMAGE_SECTION_HEADER));

    const WORD section_num = image_info->nt_header->FileHeader.NumberOfSections;
    if (size == 0)
    {
        return section_num;
    }

    // set the section attribute
    header->Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA
        | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_EXECUTE;

    // set the section name
    if (name != NULL)
    {
        const size_t name_length = strnlen(name, IMAGE_SIZEOF_SHORT_NAME);
        CopyMemory(header->Name, name, name_length);
    }

    // align the section size
    const DWORD file_align = image_info->nt_header->OptionalHeader.FileAlignment;
    const DWORD section_align = image_info->nt_header->OptionalHeader.SectionAlignment;

    const DWORD raw_size = Align(size, file_align);
    const DWORD virtual_size = Align(size, section_align);

    header->SizeOfRawData = raw_size;
    header->Misc.VirtualSize = size;

    // get the size of current headers
    const DWORD headers_size = CalcHeadersSize(image_info->image_base, NULL, NULL);
    const DWORD headers_raw_size = Align(headers_size, file_align);
    const DWORD headers_virtual_size = Align(headers_size, section_align);

    // calculate the new size of headers after appending a section
    const DWORD new_headers_size = headers_size + sizeof(IMAGE_SECTION_HEADER);
    const DWORD new_headers_raw_size = Align(new_headers_size, file_align);
    const DWORD new_headers_virtual_size = Align(new_headers_size, section_align);

    // get the offset of headers
    const DWORD headers_raw_offset = new_headers_raw_size > headers_raw_size ?
        new_headers_raw_size - headers_raw_size : 0;
    const DWORD headers_virtual_offset = new_headers_virtual_size > headers_virtual_size ?
        new_headers_virtual_size - headers_virtual_size : 0;

    // set the address of the section
    const IMAGE_SECTION_HEADER *const last_section_header =
        &image_info->section_header[section_num - 1];
    const DWORD last_section_end = last_section_header->PointerToRawData
        + last_section_header->SizeOfRawData;

    header->VirtualAddress = image_info->image_size + headers_virtual_offset;
    header->PointerToRawData = last_section_end + headers_raw_offset;

    /* adjust the PE_IMAGE_INFO structure */

    // allocate a new image
    const DWORD new_image_size = image_info->image_size
        + virtual_size + headers_virtual_offset;
    BYTE *const new_image_base = (BYTE*)VirtualAlloc(NULL,
        new_image_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (new_image_base == NULL)
    {
        SetLastErrorCode();
        return false;
    }

    // move headers
    CopyMemory(new_image_base, image_info->image_base, headers_raw_size);

    // move sections
    const DWORD first_section_rva = headers_virtual_size;
    CopyMemory(new_image_base + first_section_rva + headers_virtual_offset,
        image_info->image_base + first_section_rva,
        image_info->image_size - first_section_rva);

    image_info->nt_header = (IMAGE_NT_HEADERS*)
        (new_image_base + ((BYTE*)image_info->nt_header - image_info->image_base));
    image_info->section_header = (IMAGE_SECTION_HEADER*)
        (new_image_base + ((BYTE*)image_info->section_header - image_info->image_base));

    VirtualFree(image_info->image_base, 0, MEM_RELEASE);

    image_info->image_base = new_image_base;
    image_info->image_size = new_image_size;
    image_info->nt_header->OptionalHeader.CheckSum = 0;
    image_info->nt_header->OptionalHeader.SizeOfImage = new_image_size;
    image_info->nt_header->OptionalHeader.SizeOfInitializedData += raw_size;
    image_info->nt_header->OptionalHeader.SizeOfHeaders = new_headers_raw_size;

    if (headers_raw_offset > 0 || headers_virtual_offset > 0)
    {
        // adjust the address of sections
        for (WORD i = 0; i != section_num; ++i)
        {
            image_info->section_header[i].PointerToRawData += headers_raw_offset;
            image_info->section_header[i].VirtualAddress += headers_virtual_offset;
        }
    }

    CopyMemory(&image_info->section_header[section_num],
        header, sizeof(IMAGE_SECTION_HEADER));
    ++image_info->nt_header->FileHeader.NumberOfSections;
    return true;
}


WORD GetEncryptableSectionNumber(
    const PE_IMAGE_INFO *const image_info)
{
    assert(image_info != NULL);

    WORD count = 0;
    EnumSections(image_info, &CheckEncryptableCallBack, &count);
    return count;
}


WORD EncryptSections(
    const PE_IMAGE_INFO *const image_info,
    ENCRY_INFO *const encry_info)
{
    assert(image_info != NULL);

    const WORD encry_count = GetEncryptableSectionNumber(image_info);
    if (encry_info != NULL)
    {
        EnumSections(image_info, &EncryCallBack, (void*)&encry_info);
    }

    return encry_count;
}


void ClearSectionNames(
    const PE_IMAGE_INFO *const image_info)
{
    assert(image_info != NULL);

    EnumSections(image_info, &ClearNameCallBack, NULL);
}


void EnumSections(
    const PE_IMAGE_INFO *const image_info,
    const LPFN_ENUM_SECTION_CALLBACK callback,
    void *const arg)
{
    assert(image_info != NULL);
    assert(callback != NULL);

    const WORD section_num = image_info->nt_header->FileHeader.NumberOfSections;
    IMAGE_SECTION_HEADER *const header = image_info->section_header;
    for (WORD i = 0; i != section_num; ++i)
    {
        callback(image_info, header + i, arg);
    }
}


DWORD CalcMinSize(
    const PE_IMAGE_INFO *const image_info,
    const IMAGE_SECTION_HEADER *const header)
{
    assert(image_info != NULL);
    assert(header != NULL);

    DWORD size = header->SizeOfRawData;
    const BYTE *data = RvaToVa(image_info, header->VirtualAddress) + (size - 1);
    while (size > 0 && *data == 0)
    {
        --data;
        --size;
    }

    return size;
}


ENCRY_INFO *SaveEncryInfo(
    ENCRY_INFO *const buffer,
    const DWORD rva,
    const DWORD size)
{
    assert(buffer != NULL);

    buffer->rva = rva;
    buffer->size = size;
    return buffer + 1;
}


DWORD ClearNameCallBack(
    const PE_IMAGE_INFO *const image_info,
    IMAGE_SECTION_HEADER *const header,
    void *const arg)
{
    assert(header != NULL);

    ZeroMemory(header->Name, sizeof(header->Name));
    return 0;
}


DWORD CheckEncryptableCallBack(
    const PE_IMAGE_INFO *const image_info,
    IMAGE_SECTION_HEADER *const header,
    void *const count)
{
    assert(header != NULL);
    assert(count != NULL);

    if (IsEncryptable(header))
    {
        *(WORD*)count += 1;
    }

    return 0;
}


DWORD EncryCallBack(
    const PE_IMAGE_INFO *const image_info,
    IMAGE_SECTION_HEADER *const header,
    void *const encry_info)
{
    assert(image_info != NULL);
    assert(header != NULL);
    assert(encry_info != NULL);

    if (IsEncryptable(header) != true)
    {
        return 0;
    }

    DWORD min_size = CalcMinSize(image_info, header);
    if (min_size > 0)
    {
        // encrypt the section
        const DWORD rva = header->VirtualAddress;
        EncryptData(RvaToVa(image_info, rva), min_size);

        // save the encryption information
        ENCRY_INFO *buffer = *(ENCRY_INFO**)encry_info;
        *(ENCRY_INFO**)encry_info = SaveEncryInfo(buffer, rva, min_size);
        header->Characteristics |= IMAGE_SCN_MEM_WRITE;
    }

    return 0;
}


bool IsEncryptable(
    const IMAGE_SECTION_HEADER *const header)
{
    assert(header != NULL);

    // the sections can be encrypted
    static const CHAR *const names[] =
    {
        ".text", ".data", ".rdata", "CODE", "DATA"
    };

    for (size_t i = 0; i != sizeof(names) / sizeof(names[0]); ++i)
    {
        if (strncmp((CHAR*)header->Name, names[i],
                IMAGE_SIZEOF_SHORT_NAME) == 0)
        {
            return true;
        }
    }

    return false;
}