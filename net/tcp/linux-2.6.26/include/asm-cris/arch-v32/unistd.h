#ifndef _ASM_CRIS_ARCH_UNISTD_H_
#define _ASM_CRIS_ARCH_UNISTD_H_

/* XXX - _foo needs to be __foo, while __NR_bar could be _NR_bar. */
/*
 * Don't remove the .ifnc tests; they are an insurance against
 * any hard-to-spot gcc register allocation bugs.
 */
#define _syscall0(type,name) \
type name(void) \
{ \
  register long __a __asm__ ("r10"); \
  register long __n_ __asm__ ("r9") = (__NR_##name); \
  __asm__ __volatile__ (".ifnc %0%1,$r10$r9\n\t" \
            ".err\n\t" \
            ".endif\n\t" \
            "break 13" \
            : "=r" (__a) \
            : "r" (__n_) \
            : "memory"); \
  if (__a >= 0) \
     return (type) __a; \
  errno = -__a; \
  return (type) -1; \
}

#define _syscall1(type,name,type1,arg1) \
type name(type1 arg1) \
{ \
  register long __a __asm__ ("r10") = (long) arg1; \
  register long __n_ __asm__ ("r9") = (__NR_##name); \
  __asm__ __volatile__ (".ifnc %0%1,$r10$r9\n\t" \
            ".err\n\t" \
            ".endif\n\t" \
            "break 13" \
            : "=r" (__a) \
            : "r" (__n_), "0" (__a) \
            : "memory"); \
  if (__a >= 0) \
     return (type) __a; \
  errno = -__a; \
  return (type) -1; \
}

#define _syscall2(type,name,type1,arg1,type2,arg2) \
type name(type1 arg1,type2 arg2) \
{ \
  register long __a __asm__ ("r10") = (long) arg1; \
  register long __b __asm__ ("r11") = (long) arg2; \
  register long __n_ __asm__ ("r9") = (__NR_##name); \
  __asm__ __volatile__ (".ifnc %0%1%3,$r10$r9$r11\n\t" \
            ".err\n\t" \
            ".endif\n\t" \
            "break 13" \
            : "=r" (__a) \
            : "r" (__n_), "0" (__a), "r" (__b) \
            : "memory"); \
  if (__a >= 0) \
     return (type) __a; \
  errno = -__a; \
  return (type) -1; \
}

#define _syscall3(type,name,type1,arg1,type2,arg2,type3,arg3) \
type name(type1 arg1,type2 arg2,type3 arg3) \
{ \
  register long __a __asm__ ("r10") = (long) arg1; \
  register long __b __asm__ ("r11") = (long) arg2; \
  register long __c __asm__ ("r12") = (long) arg3; \
  register long __n_ __asm__ ("r9") = (__NR_##name); \
  __asm__ __volatile__ (".ifnc %0%1%3%4,$r10$r9$r11$r12\n\t" \
            ".err\n\t" \
            ".endif\n\t" \
            "break 13" \
            : "=r" (__a) \
            : "r" (__n_), "0" (__a), "r" (__b), "r" (__c) \
            : "memory"); \
  if (__a >= 0) \
     return (type) __a; \
  errno = -__a; \
  return (type) -1; \
}

#define _syscall4(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
type name (type1 arg1, type2 arg2, type3 arg3, type4 arg4) \
{ \
  register long __a __asm__ ("r10") = (long) arg1; \
  register long __b __asm__ ("r11") = (long) arg2; \
  register long __c __asm__ ("r12") = (long) arg3; \
  register long __d __asm__ ("r13") = (long) arg4; \
  register long __n_ __asm__ ("r9") = (__NR_##name); \
  __asm__ __volatile__ (".ifnc %0%1%3%4%5,$r10$r9$r11$r12$r13\n\t" \
            ".err\n\t" \
            ".endif\n\t" \
            "break 13" \
            : "=r" (__a) \
            : "r" (__n_), "0" (__a), "r" (__b), \
              "r" (__c), "r" (__d)\
            : "memory"); \
  if (__a >= 0) \
     return (type) __a; \
  errno = -__a; \
  return (type) -1; \
}

#define _syscall5(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4, \
      type5,arg5) \
type name (type1 arg1,type2 arg2,type3 arg3,type4 arg4,type5 arg5) \
{ \
  register long __a __asm__ ("r10") = (long) arg1; \
  register long __b __asm__ ("r11") = (long) arg2; \
  register long __c __asm__ ("r12") = (long) arg3; \
  register long __d __asm__ ("r13") = (long) arg4; \
  register long __e __asm__ ("mof") = (long) arg5; \
  register long __n_ __asm__ ("r9") = (__NR_##name); \
  __asm__ __volatile__ (".ifnc %0%1%3%4%5%6,$r10$r9$r11$r12$r13$mof\n\t" \
            ".err\n\t" \
            ".endif\n\t" \
            "break 13" \
            : "=r" (__a) \
            : "r" (__n_), "0" (__a), "r" (__b), \
              "r" (__c), "r" (__d), "h" (__e) \
            : "memory"); \
  if (__a >= 0) \
     return (type) __a; \
  errno = -__a; \
  return (type) -1; \
}

#define _syscall6(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4, \
      type5,arg5,type6,arg6) \
type name (type1 arg1,type2 arg2,type3 arg3,type4 arg4,type5 arg5,type6 arg6) \
{ \
  register long __a __asm__ ("r10") = (long) arg1; \
  register long __b __asm__ ("r11") = (long) arg2; \
  register long __c __asm__ ("r12") = (long) arg3; \
  register long __d __asm__ ("r13") = (long) arg4; \
  register long __e __asm__ ("mof") = (long) arg5; \
  register long __f __asm__ ("srp") = (long) arg6; \
  register long __n_ __asm__ ("r9") = (__NR_##name); \
  __asm__ __volatile__ (".ifnc %0%1%3%4%5%6%7,$r10$r9$r11$r12$r13$mof$srp\n\t" \
            ".err\n\t" \
            ".endif\n\t" \
            "break 13" \
            : "=r" (__a) \
            : "r" (__n_), "0" (__a), "r" (__b), \
              "r" (__c), "r" (__d), "h" (__e), "x" (__f) \
            : "memory"); \
  if (__a >= 0) \
     return (type) __a; \
  errno = -__a; \
  return (type) -1; \
}

#endif
