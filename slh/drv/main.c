//  main.c
//  Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.

//  === testing main()

#include <string.h>
#include "sloth_hal.h"

const char main_hello[] =
"\n[RESET]"
"\t   ______        __  __ __\n"
"\t  / __/ /  ___  / /_/ // /  SLotH Accelerator Test 2024/05\n"
"\t _\\ \\/ /__/ _ \\/ __/ _  /   SLH-DSA / FIPS 205 ipd\n"
"\t/___/____/\\___/\\__/_//_/    markku-juhani.saarinen@tuni.fi\n\n";

//  unit tests
int test_sloth();       //  test_sloth.c
int test_bench();       //  test_bench.c
int test_leak();        //  test_leak.c

int main()
{
    int fail = 0;

    sio_puts(main_hello);

    sio_puts("[INFO]\t=== Basic health test ===\n");
    fail += test_sloth();
    sio_puts("\n[INFO]\t=== Testbench === \n");
    fail += test_bench();
    //fail += test_leak();

    if (fail) {
        sio_puts("[FAIL]\tSome tests failed.\n");
    } else {
        sio_puts("[PASS]\tAll tests ok.\n");
    }

    //  get input (test UART)
#ifdef SLOTH
    sio_puts("\nUART Test. Press x to exit.\n");
    int ch, gpio, old_gpio;

    ch = 0;
    old_gpio = -1;

    do {
        gpio = get_gpio_in();
        if (gpio != old_gpio) {
            sio_puts("GPIO 0x");
            sio_put_hex(gpio, 2);
            sio_putc('\n');
            old_gpio = gpio;
        }

        if (get_uart_rxok()) {
            ch = get_uart_rx();
            sio_puts("UART 0x");
            sio_put_hex(ch, 2);
            sio_putc(' ');
            sio_putc(ch);
            sio_putc('\n');
        }

    } while (ch != 'x');
#endif
    sio_putc('\n');
    sio_putc(4);  //  translated to EOF
    sio_putc(0);

    return 0;
}

