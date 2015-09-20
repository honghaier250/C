
/* config.h.  Generated from config.hin by configure.  */

/* config.hin.  Generated from configure.ac by autoheader.  */

/* Define if the compiler is building for multiple architectures of Apple
   platforms at once. */

/* #undef AA_APPLE_UNIVERSAL_BUILD */

/* Define to one if you want authentication. */

/* #undef AUTHENTICATION */

/* Define to the number of bits in type 'ptrdiff_t'. */

/* #undef BITSIZEOF_PTRDIFF_T */

/* Define to the number of bits in type 'sig_atomic_t'. */

/* #undef BITSIZEOF_SIG_ATOMIC_T */

/* Define to the number of bits in type 'size_t'. */

/* #undef BITSIZEOF_SIZE_T */

/* Define to the number of bits in type 'wchar_t'. */

/* #undef BITSIZEOF_WCHAR_T */

/* Define to the number of bits in type 'wint_t'. */

/* #undef BITSIZEOF_WINT_T */

/* Define to 1 if chown fails to change ctime when at least one argument was
   not -1. */

/* #undef CHOWN_CHANGE_TIME_BUG */

/* Define if chown is not POSIX compliant regarding IDs of -1. */

/* #undef CHOWN_FAILS_TO_HONOR_ID_OF_NEGATIVE_ONE */

/* Define if chown modifies symlinks. */

/* #undef CHOWN_MODIFIES_SYMLINK */

/* Define to 1 if chown mishandles trailing slash. */

/* #undef CHOWN_TRAILING_SLASH_BUG */

/* Define to 1 if the `closedir' function returns void instead of `int'. */

/* #undef CLOSEDIR_VOID */

/* Define to one of `_getb67', `GETB67', `getb67' for Cray-2 and Cray-YMP
   systems. This function is required for `alloca.c' support on those systems.
   */

/* #undef CRAY_STACKSEG_END */

/* Define to 1 if using `alloca.c'. */

/* #undef C_ALLOCA */

/* FIXME */

/* #undef DES_ENCRYPTION */

/* the name of the file descriptor member of DIR */

/* #undef DIR_FD_MEMBER_NAME */

#ifdef DIR_FD_MEMBER_NAME
# define DIR_TO_FD(Dir_p) ((Dir_p)->DIR_FD_MEMBER_NAME)
#else
# define DIR_TO_FD(Dir_p) -1
#endif


/* Define to 1 if // is a file system root distinct from /. */

/* #undef DOUBLE_SLASH_IS_DISTINCT_ROOT */

/* Define if struct dirent has a member d_ino that actually works. */
#define D_INO_IN_DIRENT 1

/* Define to one if you want encryption. */

/* #undef ENCRYPTION */

/* 1 */

/* #undef EWOULDBLOCK */

/* Define to 1 if your platform has fchownat, but it cannot perform lchown
   tasks. */

/* #undef FCHOWNAT_NOFOLLOW_BUG */

/* Define on systems for which file names may have a so-called `drive letter'
   prefix, define this to compute the length of that prefix, including the
   colon. */
#define FILE_SYSTEM_ACCEPTS_DRIVE_LETTER_PREFIX 0

/* Define if the backslash character may also serve as a file name component
   separator. */
#define FILE_SYSTEM_BACKSLASH_IS_FILE_NAME_SEPARATOR 0

/* Define if a drive letter prefix denotes a relative path if it is not
   followed by a file name component separator. */
#define FILE_SYSTEM_DRIVE_PREFIX_CAN_BE_RELATIVE 0

/* Define to 1 if fopen() fails to recognize a trailing slash. */

/* #undef FOPEN_TRAILING_SLASH_BUG */

/* Define to 1 if mkdir mistakenly creates a directory given with a trailing
   dot component. */

/* #undef FUNC_MKDIR_DOT_BUG */

/* Define to 1 when using the gnulib module fopen-safer. */
#define GNULIB_FOPEN_SAFER 1

/* Define to 1 when using the gnulib module getopt-gnu. */
#define GNULIB_GETOPT_GNU 1

/* Define to indicate the 'malloc' module. */
#define GNULIB_MALLOC_GNU 1

/* Define to 1 when using the gnulib module open. */
#define GNULIB_OPEN 1

/* Define to 1 to add extern declaration of program_invocation_name to argp.h
   */

/* #undef GNULIB_PROGRAM_INVOCATION_NAME */

/* Define to 1 to add extern declaration of program_invocation_short_name to
   argp.h */

/* #undef GNULIB_PROGRAM_INVOCATION_SHORT_NAME */

/* Define to indicate the 'realloc' module. */
#define GNULIB_REALLOC_GNU 1

/* FIXME */
#define HAVE_AF_INET6 1

/* Define to 1 if you have `alloca', as a function or macro. */
#define HAVE_ALLOCA 1

/* Define to 1 if you have <alloca.h> and it should be used (not on Ultrix).
   */
#define HAVE_ALLOCA_H 1

/* Define to 1 if you have the <arpa/nameser.h> header file. */
#define HAVE_ARPA_NAMESER_H 1

/* Define if you have weak "assembler" references */

/* #undef HAVE_ASM_WEAK_REFS */

/* Define if you have weak "attribute" references */

/* #undef HAVE_ATTR_WEAK_REFS */

/* Define to 1 if you have the `bcmp' function. */
#define HAVE_BCMP 1

/* Define to 1 if you have the `bcopy' function. */
#define HAVE_BCOPY 1

/* Define to 1 if you have the <bp-sym.h> header file. */

/* #undef HAVE_BP_SYM_H */

/* Define to 1 if you have the `btowc' function. */
#define HAVE_BTOWC 1

/* Define to 1 if you have the `bzero' function. */
#define HAVE_BZERO 1

/* Define to 1 if the system has the type `cc_t'. */
#define HAVE_CC_T 1

/* Define to 1 if you have the `cfsetspeed' function. */
#define HAVE_CFSETSPEED 1

/* Define to 1 if you have the `cgetent' function. */

/* #undef HAVE_CGETENT */

/* Define to 1 if you have the `chown' function. */
#define HAVE_CHOWN 1

/* Define to 1 if you have the `crypt' function. */
#define HAVE_CRYPT 1

/* Define to 1 if you have the declaration of `clearerr_unlocked', and to 0 if
   you don't. */
#define HAVE_DECL_CLEARERR_UNLOCKED 1

/* Define to 1 if you have the declaration of `crypt', and to 0 if you don't.
   */
#define HAVE_DECL_CRYPT 1

/* Define to 1 if you have the declaration of `dirfd', and to 0 if you don't.
   */
#define HAVE_DECL_DIRFD 1

/* Define to 1 if you have the declaration of `environ', and to 0 if you
   don't. */
#define HAVE_DECL_ENVIRON 1

/* Define to 1 if you have the declaration of `errno', and to 0 if you don't.
   */
#define HAVE_DECL_ERRNO 1

