void capture_image(uint8_t* image_data) {
    printf("r\n");
    printf("x\n");

    int index = 0;
    int start_found = 0; // false
    char received; 
    char buffer[60];

    while (1) {
        if (start_found) {
            if (my_strcmp(buffer, "END\r") == 0) {
                break;
            }
            // Pixel data transfer
            for (int i = 0; i < WIDTH * HEIGHT; i++) {
                uart_receive(UART0, &image_data[i], sizeof(char), 100000);
            }
        } else {
            uart_receive(UART0, &received, sizeof(char), 100000);
            if (received == '\n' || index >= sizeof(buffer) - 1) {
                buffer[index] = '\0';
                index = 0;
                if (my_strcmp(buffer, "START\r") == 0) {
                    start_found = 1; // true
                }
            } else {
                buffer[index++] = received;
            }
        }

        uart_receive(UART0, &received, sizeof(char), 100000);
        if (received == '\n' || index >= sizeof(buffer) - 1) {
            buffer[index] = '\0';
            index = 0;

            if (start_found) {
                if (my_strcmp(buffer, "END\r") == 0) {
                    break;
                } else {
                    // Pixel data transfer
                    for (int i = 0; i < WIDTH * HEIGHT; i++) {
                        uart_receive(UART0, &image_data[i], sizeof(char), 100000);
                    }
                }
            } else {
                if (my_strcmp(buffer, "START\r") == 0) {
                    start_found = 1; // true
                }
            }
        } else {
            buffer[index++] = received;
        }
    }
}