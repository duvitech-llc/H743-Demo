
#include "crosslink.h"
#include "main.h"
#include "bitstream.h"
#include <string.h>
#include <stdio.h>

#define I2C_SLAVE_ADDR 0x40
#define BITSTREAM_CHUNK_SIZE 8192
#define FPGA_RESET_GPIO_Port GPIOC
#define FPGA_RESET_Pin GPIO_PIN_9

extern uint8_t bitstream_buffer[];  // Place in linker script or define in code


volatile uint8_t txComplete = 0;
volatile uint8_t rxComplete = 0;
volatile uint8_t i2cError = 0;

uint8_t write_buf[4];
uint8_t read_buf[4];

void delay_ms(uint32_t ms) {
    HAL_Delay(ms);
}

void print_hex_buf(const char *label, uint8_t *buf, size_t len) {
    if (label) printf("%s: ", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02X ", buf[i]);
    }
    printf("\r\n");
}

int i2c_write_bytes(uint8_t *data, uint16_t length) {
    return HAL_I2C_Master_Transmit(&hi2c1, I2C_SLAVE_ADDR << 1, data, length, HAL_MAX_DELAY);
}

int i2c_read_bytes(uint8_t *data, uint16_t length) {
    return HAL_I2C_Master_Receive(&hi2c1, I2C_SLAVE_ADDR << 1, data, length, HAL_MAX_DELAY);
}

int i2c_write_and_read(uint8_t *wbuf, uint16_t wlen, uint8_t *rbuf, uint16_t rlen) {
    txComplete = 0;
    rxComplete = 0;
    i2cError = 0;

    if (HAL_I2C_Master_Seq_Transmit_IT(&hi2c1, I2C_SLAVE_ADDR << 1, wbuf, wlen, I2C_FIRST_FRAME) != HAL_OK)
        return -1;

    // Wait for the transmission to complete
    while (!txComplete && !i2cError) {}

    if (i2cError)
    {
        return HAL_ERROR;
    }


    if (HAL_I2C_Master_Seq_Receive_IT(&hi2c1, I2C_SLAVE_ADDR << 1, rbuf, rlen, I2C_LAST_FRAME) != HAL_OK)
        return -1;

    // Wait for the reception to complete
    while (!rxComplete && !i2cError) {}

    if (i2cError)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}

int i2c_write_long(uint8_t *cmd, int cmd_len, uint8_t *data, size_t data_len) {
	HAL_StatusTypeDef ret;
    size_t offset = 0;
    uint32_t frame_flag;
    size_t total_len = data_len+cmd_len;
    int num_chunks = (total_len + BITSTREAM_CHUNK_SIZE - 1) / BITSTREAM_CHUNK_SIZE;  // Calculate number of chunks
    uint8_t *pData;
	uint16_t datalen;

	memset(bitstream_buffer, 0, MAX_BITSTREAM_SIZE);
    memcpy(bitstream_buffer, cmd, cmd_len);
    memcpy(bitstream_buffer + cmd_len, data, data_len);


    for (int i = 0; i < num_chunks; i++) {
        size_t current_chunk_size = (total_len - offset > BITSTREAM_CHUNK_SIZE)
                                     ? BITSTREAM_CHUNK_SIZE
                                     : (total_len - offset);

        // Determine frame flags
        if (i == 0 && num_chunks == 1) {
            frame_flag = I2C_FIRST_AND_LAST_FRAME;
        } else if (i == 0) {
            frame_flag = I2C_FIRST_AND_NEXT_FRAME;
        } else if (i == num_chunks - 1) {
            frame_flag = I2C_LAST_FRAME;
        } else {
            frame_flag = I2C_NEXT_FRAME;
        }

        // Check if the I2C peripheral is ready
        while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {
            //if(verbose_on) printf("I2C busy, waiting...\r\n");
            HAL_Delay(1);  // Add a small delay to avoid busy looping
        }

        pData = (uint8_t*)&bitstream_buffer[offset];
        datalen = (uint16_t)current_chunk_size;

        // Reset completion flags
        txComplete = 0;
        i2cError = 0;

        // Transmit the current chunk
        ret = HAL_I2C_Master_Seq_Transmit_IT(&hi2c1, I2C_SLAVE_ADDR << 1, pData, datalen, frame_flag);
        if (ret != HAL_OK) {
        	printf("++++> i2c_write_long HAL TX HAL_StatusTypeDef: 0%04X I2C_ERROR: 0%04lX\r\n", ret, hi2c1.ErrorCode);
            return ret; // Return if any transmission fails
        }

        // Wait for the transmission to complete
        while (!txComplete && !i2cError) {}

        if (i2cError)
        {
            return HAL_ERROR;
        }

        offset += current_chunk_size;
    }

    printf("Programmed Successfully\r\n");
    HAL_Delay(100);
    return HAL_OK;
}