/* Define to 1 if you have the declaration of `fclose', and to 0 if you don't.
   */
#define HAVE_DECL_FCLOSE 1

/* Define to 1 if you have the declaration of `feof_unlocked', and to 0 if you
   don't. */
#define HAVE_DECL_FEOF_UNLOCKED 1

/* Define to 1 if you have the declaration of `ferror_unlocked', and to 0 if
   you don't. */
#define HAVE_DECL_FERROR_UNLOCKED 1

/* Define to 1 if you have the declaration of `fflush_unlocked', and to 0 if
   you don't. */
#define HAVE_DECL_FFLUSH_UNLOCKED 1

/* Define to 1 if you have the declaration of `fgets_unlocked', and to 0 if
   you don't. */
#define HAVE_DECL_FGETS_UNLOCKED 1

/* Define to 1 if you have the declaration of `flockfile', and to 0 if you
   don't. */
#define HAVE_DECL_FLOCKFILE 1

/* Define to 1 if you have the declaration of `fputc_unlocked', and to 0 if
   you don't. */
#define HAVE_DECL_FPUTC_UNLOCKED 1

/* Define to 1 if you have the declaration of `fputs_unlocked', and to 0 if
   you don't. */
#define HAVE_DECL_FPUTS_UNLOCKED 1

/* Define to 1 if you have the declaration of `fread_unlocked', and to 0 if
   you don't. */
#define HAVE_DECL_FREAD_UNLOCKED 1

/* Define to 1 if you have the declaration of `funlockfile', and to 0 if you
   don't. */
#define HAVE_DECL_FUNLOCKFILE 1

/* Define to 1 if you have the declaration of `fwrite_unlocked', and to 0 if
   you don't. */
#define HAVE_DECL_FWRITE_UNLOCKED 1

/* Define to 1 if you have the declaration of `getchar_unlocked', and to 0 if
   you don't. */
#define HAVE_DECL_GETCHAR_UNLOCKED 1

/* Define to 1 if you have the declaration of `getcwd', and to 0 if you don't.
   */
#define HAVE_DECL_GETCWD 1

/* Define to 1 if you have the declaration of `getc_unlocked', and to 0 if you
   don't. */
#define HAVE_DECL_GETC_UNLOCKED 1

/* Define to 1 if you have the declaration of `getdelim', and to 0 if you
   don't. */
#define HAVE_DECL_GETDELIM 1

/* Define to 1 if you have the declaration of `getenv', and to 0 if you don't.
   */
#define HAVE_DECL_GETENV 1

/* Define to 1 if you have the declaration of `getgrnam', and to 0 if you
   don't. */
#define HAVE_DECL_GETGRNAM 1

/* Define to 1 if you have the declaration of `getline', and to 0 if you
   don't. */
#define HAVE_DECL_GETLINE 1

/* Define to 1 if you have the declaration of `getlogin', and to 0 if you
   don't. */
#define HAVE_DECL_GETLOGIN 1

/* Define to 1 if you have the declaration of `getlogin_r', and to 0 if you
   don't. */
#define HAVE_DECL_GETLOGIN_R 1

/* Define to 1 if you have the declaration of `getpass', and to 0 if you
   don't. */
#define HAVE_DECL_GETPASS 1

/* Define to 1 if you have the declaration of `getusershell', and to 0 if you
   don't. */
#define HAVE_DECL_GETUSERSHELL 1

/* Define to 1 if you have the declaration of `getutent', and to 0 if you
   don't. */
#define HAVE_DECL_GETUTENT 1

/* Define to 1 if you have the declaration of `htons', and to 0 if you don't.
   */
#define HAVE_DECL_HTONS 1

/* Define to 1 if you have the declaration of `initgroups', and to 0 if you
   don't. */
#define HAVE_DECL_INITGROUPS 1

/* Define to 1 if you have the declaration of `isblank', and to 0 if you
   don't. */
#define HAVE_DECL_ISBLANK 1

/* Define to 1 if you have the declaration of `memrchr', and to 0 if you
   don't. */
#define HAVE_DECL_MEMRCHR 1

/* Define to 1 if you have the declaration of `optreset', and to 0 if you
   don't. */
#define HAVE_DECL_OPTRESET 0

/* Define to 1 if you have the declaration of `pclose', and to 0 if you don't.
   */
#define HAVE_DECL_PCLOSE 1

/* Define to 1 if you have the declaration of `program_invocation_name', and
   to 0 if you don't. */
#define HAVE_DECL_PROGRAM_INVOCATION_NAME 1

/* Define to 1 if you have the declaration of `program_invocation_short_name',
   and to 0 if you don't. */
#define HAVE_DECL_PROGRAM_INVOCATION_SHORT_NAME 1

/* Define to 1 if you have the declaration of `putchar_unlocked', and to 0 if
   you don't. */
#define HAVE_DECL_PUTCHAR_UNLOCKED 1

/* Define to 1 if you have the declaration of `putc_unlocked', and to 0 if you
   don't. */
#define HAVE_DECL_PUTC_UNLOCKED 1

/* Define to 1 if you have the declaration of `sleep', and to 0 if you don't.
   */
#define HAVE_DECL_SLEEP 1

/* Define to 1 if you have the declaration of `snprintf', and to 0 if you
   don't. */
#define HAVE_DECL_SNPRINTF 1

/* Define to 1 if you have the declaration of `strdup', and to 0 if you don't.
   */
#define HAVE_DECL_STRDUP 1

/* Define to 1 if you have the declaration of `strerror', and to 0 if you
   don't. */

/* #undef HAVE_DECL_STRERROR */

/* Define to 1 if you have the declaration of `strerror_r', and to 0 if you
   don't. */
#define HAVE_DECL_STRERROR_R 1

/* Define to 1 if you have the declaration of `strmode', and to 0 if you
   don't. */
#define HAVE_DECL_STRMODE 0

/* Define to 1 if you have the declaration of `strncasecmp', and to 0 if you
   don't. */
#define HAVE_DECL_STRNCASECMP 1

/* Define to 1 if you have the declaration of `strndup', and to 0 if you
   don't. */
#define HAVE_DECL_STRNDUP 1

/* Define to 1 if you have the declaration of `strnlen', and to 0 if you
   don't. */
#define HAVE_DECL_STRNLEN 1

/* Define to 1 if you have the declaration of `sys_siglist', and to 0 if you
   don't. */
#define HAVE_DECL_SYS_SIGLIST 1

/* Define to 1 if you have the declaration of `ttyname', and to 0 if you
   don't. */
#define HAVE_DECL_TTYNAME 1

/* Define to 1 if you have the declaration of `vsnprintf', and to 0 if you
   don't. */
#define HAVE_DECL_VSNPRINTF 1

/* Define to 1 if you have the declaration of `_snprintf', and to 0 if you
   don't. */
#define HAVE_DECL__SNPRINTF 0

/* Define to 1 if you have the declaration of `__fsetlocking', and to 0 if you
   don't. */

