/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * linuxapi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * linuxapi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linuxapi. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __err_utils_h
#define __err_utils_h

/*
 * This is a collection of helpers to help you deal with the Linux
 * error API.
 */

/* THIS IS A C FILE, NO C++ here */

#include <firstinclude.h>
#include <us_helper.h>	// for ARRAY_SIZEOF(), myunlikely()
#include <stdio.h>	// for printf(3)
#include <string.h>	// for strcmp(3), strerror(3)
#include <stdlib.h>	// for exit(3), EXIT_FAILURE:const
#include <err.h>// for err(3)
#include <error.h>	// for error_at_line(3)

/*
 * List of all errors (can be found in 'man errno')
 */

typedef struct _err_val_and_name {
	int val;
	const char* name;
} err_val_and_name;

/*
 * List of all errors and their names. The reason for this is that the standard
 * C library does not provide a translation to/from error name to error value.
 * (it does provide some description of error values via strerror(3)).
 * Note: you cant use stringification macros here because they will substitute
 * the macro. Use only macros that don't substitute.
 */
#define entry(x) { x, # x }
static err_val_and_name err_tbl[]={
	entry(E2BIG),
	entry(EACCES),
	entry(EADDRINUSE),
	entry(EADDRNOTAVAIL),
	entry(EAFNOSUPPORT),
	entry(EAGAIN),
	entry(EALREADY),
	entry(EBADE),
	entry(EBADF),
	entry(EBADFD),
	entry(EBADMSG),
	entry(EBADR),
	entry(EBADRQC),
	entry(EBADSLT),
	entry(EBUSY),
	entry(ECANCELED),
	entry(ECHILD),
	entry(ECHRNG),
	entry(ECOMM),
	entry(ECONNABORTED),
	entry(ECONNREFUSED),
	entry(ECONNRESET),
	entry(EDEADLK),
	entry(EDEADLOCK),
	entry(EDESTADDRREQ),
	entry(EDOM),
	entry(EDQUOT),
	entry(EEXIST),
	entry(EFAULT),
	entry(EFBIG),
	entry(EHOSTDOWN),
	entry(EHOSTUNREACH),
	entry(EIDRM),
	entry(EILSEQ),
	entry(EINPROGRESS),
	entry(EINTR),
	entry(EINVAL),
	entry(EIO),
	entry(EISCONN),
	entry(EISDIR),
	entry(EISNAM),
	entry(EKEYEXPIRED),
	entry(EKEYREJECTED),
	entry(EKEYREVOKED),
	entry(EL2HLT),
	entry(EL2NSYNC),
	entry(EL3HLT),
	entry(EL3RST),
	entry(ELIBACC),
	entry(ELIBBAD),
	entry(ELIBMAX),
	entry(ELIBSCN),
	entry(ELIBEXEC),
	entry(ELOOP),
	entry(EMEDIUMTYPE),
	entry(EMFILE),
	entry(EMLINK),
	entry(EMSGSIZE),
	entry(EMULTIHOP),
	entry(ENAMETOOLONG),
	entry(ENETDOWN),
	entry(ENETRESET),
	entry(ENETUNREACH),
	entry(ENFILE),
	entry(ENOBUFS),
	entry(ENODATA),
	entry(ENODEV),
	entry(ENOENT),
	entry(ENOEXEC),
	entry(ENOKEY),
	entry(ENOLCK),
	entry(ENOLINK),
	entry(ENOMEDIUM),
	entry(ENOMEM),
	entry(ENOMSG),
	entry(ENONET),
	entry(ENOPKG),
	entry(ENOPROTOOPT),
	entry(ENOSPC),
	entry(ENOSR),
	entry(ENOSTR),
	entry(ENOSYS),
	entry(ENOTBLK),
	entry(ENOTCONN),
	entry(ENOTDIR),
	entry(ENOTEMPTY),
	entry(ENOTSOCK),
	entry(ENOTSUP),
	entry(ENOTTY),
	entry(ENOTUNIQ),
	entry(ENXIO),
	entry(EOPNOTSUPP),
	entry(EOVERFLOW),
	entry(EPERM),
	entry(EPFNOSUPPORT),
	entry(EPIPE),
	entry(EPROTO),
	entry(EPROTONOSUPPORT),
	entry(EPROTOTYPE),
	entry(ERANGE),
	entry(EREMCHG),
	entry(EREMOTE),
	entry(EREMOTEIO),
	entry(ERESTART),
	entry(EROFS),
	entry(ESHUTDOWN),
	entry(ESPIPE),
	entry(ESOCKTNOSUPPORT),
	entry(ESRCH),
	entry(ESTALE),
	entry(ESTRPIPE),
	entry(ETIME),
	entry(ETIMEDOUT),
	entry(ETXTBSY),
	entry(EUCLEAN),
	entry(EUNATCH),
	entry(EUSERS),
	entry(EWOULDBLOCK),
	entry(EXDEV),
	entry(EXFULL),
};
#undef entry

