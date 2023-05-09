#pragma once

#define FAMILY_NAME "fshook_nl_fam"

enum fsh_nl_a_tl {
    FSH_NL_A_UNSPEC = 0,

    FSH_NL_A_TL_ID,
    FSH_NL_A_TL_PROC,
    FSH_NL_A_TL_TIMESPEC,
    FSH_NL_A_TL_EVENT_TYPE,
    FSH_NL_A_TL_EVENT,

    FSH_NL_A_TL_ALLOW,

    __FSH_NL_A_MAX,
};

#define FSH_NL_ATTRIBUTE_LEN __FSH_NL_A_MAX
#define FSH_NL_ATTRIBUTE_COUNT ( __FSH_NL_A_MAX - 1 )

enum fsh_nl_a_proc {
    FSH_NL_A_PID,
    FSH_NL_A_TID,
    FSH_NL_A_UID,
    FSH_NL_A_EXE,

    __FSH_PROC_MAX,
};

enum fsh_nl_a_timespec {
    FSH_NL_A_SEC,
    FSH_NL_A_NSEC,

    __FSH_TIMESPEC_MAX,
};

enum fsh_nl_a_open {
    FSH_NL_A_OPEN_PATH,
    FSH_NL_A_OPEN_FLAGS,

    __FSH_OPEN_MAX
};

enum fsh_nl_a_rename {
    FSH_NL_A_RENAME_SRC,
    FSH_NL_A_RENAME_DST,

    __FSH_RENAME_MAX
};

enum fsh_nl_a_unlink {
    FSH_NL_A_UNLINK_PATH,

    __FSH_UNLINK_MAX
};

enum fsh_nl_cmd {
    FSH_NL_C_UNSPEC = 0,

    FSH_NL_C_REGISTER,
    FSH_NL_C_MESSAGE,
    FSH_NL_C_REPLY,

    __FSH_NL_C_MAX,
};

#define FSH_NL_COMMAND_LEN __FSH_NL_C_MAX
#define FSH_NL_COMMAND_COUNT ( __FSH_NL_C_MAX - 1 )