/* #undef HAVE_DECL___FSETLOCKING */

/* Define to 1 if you have the <des.h> header file. */

/* #undef HAVE_DES_H */

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the `dirfd' function. */
#define HAVE_DIRFD 1

/* Define to 1 if you have the `dup2' function. */
#define HAVE_DUP2 1

/* Define if you have the declaration of environ. */
#define HAVE_ENVIRON_DECL 1

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* FIXME */
#define HAVE_EWOULDBLOCK 1

/* Define to 1 if you have the `fchdir' function. */
#define HAVE_FCHDIR 1

/* Define to 1 if you have the `fchmodat' function. */
#define HAVE_FCHMODAT 1

/* Define to 1 if you have the `fchown' function. */
#define HAVE_FCHOWN 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the `fdopendir' function. */
#define HAVE_FDOPENDIR 1

/* FIXME */
#define HAVE_FD_SET_MACROS 1

/* FIXME */

/* #undef HAVE_FD_SET_MACROS_IN_SYS_TIME_H */

/* Define to 1 if you have the <features.h> header file. */
#define HAVE_FEATURES_H 1

/* Define to 1 if you have the <float.h> header file. */
#define HAVE_FLOAT_H 1

/* Define to 1 if you have the `flock' function. */
#define HAVE_FLOCK 1

/* Define to 1 if you have the `flockfile' function. */
#define HAVE_FLOCKFILE 1

/* Define to 1 if you have the `fork' function. */
#define HAVE_FORK 1

/* Define to 1 if you have the `fpathconf' function. */
#define HAVE_FPATHCONF 1

/* Define to 1 if fseeko (and presumably ftello) exists and is declared. */
#define HAVE_FSEEKO 1

/* Define to 1 if you have the `fstatat' function. */
#define HAVE_FSTATAT 1

/* Define to 1 if you have the `ftruncate' function. */
#define HAVE_FTRUNCATE 1

/* Define to 1 if you have the `funlockfile' function. */
#define HAVE_FUNLOCKFILE 1

/* Define to 1 if you have getaddrinfo(3) */
#define HAVE_GETADDRINFO 1

/* Define to 1 if you have the `getcwd' function. */
#define HAVE_GETCWD 1

/* Define to 1 if you have the `getdelim' function. */
#define HAVE_GETDELIM 1

/* Define to 1 if you have the `getdomainname' function. */
#define HAVE_GETDOMAINNAME 1

/* Define to 1 if you have the `gethostname' function. */
#define HAVE_GETHOSTNAME 1

/* Define to 1 if you have the `getlogin_r' function. */
#define HAVE_GETLOGIN_R 1

/* Define to 1 if you have the `getmsg' function. */

/* #undef HAVE_GETMSG */

/* Define to 1 if you have the <getopt.h> header file. */
#define HAVE_GETOPT_H 1

/* Define to 1 if you have the `getopt_long_only' function. */
#define HAVE_GETOPT_LONG_ONLY 1

/* Define to 1 if you have the `getpagesize' function. */
#define HAVE_GETPAGESIZE 1

/* Define to 1 if you have the `getpass' function. */
#define HAVE_GETPASS 1

/* Define to 1 if you have the `getpwnam_r' function. */
#define HAVE_GETPWNAM_R 1

/* Define to 1 if you have the `getspnam' function. */
#define HAVE_GETSPNAM 1

/* Define to 1 if you have the `getusershell' function. */
#define HAVE_GETUSERSHELL 1

/* Define to 1 if you have the <glob.h> header file. */
#define HAVE_GLOB_H 1

/* Define to one if you have hstrerror. */
#define HAVE_HSTRERROR 1

/* Define to one if you have h_errno. */
#define HAVE_H_ERRNO 1

/* Define to 1 if you have the `initgroups' function. */
#define HAVE_INITGROUPS 1

/* Define to 1 if you have the `initsetproctitle' function. */

/* #undef HAVE_INITSETPROCTITLE */

/* Define to 1 if the compiler supports one of the keywords 'inline',
   '__inline__', '__inline' and effectively inlines functions marked as such.
   */

/* #undef HAVE_INLINE */

/* Define if you have the 'intmax_t' type in <stdint.h> or <inttypes.h>. */
#define HAVE_INTMAX_T 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define if <inttypes.h> exists, doesn't clash with <sys/types.h>, and
   declares uintmax_t. */
#define HAVE_INTTYPES_H_WITH_UINTMAX 1

/* FIXME */
#define HAVE_IPV6_V6ONLY 1

/* Define to 1 if you have the `isblank' function. */
#define HAVE_ISBLANK 1

/* Define to 1 if you have the `iswcntrl' function. */
#define HAVE_ISWCNTRL 1

/* Define to 1 if you have the `iswctype' function. */
#define HAVE_ISWCTYPE 1

/* Define to 1 if you have the `killpg' function. */
#define HAVE_KILLPG 1

/* Define to 1 if you have the <krb.h> header file. */

/* #undef HAVE_KRB_H */

/* Define if you have <langinfo.h> and nl_langinfo(CODESET). */
#define HAVE_LANGINFO_CODESET 1

/* Define to 1 if you have the `lchmod' function. */

/* #undef HAVE_LCHMOD */

/* Define to 1 if you have the `lchown' function. */
#define HAVE_LCHOWN 1

/* Define to 1 if you have the `dl' library (-ldl). */

/* #undef HAVE_LIBDL */

/* Define to one if you have -lhistory */

/* #undef HAVE_LIBHISTORY */

/* Define to 1 if you have the <libintl.h> header file. */
#define HAVE_LIBINTL_H 1

/* Define to 1 if you have the `nsl' library (-lnsl). */
#define HAVE_LIBNSL 1

/* Define to 1 if you have the `opie' library (-lopie). */

/* #undef HAVE_LIBOPIE */

/* Define to one if you have -lreadline */

/* #undef HAVE_LIBREADLINE */

/* Define to 1 if you have the `resolv' library (-lresolv). */
#define HAVE_LIBRESOLV 1

/* Define to 1 if you have the `socket' library (-lsocket). */

/* #undef HAVE_LIBSOCKET */

/* Define to 1 if you have the <linewrap.h> header file. */

/* #undef HAVE_LINEWRAP_H */

/* Define to 1 if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1

/* Define to 1 if the system has the type `long long int'. */
#define HAVE_LONG_LONG_INT 1

/* Define to 1 if you have the `lstat' function. */
#define HAVE_LSTAT 1

/* Define to 1 if your system has a GNU libc compatible `malloc' function, and
   to 0 otherwise. */
#define HAVE_MALLOC 1

/* Define to 1 if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define if the 'malloc' function is POSIX compliant. */
#define HAVE_MALLOC_POSIX 1

/* Define to 1 if mmap()'s MAP_ANONYMOUS flag is available after including
   config.h and <sys/mman.h>. */
#define HAVE_MAP_ANONYMOUS 1

