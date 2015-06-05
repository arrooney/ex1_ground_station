#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <printf.h>
#include <inttypes.h>

#include <log/log.h>

typedef struct {
   unsigned int gp_offset;
   unsigned int fp_offset;
   void *overflow_arg_area;
   void *reg_save_area;
} va_list_real;

int log_pack(char * buf, int max_size, const char * format, va_list ap) {
	int argtypes[20] = {};
	int n = parse_printf_format(format, 20, argtypes);

#if 0
	va_list_real * va_real = ap;
	printf("VAlist\r\n");
	hex_dump((void *) va_real, 24);
	printf("reg save area\r\n");
	hex_dump((void *) va_real->reg_save_area, 200);
	printf("overflow save area\r\n");
	hex_dump((void *) va_real->overflow_arg_area, 100);
#endif

	int size = 0;
	for (int i = 0; i < n; i++) {
		switch(argtypes[i]) {
		case PA_INT: {
			long tmp = va_arg(ap, long);
			memcpy(buf + size, &tmp, sizeof(long));
			size += sizeof(long);
			break;
		}
		case PA_CHAR: {
			char tmp = (char) va_arg(ap, long);
			memcpy(buf + size, &tmp, sizeof(long));
			size += sizeof(long);
			break;
		}
		case PA_POINTER: {
			void * tmp = va_arg(ap, void *);
			memcpy(buf + size, &tmp, sizeof(void *));
			size += sizeof(void *);
			break;
		}
		case PA_FLOAT:
		case PA_DOUBLE: {
			double tmp = va_arg(ap, double);
			memcpy(buf + size, &tmp, sizeof(double));
			size += sizeof(double);
			break;
		}
		case PA_STRING: {
			char * tmp = va_arg(ap, char *);
			strcpy(buf + size, tmp);
			size += strlen(tmp);
			break;
		}
		case PA_WCHAR:
		case PA_WSTRING:
		case PA_LAST:
			printf("Unsupported type %d\r\n", argtypes[i]);
			break;
		}
	}

	return size;
}

va_list * log_unpack(char * buf, int max_size, const char * format, const char * data, int data_len) {
	int argtypes[20] = {};
	int n = parse_printf_format(format, 20, argtypes);

	va_list_real * va = (va_list_real *) buf;
	va->gp_offset = 0;
	va->fp_offset = 0x40;
	va->overflow_arg_area = &buf;
	va->reg_save_area = buf + sizeof(va_list_real);

	buf += sizeof(va_list_real);

	int out = 0, out_fp = 0x40, in = 0;
	for (int i = 0; i < n; i++) {
		switch(argtypes[i]) {
		case PA_INT: {
			//printf("int %u\r\n", out);
			memcpy(buf + out, data + in, sizeof(long));
			out += sizeof(long) ;
			in += sizeof(long);
			break;
		}
		case PA_CHAR: {
			//printf("char %u\r\n", out);
			memcpy(buf + out, data + in, sizeof(long));
			out += sizeof(long);
			in += sizeof(long);
			break;
		}
		case PA_POINTER: {
			//printf("ptr\r\n");
			memcpy(buf + out, data + in, sizeof(void *));
			out += sizeof(void *);
			in += sizeof(void *);
			break;
		}
		case PA_FLOAT:
		case PA_DOUBLE: {
			//printf("dbl\r\n");
			memcpy(buf + out_fp, data + in, sizeof(double));
			out_fp += sizeof(double) * 2;
			in += sizeof(double);
			break;
		}
		case PA_STRING:
		case PA_WCHAR:
		case PA_WSTRING:
		case PA_LAST:
			printf("Unsupported type %d\r\n", argtypes[i]);
			break;
		}
	}

	return va;
}

