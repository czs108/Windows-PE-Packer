/**
 * @file install_shell.h
 * @brief Install the shell.
 *
 * @author Chen Zhenshuo (chenzs108@outlook.com)
 * @version 1.0
 * @date 2020-01-15
 * @par GitHub
 * https://github.com/czs108
 *
 * @see @ref entry-code-of-shell
 */

#pragma once

#include "../image.h"
#include "../section.h"

#include <windows.h>

#include <stdbool.h>

/**
 * @brief Get the size of the shell.
 *
 * @param new_imp_table_size    The size of the transformed import table.
 * @return The size of the shell.
 */
DWORD CalcShellSize(DWORD new_imp_table_size);


/**
 * @brief Install the shell.
 *
 * @param image_info            The PE image.
 * @param new_imp_table         The transformed import table.
 * @param new_imp_table_size    The size of the transformed import table.
 * @param encry_info            The encryption information of sections.
 * @param encry_count           The count of encryption information.
 * @return @p true if the method succeeds, otherwise @p false.
 *
 * @see @ref entry-code-of-shell
 */
bool InstallShell(const PE_IMAGE_INFO* image_info, const BYTE* new_imp_table,
                  DWORD new_imp_table_size, const ENCRY_INFO* encry_info,
                  DWORD encry_count);