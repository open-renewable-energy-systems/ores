#include <stdio.h>
#include <stdlib.h>
#include "log_data_rw.h"

void print_data_summary(const struct LogData* data) {
    if (!data) return;
    
    printf("\nData Summary:\n");
    printf("Number of samples: %d\n", data->length);
    printf("\nFirst sample:\n");
    printf("Time: %.3f, Time_us: %.3f\n", data->time_stamp[0], data->time_us[0]);
    printf("Current: %.3f ∠ %.3f\n", data->curr_val[0], data->curr_angle[0]);
    printf("DQ values: d=%.3f, q=%.3f\n", data->d[0], data->q[0]);
    printf("Filtered DQ: d=%.3f, q=%.3f\n", data->filtered_d[0], data->filtered_q[0]);
    
    printf("\nLast sample:\n");
    int last = data->length - 1;
    printf("Time: %.3f, Time_us: %.3f\n", data->time_stamp[last], data->time_us[last]);
    printf("Current: %.3f ∠ %.3f\n", data->curr_val[last], data->curr_angle[last]);
    printf("DQ values: d=%.3f, q=%.3f\n", data->d[last], data->q[last]);
    printf("Filtered DQ: d=%.3f, q=%.3f\n", data->filtered_d[last], data->filtered_q[last]);
}

void save_for_plotting(const struct LogData* data, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error opening output file\n");
        return;
    }
    
    // Write header
    fprintf(file, "time_us,d,q,filtered_d,filtered_q,curr_val,curr_angle\n");
    
    // Write data using time_us for better resolution
    for (int i = 0; i < data->length; i++) {
        fprintf(file, "%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f\n",
                data->time_us[i],  // Use time_us directly
                data->d[i],
                data->q[i],
                data->filtered_d[i],
                data->filtered_q[i],
                data->curr_val[i],
                data->curr_angle[i]);
    }
    
    fclose(file);
    printf("Data saved to %s\n", filename);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_log_file> <output_csv_file>\n", argv[0]);
        return 1;
    }

    // Load data
    struct LogData* data = load_log_data(argv[1]);
    if (!data) {
        printf("Failed to load data\n");
        return 1;
    }

    // Print summary
    print_data_summary(data);
    
    // Save data for plotting
    save_for_plotting(data, argv[2]);
    
    // Cleanup
    cleanup_data(data);
    
    return 0;
}