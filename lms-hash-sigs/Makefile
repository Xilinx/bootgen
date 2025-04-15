CFLAGS ?= -Wall -O0 -g

hss_lib_thread.a: lms_sign.o lms_keys.o hss.o hss_alloc.o hss_aux.o hss_common.o \
     hss_compute.o hss_generate.o hss_keygen.o hss_param.o hss_reserve.o \
     hss_sign.o hss_sign_inc.o hss_thread_pthread.o \
     hss_verify.o hss_verify_inc.o \
     hss_derive.o hss_zeroize.o lm_common.o \
     lm_ots_common.o lm_ots_sign.o lm_ots_verify.o lm_verify.o endian.o \
     lms_hash.o sha256.o fips202.o
	$(AR) rcs $@ $^


hss.o: hss.c hss.h common_defs.h lms_hash.h endian.h hss_internal.h hss_aux.h hss_derive.h
	$(CC) $(CFLAGS) -c hss.c -o $@

hss_alloc.o: hss_alloc.c hss.h hss_internal.h lm_common.h config.h
	$(CC) $(CFLAGS) -c hss_alloc.c -o $@

hss_aux.o: hss_aux.c hss_aux.h hss_internal.h common_defs.h lm_common.h endian.h lms_hash.h config.h
	$(CC) $(CFLAGS) -c hss_aux.c -o $@

hss_common.o: hss_common.c common_defs.h hss_common.h lm_common.h config.h
	$(CC) $(CFLAGS) -c hss_common.c -o $@

hss_compute.o: hss_compute.c hss_internal.h lms_hash.h hss_thread.h lm_ots_common.h lm_ots.h endian.h hss_derive.h config.h
	$(CC) $(CFLAGS) -c hss_compute.c -o $@

hss_derive.o: hss_derive.c hss_derive.h hss_internal.h lms_hash.h endian.h config.h
	$(CC) $(CFLAGS) -c hss_derive.c -o $@

hss_generate.o: hss_generate.c hss.h hss_internal.h hss_aux.h lms_hash.h hss_thread.h hss_reserve.h lm_ots_common.h endian.h config.h
	$(CC) $(CFLAGS) -c hss_generate.c -o $@

hss_keygen.o: hss_keygen.c hss.h common_defs.h hss_internal.h hss_aux.h endian.h lms_hash.h hss_thread.h lm_common.h lm_ots_common.h config.h
	$(CC) $(CFLAGS) -c hss_keygen.c -o $@

hss_param.o: hss_param.c hss.h hss_internal.h endian.h hss_zeroize.h config.h
	$(CC) $(CFLAGS) -c hss_param.c -o $@

hss_reserve.o: hss_reserve.c common_defs.h hss_internal.h hss_reserve.h endian.h config.h
	$(CC) $(CFLAGS) -c hss_reserve.c -o $@
   
hss_sign.o: hss_sign.c common_defs.h hss.h lms_hash.h endian.h hss_internal.h hss_aux.h hss_thread.h hss_reserve.h lm_ots.h lm_ots_common.h hss_derive.h config.h
	$(CC) $(CFLAGS) -c hss_sign.c -o $@
   
hss_sign_inc.o: hss_sign_inc.c hss.h common_defs.h hss.h lms_hash.h endian.h hss_internal.h hss_aux.h hss_reserve.h hss_derive.h lm_ots.h lm_ots_common.h hss_sign_inc.h config.h
	$(CC) $(CFLAGS) -c hss_sign_inc.c -o $@

hss_thread_single.o: hss_thread_single.c hss_thread.h config.h
	$(CC) $(CFLAGS) -c hss_thread_single.c -o $@

hss_thread_pthread.o: hss_thread_pthread.c hss_thread.h config.h
	$(CC) $(CFLAGS) -c hss_thread_pthread.c -o $@

hss_verify.o: hss_verify.c hss_verify.h common_defs.h lm_verify.h lm_common.h lm_ots_verify.h lms_hash.h endian.h hss_thread.h config.h
	$(CC) $(CFLAGS) -c hss_verify.c -o $@

hss_verify_inc.o: hss_verify_inc.c hss_verify_inc.h common_defs.h lm_verify.h lm_common.h lm_ots_verify.h lms_hash.h endian.h hss_thread.h config.h
	$(CC) $(CFLAGS) -c hss_verify_inc.c -o $@

hss_zeroize.o: hss_zeroize.c hss_zeroize.h config.h
	$(CC) $(CFLAGS) -c hss_zeroize.c -o $@

lm_common.o: lm_common.c lm_common.h lms_hash.h common_defs.h lm_ots_common.h config.h
	$(CC) $(CFLAGS) -c lm_common.c -o $@

lm_ots_common.o: lm_ots_common.c common_defs.h lms_hash.h config.h
	$(CC) $(CFLAGS) -c lm_ots_common.c -o $@

lm_ots_sign.o: lm_ots_sign.c common_defs.h lm_ots.h lm_ots_common.h lms_hash.h endian.h hss_zeroize.h hss_derive.h config.h
	$(CC) $(CFLAGS) -c lm_ots_sign.c -o $@

lm_ots_verify.o: lm_ots_verify.c lm_ots_verify.h lm_ots_common.h lms_hash.h endian.h common_defs.h config.h
	$(CC) $(CFLAGS) -c lm_ots_verify.c -o $@

lm_verify.o: lm_verify.c lm_verify.h lm_common.h lm_ots_common.h lm_ots_verify.h lms_hash.h endian.h common_defs.h config.h
	$(CC) $(CFLAGS) -c lm_verify.c -o $@

endian.o: endian.c endian.h config.h
	$(CC) $(CFLAGS) -c endian.c -o $@

lms_hash.o: lms_hash.c lms_hash.h sha256.h fips202.h hss_zeroize.h config.h
	$(CC) $(CFLAGS) -c lms_hash.c -o $@

sha256.o: sha256.c sha256.h endian.h config.h
	$(CC) $(CFLAGS) -c sha256.c -o $@

fips202.o: fips202.c fips202.h
	$(CC) $(CFLAGS) -c fips202.c -o $@

clean:
	-rm -f *.o *.a