/* Define to 1 if you have the `mbrtowc' function. */
#define HAVE_MBRTOWC 1

/* Define to 1 if you have the `mbsinit' function. */
#define HAVE_MBSINIT 1

/* Define to 1 if you have the `mbsrtowcs' function. */
#define HAVE_MBSRTOWCS 1

/* Define to 1 if <wchar.h> declares mbstate_t. */
#define HAVE_MBSTATE_T 1

/* Define to 1 if you have the `memchr' function. */
#define HAVE_MEMCHR 1

/* Define to 1 if you have the `memcmp' function. */
#define HAVE_MEMCMP 1

/* Define to 1 if you have the `memcpy' function. */
#define HAVE_MEMCPY 1

/* Define to 1 if you have the `memmove' function. */
#define HAVE_MEMMOVE 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `mempcpy' function. */
#define HAVE_MEMPCPY 1

/* Define to 1 if you have the `memrchr' function. */
#define HAVE_MEMRCHR 1

/* Define to 1 if you have the `memset' function. */
#define HAVE_MEMSET 1

/* Define to 1 if <limits.h> defines the MIN and MAX macros. */

/* #undef HAVE_MINMAX_IN_LIMITS_H */

/* Define to 1 if <sys/param.h> defines the MIN and MAX macros. */
#define HAVE_MINMAX_IN_SYS_PARAM_H 1

/* Define to 1 if you have the `mkdirat' function. */
#define HAVE_MKDIRAT 1

/* Define to 1 if you have the `mkstemp' function. */
#define HAVE_MKSTEMP 1

/* Define to 1 if you have a working `mmap' system call. */
#define HAVE_MMAP 1

/* Define to 1 if you have the `mprotect' function. */
#define HAVE_MPROTECT 1

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */

/* #undef HAVE_NDIR_H */

/* Define to 1 if you have the <netdb.h> header file. */
#define HAVE_NETDB_H 1

/* Define to 1 if you have the <netinet/in_systm.h> header file. */
#define HAVE_NETINET_IN_SYSTM_H 1

/* Define to 1 if you have the <netinet/ip.h> header file. */
#define HAVE_NETINET_IP_H 1

/* Define to 1 if you have the <netinet/ip_icmp.h> header file. */
#define HAVE_NETINET_IP_ICMP_H 1

/* Define to 1 if you have the <netinet/ip_var.h> header file. */

/* #undef HAVE_NETINET_IP_VAR_H */

/* Define to 1 if libc includes obstacks. */
#define HAVE_OBSTACK 1

/* Define to 1 if you have the `openat' function. */
#define HAVE_OPENAT 1

/* Define to 1 if you have the `opendir2' function. */

/* #undef HAVE_OPENDIR2 */

/* Define to one if you have <osockaddr.h>. */

/* #undef HAVE_OSOCKADDR_H */

/* Define to 1 if getcwd works, except it sometimes fails when it shouldn't,
   setting errno to ERANGE, ENAMETOOLONG, or ENOENT. If __GETCWD_PREFIX is not
   defined, it doesn't matter whether HAVE_PARTLY_WORKING_GETCWD is defined.
   */
#define HAVE_PARTLY_WORKING_GETCWD 1

/* Define if you have the <paths.h> header file */
#define HAVE_PATHS_H 1

/* Define to 1 if you have the `pipe' function. */
#define HAVE_PIPE 1

/* Define to 1 if you have the `poll' function. */
#define HAVE_POLL 1

/* Define to one if you have <poll.h> */
#define HAVE_POLL_H 1

/* Define if you have weak "pragma" references */
#define HAVE_PRAGMA_WEAK_REFS 1

/* Define if program_invocation_name is defined */
#define HAVE_PROGRAM_INVOCATION_NAME 1

/* Define if program_invocation_short_name is defined */
#define HAVE_PROGRAM_INVOCATION_SHORT_NAME 1

/* Define to 1 if you have the `ptsname' function. */
#define HAVE_PTSNAME 1

/* Define to 1 if you have the <pty.h> header file. */
#define HAVE_PTY_H 1

/* Define to 1 if you have the <random.h> header file. */

/* #undef HAVE_RANDOM_H */

/* Define to 1 if you have the `rawmemchr' function. */
#define HAVE_RAWMEMCHR 1

/* Define if you have the readline library. */

/* #undef HAVE_READLINE */

/* Define to 1 if you have the <readline/readline.h> header file. */

/* #undef HAVE_READLINE_READLINE_H */

/* Define to 1 if your system has a GNU libc compatible `realloc' function,
   and to 0 otherwise. */
#define HAVE_REALLOC 1

/* Define if the 'realloc' function is POSIX compliant. */
#define HAVE_REALLOC_POSIX 1

/* Define to 1 if you have the `rmdir' function. */
#define HAVE_RMDIR 1

/* Define to 1 if the system has the type `sa_family_t'. */
#define HAVE_SA_FAMILY_T 1

/* Define to 1 if you have the <search.h> header file. */
#define HAVE_SEARCH_H 1

/* Define to 1 if you have the <security/pam_appl.h> header file. */

/* #undef HAVE_SECURITY_PAM_APPL_H */

/* Define to 1 if you have the `setegid' function. */
#define HAVE_SETEGID 1

/* Define to 1 if you have the `setenv' function. */
#define HAVE_SETENV 1

/* Define to 1 if you have the `seteuid' function. */
#define HAVE_SETEUID 1

/* Define to 1 if you have the `setpgid' function. */
#define HAVE_SETPGID 1

/* Define to 1 if you have the `setproctitle' function. */

/* #undef HAVE_SETPROCTITLE */

/* Define to 1 if you have the `setregid' function. */
#define HAVE_SETREGID 1

/* Define to 1 if you have the `setresgid' function. */
#define HAVE_SETRESGID 1

/* Define to 1 if you have the `setresuid' function. */
#define HAVE_SETRESUID 1

/* Define to 1 if you have the `setreuid' function. */
#define HAVE_SETREUID 1

/* Define to 1 if you have the `setsid' function. */
#define HAVE_SETSID 1

/* Define to 1 if you have the `setutent_r' function. */

/* #undef HAVE_SETUTENT_R */

/* Define to 1 if you have the <shadow.h> header file. */
#define HAVE_SHADOW_H 1

/* Define to 1 if you have the `shutdown' function. */
#define HAVE_SHUTDOWN 1

/* Define to 1 if you have the `sigaction' function. */
#define HAVE_SIGACTION 1

/* Define to 1 if 'sig_atomic_t' is a signed integer type. */

/* #undef HAVE_SIGNED_SIG_ATOMIC_T */

/* Define to 1 if 'wchar_t' is a signed integer type. */

/* #undef HAVE_SIGNED_WCHAR_T */

/* Define to 1 if 'wint_t' is a signed integer type. */

/* #undef HAVE_SIGNED_WINT_T */

/* Define to 1 if you have the `sigvec' function. */
#define HAVE_SIGVEC 1

