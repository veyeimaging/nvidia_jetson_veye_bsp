#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OUTPUT_JSON="$SCRIPT_DIR/auto_camera_index.json"

camera_infos=()

for media in /dev/media*; do
    media-ctl -d "$media" -p | awk '
    BEGIN {
        RS = ""
    }

    {
        # 找到所有 sensor 名称和 i2c bus
        if ($0 ~ /subtype Sensor/) {
            if (match($0, /entity [0-9]+: ([^(\n]+) \(/, m)) {
                sensor_name = m[1]
                if (match(sensor_name, /([0-9]+)-0*[0-9a-fA-F]+/, i2c)) {
                    i2c_bus = i2c[1]
                    sensor_map_name = sensor_name
                    sensor_map_bus = i2c_bus
                }
            }
        }

        # 找出所有 /dev/videoX 节点并匹配
        if ($0 ~ /type Node subtype V4L/) {
            if (match($0, /entity [0-9]+: ([^(\n]+) \(/, m1) &&
                match($0, /device node name (\/dev\/video[0-9]+)/, m2)) {

                entity_name = m1[1]
                video_node = m2[1]

                if (index(entity_name, sensor_map_name) > 0) {
                    printf("{\"i2c_bus\": \"%s\", \"video_node\": \"%s\"}\n", sensor_map_bus, video_node)
                }
            }
        }
    }' >> "$OUTPUT_JSON.tmp"
done

# 组装 JSON 格式
{
  echo "["
  sed '$!s/$/,/' "$OUTPUT_JSON.tmp"
  echo "]"
} > "$OUTPUT_JSON"
rm -f "$OUTPUT_JSON.tmp"

echo "Done --> $OUTPUT_JSON"