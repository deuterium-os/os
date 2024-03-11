// SPDX-License-Identifier: MIT
/*
 * arch/x86/include/kernel/paging.h
 *
 * Copyright (c) 2024 CharaDrinkingTea
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Definitions of x86-64 paging
 *
 */

#ifndef PAGING_H
#define PAGING_H

enum pt_ent_flags
{
    P = 1,      // Present
    RW = 2,     // Read/Write
    US = 4,     // User/Supervisor
    PWT = 8,    // Page-level write-through
    PCD = 16,   // Page-level cache disable
    A = 32,     // Accessed
    D = 64,     // Dirty
    PS = 128,   // Page Size
    G = 256,    // Global
    R = 2048,   // Restart
    PAT = 4096  // Page Attribute Table
};

typedef struct
{
    enum pt_ent_flags flags : 12;
    long phy_addr           : 40;
    int                     : 7;    // Ignored
    int prot_key            : 4;
    int xd                  : 1;    // Execute-disable bit
} pte_t;

typedef struct  // PDE references to a Page Table
{
    enum pt_ent_flags flags : 12;
    long phy_addr           : 38;
    int                     : 13;   // Ignored
    int xd                  : 1;    // Execute-disable bit
} pde_table_t;

typedef struct  // PDE references to a 2MB Page
{
    enum pt_ent_flags flags : 13;
    int reserved            : 8;
    int phy_addr            : 31;
    int                     : 7;    // Ignored
    int prot_key            : 4;
    int xd                  : 1;    // Execute-disable bit
} pde_page_t;

typedef union
{
    pde_table_t table;
    pde_page_t page;
} pde_t;

typedef struct  // PDPTE references to a Page Directory
{
    enum pt_ent_flags flags : 12;
    long phy_addr           : 40;
    int                     : 11;   // Ignored
    int xd                  : 1;    // Execute-disable bit
} pdpte_dir_t;

typedef struct  // PDE references to a 1GB Page
{
    enum pt_ent_flags flags : 13;
    int reserved            : 17;
    int phy_addr            : 22;
    int                     : 7;    // Ignored
    int prot_key            : 4;
    int xd                  : 1;    // Execute-disable bit
} pdpte_page_t;

typedef union
{
    pdpte_dir_t table;
    pdpte_page_t page;
} pdpte_t;

typedef struct
{
    enum pt_ent_flags flags : 12;
    long phy_addr           : 40;
    int                     : 11;   // Ignored
    int xd                  : 1;    // Execute-disable bit
} pml4e_t;

typedef struct
{
    enum pt_ent_flags flags : 12;
    long phy_addr           : 40;
    int                     : 11;   // Ignored
    int xd                  : 1;    // Execute-disable bit
} pml5e_t;

enum cr3_flags
{
    PWT = 8,    // Page-level write-through
    PCD = 16,   // Page-level cache disable
};

typedef struct
{
    union
    {
        enum cr3_flags cr3_flags : 12;
        int pcid                 : 12;
    };
    long phy_addr                : 52;
} cr3_t;

#endif/* PAGING_H */