/* Define to 1 if the system has the type `sig_t'. */
#define HAVE_SIG_T 1

/* Define to 1 if you have the `sleep' function. */
#define HAVE_SLEEP 1

/* Define to one if you have snprintf */
#define HAVE_SNPRINTF 1

/* Define to 1 if you have the <stdarg.h> header file. */
#define HAVE_STDARG_H 1

/* Define to 1 if stdbool.h conforms to C99. */
#define HAVE_STDBOOL_H 1

/* Define to 1 if you have the <stddef.h> header file. */
#define HAVE_STDDEF_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define if <stdint.h> exists, doesn't clash with <sys/types.h>, and declares
   uintmax_t. */
#define HAVE_STDINT_H_WITH_UINTMAX 1

/* Define to 1 if you have the <stdio_ext.h> header file. */
#define HAVE_STDIO_EXT_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strcasecmp' function. */
#define HAVE_STRCASECMP 1

/* Define to 1 if you have the `strchr' function. */
#define HAVE_STRCHR 1

/* Define to 1 if you have the `strchrnul' function. */
#define HAVE_STRCHRNUL 1

/* Define to 1 if you have the `strcoll' function and it is properly defined.
   */
#define HAVE_STRCOLL 1

/* Define to 1 if you have the `strdup' function. */
#define HAVE_STRDUP 1

/* FIXME */

/* #undef HAVE_STREAMSPTY */

/* Define to 1 if you have the `strerror_r' function. */
#define HAVE_STRERROR_R 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strncasecmp' function. */
#define HAVE_STRNCASECMP 1

/* Define to 1 if you have the `strndup' function. */
#define HAVE_STRNDUP 1

/* Define to 1 if you have the `strnlen' function. */
#define HAVE_STRNLEN 1

/* Define to 1 if you have the <stropts.h> header file. */

/* #undef HAVE_STROPTS_H */

/* Define if there is a member named d_type in the struct describing directory
   headers. */
#define HAVE_STRUCT_DIRENT_D_TYPE 1

/* Define to 1 if `h_addr_list' is member of `struct hostent'. */
#define HAVE_STRUCT_HOSTENT_H_ADDR_LIST 1

/* Define to 1 if `ifr_broadaddr' is member of `struct ifreq'. */
#define HAVE_STRUCT_IFREQ_IFR_BROADADDR 1

/* Define to 1 if `ifr_index' is member of `struct ifreq'. */

/* #undef HAVE_STRUCT_IFREQ_IFR_INDEX */

/* Define to 1 if `ifr_map' is member of `struct ifreq'. */
#define HAVE_STRUCT_IFREQ_IFR_MAP 1

/* Define to 1 if `ifr_netmask' is member of `struct ifreq'. */
#define HAVE_STRUCT_IFREQ_IFR_NETMASK 1

/* Define to 1 if the system has the type `struct if_nameindex'. */
#define HAVE_STRUCT_IF_NAMEINDEX 1

/* Define to 1 if the system has the type `struct lastlog'. */
#define HAVE_STRUCT_LASTLOG 1

/* Define to 1 if the system has the type `struct random_data'. */
#define HAVE_STRUCT_RANDOM_DATA 1

/* Define to 1 if `sin_len' is member of `struct sockaddr_in'. */

/* #undef HAVE_STRUCT_SOCKADDR_IN_SIN_LEN */

/* Define to 1 if `sa_len' is member of `struct sockaddr'. */

/* #undef HAVE_STRUCT_SOCKADDR_SA_LEN */

/* Define to 1 if the system has the type `struct sockaddr_storage'. */
#define HAVE_STRUCT_SOCKADDR_STORAGE 1

/* Define to 1 if `st_blksize' is member of `struct stat'. */

/* #undef HAVE_STRUCT_STAT_ST_BLKSIZE */

/* Define to 1 if `ut_exit' is member of `struct utmpx'. */
#define HAVE_STRUCT_UTMPX_UT_EXIT 1

/* Define to 1 if `ut_exit.e_exit' is member of `struct utmpx'. */
#define HAVE_STRUCT_UTMPX_UT_EXIT_E_EXIT 1

/* Define to 1 if `ut_exit.e_termination' is member of `struct utmpx'. */
#define HAVE_STRUCT_UTMPX_UT_EXIT_E_TERMINATION 1

/* Define to 1 if `ut_exit.ut_exit' is member of `struct utmpx'. */

/* #undef HAVE_STRUCT_UTMPX_UT_EXIT_UT_EXIT */

/* Define to 1 if `ut_exit.ut_termination' is member of `struct utmpx'. */

/* #undef HAVE_STRUCT_UTMPX_UT_EXIT_UT_TERMINATION */

/* Define to 1 if `ut_id' is member of `struct utmpx'. */
#define HAVE_STRUCT_UTMPX_UT_ID 1

/* Define to 1 if `ut_name' is member of `struct utmpx'. */
#define HAVE_STRUCT_UTMPX_UT_NAME 1

/* Define to 1 if `ut_pid' is member of `struct utmpx'. */
#define HAVE_STRUCT_UTMPX_UT_PID 1

/* Define to 1 if `ut_tv' is member of `struct utmpx'. */
#define HAVE_STRUCT_UTMPX_UT_TV 1

/* Define to 1 if `ut_type' is member of `struct utmpx'. */
#define HAVE_STRUCT_UTMPX_UT_TYPE 1

/* Define to 1 if `ut_user' is member of `struct utmpx'. */
#define HAVE_STRUCT_UTMPX_UT_USER 1

/* Define to 1 if `ut_exit' is member of `struct utmp'. */
#define HAVE_STRUCT_UTMP_UT_EXIT 1

/* Define to 1 if `ut_exit.e_exit' is member of `struct utmp'. */
#define HAVE_STRUCT_UTMP_UT_EXIT_E_EXIT 1

/* Define to 1 if `ut_exit.e_termination' is member of `struct utmp'. */
#define HAVE_STRUCT_UTMP_UT_EXIT_E_TERMINATION 1

/* Define to 1 if `ut_exit.ut_exit' is member of `struct utmp'. */

/* #undef HAVE_STRUCT_UTMP_UT_EXIT_UT_EXIT */

/* Define to 1 if `ut_exit.ut_termination' is member of `struct utmp'. */

/* #undef HAVE_STRUCT_UTMP_UT_EXIT_UT_TERMINATION */

/* Define to 1 if `ut_host' is member of `struct utmp'. */
#define HAVE_STRUCT_UTMP_UT_HOST 1

/* Define to 1 if `ut_id' is member of `struct utmp'. */
#define HAVE_STRUCT_UTMP_UT_ID 1

/* Define to 1 if `ut_name' is member of `struct utmp'. */
#define HAVE_STRUCT_UTMP_UT_NAME 1

/* Define to 1 if `ut_pid' is member of `struct utmp'. */
#define HAVE_STRUCT_UTMP_UT_PID 1

