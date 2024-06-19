/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   _30x30y_wav;
    const int            _30x30y_wavSize = 576044;

    extern const char*   _30x30yS_wav;
    const int            _30x30yS_wavSize = 576044;

    extern const char*   _30x35yFull_wav;
    const int            _30x35yFull_wavSize = 576044;

    extern const char*   _35x25y_wav;
    const int            _35x25y_wavSize = 24908;

    extern const char*   s1_r3_o_wav;
    const int            s1_r3_o_wavSize = 21908;

    extern const char*   s2_r3_o_wav;
    const int            s2_r3_o_wavSize = 1152044;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 6;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
