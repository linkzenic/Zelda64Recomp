#ifndef __REPY_API__
#define __REPY_API__

#include "modding.h"
#include "recomputils.h"
#include "recompconfig.h"

/*! \file repy_api.h
    \version 2.0.0
    \brief The main header for Recomp External Python. Header version 2.
    \
 */

/**
 * @brief The mod id string for REPY.
 * 
 * The `recomp_py.h` imports all the functions and events needed for REPY, 
 * so you probably won't need to use this directly.
 */
#define REPY_MOD_ID_STR "RecompExternalPython_API"

// For internal use only. This flag is set when generating Doxygen documentation from this header.
// Otherwise, Doxygen gets confused by the `RECOMP_IMPORT` statements.
#ifdef DOXYGEN
#define REPY_IMPORT(func) func

// For internal use only. This flag is set when building RecompExternalPython from source to disable importing.
#elif RECOMP_PY_BUILD_MODE
#define REPY_IMPORT(func) func
#else 
#define REPY_IMPORT(func) RECOMP_IMPORT(REPY_MOD_ID_STR, func)

// In case the compiler doesn't define __FILE_NAME__
#ifndef __FILE_NAME__
#define __FILE_NAME__ "Unknown Source File"
#endif

#endif
/**
 * \defgroup mod_code_c_api C API - N64Recompiled Mod Code
 * REPY C API for mod code
 * @{
 */


/** \defgroup repy_types Types
 * \brief All of the C types that REPY defines.
 *  @{
 */

/** \defgroup repy_numerical_types Numerical Types
 * \brief Numerical Types used by REPY.
 * 
 * The `repy_api.h` header specifies it's own numerical types to avoid relying on libultra headers.
 *  @{
 */

 /**
  * @brief A signed, 8-bit integer compatible with libultra's `s8`.
  * 
  * Casting from the libultra `s8` type should not be required.
  */
typedef signed char            REPY_s8;

 /**
  * @brief An unsigned, 8-bit integer compatible with libultra's `u8`.
  * 
  * Casting from the libultra `u8` type should not be required.
  */
typedef unsigned char          REPY_u8;

 /**
  * @brief A signed, 16-bit integer compatible with libultra's `s16`.
  * 
  * Casting from the libultra `s16` type should not be required.
  */
typedef signed short int       REPY_s16;

 /**
  * @brief An unsigned, 16-bit integer compatible with libultra's `u16`.
  * 
  * Casting from the libultra `u16` type should not be required.
  */
typedef unsigned short int     REPY_u16;

 /**
  * @brief A signed, 32-bit integer compatible with libultra's `s32`.
  * 
  * Casting from the libultra `s8` type should not be required.
  */
typedef signed long            REPY_s32;

 /**
  * @brief An unsigned, 13-bit integer compatible with libultra's `u32`.
  * 
  * Casting from the libultra `u32` type should not be required.
  */
typedef unsigned long          REPY_u32;

 /**
  * @brief A signed, 64-bit integer compatible with libultra's `s64`.
  * 
  * Casting from the libultra `s64` type should not be required.
  */
typedef signed long long int   REPY_s64;

 /**
  * @brief An unsigned, 64-bit integer compatible with libultra's `u64`.
  * 
  * Casting from the libultra `s64` type should not be required.
  */
typedef unsigned long long int REPY_u64;

 /**
  * @brief A 32-bit floating point value compatible with libultra's `f32`.
  * 
  * Casting from the libultra `f32` type should not be required.
  */
typedef float  REPY_f32;

 /**
  * @brief A 64-bit floating point value compatible with libultra's `f64`.
  * 
  * Casting from the libultra `f64` type should not be required.
  */
typedef double REPY_f64;

/**
 * @brief A boolean type compatible with libultra's `bool` type.
 * 
 * Casting from the libultra `bool` type should not be required.
 */
typedef _Bool REPY_bool;

/**
 * @brief Used to set the type of code-string being compiled, in line with how Python's
 * built-in `compile` function operates.
 * 
 * Used with `REPY_CompileCStr` and `REPY_CompileCStr`. `REPY_Compile` accepts a `REPY_Handle` argument instead, 
 * which should reference a Python `str` object.
 * 
 * See `REPY_CodeModeEnum` for values.
 */
typedef enum REPY_CodeModeEnum {
    REPY_CODE_EXEC = 0, ///< Equivalent to `exec`
    REPY_CODE_EVAL = 1, ///< Equivalent to `eval`
    REPY_CODE_SINGLE = 2 ///< Equivalent to `single`
} REPY_CodeMode;

/** @}*/

/** \defgroup repy_handle_types Handle Types
 * \brief Types used by REPY to represent entities that exist outside of mod code.
 * 
 * Generally, these are represented using integer values.
 * @{
 */

/**
 * @brief Represents a Python object in REPY API functions. 
 * 
 * Technically, the handle is a unsigned integer value between 0x00000001 and 0xFFFFFFFF, inclusive. The REPY external library
 * will look up the corresponding Python object from an internally managed slot map. A handle is considered 'valid'
 * if the value is assigned to a Python object. 
 * 
 * In terms of Python's reference-count-based garbage collection, each valid handle counts as a single reference 
 * to an object, and multiple handles can be mapped to a single Python object. Once created, a handle must be released
 * to remove that reference, either by using `REPY_Release` or flagging the handle as Single-Use (more on that below). 
 * If you need multiple references to a Python object, you can get a new handle to the same object using `REPY_CopyHandle`.
 * With the exception of `REPY_MakeSUH`, `REPY_VariadicLocals` and `REPY_VL_SUH`, any REPY API function that returns a REPY_Handle always creates a new handle. 
 * Failure to release handles will result in resource/memory leaks. 
 * 
 * The handle value of `REPY_NO_OBJECT` (the numerical value 0) is a special case, and represents the absense of any Python object. 
 * Note that this is different from Python's `None`, which is itself a Python object. If an API function with `REPY_Handle`
 * as the return type returns `REPY_NO_OBJECT`, that will mean a Python error has occured (unless otherwise specified in the function's 
 * documentation).
 * 
 * There are a few cases where a REPY_Handle of `REPY_NO_OBJECT` is acceptable as a function argument, indicating that the argument
 * is not used. These arguments are named with the `_nullable` suffix.
 * 
 * Handles can be flagged as Single-Use, which means that the handle will be immediately released after it's next use in a REPY
 * API function. This is primarily to allow nesting calls to REPY API functions without incurring resource/memory leaks. Any handle
 * can be flagged as Single-Use using `REPY_MakeSUH` or `REPY_SetSUH`, and the Single-Use status can be checked with `REPY_GetSUH`.
 * Several common REPY API functions have alternative versions that automatically return a Single Use handle. These functions will
 * have the suffix `_SUH` in their names.
 * 
 * Not that the only functions that won't release a Single-Use handle are ones meant to manipulate the handles themselves:
 * * `REPY_MakeSUH`
 * * `REPY_IsValidHandle`
 * * `REPY_GetSUH`
 * * `REPY_SetSUH`
 * * `REPY_DeferredCleanupHelper_AddHandle`
 * 
 * Their REPY_Handle argument names have the suffix `_no_release`
 * to reflect this.
 * 
 * If at any point this documentation refers to a `REPY_Handle` as a Python object, understand that it is referring to the Python object that
 * the handle represents. The mechanism for mapping `REPY_Handle` values to Python objects is written entirely in C++, and the Python interpreter 
 * does not have access to it.
 */
typedef unsigned int REPY_Handle;

/**
 * @brief Index value for a specific Python interpreter, either the main interpreter or a registered subinterpreter.
 * 
 * Values of `REPY_InterpreterIndex` follow this schema:
 * 
 * * Zero is a special value representing the main Python interpreter. Can also be represented with the `REPY_MAIN_INTERPRETER` macro.
 * * Positive integers represent subinterpreters instances, when are created by and the index returned by `REPY_RegisterSubinterpreter`.
 * * Negative integers are invalid. `REPY_GetCurrentInterpreter` will return -1 when no interpreter is currently active.
 */
typedef signed int REPY_InterpreterIndex;

/** \defgroup repy_handle_types_special_values Handle Special Values
 * \brief Macros representing special values for REPY \ref repy_handle_types.
 * 
 * Read the descriptions to see which handle type the macro goes with.
 * @{
 */

/**
 * @brief Represents the absence of a Python object in REPY API functions. Goes with `REPY_Handle`.
 * 
 * A more readable alternative to simply entering 0.
 */
#define REPY_NO_OBJECT 0

/**
 * @brief Represents the index of the main Python interpreter. Goes with `REPY_InterpreterIndex`.
 * 
 * A more readable alternative to simply entering 0.
 */
#define REPY_MAIN_INTERPRETER 0

/**
 * @brief A value indicating that the interpreter stack is empty. Goes with `REPY_InterpreterIndex`.
 * 
 * A more readable alternative to simply entering -1.
 * 
 */
#define REPY_INTERPRETER_STACK_EMPTY -1

/** @}*/
/** @}*/

/** \defgroup repy_object_types Object Types
 * \brief Types used by REPY for objects that exist in mod memory.
 * 
 * Specified as `void` in this header to conceal internals for ABI compatability.
 * Use pointers to access the objects themselves.
 * @{
 */

/**
 * @brief Helper object used to when iterating through Python objects in loops in C code.
 * 
 * These objects are primarily used as part of the the `REPY_FOREACH` and `REPY_FN_FOREACH_CACHE` macros,
 * which simulate the behavior of Python's own `for` loops. You can use them manually as well.
 * 
 * For ABI compatability, the internals of these objects have been obfuscated, and they can only be allocated on the heap.
 * 
 * The methods for this type can be found at \ref repy_iterator_helper_methods.
 */
typedef void REPY_IteratorHelper;


/**
 * @brief Helper object used to cache Python expressions as bytecode, so that they don't need to be re-parsed and compiled every time they're run.
 * 
 * Used as part of the macro `REPY_FN_IF_CACHE_INIT`, and is used with `REPY_FN_IF_CACHE_STMT`, `REPY_FN_IF_CACHE` and `REPY_FN_ELIF_CACHE`.
 * Generally initialized as a `static` variable, so that that the compiled bytecode is is preserverd between uses.
 * 
 * The chain is a singly-linked list with the bytecode for each Python expression from a `REPY_IfStmtHelper_Step` call. If the next link does not
 * exist when that step is called, it will be created and appended to the chain.
 * 
 * For ABI compatability, the internals of these objects have been obfuscated, and they can only be allocated on the heap.
 * 
 * The methods for this type can be found at \ref repy_if_stmt_chain_methods.
 */
typedef void REPY_IfStmtChain;

/**
 * @brief Helper object used to step through a `REPY_IfStmtChain` while it's being evaluated, generating new links as necessary.
 * 
 * Used as part of the `REPY_FN_IF_CACHE_STMT` macro. See `REPY_IfStmtChain` for more info. While the chain itself should be preserved
 * between executions, this helper object should not.
 *
 * For ABI compatability, the internals of these objects have been obfuscated, and they can only be allocated on the heap.
 * 
 * The methods for this type can be found at \ref repy_if_stmt_helper_methods.
 */
typedef void REPY_IfStmtHelper;

/**
 * @brief Helper object used to deallocate `REPY_IfStmtHelper` and `REPY_IteratorHelper` objects when a `REPY_FN` scope exits.
 * 
 * Support for cleaning up other things may be added in the future.
 * 
 * For ABI compatability, the internals of these objects have been obfuscated, and they can only be allocated on the heap.
 * 
 * The methods for this type can be found at \ref repy_deferred_cleanup_helper_methods.
 */
typedef void REPY_DeferredCleanupHelper;

/** @}*/

/** @}*/

/** \defgroup repy_events Events
 * 
 * N64Recompiled events defined by REPY.
 * 
 *  @{
 */

/**
 * @brief Runs before the Python interpreter is initialized.
 * 
 * Primarily used to flag add NRM files to the Python module search path on startup.
 * 
 * Takes no arguments, returns void.
 */
#define REPY_ON_PRE_INIT RECOMP_CALLBACK(REPY_MOD_ID_STR, REPY_OnPreInit) 

/**
 * @brief Event that runs immediately after the Python interpreter is initialized. Used by the `REPY_REGISTER_SUBINTERPRETER` macro.
 * 
 * If you want to ensure that your code is called after events specified by REPY macros are called, use `REPY_ON_POST_INIT`.
 * 
 * Takes no arguments, returns void.
 */
#define REPY_ON_CONFIG_INTERPRETERS RECOMP_CALLBACK(REPY_MOD_ID_STR, REPY_OnConfigInterpreters)

/**
 * @brief Event that runs immediately after `REPY_ON_INIT_SUBINTERPRETERS`. Used by the various global and static code caching macros.
 * 
 * If you want to ensure that your code is called after events specified by REPY macros are called, use `REPY_ON_POST_INIT`.
 * 
 * Takes no arguments, returns void.
 */
#define REPY_ON_INIT_CODE_CACHE RECOMP_CALLBACK(REPY_MOD_ID_STR, REPY_OnInitCodeCache)

/**
 * @brief Event that runs immediately after the `REPY_ON_INIT` is called. Since many REPY macros use `REPY_ON_INIT`, use this event to ensure your code runs after.
 * 
 * Nothing in the `repy_api.h` header uses this event. Use for your own mod's initialization code.
 * 
 * Takes no arguments, returns void.
 */
#define REPY_ON_POST_INIT RECOMP_CALLBACK(REPY_MOD_ID_STR, REPY_OnPostInit)

/** @}*/

/** \defgroup repy_macros Macros
 * 
 * REPY makes considerable use of macros streamline common tasks.
 * 
 *  @{
 */

/** \defgroup repy_init_macros Initialization Macros
 * 
 * Macros that initialize REPY resources before or during REPY's initialization process.
 * 
 * @{
 */

/**
 * @brief Adds this .nrm file to Python's module search path.
 * 
 * This will allow you to add Python modules (both single files and module folders) to your mod by
 * including them under the `additional_files` section of your mod.toml. See \ref including_python_modules for more information.
 * 
 * These modules will be available by the time `REPY_ON_CONFIG_INTERPRETERS` runs, and will be available to all subinterpreters.
 * Unless you're creating a mod that bundles Python packages for other mods to use, this is likely not the behavior you want.
 */
#define REPY_PREINIT_ADD_NRM_TO_ALL_INTERPRETERS \
REPY_ON_PRE_INIT void _repy_register_nrm () { \
    const unsigned char* nrm_file_path = recomp_get_mod_file_path(); \
    REPY_PreInitAddSysPath(nrm_file_path); \
    recomp_free((void*)nrm_file_path); \
};


/**
 * @brief Adds this .nrm file to the main Python interpreter's module search path.
 * 
 * This will allow you to add Python modules (both single files and module folders) to your mod by
 * including them under the `additional_files` section of your mod.toml. See \ref including_python_modules for more information.
 * 
 * These modules will be available during `REPY_ON_CONFIG_INTERPRETERS`.
 */
#define REPY_ADD_NRM_TO_MAIN_INTERPRETER \
REPY_ON_CONFIG_INTERPRETERS void __repy_config_main_interpreter() { \
    recomp_printf("Configuring Main Interpreter (Index %i)\n", 0); \
    REPY_PushInterpreter(subinterp_identifier); \
    REPY_AddNrmToSysPath(); \
    REPY_PopInterpreter(); \
} 

 /**
  * @brief Use this macro at the global level of a C file to initialize a subinterpreter on startup.
  * 
  * The index of the subinterpreter will be stored in a global `REPY_InterpreterIndex` variable, the name 
  * for which is set via the `subinterp_identifier` argument.
  * 
  * This macro also adds this .nrm to the import path`sys.path` for the new subinterpreter, meaning that
  * the subinterpreter can import Python modules stored within the `.nrm`. See \ref including_python_modules for more information.
  */
#define REPY_REGISTER_SUBINTERPRETER(subinterp_identifier) \
REPY_InterpreterIndex subinterp_identifier = 0; \
REPY_ON_PRE_INIT void __repy_config_ ## subinterp_identifier () { \
    subinterp_identifier = REPY_PreInitRegisterSubinterpreter(); \
    recomp_printf("Registering Subinterpreter '%s' (Index %i)\n", #subinterp_identifier, subinterp_identifier); \
} \
REPY_ON_CONFIG_INTERPRETERS void subinterp_identifier ## _config() { \
    recomp_printf("Configuring Subinterpreter '%s' (Index %i)\n", #subinterp_identifier, subinterp_identifier); \
    REPY_PushInterpreter(subinterp_identifier); \
    REPY_AddNrmToSysPath(); \
    REPY_PopInterpreter(); \
} \

/**
 * @brief Macro used to extern in a subinterpreter index variable.
 * 
 * An alternative to `extern REPY_InterpreterIndex subinterp_identifier`. Potentially more readable.
 * 
 */
#define REPY_EXTERN_SUBINTERPRETER(subinterp_identifier) \
extern REPY_InterpreterIndex subinterp_identifier; \


/**
 * @brief Macro that creates `extern` statements for data included by `REPY_INCBIN` and `REPY_INCBIN_TEXT`.
 * 
 * See those macros for more information.
 */
#define REPY_EXTERN_INCBIN(identifier) \
    extern REPY_u8 identifier[]; \
    extern REPY_u8 identifier##_end[]

#ifdef REPY_SILENCE_INCBIN_SQUIGGLES
#define REPY_INCBIN(identifier, filename) \
    extern REPY_u8 identifier[]; \
    extern REPY_u8 identifier##_end[]

#define REPY_INCBIN_TEXT(identifier, filename) \
    extern REPY_u8 identifier[]; \
    extern REPY_u8 identifier##_end[]               
#else

/**
 * @brief General INCBIN macro used by several other initialization macros to include external Python code.
 * 
 * The data included by this macro is not NULL-terminated. You'll need to use `identifier_end` to find the
 * end of the code-block and use `identifier_end - identifier` to find the length.
 * 
 * @param identifier The variable name for the start of the data. `identifier_end` will indicate the end of the data.
 * @param filename The path to the included file. The file needs to be in your include path.
 */
#define REPY_INCBIN(identifier, filename)      \
    asm(".pushsection .rodata\n"                      \
        "\t.globl " #identifier "\n"                  \
        "\t.type " #identifier ", @object\n"          \
        "\t.balign 8\n"                               \
        #identifier ":\n"                             \
        "\t.incbin \"" filename "\"\n"                \
        "\t.globl " #identifier "_end\n"              \
        #identifier "_end:\n"                         \
        "\t.popsection\n");                           \
        REPY_EXTERN_INCBIN(identifier)

/**
 * @brief General INCBIN macro used by several other initialization macros to include external Python code.
 * 
 * The data included by this macro is NULL-terminated, with `identifier_end` pointing to the termination character.
 * Therefore, you'll need to use `identifier_end - identifier - 1` to find the length of the text.
 * 
 * @param identifier the variable name for the start of the data. `identifier_end` will indicate the end of the data.
 * @param filename the path to the included file. The file needs to be in your include path.
 */
#define REPY_INCBIN_TEXT(identifier, filename)      \
    asm(".pushsection .rodata\n"                      \
        "\t.globl " #identifier "\n"                  \
        "\t.type " #identifier ", @object\n"          \
        "\t.balign 8\n"                               \
        #identifier ":\n"                             \
        "\t.incbin \"" filename "\"\n"                \
        "\t.space 1"                                  \
        "\t.globl " #identifier "_end\n"              \
        #identifier "_end:\n"                         \
        "\t.popsection\n");                           \
        REPY_EXTERN_INCBIN(identifier)
#endif

/**
 * @brief On startup, compiles a Python code string into bytecode with a global handle. Use outside of any functions.
 * 
 * To access the the bytecode from another source file, use `extern REPY_Handle bytecode_identifier;`,
 * where `bytecode_identifier` is the variable name you entered in the `bytecode_identifier` parameter.
 * 
 * @param interpreter_index The index for which interpreter should cache this code. Should be a `REPY_InterpreterIndex`.
 * @param bytecode_identifier The variable name for the resultant `REPY_Handle` bytecode handle.
 * @param code_mode The type of code being compiled. See `REPY_CodeMode` for valid modes.
 * @param code_str The Python code string to compile. Must be NULL-terminated.
 */
#define REPY_GLOBAL_COMPILE_CACHE(interpreter_index, bytecode_identifier, code_mode, code_str) \
REPY_Handle bytecode_identifier = 0; \
REPY_ON_INIT_CODE_CACHE void _cache_code_ ## bytecode_identifier () { \
    if (bytecode_identifier == 0) { \
        REPY_PushInterpreter(interpreter_index); \
        char* iden_str = REPY_InlineCodeSourceStrHelper("REPY_GLOBAL_COMPILE_CACHE", __FILE_NAME__, (char*) __func__, __LINE__, #bytecode_identifier); \
        bytecode_identifier = REPY_CompileCStr(code_str, (const char*)iden_str, code_mode); \
        recomp_free(iden_str); \
        REPY_PopInterpreter(); \
    } \
}

/**
 * @brief On startup, compiles a Python code string into bytecode with a static handle. Use outside of any functions.
 * 
 * Because the `REPY_Handle` variable is marked as `static`, there will be no accessing it from other source files.
 * 
 * @param interpreter_index The index for which interpreter should cache this code. Should be a `REPY_InterpreterIndex`.
 * @param bytecode_identifier The variable name for the resultant `static REPY_Handle` bytecode handle.
 * @param code_mode The type of code being compiled. See `REPY_CodeMode` for valid modes.
 * @param code_str The Python code string to compile. Must be NULL-terminated.
 */
#define REPY_STATIC_COMPILE_CACHE(interpreter_index, bytecode_identifier, code_mode, code_str) \
static REPY_Handle bytecode_identifier = 0; \
REPY_ON_INIT_CODE_CACHE void _cache_code_ ## bytecode_identifier () { \
    if (bytecode_identifier == 0) { \
        REPY_PushInterpreter(interpreter_index); \
        char* iden_str = REPY_InlineCodeSourceStrHelper("REPY_STATIC_COMPILE_CACHE", __FILE_NAME__, (char*) __func__, __LINE__, #bytecode_identifier); \
        bytecode_identifier = REPY_CompileCStr(code_str, (const char*)iden_str, code_mode); \
        recomp_free(iden_str); \
        REPY_PopInterpreter(); \
    } \
}

/**
 * @brief On startup, compiles a INCBINed Python code file into bytecode with a global handle.
 * 
 * To access the the bytecode from another source file, use `extern REPY_Handle bytecode_identifier;`,
 * where `bytecode_identifier` is the variable name you entered in the `bytecode_identifier` parameter.
 * 
 * Unlike `REPY_GLOBAL_COMPILE_CACHE`, this macro assumes a REPY_CodeMode of `REPY_CODE_EXEC`.
 *
 * @param interpreter_index The index for which interpreter should cache this code. Should be a `REPY_InterpreterIndex`.
 * @param bytecode_identifier The variable name for the resultant `REPY_Handle` bytecode handle.
 * @param filename The path to the module file to INCBIN. The file needs to be in your include path. 
 */
