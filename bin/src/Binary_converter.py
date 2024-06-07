import csv
import sys
import os

def binary_to_csv(binary_file, csv_file, sample_size, num_channels):
    with open(binary_file, 'rb') as f:
        binary_data = f.read()

    num_samples = len(binary_data) * 8 // (sample_size * num_channels)

    with open(csv_file, 'w', newline='') as f:
        writer = csv.writer(f)

        # Write the header row
        writer.writerow(['Sample'] + [f'Channel {i + 1}' for i in range(num_channels)])

        # Iterate over the binary data and write each sample to the CSV file
        for i in range(num_samples):
            sample_start_index = i * sample_size * num_channels // 8
            sample_end_index = (i + 1) * sample_size * num_channels // 8

            sample_bytes = binary_data[sample_start_index:sample_end_index]
            sample_values = []

            # Extract the channel values from the sample bytes
            for j in range(num_channels):
                channel_start_bit = j * sample_size
                channel_end_bit = (j + 1) * sample_size

                value = int.from_bytes(sample_bytes, byteorder='little')
                value >>= channel_start_bit
                value &= (1 << sample_size) - 1

                sample_values.append(value)

            writer.writerow([i] + sample_values)

    print("Binary file converted to CSV successfully.")

# Usage example
#binary_file = 'Shk00.bin'
binary_file = sys.argv[1]
#csv_file = 'Shk00.csv'
csv_file = os.path.splitext(binary_file)[0] + '.csv'
sample_size = 16  # 10 bits per sample
num_channels = 1  # Single channel
binary_to_csv(binary_file, csv_file, sample_size, num_channels)

