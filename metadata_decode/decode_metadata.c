#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(push, 1)
typedef struct {
    uint32_t sync_code;
    uint32_t frame_id;
    uint64_t SOF_timestamp_us;
    uint32_t exp_mid_offset_us;
    uint32_t exp_time_us;
    uint32_t epx_ISO;
    uint16_t color_temp;
    uint16_t reserved_1;
    uint64_t trg_timestamp_us;
    uint32_t reserved[4];
} FrameMetaData;
#pragma pack(pop)

size_t pack7bit_decode(const uint8_t *in, size_t in_len, uint8_t *out)
{
    uint32_t buffer = 0;
    int bits = 0;
    size_t o = 0;
    for (size_t i = 0; i < in_len; i++)
    {
        uint8_t v = in[i] - 1; 
        buffer |= ((uint32_t)v) << bits;
        bits += 7;
        while (bits >= 8) 
        {
            out[o++] = buffer & 0xFF;
            buffer >>= 8;
            bits -= 8;
        }
    }
    return o;
}

void print_hex_dump(const char *title, const uint8_t *data, size_t len)
{
    printf("\n[DEBUG] %s (%zu bytes):\n", title, len);
    for (size_t i = 0; i < len; i++)
    {
        printf("%02X ", data[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    if (len % 16 != 0) printf("\n");
}

int main(int argc, char *argv[])
{
    if (argc != 5 || strcmp(argv[1], "-i") != 0) 
    {
        printf("Usage: %s -i <yuv_file> <width> <height>\n", argv[0]);
        return -1;
    }

    int FRAME_WIDTH = atoi(argv[3]);
    int FRAME_HEIGHT = atoi(argv[4]);
    long FRAME_SIZE = (long)FRAME_WIDTH * FRAME_HEIGHT * 2; 

    FILE *fp = fopen(argv[2], "rb");
    if (!fp) 
    {
        printf("[ERROR] Cannot open file: %s\n", argv[2]); 
        return -1; 
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    uint8_t *file_buffer = (uint8_t *)malloc(file_size);
    if (!file_buffer) 
    {
        printf("[ERROR] Memory alloc failed\n"); 
        fclose(fp); 
        return -1; 
    }

    if (fread(file_buffer, 1, file_size, fp) != (size_t)file_size) 
    {
        printf("[ERROR] Failed to read entire file\n"); 
        free(file_buffer); 
        fclose(fp); 
        return -1;
    }
    fclose(fp);

    int total_frames = file_size / FRAME_SIZE;
    printf("[INFO] Total frames detected: %d\n", total_frames);

    for (int f = 0; f < total_frames; f++)
    {
        uint8_t *pFrameBase = file_buffer + (f * FRAME_SIZE);
        uint8_t *pMetaRegion = pFrameBase + FRAME_SIZE - 128;
        
        printf("\n==============================\n");
        printf("  Parsing Frame ID: %d\n", f);
        printf("==============================\n");

        //print_hex_dump("RAW Last 128 Bytes", pMetaRegion, 128);

        uint8_t extracted_y[64];
       
        uint8_t decoded_meta[64] = {0};
        int found_valid_meta = 0;

        for (int i = 0; i < 64; i++) 
        {
            extracted_y[i]  = pMetaRegion[i * 2 + 1];
        }


        //print_hex_dump("Extracted Y Data (Odd Bytes)", extracted_y, 64);

        size_t decoded_len = pack7bit_decode(extracted_y, 64, decoded_meta);
        if (decoded_len >= sizeof(FrameMetaData)) 
        {
            FrameMetaData *meta = (FrameMetaData *)decoded_meta;
            if (meta->sync_code == 0x55AA5A01)
            {
                found_valid_meta = 1;
            }
        }

        if (found_valid_meta) 
        {
            FrameMetaData *meta = (FrameMetaData *)decoded_meta;
            printf("\n=== PARSED FRAME METADATA ===\n");
            printf("sync_code       : 0x%08X\n", meta->sync_code);
            printf("frame_id        : %u\n", meta->frame_id);
            printf("SOF_timestamp_us: %llu us\n", (unsigned long long)meta->SOF_timestamp_us);
            printf("exp_mid_offset_us: %u us\n", meta->exp_mid_offset_us);
            printf("exp_time_us     : %u us\n", meta->exp_time_us);
            printf("epx_ISO         : %u\n", meta->epx_ISO);
            printf("color_temp      : %u K\n", meta->color_temp);
            printf("trg_timestamp_us: %llu us\n", (unsigned long long)meta->trg_timestamp_us);
        } 
        else 
        {
            printf("[ERROR] Failed to find valid Metadata in this frame.\n");
        }
    }

    free(file_buffer);
    return 0;
}