#define REPY_GLOBAL_COMPILE_INCBIN_CACHE(interpreter_index, bytecode_identifier, filename) \
REPY_INCBIN(bytecode_identifier ## _code_str, filename); \
REPY_Handle bytecode_identifier = 0; \
REPY_ON_INIT_CODE_CACHE void _cache_code_ ## bytecode_identifier () { \
    if (bytecode_identifier == 0) { \
        REPY_PushInterpreter(interpreter_index); \
        char* iden_str = REPY_InlineCodeSourceStrHelper("REPY_GLOBAL_COMPILE_INCBIN_CACHE: " filename, __FILE_NAME__, (char*) __func__, __LINE__, #bytecode_identifier); \
        bytecode_identifier = REPY_CompileCStrN((const char*)bytecode_identifier ## _code_str, bytecode_identifier ## _code_str_end - bytecode_identifier ## _code_str, \
            (const char*)iden_str, REPY_CODE_EXEC); \
        recomp_free(iden_str); \
        REPY_PopInterpreter(); \
    } \
}

/**
 * @brief On startup, compiles a INCBINed Python code file into bytecode with a global handle. Use outside of any functions.
 * 
 * Because the `REPY_Handle` variable is marked as `static`, there will be no accessing it from other source files.
 * 
 * Unlike `REPY_GLOBAL_COMPILE_CACHE`, this macro assumes a REPY_CodeMode of `REPY_CODE_EXEC`.
 * 
 * @param interpreter_index The index for which interpreter should cache this code. Should be a `REPY_InterpreterIndex`.
 * @param bytecode_identifier The variable name for the resultant `static REPY_Handle` bytecode handle.
 * @param filename The path to the module file to INCBIN. The file needs to be in your include path. 
 */
#define REPY_STATIC_COMPILE_INCBIN_CACHE(interpreter_index, bytecode_identifier, filename) \
REPY_INCBIN(bytecode_identifier ## _code_str, filename); \
REPY_Handle bytecode_identifier = 0; \
REPY_ON_INIT_CODE_CACHE void _cache_code_ ## bytecode_identifier () { \
    REPY_PushInterpreter(interpreter_index); \
    if (bytecode_identifier == 0) { \
        char* iden_str = REPY_InlineCodeSourceStrHelper("REPY_STATIC_COMPILE_INCBIN_CACHE: " filename, __FILE_NAME__, (char*) __func__, __LINE__, #bytecode_identifier); \
        bytecode_identifier = REPY_CompileCStrN((const char*)bytecode_identifier ## _code_str, bytecode_identifier ## _code_str_end - bytecode_identifier ## _code_str, \
            (const char*)iden_str, REPY_CODE_EXEC); \
        recomp_free(iden_str); \
        REPY_PopInterpreter(); \
    } \
}
/** @}*/

/** \defgroup repy_inline_cache_macros Inline Code Caching Macros
 * 
 * Parsing a Python code string every time it needs to be executed would make for poor performance.
 * The macros here will parse a Python code string the first time they are run, and provide a `REPY_Handle`
 * to a Python bytecode object.
 * 
 * @{
 */

/**
 * @brief Inside a function, construct a code block that compiles to bytecode a Python code string the first time it's run,
 * storing the handle for that bytecode in a static variable. Allows setting a category name.
 * 
 * This macro forms the backbone of many of the other macros in this header, expecially those in the `REPY_FN` section.
 * By only parsing and compiling the Python code string once, we can dramatically improve the performance of any function
 * that needs to execute inline Python code.
 * 
 * The category argument becomes part of the resultant bytecode object's `identifier` string, which is displayed in Python error messages.
 * In this header, macros that depend this one will use their own name for this argument. That way, if you get a error message from running
 * inline Python code, you can see which macro caused the error along with the other source information.
 * 
 * @param category A category name used as part of the bytecode's identifying string. Many REPY macros that use this one will set this to their own macro names.
 * @param bytecode_identifier The name of the static variable that the bytecode handle will be assigned to.
 * @param code_mode The type of code being compiled. See `REPY_CodeMode` for valid modes.
 * @param code_str The Python code string to compile. Should be NULL-terminated.
 */
#define REPY_INLINE_COMPILE_CACHE_BLOCK(category, bytecode_identifier, code_mode, code_str) \
static REPY_Handle bytecode_identifier = 0; \
if (bytecode_identifier == 0) { \
    char* iden_str = REPY_InlineCodeSourceStrHelper(category, __FILE_NAME__, (char*) __func__, __LINE__, #bytecode_identifier); \
    bytecode_identifier = REPY_CompileCStr(code_str, (const char*)iden_str, code_mode); \
    recomp_free(iden_str); \
}

/**
 * @brief Inside a function, construct a code block that compiles to bytecode a Python code string the first time it's run,
 * storing the handle for that bytecode in a static variable.
 * 
 * By only parsing and compiling the Python code string once, we can dramatically improve the performance of any function
 * that needs to execute inline Python code.
 * 
 * This macro is identical to `REPY_INLINE_COMPILE_CACHE_BLOCK`, except it uses "REPY_INLINE_COMPILE_CACHE" as the category.
 * 
 * @param bytecode_identifier The name of the static variable that the bytecode handle will be assigned to.
 * @param code_mode The type of code being compiled. See `REPY_CodeMode` for valid modes.
 * @param code_str The Python code string to compile. Should be NULL-terminated.
 */
#define REPY_INLINE_COMPILE_CACHE(bytecode_identifier, code_mode, code_str) \
REPY_INLINE_COMPILE_CACHE_BLOCK("REPY_INLINE_COMPILE_CACHE", bytecode_identifier, code_mode, code_str)

/** @}*/

/** \defgroup repy_foreach_macros REPY_FOREACH - Python Object Iteration Macros
 * 
 * A convienient way to iterate through a Python object in your C mod code.
 * 
 * @{
 */

/**
 * @brief Iterate through a Python object, the way Python's `for` loops do.
 * 
 * A `REPY_IteratorHelper` object is created to manage the iteration process. The current object of the loop can be
 * accessed via `REPY_IteratorHelper_BorrowCurrent`, and the index of that object can be accessed via
 * `REPY_IteratorHelper_GetIndex`. See the `REPY_IteratorHelper` documentation for more information.
 * 
 * REPY_IteratorHelper copies the `py_object` `REPY_Handle` you pass in, so it's safe to release it of have it be Single-Use.
 * 
 * Unlike `REPY_FOREACH`, this macro assumes that it's being called inside a `REPY_FN` scope, and therefore will use
 * the scope's auto cleanup. That way, you don't need to worry about doing it yourself with the other macros in this section.
 * 
 * @param iter_identifier the variable name for the `REPY_IteratorHelper` pointer.
 * @param py_object a `REPY_Handle` for the Python object to iterate through.
 */
#define REPY_FOREACH_FNAC(iter_identifier, py_object) \
for (REPY_IteratorHelper* iter_identifier = \
    REPY_DeferredCleanupHelper_AddIteratorHelper(REPY_FN_AUTO_CLEANUP, REPY_IteratorHelper_Create(py_object, REPY_NO_OBJECT, NULL, false)); \
    REPY_IteratorHelper_Update(iter_identifier); \
) \

/**
 * @brief Iterate through a Python object, the way Python's `for` loops do.
 * 
 * A `REPY_IteratorHelper` object is created to manage the iteration process. The current object of the loop can be
 * accessed via `REPY_IteratorHelper_BorrowCurrent`, and the index of that object can be accessed via `REPY_IteratorHelper_GetIndex`.
 * See the `REPY_IteratorHelper` documentation for more information.
 * 
 * REPY_IteratorHelper copies the `py_object` handle you pass in, so it's safe to release it of have it be Single-Use.
 * 
 * In this macro, `REPY_IteratorHelper_Update` is set to clean up the `REPY_IteratorHelper` automatically, once the iteration ends.
 * Ergo, you only have to clean up if you end the loop early, such as through a break or a return. The macros `REPY_FOREACH_CLEANUP_NOW`,
 * `REPY_FOREACH_BREAK`, and `REPY_FOREACH_RETURN` are provided to facillitate that. Failure to manually clean up the `REPY_IteratorHelper`
 * when exiting the loop early will result in a memory leak.
 * 
 * Because this macro can be used outside of a `REPY_FN` scope, this macro cannot automatically add it to the auto-cleanup handler.
 * 
 * @param iter_identifier the variable name for the `REPY_IteratorHelper` pointer.
 * @param py_object a `REPY_Handle` for the Python object to iterate through.
 * @param auto_self_destuct whether or not this helper should destroy itself on the last `REPY_IteratorHelper_Update` call.
 */
#define REPY_FOREACH(iter_identifier, py_object, auto_self_destuct) \
for (REPY_IteratorHelper* iter_identifier = REPY_IteratorHelper_Create(py_object, REPY_NO_OBJECT, NULL, auto_self_destuct); REPY_IteratorHelper_Update(iter_identifier);)

/**
 * @brief Manually clean up the `REPY_IteratorHelper` for a `REPY_FOREACH` loop.
 * 
 * Once this had been called, you'll have a crash if you attempt another pass of the loop.
 * 
 * @param iter_identifier the `REPY_IteratorHelper` pointer.
 */
#define REPY_FOREACH_CLEANUP_NOW(iter_identifier) \
REPY_IteratorHelper_Destroy(iter_identifier)

/**
 * @brief Manually clean up the `REPY_IteratorHelper` for a `REPY_FOREACH` loop, and immediately break.
 * 
 * @param iter_identifier the `REPY_IteratorHelper` pointer.
 */
#define REPY_FOREACH_BREAK(iter_identifier) \
REPY_FOREACH_CLEANUP_NOW(iter_identifier); break

/**
 * @brief Manually clean up the `REPY_IteratorHelper` for a `REPY_FOREACH` loop, and immediately return.
 * 
 * Supports returning a value.
 * 
 * @param iter_identifier the `REPY_IteratorHelper` pointer.
 */
#define REPY_FOREACH_RETURN(iter_identifier, retType, retVal) \
retType __repy_retVal = retVal; \
REPY_FOREACH_CLEANUP_NOW(iter_identifier); \
return __repy_retVal

/** @}*/

/** \defgroup repy_fn REPY_FN - Python Interpreter Operations Matching C Function Scopes.
 * 
 * The `REPY_FN` macro collection is the real powerhouse of the REPY API. Using the exposed API functions,
 * these macros will allow you to execute Python code as part of your mod code functions, with Python scopes
 * that correspond to the scope of the mod code functions themselves, and a control of flow that feels natural.
 * 
 * Interlacing your moc code functions with Python code serves as an excellent workaround for native code 
 * being unable to call mod code.
 * 
 * Go to \ref repy_fn_overview for more information.
 * 
 *  @{
 */

/** \defgroup repy_fn_setupcleanup REPY_FN_SETUP/CLEANUP - Initialization and cleanup of Python scopes.
 *  @{
 */

/**
 * @brief The variable name for inline execution global scopes.
 * 
 */
#define REPY_FN_GLOBAL_SCOPE __repy_globals

/**
 * @brief The variable name for inline execution local scopes.
 * 
 */
#define REPY_FN_LOCAL_SCOPE __repy_locals

/**
 * @brief The variable name for helper auto-cleanup object.
 * 
 */
#define REPY_FN_AUTO_CLEANUP __repy_auto_cleanup


/**
 * @brief Create an inline execution scope for your function without any globals.
 * 
 * The global and local scope `dict` objects will the same. Python's built-ins will be added
 * whenever Python code is first executed.
 * 
 * `REPY_FN` scopes also create a `REPY_DeferredCleanupHandler` instance, which is used to automatically destroy any
 * helper objects created by other `REPY_FN` macros. The cleanup handler is also capable of releasing 
 * `REPY_Handle` instances and freeing memory using `recomp_free` for you as well, but those cases must be
 * declared manually using `REPY_FN_DEFER_RELEASE` and `REPY_FN_DEFER_RECOMP_FREE`.
 * 
 * The Python interpreter to use is defined by the `interp_index` argument.
 * 
 * @param interp_index a valid interpreter index. Should be of the type `REPY_InterpreterIndex`.
 */
#define REPY_FN_SETUP_INTERP(interp_index) \
REPY_PushInterpreter(interp_index); \
REPY_Handle REPY_FN_GLOBAL_SCOPE = REPY_CreateDict(0); \
REPY_Handle REPY_FN_LOCAL_SCOPE = REPY_FN_GLOBAL_SCOPE; \
REPY_DeferredCleanupHelper* REPY_FN_AUTO_CLEANUP = REPY_DeferredCleanupHelper_Create() \

/**
 * @brief Create an inline execution scope for your function, using a pre-defined Python
 * `dict` as your global scope object, and creating a new Python `dict` for the local scope.
 * Note that, unless otherwise specified, executing Python code stores variables on the local scope.
 * 
 * If the global scope `dict` doesn't have Python's builtins predefined, they will be added to the `dict` whenever
 * Python code is first executed.
 * 
 * `REPY_FN` scopes also create a `REPY_DeferredCleanupHandler` instance, which is used to automatically destroy any
 * helper objects created by other `REPY_FN` macros. The cleanup handler is also capable of releasing 
 * `REPY_Handle` instances and freeing memory using `recomp_free` for you as well, but those cases must be
 * declared manually using `REPY_FN_DEFER_RELEASE` and `REPY_FN_DEFER_RECOMP_FREE`.
 * 
 * The Python interpreter to use is defined by the `interp_index` argument.
 * 
 * @param interp_index a valid interpreter index. Should be of the type `REPY_InterpreterIndex`.
 * @param globals The Python `dict` to use as a global scope.
 */
#define REPY_FN_SETUP_INTERP_WITH_GLOBALS(interp_index, globals) \
REPY_PushInterpreter(interp_index); \
REPY_Handle REPY_FN_GLOBAL_SCOPE = globals; \
REPY_Handle REPY_FN_LOCAL_SCOPE = REPY_CreateDict(0); \
REPY_DeferredCleanupHelper* REPY_FN_AUTO_CLEANUP = REPY_DeferredCleanupHelper_Create() \

/**
 * @brief Create an inline execution scope for your function without any globals.
 * 
 * The global and local scope `dict` objects will the same. Python's built-ins will be added
 * whenever Python code is first executed.
 * 
 * `REPY_FN` scopes also create a `REPY_DeferredCleanupHandler` instance, which is used to automatically destroy any
 * helper objects created by other `REPY_FN` macros. The cleanup handler is also capable of releasing 
 * `REPY_Handle` instances and freeing memory using `recomp_free` for you as well, but those cases must be
 * declared manually using `REPY_FN_DEFER_RELEASE` and `REPY_FN_DEFER_RECOMP_FREE`.
 * 
 * This execution scope will use the main Python interpreter.
 */
#define REPY_FN_SETUP \
REPY_FN_SETUP_INTERP(REPY_MAIN_INTERPRETER)

/**
 * @brief Create an inline execution scope for your function, using a pre-defined Python
 * `dict` as your global scope object, and creating a new Python `dict` for the local scope.
 * Note that, unless otherwise specified, executing Python code stores variables on the local scope.
 * 
 * If the global scope `dict` doesn't have Python's builtins predefined, they will be added to the `dict` whenever
 * Python code is first executed.
 * 
 * `REPY_FN` scopes also create a `REPY_DeferredCleanupHandler` instance, which is used to automatically destroy any
 * helper objects created by other `REPY_FN` macros. The cleanup handler is also capable of releasing 
 * `REPY_Handle` instances and freeing memory using `recomp_free` for you as well, but those cases must be
 * declared manually using `REPY_FN_DEFER_RELEASE` and `REPY_FN_DEFER_RECOMP_FREE`.
 * This execution scope will use the main Python interpreter.
 * 
 * @param globals The Python `dict` to use as a global scope.
 */
#define REPY_FN_SETUP_WITH_GLOBALS(globals) \
REPY_FN_SETUP_INTERP_WITH_GLOBALS(REPY_MAIN_INTERPRETER) 

/**
 * @brief Clean up a inline execution scope by releasing the local scope and popping the last interpreter index.
 * 
 * The `REPY_DeferredCleanupHelper` will perform all queued cleanup tasks at this time.
 * 
 * The global scope is only released if the global and local scopes are the same.
 */
#define REPY_FN_CLEANUP \
REPY_DeferredCleanupHelper_Destroy(REPY_FN_AUTO_CLEANUP); \
REPY_Release(REPY_FN_LOCAL_SCOPE); \
REPY_PopInterpreter() \

/**
 * @brief Clean up a inline scope by releasing the local scope and popping the last interpreter index, then returns.
 * 
 * The `REPY_DeferredCleanupHelper` will perform all queued cleanup tasks at this time.
 * 
 * Supports returning a value, which is captured before cleanup starts.
 * 
 * The global scope is only released if the global and local scopes are the same.
 */
#define REPY_FN_RETURN(retType, retVal) \
retType __repy_retVal = retVal; \
REPY_DeferredCleanupHelper_Destroy(REPY_FN_AUTO_CLEANUP, 1); \
REPY_Release(REPY_FN_LOCAL_SCOPE); \
REPY_PopInterpreter(); \
return retVal 

/**
 * @brief Registers a `REPY_Handle` to be released by the `REPY_DeferredCleanupHelper` for this `REPY_FN` scope.
 * 
 * This handle will now be released when the scope is cleaned up. Do not make the handle Single-Use or
 * otherwise release the handle manually before that point.
 * 
 * @param handle a `REPY_Handle` to be released on scope cleanup.
 */
#define REPY_FN_DEFER_RELEASE(handle) \
REPY_DeferredCleanupHelper_AddHandle(REPY_FN_AUTO_CLEANUP, handle) 

/**
 * @brief Registers a `void*` to be deallocated by the `REPY_DeferredCleanupHelper` for this `REPY_FN` scope.
 * 
 * This pointer will now be deallocated when the scope is cleaned up. Do not deallocate manually before that point.
 * 
 * Deallocation is handled by calling `recomp_free`. If this is inapproproate for whatever you want to deallocate,
 * do not use this mechanism.
 * 
 * @param pointer a `void*` to be released on scope cleanup.
 */
#define REPY_FN_DEFER_RECOMP_FREE(pointer) \
REPY_DeferredCleanupHelper_AddRecompFree(REPY_FN_AUTO_CLEANUP, (void*) pointer) 

/** @}*/

/** \defgroup repy_fn_exec REPY_FN_EXEC - Python Scoped Inline Code Execution.
 * 
 * Parse and execute Python code strings within this scope. An extremely flexable way of interweaving mod code and Python code.
 *  @{
 */

/**
 * @brief Executes Python code object within the current inline execution scope.
 * 
 * The code to execute should already be a Python object. A precompiled bytecode object is recommended for performance
 * reasons, but a Python `str` object will also work.
 * 
 * @param code_handle The Python code to execute. Should be a `REPY_Handle` to a valid code object.
 * @return 1 if execution was successful, 0 if there was an error.
 */
#define REPY_FN_EXEC(code_handle) \
REPY_Exec(code_handle, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE) 

/**
 * @brief Executes Python code string string within the current inline execution scope.
 * 
 * Not generally recommended, since this will require recompiling the Python code string every time it's run.
 * It is technically faster than `REPY_FN_EXEC_CACHE` for code that is only used once (since one fewer handle lookup is involved),
 * but the difference is so marginal that it really doesn't matter.
 * 
 * @param code_str The Python code to execute. Should be a NULL-terminated C-string such as a string literal or a `const char*`.
 * @return 1 if execution was successful, 0 if there was an error.
 */
#define REPY_FN_EXEC_CSTR(code_str) \
REPY_ExecCStr(code_str, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE) 

/**
 * @brief Executes a Python code string string within the current inline execution scope, compiling it
 * the first time it's run and caching the bytecode for subsequent uses.
 * 
 * This is the recommended method of inlining executable blocks of Python code inside of functions. The performance difference
 * made by not having to recompile the code strings into bytecode for every run is substantial.
 * 
 * Because this macro expands to a block of code, rather than a single funtion call, the success of the execution is stored in a
 * variable named `[identifier]_success`. This variable will be 1 if execution was successful, 0 if there was an error.
 * 
 * @param identifier The name for a static variable that will hold the Python bytecode handle once created.
 * @param code_str The Python code to execute. Should be a NULL-terminated C-string such as a string literal.
 * This code string will only be parsed and compiled once.
 */
#define REPY_FN_EXEC_CACHE(identifier, code_str) \
REPY_INLINE_COMPILE_CACHE_BLOCK("REPY_FN_EXEC_CACHE", identifier, REPY_CODE_EXEC, code_str) \
REPY_bool identifier ## _success = REPY_FN_EXEC(identifier) 

/** @}*/

/** \defgroup repy_fn_eval REPY_FN_EVAL - Python Scope Expression Evaluation.
 * 
 * Evaluate Python code expressions and get the results. For convienience, many common casting operations have their own
 * dedicated variant macro.
 * 
 *  @{
 */

/**
 * @brief Evaluates a Python expression code object within the current inline execution scope, and returns the result.
 * 
 * The expression to evaluate should already be a Python object. A precompiled bytecode object is recommended for performance
 * reasons, but a Python `str` object will also work.
 * 
 * @param code_handle The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return a `REPY_Handle` for the resultant Python object. Will be `REPY_NO_OBJECT` if an error has occured.
 */
#define REPY_FN_EVAL(code_handle) \
REPY_Eval(code_handle, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE) 

/**
 * @brief Evaluates a Python expression code object within the current inline execution scope, and returns the result as a `bool`.
 * 
 * The expression to evaluate should already be a Python object. A precompiled bytecode object is recommended for performance
 * reasons, but a Python `str` object will also work.
 * 
 * @param code_handle The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `bool`.
 */
#define REPY_FN_EVAL_BOOL(code_handle) \
REPY_CastBool(REPY_MakeSUH(REPY_Eval(code_handle, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))

/**
 * @brief Evaluates a Python expression code object within the current inline execution scope, and returns the result as a `u8`.
 * 
 * The expression to evaluate should already be a Python object. A precompiled bytecode object is recommended for performance
 * reasons, but a Python `str` object will also work.
 * 
 * @param code_handle The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `u8`.
 */
#define REPY_FN_EVAL_U8(code_handle) \
REPY_CastU8(REPY_MakeSUH(REPY_Eval(code_handle, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))

/**
 * @brief Evaluates a Python expression code object within the current inline execution scope, and returns the result as a `s8`.
 * 
 * The expression to evaluate should already be a Python object. A precompiled bytecode object is recommended for performance
 * reasons, but a Python `str` object will also work.
 * 
 * @param code_handle The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `s8`.
 */
#define REPY_FN_EVAL_S8(code_handle) \
REPY_CastS8(REPY_MakeSUH(REPY_Eval(code_handle, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))

/**
 * @brief Evaluates a Python expression code object within the current inline execution scope, and returns the result as a `u16`.
 * 
 * The expression to evaluate should already be a Python object. A precompiled bytecode object is recommended for performance
 * reasons, but a Python `str` object will also work.
 * 
 * @param code_handle The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `u16`.
 */
#define REPY_FN_EVAL_U16(code_handle) \
REPY_CastU16(REPY_MakeSUH(REPY_Eval(code_handle, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))

/**
 * @brief Evaluates a Python expression code object within the current inline execution scope, and returns the result as a `s16`.
 * 
 * The expression to evaluate should already be a Python object. A precompiled bytecode object is recommended for performance
 * reasons, but a Python `str` object will also work.
 * 
 * @param code_handle The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `s16`.
 */
#define REPY_FN_EVAL_S16(code_handle) \
REPY_CastS16(REPY_MakeSUH(REPY_Eval(code_handle, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))

/**
 * @brief Evaluates a Python expression code object within the current inline execution scope, and returns the result as a `u32`.
 * 
 * The expression to evaluate should already be a Python object. A precompiled bytecode object is recommended for performance
 * reasons, but a Python `str` object will also work.
 * 
 * @param code_handle The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `u32`.
 */
#define REPY_FN_EVAL_U32(code_handle) \
REPY_CastU32(REPY_MakeSUH(REPY_Eval(code_handle, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))

/**
 * @brief Evaluates a Python expression code object within the current inline execution scope, and returns the result as a `s32`.
 * 
 * The expression to evaluate should already be a Python object. A precompiled bytecode object is recommended for performance
 * reasons, but a Python `str` object will also work.
 * 
 * @param code_handle The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `s32`.
 */
#define REPY_FN_EVAL_S32(code_handle) \
REPY_CastS32(REPY_MakeSUH(REPY_Eval(code_handle, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))

/**
 * @brief Evaluates a Python expression code object within the current inline execution scope, and returns the result as a `f32`.
 * 
 * The expression to evaluate should already be a Python object. A precompiled bytecode object is recommended for performance
 * reasons, but a Python `str` object will also work.
 * 
 * @param code_handle The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `f32`.
 */
#define REPY_FN_EVAL_F32(code_handle) \
REPY_CastF32(REPY_MakeSUH(REPY_Eval(code_handle, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))

/**
 * @brief Evaluates a Python expression code object within the current inline execution scope, and returns the result as a `u64`.
 * 
 * The expression to evaluate should already be a Python object. A precompiled bytecode object is recommended for performance
 * reasons, but a Python `str` object will also work.
 * 
 * @param code_handle The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `u64`.
 */
#define REPY_FN_EVAL_U64(code_handle) \
REPY_CastU64(REPY_MakeSUH(REPY_Eval(code_handle, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))

/**
 * @brief Evaluates a Python expression code object within the current inline execution scope, and returns the result as a `s64`.
 * 
 * The expression to evaluate should already be a Python object. A precompiled bytecode object is recommended for performance
 * reasons, but a Python `str` object will also work.
 * 
 * @param code_handle The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `s64`.
 */
#define REPY_FN_EVAL_S64(code_handle) \
REPY_CastS64(REPY_MakeSUH(REPY_Eval(code_handle, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))

/**
 * @brief Evaluates a Python expression code object within the current inline execution scope, and returns the result as a `f64`.
 * 
 * The expression to evaluate should already be a Python object. A precompiled bytecode object is recommended for performance
 * reasons, but a Python `str` object will also work.
 * 
 * @param code_handle The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `f64`.
 */
#define REPY_FN_EVAL_F64(code_handle) \
REPY_CastF64(REPY_MakeSUH(REPY_Eval(code_handle, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))

/**
 * @brief Evaluates a Python expression code object within the current inline execution scope, and returns the result as a `char*` C string.
 * 
 * The expression to evaluate should already be a Python object. A precompiled bytecode object is recommended for performance
 * reasons, but a Python `str` object will also work.
 * 
 * Intended to be used when the result of the evaluation is a Python `str`. 
 * 
 * The C string returned by this macro will need to be freed with `recomp_free`. Failure to do so will result in a memory leak.
 * 
 * @param code_handle The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `char*`.
 */
#define REPY_FN_EVAL_STR(code_handle) \
REPY_CastStr(REPY_MakeSUH(REPY_Eval(code_handle, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))


/**
 * @brief Evaluates a Python expression code object within the current inline execution scope, and returns the result as a `char*` C string.
 * 
 * The expression to evaluate should already be a Python object. A precompiled bytecode object is recommended for performance
 * reasons, but a Python `str` object will also work.
 * 
 * Intended to be used when the result of the evaluation is a Python `bytes` object. 
 * 
 * The C string returned by this macro will need to be freed with `recomp_free`. Failure to do so will result in a memory leak.
 * 
 * @param code_handle The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `char*`.
 */
#define REPY_FN_EVAL_BYTESTR(code_handle) \
REPY_CastByteStr(REPY_MakeSUH(REPY_Eval(code_handle, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))

/**
 * @brief Evaluates a Python expression code string within the current inline execution scope, and returns the result.
 * 
 * Not generally recommended, since this will require recompiling the Python code string every time it's run, which will result in significant slowdown.
 * It is technically faster than `REPY_FN_EVAL_CACHE` for code that is only used once (since one fewer handle lookup is involved),
 * but the difference is so marginal that it really doesn't matter.
 * 
 * @param code_str The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return a `REPY_Handle` for the resultant Python object. Will be `REPY_NO_OBJECT` if an error has occured.
 */
#define REPY_FN_EVAL_CSTR(code_str) \
REPY_EvalCStr(code_str, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE) 

/**
 * @brief Evaluates a Python expression code string within the current inline execution scope, and returns the result as a `bool`.
 * 
 * Not generally recommended, since this will require recompiling the Python code string every time it's run, which will result in significant slowdown.
 * It is technically faster than `REPY_FN_EVAL_CACHE_BOOL` for code that is only used once (since one fewer handle lookup is involved),
 * but the difference is so marginal that it really doesn't matter.
 * 
 * @param code_str The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `bool`.
 */
#define REPY_FN_EVAL_CSTR_BOOL(code_str) \
REPY_CastBool(REPY_MakeSUH(REPY_EvalCStr(code_str, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))

/**
 * @brief Evaluates a Python expression code string within the current inline execution scope, and returns the result as a `u8`.
 * 
 * Not generally recommended, since this will require recompiling the Python code string every time it's run, which will result in significant slowdown.
 * It is technically faster than `REPY_FN_EVAL_CACHE_U8` for code that is only used once (since one fewer handle lookup is involved),
 * but the difference is so marginal that it really doesn't matter.
 * 
 * @param code_str The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `u8`.
 */
#define REPY_FN_EVAL_CSTR_U8(code_str) \
REPY_CastU8(REPY_MakeSUH(REPY_EvalCStr(code_str, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))

/**
 * @brief Evaluates a Python expression code string within the current inline execution scope, and returns the result as a `s8`.
 * 
 * Not generally recommended, since this will require recompiling the Python code string every time it's run, which will result in significant slowdown.
 * It is technically faster than `REPY_FN_EVAL_CACHE_S8` for code that is only used once (since one fewer handle lookup is involved),
 * but the difference is so marginal that it really doesn't matter.
 * 
 * @param code_str The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `s8`.
 */
#define REPY_FN_EVAL_CSTR_S8(code_str) \
REPY_CastS8(REPY_MakeSUH(REPY_EvalCStr(code_str, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))

/**
 * @brief Evaluates a Python expression code string within the current inline execution scope, and returns the result as a `u16`.
 * 
 * Not generally recommended, since this will require recompiling the Python code string every time it's run, which will result in significant slowdown.
 * It is technically faster than `REPY_FN_EVAL_CACHE_U16` for code that is only used once (since one fewer handle lookup is involved),
 * but the difference is so marginal that it really doesn't matter.
 * 
 * @param code_str The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `u16`.
 */
#define REPY_FN_EVAL_CSTR_U16(code_str) \
REPY_CastU16(REPY_MakeSUH(REPY_EvalCStr(code_str, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))

/**
 * @brief Evaluates a Python expression code string within the current inline execution scope, and returns the result as a `s16`.
 * 
 * Not generally recommended, since this will require recompiling the Python code string every time it's run, which will result in significant slowdown.
 * It is technically faster than `REPY_FN_EVAL_CACHE_S16` for code that is only used once (since one fewer handle lookup is involved),
 * but the difference is so marginal that it really doesn't matter.
 * 
 * @param code_str The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `s16`.
 */
#define REPY_FN_EVAL_CSTR_S16(code_str) \
REPY_CastS16(REPY_MakeSUH(REPY_EvalCStr(code_str, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))

/**
 * @brief Evaluates a Python expression code string within the current inline execution scope, and returns the result as a `u32`.
 * 
 * Not generally recommended, since this will require recompiling the Python code string every time it's run, which will result in significant slowdown.
 * It is technically faster than `REPY_FN_EVAL_CACHE_U32` for code that is only used once (since one fewer handle lookup is involved),
 * but the difference is so marginal that it really doesn't matter.
 * 
 * @param code_str The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `u32`.
 */
#define REPY_FN_EVAL_CSTR_U32(code_str) \
REPY_CastU32(REPY_MakeSUH(REPY_EvalCStr(code_str, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))

/**
 * @brief Evaluates a Python expression code string within the current inline execution scope, and returns the result as a `s32`.
 * 
 * Not generally recommended, since this will require recompiling the Python code string every time it's run, which will result in significant slowdown.
 * It is technically faster than `REPY_FN_EVAL_CACHE_S32` for code that is only used once (since one fewer handle lookup is involved),
 * but the difference is so marginal that it really doesn't matter.
 * 
 * @param code_str The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `s32`.
 */
#define REPY_FN_EVAL_CSTR_S32(code_str) \
REPY_CastS32(REPY_MakeSUH(REPY_EvalCStr(code_str, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))

/**
 * @brief Evaluates a Python expression code string within the current inline execution scope, and returns the result as a `f32`.
 * 
 * Not generally recommended, since this will require recompiling the Python code string every time it's run, which will result in significant slowdown.
 * It is technically faster than `REPY_FN_EVAL_CACHE_F32` for code that is only used once (since one fewer handle lookup is involved),
 * but the difference is so marginal that it really doesn't matter.
 * 
 * @param code_str The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `f32`.
 */
#define REPY_FN_EVAL_CSTR_F32(code_str) \
REPY_CastF32(REPY_MakeSUH(REPY_EvalCStr(code_str, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))

/**
 * @brief Evaluates a Python expression code string within the current inline execution scope, and returns the result as a `u64`.
 * 
 * Not generally recommended, since this will require recompiling the Python code string every time it's run, which will result in significant slowdown.
 * It is technically faster than `REPY_FN_EVAL_CACHE_U64` for code that is only used once (since one fewer handle lookup is involved),
 * but the difference is so marginal that it really doesn't matter.
 * 
 * @param code_str The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `u64`.
 */
#define REPY_FN_EVAL_CSTR_U64(code_str) \
REPY_CastU64(REPY_MakeSUH(REPY_EvalCStr(code_str, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))

/**
 * @brief Evaluates a Python expression code string within the current inline execution scope, and returns the result as a `s64`.
 * 
 * Not generally recommended, since this will require recompiling the Python code string every time it's run, which will result in significant slowdown.
 * It is technically faster than `REPY_FN_EVAL_CACHE_S64` for code that is only used once (since one fewer handle lookup is involved),
 * but the difference is so marginal that it really doesn't matter.
 * 
 * @param code_str The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `s64`.
 */
#define REPY_FN_EVAL_CSTR_S64(code_str) \
REPY_CastS64(REPY_MakeSUH(REPY_EvalCStr(code_str, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))

/**
 * @brief Evaluates a Python expression code string within the current inline execution scope, and returns the result as a `f64`.
 * 
 * Not generally recommended, since this will require recompiling the Python code string every time it's run, which will result in significant slowdown.
 * It is technically faster than `REPY_FN_EVAL_CACHE_F64` for code that is only used once (since one fewer handle lookup is involved),
 * but the difference is so marginal that it really doesn't matter.
 * 
 * @param code_str The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `f64`.
 */
#define REPY_FN_EVAL_CSTR_F64(code_str) \
REPY_CastF64(REPY_MakeSUH(REPY_EvalCStr(code_str, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))

/**
 * @brief Evaluates a Python expression code string within the current inline execution scope, and returns the result as a `char*`.
 * 
 * Not generally recommended, since this will require recompiling the Python code string every time it's run, which will result in significant slowdown.
 * It is technically faster than `REPY_FN_EVAL_CACHE_STR` for code that is only used once (since one fewer handle lookup is involved),
 * but the difference is so marginal that it really doesn't matter.
 * 
 * Intended to be used when the result of the evaluation is a Python `str`. 
 * 
 * The C string returned by this macro will need to be freed with `recomp_free`. Failure to do so will result in a memory leak.
 * 
 * @param code_str The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `bool`.
 */
#define REPY_FN_EVAL_CSTR_STR(code_str) \
REPY_CastStr(REPY_MakeSUH(REPY_EvalCStr(code_str, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))

/**
 * @brief Evaluates a Python expression code string within the current inline execution scope, and returns the result as a `char*`.
 * 
 * Not generally recommended, since this will require recompiling the Python code string every time it's run, which will result in significant slowdown.
 * It is technically faster than `REPY_FN_EVAL_CACHE_BYTESTR` for code that is only used once (since one fewer handle lookup is involved),
 * but the difference is so marginal that it really doesn't matter.
 * 
 * Intended to be used when the result of the evaluation is a Python `bytes` object. 
 * 
 * The C string returned by this macro will need to be freed with `recomp_free`. Failure to do so will result in a memory leak.
 * 
 * @param code_str The Python expression to evaluate. Should be a `REPY_Handle` to a valid code object.
 * @return The resultant Python object, cast to `char*`.
 */
#define REPY_FN_EVAL_CSTR_BYTESTR(code_str) \
REPY_CastByteStr(REPY_MakeSUH(REPY_EvalCStr(code_str, REPY_FN_GLOBAL_SCOPE, REPY_FN_LOCAL_SCOPE)))

/**
 * @brief Evaluate a Python expression code string within the current inline execution scope, compiling it
 * the first time it's run and caching the bytecode for subsequent uses, and storing its result as a `REPY_Handle`.
 * 
 * The various `REPY_FN_EVAL_CACHE` macros are the recommended method of inlining Python expression evaluations inside of functions.
 * The performance difference made by not having to recompile the code strings into bytecode for every run is substantial.
 * 
 * Because this macro expands to a block of code, rather than a single funtion call, the evaluation is stored in a `REPY_Handle`
 * variable named using the `out_var` argument. If `out_var` is `REPY_NO_OBJECT`, a Python error has occured.
 * 
 * @param bytecode_identifier The name for a static variable that will hold the Python bytecode handle once created.
 * @param code_str The Python expression to evaluate. Should be a NULL-terminated C-string such as a string literal.
 * This code string will only be parsed and compiled once.
 * @param out_var The name of a `REPY_Handle` argument that will hold the expression result.
 */
#define REPY_FN_EVAL_CACHE(bytecode_identifier, code_str, out_var) \
REPY_INLINE_COMPILE_CACHE_BLOCK("REPY_FN_EVAL_CACHE", bytecode_identifier, REPY_CODE_EVAL, code_str) \
REPY_Handle out_var = REPY_FN_EVAL(bytecode_identifier)

/**
 * @brief Evaluate a Python expression code string within the current inline execution scope, compiling it
 * the first time it's run and caching the bytecode for subsequent uses, and storing its result as a `bool`.
 * 
 * The various `REPY_FN_EVAL_CACHE` macros are the recommended method of inlining Python expression evaluations inside of functions.
 * The performance difference made by not having to recompile the code strings into bytecode for every run is substantial.
 * 
 * Because this macro expands to a block of code, rather than a single funtion call, the evaluation is stored in a `bool`
 * variable named using the `out_var` argument.
 * 
 * @param bytecode_identifier The name for a static variable that will hold the Python bytecode handle once created.
 * @param code_str The Python expression to evaluate. Should be a NULL-terminated C-string such as a string literal.
 * This code string will only be parsed and compiled once.
 * @param out_var The name of a `bool` argument that will hold the expression result.
 */
#define REPY_FN_EVAL_CACHE_BOOL(bytecode_identifier, code_str, out_var) \
REPY_INLINE_COMPILE_CACHE_BLOCK("REPY_FN_EVAL_CACHE_BOOL", bytecode_identifier, REPY_CODE_EVAL, code_str) \
bool out_var = REPY_FN_EVAL_BOOL(bytecode_identifier)

/**
 * @brief Evaluate a Python expression code string within the current inline execution scope, compiling it
 * the first time it's run and caching the bytecode for subsequent uses, and storing its result as a `u8`.
 * 
 * The various `REPY_FN_EVAL_CACHE` macros are the recommended method of inlining Python expression evaluations inside of functions.
 * The performance difference made by not having to recompile the code strings into bytecode for every run is substantial.
 * 
 * Because this macro expands to a block of code, rather than a single funtion call, the evaluation is stored in a `u8`
 * variable named using the `out_var` argument.
 * 
 * @param bytecode_identifier The name for a static variable that will hold the Python bytecode handle once created.
 * @param code_str The Python expression to evaluate. Should be a NULL-terminated C-string such as a string literal.
 * This code string will only be parsed and compiled once.
 * @param out_var The name of a `u8` argument that will hold the expression result.
 */
#define REPY_FN_EVAL_CACHE_U8(bytecode_identifier, code_str, out_var) \
REPY_INLINE_COMPILE_CACHE_BLOCK("REPY_FN_EVAL_CACHE_U8", bytecode_identifier, REPY_CODE_EVAL, code_str) \
REPY_u8 out_var = REPY_FN_EVAL_U8(bytecode_identifier)

/**
 * @brief Evaluate a Python expression code string within the current inline execution scope, compiling it
 * the first time it's run and caching the bytecode for subsequent uses, and storing its result as a `s8`.
 * 
 * The various `REPY_FN_EVAL_CACHE` macros are the recommended method of inlining Python expression evaluations inside of functions.
 * The performance difference made by not having to recompile the code strings into bytecode for every run is substantial.
 * 
 * Because this macro expands to a block of code, rather than a single funtion call, the evaluation is stored in a `s8`
 * variable named using the `out_var` argument.
 * 
 * @param bytecode_identifier The name for a static variable that will hold the Python bytecode handle once created.
 * @param code_str The Python expression to evaluate. Should be a NULL-terminated C-string such as a string literal.
 * This code string will only be parsed and compiled once.
 * @param out_var The name of a `s8` argument that will hold the expression result.
 */
#define REPY_FN_EVAL_CACHE_S8(bytecode_identifier, code_str, out_var) \
REPY_INLINE_COMPILE_CACHE_BLOCK("REPY_FN_EVAL_CACHE_S8", bytecode_identifier, REPY_CODE_EVAL, code_str) \
REPY_s8 out_var = REPY_FN_EVAL_S8(bytecode_identifier)

/**
 * @brief Evaluate a Python expression code string within the current inline execution scope, compiling it
 * the first time it's run and caching the bytecode for subsequent uses, and storing its result as a `u16`.
 * 
 * The various `REPY_FN_EVAL_CACHE` macros are the recommended method of inlining Python expression evaluations inside of functions.
 * The performance difference made by not having to recompile the code strings into bytecode for every run is substantial.
 * 
 * Because this macro expands to a block of code, rather than a single funtion call, the evaluation is stored in a `u16`
 * variable named using the `out_var` argument.
 * 
 * @param bytecode_identifier The name for a static variable that will hold the Python bytecode handle once created.
 * @param code_str The Python expression to evaluate. Should be a NULL-terminated C-string such as a string literal.
 * This code string will only be parsed and compiled once.
 * @param out_var The name of a `u16` argument that will hold the expression result.
 */
#define REPY_FN_EVAL_CACHE_U16(bytecode_identifier, code_str, out_var) \
REPY_INLINE_COMPILE_CACHE_BLOCK("REPY_FN_EVAL_CACHE_U16", bytecode_identifier, REPY_CODE_EVAL, code_str) \
REPY_u16 out_var = REPY_FN_EVAL_U16(bytecode_identifier)

/**
 * @brief Evaluate a Python expression code string within the current inline execution scope, compiling it
 * the first time it's run and caching the bytecode for subsequent uses, and storing its result as a `s16`.
 * 
 * The various `REPY_FN_EVAL_CACHE` macros are the recommended method of inlining Python expression evaluations inside of functions.
 * The performance difference made by not having to recompile the code strings into bytecode for every run is substantial.
 * 
 * Because this macro expands to a block of code, rather than a single funtion call, the evaluation is stored in a `s16`
 * variable named using the `out_var` argument.
 * 
 * @param bytecode_identifier The name for a static variable that will hold the Python bytecode handle once created.
 * @param code_str The Python expression to evaluate. Should be a NULL-terminated C-string such as a string literal.
 * This code string will only be parsed and compiled once.
 * @param out_var The name of a `s16` argument that will hold the expression result.
 */
#define REPY_FN_EVAL_CACHE_S16(bytecode_identifier, code_str, out_var) \
REPY_INLINE_COMPILE_CACHE_BLOCK("REPY_FN_EVAL_CACHE_S16", bytecode_identifier, REPY_CODE_EVAL, code_str) \
REPY_s16 out_var = REPY_FN_EVAL_S16(bytecode_identifier)

/**
 * @brief Evaluate a Python expression code string within the current inline execution scope, compiling it
 * the first time it's run and caching the bytecode for subsequent uses, and storing its result as a `u32`.
 * 
 * The various `REPY_FN_EVAL_CACHE` macros are the recommended method of inlining Python expression evaluations inside of functions.
 * The performance difference made by not having to recompile the code strings into bytecode for every run is substantial.
 * 
 * Because this macro expands to a block of code, rather than a single funtion call, the evaluation is stored in a `u32`
 * variable named using the `out_var` argument.
 * 
 * @param bytecode_identifier The name for a static variable that will hold the Python bytecode handle once created.
 * @param code_str The Python expression to evaluate. Should be a NULL-terminated C-string such as a string literal.
 * This code string will only be parsed and compiled once.
 * @param out_var The name of a `u32` argument that will hold the expression result.
 */
#define REPY_FN_EVAL_CACHE_U32(bytecode_identifier, code_str, out_var) \
REPY_INLINE_COMPILE_CACHE_BLOCK("REPY_FN_EVAL_CACHE_U32", bytecode_identifier, REPY_CODE_EVAL, code_str) \
REPY_u32 out_var = REPY_FN_EVAL_U32(bytecode_identifier)

/**
 * @brief Evaluate a Python expression code string within the current inline execution scope, compiling it
 * the first time it's run and caching the bytecode for subsequent uses, and storing its result as a `s32`.
 * 
 * The various `REPY_FN_EVAL_CACHE` macros are the recommended method of inlining Python expression evaluations inside of functions.
 * The performance difference made by not having to recompile the code strings into bytecode for every run is substantial.
 * 
 * Because this macro expands to a block of code, rather than a single funtion call, the evaluation is stored in a `s32`
 * variable named using the `out_var` argument.
 * 
 * @param bytecode_identifier The name for a static variable that will hold the Python bytecode handle once created.
 * @param code_str The Python expression to evaluate. Should be a NULL-terminated C-string such as a string literal.
 * This code string will only be parsed and compiled once.
 * @param out_var The name of a `s32` argument that will hold the expression result.
 */
#define REPY_FN_EVAL_CACHE_S32(bytecode_identifier, code_str, out_var) \
REPY_INLINE_COMPILE_CACHE_BLOCK("REPY_FN_EVAL_CACHE_S32", bytecode_identifier, REPY_CODE_EVAL, code_str) \
REPY_s32 out_var = REPY_FN_EVAL_S32(bytecode_identifier)

/**
 * @brief Evaluate a Python expression code string within the current inline execution scope, compiling it
 * the first time it's run and caching the bytecode for subsequent uses, and storing its result as a `f32`.
 * 
 * The various `REPY_FN_EVAL_CACHE` macros are the recommended method of inlining Python expression evaluations inside of functions.
 * The performance difference made by not having to recompile the code strings into bytecode for every run is substantial.
 * 
 * Because this macro expands to a block of code, rather than a single funtion call, the evaluation is stored in a `f32`
 * variable named using the `out_var` argument.
 * 
 * @param bytecode_identifier The name for a static variable that will hold the Python bytecode handle once created.
 * @param code_str The Python expression to evaluate. Should be a NULL-terminated C-string such as a string literal.
 * This code string will only be parsed and compiled once.
 * @param out_var The name of a `f32` argument that will hold the expression result.
 */
#define REPY_FN_EVAL_CACHE_F32(bytecode_identifier, code_str, out_var) \
REPY_INLINE_COMPILE_CACHE_BLOCK("REPY_FN_EVAL_CACHE_F32", bytecode_identifier, REPY_CODE_EVAL, code_str) \
REPY_f32 out_var = REPY_FN_EVAL_F32(bytecode_identifier)

/**
 * @brief Evaluate a Python expression code string within the current inline execution scope, compiling it
 * the first time it's run and caching the bytecode for subsequent uses, and storing its result as a `u64`.
 * 
 * The various `REPY_FN_EVAL_CACHE` macros are the recommended method of inlining Python expression evaluations inside of functions.
 * The performance difference made by not having to recompile the code strings into bytecode for every run is substantial.
 * 
 * Because this macro expands to a block of code, rather than a single funtion call, the evaluation is stored in a `u64`
 * variable named using the `out_var` argument.
 * 
 * @param bytecode_identifier The name for a static variable that will hold the Python bytecode handle once created.
 * @param code_str The Python expression to evaluate. Should be a NULL-terminated C-string such as a string literal.
 * This code string will only be parsed and compiled once.
 * @param out_var The name of a `u64` argument that will hold the expression result.
 */
#define REPY_FN_EVAL_CACHE_U64(bytecode_identifier, code_str, out_var) \
REPY_INLINE_COMPILE_CACHE_BLOCK("REPY_FN_EVAL_CACHE_U64", bytecode_identifier, REPY_CODE_EVAL, code_str) \
REPY_u64 out_var = REPY_FN_EVAL_U64(bytecode_identifier)

/**
 * @brief Evaluate a Python expression code string within the current inline execution scope, compiling it
 * the first time it's run and caching the bytecode for subsequent uses, and storing its result as a `s64`.
 * 
 * The various `REPY_FN_EVAL_CACHE` macros are the recommended method of inlining Python expression evaluations inside of functions.
 * The performance difference made by not having to recompile the code strings into bytecode for every run is substantial.
 * 
 * Because this macro expands to a block of code, rather than a single funtion call, the evaluation is stored in a `s64`
 * variable named using the `out_var` argument.
 * 
 * @param bytecode_identifier The name for a static variable that will hold the Python bytecode handle once created.
 * @param code_str The Python expression to evaluate. Should be a NULL-terminated C-string such as a string literal.
 * This code string will only be parsed and compiled once.
 * @param out_var The name of a `s64` argument that will hold the expression result.
 */
#define REPY_FN_EVAL_CACHE_S64(bytecode_identifier, code_str, out_var) \
REPY_INLINE_COMPILE_CACHE_BLOCK("REPY_FN_EVAL_CACHE_S64", bytecode_identifier, REPY_CODE_EVAL, code_str) \
REPY_s64 out_var = REPY_FN_EVAL_S64(bytecode_identifier)

/**
 * @brief Evaluate a Python expression code string within the current inline execution scope, compiling it
 * the first time it's run and caching the bytecode for subsequent uses, and storing its result as a `f64`.
 * 
 * The various `REPY_FN_EVAL_CACHE` macros are the recommended method of inlining Python expression evaluations inside of functions.
 * The performance difference made by not having to recompile the code strings into bytecode for every run is substantial.
 * 
 * Because this macro expands to a block of code, rather than a single funtion call, the evaluation is stored in a `f64`
 * variable named using the `out_var` argument.
 * 
 * @param bytecode_identifier The name for a static variable that will hold the Python bytecode handle once created.
 * @param code_str The Python expression to evaluate. Should be a NULL-terminated C-string such as a string literal.
 * This code string will only be parsed and compiled once.
 * @param out_var The name of a `f64` argument that will hold the expression result.
 */
#define REPY_FN_EVAL_CACHE_F64(bytecode_identifier, code_str, out_var) \
REPY_INLINE_COMPILE_CACHE_BLOCK("REPY_FN_EVAL_CACHE_F64", bytecode_identifier, REPY_CODE_EVAL, code_str) \
REPY_f64 out_var = REPY_FN_EVAL_F64(bytecode_identifier)

/**
 * @brief Evaluate a Python expression code string within the current inline execution scope, compiling it
 * the first time it's run and caching the bytecode for subsequent uses, and storing its result as a `char*`.
 * 
 * The various `REPY_FN_EVAL_CACHE` macros are the recommended method of inlining Python expression evaluations inside of functions.
 * The performance difference made by not having to recompile the code strings into bytecode for every run is substantial.
 * 
 * Because this macro expands to a block of code, rather than a single funtion call, the evaluation is stored in a `char*`
 * variable named using the `out_var` argument. The C string stored in `out_var` will need to be freed with `recomp_free`.
 * Failure to do so will result in a memory leak.
 * 
 * Intended to be used when the result of the evaluation is a Python `str` object. 
 * 
 * @param bytecode_identifier The name for a static variable that will hold the Python bytecode handle once created.
 * @param code_str The Python expression to evaluate. Should be a NULL-terminated C-string such as a string literal.
 * This code string will only be parsed and compiled once.
 * @param out_var The name of a `char*` argument that will hold the expression result.
 */
#define REPY_FN_EVAL_CACHE_STR(bytecode_identifier, code_str, out_var) \
REPY_INLINE_COMPILE_CACHE_BLOCK("REPY_FN_EVAL_CACHE_STR", bytecode_identifier, REPY_CODE_EVAL, code_str) \
char* out_var = REPY_FN_EVAL_STR(bytecode_identifier)

/**
 * @brief Evaluate a Python expression code string within the current inline execution scope, compiling it
 * the first time it's run and caching the bytecode for subsequent uses, and storing its result as a `char*`.
 * 
 * The various `REPY_FN_EVAL_CACHE` macros are the recommended method of inlining Python expression evaluations inside of functions.
 * The performance difference made by not having to recompile the code strings into bytecode for every run is substantial.
 * 
 * Because this macro expands to a block of code, rather than a single funtion call, the evaluation is stored in a `char*`
 * variable named using the `out_var` argument. The C string stored in `out_var` will need to be freed with `recomp_free`.
 * Failure to do so will result in a memory leak.
 * 
 * Intended to be used when the result of the evaluation is a Python `bytes` object. 
 * 
 * @param bytecode_identifier The name for a static variable that will hold the Python bytecode handle once created.
 * @param code_str The Python expression to evaluate. Should be a NULL-terminated C-string such as a string literal.
 * This code string will only be parsed and compiled once.
 * @param out_var The name of a `char*` argument that will hold the expression result.
 */
#define REPY_FN_EVAL_CACHE_BYTESTR(bytecode_identifier, code_str, out_var) \
REPY_INLINE_COMPILE_CACHE_BLOCK("REPY_FN_EVAL_CACHE_BYTESTR", bytecode_identifier, REPY_CODE_EVAL, code_str) \
char* out_var = REPY_FN_EVAL_BYTESTR(bytecode_identifier)

/** @}*/

/** \defgroup repy_fn_getset REPY_FN_GET/SET - Python Scope Namespace Management.
 * 
 * Add variables to the Python scope, and retrieve handles and values. For convenience, many common casting 
 * operations have their own variant macros.
 * 
 * @{
 */

/**
 * @brief Inserts a Python module into the local scope. 
 * 
 * The variable name for the module will be the same as the module name.
 * 
 * @param module_name The name of the Python module as a C string.
 */
#define REPY_FN_IMPORT(module_name) \
REPY_DictSetCStr(REPY_FN_LOCAL_SCOPE, module_name, REPY_MakeSUH(REPY_ImportModule(module_name)))


/**
 * @brief Gets a variable from the local scope.
 * 
 * Note that this macro will not retrieve global variables.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @return The value of the variable as a `REPY_Handle`.
 */
#define REPY_FN_GET(var_name) \
REPY_DictGetCStr(REPY_FN_LOCAL_SCOPE, var_name)

/**
 * @brief Sets a variable in the the local scope, using a `REPY_Handle` for the value.
 * 
 * Note that the global scope will be unaffected.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @param py_object The python object to insert. Should be a `REPY_Handle`.
 */
#define REPY_FN_SET(var_name, py_object) \
REPY_DictSetCStr(REPY_FN_LOCAL_SCOPE, var_name, py_object)

/**
 * @brief Gets a variable from the local scope and casts it to a `bool`.
 * 
 * Note that this macro will not retrieve global variables.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @return The value of the variable as a `bool`.
 */
#define REPY_FN_GET_BOOL(var_name) \
REPY_CastBool(REPY_MakeSUH(REPY_DictGetCStr(REPY_FN_LOCAL_SCOPE, var_name)))

/**
 * @brief Sets a variable of the Python type `bool` in the the local scope, using a `bool` for the value.
 * 
 * Note that the global scope will be unaffected.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @param value The python object to insert. Should be a `bool`.
 */
#define REPY_FN_SET_BOOL(var_name, value) \
REPY_DictSetCStr(REPY_FN_LOCAL_SCOPE, var_name, REPY_MakeSUH(REPY_CreateBool(value)))

/**
 * @brief Gets a variable from the local scope and casts it to a `u8`.
 * 
 * Note that this macro will not retrieve global variables.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @return The value of the variable as a `u8`.
 */
#define REPY_FN_GET_U8(var_name) \
REPY_CastU8(REPY_MakeSUH(REPY_DictGetCStr(REPY_FN_LOCAL_SCOPE, var_name)))

/**
 * @brief Sets a variable of the Python type `int` in the the local scope, using a `u8` for the value.
 * 
 * Note that the global scope will be unaffected.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @param value The value of the Python `int`. Should be a `u8`.
 */
#define REPY_FN_SET_U8(var_name, value) \
REPY_DictSetCStr(REPY_FN_LOCAL_SCOPE, var_name, REPY_MakeSUH(REPY_CreateU8(value)))

/**
 * @brief Gets a variable from the local scope and casts it to a `s8`.
 * 
 * Note that this macro will not retrieve global variables.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @return The value of the variable as a `s8`.
 */
#define REPY_FN_GET_S8(var_name) \
REPY_CastS8(REPY_MakeSUH(REPY_DictGetCStr(REPY_FN_LOCAL_SCOPE, var_name)))

/**
 * @brief Sets a variable of the Python type `int` in the the local scope, using a `s8` for the value.
 * 
 * Note that the global scope will be unaffected.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @param value The value of the Python `int`. Should be a `s8`.
 */
#define REPY_FN_SET_S8(var_name, value) \
REPY_DictSetCStr(REPY_FN_LOCAL_SCOPE, var_name, REPY_MakeSUH(REPY_CreateS8(value)))

/**
 * @brief Gets a variable from the local scope and casts it to a `u16`.
 * 
 * Note that this macro will not retrieve global variables.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @return The value of the variable as a `u16`.
 */
#define REPY_FN_GET_U16(var_name) \
REPY_CastU16(REPY_MakeSUH(REPY_DictGetCStr(REPY_FN_LOCAL_SCOPE, var_name)))

/**
 * @brief Sets a variable of the Python type `int` in the the local scope, using a `u16` for the value.
 * 
 * Note that the global scope will be unaffected.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @param value The value of the Python `int`. Should be a `u16`.
 */
#define REPY_FN_SET_U16(var_name, value) \
REPY_DictSetCStr(REPY_FN_LOCAL_SCOPE, var_name, REPY_MakeSUH(REPY_CreateU16(value)))

/**
 * @brief Gets a variable from the local scope and casts it to a `s16`.
 * 
 * Note that this macro will not retrieve global variables.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @return The value of the variable as a `s16`.
 */
#define REPY_FN_GET_S16(var_name) \
REPY_CastS16(REPY_MakeSUH(REPY_DictGetCStr(REPY_FN_LOCAL_SCOPE, var_name)))

/**
 * @brief Sets a variable of the Python type `int` in the the local scope, using a `s16` for the value.
 * 
 * Note that the global scope will be unaffected.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @param value The value of the Python `int`. Should be a `s16`.
 */
#define REPY_FN_SET_S16(var_name, value) \
REPY_DictSetCStr(REPY_FN_LOCAL_SCOPE, var_name, REPY_MakeSUH(REPY_CreateS16(value)))

/**
 * @brief Gets a variable from the local scope and casts it to a `u32`.
 * 
 * Note that this macro will not retrieve global variables.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @return The value of the variable as a `u32`.
 */
#define REPY_FN_GET_U32(var_name) \
REPY_CastU32(REPY_MakeSUH(REPY_DictGetCStr(REPY_FN_LOCAL_SCOPE, var_name)))

/**
 * @brief Sets a variable of the Python type `int` in the the local scope, using a `u32` for the value.
 * 
 * Note that the global scope will be unaffected.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @param value The value of the Python `int`. Should be a `u32`.
 */
#define REPY_FN_SET_U32(var_name, value) \
REPY_DictSetCStr(REPY_FN_LOCAL_SCOPE, var_name, REPY_MakeSUH(REPY_CreateU32(value)))

/**
 * @brief Gets a variable from the local scope and casts it to a `s32`.
 * 
 * Note that this macro will not retrieve global variables.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @return The value of the variable as a `s32`.
 */
#define REPY_FN_GET_S32(var_name) \
REPY_CastS32(REPY_MakeSUH(REPY_DictGetCStr(REPY_FN_LOCAL_SCOPE, var_name)))

/**
 * @brief Sets a variable of the Python type `int` in the the local scope, using a `s32` for the value.
 * 
 * Note that the global scope will be unaffected.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @param value The value of the Python `int`. Should be a `s32`.
 */
#define REPY_FN_SET_S32(var_name, value) \
REPY_DictSetCStr(REPY_FN_LOCAL_SCOPE, var_name, REPY_MakeSUH(REPY_CreateS32(value)))

/**
 * @brief Gets a variable from the local scope and casts it to a `f32`.
 * 
 * Note that this macro will not retrieve global variables.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @return The value of the variable as a `f32`.
 */
#define REPY_FN_GET_F32(var_name) \
REPY_CastF32(REPY_MakeSUH(REPY_DictGetCStr(REPY_FN_LOCAL_SCOPE, var_name)))

/**
 * @brief Sets a variable of the Python type `float` in the the local scope, using a `f32` for the value.
 * 
 * Note that the global scope will be unaffected.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @param value The value of the Python `float`. Should be a `f32`.
 */
#define REPY_FN_SET_F32(var_name, value) \
REPY_DictSetCStr(REPY_FN_LOCAL_SCOPE, var_name, REPY_MakeSUH(REPY_CreateF32(value)))

/**
 * @brief Gets a variable from the local scope and casts it to a `u64`.
 * 
 * Note that this macro will not retrieve global variables.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @return The value of the variable as a `u64`.
 */
#define REPY_FN_GET_U64(var_name) \
REPY_CastU64(REPY_MakeSUH(REPY_DictGetCStr(REPY_FN_LOCAL_SCOPE, var_name)))

/**
 * @brief Sets a variable of the Python type `int` in the the local scope, using a `u64` for the value.
 * 
 * Note that the global scope will be unaffected.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @param value The value of the Python `int`. Should be a `u64`.
 */
#define REPY_FN_SET_U64(var_name, value) \
REPY_DictSetCStr(REPY_FN_LOCAL_SCOPE, var_name, REPY_MakeSUH(REPY_CreateU64(value)))

/**
 * @brief Gets a variable from the local scope and casts it to a `s64`.
 * 
 * Note that this macro will not retrieve global variables.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @return The value of the variable as a `s64`.
 */
#define REPY_FN_GET_S64(var_name) \
REPY_CastS64(REPY_MakeSUH(REPY_DictGetCStr(REPY_FN_LOCAL_SCOPE, var_name)))

/**
 * @brief Sets a variable of the Python type `int` in the the local scope, using a `s64` for the value.
 * 
 * Note that the global scope will be unaffected.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @param value The value of the Python `int`. Should be a `s64`.
 */
#define REPY_FN_SET_S64(var_name, value) \
REPY_DictSetCStr(REPY_FN_LOCAL_SCOPE, var_name, REPY_MakeSUH(REPY_CreateS64(value)))

/**
 * @brief Gets a variable from the local scope and casts it to a `f64`.
 * 
 * Note that this macro will not retrieve global variables.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @return The value of the variable as a `f64`.
 */
#define REPY_FN_GET_F64(var_name) \
REPY_CastF64(REPY_MakeSUH(REPY_DictGetCStr(REPY_FN_LOCAL_SCOPE, var_name)))

/**
 * @brief Sets a variable of the Python type `float` in the the local scope, using a `f64` for the value.
 * 
 * Note that the global scope will be unaffected.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @param value The value of the Python `float`. Should be a `f64`.
 */
#define REPY_FN_SET_F64(var_name, value) \
REPY_DictSetCStr(REPY_FN_LOCAL_SCOPE, var_name, REPY_MakeSUH(REPY_CreateF64(value)))

/**
 * @brief Gets a variable from the local scope and casts it to a `char*`. Intended to be used when the variable object is a Python `str`. 
 * 
 * Note that this macro will not retrieve global variables.
 * 
 * The C string returned by this macro will need to be freed with `recomp_free`. Failure to do so will result in a memory leak.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @return The value of the variable as a `char*`.
 */
#define REPY_FN_GET_STR(var_name) \
REPY_CastStr(REPY_MakeSUH(REPY_DictGetCStr(REPY_FN_LOCAL_SCOPE, var_name)))

/**
 * @brief Sets a variable of the Python type `str` in the the local scope, using a  NULL-terminated C string.
 * 
 * Note that the global scope will be unaffected.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @param value The value of the Python `str`. Should be a NULL-terminated C string.
 */
#define REPY_FN_SET_STR(var_name, value) \
REPY_DictSetCStr(REPY_FN_LOCAL_SCOPE, var_name, REPY_MakeSUH(REPY_CreateStr(value)))

/**
 * @brief Sets a variable of the Python type `str` in the the local scope, using `char` array of `N` length for the value.
 * 
 * Note that the global scope will be unaffected.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @param value The content of the Python `str`. Should be a `char*`.
 * @param len The length of `value` in bytes.
 */
#define REPY_FN_SET_STR_N(var_name, value, len) \
REPY_DictSetCStr(REPY_FN_LOCAL_SCOPE, var_name, REPY_MakeSUH(REPY_CreateStrN(value, len)))

/**
 * @brief Gets a variable from the local scope and casts it to a `char*`. Intended to be used when the variable object is a Python `bytes`. 
 * 
 * Note that this macro will not retrieve global variables.
 * 
 * The C string returned by this macro will need to be freed with `recomp_free`. Failure to do so will result in a memory leak.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @return The value of the variable as a `char*`.
 */
#define REPY_FN_GET_BYTESTR(var_name) \
REPY_CastByteStr(REPY_MakeSUH(REPY_DictGetCStr(REPY_FN_LOCAL_SCOPE, var_name)))

/**
 * @brief Sets a variable of the Python type `str` in the the local scope, using a  NULL-terminated C string.
 * 
 * Note that the global scope will be unaffected.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @param value The value of the Python `str`. Should be a NULL-terminated C string.
 */
#define REPY_FN_SET_BYTESTR(var_name, value) \
REPY_DictSetCStr(REPY_FN_LOCAL_SCOPE, var_name, REPY_MakeSUH(REPY_CreateByteStr(value)))

/**
 * @brief Sets a variable of the Python type `bytes` in the the local scope, using `char` array of `N` length for the value.
 * 
 * Note that the global scope will be unaffected.
 * 
 * @param var_name The name of the variable. Should be a NULL-terminated C string.
 * @param value The content of the Python `bytes`. Should be a `char*`.
 * @param len The length of `value` in bytes.
 */
#define REPY_FN_SET_BYTESTR_N(var_name, value, len) \
REPY_DictSetCStr(REPY_FN_LOCAL_SCOPE, var_name, REPY_MakeSUH(REPY_CreateByteStrN(value, len)))

/** @}*/

/** \defgroup repy_fn_if_cache REPY_FN_IF_CACHE - Conditional Statements Based on Python Scopes
 * 
 * Because code string parsing and execution requires contiguous, compilable strings, there is 
 * no good way to call back into mod code from within a `if/else` block within Python code.
 * As an alternative, this section provides an easy and performant way to use the Python scope
 * to govern mod code `if/else` blocks utilizing `REPY_IfStmtHelper`.
 * 
 * There are no `REPY_FN_IF` or `REPY_FN_IF_CSTR` variations of these macros, as simple 
 * `REPY_FN_EVAL_BOOL` and `REPY_FN_EVAL_CSTR_BOOL` calls could be used as the conditions of
 * if statements directly. Nevertheless, the term `CACHE` is included in these macro names to
 * indicate that purpose and behavior; As per the design of `REPY_IfStmtHelper`, the code strings
 * given to these macros are only parsed once, and the bytecode stored for repeated evaluations.
 * 
 * @{
 */

/**
 * @brief Initializes the helpers for cached Pythonic `if` statements in the current scope.
 * 
 * This macro is used as part of `REPY_FN_IF_CACHE`, but you can invoke it manually if needed
 * (for example, if you're mixing Python and C `if` statements in one if/else block).
 * 
 * See the `REPY_IfStmtHelper` section for more information.
 * 
 * @param helper_identifier The variable name of the `REPY_IfStmtHelper` object, as well as the prefix for the
 * `static REPY_IfStmtChain*` object. 
 */
#define REPY_FN_IF_CACHE_INIT(helper_identifier) \
static REPY_IfStmtChain* helper_identifier ## _chain_root = NULL; \
REPY_IfStmtHelper* helper_identifier = REPY_DeferredCleanupHelper_AddIfStmtHelper(REPY_FN_AUTO_CLEANUP, REPY_IfStmtHelper_Create(&helper_identifier ## _chain_root)); \

/**
 * @brief Constructs a `if` statement that uses a cached Python expression executed in the current scope.
 * 
 * This macro is used as part of `REPY_FN_IF_CACHE` and `REPY_FN_ELIF_CACHE`, but you can invoke it manually if needed
 * (for example, if you're mixing Python and C `if` statements in one if/else block).
 * 
 * See the `REPY_IfStmtHelper` section for more information.
 * 
 * @param helper_identifier The variable name of the `REPY_IfStmtHelper` object. Needs to be the same as the value used 
 * when initializing this if/else block (using either `REPY_FN_IF_CACHE_INIT` or `REPY_FN_IF_CACHE`).
 * @param py_expression The Python expression to evaluate within the scope. Should be a NULL-terminated C string.
 */
#define REPY_FN_IF_CACHE_STMT(helper_identifier, py_expression) \
if ( \
    REPY_IfStmtHelper_Step( \
        helper_identifier, \
        REPY_FN_GLOBAL_SCOPE, \
        REPY_FN_LOCAL_SCOPE, \
        py_expression, \
        __FILE_NAME__, \
        (char*) __func__, \
        __LINE__, \
        #helper_identifier \
    ) \
) 

/**
 * @brief Initializes the helpers for a cached Pythonic `if/else` block, and constructs the first `if` statement,
 * using a cached Python expression executed in the local scope.
 * 
 * This is the simplest way to handle Pythonic if/else blocks in C code, provided there are no additional C expressions
 * that need to be evaluated as well.
 * 
 * See the `REPY_IfStmtHelper` section for more information.
 * 
 * @param helper_identifier The variable name of the `REPY_IfStmtHelper` object, as well as the prefix for the
 * `static REPY_IfStmtChain*` object. 
 * @param py_expression The Python expression to evaluate within the scope. Should be a NULL-terminated C string.
 */
#define REPY_FN_IF_CACHE(helper_identifier, py_expression) \
REPY_FN_IF_CACHE_INIT(helper_identifier) \
REPY_FN_IF_CACHE_STMT(helper_identifier, py_expression) 

/**
 * @brief Constructs a `else if` statement that uses a cached Python expression executed in the current scope.
 * 
 * Requires using the helpers initialized by `REPY_FN_IF_CACHE_INIT` or `REPY_FN_IF_CACHE`.
 * 
 * See the `REPY_IfStmtHelper` section for more information.
 * 
 * @param helper_identifier The variable name of the `REPY_IfStmtHelper` object. Needs to be the same as the value used 
 * when initializing this if/else block (using either `REPY_FN_IF_CACHE_INIT` or `REPY_FN_IF_CACHE`).
 * @param py_expression The Python expression to evaluate within the scope. Should be a NULL-terminated C string.
 */
#define REPY_FN_ELIF_CACHE(helper_identifier, py_expression) \
else REPY_FN_IF_CACHE_STMT(helper_identifier, py_expression)

/** @}*/

/** \defgroup repy_fn_loops REPY_FN Loops - C Loops Controlled By Python Scope
 * 
 * Because code string parsing and execution requires contiguous, compilable strings, there is 
 * no good way to call back into mod code from within a `for` or `while` loop in Python code.
 * As an alternative, this section provides an easy and performant way to use the Python scope
 * to govern mod code `for` and `while` loops, and even allows mod code to iterate through a Python
 * object using `REPY_IteratorHelper`.
 * 
 * Provides an easy and performant way to use the Python scope for mod code loops.
 * 
 * There are no non-`CACHE` variations of these macros, as simple, `REPY_FN_EVAL_BOOL` and 
 * `REPY_FN_EVAL_CSTR_BOOL` can be used as the conditions of `for` and `while` loops directly, 
 * and `REPY_FN_EXEC`, `REPY_FN_EXEC_CSTR` can be used for the initialized and iterator of a `for` loop.
 * Nevertheless, the term `CACHE` is included in these macro names to indicate that purpose and behavior;
 * the code strings given to these macros are only parsed once, and the bytecode stored for repeated evaluations.
 * 
 * @{
 */

/**
 * @brief Constructs a `while` loop that uses a cached Python expression executed in the current scope.
 * 
 * The Python expression is cast to `bool` after evaluation.
 * 
 * @param bytecode_identifier The name for a static variable that will hold the Python bytecode handle once created.
 * @param py_expression The Python code to evaluate as the loop condition. Should be a NULL-terminated C-string such
 * as a string literal. This code string will only be parsed and compiled once.
 */
#define REPY_FN_WHILE_CACHE(bytecode_identifier, py_expression) \
REPY_INLINE_COMPILE_CACHE_BLOCK("REPY_FN_WHILE_CACHE", bytecode_identifier, REPY_CODE_EVAL, py_expression); \
while (REPY_FN_EVAL_BOOL(bytecode_identifier))

/**
 * @brief Constructs a `for` loop that iterates through a Python object in the current scope, similar to how a Pythonic `for` loop works.
 * 
 * A Python expression is evaluated in the object to iterate through. This can be a literal, a variable, function call, or any other
 * valid Python expression, so long as the result is iterable. Each iteration, the current object will be added to the local scope
 * using the variable name of `var_name`.
 * 
 * Much like `REPY_FOREACH`, A `REPY_IteratorHelper` object is created to manage the iteration process. The variable name for this helper
 * in the format of `bytecode_handle_iter`. In addition to being added to the scope, the current object of the loop can be accessed via 
 * `REPY_IteratorHelper_BorrowCurrent`, and the index of that object can be accessed via  * `REPY_IteratorHelper_GetIndex`. 
 * See the `REPY_IteratorHelper` documentation for more information.
 * 
 * Unlike `REPY_FOREACH`, this macro uses the `REPY_FN` scope's `REPY_DeferredCleanupHelper` to handle cleanup. So you don't need to
 * worry about handling that yourself.
 * 
 * @param bytecode_identifier The name for a static variable that will hold the Python bytecode handle once created.
 * @param var_name Each iteration of the loop, the current loop object will be added to the local scope under this variable name.
 * Should be a NULL-terminated C-string such as a string literal.
 * @param py_expression A Pythonic expression that evaluates to the iterable object. Should be a NULL-terminated C-string
 * such as a string literal. This code string will only be parsed and compiled once.
 */
#define REPY_FN_FOREACH_CACHE(bytecode_identifier, var_name, py_expression) \
REPY_INLINE_COMPILE_CACHE_BLOCK("REPY_FN_FOREACH_CACHE", bytecode_identifier, REPY_CODE_EVAL, py_expression); \
for ( \
    REPY_IteratorHelper* bytecode_identifier ## _iter = REPY_DeferredCleanupHelper_AddIteratorHelper( \
        REPY_FN_AUTO_CLEANUP, \
        REPY_IteratorHelper_Create(REPY_MakeSUH(REPY_FN_EVAL(bytecode_identifier)), REPY_FN_LOCAL_SCOPE, var_name, false) \
    ); \
    REPY_IteratorHelper_Update(bytecode_identifier ## _iter);\
) \


/**
 * @brief Constructs a C style for loop, using cached Python code.
 * 
 * While the various `FOREACH` macros iterate through a Python object in the way that a `for` loop does in Python,
 * this macro iterates in the way that a `for` loop in C does, with an initialization statement, a continuation condition expression,
 * and an update statement. The key is that, with this macro, each of these statements/expressions are Python code, evaluated in the
 * current scope.
 * 
 * @param bytecode_identifier Used to construct the names for 3 static Python bytecode variables, needed by the three statements/expressions.
 * @param py_init_statement The Python code to use as the initialization statement. Should be a NULL-terminated C-string such as a string literal.
 * @param py_init_statement The Python code to use as the continuation condition expression. Should be a NULL-terminated C-string such as a string literal.
 * @param py_init_statement The Python code to use as the update statement. Should be a NULL-terminated C-string such as a string literal.
 */
#define REPY_FN_FOR_CACHE(bytecode_identifier, py_init_statement, py_eval_expression, py_update_statement) \
REPY_INLINE_COMPILE_CACHE_BLOCK("REPY_FN_FOR_CACHE", bytecode_identifier ## _init_statement, REPY_CODE_EXEC, py_init_statement); \
REPY_INLINE_COMPILE_CACHE_BLOCK("REPY_FN_FOR_CACHE", bytecode_identifier ## _eval_expression, REPY_CODE_EVAL, py_eval_expression); \
REPY_INLINE_COMPILE_CACHE_BLOCK("REPY_FN_FOR_CACHE", bytecode_identifier ## _update_statement, REPY_CODE_EXEC, py_update_statement); \
for ( \
    REPY_FN_EXEC(bytecode_identifier ## _init_statement); \
    REPY_FN_EVAL_BOOL(bytecode_identifier ## _eval_expression); \
    REPY_FN_EXEC(bytecode_identifier ## _update_statement) \
) 

/** @}*/
/** @}*/
/** @}*/

/** \defgroup repy_funcs API Functions
 * \brief The overview of all REPY API functions
 *  @{
 */

/** \defgroup repy_preinit_funcs PreInit Functions
 * \brief Functions that affect Python Initialization.
 *  @{
 */

 /**
  * @brief Instruct REPY to add a path to Python's module search path during initialization.
  * 
  * If you pass the path of an NRM (such as the path to the NRM for this mod obtained via `recomp_get_mod_file_path`),
  * REPY treat the NRM as a .zip file and will be able to search it for Python modules. 
  * 
  * Python module files and folders can be added to the NRM by including them under the `additional_files` section of the `mod.toml`
  * 
  * @param nrm_file_path 
  */
REPY_IMPORT(void REPY_PreInitAddSysPath(const unsigned char* nrm_file_path));

/**
 * @brief Registers a new Python subinterpreter to be created on initialization, and return a `REPY_InterpreterIndex` corresponding to it.
 * 
 * Note that initializing a subinterpreter is expensive performance-wise, and can add a slight delay on startup.
 * 
 * @return the index that will correspond to the new interpreter.
 */
REPY_IMPORT(REPY_InterpreterIndex REPY_PreInitRegisterSubinterpreter());

/** @}*/

/** \defgroup repy_handle_funcs Handle Functions
 * \brief Functions that operate on `REPY_Handle` values directly, rather than the Python objects they represent.
 *  @{
 */

/**
 * @brief Releases a `REPY_Handle`, removing the reference to the Python object and rendering this handle invalid.
 * 
 * It is safe to use this function with a Single-Use handle, since it will not try to release the handle twice.
 * 
 * @param py_handle The handle to release.
 */
REPY_IMPORT(void REPY_Release(REPY_Handle py_handle));

/**
 * @brief Convienience function that marks a `REPY_Handle` as Single-Use and then returns the value of the provided handle.
 * 
 * This function primarily exists to allow you to nest calls to API functions without causing memory/resource leaks.
 * Consider the following code:
 * 
 * ```C
 * REPY_CastBool(REPY_CreateBool(true));
 * ```
 * 
 * This is will result in the `REPY_Handle` created by `REPY_CreateBool` not getting released. and since the handle was never 
 * captured in a variable, We can't make a call to release it. In effect, this is a memory leak.
 * 
 *  ```C
 * REPY_CastBool(REPY_MakeSUH(REPY_CreateBool(true)));
 * ```
 * 
 * By marking the `REPY_Handle` from `REPY_CreateBool` as Single-Use, it will be released as soon as REPY_CastBool is done with it.
 * Thus, no memory leak.
 * 
 * @param py_handle_no_release The handle to make Single-Use.
 * @return The same handle as was passed in via `py_handle_no_release`..
 */
REPY_IMPORT(REPY_Handle REPY_MakeSUH(REPY_Handle py_handle_no_release));

/**
 * @brief Returns whether the a handle value is mapped to a Python object.
 * 
 * A handle of 0 (`REPY_NO_OBJECT`) will always return false.
 * 
 * This function is special in that it will not release a Single-Use `REPY_Handle` handle.
 * 
 * @param py_handle_no_release The handle in question.
 * @return True if the handle is valid. False otherwise.
 */
REPY_IMPORT(REPY_bool REPY_IsValidHandle(REPY_Handle py_handle_no_release));

/**
 * @brief Gets whether or not a `REPY_Handle` is Single-Use.
 * 
 * This function is special in that it will not release a Single-Use `REPY_Handle` handle.
 * 
 * @param py_handle_no_release The handle in question.
 * @return True if the handle is Single-Use. False otherwise.
 */
REPY_IMPORT(REPY_bool REPY_GetSUH(REPY_Handle py_handle_no_release));

/**
 * @brief Sets whether or not a `REPY_Handle` is Single-Use.
 * 
 * This function is special in that it will not release a Single-Use `REPY_Handle` handle.
 * 
 * @param py_handle_no_release The handle in question.
 * @param value True will make the handle Single-Use. False will make it not Single-Use.
 */
REPY_IMPORT(void REPY_SetSUH(REPY_Handle py_handle_no_release, REPY_bool value));

/**
 * @brief Creates a new handle to the same Python object as another handle.
 * 
 * Note that, unlike the other functions in this category, this function WILL release Single-Use handles.
 * In the future, a `_no_release` version of this function may be added.
 * 
 * @param handle_no_release A handle for an object you need another handle to.
 * @return A new handle to the same object.
 */
REPY_IMPORT(REPY_Handle REPY_CopyHandle(REPY_Handle handle_no_release));

/** @}*/

/** \defgroup repy_interpreter_funcs (Sub)Interpreter Functions
 * \brief Functions Used for REPY's Interpreter Stack, which handles interpreter/subinterpreter operations as well as `PyThreadState` management.
 * 
 * REPY used a free-threaded build of Python, with the traditional Python GIL is a non-factor. However, the CPython still needs to know whether
 * or not a given thread needs access to the interpreter (and when subinterpreters are in use, which interpreter) using an opaque struct in CPython
 * called `PyThreadState`.
 *
 * Because the mod developer has no control over what other mods a player may install, and whether these mods use REPY, no single mod can be given 
 * responsibility for managing the `PyThreadState` directly. Therefore, REPY implements something called the Interpreter Stack: If a given mod code
 * thread requires access to the main interpreter, a call to `REPY_PushInterpreter` tells REPY a mod needs access to a specific interpreter, and a 
 * call to `REPY_PopInterpreter` tells REPY that the previously requested access is no longer needed. Using this, REPY itself manages the `PyThreadState`
 * of a given N64Recompiled thread, making changes to the state only necessary.
 * 
 * For this reason, it is recommended to call `REPY_PushInterpreter` at the top of any function that needs Python interpreter access, and to call
 * `REPY_PopInterpreter` before that function returns. The exception is when `REPY_FN` macros, as the setup and cleanup macros will automatically do 
 * this for you.
 * 
 waitr* See \ref repy_performance_interpreter_stack for potential performance pitfalls related to Interpreter Stack management.
 * 
 * See \ref subinterpreters for more information on what they are, and REPY's usage of them.
 * 
 * @{
 */

/**
 * @brief Pushes an interpreter index to the interpreter stack, and switching the active interpreter if necessary.
 * 
 * See \ref subinterpreters for more information.
 * 
 * @param interpreter_handle The index to push to the subinterpreter stack.
 */
REPY_IMPORT(void REPY_PushInterpreter(REPY_InterpreterIndex interpreter_handle));

/**
 * @brief Pops an interpreter index from the interpreter stack, switching active interpreters if necessary.
 * 
 * See \ref subinterpreters for more information.
 */
REPY_IMPORT(void REPY_PopInterpreter());

/**
 * @brief Gets the interpreter index at the top of the interpreter stack.
 * 
 * Returns `REPY_INTERPRETER_STACK_EMPTY` when the interpreter stack is empty.
 * See \ref subinterpreters for more information.
 * 
 * @return the interpreter index at the top of the stack.
 */
REPY_IMPORT(REPY_InterpreterIndex REPY_GetCurrentInterpreter());

/**
 * @brief Gets whether or not this subinterpreter should be 'disarmed' when shutting down.
 * 
 * This is a workaround for N64Recompiled titles freezing on exit when a subinterpreter has additional
 * threads running. When auto-disarm is set to true, REPY will simply let the subinterpreter leak
 * and be cleaned up by the OS. It's hardly a clean exit, but it's the best solution we have until
 * a proper `on_shutdown` event is implemented in the N64Recompiled runtime.
 * 
 * @param index The subinterpreter to set this value for. Does nothing for the main interpreter.
 */
REPY_IMPORT(REPY_bool REPY_GetInterpreterAutoDisarm(REPY_InterpreterIndex index));

/**
 * @brief Sets whether or not this subinterpreter should be 'disarmed' when shutting down.
 * 
 * This is a workaround for N64Recompiled titles freezing on exit when a subinterpreter has additional
 * threads running. When auto-disarm is set to true, REPY will simply let the subinterpreter leak
 * and be cleaned up by the OS. It's hardly a clean exit, but it's the best solution we have until
 * a proper `on_shutdown` event is implemented in the N64Recompiled runtime.
 * 
 * @param index The subinterpreter to set this value for. Does nothing for the main interpreter.
 * @param value The new value for auto-disarm.
 */
REPY_IMPORT(void REPY_SetInterpreterAutoDisarm(REPY_InterpreterIndex index, REPY_bool value));

/**
 * @brief Gets the index of the interpreter a specific `REPY_Handle` object is associated with.
 * 
 * This function will not release Single-Use handles.
 * 
 * @param handle_no_release a `REPY_Handle` for get the interpreter for. 
 * @return the interpreter index corresponding to this `REPY_Handle`.
 */
REPY_IMPORT(REPY_InterpreterIndex REPY_GetHandleInterpreter(REPY_Handle handle_no_release));

/** @}*/

/** \defgroup repy_module_funcs Module Functions
 * \brief Functions Used for Python module operations.
 *  @{
 */

 /**
  * @brief Casts a null-terminated C-string to a Python `str` and appends it to the current interpreter's `sys.path`
  * 
  * Unlike `REPY_PreInitAddToSysPath`, this will only add a path to the current interpreter. The `sys.path` values of any
  * other interpreters defined before this point will be unaffected.
  * 
  * This function primarily exists to support for `REPY_AddCStrToSysPath`, since that needs to be `inline` in order to work.
  * 
  * @param filepath a null-terminated path string to be added to `sys.path`
  */
REPY_IMPORT(void REPY_AddCStrToSysPath(const char* filepath));

/**
 * @brief Adds this NRM to the current interpreter's `sys.path`. 
 * 
 * Unlike `REPY_PreInitAddToSysPath`, this will only add a path to the current interpreter. The `sys.path` values of any
* other interpreters defined before this point will be unaffected.
* 
* This function is defined as `inline` in the `repy_api.h` header in order to be able to grab this mod's nrm path, and
* is primarily a wrapper for `REPY_AddCStrToSysPath`.
 */
inline void REPY_AddNrmToSysPath() {
    const char* filepath = (const char*) recomp_get_mod_file_path();
    REPY_AddCStrToSysPath(filepath);
    recomp_free((void*) filepath);
}

/**
 * @brief Construct a new Python module from a NULL-terminated code string, optionally importable by name.
 * 
 * The Python code of the module is run immediately, rather than on first import.
 * 
 * @param identifier The name of the new module. Should be NULL-terminated.
 * @param code The Python code for the module. Should be NULL-terminated.
 * @param add_to_sys If true, the module is added to Python's `sys.modules`, allowing it to be brought into scope via an `import` statement. 
 * If false, the module will only be available through the returned `REPY_Handle`.
 * @return a new `REPY_Handle` for the module.
 */
REPY_IMPORT(REPY_Handle REPY_ConstructModuleFromCStr(const char* identifier, const char* code, REPY_bool add_to_sys));

/**
 * @brief Construct a new Python module from a `char` array, optionally importable by name.
 * 
 * The Python code of the module is run immediately, rather than on first import.
 * 
 * @param identifier The name of the new module. Should be NULL-terminated.
 * @param code The Python code for the module. Does not need to be null terminated.
 * @param len The length of the Python code, in bytes.
 * @param add_to_sys If true, the module is added to Python's `sys.modules`, allowing it to be brought into scope via an `import` statement. 
 * If false, the module will only be available through the returned `REPY_Handle`.
 * @return a new `REPY_Handle` for the module.
 */
REPY_IMPORT(REPY_Handle REPY_ConstructModuleFromCStrN(const char* identifier, const char* code, REPY_u32 len, REPY_bool add_to_sys));

/**
 * @brief Imports a Python module by name and returns a handle to it.
 * 
 * Members of the module can be accessed with the various attribute access functions.
 * 
 * @param identifier The name of the Python module. Should be NULL-terminated.
 */
REPY_IMPORT(REPY_Handle REPY_ImportModule(const char* identifier));

/** @}*/

/** \defgroup repy_primative_funcs Primative Operations
 * \brief Functions that deal with converting between C and Python primatives.
 *  @{
 */

/**
 * @brief Returns a `REPY_Handle` for a Python `bool` object, based on a C `bool`. 
 * 
 * The handle returned by this function will need to be released, either by making is Single-Use or by calling `REPY_Release`.
 * Even though both Python's `True` and `False` objects are immortal objects, will still take up space in the handle management system.
 * Ergo, failure to release this handle will result in a memory leak. 
 * 
 * @param value The value for the Python `bool`.
 * @return A new handle for your Python `bool`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateBool(REPY_bool value));

/**
 * @brief Returns a Single-Use `REPY_Handle` for a Python `bool` object, based on a C `bool`. 
 * 
 * At this time, this function is just shorthand for `REPY_MakeSUH(REPY_CreateBool(value))`, and thus will perform similarly.
 * However, internal performance improvements may make this function more performant in the future.
 * 
 * @param value The value for the Python `bool`.
 * @return A new Single-Use handle for your Python `bool`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateBool_SUH(REPY_bool value));

/** @brief Casts a Python object to a C `bool`. 
 * 
 * Intended to be used with a Python `bool`. Behavior with other Python types may change between versions.
 * 
 * @param object The handle for the Python object in question.
 * @return A `bool` value corresponding to the Python `object`. If a Python error occurs, this value is undefined.
 */
REPY_IMPORT(REPY_bool REPY_CastBool(REPY_Handle object));

/**
 * @brief Returns a `REPY_Handle` for a Python `int` object, based on a C `u8`. 
 * 
 * The handle returned by this function will need to be released, either by making is Single-Use or by calling `REPY_Release`.
 * Failure to release this handle will result in a memory leak. 
 * 
 * @param value The value for the Python `int`.
 * @return A new handle for your Python `int`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateU8(REPY_u8 value));

/**
 * @brief Returns a Single-Use `REPY_Handle` for a Python `int` object, based on a C `u8`. 
 * 
 * At this time, this function is just shorthand for `REPY_MakeSUH(REPY_CreateU8(value))`, and thus will perform similarly.
 * However, internal performance improvements may make this function more performant in the future.
 * 
 * @param value The value for the Python `int`.
 * @return A new Single-Use handle for your Python `int`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateU8_SUH(REPY_u8 value));

/** @brief Casts a Python object to a C `u8`. 
 * 
 * Intended to be used with a Python `int`. Behavior with other Python types may change between versions.
 * 
 * @param object The handle for the Python object in question.
 * @return A `u8` value corresponding to the Python `object`. If a Python error occurs, this value is undefined.
 */
REPY_IMPORT(REPY_u8 REPY_CastU8(REPY_Handle object));

/**
 * @brief Returns a `REPY_Handle` for a Python `int` object, based on a C `s8`. 
 * 
 * The handle returned by this function will need to be released, either by making is Single-Use or by calling `REPY_Release`.
 * Failure to release this handle will result in a memory leak. 
 * 
 * @param value The value for the Python `int`.
 * @return A new handle for your Python `int`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateS8(REPY_s8 value));

/**
 * @brief Returns a Single-Use `REPY_Handle` for a Python `int` object, based on a C `s8`. 
 * 
 * At this time, this function is just shorthand for `REPY_MakeSUH(REPY_CreateS8(value))`, and thus will perform similarly.
 * However, internal performance improvements may make this function more performant in the future.
 * 
 * @param value The value for the Python `int`.
 * @return A new Single-Use handle for your Python `int`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateS8_SUH(REPY_s8 value));

/** @brief Casts a Python object to a C `s8`. 
 * 
 * Intended to be used with a Python `int`. Behavior with other Python types may change between versions.
 * 
 * @param object The handle for the Python object in question.
 * @return An `s8` value corresponding to the Python `object`. If a Python error occurs, this value is undefined.
 */
REPY_IMPORT(REPY_s8 REPY_CastS8(REPY_Handle object));

/**
 * @brief Returns a `REPY_Handle` for a Python `int` object, based on a C `u16`. 
 * 
 * The handle returned by this function will need to be released, either by making is Single-Use or by calling `REPY_Release`.
 * Failure to release this handle will result in a memory leak. 
 * 
 * @param value The value for the Python `int`.
 * @return A new handle for your Python `int`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateU16(REPY_u16 value));

/**
 * @brief Returns a Single-Use `REPY_Handle` for a Python `u16` object, based on a C `int`. 
 * 
 * At this time, this function is just shorthand for `REPY_MakeSUH(REPY_CreateU16(value))`, and thus will perform similarly.
 * However, internal performance improvements may make this function more performant in the future.
 * 
 * @param value The value for the Python `int`.
 * @return A new Single-Use handle for your Python `int`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateU16_SUH(REPY_u16 value));

/** @brief Casts a Python object to a C `u16`. 
 * 
 * Intended to be used with a Python `int`. Behavior with other Python types may change between versions.
 * 
 * @param object The handle for the Python object in question.
 * @return A `u16` value corresponding to the Python `object`. If a Python error occurs, this value is undefined.
 */
REPY_IMPORT(REPY_u16 REPY_CastU16(REPY_Handle object));

/**
 * @brief Returns a `REPY_Handle` for a Python `int` object, based on a C `s16`. 
 * 
 * The handle returned by this function will need to be released, either by making is Single-Use or by calling `REPY_Release`.
 * Failure to release this handle will result in a memory leak. 
 * 
 * @param value The value for the Python `int`.
 * @return A new handle for your Python `int`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateS16(REPY_s16 value));

/**
 * @brief Returns a Single-Use `REPY_Handle` for a Python `int` object, based on a C `s16`. 
 * 
 * At this time, this function is just shorthand for `REPY_MakeSUH(REPY_CreateS16(value))`, and thus will perform similarly.
 * However, internal performance improvements may make this function more performant in the future.
 * 
 * @param value The value for the Python `int`.
 * @return A new Single-Use handle for your Python `int`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateS16_SUH(REPY_s16 value));

/** @brief Casts a Python object to a C `s16`. 
 * 
 * Intended to be used with a Python `int`. Behavior with other Python types may change between versions.
 * 
 * @param object The handle for the Python object in question.
 * @return An `s16` value corresponding to the Python `object`. If a Python error occurs, this value is undefined.
 */
REPY_IMPORT(REPY_s16 REPY_CastS16(REPY_Handle object));

/**
 * @brief Returns a `REPY_Handle` for a Python `int` object, based on a C `u32`. 
 * 
 * The handle returned by this function will need to be released, either by making is Single-Use or by calling `REPY_Release`.
 * Failure to release this handle will result in a memory leak. 
 * 
 * @param value The value for the Python `int`.
 * @return A new handle for your Python `int`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateU32(REPY_u32 value));

/**
 * @brief Returns a Single-Use `REPY_Handle` for a Python `int` object, based on a C `u32`. 
 * 
 * At this time, this function is just shorthand for `REPY_MakeSUH(REPY_CreateU32(value))`, and thus will perform similarly.
 * However, internal performance improvements may make this function more performant in the future.
 * 
 * @param value The value for the Python `int`.
 * @return A new Single-Use handle for your Python `int`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateU32_SUH(REPY_u32 value));

/** @brief Casts a Python object to a C `u32`. 
 * 
 * Intended to be used with a Python `int`. Behavior with other Python types may change between versions.
 * 
 * @param object The handle for the Python object in question.
 * @return A `u32` value corresponding to the Python `object`. If a Python error occurs, this value is undefined.
 */
REPY_IMPORT(REPY_u32 REPY_CastU32(REPY_Handle object));

/**
 * @brief Returns a `REPY_Handle` for a Python `int` object, based on a C `s32`. 
 * 
 * The handle returned by this function will need to be released, either by making is Single-Use or by calling `REPY_Release`.
 * Failure to release this handle will result in a memory leak. 
 * 
 * @param value The value for the Python `int`.
 * @return A new handle for your Python `int`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateS32(REPY_s32 value));

/**
 * @brief Returns a Single-Use `REPY_Handle` for a Python `int` object, based on a C `s32`. 
 * 
 * At this time, this function is just shorthand for `REPY_MakeSUH(REPY_CreateS32(value))`, and thus will perform similarly.
 * However, internal performance improvements may make this function more performant in the future.
 * 
 * @param value The value for the Python `int`.
 * @return A new Single-Use handle for your Python `int`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateS32_SUH(REPY_s32 value));

/** @brief Casts a Python object to a C `s32`. 
 * 
 * Intended to be used with a Python `int`. Behavior with other Python types may change between versions.
 * 
 * @param object The handle for the Python object in question.
 * @return An `s32*` value corresponding to the Python `object`. If a Python error occurs, this value is undefined.
 */
REPY_IMPORT(REPY_s32 REPY_CastS32(REPY_Handle object));

/**
 * @brief Returns a `REPY_Handle` for a Python `float` object, based on a C `f32`. 
 * 
 * The handle returned by this function will need to be released, either by making is Single-Use or by calling `REPY_Release`.
 * Failure to release this handle will result in a memory leak. 
 * 
 * @param value The value for the Python `float`.
 * @return A new handle for your Python `float`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateF32(REPY_f32 value));

/**
 * @brief Returns a Single-Use `REPY_Handle` for a Python `float` object, based on a C `f32`. 
 * 
 * At this time, this function is just shorthand for `REPY_MakeSUH(REPY_CreateBool(value))`, and thus will perform similarly.
 * However, internal performance improvements may make this function more performant in the future.
 * 
 * @param value The value for the Python `float`.
 * @return A new Single-Use handle for your Python `float`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateF32_SUH(REPY_f32 value));

/** @brief Casts a Python object to a C `f32`. 
 * 
 * Intended to be used with a Python `float`. Behavior with other Python types may change between versions.
 * 
 * @param object The handle for the Python object in question.
 * @return An `f32` value corresponding to the Python `object`. If a Python error occurs, this value is undefined.
 */
REPY_IMPORT(REPY_f32 REPY_CastF32(REPY_Handle object));

/**
 * @brief Returns a `REPY_Handle` for a Python `int` object, based on a C `void*`. 
 * 
 * The handle returned by this function will need to be released, either by making is Single-Use or by calling `REPY_Release`.
 * Failure to release this handle will result in a memory leak. 
 * 
 * @param value The value for the Python `int`.
 * @return A new handle for your Python `int`.
 */
REPY_IMPORT(REPY_Handle REPY_CreatePtr(void* value));

/**
 * @brief Returns a Single-Use `REPY_Handle` for a Python `int` object, based on a C `void*`. 
 * 
 * At this time, this function is just shorthand for `REPY_MakeSUH(REPY_CreatePtr(value))`, and thus will perform similarly.
 * However, internal performance improvements may make this function more performant in the future.
 * 
 * @param value The value for the Python `int`.
 * @return A new Single-Use handle for your Python `int`.
 */
REPY_IMPORT(REPY_Handle REPY_CreatePtr_SUH(void* value));

/** @brief Casts a Python object to a C `void*`. 
 * 
 * Intended to be used with a Python `int`. Behavior with other Python types may change between versions.
 * 
 * @param object The handle for the Python object in question.
 * @return A `void*` value corresponding to the Python `object`. If a Python error occurs, this value is undefined.
 */
REPY_IMPORT(void* REPY_CastPtr(REPY_Handle object));

/**
 * @brief Returns a `REPY_Handle` for a Python `int` object, based on a C `u64`. 
 * 
 * The handle returned by this function will need to be released, either by making is Single-Use or by calling `REPY_Release`.
 * Failure to release this handle will result in a memory leak. 
 * 
 * @param value The value for the Python `int`.
 * @return A new handle for your Python `int`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateU64(REPY_u64 value));

/**
 * @brief Returns a Single-Use `REPY_Handle` for a Python `int` object, based on a C `u64`. 
 * 
 * At this time, this function is just shorthand for `REPY_MakeSUH(REPY_CreateU64(value))`, and thus will perform similarly.
 * However, internal performance improvements may make this function more performant in the future.
 * 
 * @param value The value for the Python `int`.
 * @return A new Single-Use handle for your Python `int`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateU64_SUH(REPY_u64 value));

/** @brief Casts a Python object to a C `u64`. 
 * 
 * Intended to be used with a Python `int`. Behavior with other Python types may change between versions.
 * 
 * @param object The handle for the Python object in question.
 * @return A `u64` value corresponding to the Python `object`. If a Python error occurs, this value is undefined.
 */
REPY_IMPORT(REPY_u64 REPY_CastU64(REPY_Handle object));

/**
 * @brief Returns a `REPY_Handle` for a Python `int` object, based on a C `s64`. 
 * 
 * The handle returned by this function will need to be released, either by making is Single-Use or by calling `REPY_Release`.
 * Failure to release this handle will result in a memory leak. 
 * 
 * @param value The value for the Python `int`.
 * @return A new handle for your Python `int`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateS64(REPY_s64 value));

/**
 * @brief Returns a Single-Use `REPY_Handle` for a Python `int` object, based on a C `s64`. 
 * 
 * At this time, this function is just shorthand for `REPY_MakeSUH(REPY_CreateS64(value))`, and thus will perform similarly.
 * However, internal performance improvements may make this function more performant in the future.
 * 
 * @param value The value for the Python `int`.
 * @return A new Single-Use handle for your Python `int`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateS64_SUH(REPY_s64 value)); 

/** @brief Casts a Python object to a C `s64`. 
 * 
 * Intended to be used with a Python `int`. Behavior with other Python types may change between versions.
 * 
 * @param object The handle for the Python object in question.
 * @return An `s64` value corresponding to the Python `object`. If a Python error occurs, this value is undefined.
 */
REPY_IMPORT(REPY_s64 REPY_CastS64(REPY_Handle object));

/**
 * @brief Returns a `REPY_Handle` for a Python `int` object, based on a C `u8`. 
 * 
 * The handle returned by this function will need to be released, either by making is Single-Use or by calling `REPY_Release`.
 * Failure to release this handle will result in a memory leak. 
 * 
 * @param value The value for the Python `int`.
 * @return A new handle for your Python `int`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateF64(REPY_f64 value));

/**
 * @brief Returns a Single-Use `REPY_Handle` for a Python `int` object, based on a C `bool`. 
 * 
 * At this time, this function is just shorthand for `REPY_MakeSUH(REPY_CreateBool(value))`, and thus will perform similarly.
 * However, internal performance improvements may make this function more performant in the future.
 * 
 * @param value The value for the Python `int`.
 * @return A new Single-Use handle for your Python `int`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateF64_SUH(REPY_f64 value));

/** @brief Casts a Python object to a C `u8`. 
 * 
 * Intended to be used with a Python `float`. Behavior with other Python types may change between versions.
 * 
 * @param object The handle for the Python object in question. 
 * @return An `f64` value corresponding to the Python `object`. If a Python error occurs, this value is undefined.
 */
REPY_IMPORT(REPY_f64 REPY_CastF64(REPY_Handle object));

/**
 * @brief Returns a `REPY_Handle` for a Python `str` object, based on a NULL-terminated C string. 
 * 
 * The handle returned by this function will need to be released, either by making is Single-Use or by calling `REPY_Release`.
 * Failure to release this handle will result in a memory leak. 
 * 
 * @param string The value for the Python `str`.
 * @return A new handle for your Python `str`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateStr(const char* string));

/**
 * @brief Returns a Single-Use `REPY_Handle` for a Python `str` object, based on a NULL-terminated C string. 
 * 
 * At this time, this function is just shorthand for `REPY_MakeSUH(REPY_CreateStr(string))`, and thus will perform similarly.
 * However, internal performance improvements may make this function more performant in the future.
 * 
 * @param string The value for the Python `str`.
 * @return A new Single-Use handle for your Python `str`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateStr_SUH(const char* string));

/**
 * @brief Returns a `REPY_Handle` for a Python `str` object, based on `char` array.
 * 
 * The handle returned by this function will need to be released, either by making is Single-Use or by calling `REPY_Release`.
 * Failure to release this handle will result in a memory leak. 
 * 
 * @param string The beginning of the `char` array to create a Python `str` from.
 * @param len The length of the `string` array.
 * @return A new Single-Use handle for your Python `str`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateStrN(const char* string, REPY_u32 len) );

/**
 * @brief Returns a Single-Use `REPY_Handle` for a Python `str` object, based on `char` array. 
 * 
 * At this time, this function is just shorthand for `REPY_MakeSUH(REPY_CreateStrN_SUH(string, len))`, and thus will perform similarly.
 * However, internal performance improvements may make this function more performant in the future.
 * 
 * @param string The beginning of the `char` array to create a Python `str` from.
 * @param len The length of the `string` array.
 * @return A new Single-Use handle for your Python `str`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateStrN_SUH(const char* string, REPY_u32 len) );

/** @brief Casts a Python object to NULL-terminated C string. Intended to be used with a Python `str`.
 * 
 * Behavior with other Python types may change between versions.
 * 
 * @param object The handle for the Python object in question.
 */
REPY_IMPORT(char* REPY_CastStr(REPY_Handle object));

/**
 * @brief Returns a `REPY_Handle` for a Python `bytes` object, based on a NULL-terminated C string. 
 * 
 * The handle returned by this function will need to be released, either by making is Single-Use or by calling `REPY_Release`.
 * Failure to release this handle will result in a memory leak. 
 * 
 * @param string The value for the Python `bytes`.
 * @return A new handle for your Python `bytes`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateByteStr(const char* string));

/**
 * @brief Returns a Single-Use `REPY_Handle` for a Python `bytes` object, based on a NULL-terminated C string.
 * 
 * At this time, this function is just shorthand for `REPY_MakeSUH(REPY_CreateByteStr(string))`, and thus will perform similarly.
 * However, internal performance improvements may make this function more performant in the future.
 * 
 * @param string The value for the Python `bytes`.
 * @return A new Single-Use handle for your Python `bytes`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateByteStr_SUH(const char* string));

/**
 * @brief Returns a `REPY_Handle` for a Python `bytes` object, based on a `char` array.
 * 
 * This function will stop reading from `string` if it encounters a NULL-terminator. If you need to copy a region of memory
 * regardless of it's contents, use `REPY_MemcpyToBytes`.
 * 
 * The handle returned by this function will need to be released, either by making is Single-Use or by calling `REPY_Release`.
 * Failure to release this handle will result in a memory leak. 
 * 
 * @param string The beginning of the `char` array to create a Python `bytes` from.
 * @param len The maximum length of the `string` array.
 * @return A new Single-Use handle for your Python `bytes`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateByteStrN(const char* string, REPY_u32 len));

/**
 * @brief Returns a Single-Use `REPY_Handle` for a Python `bytes` object, based on a `char` array.
 * 
 * This function will stop reading from `string` if it encounters a NULL-terminator. If you need to copy a region of memory
 * regardless of it's contents, use `REPY_MemcpyToBytes`.
 * 
 * At this time, this function is just shorthand for `REPY_MakeSUH(REPY_CreateByteStrN(string))`, and thus will perform similarly.
 * However, internal performance improvements may make this function more performant in the future.
 * 
 * @param string The beginning of the `char` array to create a Python `bytes` from.
 * @param len The length of the `string` array.
 * @return A new Single-Use handle for your Python `bytes`.
 */
REPY_IMPORT(REPY_Handle REPY_CreateByteStrN_SUH(const char* string, REPY_u32 len));

/** @brief Casts a Python object to a NULL-terminated C string. Intended to be used with a Python `bytes`.
 * 
 * Behavior with other Python types may change between versions.
 * 
 * @param object The handle for the Python object in question.
 */
REPY_IMPORT(char* REPY_CastByteStr(REPY_Handle object));

/** @}*/

/** \defgroup repy_memcpy_funcs Memcpy Functions
 * \brief Functins used to directly transfer data between mod memory and the Python interpreter, using appropriate Python types.
 *  @{
 */

/**
 * @brief Create a Python `bytes` object from a chunk of mod memory.
 * 
 * This function copies the entire `src` region exactly as is, NULL-terminators and all.
 * 
 * @param src The beginning of the memory region to copy. 
 * @param len The size of the region to copy, in bytes. 
 * @param reverse Set to `false` to copy normally, or `true` to reverse the byte order of the memory being copied.
 * @return A `REPY_Handle` to the `bytes` obect created.
 */
REPY_IMPORT(REPY_Handle REPY_MemcpyToBytes(void* src, REPY_u32 len, REPY_bool reverse));

/**
 * @brief Create a Python `bytearray` object from a chunk of mod memory.
 * 
 * This function copies the entire `src` region exactly as is, NULL-terminators and all.
 * 
 * @param src The beginning of the memory region to copy. 
 * @param len The size of the region to copy, in bytes. 
 * @param reverse Set to `false` to copy normally, or `true` to reverse the byte order of the memory being copied.
 * @return A `REPY_Handle` to the `bytearray` obect created.
 */
REPY_IMPORT(REPY_Handle REPY_MemcpyToByteArray(void* src, REPY_u32 len, REPY_bool reverse));

/**
 * @brief Copy the content of a Python object supporting buffer protocol (such as `bytes` and `bytearray`) into mod memory.
 * 
 * The binary data from the buffer will be copied exactly as is, NULL bytes and all. No additional NULL bytes will be 
 * added to the end of the data.
 * 
 * If the Python buffer object is smaller than `len`, then the remainder of destination will be untouched. If the
 * `bytearray` object is larger than len, then copying will terminate when `len` is reached. Use the return value to get
 * how many bytes were actually copied.
 * 
 * @param dst The beginning of the memory region to write to. 
 * @param len The size of the destination region to copy, in bytes. 
 * @param reverse Set to `false` to copy normally, or `true` to reverse the byte order of the data being copied.
 * @param buffer The Python buffer object to copy from.
 * @return The number of bytes actually copied.
 */
REPY_IMPORT(REPY_u32 REPY_MemcpyFromBuffer(void* dst, REPY_u32 len, REPY_bool reverse, REPY_Handle buffer));

/**
 * @brief Copy the content of a Python object supporting buffer protocol into mod memory, automatically allocating 
 * the space for it with `recomp_alloc`.
 * 
 * The binary data from the buffer will be copied exactly as is, NULL bytes and all. No additional NULL bytes will
 * be added to the end of the data.
 * 
 * Failure to free the `void*` returned by this function with `recomp_free` will result in a memory leak.
 * 
 * Behavior with Python types other than `bytearray` is undefined, and may change between versions.
 * 
 * @param reverse Set to `false` to copy normally, or `true` to reverse the byte order of the data being copied.
 * @param buffer The Python buffer object to copy from.
 * @param write_size A pointer to a `u32`, where the number of bytes copied can be written to. Can be NULL.
 * @return A `void*` to the data copied into mod memory.
 */
REPY_IMPORT(void* REPY_AllocAndCopyBuffer(REPY_bool reverse, REPY_Handle buffer, REPY_u32* write_size));

/** @}*/

/** \defgroup repy_general_funcs Object Attribute Functions
 * \brief Functions related to accessing the members and propertied of objects.
 *  @{
 */

/**
 * @brief Gets the length of an appropriate Python object as a `u32`.
 * 
 * This function is analogous to Python's own `len` function, and should work in all the same circumstances.
 * 
 * @param object The Python object to get the length of.
 */
REPY_IMPORT(REPY_u32 REPY_Len(REPY_Handle object));

/**
 * @brief Returns a `REPY_Handle` for a named attribute of a Python object, using a `REPY_Handle` for the attribute name.
 * 
 * This function is analogous to Python's own `getattr` function, and should work in all the same circumstances.
 * 
 * @param object The parent Python object.
 * @param key The name of attribute you're trying to access. The Python object referenced should generally be a `str`, as is standard with `getattr`.
 * @param default_value_nullable A default Python object to return if the attribute isn't found. Use `REPY_NO_OBJECT` to ignore this argument.
 * Note that if this default object is returned, the return value will be a new handle to the same object.
 * @return A handle to the desired attribute. Not that `REPY_NO_OBJECT` being returned doesn't necessarily mean that the object doesn't exist, 
 * only that an error has occured.
 */
REPY_IMPORT(REPY_Handle REPY_GetAttr(REPY_Handle object, REPY_Handle key, REPY_Handle default_value_nullable));

/**
 * @brief Returns a `REPY_Handle` for a named attribute of a Python object, using a NULL-terminated C string for the name.
 * 
 * This function is analogous to Python's own `getattr` function, and should work in all the same circumstances.
 * 
 * @param object The parent Python object.
 * @param key The name of attribute you're trying to access. Should be a NULL-terminated C string.
 * @param default_value_nullable A default Python object to return if the attribute isn't found. Use `REPY_NO_OBJECT` to ignore this argument.
 * Note that if this default object is returned, the return value will be a new handle to the same object.
 * @return A handle to the desired attribute. Not that `REPY_NO_OBJECT` being returned doesn't necessarily mean that the object doesn't exist, 
 * only that an error has occured.
 */
REPY_IMPORT(REPY_Handle REPY_GetAttrCStr(REPY_Handle object, char* key, REPY_Handle default_value_nullable));

/**
 * @brief Assigned a named attribute of a Python object, using a `REPY_Handle` for the attribute name.
 * 
 * This function is analogous to Python's own `setattr` function, and should work in all the same circumstances.
 * 
 * @param object The parent Python object.
 * @param key The name of attribute you're trying to assign to. The Python object referenced should generally be a `str`, as is standard with `setattr`.
 * @param value The Python object to assign to the attribute.
 */
REPY_IMPORT(void REPY_SetAttr(REPY_Handle object, REPY_Handle key, REPY_Handle value));

/**
 * @brief Assigned a named attribute of a Python object, using a NULL-terminated C string for the name.
 * 
 * This function is analogous to Python's own `setattr` function, and should work in all the same circumstances.
 * 
 * @param object The parent Python object.
 * @param key The name of attribute you're trying to assign to. The Python object referenced should generally be a `str`, as is standard with `setattr`.
 * @param value The Python object to assign to the attribute.
 */
REPY_IMPORT(void REPY_SetAttrCStr(REPY_Handle object, char* key, REPY_Handle value));

/**
 * @brief Checks if a Python object has an attribute with a specific name, using a `REPY_Handle` for the attribute name.
 * 
 * This function is analogous to Python's own `hasattr` function, and should work in all the same circumstances.
 * 
 * @param object The parent Python object.
 * @param key The name of attribute to check. The Python object referenced should generally be a `str`, as is standard with `hasattr`.
 * @return `true` if the attribute exists, `false` otherwise.
 */
REPY_IMPORT(REPY_bool REPY_HasAttr(REPY_Handle object, REPY_Handle key));

/**
 * @brief Checks if a Python object has an attribute with a specific name, using a NULL-terminated C string for the name.
 * 
 * This function is analogous to Python's own `hasattr` function, and should work in all the same circumstances.
 * 
 * @param object The parent Python object.
 * @param key The name of attribute to check. Should be a NULL-terminated C string.
 * @return `true` if the attribute exists, `false` otherwise.
 */
REPY_IMPORT(REPY_bool REPY_HasAttrCStr(REPY_Handle object, char* key));

/**
 * @brief Deletes an attribute from a Python object by name, using a `REPY_Handle` for the attribute name.
 * 
 * This function is analogous to Python's own `delattr`, and should work all the same circumstances.
 * 
 * @param object The parent Python object.
 * @param key The name of attribute to delete. The Python object referenced should generally be a `str`, as is standard with `delattr`.
 */
REPY_IMPORT(void REPY_DelAttr(REPY_Handle object, REPY_Handle key));

/**
 * @brief Deletes an attribute from a Python object by name, using a NULL-terminated C string for the name.
 * 
 * This function is analogous to Python's own `delattr`, and should work all the same circumstances.
 * 
 * @param object The parent Python object.
 * @param key The name of attribute to delete. Should be a NULL-terminated C string. Will be `REPY_NO_HANDLE` if an error occured.
 */
REPY_IMPORT(void REPY_DelAttrCStr(REPY_Handle object, char* key));
/** @}*/

/** \defgroup repy_iteration_funcs Iteration Functions
 * \brief Functions that operate on `REPY_Handle` values directly, rather than the Python objects they represent.
 * 
 * The same DLL mechanisms used by these functions are used internally by `REPY_IteratorHelper`.
 *  @{
 */

/**
 * @brief Gets an iterator for a given Python object.
 * 
 * This function is analogous to Python's own `iter` function, and should work in all the same circumstances.
 * 
 * @param object The Python object to get an iterator for. Will be `REPY_NO_HANDLE` if an error occured.
 */
REPY_IMPORT(REPY_Handle REPY_Iter(REPY_Handle object));

/**
 * @brief Gets the next Python object from a Python iterator.
 * 
 * This function is mostly analogous to Python's own `next` function, and should work in all the same circumstances.
 * 
 * Generally, this function returning `REPY_NO_OBJECT` indicates that an error has occured, but there is a special case. 
 * In Python, the end of iteraton is indicated by the raising of a `StopIteration` exception. Instead of requiring the user
 * to handle this exception manually handle this exception, the argument `process_stop_iteration` enables this functin 
 * silently and automatically handle the `StopIteration` exception while still returning `REPY_NO_OBJECT`. This way,
 * you can easily check whether iteration ended successfully by simply calling `REPY_IsErrorSet`, instead of having to
 * write more elaborate exception handling code.
 * 
 * @param iterator The Python iterator being used.
 * @param default_obj_nullable A default object to return if iteration is complete. Use `REPY_NO_OBJECT` to ignore this argument.
 * @param process_stop_iteration Set to `true` to enable automatic handling of the `StopIteration` exception. Set to `false` otherwise.
 * @return The next object from the iterator. Will be `REPY_NO_HANDLE` an error occured, or if `process_stop_iteration` is `true` and
 * iteration has ended
 */
REPY_IMPORT(REPY_Handle REPY_Next(REPY_Handle iterator, REPY_Handle default_obj_nullable, REPY_bool process_stop_iteration));

/** @}*/

/** \defgroup repy_tuple_funcs Tuple Functions
 * \brief Functions that operate on Python `tuple` objects.
 *  @{
 */

/**
 * @brief Create a Python `tuple` and return a `REPY_Handle` for it.
 * 
 * Because the `REPY_Handle` created by this function requires manual release, it's not recommended to use this function nested
 * inside `REPY_Call` (or any of it's sister functions) to pass positional arguments. Use `REPY_CreateTuple_SUH` instead.
 * 
 * @param size The number of entries in the tuple. Use 0 for an empty tuple.
 * @param ... `REPY_Handle` arguments for each entry in the tuple. Leave out if `size` is 0.
 * @return A `REPY_Handle` for the new tuple. Will be `REPY_NO_HANDLE` if an error occured.
 */
REPY_IMPORT(REPY_Handle REPY_CreateTuple(REPY_u32 size, ...));

/**
 * @brief Create a Python `tuple` and return a Single-Use `REPY_Handle` for it.
 * 
 * This is the recommended function for constructing a positional arguments `tuple` nested inside a call 
 * to `REPY_Call` (or any of it's sister functions).
 * 
 * At this time, this function is just shorthand for `REPY_MakeSUH(REPY_CreateTuple(size, ...))`, and thus will perform similarly.
 * However, internal performance improvements may make this function more performant in the future.
 * 
 * @param size The number of entries in the tuple. Use 0 for an empty tuple.
 * @param ... `REPY_Handle` arguments for each entry in the tuple. Leave out if `size` is 0.
 * @return A Single-Use `REPY_Handle` for the new tuple. Will be `REPY_NO_HANDLE` if an error occured.
 */
REPY_IMPORT(REPY_Handle REPY_CreateTuple_SUH(REPY_u32 size, ...));

/**
 * @brief Create a Python `tuple` with exactly 2 entries and return a `REPY_Handle` for it.
 * 
 * Used primarily for constucting key/value pairs when calling `REPY_CreateDict`. However, because the `REPY_Handle` 
 * created by this function requires manual release, it's not recommended to use this nest a call to this function 
 * inside another.
 * 
 * @param key `REPY_Handle` argument for the first entry in the tuple.
 * @param value `REPY_Handle` argument for the second entry in the tuple.
 * @return A `REPY_Handle` for the new tuple. Will be `REPY_NO_HANDLE` if an error occured.
 */
REPY_IMPORT(REPY_Handle REPY_CreatePair(REPY_Handle key, REPY_Handle value));

/**
 * @brief Create a Python `tuple` with exactly 2 entries and return a Single-Use `REPY_Handle` for it.
 * 
 * Used primarily for constucting key/value pairs when calling `REPY_CreateDict`. Because the returned handle is 
 * Single-Use, this is the recommended function for nesting inside `REPY_CreateDict` calls.
 * 
 * At this time, this function is just shorthand for `REPY_MakeSUH(REPY_CreatePair(key, value))`, and thus will perform similarly.
 * However, internal performance improvements may make this function more performant in the future.
 * 
 * @param key `REPY_Handle` argument for the first entry in the `tuple`.
 * @param value `REPY_Handle` argument for the second entry in the `tuple`.
 * @return A `REPY_Handle` for the new tuple. Will be `REPY_NO_HANDLE` if an error occured.
 */
REPY_IMPORT(REPY_Handle REPY_CreatePair_SUH(REPY_Handle key, REPY_Handle value));

/**
 * @brief Create a Python `tuple` with exactly 2 entries and return a `REPY_Handle` for it. The first entry is 
 * automatically cast to `str`.
 * 
 * Used primarily for constucting key/value pairs when calling `REPY_CreateDict`. However, because the `REPY_Handle` 
 * created by this function requires manual release, it's not recommended to use this nest a call to this function 
 * inside another.
 * 
 * @param key `REPY_Handle` argument for the first entry in the tuple.
 * @param value `REPY_Handle` argument for the second entry in the tuple.
 * @return A `REPY_Handle` for the new tuple. Will be `REPY_NO_HANDLE` if an error occured.
 */
REPY_IMPORT(REPY_Handle REPY_CreatePairCStr(char* key, REPY_Handle value));

/**
 * @brief Create a Python `tuple` with exactly 2 entries and return a Single-Use `REPY_Handle` for it. The first entry is 
 * automatically cast to `str`.
 * 
 * Used primarily for constucting key/value pairs when calling `REPY_CreateDict`. Because the returned handle is 
 * Single-Use, this is the recommended function for nesting inside `REPY_CreateDict` calls.
 * 
 * At this time, this function is just shorthand for `REPY_MakeSUH(REPY_CreatePair(key, value))`, and thus will perform similarly.
 * However, internal performance improvements may make this function more performant in the future.
 * 
 * @param key `REPY_Handle` argument for the first entry in the `tuple`.
 * @param value `REPY_Handle` argument for the second entry in the `tuple`.
 * @return A `REPY_Handle` for the new tuple. Will be `REPY_NO_HANDLE` if an error occured.
 */
REPY_IMPORT(REPY_Handle REPY_CreatePairCStr_SUH(char* key, REPY_Handle value));

/**
 * @brief Returns the Python object at a specific index from a `tuple`
 * 
 * Currently, is a known issue with this interface: Negative index values (which represent the position from the end of 
 * the `tuple` in Python code) do not currently work. This will be fixed in a future update.
 * 
 * Only works with Python `tuple` objects.
 * 
 * @param tuple The `tuple` to get an object from.
 * @param index The index of the desired object.
 * @return The Python object at `index` in `tuple`. Will be `REPY_NO_HANDLE` if an error occured.
 */
REPY_IMPORT(REPY_Handle REPY_TupleGetIndexS32(REPY_Handle tuple, int index));
/** @}*/

/** \defgroup repy_dict_funcs Dict Functions
 * \brief Functions that operate on Python `dict` objects.
 *  @{
 */

/**
 * @brief Create a Python `dict` and return a `REPY_Handle` for it.
 * 
 * Because the `REPY_Handle` created by this function requires manual release, it's not recommended to use this function nested
 * inside `REPY_Call` (or any of it's sister functions) to pass keyword arguments. Use `REPY_CreateDict_SUH` instead.
 * 
 * @param size The number of entries in the `dict`. Use 0 for an empty `dict`.
 * @param ... `REPY_Handle` arguments for each entry in the `dict`. Each argument should be a two-entry tuple representing a 
 * key/value pair (using `REPY_CreatePair` or `REPY_CreatePair_SUH` makes this easy). Leave out if `size` is 0. 
 * @return A `REPY_Handle` for the new `dict`. Will be `REPY_NO_HANDLE` if an error occured.
 */
REPY_IMPORT(REPY_Handle REPY_CreateDict(REPY_u32 size, ...));

/**
 * @brief Create a Python `dict` and return a `REPY_Handle` for it.
 * 
 * This is the recommended function for constructing a keyword arguments `tuple` nested inside a call  to `REPY_Call` 
 * (or any of it's sister functions).
 * 
 * At this time, this function is just shorthand for `REPY_MakeSUH(REPY_CreateDict(size, ...))`, and thus will perform similarly.
 * However, internal performance improvements may make this function more performant in the future.
 * 
 * @param size The number of entries in the `dict`. Use 0 for an empty `dict`.
 * @param ... `REPY_Handle` arguments for each entry in the `dict`. Each argument should be a two-entry tuple representing a 
 * key/value pair (using `REPY_CreatePair` or `REPY_CreatePair_SUH` makes this easy). Leave out if `size` is 0. 
 * @return A `REPY_Handle` for the new `dict`. Will be `REPY_NO_HANDLE` if an error occured.
 */
REPY_IMPORT(REPY_Handle REPY_CreateDict_SUH(REPY_u32 size, ...));

/**
 * @brief Get an entry from a Python `dict`, using a REPY handle for the key.
 * 
 * Note that any hashable Python type can be used as a dict key, not just `str` objects.
 * 
 * @param dict The `dict` to get an entry from.
 * @param key The key for the entry. Should be a hashable Python type.
 * @return The retrieved object. Will be `REPY_NO_HANDLE` if an error occured.
 */
REPY_IMPORT(REPY_Handle REPY_DictGet(REPY_Handle dict, REPY_Handle key));

/**
 * @brief Get an entry from a Python `dict`, using a NULL-terminated C string for the key.
 * 
 * The `key` variable will treated as a `str` object, as that is the most common use-case, as well as the use-case for
 * code execution scopes.
 * 
 * @param dict The `dict` to get an entry from.
 * @param key The key for the entry. Should be a NULL-terminated C string.
 * @return The retrieved object. Will be `REPY_NO_HANDLE` if an error occured.
 */
REPY_IMPORT(REPY_Handle REPY_DictGetCStr(REPY_Handle dict, char* key));

/**
 * @brief Sets/adds a Python object to a Python `dict`, using a REPY handle for the key.
 * 
 * Note that any hashable Python type can be used as a dict key, not just `str` objects.
 * 
 * @param dict The `dict` to insert into.
 * @param key The key for the entry. Should be a hashable Python type.
 * @param value The object to add.
 */
REPY_IMPORT(void REPY_DictSet(REPY_Handle dict, REPY_Handle key, REPY_Handle value));


/**
 * @brief Sets/adds a Python object to a Python `dict`, using a NULL-terminated C string for the key.
 * 
 * The `key` variable will treated as a `str` object, as that is the most common use-case, as well as the use-case for
 * code execution scopes.
 * 
 * @param dict The `dict` to insert into.
 * @param key The key for the entry. Should be a NULL-terminated C string.
 * @param value The object to add.
 */
REPY_IMPORT(void REPY_DictSetCStr(REPY_Handle dict, char* key, REPY_Handle value));

/**
 * @brief Checks if a Python `dict` has an entry with a specific key, using a REPY handle for the key.
 * 
 * Note that any hashable Python type can be used as a dict key, not just `str` objects.
 * 
 * @param dict The `dict` to check for a key in.
 * @param key The key for the entry. Should be a hashable Python type.
 * @return `true` if the entry exists, `false` otherwise.
 */
REPY_IMPORT(REPY_bool REPY_DictHas(REPY_Handle dict, REPY_Handle key));

/**
 * @brief Checks if a Python `dict` has an entry with a specific key, using a NULL-terminated C string for the key.
 * 
 * The `key` variable will treated as a `str` object, as that is the most common use-case, as well as the use-case for
 * code execution scopes.
 * 
 * @param dict The `dict` to check for a key in.
 * @param key The key for the entry. Should be a NULL-terminated C string.
 * @return `true` if the entry exists, `false` otherwise.
 */
REPY_IMPORT(REPY_bool REPY_DictHasCStr(REPY_Handle dict, char* key));

/**
 * @brief Remove an entry from a `dict` via it's key, using a REPY handle for the key.
 * 
 * Note that any hashable Python type can be used as a dict key, not just `str` objects.
 * 
 * @param dict The `dict` to remove an entry from.
 * @param key The key for the entry. Should be a hashable Python type.
 */
REPY_IMPORT(void REPY_DictDel(REPY_Handle dict, REPY_Handle key));

/**
 * @brief Remove an entry from a `dict` via it's key, using a NULL-terminated C string for the key.
 * 
 * The `key` variable will treated as a `str` object, as that is the most common use-case, as well as the use-case for
 * code execution scopes.
 * 
 * @param dict The `dict` to remove an entry from.
 * @param key The key for the entry. Should be a NULL-terminated C string.
 */
REPY_IMPORT(void REPY_DictDelCStr(REPY_Handle dict, char* key));
/** @}*/

/** \defgroup repy_code_exec_funcs Code Compilation and Execution Functions
 * \brief Functions that deal with the compiling and executing Python code from inside mod code.
 *  @{
 */

/**
 * @brief Compile a Python code string into Python bytecode, using a `REPY_Handle` for the text.
 * 
 * This function is generally analogous to Python's own `compile` function, and should work in all the same circumstances.
 * 
 * @param code The Python text object containing the code to compile. Python's `compile` function works accepts `str`, `bytes`, and `bytearray`.
 * @param identifier A Python text object identifying where the code came from. 
 * @param mode A Python `str` identifying the type of code this is. Use "exec" for an executable statement or series of statements, "eval"
 * for an expression, or "single" for a interactive REPL statement.
 * @return A handle to the resulting Python bytecode object. Will be `REPY_NO_HANDLE` if an error occured.
 */
REPY_IMPORT(REPY_Handle REPY_Compile(REPY_Handle code, REPY_Handle identifier, REPY_Handle mode));

/**
 * @brief Compile a Python code string into Python bytecode, using a NULL-terminated C string for the code.
 * 
 * This function is generally analogous to Python's own `compile` function, and should work in all the same circumstances.
 * 
 * @param code A NULL-terminated C string of Python code to compile.
 * @param identifier A NULL-terminated C string identifying where the code came from. 
 * @param mode The type of code this is. Use `REPY_CODE_EXEC` for an executable statement or series of statements, `REPY_CODE_EVAL`
 * for an expression, or `REPY_CODE_SINGLE` for a interactive REPL statement.
 * @return A handle to the resulting Python bytecode object. Will be `REPY_NO_HANDLE` if an error occured.
 */
REPY_IMPORT(REPY_Handle REPY_CompileCStr(const char* code, const char* identifier, REPY_CodeMode mode));

/**
 * @brief Compile a Python code string into Python bytecode, using a C `char` array for the code.
 * 
 * This function is generally analogous to Python's own `compile` function, and should work in all the same circumstances.
 * 
 * @param code A pointer to the beginning of the `char` array to compile.
 * @param len The length of the `char` array to compile.
 * @param identifier A NULL-terminated C string identifying where the code came from. 
 * @param mode The type of code this is. Use `REPY_CODE_EXEC` for an executable statement or series of statements, `REPY_CODE_EVAL`
 * for an expression, or `REPY_CODE_SINGLE` for a interactive REPL statement.
 * @return A handle to the resulting Python bytecode object. Will be `REPY_NO_HANDLE` if an error occured.
 */
REPY_IMPORT(REPY_Handle REPY_CompileCStrN(const char* code, REPY_u32 len, const char* identifier, REPY_CodeMode mode));

/**
 * @brief Execute Python code statements from a `REPY_Handle`. Optionally provide `dict` objects to serve as a scope.
 * 
 * This function is generally analogous to Python's `exec` function, with the exception that calling directly from the API
 * means that there is no Python stack frame to inherit a scope from when executing. Hence, this function will create it's own
 * if no scope `dict` objects are provided.
 * 
 * As this function can execute precompiled Python bytecode, this is the recommended function for executing Python statements from the API.
 * 
 * @param code The Python code to execute. A precompiled bytecode object is recommended for performance reasons, but a Python `str`,
 * `bytes`, or `bytearray` object will also work.
 * @param global_scope_nullable The global scope to execute code in. Use `REPY_NO_OBJECT` to execute in a new, temporary scope.
 * @param local_scope_nullable The global scope to execute code in. Using `REPY_NO_OBJECT` will make the global and local scopes
 * be the same.
 * @return `true` if the code executed without error, `false` otherwise.
 */
REPY_IMPORT(REPY_bool REPY_Exec(REPY_Handle code, REPY_Handle global_scope_nullable, REPY_Handle local_scope_nullable));

/**
 * @brief Execute Python code from a NULL-terminated C string. Optionally provide `dict` objects to serve as a scope.
 * 
 * This function is generally analogous to Python's `exec` function, with the exception that calling directly from the API
 * means that there is no Python stack frame to inherit a scope from when executing. Hence, this function will create it's own
 * if no scope `dict` objects are provided.
 * 
 * This function is not recommended for most use-cases, since it must recompile the Python bytecode from the `code` string 
 * with each run.
 * 
 * @param code The Python code to execute. Should be a NULL-terminated C string.
 * @param global_scope_nullable The global scope to execute code in. Use `REPY_NO_OBJECT` to execute in a new, temporary scope.
 * @param local_scope_nullable The global scope to execute code in. Using `REPY_NO_OBJECT` will make the global and local scopes
 * be the same.
 * @return `true` if the code executed without error, `false` otherwise.
 */
REPY_IMPORT(REPY_bool REPY_ExecCStr(const char* code, REPY_Handle global_scope_nullable, REPY_Handle local_scope_nullable));

/**
 * @brief Execute Python code from a C `char` array. Optionally provide `dict` objects to serve as a scope.
 * 
 * This function is generally analogous to Python's `exec` function, with the exception that calling directly from the API
 * means that there is no Python stack frame to inherit a scope from when executing. Hence, this function will create it's own
 * if no scope `dict` objects are provided.
 * 
 * This function is not recommended for most use-cases, since it must recompile the Python bytecode from the `code` string 
 * with each run.
 * 
 * @param code A pointer to the beginning of the `char` array code to execute.
 * @param len The length of the `char` array in bytes.
 * @param global_scope_nullable The global scope to execute code in. Use `REPY_NO_OBJECT` to execute in a new, temporary scope.
 * @param local_scope_nullable The global scope to execute code in. Using `REPY_NO_OBJECT` will make the global and local scopes
 * be the same.
 * @return `true` if the code executed without error, `false` otherwise.
 */
REPY_IMPORT(REPY_bool REPY_ExecCStrN(const char* code, REPY_u32 len, REPY_Handle global_scope_nullable, REPY_Handle local_scope_nullable));

/**
 * @brief Evaluate a Python code expression from a `REPY_Handle`. Optionally provide `dict` objects to serve as a scope.
 * 
 * This function is generally analogous to Python's `eval` function, with the exception that calling directly from the API
 * means that there is no Python stack frame to inherit a scope from when executing. Hence, this function will create it's own
 * if no scope `dict` objects are provided.
 * 
 * As this function can execute precompiled Python bytecode, this is the recommended function for evaluating Python expressions from the API.
 * 
 * @param code The Python expression to evaluate. A precompiled bytecode object is recommended for performance reasons, but a Python `str`,
 * `bytes`, or `bytearray` object will also work.
 * @param global_scope_nullable The global scope to execute code in. Use `REPY_NO_OBJECT` to execute in a new, temporary scope.
 * @param local_scope_nullable The global scope to execute code in. Using `REPY_NO_OBJECT` will make the global and local scopes
 * be the same.
 * @return A handle for the resulting object. Will be `REPY_NO_HANDLE` if an error occured.
 */
REPY_IMPORT(REPY_Handle REPY_Eval(REPY_Handle code, REPY_Handle global_scope_nullable, REPY_Handle local_scope_nullable));

/**
 * @brief Evaluate a Python code expression from a NULL-terminated C string. Optionally provide `dict` objects to serve as a scope.
 * 
 * This function is generally analogous to Python's `eval` function, with the exception that calling directly from the API
 * means that there is no Python stack frame to inherit a scope from when executing. Hence, this function will create it's own
 * if no scope `dict` objects are provided.
 * 
 * This function is not recommended for most use-cases, since it must recompile the Python bytecode from the `code` string 
 * with each run.
 * 
 * @param code The Python expression to evaluate. Should be a NULL-terminated C string.
 * @param global_scope_nullable The global scope to execute code in. Use `REPY_NO_OBJECT` to execute in a new, temporary scope.
 * @param local_scope_nullable The global scope to execute code in. Using `REPY_NO_OBJECT` will make the global and local scopes
 * be the same.
* @return A handle for the resulting object. Will be `REPY_NO_HANDLE` if an error occured.
 */
REPY_IMPORT(REPY_Handle REPY_EvalCStr(const char* code, REPY_Handle global_scope_nullable, REPY_Handle local_scope_nullable));

/**
 * @brief Evaluate a Python code expression from a C `char` array. Optionally provide `dict` objects to serve as a scope.
 * 
 * This function is generally analogous to Python's `eval` function, with the exception that calling directly from the API
 * means that there is no Python stack frame to inherit a scope from when executing. Hence, this function will create it's own
 * if no scope `dict` objects are provided.
 * 
 * This function is not recommended for most use-cases, since it must recompile the Python bytecode from the `code` string 
 * with each run.
 * 
 * @param code A pointer to the beginning of the `char` array expression to evaluate.
 * @param len The length of the `char` array in bytes.
 * @param global_scope_nullable The global scope to execute code in. Use `REPY_NO_OBJECT` to execute in a new, temporary scope.
 * @param local_scope_nullable The global scope to execute code in. Using `REPY_NO_OBJECT` will make the global and local scopes
 * be the same.
 * @return A handle for the resulting object. Will be `REPY_NO_HANDLE` if an error occured.
 */
REPY_IMPORT(REPY_Handle REPY_EvalCStrN(const char* code, REPY_u32 len, REPY_Handle global_scope_nullable, REPY_Handle local_scope_nullable));

/**
 * @brief Adds the Python object represented by a set of `REPY_Handle` handles to a dict, using the keys following the scheme `_0`, `_1`, `_2`, etc.
 * These keys serve as valid Python variable names to be used in code strings.
 * 
 * This is a convienience function for quick `REPY_Exec` and `REPY_Eval` statements where establishing a
 * scope or managing a whole dict is inconvenient. These keys serve as valid Python variable names to be used in code strings.
 * 
 * This function has slightly different behavior depending on whether or not `dict_nullable` is a valid dictionary.
 * If `dict_nullable` is `REPY_NO_OBJECT`, then a new dictionary will be created, and a new handle returned. Otherwise, the provided
 * `dict` will have new key-value pairs added to it, and **provided** handle is returned (that is to say, no new handle is created). 
 * Be advised that this will cause issues if `dict_nullable` is Single-Use.
 * 
 * @param dict_nullable_no_release Should be either a valid `REPY_Handle` for a dictionary, or `REPY_NO_OBJECT`.
 * @param size the number of Python objects to add to the `dict`
 * @param ... The Python objects to add to the `dict`. 
 * @return A `REPY_Handle` for the resulting dict. Will be the same as `dict_nullable` if that argument was set to anything other than
 * `REPY_NO_OBJECT`
 */
REPY_IMPORT(REPY_Handle REPY_VariadicLocals(REPY_Handle dict_nullable_no_release, REPY_u32 size, ...));

/**
 * @brief Shorthand for `REPY_VariadicLocals`
 * 
 */
#define REPY_VL REPY_VariadicLocals

/**
 * @brief Adds the Python object represented by a set of `REPY_handle`s to a dict, using the keys following the scheme `_0`, `_1`, `_2`, etc.
 * These keys serve as valid Python variable names to be used in code strings. This function also marks the resulting handle as Single-Use.
 * 
 * This is a convienience function for quick `REPY_Exec` and `REPY_Eval` statements where establishing a scope or managing a whole dict is inconvenient.
 * These keys serve as valid Python variable names to be used in code strings.
 * 
 * This function has slightly different behavior depending on whether or not `dict_nullable_no_release` is a valid dictionary.
 * If `dict_nullable` is `REPY_NO_OBJECT`, then a new dictionary will be created, and a new handle returned. Otherwise, the provided
 * `dict` will have new key-value pairs added to it, and **provided** handle is returned (that is to say, no new handle is created).
 * Be advised that this will cause issues if `dict_nullable` is Single-Use.
 * 
 * Also important, because this function marks the returned `REPY_Handle` as single use, but can potentially return the same handle as it was given,
 * the handle for `dict_nullable` will become Single-Use if it was previously permanent.
 * 
 * @param dict_nullable_no_release Should be either a valid `REPY_Handle` for a dictionary, or `REPY_NO_OBJECT`.
 * @param size the number of Python objects to add to the `dict`
 * @param ... The Python objects to add to the dict. 
 * @return A `REPY_Handle` for the resulting dict. Will be the same as `dict_nullable` if that argument was set to anything other than
 * `REPY_NO_OBJECT`.
 */
REPY_IMPORT(REPY_Handle REPY_VariadicLocals_SUH(REPY_Handle dict_nullable_no_release, REPY_u32 size, ...));

/**
 * @brief Shorthand for `REPY_VariadicLocals_SUH`
 * 
 */
#define REPY_VL_SUH REPY_VariadicLocals_SUH

/** @}*/

/** \defgroup repy_fn_call_funcs Python Function Calling
 * \brief Functions that call Python functions and other callables.
 *  @{
 */

/**
 * @brief Call a Python function (or another callable object) by handle, and discard the return value.
 * 
 * `REPY_CreateTuple_SUH` and `REPY_CreateDict_SUH` serve as easy methods of nesting argument construction into a call to this function.
 * 
 * @param func The Python function or callable to call. Should be the actual function object itself.
 * @param args_nullable A `tuple` of the positional arguments to pass to the function. Use `REPY_NO_OBJECT` to forgo positional arguments.
 * @param kwargs_nullable A `dict` of the keyword arguments to pass to the function. Use `REPY_NO_OBJECT` to forgo keyword arguments.
 * @return `true` if the call executed without error, `false` otherwise.
 */
REPY_IMPORT(REPY_bool REPY_Call(REPY_Handle func, REPY_Handle args_nullable, REPY_Handle kwargs_nullable));

/**
 * @brief Call a Python function (or another callable object) by handle, and get the return value as a `REPY_Handle`.
 * 
 * Note that technically all Python functions and callables return a value. Functions that are typically thought of not having a return
 * will actually return Python's `None` object. This mean that, unless an error occurs, this API function will always return a `REPY_Handle`
 * that will need to be released.
 * 
 * `REPY_CreateTuple_SUH` and `REPY_CreateDict_SUH` serve as easy methods of nesting argument construction into a call to this function.
 * 
 * @param func The Python function or callable to call. Should be the actual function object itself.
 * @param args_nullable A `tuple` of the positional arguments to pass to the function. Use `REPY_NO_OBJECT` to forgo positional arguments.
 * @param kwargs_nullable A `dict` of the keyword arguments to pass to the function. Use `REPY_NO_OBJECT` to forgo keyword arguments.
 * @return `true` if the call executed without error, `false` otherwise.
 */
REPY_IMPORT(REPY_Handle REPY_CallReturn(REPY_Handle func, REPY_Handle args_nullable, REPY_Handle kwargs_nullable));

/**
 * @brief Call a Python object member (such as a module function, object method, or another callable) by parent object and attribute
 * name, and discard the return value. The attribute name should be a Python `str`.
 * 
 * `REPY_CreateTuple_SUH` and `REPY_CreateDict_SUH` serve as easy methods of nesting argument construction into a call to this function.
 * 
 * @param object The Python object to call a member of.
 * @param name The name of the object attribute to call. Should be a Python `str`.
 * @param args_nullable A `tuple` of the positional arguments to pass to the function. Use `REPY_NO_OBJECT` to forgo positional arguments.
 * @param kwargs_nullable A `dict` of the keyword arguments to pass to the function. Use `REPY_NO_OBJECT` to forgo keyword arguments.
 * @return `true` if the call executed without error, `false` otherwise.
 */
REPY_IMPORT(REPY_bool REPY_CallAttr(REPY_Handle object, REPY_Handle name, REPY_Handle args_nullable, REPY_Handle kwargs_nullable));

/**
 * @brief Call a Python object member (such as a module function, object method, or another callable) by parent object and attribute
 * name, and discard the return value. The attribute name should be a NULL-terminated C string.
 * 
 * `REPY_CreateTuple_SUH` and `REPY_CreateDict_SUH` serve as easy methods of nesting argument construction into a call to this function.
 * 
 * @param object The Python object to call a member of.
 * @param name The name of the object attribute to call. Should be a NULL-terminated C string.
 * @param args_nullable A `tuple` of the positional arguments to pass to the function. Use `REPY_NO_OBJECT` to forgo positional arguments.
 * @param kwargs_nullable A `dict` of the keyword arguments to pass to the function. Use `REPY_NO_OBJECT` to forgo keyword arguments.
 * @return `true` if the call executed without error, `false` otherwise.
 */
REPY_IMPORT(REPY_bool REPY_CallAttrCStr(REPY_Handle object, char* name, REPY_Handle args_nullable, REPY_Handle kwargs_nullable));

/**
 * @brief Call a Python object member (such as a module function, object method, or another callable) by parent object and attribute
 * name, and discard the return value. The attribute name should be a NULL-terminated C string.
 * 
 * Note that technically all Python functions and callables return a value. Functions that are typically thought of not having a return
 * will actually return Python's `None` object. This mean that, unless an error occurs, this API function will always return a `REPY_Handle`
 * that will need to be released.
 * 
 * `REPY_CreateTuple_SUH` and `REPY_CreateDict_SUH` serve as easy methods of nesting argument construction into a call to this function.
 * 
 * @param object The Python object to call a member of.
 * @param name The name of the object attribute to call. Should be a NULL-terminated C string.
 * @param args_nullable A `tuple` of the positional arguments to pass to the function. Use `REPY_NO_OBJECT` to forgo positional arguments.
 * @param kwargs_nullable A `dict` of the keyword arguments to pass to the function. Use `REPY_NO_OBJECT` to forgo keyword arguments.
 * @return A handle for the resulting object. Will be `REPY_NO_HANDLE` if an error occured.
 */
REPY_IMPORT(REPY_Handle REPY_CallAttrReturn(REPY_Handle object, REPY_Handle name, REPY_Handle args_nullable, REPY_Handle kwargs_nullable));

/**
 * @brief Call a Python object member (such as a module function, object method, or another callable) by parent object and attribute
 * name, and discard the return value. The attribute name should be a NULL-terminated C string.
 * 
 * Note that technically all Python functions and callables return a value. Functions that are typically thought of not having a return
 * will actually return Python's `None` object. This mean that, unless an error occurs, this API function will always return a `REPY_Handle`
 * that will need to be released.
 * 
 * `REPY_CreateTuple_SUH` and `REPY_CreateDict_SUH` serve as easy methods of nesting argument construction into a call to this function.
 * 
 * @param object The Python object to call a member of.
 * @param name The name of the object attribute to call. Should be a NULL-terminated C string.
 * @param args_nullable A `tuple` of the positional arguments to pass to the function. Use `REPY_NO_OBJECT` to forgo positional arguments.
 * @param kwargs_nullable A `dict` of the keyword arguments to pass to the function. Use `REPY_NO_OBJECT` to forgo keyword arguments.
 * @return A handle for the resulting object. Will be `REPY_NO_HANDLE` if an error occured.
 */
REPY_IMPORT(REPY_Handle REPY_CallAttrCStrReturn(REPY_Handle object, char* name, REPY_Handle args_nullable, REPY_Handle kwargs_nullable));

/** @}*/

/** \defgroup repy_error_funcs Error Handling
 * \brief Functions used for Python error handling.
 * 
 * REPY API functions will try to catch any unhandled Python errors they raise, print them to the console, and make the exception 
 * objects available through this interface. Be advised that the Python error state is thread and interpreter specific, and
 * therefore these functions will only provide error information for the current interpreter and the current N64Recompiled thread.
 * Errors from pure Python threads (such as those created by `threading.Thread`) will need to be handled by Python code in their 
 * own threads.
 * 
 * Note that passing bad/invalid handles (or using `REPY_NO_OBJECT` when not allowed) will not result in Python 
 * errors to be raised, because looking up the Python object for a given handle happens before the interpreter is invoked. As a result,
 * such issues will result in a fatal error message being printed to REPY's log, followed by a fatal exit.
 * @{
 */

/**
 * @brief Checks if a Python exception was raised and caught by the REPY API.
 * 
 * If an exception was handled in Python code, then it will not be captured by REPY.
 * 
 * @return `true` if the REPY API currently has a Python exception captured, `false` otherwise.
 */
REPY_IMPORT(REPY_bool REPY_IsErrorSet());

/**
 * @brief Get the type of a captured Python exception for the current interpreter and thread.
 * 
 * Note that each invokation of this method will result in a new handle being created to the appropriate type object.
 * 
 * @return The type for the captured Python exception. Returns `None` if no exception is currently captured.
 */
REPY_IMPORT(REPY_Handle REPY_GetErrorType());

/**
 * @brief Get the trace object for the Python exception for the current interpreter and thread.
 * 
 * Note that each invokation of this method will result in a new handle being created to the trace object.
 * 
 * @return The trace object for the captured Python exception. Returns `None` if no exception is currently captured.
 */
REPY_IMPORT(REPY_Handle REPY_GetErrorTrace());

/**
 * @brief Get the captured Python exception object for the current interpreter and thread.
 * 
 * Note that each invokation of this method will result in a new handle being created to the exception.
 * 
 * @return The captured Python exception. Returns `None` if no exception is currently captured.
 */
REPY_IMPORT(REPY_Handle REPY_GetErrorValue());

/**
 * @brief Releases the captured exception for the current interpreter and thread.
 * 
 * Call once you're done with your error handling, so that `REPY_IsErrorSet` doesn't return `true` for an error already dealt with.
 */
REPY_IMPORT(void REPY_ClearError());

/** @}*/

/** \defgroup repy_zipfile_funcs Nrm ZipFile Functions
 * \brief Functions to access nrm files read-only as ZipFile objects.
 *  @{
 */

/**
 * @brief Opens a read-only instance of `zipfile.ZipFile` as indicated by `filepath`.
 * 
 * This function primarily exists to support the function `REPY_GetNrmZipFile`, since that needs to be `inline` in order to work, 
 * and having this implemented directly in the REPY external library is slightly more performant than making the corresponding API calls.
 * 
 * @param filepath a null-terminated path string to a zip file.
 * @return a REPY_Handle to a Python `zipfile.ZipFile` instance.
 */
REPY_IMPORT(REPY_Handle REPY_GetZipFileFromPath(const char* filepath));

/**
 * @brief Opens a read-only instance of `zipfile.ZipFile` for this `.nrm` file. Provides an easy way to access additional files inside the mod.
 * 
 * This function is implemented as `inline` within the `repy_api.h` header in order to get the path for the current `.nrm` file. 
 * It wraps the API function `REPY_GetZipFileFromPath`.
 * 
 * @return REPY_Handle 
 */
inline REPY_Handle REPY_GetNrmZipFile() {
    const char* filepath = (const char*) recomp_get_mod_file_path();
    REPY_Handle retVal = REPY_GetZipFileFromPath(filepath);
    recomp_free((void*) filepath);
    return retVal;
}

/** @}*/

/** \defgroup repy_helper_funcs Helper Functions
 * \brief Various helper functions, primarily used by API macros.
 * 
 * You don't usually need to invoke these directly.
 *  @{
 */
/**
 * @brief Constructs the `filename` strings used by most macros that enable inlining Python code into C files.
 * 
 * The pointer returned by this function must be freed with `recomp_free`. Failure to do so will result in a memory leak.
 * 
 * @param category A category prefix for the filename string. Usually the name of the invoking macro.
 * @param filename A C filename to associate with a piece of Python code. Usually `_FILE_NAME__`.
 * @param function_name The name of C function to associate with a piece of Python code. Usually `__func__`.
 * @param line_number A line number in a C file to associate with a piece of Python code. Usually `__LINE__`.
 * @param identifier An identifiying string for a piece of Python code. Usually the bytecode identifier from the C file.
 * @return The constructed source name as a NULL-terminated C string.
 */
REPY_IMPORT(char* REPY_InlineCodeSourceStrHelper(char* category, char* filename, char* function_name, REPY_u32 line_number, char* identifier));

/** \defgroup repy_iterator_helper_methods REPY_IteratorHelper Methods
 * \brief Method functions to operate on `REPY_IteratorHelper` objects.
 * 
 *  @{
 */

/**
 * @brief Create a `REPY_IteratorHelper` object on the heap.
 * 
 * Used by the `REPY_FOREACH` and `REPY_FN_FOREACH_CACHE` macros.
 * 
 * @param py_object a `REPY_Handle` for the Python object to iterate through.
 * @param py_scope_nullable a `REPY_Handle` to a Python `dict` being used a local scope. Can be `REPY_NO_OBJECT`.
 * @param var_name the variable name for the `REPY_IteratorHelper` pointer. If `py_scope_nullable` is set to `REPY_NO_OBJECT`, use `NULL`.
 * @param auto_destroy If true, the `REPY_IteratorHelper` will automatically be destroyed once the loop ends.
 * @return A pointer to the new `REPY_IteratorHelper` on the heap.
 */
REPY_IMPORT(REPY_IteratorHelper* REPY_IteratorHelper_Create(REPY_Handle py_object, REPY_Handle py_scope_nullable, const char* var_name, REPY_bool auto_destroy));

/**
 * @brief Destructs a `REPY_IteratorHelper` object from the heap.
 * 
 * Used by the various cleanup macros for `REPY_FOREACH` and `REPY_FN_FOREACH_CACHE`.
 * 
 * `REPY_IteratorHelper_Update` can also destroy a `REPY_IteratorHelper` when it's finished, if `auto_destroy` is set to true.
 * 
 * @param helper A pointer to the `REPY_IteratorHelper` to destroy.
 */
REPY_IMPORT(void REPY_IteratorHelper_Destroy(REPY_IteratorHelper* helper));

/**
 * @brief Makes the `REPY_IteratorHelper` move on to the next object in the iteration.
 * 
 * Used by the `REPY_FOREACH` and `REPY_FN_FOREACH_CACHE` macros.
 * 
 * @param helper A pointer to the `REPY_IteratorHelper` to update.
 * @return `true` if the iteration/loop should continue. `false` once it's time to end.
 */
REPY_IMPORT(REPY_bool REPY_IteratorHelper_Update(REPY_IteratorHelper* helper));

/**
 * @brief Gets the current index of a `REPY_IteratorHelper` object.
 * 
 * An index value isn't something intrinsic to Pythonic `for` loops. Instead, the `REPY_IteratorHelper`
 * internally counts what index value the iterator is on.
 * 
 * @param helper The `REPY_IteratorHelper` to get the index from.
 * @return The index of the iteration as an `unsigned int`.
 */
REPY_IMPORT(REPY_u32 REPY_IteratorHelper_GetIndex(REPY_IteratorHelper* helper));

/**
 * @brief Gets the `REPY_Handle` for a `REPY_IteratorHelper` object's current Python object.
 * 
 * The term `Borrow` is used because the lifetime of the returned `REPY_Handle` is managed by
 * the `REPY_IteratorHelper` itself, and therefore you should not release it manually.
 * 
 * If you need to access this object outside of the current iteration step, use `REPY_CopyHandle`.
 * 
 * @param helper The `REPY_teratorHelper` to get the `REPY_Handle` from.
 * @return The `REPY_Handle` referring to the current object from the `REPY_IteratorHelper`.
 */
REPY_IMPORT(REPY_Handle REPY_IteratorHelper_BorrowCurrent(REPY_IteratorHelper* helper));

/** @}*/

/** \defgroup repy_if_stmt_chain_methods REPY_IfStmtChain Methods
 * \brief Method functions to operate on `REPY_IfStmtChain` objects.
 * 
 *  @{
 */

/**
 * @brief Creates a new link in a `REPY_IfStmtChain` if statement chain.
 * 
 * Invoked as part of `REPY_IfStmtHelper` operations, but exposed for manual use here.
 * 
 * @param expr_string The Python expression to evaluate. Should be a NULL-terminated C string. Will be compiled into Python bytecode immediately.
 * @param filename A C filename to associate with the Python expression. Usually `_FILE_NAME__`.
 * @param function_name The name of C function to associate with the Python expression. Usually `__func__`.
 * @param line_number A line number in a C file to associate with the Python expression. Usually `__LINE__`.
 * @param identifier An identifiying string for the Python expression. Usually the bytecode identifier from the C file.
 * @return A pointer to the new `REPY_IfStmtChain` object.
 */
REPY_IMPORT(REPY_IfStmtChain* REPY_IfStmtChain_Create(char* expr_string, char* filename, char* function_name, REPY_u32 line_number, char* identifier));


/**
 * @brief Destructs a `REPY_IfStmtChain` object from the heap, recursively destructs and all additional links down the chain.
 * 
 * Exists for completeness sake. Since `REPY_IfStmtChain` pointers should usualy be `static` in their own functions, this doesn't really get much use.
 * 
 * @param chain A pointer to the `REPY_IfStmtChain` to recursively destroy.
 */
REPY_IMPORT(void REPY_IfStmtChain_Destroy(REPY_IfStmtChain* chain));

/**
 * @brief Returns a pointer to the next `IfStmtChain` in the series.
 *
 * The term `Borrow` is used because the lifetime of the returned `IfStmtChain` is managed by it's parent (the `chain` argument).
 * If the parent is destroyed (using `REPY_IfStmtChain_Destroy`), this pointer will no longer be valid.
 *
 * Destroying the returned `IfStmtChain` manually is not catastrophic so long as you set the parent's next link to `NULL`,
 * as this signals to `REPY_IfStmtHelper_Step` to simply regenerate the link. However, there isn't much benefit to doing so.
 * Chains are meant to be preserved as a means of code caching.
 * 
 * @param chain The `REPY_IfStmtChain` to get the next element from.
 * @return A pointer to the next element in the chain.
 */
REPY_IMPORT(REPY_IfStmtChain* REPY_IfStmtChain_BorrowNext(REPY_IfStmtChain* chain));

/**
 * @brief Sets the next `REPY_IfStmtChain` in a series.
 *
 * The term `Steal` is used because the `REPY_IfStmtHelper` represented by the `chain` argument (IE, the parent) assumes ownership
 * of the `REPY_IfStmtHelper` represented by `next`. Ergo, you shouldn't destroy `next` youself without setting the next link
 * of `chain` to `NULL`.
 * 
 * @param chain The `REPY_IfStmtChain` to set the next element for.
 * @param next The `REPY_IfStmtChain` to be the next element in the chain.
 */
REPY_IMPORT(void REPY_IfStmtChain_StealNext(REPY_IfStmtChain* chain, REPY_IfStmtChain* next));

/**
 * @brief Retrieves the evaluated bytecode expression for this `REPY_IfStmtChain` as a `REPY_Handle`.
 * 
 *  The term `Borrow` is used because the lifetime of the returned `REPY_Handle` is managed by the `REPY_IfStmtChain` specified in the
 * `chain` argument. If the parent is destroyed (using `REPY_IfStmtChain_Destroy`), this pointer will no longer be valid. Additionally,
 * you should not release this handle yourself, as `REPY_IfStmtHelper_Step` will not regenerate it.
 * 
 * @param chain The `REPY_IfStmtChain` to get the expression bytecode from.
 * @return The `REPY_Handle` for expression bytecode.
 */
REPY_IMPORT(REPY_Handle REPY_IfStmtChain_BorrowEvalBytecode(REPY_IfStmtChain* chain));

/**
 * @brief Sets 
 * @brief Retrieves the evaluated bytecode expression for this `REPY_IfStmtChain`.
 *
 * The term `Steal` is used because the `REPY_IfStmtHelper` represented by the `chain` argument (IE, the parent) assumes ownership
 * of the `REPY_Handle` passed in as `eval_bytecode`. Ergo, you shouldn't release `eval_bytecode` youself after this.
 * 
 * @param chain The `REPY_IfStmtChain` to set the expression bytecode for.
 * @param eval_bytecode A `REPY_Handle` to be the new expression bytecode.
 */
REPY_IMPORT(void REPY_IfStmtChain_StealEvalBytecode(REPY_IfStmtChain* chain, REPY_Handle eval_bytecode));

/** @}*/

/** \defgroup repy_if_stmt_helper_methods REPY_IfStmtHelper Methods
 * \brief Method functions to operate on `REPY_IfStmtHelper` objects.
 * 
 *  @{
 */

/**
 * @brief Initializes a `REPY_IfStmtHelper` for controlling managing a Pythonic if/else block.
 * 
 * Used by several of the various `REPY_FN_IF_CACHE` macros.
 * 
 * @param chain_root A pointer to the `REPY_IfStmtChain*` (ergo, a douple-pointer) variable for the first link in the if/else chain. For caching purposes, this
 * will usually be a `static` variable. If value of the variable at `root` us NULL, that will be taken to mean that the chain has not been created yet
 * (IE, this is the first run of this if/else block).
 * @return A pointer to the new `REPY_IfStmtHelper`.
 */
REPY_IMPORT(REPY_IfStmtHelper* REPY_IfStmtHelper_Create(REPY_IfStmtChain** chain_root));

/**
 * @brief Destroys and deallocates a `REPY_IfStmtHelper`
 * 
 * Note that the `REPY_IfStmtChain` attached to this helper will not be destroyed.
 * 
 * @param helper A pointer to the `REPY_IfStmtHelper` being destroyed.
 */
REPY_IMPORT(void REPY_IfStmtHelper_Destroy(REPY_IfStmtHelper* helper));


/**
 * @brief Steps through and evaluate the next link in the `REPY_IfStmtChain` chain provided to the `REPY_IfStmtHelper`, or creates a new link if one
 * doesn't exist.
 * 
 * Used by several of the various `REPY_FN_IF_CACHE` macros. Can be used within the actual C `if` statements themselves.
 * 
 * @param helper A pointer to the `REPY_IfStmtHelper` controlling this if/else block.
 * @param global_scope The global scope `dict` to evaluate the expression in.
 * @param local_scope The local scope `dict` to evaluate the expression in.
 * @param expr_string The Python expression to evaluate. Should be a NULL-terminated C string. Will be compiled into Python bytecode immediately.
 * This argument is only used if there is no next link in the `REPY_IfStmtChain` chain (meaning the link needs to be created).
 * @param filename A C filename to associate with the Python expression. Usually `_FILE_NAME__`.
 * This argument is only used if there is no next link in the `REPY_IfStmtChain` chain (meaning the link needs to be created).
 * @param function_name The name of C function to associate with the Python expression. Usually `__func__`.
 * This argument is only used if there is no next link in the `REPY_IfStmtChain` chain (meaning the link needs to be created).
 * @param line_number A line number in a C file to associate with the Python expression. Usually `__LINE__`.
 * This argument is only used if there is no next link in the `REPY_IfStmtChain` chain (meaning the link needs to be created).
 * @param identifier An identifiying string for the Python expression. Usually the bytecode identifier from the C file.
 * This argument is only used if there is no next link in the `REPY_IfStmtChain` chain (meaning the link needs to be created).
 */
REPY_IMPORT(REPY_bool REPY_IfStmtHelper_Step(REPY_IfStmtHelper* helper, REPY_Handle global_scope, REPY_Handle local_scope, char* expr_string, char* filename, char* function_name, REPY_u32 line_number, char* identifier));

/** @}*/

/** \defgroup repy_deferred_cleanup_helper_methods REPY_DeferredCleanupHelper Methods
 * \brief Method functions to operate on `REPY_DeferredCleanupHelper` objects.
 * 
 *  @{
 */

 /**
  * @brief Create a new `REPY_DeferredCleanupHelper` object.
  * 
  * Internally, allocation is handled via `recomp_alloc`.
  * 
  * @return A pointer to the new `REPY_DeferredCleanupHelper` instance.
  */
REPY_IMPORT(REPY_DeferredCleanupHelper* REPY_DeferredCleanupHelper_Create());

/**
 * @brief Adds a `REPY_Handle` to be release by a `REPY_DeferredCleanupHelper` instance.
 * 
 * The `REPY_DeferredCleanupHelper` will now be responsible for releasing this handle. Don't release it yourself,
 * or make the Single-Use. This function will not release a Single-Use handle if it recieves one.
 * 
 * For the sake of easily wrapping other function calls, this function returns the same `REPY_Handle` it was given.
 * 
 * @param cleanup The `REPY_DeferredCleanupHelper` to add `handle_no_release` to.
 * @param handle_no_release A `REPY_Handle` to be released by this cleanup helper.
 * @return the `REPY_Handle` specified in `handle_no_release`.
 */
REPY_IMPORT(REPY_Handle REPY_DeferredCleanupHelper_AddHandle(REPY_DeferredCleanupHelper* cleanup, REPY_Handle handle_no_release));

/**
 * @brief Adds a region of memory (specified by a `void*`) to be freed with `recomp_free` by a `REPY_DeferredCleanupHelper` instance.
 * 
 * The `REPY_DeferredCleanupHelper` will now be responsible for freeing this memory. Do not free it yourself.
 * 
 * The memory will be released using `recomp_free`. If `recomp_free` is not the appropriate for releasing this memory,
 * then do not use this mechanism. In the future, `REPY_DeferredCleanupHelper` may be updated to allow specifying custom
 * types to be released. 
 * 
 * For the sake of easily wrapping other function calls, this function returns the same `void*`it was given.
 * 
 * @param cleanup The `REPY_DeferredCleanupHelper` to add `pointer` to.
 * @param pointer A `void*` to be freed by this cleanup helper.
 * @return the `void*` specified in `pointer`.
 */
REPY_IMPORT(void* REPY_DeferredCleanupHelper_AddRecompFree(REPY_DeferredCleanupHelper* cleanup, void* pointer));

/**
 * @brief Adds a `REPY_IteratorHelper` to be destroyed by a `REPY_DeferredCleanupHelper` instance.
 * 
 * The `REPY_DeferredCleanupHelper` will now be responsible for destroying this `REPY_IteratorHelper`. Do not destroy it yourself.
 * 
 * For the sake of easily wrapping other function calls, this function returns the same `REPY_IteratorHelper*` it was given.
 * 
 * @param cleanup The `REPY_DeferredCleanupHelper` to add `iterator_helper` to.
 * @param iterator_helper A `REPY_IteratorHelper` to be released by this cleanup helper.
 * @return the `REPY_IteratorHelper` specified in `iterator_helper`.
 */
REPY_IMPORT(REPY_IteratorHelper* REPY_DeferredCleanupHelper_AddIteratorHelper(REPY_DeferredCleanupHelper* cleanup, REPY_IteratorHelper* iterator_helper));

/**
 * @brief Adds a `REPY_IfStmtHelper` to be destroyed by a `REPY_DeferredCleanupHelper` instance.
 * 
 * The `REPY_DeferredCleanupHelper` will now be responsible for destroying this `REPY_IfStmtHelper`. Do not destroy it yourself.
 * 
 * For the sake of easily wrapping other function calls, this function returns the same `REPY_IfStmtHelper*` it was given.
 * 
 * @param cleanup The `REPY_DeferredCleanupHelper` to add `if_stmt_helper` to.
 * @param if_stmt_helper A `REPY_IfStmtHelper` to be released by this cleanup helper.
 * @return the `REPY_IfStmtHelper` specified in `if_stmt_helper`.
 */
REPY_IMPORT(REPY_IfStmtHelper* REPY_DeferredCleanupHelper_AddIfStmtHelper(REPY_DeferredCleanupHelper* cleanup, REPY_IfStmtHelper* if_stmt_helper));

/**
 * @brief Clean up all of the resources registered to a `REPY_DeferredCleanupHelper` without destroying the instance itself.
 * 
 * The `REPY_DeferredCleanupHelper` now be considered empty again, and thus you can reuse it by adding new resources for it to clean up.
 * 
 * @param cleanup The `REPY_DeferredCleanupHelper` to run.
 */
REPY_IMPORT(void REPY_DeferredCleanupHelper_CleanNow(REPY_DeferredCleanupHelper* cleanup));

/**
 * @brief Destroy a `REPY_DeferredCleanupHelper` instance and clean up all of the resources registered to it.
 * 
 * It is safe to destroy a `REPY_DeferredCleanupHelper` instance that never had any resources added to it.
 * 
 * @param cleanup The `REPY_DeferredCleanupHelper` to run.
 */
REPY_IMPORT(void REPY_DeferredCleanupHelper_Destroy(REPY_DeferredCleanupHelper* cleanup));

/** @}*/
/** @}*/
/** @}*/
/** @}*/

#endif