int save_array_to_fits(const char *array_name, double *array, long n_iter, long bootstrap_n_iter) {
    // 1) get executable directory
    char exe_path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len == -1) {
        perror("readlink");
        return -1;
    }
    exe_path[len] = '\0';
    char *exe_dir = dirname(exe_path);

    // 2) define results dir relative to exe location
    char results_dir[MAX_PATH];
    snprintf(results_dir, sizeof(results_dir), "%s/../results", exe_dir);

    // 3) timestamped folder
    char folder_name[64];
    time_t t = time(NULL);
    struct tm tm_info;
    localtime_r(&t, &tm_info);
    strftime(folder_name, sizeof(folder_name), "%Y-%m-%d_%H-%M-%S", &tm_info);
    strcat(folder_name, "_c");

    // 4) build full folder path
    char full_path[MAX_PATH];
    snprintf(full_path, MAX_PATH, "%s/%s", results_dir, folder_name);

    if (mkdir_if_not_exists(full_path) != 0) return -1;

    // 5) slice array
    long n = n_iter - bootstrap_n_iter;
    double *array_ptr = &array[bootstrap_n_iter];

    // 6) build FITS filename
    char fits_file[MAX_PATH];
    snprintf(fits_file, MAX_PATH, "%s/%s.fits", full_path, array_name);

    if (write_fits_double(fits_file, array_ptr, n) != 0) {
        fprintf(stderr, "Error writing %s\n", fits_file);
        return -1;
    }

    printf("Saved %s to %s\n", array_name, full_path);
    return 0;
}


// Save a single array to a timestamped folder
int save_array_to_fits(const char *array_name, double *array, long n_iter, long bootstrap_n_iter) {
    // 1) timestamped folder
    char folder_name[64];
    time_t t = time(NULL);
    struct tm tm_info;
    localtime_r(&t, &tm_info);
    strftime(folder_name, sizeof(folder_name), "%Y-%m-%d_%H-%M-%S", &tm_info);
    strcat(folder_name, "_c");

    char full_path[MAX_PATH];
    snprintf(full_path, MAX_PATH, "%s%s", PARENT_DIR, folder_name);

    if (mkdir_if_not_exists(full_path) != 0) return -1;

    // 2) slice array
    long n = n_iter - bootstrap_n_iter;
    double *array_ptr = &array[bootstrap_n_iter];

     // 3) FITS filename (safe join)
    char fits_file[MAX_PATH];

    // start with full_path
    strncpy(fits_file, full_path, MAX_PATH - 1);
    fits_file[MAX_PATH - 1] = '\0';

    // add "/"
    strncat(fits_file, "/", MAX_PATH - strlen(fits_file) - 1);

    // add array_name
    strncat(fits_file, array_name, MAX_PATH - strlen(fits_file) - 1);

    // add ".fits"
    strncat(fits_file, ".fits", MAX_PATH - strlen(fits_file) - 1);

    if (write_fits_double(fits_file, array_ptr, n) != 0) {
        fprintf(stderr, "Error writing %s\n", fits_file);
        return -1;
    }

    printf("Saved %s to %s\n", array_name, full_path);
    return 0;
}