/*
 * Translate error name to error value
 */
static inline int error_get_by_name(const char* name) {
	unsigned int i;
	for(i=0; i<ARRAY_SIZEOF(err_tbl); i++) {
		if(strcmp(name, err_tbl[i].name)==0) {
			return err_tbl[i].val;
		}
	}
	err(-1, "bad error name %s", name);
	return -1;
}

/*
 * Translate error values to error names
 */
static inline const char* error_get_by_val(int val) {
	unsigned int i;
	for(i=0; i<ARRAY_SIZEOF(err_tbl); i++) {
		if(err_tbl[i].val==val) {
			return err_tbl[i].name;
		}
	}
	err(-1, "bad error value %d", val);
	return NULL;
}

/*
 * Print out a table of all error values, names and descriptions
 */
static inline void print_error_table() {
	unsigned int i;
	printf("number of error values is %zd\n", ARRAY_SIZEOF(err_tbl));
	for(i=0; i<ARRAY_SIZEOF(err_tbl); i++) {
		int val=err_tbl[i].val;
		const char* name=err_tbl[i].name;
		printf("i=%d, sig=%d, in_code=%s, strerror(%d)=%s\n", i, val, name, val, strerror(val));
	}
}

/*
 * A error handler, will take care of all those pesky error values
 * This is not a C++ framework so I do not throw an exception here.
 */