void fpga_configure() {
    printf("Starting FPGA configuration...\r\n");

    // Set GPIO LOW
    HAL_GPIO_WritePin(FPGA_RESET_GPIO_Port, FPGA_RESET_Pin, GPIO_PIN_RESET);
    delay_ms(1000);

    // Activation Key
    uint8_t activation_key[] = {0xFF, 0xA4, 0xC6, 0xF4, 0x8A};
    i2c_write_bytes(activation_key, 5);
    HAL_GPIO_WritePin(FPGA_RESET_GPIO_Port, FPGA_RESET_Pin, GPIO_PIN_SET);
    delay_ms(10);

    // IDCODE
    memset(read_buf, 0, 4);
    memcpy(write_buf, (uint8_t[]){0xE0,0x00,0x00,0x00}, 4);
    i2c_write_and_read(write_buf, 4, read_buf, 4);
    print_hex_buf("IDCODE", read_buf, 4);

    // Enable SRAM
    memcpy(write_buf, (uint8_t[]){0xC6,0x00,0x00,0x00}, 4);
    i2c_write_bytes(write_buf, 4);
    delay_ms(1);

    // Erase SRAM
    memcpy(write_buf, (uint8_t[]){0x0E,0x00,0x00,0x00}, 4);
    i2c_write_bytes(write_buf, 4);
    delay_ms(5000);

    // Read Status
    memset(read_buf, 0, 4);
    memcpy(write_buf, (uint8_t[]){0x3C,0x00,0x00,0x00}, 4);
    i2c_write_and_read(write_buf, 4, read_buf, 4);
    print_hex_buf("Erase Status", read_buf, 4);

    // Program Command
    memcpy(write_buf, (uint8_t[]){0x46,0x00,0x00,0x00}, 4);
    i2c_write_bytes(write_buf, 4);
    delay_ms(1);

    // Send Bitstream
    memcpy(write_buf, (uint8_t[]){0x7A,0x00,0x00,0x00}, 4);
    i2c_write_long(write_buf, 4, (uint8_t*)bitstream_file_buffer, (size_t)bitstream_file_buffer_SIZE);
    delay_ms(100);

    // USERCODE (optional)
    memset(read_buf, 0, 4);
    memcpy(write_buf, (uint8_t[]){0xC0,0x00,0x00,0x00}, 4);
    i2c_write_and_read(write_buf, 4, read_buf, 4);
    print_hex_buf("User Register", read_buf, 4);

    // Final Status
    memset(read_buf, 0, 4);
    memcpy(write_buf, (uint8_t[]){0x3C,0x00,0x00,0x00}, 4);
    i2c_write_and_read(write_buf, 4, read_buf, 4);
    print_hex_buf("Program Status", read_buf, 4);

    // Exit Program Mode
    memcpy(write_buf, (uint8_t[]){0x26,0x00,0x00,0x00}, 4);
    i2c_write_bytes(write_buf, 4);

    printf("FPGA configuration complete.\r\n");
}

// Callback implementations
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    txComplete = 1;
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    rxComplete = 1;
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    i2cError = 1;
}

