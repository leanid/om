# Idempotent `git apply` for ExternalProject PATCH_COMMAND.
#
# Usage (working directory must be the external project's source dir):
#   cmake -DPATCH_FILE=/abs/path/to/fix.patch -P git-apply-or-skip.cmake
#
# Logic:
#   1. patch applies cleanly      -> apply it
#   2. patch is already applied   -> skip silently (success)
#   3. neither                    -> hard error (file changed upstream?)

if(NOT DEFINED PATCH_FILE)
    message(FATAL_ERROR "PATCH_FILE is not set")
endif()

execute_process(
    COMMAND git apply --check ${PATCH_FILE}
    RESULT_VARIABLE check_result
    OUTPUT_QUIET ERROR_QUIET)

if(check_result EQUAL 0)
    execute_process(COMMAND git apply ${PATCH_FILE}
                    COMMAND_ERROR_IS_FATAL ANY)
    message(STATUS "git-apply-or-skip: applied ${PATCH_FILE}")
    return()
endif()

execute_process(
    COMMAND git apply --reverse --check ${PATCH_FILE}
    RESULT_VARIABLE reverse_result
    OUTPUT_QUIET ERROR_QUIET)

if(reverse_result EQUAL 0)
    message(STATUS "git-apply-or-skip: ${PATCH_FILE} already applied, skipping")
else()
    message(
        FATAL_ERROR
            "git-apply-or-skip: ${PATCH_FILE} does not apply and is not "
            "applied - the patched file probably changed upstream")
endif()