static inline void handle_error(int printBadVal, int badVal, int replace_errno, int new_errno, int useerrno, int errnotouse, const char* msg, const char* file, const char* function, const int line, const char* m) __attribute__((noreturn));
static inline void handle_error(int printBadVal, int badVal, int replace_errno, int new_errno, int useerrno, int errnotouse, const char* msg, const char* file, const char* function, const int line, const char* m) {
	// this is for pthread type errors
	if(replace_errno) {
		errno=new_errno;
	}
	// error_at_line(errno, errno, file, line, "ERROR\nfunction is [%s]\ntext that caused the error was [%s]\nerrno numeric is %d\nerrno macro is [%s]\n", function, msg, errno, error_get_by_val(errno));
	// error_at_line(errno, errno, file, line, "function is %s, msg is %s", function, msg);
	fprintf(stderr, "============ ERROR ============\n");
	fprintf(stderr, "file is [%s:%d]\n", file, line);
	fprintf(stderr, "function is [%s]\n", function);
	fprintf(stderr, "text that caused the error was [%s]\n", msg);
	if (m!=NULL) {
		fprintf(stderr, "message is [%s]\n", m);
	}
	if (printBadVal) {
		fprintf(stderr, "bad value is [%d]\n", badVal);
	}
	if(useerrno) {
		fprintf(stderr, "errno numeric is [%d]\n", errnotouse);
		fprintf(stderr, "errno symbolic is [%s]\n", error_get_by_val(errnotouse));
		fprintf(stderr, "errno string is [%s]\n", strerror(errnotouse));
	}
	if(useerrno) {
		exit(errnotouse);
	} else {
		exit(EXIT_FAILURE);
	}
	// old code follows
	// int save_errno=errno;
	// err(EXIT_FAILURE,"code is %s",msg);
	// fprintf(stderr,"command is %s\n",msg);
	// fprintf(stderr,"location is %s, %s, %d\n",base_file,file,line);
	// if(save_errno!=0) {
	// perror("error in system call");
	// } else {
	// fprintf(stderr,"error: %s\n",strerror(val));
	// }
	// exit(EXIT_FAILURE);
}
static inline int check_error(int val, const char* msg, const char* file, const char* function, const int line) __attribute__((noreturn));
static inline int check_error(int val, const char* msg, const char* file, const char* function, const int line) {
	handle_error(0, 0, 0, 0, 1, errno, msg, file, function, line, NULL);
}
static inline int check_zero(int val, const char* msg, const char* file, const char* function, const int line, const char* m) {
	if(myunlikely(val!=0)) {
		handle_error(0, 0, 0, 0, 1, errno, msg, file, function, line, m);
	}
	return val;
}
static inline int check_zero_errno(int val, const char* msg, const char* file, const char* function, const int line) {
	if(myunlikely(val!=0)) {
		handle_error(0, 0, 1, val, 0, val, msg, file, function, line, NULL);
	}
	return val;
}
static inline int check_not_zero(int val, const char* msg, const char* file, const char* function, const int line) {
	if(myunlikely(val==0)) {
		handle_error(0, 0, 0, 0, 1, errno, msg, file, function, line, NULL);
	}
	return val;
}
static inline int check_not_m1(int val, const char* msg, const char* file, const char* function, const int line) {
	if(myunlikely(val==-1)) {
		handle_error(0, 0, 0, 0, 1, errno, msg, file, function, line, NULL);
	}
	return val;
}
static inline int check_not_eof(int val, const char* msg, const char* file, const char* function, const int line) {
	if(myunlikely(val==EOF)) {
		handle_error(0, 0, 0, 0, 1, errno, msg, file, function, line, NULL);
	}
	return val;
}
static inline int check_1(int val, const char* msg, const char* file, const char* function, const int line) {
	if(myunlikely(val!=1)) {
		handle_error(0, 0, 0, 0, 1, errno, msg, file, function, line, NULL);
	}
	return val;
}
static inline int check_not_negative(int val, const char* msg, const char* file, const char* function, const int line) {
	if(myunlikely(val<0)) {
		handle_error(0, 0, 0, 0, 1, errno, msg, file, function, line, NULL);
	}
	return val;
}
static inline void* check_not_null(void* val, const char* msg, const char* file, const char* function, const int line) {
	if(myunlikely(val==NULL)) {
		handle_error(0, 0, 0, 0, 0, 0, msg, file, function, line, NULL);
	}
	return val;
}
static inline const void* check_not_null_const(const void* val, const char* msg, const char* file, const char* function, const int line) {
	if(myunlikely(val==NULL)) {
		handle_error(0, 0, 0, 0, 0, 0, msg, file, function, line, NULL);
	}
	return val;
}
static inline FILE* check_not_null_filep(FILE* val, const char* msg, const char* file, const char* function, const int line) {
	if(myunlikely(val==NULL)) {
		handle_error(0, 0, 0, 0, 0, 0, msg, file, function, line, NULL);
	}
	return val;
}
static inline char* check_not_null_charp(char* val, const char* msg, const char* file, const char* function, const int line) {
	if(myunlikely(val==NULL)) {
		handle_error(0, 0, 0, 0, 0, 0, msg, file, function, line, NULL);
	}
	return val;
}
static inline int check_oneoftwo(int val, const char* msg, int e1, int e2, const char* file, const char* function, const int line) {
	if(myunlikely(val!=e1 && val!=e2)) {
		handle_error(0, 0, 0, 0, 1, errno, msg, file, function, line, NULL);
	}
	return val;
}
static inline int check_assert(int val, const char* msg, const char* file, const char* function, const int line) {
	if(myunlikely(!val)) {
		handle_error(0, 0, 0, 0, 0, 0, msg, file, function, line, NULL);
	}
	return val;
}
static inline void* check_not_voidp(void* val, const char *msg, void* errval, const char* file, const char* function, const int line) {
	if(myunlikely(val==errval)) {
		handle_error(0, 0, 0, 0, 0, 0, msg, file, function, line, NULL);
	}
	return val;
}
static inline void* check_voidp(void* val, const char *msg, void* errval, const char* file, const char* function, const int line) {
	if(myunlikely(val!=errval)) {
		handle_error(0, 0, 0, 0, 0, 0, msg, file, function, line, NULL);
	}
	return val;
}
static inline sighandler_t check_not_sigt(sighandler_t val, const char *msg, sighandler_t errval, const char* file, const char* function, const int line) {
	if(myunlikely(val==errval)) {
		handle_error(0, 0, 0, 0, 0, 0, msg, file, function, line, NULL);
	}
	return val;
}
static inline int check_int(int val, const char *msg, int expected, const char* file, const char* function, const int line) {
	if(myunlikely(val!=expected)) {
		handle_error(0, 0, 0, 0, 1, errno, msg, file, function, line, NULL);
	}
	return val;
}
static inline int check_int_noerrno(int val, const char *msg, int expected, const char* file, const char* function, const int line) {
	if(myunlikely(val!=expected)) {
		handle_error(1, val, 0, 0, 0, 0, msg, file, function, line, NULL);
	}
	return val;
}
static inline int check_not_int(int val, const char *msg, int expected, const char* file, const char* function, const int line) {
	if(myunlikely(val==expected)) {
		handle_error(0, 0, 0, 0, 1, errno, msg, file, function, line, NULL);
	}
	return val;
}
static inline char* check_charp(char* val, const char *msg, char* expected, const char* file, const char* function, const int line) {
	if(myunlikely(val!=expected)) {
		handle_error(0, 0, 0, 0, 0, 0, msg, file, function, line, NULL);
	}
	return val;
}
static inline int check_in_range(int val, const char *msg, int min, int max, const char* file, const char* function, const int line) {
	if(myunlikely(val<min || val>=max)) {
		handle_error(0, 0, 0, 0, 1, errno, msg, file, function, line, NULL);
	}
	return val;
}
static inline int check_positive(int val, const char* msg, const char* file, const char* function, const int line) {
	if(myunlikely(val<=0)) {
		handle_error(0, 0, 0, 0, 1, errno, msg, file, function, line, NULL);
	}
	return val;
}
static inline int check_gezero(int val, const char* msg, const char* file, const char* function, const int line) {
	if(myunlikely(val<0)) {
		handle_error(0, 0, 0, 0, 1, errno, msg, file, function, line, NULL);
	}
	return val;
}

