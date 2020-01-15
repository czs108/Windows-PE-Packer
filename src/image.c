/**
 * @file image.c
 * @brief basic PE image methods.
 * 
 * @author Chen Zhenshuo (chenzs108@outlook.com)
 * @version 1.0
 * @date 2020-01-07
 * 
 * @par GitHub
 * https://github.com/czs108/
 */

#include "image.h"
#include "utility/error_handling.h"
#include "utility/file_access.h"

#include <assert.h>

/**
 * @brief Get the base address of `IMAGE_NT_HEADERS` structure.
 * 
 * @param file_base The base address of the file content.
 * @return The base address of `IMAGE_NT_HEADERS` structure, `NULL` if it is not a PE file.
 */
static IMAGE_NT_HEADERS *GetNtHeader(
    const BYTE *const file_base);


bool LoadPeImage(
    const BYTE *const file_base,
    const DWORD file_size,
    PE_IMAGE_INFO *const image_info,
    EXTRA_DATA_VIEW *const extra_data)
{
    assert(IsPeFile(file_base));
    assert(image_info != NULL);
    assert(extra_data != NULL);

    bool success = false;
    ZeroMemory(image_info, sizeof(PE_IMAGE_INFO));
    ZeroMemory(extra_data, sizeof(EXTRA_DATA_VIEW));

    // get the size of the PE image and allocate memory
    const IMAGE_NT_HEADERS *const nt_header = GetNtHeader(file_base);
    image_info->image_size = nt_header->OptionalHeader.SizeOfImage;
    image_info->image_base = (BYTE*)VirtualAlloc(NULL, image_info->image_size,
        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (image_info->image_base == NULL)
    {
        SetLastErrorCode();
        goto _Exit;
    }

    // copy the PE headers
    DWORD dos_header_size = 0, nt_header_size = 0;
    const DWORD headers_size = CalcHeadersSize(file_base, &dos_header_size, &nt_header_size);
    CopyMemory(image_info->image_base, file_base, headers_size);

    image_info->nt_header = (IMAGE_NT_HEADERS*)(image_info->image_base + dos_header_size);
    image_info->section_header = (IMAGE_SECTION_HEADER*)((BYTE*)image_info->nt_header + nt_header_size);

    // map the section data to memory
    const WORD section_num = image_info->nt_header->FileHeader.NumberOfSections;
    const IMAGE_SECTION_HEADER *const section_header = (IMAGE_SECTION_HEADER*)
        (file_base + dos_header_size + nt_header_size);
    for (WORD i = 0; i != section_num; ++i)
    {
        if (section_header[i].SizeOfRawData == 0)
        {
            continue;
        }

        const BYTE *const src = file_base + section_header[i].PointerToRawData;
        BYTE *const dest = image_info->image_base + section_header[i].VirtualAddress;
        CopyMemory(dest, src, section_header[i].SizeOfRawData);
    }

    // save the extra data
    const IMAGE_SECTION_HEADER *const last_section_header = &section_header[section_num - 1];
    const BYTE *const last_section_end = file_base
        + last_section_header->PointerToRawData + last_section_header->SizeOfRawData;
    extra_data->size = file_size - (last_section_end - file_base);
    if (extra_data->size > 0)
    {
        // it's a read-only view
        extra_data->base = last_section_end;
    }

    success = true;

_Exit:
    if (!success)
    {
        FreePeImage(image_info);
        ZeroMemory(image_info, sizeof(PE_IMAGE_INFO));
        ZeroMemory(extra_data, sizeof(EXTRA_DATA_VIEW));
    }

    return success;
}


bool WriteImageToFile(
    const PE_IMAGE_INFO *const image_info,
    const TCHAR *const file_name)
{
    assert(image_info != NULL);
    assert(file_name != NULL);

    bool success = false;
    HANDLE file = CreateFile(file_name, GENERIC_WRITE, FILE_SHARE_READ,
        NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
    {
        SetLastErrorCode();
        goto _Exit;
    }

    // save the PE headers
    const DWORD file_align = image_info->nt_header->OptionalHeader.FileAlignment;
    const DWORD header_size = CalcHeadersSize(image_info->image_base, NULL, NULL);
    if (WriteAllToFile(file, image_info->image_base, header_size) == false)
    {
        goto _Exit;
    }

    // save the data of all sections
    const DWORD alignment = image_info->nt_header->OptionalHeader.FileAlignment;
    const WORD section_num = image_info->nt_header->FileHeader.NumberOfSections;
    const IMAGE_SECTION_HEADER *const section_header = image_info->section_header;
    for (WORD i = 0; i != section_num; ++i)
    {
        if (section_header[i].SizeOfRawData == 0)
        {
            continue;
        }

        const DWORD rva = section_header[i].VirtualAddress;
        const BYTE *const data = RvaToVa(image_info, rva);

        // sometimes the SizeOfRawData is zero, but the section still has its space in the file
        // so we must set the file pointer to the beginning of each section to write the data
        SetFilePointer(file, section_header[i].PointerToRawData, NULL, FILE_BEGIN);
        if (WriteAllToFile(file, data, section_header[i].SizeOfRawData) == false)
        {
            goto _Exit;
        }
    }

    success = true;

_Exit:
    if (file != INVALID_HANDLE_VALUE)
    {
        CloseHandle(file);
        file = INVALID_HANDLE_VALUE;
    }

    return success;
}


bool IsPe64(
    const PE_IMAGE_INFO *const image_info)
{
    assert(image_info != NULL);

    const WORD magic = image_info->nt_header->OptionalHeader.Magic;

    assert(magic != IMAGE_ROM_OPTIONAL_HDR_MAGIC);

    return magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC;
}


void FreePeImage(
    const PE_IMAGE_INFO *const image_info)
{
    assert(image_info != NULL);

    if (image_info->image_base != NULL)
    {
        VirtualFree(image_info->image_base, 0, MEM_RELEASE);
    }
}


bool IsPeFile(
    const BYTE *const file_base)
{
    assert(file_base != NULL);

    const IMAGE_NT_HEADERS *const nt_header = GetNtHeader(file_base);
    if (nt_header == NULL)
    {
        return false;
    }

    // check "PE" field and the number of sections.
    if (nt_header->Signature != IMAGE_NT_SIGNATURE
        || nt_header->FileHeader.NumberOfSections <= 1)
    {
        return false;
    }

    return true;
}


DWORD Align(
    const DWORD value,
    const DWORD align)
{
    assert(align != 0);

    return ((value + align - 1) / align * align);
}


BYTE *RvaToVa(
    const PE_IMAGE_INFO *const image_info,
    const DWORD rva)
{
    assert(image_info != NULL);

    return image_info->image_base + rva;
}


IMAGE_NT_HEADERS *GetNtHeader(
    const BYTE *const file_base)
{
    assert(file_base != NULL);

    // check "MZ" field
    const IMAGE_DOS_HEADER *const dos_header = (IMAGE_DOS_HEADER*)file_base;
    if (dos_header->e_magic != IMAGE_DOS_SIGNATURE)
    {
        return NULL;
    }

    const DWORD _dos_header_size = dos_header->e_lfanew;
    return (IMAGE_NT_HEADERS*)((BYTE*)dos_header + _dos_header_size);
}


DWORD CalcHeadersSize(
    const BYTE *const file_base,
    DWORD *const dos_header_size,
    DWORD *const nt_header_size)
{
    assert(file_base != NULL);

    // get the size of IMAGE_DOS_HEADER structure
    const DWORD _dos_header_size = ((IMAGE_DOS_HEADER*)file_base)->e_lfanew;
    if (dos_header_size != NULL)
    {
        *dos_header_size = _dos_header_size;
    }

    // get the size of IMAGE_NT_HEADERS structure
    const IMAGE_NT_HEADERS *const nt_header = GetNtHeader(file_base);
    const DWORD _nt_header_size = sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER)
        + nt_header->FileHeader.SizeOfOptionalHeader;
    if (nt_header_size != NULL)
    {
        *nt_header_size = _nt_header_size;
    }

    // get the size of all IMAGE_SECTION_HEADER structures
    const WORD section_num = nt_header->FileHeader.NumberOfSections;
    const DWORD section_header_size = section_num * sizeof(IMAGE_SECTION_HEADER);

    return _dos_header_size + _nt_header_size + section_header_size;
}