/* Define to 1 if `ut_tv' is member of `struct utmp'. */
#define HAVE_STRUCT_UTMP_UT_TV 1

/* Define to 1 if `ut_type' is member of `struct utmp'. */
#define HAVE_STRUCT_UTMP_UT_TYPE 1

/* Define to 1 if `ut_user' is member of `struct utmp'. */
#define HAVE_STRUCT_UTMP_UT_USER 1

/* Define if struct stat has an st_dm_mode member. */

/* #undef HAVE_ST_DM_MODE */

/* Define to 1 if you have the <sysexits.h> header file. */
#define HAVE_SYSEXITS_H 1

/* FIXME */
#define HAVE_SYSLOG_INTERNAL 1

/* Define to 1 if you have the <sys/bitypes.h> header file. */

/* #undef HAVE_SYS_BITYPES_H */

/* Define to 1 if you have the <sys/cdefs.h> header file. */
#define HAVE_SYS_CDEFS_H 1

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */

/* #undef HAVE_SYS_DIR_H */

/* Define to one if you have sys_errlist. */

/* #undef HAVE_SYS_ERRLIST */

/* Define to 1 if you have the <sys/file.h> header file. */
#define HAVE_SYS_FILE_H 1

/* Define to 1 if you have the <sys/filio.h> header file. */

/* #undef HAVE_SYS_FILIO_H */

/* Define to 1 if you have the <sys/inttypes.h> header file. */

/* #undef HAVE_SYS_INTTYPES_H */

/* Define to 1 if you have the <sys/ioctl_compat.h> header file. */

/* #undef HAVE_SYS_IOCTL_COMPAT_H */

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/mkdev.h> header file. */

/* #undef HAVE_SYS_MKDEV_H */

/* Define to 1 if you have the <sys/mman.h> header file. */
#define HAVE_SYS_MMAN_H 1

/* Define to 1 if you have the <sys/msgbuf.h> header file. */

/* #undef HAVE_SYS_MSGBUF_H */

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */

/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/proc.h> header file. */

/* #undef HAVE_SYS_PROC_H */

/* Define to 1 if you have the <sys/ptyvar.h> header file. */

/* #undef HAVE_SYS_PTYVAR_H */

/* Define to 1 if you have the <sys/resource.h> header file. */
#define HAVE_SYS_RESOURCE_H 1

/* Define to 1 if you have the <sys/select.h> header file. */
#define HAVE_SYS_SELECT_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/sockio.h> header file. */

/* #undef HAVE_SYS_SOCKIO_H */

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/stream.h> header file. */

/* #undef HAVE_SYS_STREAM_H */

/* Define to 1 if you have the <sys/sysmacros.h> header file. */
#define HAVE_SYS_SYSMACROS_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/tty.h> header file. */

/* #undef HAVE_SYS_TTY_H */

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/utsname.h> header file. */
#define HAVE_SYS_UTSNAME_H 1

/* Define to 1 if you have the <sys/wait.h> header file. */
#define HAVE_SYS_WAIT_H 1

/* Define to 1 if you have the `tcgetattr' function. */
#define HAVE_TCGETATTR 1

/* Define to 1 if you have the <tcpd.h> header file. */

/* #undef HAVE_TCPD_H */

/* Define to 1 if you have the `tcsetattr' function. */
#define HAVE_TCSETATTR 1

/* Define to 1 if you have the <termios.h> header file. */
#define HAVE_TERMIOS_H 1

/* Define to 1 if you have the <termio.h> header file. */
#define HAVE_TERMIO_H 1

/* Define to 1 if you have the <time.h> header file. */
#define HAVE_TIME_H 1

/* Define to 1 if you have the `tsearch' function. */
#define HAVE_TSEARCH 1

/* Define to 1 if you have the `tzset' function. */
#define HAVE_TZSET 1

/* Define to 1 if you have the `uname' function. */
#define HAVE_UNAME 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `unlinkat' function. */
#define HAVE_UNLINKAT 1

/* Define to 1 if the system has the type `unsigned long long int'. */
#define HAVE_UNSIGNED_LONG_LONG_INT 1

/* Define to 1 if you have the `updwtmp' function. */
#define HAVE_UPDWTMP 1

/* Define to 1 if you have the `updwtmpx' function. */
#define HAVE_UPDWTMPX 1

/* Define to 1 if you have the `utime' function. */
#define HAVE_UTIME 1

/* Define to 1 if you have the `utimes' function. */
#define HAVE_UTIMES 1

/* Define to 1 if you have the `utmpname' function. */
#define HAVE_UTMPNAME 1

/* Define to 1 if you have the `utmpxname' function. */
#define HAVE_UTMPXNAME 1

/* Define to 1 if you have the <utmpx.h> header file. */
#define HAVE_UTMPX_H 1

/* Define to 1 if you have the <utmp.h> header file. */
#define HAVE_UTMP_H 1

/* Define to 1 if you have the `vasnprintf' function. */

/* #undef HAVE_VASNPRINTF */

/* Define to 1 if you have the `vfork' function. */
#define HAVE_VFORK 1

/* Define to 1 if you have the <vfork.h> header file. */

/* #undef HAVE_VFORK_H */

/* Define to 1 if you have the `vhangup' function. */
#define HAVE_VHANGUP 1

/* Define to 1 if you have the <vis.h> header file. */

/* #undef HAVE_VIS_H */

/* Define to one if you have vsnprintf */
#define HAVE_VSNPRINTF 1

/* Define to 1 if you have the `wait3' function. */
#define HAVE_WAIT3 1

/* Define to 1 if you have the `wait4' function. */
#define HAVE_WAIT4 1

/* Define to 1 if you have the <wchar.h> header file. */
#define HAVE_WCHAR_H 1

/* Define if you have the 'wchar_t' type. */
#define HAVE_WCHAR_T 1

/* Define to 1 if you have the `wcrtomb' function. */
#define HAVE_WCRTOMB 1

/* Define to 1 if you have the `wcscoll' function. */
#define HAVE_WCSCOLL 1

/* Define to 1 if you have the `wcslen' function. */
#define HAVE_WCSLEN 1

/* Define to 1 if you have the `wcsnlen' function. */
#define HAVE_WCSNLEN 1

/* Define to 1 if you have the <wctype.h> header file. */
#define HAVE_WCTYPE_H 1

/* 1 */
#define HAVE_WEAK_REFS 1

/* Define to 1 if you have the <winsock2.h> header file. */

/* #undef HAVE_WINSOCK2_H */

/* Define if you have the 'wint_t' type. */
#define HAVE_WINT_T 1

/* Define to 1 if you have the `wmemchr' function. */
#define HAVE_WMEMCHR 1

/* Define to 1 if you have the `wmemcpy' function. */
#define HAVE_WMEMCPY 1

/* Define to 1 if you have the `wmempcpy' function. */
#define HAVE_WMEMPCPY 1

/* Define to 1 if `fork' works. */
#define HAVE_WORKING_FORK 1