#define CHECK_ZERO(v) check_zero(v, stringify(v), __FILE__, __FUNCTION__, __LINE__, NULL)
#define CHECK_MSG_ZERO(v, m) check_zero(v, stringify(v), __FILE__, __FUNCTION__, __LINE__, m)
#define CHECK_ZERO_ERRNO(v) check_zero_errno(v, stringify(v), __FILE__, __FUNCTION__, __LINE__)
#define CHECK_NOT_ZERO(v) check_not_zero(v, stringify(v), __FILE__, __FUNCTION__, __LINE__)
#define CHECK_NOT_M1(v) check_not_m1(v, stringify(v), __FILE__, __FUNCTION__, __LINE__)
#define CHECK_NOT_EOF(v) check_not_eof(v, stringify(v), __FILE__, __FUNCTION__, __LINE__)
#define CHECK_1(v) check_1(v, stringify(v), __FILE__, __FUNCTION__, __LINE__)
#define CHECK_NOT_NEGATIVE(v) check_not_negative(v, stringify(v), __FILE__, __FUNCTION__, __LINE__)
#define CHECK_NOT_NULL(v) check_not_null(v, stringify(v), __FILE__, __FUNCTION__, __LINE__)
#define CHECK_NOT_NULL_CONST(v) check_not_null_const(v, stringify(v), __FILE__, __FUNCTION__, __LINE__)
#define CHECK_NOT_NULL_FILEP(v) check_not_null_filep(v, stringify(v), __FILE__, __FUNCTION__, __LINE__)
#define CHECK_NOT_NULL_CHARP(v) check_not_null_charp(v, stringify(v), __FILE__, __FUNCTION__, __LINE__)
#define CHECK_ONEOFTWO(v, e1, e2) check_oneoftwo(v, stringify(v), e1, e2, __FILE__, __FUNCTION__, __LINE__)
#define CHECK_ASSERT(v) check_assert(v, stringify(v), __FILE__, __FUNCTION__, __LINE__)
#define CHECK_NOT_VOIDP(v, e) check_not_voidp(v, stringify(v), e, __FILE__, __FUNCTION__, __LINE__)
#define CHECK_VOIDP(v, e) check_not_voidp(v, stringify(v), e, __FILE__, __FUNCTION__, __LINE__)
#define CHECK_NOT_SIGT(v, e) check_not_sigt(v, stringify(v), e, __FILE__, __FUNCTION__, __LINE__)
#define CHECK_INT(v, e) check_int(v, stringify(v), e, __FILE__, __FUNCTION__, __LINE__)
#define CHECK_INT_NOERRNO(v, e) check_int_noerrno(v, stringify(v), e, __FILE__, __FUNCTION__, __LINE__)
#define CHECK_NOT_INT(v, e) check_not_int(v, stringify(v), e, __FILE__, __FUNCTION__, __LINE__)
#define CHECK_CHARP(v, e) check_charp(v, stringify(v), e, __FILE__, __FUNCTION__, __LINE__)
#define CHECK_IN_RANGE(v, min, max) check_in_range(v, stringify(v), min, max, __FILE__, __FUNCTION__, __LINE__)
#define CHECK_POSITIVE(v) check_positive(v, stringify(v), __FILE__, __FUNCTION__, __LINE__)
#define CHECK_GEZERO(v) check_gezero(v, stringify(v), __FILE__, __FUNCTION__, __LINE__)
#define CHECK_ERROR(m) check_error(0, m, __FILE__, __FUNCTION__, __LINE__)

#endif	/* !__err_utils_h */