/* Define to 1 if O_NOATIME works. */
#define HAVE_WORKING_O_NOATIME 1

/* Define to 1 if O_NOFOLLOW works. */
#define HAVE_WORKING_O_NOFOLLOW 1

/* Define to 1 if `vfork' works. */
#define HAVE_WORKING_VFORK 1

/* Define to 1 if you have the <ws2tcpip.h> header file. */

/* #undef HAVE_WS2TCPIP_H */

/* Define to 1 if the system has the type `_Bool'. */
#define HAVE__BOOL 1

/* Define to 1 if you have the `__fsetlocking' function. */
#define HAVE___FSETLOCKING 1

/* Define HOST_NAME_MAX when <limits.h> does not define it. */

/* #undef HOST_NAME_MAX */

/* Define to ICMP6_DST_UNREACH_NOTNEIGHBOR if not available */

/* #undef ICMP6_DST_UNREACH_BEYONDSCOPE */

/* Define to one if you want IPv6. */
#define IPV6 1

/* Define to IPV6_HOPLIMIT if not available */

/* #undef IPV6_RECVHOPLIMIT */

#if FILE_SYSTEM_BACKSLASH_IS_FILE_NAME_SEPARATOR
# define ISSLASH(C) ((C) == '/' || (C) == '\\')
#else
# define ISSLASH(C) ((C) == '/')
#endif

/* Define to one if you have Kerberos */

/* #undef KERBEROS */

/* Define to one if you have Kerberos IV */

/* #undef KRB4 */

/* Define to one if you have Kerberos V */

/* #undef KRB5 */

/* Define to double if compiler does not support */

/* #undef LONG_DOUBLE */

/* Define to long if compiler does not support */

/* #undef LONG_LONG */

/* Define to 1 if lseek does not detect pipes. */

/* #undef LSEEK_PIPE_BROKEN */

/* Define to 1 if `lstat' dereferences a symlink specified with a trailing
   slash. */
#define LSTAT_FOLLOWS_SLASHED_SYMLINK 1

/* If malloc(0) is != NULL, define this to 1. Otherwise define this to 0. */
#define MALLOC_0_IS_NONNULL 1

/* Define to a substitute value for mmap()'s MAP_ANONYMOUS flag. */

/* #undef MAP_ANONYMOUS */

/* Define if the mbrtowc function has the NULL string argument bug. */

/* #undef MBRTOWC_NULL_ARG_BUG */

/* Define if the mbrtowc function does not return 0 for a NUL character. */

/* #undef MBRTOWC_NUL_RETVAL_BUG */

/* Define if the mbrtowc function returns a wrong return value. */

/* #undef MBRTOWC_RETVAL_BUG */

/* Define to 1 if open() fails to recognize a trailing slash. */

/* #undef OPEN_TRAILING_SLASH_BUG */

/* Name of package */
#define PACKAGE "inetutils"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "bug-inetutils@gnu.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "GNU inetutils"

/* String identifying the packager of this software */

/* #undef PACKAGE_PACKAGER */

/* Packager info for bug reports (URL/e-mail/...) */

/* #undef PACKAGE_PACKAGER_BUG_REPORTS */

/* Packager-specific version information */

/* #undef PACKAGE_PACKAGER_VERSION */

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "GNU inetutils 1.7"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "inetutils"

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.7"

/* Define to the type that is the result of default argument promotions of
   type mode_t. */
#define PROMOTED_MODE_T mode_t

/* Define to 1 if the C compiler supports function prototypes. */
#define PROTOTYPES 1

/* Define to l, ll, u, ul, ull, etc., as suitable for constants of type
   'ptrdiff_t'. */

/* #undef PTRDIFF_T_SUFFIX */

/* Define to 1 if gnulib's fchdir() replacement is used. */

/* #undef REPLACE_FCHDIR */

/* Define to 1 if stat needs help when passed a directory name with a trailing
   slash */

/* #undef REPLACE_FUNC_STAT_DIR */

/* Define to 1 if stat needs help when passed a file name with a trailing
   slash */

/* #undef REPLACE_FUNC_STAT_FILE */

/* Define to 1 if open() should work around the inability to open a directory.
   */

/* #undef REPLACE_OPEN_DIRECTORY */

/* Define this to 1 if strerror is broken. */

/* #undef REPLACE_STRERROR */

/* Define if vasnprintf exists but is overridden by gnulib. */

/* #undef REPLACE_VASNPRINTF */

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

/* FIXME */

/* #undef SEEK_CUR */

/* FIXME */

/* #undef SEEK_END */

/* FIXME */

/* #undef SEEK_SET */

/* Define to 1 if you have Shishi */

/* #undef SHISHI */

/* Define to l, ll, u, ul, ull, etc., as suitable for constants of type
   'sig_atomic_t'. */

/* #undef SIG_ATOMIC_T_SUFFIX */

/* Define as the maximum value of type 'size_t', if the system doesn't define
   it. */
#ifndef SIZE_MAX

/* # undef SIZE_MAX */
#endif

/* Define to l, ll, u, ul, ull, etc., as suitable for constants of type
   'size_t'. */

/* #undef SIZE_T_SUFFIX */

/* FIXME */

/* #undef SOLARIS */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at runtime.
    STACK_DIRECTION > 0 => grows toward higher addresses
    STACK_DIRECTION < 0 => grows toward lower addresses
    STACK_DIRECTION = 0 => direction of growth unknown */

/* #undef STACK_DIRECTION */

/* Define to 1 if the `S_IS*' macros in <sys/stat.h> do not work properly. */

/* #undef STAT_MACROS_BROKEN */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* FIXME */

/* #undef STDERR_FILENO */

/* FIXME */

/* #undef STDIN_FILENO */

/* FIXME */

/* #undef STDOUT_FILENO */

/* Define to 1 if strerror_r returns char *. */
#define STRERROR_R_CHAR_P 1

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Define to 1 if you want getc etc. to use unlocked I/O if available.
   Unlocked I/O can improve performance in unithreaded apps, but it is not
   safe for multithreaded apps. */
#define USE_UNLOCKED_IO 1

/* FIXME */

/* #undef UTMPX */

/* Version number of package */
#define VERSION "1.7"

/* Define to l, ll, u, ul, ull, etc., as suitable for constants of type
   'wchar_t'. */

/* #undef WCHAR_T_SUFFIX */

/* Define if WSAStartup is needed. */

/* #undef WINDOWS_SOCKETS */

/* Define to l, ll, u, ul, ull, etc., as suitable for constants of type
   'wint_t'. */

/* #undef WINT_T_SUFFIX */

/* Define to one if you have -lls */
#define WITH_LIBLS 1

/* Define to one if you have -lpam. */

/* #undef WITH_PAM */

/* Define to one if you have -lwrap. */

/* #undef WITH_WRAP */

/* Define to 1 to make fseeko visible on some hosts (e.g. glibc 2.2). */

/* #undef _LARGEFILE_SOURCE */

/* Define to 1 if on MINIX. */

/* #undef _MINIX */

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */

/* #undef _POSIX_1_SOURCE */

/* Define to 1 if you need to in order for `stat' and other things to work. */

/* #undef _POSIX_SOURCE */

/* Define if you want regoff_t to be at least as wide POSIX requires. */
#define _REGEX_LARGE_OFFSETS 1

/* Define to 500 only on HP-UX. */

/* #undef _XOPEN_SOURCE */

/* Enable extensions on AIX 3, Interix.  */
#ifndef _ALL_SOURCE
# define _ALL_SOURCE 1
#endif

/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif

/* Enable threading extensions on Solaris.  */
#ifndef _POSIX_PTHREAD_SEMANTICS
# define _POSIX_PTHREAD_SEMANTICS 1
#endif

/* Enable extensions on HP NonStop.  */
#ifndef _TANDEM_SOURCE
# define _TANDEM_SOURCE 1
#endif

/* Enable general extensions on Solaris.  */
#ifndef __EXTENSIONS__
# define __EXTENSIONS__ 1
#endif


/* Define to rpl_ if the getopt replacement functions and variables should be
   used. */
#define __GETOPT_PREFIX rpl_

/* Define like PROTOTYPES; this can be used by system headers. */
#define __PROTOTYPES 1

/* Define to empty if `const' does not conform to ANSI C. */

/* #undef const */

/* Define to a replacement function name for fnmatch(). */

/* #undef fnmatch */

/* Define to `int' if <sys/types.h> doesn't define. */

/* #undef gid_t */

/* A replacement for va_copy, if needed.  */
#define gl_va_copy(a,b) ((a) = (b))

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus

/* #undef inline */
#endif

/* Define to long or long long if <stdint.h> and <inttypes.h> don't define. */

/* #undef intmax_t */

/* Work around a bug in Apple GCC 4.0.1 build 5465: In C99 mode, it supports
   the ISO C 99 semantics of 'extern inline' (unlike the GNU C semantics of
   earlier versions), but does not display it by setting __GNUC_STDC_INLINE__.
   __APPLE__ && __MACH__ test for MacOS X.
   __APPLE_CC__ tests for the Apple compiler and its version.
   __STDC_VERSION__ tests for the C99 mode.  */
#if defined __APPLE__ && defined __MACH__ && __APPLE_CC__ >= 5465 && !defined __cplusplus && __STDC_VERSION__ >= 199901L && !defined __GNUC_STDC_INLINE__
# define __GNUC_STDC_INLINE__ 1
#endif

/* Define to rpl_malloc if the replacement function should be used. */

/* #undef malloc */

/* Define to a type if <wchar.h> does not define. */

/* #undef mbstate_t */

/* Define to `int' if <sys/types.h> does not define. */

/* #undef mode_t */

/* Define to the type of st_nlink in struct stat, or a supertype. */

/* #undef nlink_t */

/* Define to `long int' if <sys/types.h> does not define. */

/* #undef off_t */

/* Define to `int' if <sys/types.h> does not define. */

/* #undef pid_t */

/* Define to poll if the replacement function should be used. */

/* #undef poll */

/* Define as the type of the result of subtracting two pointers, if the system
   doesn't define it. */

/* #undef ptrdiff_t */

/* Define to rpl_re_comp if the replacement should be used. */
#define re_comp rpl_re_comp

/* Define to rpl_re_compile_fastmap if the replacement should be used. */
#define re_compile_fastmap rpl_re_compile_fastmap

/* Define to rpl_re_compile_pattern if the replacement should be used. */
#define re_compile_pattern rpl_re_compile_pattern

/* Define to rpl_re_exec if the replacement should be used. */
#define re_exec rpl_re_exec

/* Define to rpl_re_match if the replacement should be used. */
#define re_match rpl_re_match

/* Define to rpl_re_match_2 if the replacement should be used. */
#define re_match_2 rpl_re_match_2

/* Define to rpl_re_search if the replacement should be used. */
#define re_search rpl_re_search

/* Define to rpl_re_search_2 if the replacement should be used. */
#define re_search_2 rpl_re_search_2

/* Define to rpl_re_set_registers if the replacement should be used. */
#define re_set_registers rpl_re_set_registers

/* Define to rpl_re_set_syntax if the replacement should be used. */
#define re_set_syntax rpl_re_set_syntax

/* Define to rpl_re_syntax_options if the replacement should be used. */
#define re_syntax_options rpl_re_syntax_options

/* Define to rpl_realloc if the replacement function should be used. */

/* #undef realloc */

/* Define to rpl_regcomp if the replacement should be used. */
#define regcomp rpl_regcomp

/* Define to rpl_regerror if the replacement should be used. */
#define regerror rpl_regerror

/* Define to rpl_regexec if the replacement should be used. */
#define regexec rpl_regexec

/* Define to rpl_regfree if the replacement should be used. */
#define regfree rpl_regfree

/* Define to the equivalent of the C99 'restrict' keyword, or to
   nothing if this is not supported.  Do not define if restrict is
   supported directly.  */
#define restrict __restrict

/* Work around a bug in Sun C++: it does not support _Restrict, even
   though the corresponding Sun C compiler does, which causes
   "#define restrict _Restrict" in the previous line.  Perhaps some future
   version of Sun C++ will work with _Restrict; if so, it'll probably
   define __RESTRICT, just as Sun C does.  */
#if defined __SUNPRO_CC && !defined __RESTRICT
# define _Restrict
#endif

/* FIXME */

/* #undef sa_family_t */

/* Define to `setpgrp' if you don't have `setpgid'. */

/* #undef setpgid */

/* FIXME */

/* #undef sig_atomic_t */

/* FIXME */

/* #undef sigset_t */

/* Define to `unsigned int' if <sys/types.h> does not define. */

/* #undef size_t */

/* Define to int if <sys/types.h> does not define */

/* #undef socklen_t */

/* Define as a signed type of the same size as size_t. */

/* #undef ssize_t */

/* Define to rpl_strnlen if the replacement function should be used. */

/* #undef strnlen */

/* Define to `int' if <sys/types.h> doesn't define. */

/* #undef uid_t */

/* Define as a marker that can be attached to function parameter declarations
   for parameters that are not used.  This helps to reduce warnings, such as
   from GCC -Wunused-parameter.  */
#if __GNUC__ >= 3 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)
# define _UNUSED_PARAMETER_ __attribute__ ((__unused__))
#else
# define _UNUSED_PARAMETER_
#endif


/* Define as a macro for copying va_list variables. */

/* #undef va_copy */

/* Define as `fork' if `vfork' does not work. */

/* #undef vfork */

/* Defaults for PATH_ variables.  */
#include <confpaths.h>


#if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 7)
#  define __attribute__(x)
#endif

#ifndef ARG_UNUSED
# define ARG_UNUSED __attribute__ ((__unused__))
#endif
